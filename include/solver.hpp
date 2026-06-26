#ifndef SOLVER_HPP
#define SOLVER_HPP

#include "graph.hpp"
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <utility>

class MLSPTSolver
{
private:
    const Graph &graph;

    // Fase 1: Estado do pré-processamento
    std::unordered_map<int, int> recurrence_map;          // Mapa de recorrência global (Rótulo -> Frequência)
    std::vector<std::pair<int, int>> unavoidable_edges;   // Arestas incontornáveis (u, v) onde u < v
    std::unordered_set<int> unavoidable_labels;           // Rótulos incontornáveis identificados

    // Fase 2: Estado da solução ativa
    std::vector<std::pair<int, int>> selected_edges;      // Solução S
    std::unordered_set<int> visited_vertices;             // Visitados
    std::unordered_set<int> used_labels;                  // Usados
    std::vector<int> label_priority_queue;                // Fila de Prioridades

    // Estrutura de hash para std::pair para podermos usar em std::unordered_set
    struct PairHash {
        std::size_t operator()(const std::pair<int, int> &p) const {
            return std::hash<int>()(p.first) ^ std::hash<int>()(p.second);
        }
    };
    std::unordered_set<std::pair<int, int>, PairHash> selected_edges_set; // Busca rápida para duplicados

    // Métodos privados das fases
    void preprocess();            // Fase 1
    void initialize_solution();   // Fase 2
    void update_priority_queue(); // Ordenação da fila de prioridade
    void expansion_loop();        // Fase 3 e 4

public:
    MLSPTSolver(const Graph &graph);
    
    // Método principal de orquestração do solver
    void solve();
    
    // Getters para fins de teste
    const std::vector<std::pair<int, int>>& get_unavoidable_edges() const;
    const std::unordered_set<int>& get_unavoidable_labels() const;
    const std::unordered_map<int, int>& get_recurrence_map() const;
    const std::vector<std::pair<int, int>>& get_selected_edges() const;
    const std::unordered_set<int>& get_visited_vertices() const;
    const std::unordered_set<int>& get_used_labels() const;
    const std::vector<int>& get_label_priority_queue() const;
};

#endif // SOLVER_HPP
