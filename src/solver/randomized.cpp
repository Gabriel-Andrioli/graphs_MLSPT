#include "solver.hpp"
#include "random_manager.hpp"
#include <algorithm>
#include <iostream>
#include <limits>

using namespace std;

void MLSPTSolver::solve_randomized(double alpha, int iterations)
{
    // Armazena a melhor solução encontrada ao longo de todas as iterações
    vector<pair<int, int>> best_selected_edges;
    unordered_set<int> best_visited_vertices;
    unordered_set<int> best_used_labels;
    vector<int> best_label_priority_queue;
    size_t best_cost = numeric_limits<size_t>::max();
    bool any_success = false;

    int num_vertices = graph.get_vertices_count();

    for (int iter = 0; iter < iterations; ++iter)
    {
        reset_state();
        preprocess();

        // 1. Inicialização Randomizada
        // Coleta todos os rótulos que têm pelo menos uma aresta no grafo
        vector<int> sorted_initial_labels;
        for (const auto& pair : recurrence_map)
        {
            if (pair.second > 0)
            {
                sorted_initial_labels.push_back(pair.first);
            }
        }

        if (sorted_initial_labels.empty())
        {
            continue; // Grafo sem arestas
        }

        // Ordena sorted_initial_labels por frequência global (maior frequência primeiro)
        sort(sorted_initial_labels.begin(), sorted_initial_labels.end(), [this](int a, int b) {
            int freq_a = recurrence_map.at(a);
            int freq_b = recurrence_map.at(b);
            if (freq_a != freq_b)
            {
                return freq_a > freq_b;
            }
            return a < b; // Desempate consistente
        });

        // Calcula o tamanho da RCL de rótulos iniciais
        int rcl_size_init = 1 + static_cast<int>(alpha * (sorted_initial_labels.size() - 1));
        if (rcl_size_init < 1) rcl_size_init = 1;
        if (rcl_size_init > (int)sorted_initial_labels.size()) rcl_size_init = (int)sorted_initial_labels.size();

        // Sorteia um rótulo da RCL
        int chosen_label_init = sorted_initial_labels[RandomManager::next_int(0, rcl_size_init - 1)];

        // Encontra a primeira aresta com esse rótulo no grafo para inicializar
        pair<int, int> initial_edge;
        bool found_init_edge = false;
        for (int u = 0; u < num_vertices && !found_init_edge; ++u)
        {
            for (int v : graph.get_neighbors(u))
            {
                if (u < v && graph.get_edge_label(u, v) == chosen_label_init)
                {
                    initial_edge = {u, v};
                    found_init_edge = true;
                    break;
                }
            }
        }

        if (!found_init_edge)
        {
            continue; // Caso de segurança
        }

        selected_edges.push_back(initial_edge);
        visited_vertices.insert(initial_edge.first);
        visited_vertices.insert(initial_edge.second);
        used_labels.insert(chosen_label_init);

        update_priority_queue();

        // 2. Loop de Expansão Randomizada
        selected_edges_set.clear();
        selected_edges_set.insert({min(initial_edge.first, initial_edge.second), max(initial_edge.first, initial_edge.second)});

        unordered_map<int, vector<pair<int, int>>> frontier_by_label;

        // Inicializa a fronteira
        unordered_set<int> initial_visited = visited_vertices;
        visited_vertices.clear();
        for (int v : initial_visited)
        {
            visit_vertex(v, frontier_by_label);
        }

        bool disconnected = false;
        while ((int)visited_vertices.size() < num_vertices)
        {
            // Filtra os rótulos disponíveis na fronteira que têm arestas válidas
            vector<int> available_labels;
            for (int label : label_priority_queue)
            {
                auto it = frontier_by_label.find(label);
                if (it != frontier_by_label.end() && !it->second.empty())
                {
                    // Limpa arestas inválidas in place (evita alocações)
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
                        available_labels.push_back(label);
                    }
                }
            }

            if (available_labels.empty())
            {
                disconnected = true;
                break;
            }

            // Calcula o tamanho da RCL de rótulos
            int rcl_size = 1 + static_cast<int>(alpha * (available_labels.size() - 1));
            if (rcl_size < 1) rcl_size = 1;
            if (rcl_size > (int)available_labels.size()) rcl_size = (int)available_labels.size();

            // Sorteia o rótulo da RCL
            int chosen_label = available_labels[RandomManager::next_int(0, rcl_size - 1)];

            // Escolhe a melhor aresta para o chosen_label (lexicograficamente)
            const auto& edges = frontier_by_label[chosen_label];
            pair<int, int> best_edge;
            bool found_edge = false;
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

            // Atualiza o estado
            if (selected_edges_set.count(best_edge) == 0)
            {
                selected_edges.push_back(best_edge);
                selected_edges_set.insert(best_edge);
            }

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

            if (used_labels.count(chosen_label) == 0)
            {
                used_labels.insert(chosen_label);
                update_priority_queue();
            }
        }

        // 3. Avalia o resultado desta iteração
        if (!disconnected && visited_vertices.size() == (size_t)num_vertices)
        {
            if (used_labels.size() < best_cost)
            {
                best_cost = used_labels.size();
                best_selected_edges = selected_edges;
                best_visited_vertices = visited_vertices;
                best_used_labels = used_labels;
                best_label_priority_queue = label_priority_queue;
                any_success = true;
            }
        }
    }

    // 4. Salva a melhor solução no estado do solver
    reset_state();
    preprocess();

    if (any_success)
    {
        selected_edges = best_selected_edges;
        visited_vertices = best_visited_vertices;
        used_labels = best_used_labels;
        label_priority_queue = best_label_priority_queue;
        // Reconstrói selected_edges_set
        for (const auto& edge : selected_edges)
        {
            selected_edges_set.insert({min(edge.first, edge.second), max(edge.first, edge.second)});
        }
    }
}
