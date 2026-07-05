#include "solver.hpp"
#include <algorithm>

using namespace std;

MLSPTSolver::MLSPTSolver(const Graph &g) : graph(g)
{
}

void MLSPTSolver::reset_state()
{
    recurrence_map.clear();
    selected_edges.clear();
    visited_vertices.clear();
    used_labels.clear();
    label_priority_queue.clear();
    selected_edges_set.clear();
}

void MLSPTSolver::visit_vertex(int v, unordered_map<int, vector<pair<int, int>>> &frontier_by_label)
{
    visited_vertices.insert(v);
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
}

void MLSPTSolver::preprocess()
{
    recurrence_map = graph.get_label_frequencies();
}

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

double MLSPTSolver::get_best_alpha() const
{
    return best_alpha;
}
