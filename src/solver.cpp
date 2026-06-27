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

        // Se ambos não foram usados, checamos se são incontornáveis
        if (!a_used)
        {
            bool a_unavoidable = unavoidable_labels.count(a) > 0;
            bool b_unavoidable = unavoidable_labels.count(b) > 0;
            if (a_unavoidable != b_unavoidable)
            {
                return a_unavoidable; // Incontornáveis não usados primeiro
            }
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

    // Fronteira incremental de arestas candidatas
    std::vector<std::pair<int, int>> frontier_edges;

    // Função lambda para visitar um novo vértice e atualizar a fronteira de forma incremental
    auto visit_vertex = [&](int v) {
        visited_vertices.insert(v);
        // Adiciona arestas incidentes em v para vizinhos não visitados
        for (int w : graph.get_neighbors(v))
        {
            if (visited_vertices.count(w) == 0)
            {
                frontier_edges.push_back({std::min(v, w), std::max(v, w)});
            }
        }
        // Remove arestas da fronteira onde ambos os extremos já foram visitados
        frontier_edges.erase(
            std::remove_if(frontier_edges.begin(), frontier_edges.end(), [&](const std::pair<int, int>& edge) {
                return visited_vertices.count(edge.first) > 0 && visited_vertices.count(edge.second) > 0;
            }),
            frontier_edges.end()
        );
    };

    // Inicializa a fronteira a partir dos vértices já visitados na Fase 2
    std::unordered_set<int> initial_visited = visited_vertices;
    visited_vertices.clear();
    for (int v : initial_visited)
    {
        visit_vertex(v);
    }

    // Loop de Expansão (Greedy Expansion)
    while ((int)visited_vertices.size() < num_vertices)
    {
        if (frontier_edges.empty())
        {
            std::cerr << "[Erro] Grafo desconexo. Nao e possivel gerar arvore geradora.\n";
            break;
        }

        // 1. Coleta os rótulos presentes na fronteira
        std::unordered_set<int> candidate_labels;
        for (const auto& edge : frontier_edges)
        {
            int label = graph.get_edge_label(edge.first, edge.second);
            if (label != -1)
            {
                candidate_labels.insert(label);
            }
        }

        // 2. Busca na fila de prioridade o primeiro rótulo que tem aresta na fronteira
        int best_label = -1;
        for (int label : label_priority_queue)
        {
            if (candidate_labels.count(label) > 0)
            {
                best_label = label;
                break;
            }
        }

        if (best_label == -1)
        {
            std::cerr << "[Erro] Nenhum rotulo correspondente encontrado na fronteira.\n";
            break;
        }

        // 3. Seleciona a melhor aresta candidata com o best_label
        // Desempate consistente: menor ID de u, depois de v (com u < v na representação ordenada da aresta)
        std::pair<int, int> best_edge;
        bool found = false;

        for (const auto& edge : frontier_edges)
        {
            if (graph.get_edge_label(edge.first, edge.second) == best_label)
            {
                if (!found)
                {
                    best_edge = edge;
                    found = true;
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
        }

        // 4. Atualização de Estado
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
