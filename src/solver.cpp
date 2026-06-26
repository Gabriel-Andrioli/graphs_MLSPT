#include "solver.hpp"
#include <algorithm>

MLSPTSolver::MLSPTSolver(const Graph &g) : graph(g)
{
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
    int num_vertices = graph.get_vertices_count();
    
    for (int u = 0; u < num_vertices; ++u)
    {
        // Verifica se o nó é mono-label
        if (graph.is_vertex_mono_label(u))
        {
            vector<int> neighbors = graph.get_neighbors(u);
            if (!neighbors.empty())
            {
                // Como é mono-label, qualquer uma das arestas incidentes tem o mesmo rótulo.
                // Consultamos o rótulo da primeira aresta incidente.
                int label = graph.get_edge_label(u, neighbors[0]);
                if (label != -1)
                {
                    unavoidable_labels.insert(label);
                }
            }
        }

        // Verifica se o grau do nó é exatamente 1 (vértice folha)
        if (graph.degree(u) == 1)
        {
            vector<int> neighbors = graph.get_neighbors(u);
            if (!neighbors.empty())
            {
                int v = neighbors[0];
                int edge_u = std::min(u, v);
                int edge_v = std::max(u, v);
                pair<int, int> edge = {edge_u, edge_v};
                
                // Evita inserir arestas duplicadas
                if (std::find(unavoidable_edges.begin(), unavoidable_edges.end(), edge) == unavoidable_edges.end())
                {
                    unavoidable_edges.push_back(edge);
                }

                // A única aresta de um nó folha também tem rótulo incontornável
                int label = graph.get_edge_label(u, v);
                if (label != -1)
                {
                    unavoidable_labels.insert(label);
                }
            }
        }
    }
}

// Ordena a fila de prioridades de rótulos
void MLSPTSolver::update_priority_queue()
{
    label_priority_queue.clear();
    for (const auto& pair : recurrence_map)
    {
        label_priority_queue.push_back(pair.first);
    }

    std::sort(label_priority_queue.begin(), label_priority_queue.end(), [this](int a, int b) {
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

// Fase 2: Inicialização
void MLSPTSolver::initialize_solution()
{
    selected_edges.clear();
    visited_vertices.clear();
    used_labels.clear();

    if (!unavoidable_edges.empty())
    {
        // Fluxo A: Há arestas incontornáveis.
        for (const auto& edge : unavoidable_edges)
        {
            selected_edges.push_back(edge);
            
            // Marca apenas o vértice de grau 1 (folha) como visitado
            if (graph.degree(edge.first) == 1)
            {
                visited_vertices.insert(edge.first);
            }
            else
            {
                visited_vertices.insert(edge.second);
            }

            int label = graph.get_edge_label(edge.first, edge.second);
            if (label != -1)
            {
                used_labels.insert(label);
            }
        }
    }
    else
    {
        // Fluxo B: Sem arestas incontornáveis. Escolhe a primeira aresta do rótulo mais recorrente.
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
    }

    // Ordena a fila de prioridades inicial
    update_priority_queue();
}

const std::vector<std::pair<int, int>>& MLSPTSolver::get_unavoidable_edges() const
{
    return unavoidable_edges;
}

const std::unordered_set<int>& MLSPTSolver::get_unavoidable_labels() const
{
    return unavoidable_labels;
}

const std::unordered_map<int, int>& MLSPTSolver::get_recurrence_map() const
{
    return recurrence_map;
}

const std::vector<std::pair<int, int>>& MLSPTSolver::get_selected_edges() const
{
    return selected_edges;
}

const std::unordered_set<int>& MLSPTSolver::get_visited_vertices() const
{
    return visited_vertices;
}

const std::unordered_set<int>& MLSPTSolver::get_used_labels() const
{
    return used_labels;
}

const std::vector<int>& MLSPTSolver::get_label_priority_queue() const
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
        selected_edges_set.insert({std::min(edge.first, edge.second), std::max(edge.first, edge.second)});
    }

    // Loop de Expansão (Greedy Expansion)
    while ((int)visited_vertices.size() < num_vertices)
    {
        std::vector<CandidateEdge> candidates;

        // 1. Mapear Fronteira: busca vizinhos não visitados a partir de nós visitados
        for (int u : visited_vertices)
        {
            for (int v : graph.get_neighbors(u))
            {
                if (visited_vertices.count(v) == 0)
                {
                    int label = graph.get_edge_label(u, v);
                    
                    // Busca a posição do rótulo na fila de prioridades
                    int priority_index = -1;
                    for (int i = 0; i < (int)label_priority_queue.size(); ++i)
                    {
                        if (label_priority_queue[i] == label)
                        {
                            priority_index = i;
                            break;
                        }
                    }

                    candidates.push_back({u, v, label, priority_index});
                }
            }
        }

        // Se a fronteira for vazia e ainda há nós não visitados, o grafo de entrada é desconexo
        if (candidates.empty())
        {
            std::cerr << "[Erro] Grafo desconexo. Nao e possivel gerar arvore geradora.\n";
            break;
        }

        // 2. Seleção: Encontra o candidato com a melhor prioridade (menor índice na fila)
        auto best_it = std::min_element(candidates.begin(), candidates.end(), [](const CandidateEdge &a, const CandidateEdge &b) {
            if (a.priority_index != b.priority_index)
            {
                return a.priority_index < b.priority_index;
            }
            // Critério de desempate consistente
            if (a.u != b.u) return a.u < b.u;
            return a.v < b.v;
        });

        CandidateEdge best = *best_it;

        // 3. Atualização de Estado
        std::pair<int, int> edge_pair = {std::min(best.u, best.v), std::max(best.u, best.v)};
        
        // Insere em S se ainda não estiver presente (ex: no caso de incontornáveis)
        if (selected_edges_set.count(edge_pair) == 0)
        {
            selected_edges.push_back(edge_pair);
            selected_edges_set.insert(edge_pair);
        }

        // Marca o novo vértice vizinho como visitado
        visited_vertices.insert(best.v);

        // 4. Atualização Dinâmica de Prioridade
        if (used_labels.count(best.label) == 0)
        {
            used_labels.insert(best.label);
            update_priority_queue(); // Recalcula a ordenação
        }
    }
}
