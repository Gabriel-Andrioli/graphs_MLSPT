#include "solver.hpp"
#include <algorithm>

using namespace std;

MLSPTSolver::MLSPTSolver(const Graph &g) {
    this->graph = g;
}

void MLSPTSolver::solve()
{
    preprocess();          // Fase 1
    initialize_solution(); // Fase 2
    expansion_loop();      // Fase 3 e 4
}

void MLSPTSolver::preprocess()
{
    recurrence_map = graph.get_label_frequencies();
}

// Ordena a fila de prioridades de rótulos
void MLSPTSolver::update_priority_queue()
{
    label_priority_queue.clear();
    for (const auto& pair : recurrence_map)
    {
        label_priority_queue.push_back(pair.first);
    }

    sort(label_priority_queue.begin(), label_priority_queue.end(), [this](int a, int b) {
        bool a_used = used_labels.count(a) > 0;
        bool b_used = used_labels.count(b) > 0;

        if (a_used != b_used)
        {
            return a_used; // Usados primeiro
        }

        int freq_a = recurrence_map.at(a);
        int freq_b = recurrence_map.at(b);
        if (freq_a != freq_b)
        {
            return freq_a > freq_b; // Maior ocorrência global primeiro
        }

        return a < b; // Desempate consistente
    });
}

void MLSPTSolver::initialize_solution()
{
    selected_edges.clear();
    visited_vertices.clear();
    used_labels.clear();

    // Escolhe a primeira aresta do rótulo mais recorrente.
    int max_label = -1;
    int max_freq = -1;
    for (const auto& pair : recurrence_map)
    {
        if (pair.second > max_freq)
        {
            max_freq = pair.second;
            max_label = pair.first;
        }
        else if (pair.second == max_freq && (max_label == -1 || pair.first < max_label))
        {
            max_label = pair.first;
        }
    }

    if (max_label != -1)
    {
        int num_vertices = graph.get_vertices_count();
        bool found = false;
        for (int u = 0; u < num_vertices && !found; ++u)
        {
            for (int v : graph.get_neighbors(u))
            {
                if (u < v && graph.get_edge_label(u, v) == max_label)
                {
                    selected_edges.push_back({u, v});
                    visited_vertices.insert(u);
                    visited_vertices.insert(v);
                    used_labels.insert(max_label);
                    found = true;
                    break;
                }
            }
        }
    }

    // Ordena a fila de prioridades inicial
    update_priority_queue();
}



const unordered_map<int, int>& MLSPTSolver::get_recurrence_map() const
{
    return recurrence_map;
}

const vector<pair<int, int>>& MLSPTSolver::get_selected_edges() const
{
    return selected_edges;
}

const unordered_set<int>& MLSPTSolver::get_visited_vertices() const
{
    return visited_vertices;
}

const unordered_set<int>& MLSPTSolver::get_used_labels() const
{
    return used_labels;
}

const vector<int>& MLSPTSolver::get_label_priority_queue() const
{
    return label_priority_queue;
}

struct CandidateEdge {
    int u;
    int v;
    int label;
    int priority_index;
};

void MLSPTSolver::expansion_loop()
{
    int num_vertices = graph.get_vertices_count();
    
    // Alimenta o conjunto de busca rápida com as arestas iniciais
    selected_edges_set.clear();
    for (const auto& edge : selected_edges)
    {
        selected_edges_set.insert({min(edge.first, edge.second), max(edge.first, edge.second)});
    }

    // Fronteira de arestas agrupadas por rótulo (map)
    // Para acesso rápido e busca eficiente
    unordered_map<int, vector<pair<int, int>>> frontier_by_label;

    // Função lambda para visitar um novo vértice e atualizar a fronteira por rótulo
    auto visit_vertex = [&](int v) {
        visited_vertices.insert(v);
        // Adiciona arestas incidentes em v para vizinhos não visitados
        for (int w : graph.get_neighbors(v))
        {
            if (visited_vertices.count(w) == 0)
            {
                int label = graph.get_edge_label(v, w);
                if (label != -1)
                {
                    frontier_by_label[label].push_back({min(v, w), max(v, w)});
                }
            }
        }
    };

    // Inicializa a fronteira a partir dos vértices já visitados na Fase 2
    unordered_set<int> initial_visited = visited_vertices;
    visited_vertices.clear();
    for (int v : initial_visited)
    {
        visit_vertex(v);
    }

    // Loop de Expansão
    while ((int)visited_vertices.size() < num_vertices)
    {
        // 1. Busca na fila de prioridade o primeiro rótulo que tem arestas válidas na fronteira
        int best_label = -1;
        pair<int, int> best_edge;
        bool found_edge = false;

        for (int label : label_priority_queue)
        {
            auto it = frontier_by_label.find(label);
            if (it != frontier_by_label.end() && !it->second.empty())
            {
                // Limpa arestas inválidas desse rótulo (onde ambos os extremos já foram visitados)
                auto &edges = it->second;
                edges.erase(
                    remove_if(edges.begin(), edges.end(), [&](const pair<int, int>& edge) {
                        return visited_vertices.count(edge.first) > 0 && visited_vertices.count(edge.second) > 0;
                    }),
                    edges.end()
                );

                if (!edges.empty())
                {
                    best_label = label;
                    // Encontra a melhor aresta para o best_label (desempate consistente)
                    for (const auto& edge : edges)
                    {
                        if (!found_edge)
                        {
                            best_edge = edge;
                            found_edge = true;
                        }
                        else
                        {
                            if (edge.first < best_edge.first)
                            {
                                best_edge = edge;
                            }
                            else if (edge.first == best_edge.first && edge.second < best_edge.second)
                            {
                                best_edge = edge;
                            }
                        }
                    }
                    break; // Encontramos o rótulo prioritário com aresta válida
                }
            }
        }

        if (!found_edge)
        {
            // Se nenhuma aresta foi encontrada mas ainda não visitamos todos, o grafo é desconexo
            cerr << "[Erro] Grafo desconexo. Nao e possivel gerar arvore geradora.\n";
            break;
        }

        // 2. Atualização de Estado
        if (selected_edges_set.count(best_edge) == 0)
        {
            selected_edges.push_back(best_edge);
            selected_edges_set.insert(best_edge);
        }

        // Encontra qual dos extremos da aresta é o novo vértice a ser visitado
        int new_v = -1;
        if (visited_vertices.count(best_edge.first) == 0)
        {
            new_v = best_edge.first;
        }
        else if (visited_vertices.count(best_edge.second) == 0)
        {
            new_v = best_edge.second;
        }

        if (new_v != -1)
        {
            visit_vertex(new_v);
        }

        // Atualização Dinâmica de Prioridade se o rótulo for inédito
        if (used_labels.count(best_label) == 0)
        {
            used_labels.insert(best_label);
            update_priority_queue(); // Recalcula a ordenação
        }
    }
}
