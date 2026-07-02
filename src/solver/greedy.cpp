#include "solver.hpp"
#include <iostream>
#include <algorithm>

using namespace std;

void MLSPTSolver::solve()
{
    solve_greedy();
}

void MLSPTSolver::solve_greedy()
{
    reset_state();
    preprocess();
    initialize_solution();
    expansion_loop();
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

    // Inicializa a fronteira a partir dos vértices já visitados na Fase 2
    unordered_set<int> initial_visited = visited_vertices;
    visited_vertices.clear();
    for (int v : initial_visited)
    {
        visit_vertex(v, frontier_by_label);
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
                // Limpa arestas inválidas desse rótulo in place (evita alocações)
                auto &edges = it->second;
                size_t write_idx = 0;
                for (size_t read_idx = 0; read_idx < edges.size(); ++read_idx)
                {
                    if (visited_vertices.count(edges[read_idx].first) == 0 || visited_vertices.count(edges[read_idx].second) == 0)
                    {
                        edges[write_idx++] = edges[read_idx];
                    }
                }
                edges.resize(write_idx);

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
            visit_vertex(new_v, frontier_by_label);
        }

        // Atualização Dinâmica de Prioridade se o rótulo for inédito
        if (used_labels.count(best_label) == 0)
        {
            used_labels.insert(best_label);
            update_priority_queue(); // Recalcula a ordenação
        }
    }
}
