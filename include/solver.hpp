#ifndef SOLVER_HPP
#define SOLVER_HPP

#include "graph.hpp"
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <utility>

using namespace std;

class MLSPTSolver
{
private:
    const Graph &graph;
    unordered_map<int, int> recurrence_map;          // Mapa de recorrência global (Rótulo -> Frequência)
    vector<pair<int, int>> selected_edges;           // Solução S
    unordered_set<int> visited_vertices;             // Visitados
    unordered_set<int> used_labels;                  // Usados
    vector<int> label_priority_queue;                // Fila de Prioridades
    struct PairHash {
        size_t operator()(const pair<int, int> &p) const {
            return hash<int>()(p.first) ^ hash<int>()(p.second);
        }
    };
    unordered_set<pair<int, int>, PairHash> selected_edges_set; // Busca rápida para duplicados

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
    const unordered_map<int, int>& get_recurrence_map() const;
    const vector<pair<int, int>>& get_selected_edges() const;
    const unordered_set<int>& get_visited_vertices() const;
    const unordered_set<int>& get_used_labels() const;
    const vector<int>& get_label_priority_queue() const;
};

#endif // SOLVER_HPP
