#include "generator_core.hpp"
#include "random_manager.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace fs = std::filesystem;
using namespace std;

const vector<ParameterSet> PARAMETER_SETS = {
    // Grupo 1 de Cerulli: n=20,30,40,50; l=n (multiplicador 1.0); d=0.2,0.5,0.8
    { {20, 30, 40, 50}, {1.0}, {0.2, 0.5, 0.8} },
    
    // Grupo 2 de Cerulli (n=50): omitido multiplicador 1.0 (l=50) para evitar duplicados
    { {50}, {0.25, 0.5, 1.25}, {0.2, 0.5, 0.8} },
    
    // Grupo 2 de Cerulli (n=100): todos os multiplicadores {0.25, 0.5, 1.0, 1.25}
    { {100}, {0.25, 0.5, 1.0, 1.25}, {0.2, 0.5, 0.8} },
    
    // Grupo de Consoli: n=500,1000; l={0.25, 0.5, 0.75, 1.0, 1.25}; d=0.2,0.5,0.8
    { {500, 1000}, {0.25, 0.5, 0.75, 1.0, 1.25}, {0.2, 0.5, 0.8} }
};

string get_project_root()
{
    fs::path current_path = fs::current_path();
    while (!fs::exists(current_path / "CMakeLists.txt") && !fs::exists(current_path / ".gitignore")) {
        if (!current_path.has_parent_path() || current_path == current_path.parent_path()) {
            return ".";
        }
        current_path = current_path.parent_path();
    }
    return current_path.string();
}

void generate_instance(int n, int l, int m, const string &filepath)
{
    vector<GeneratedEdge> edges;
    
    vector<int> connected;
    vector<int> unconnected;
    
    connected.push_back(0);
    for (int i = 1; i < n; ++i)
    {
        unconnected.push_back(i);
    }
    
    vector<vector<bool>> edge_exists(n, vector<bool>(n, false));

    while (!unconnected.empty())
    {
        int conn_idx = RandomManager::next_int(0, connected.size() - 1);
        int u = connected[conn_idx];
        
        int unconn_idx = RandomManager::next_int(0, unconnected.size() - 1);
        int v = unconnected[unconn_idx];
        
        GeneratedEdge e;
        e.u = min(u, v);
        e.v = max(u, v);
        e.label = 0; 
        edges.push_back(e);
        
        edge_exists[e.u][e.v] = true;
        
        connected.push_back(v);
        unconnected.erase(unconnected.begin() + unconn_idx);
    }
    
    int remaining_edges = m - (n - 1);
    
    vector<pair<int, int>> potential_edges;
    for (int i = 0; i < n; ++i)
    {
        for (int j = i + 1; j < n; ++j)
        {
            if (!edge_exists[i][j])
            {
                potential_edges.push_back({i, j});
            }
        }
    }
    
    for (int i = potential_edges.size() - 1; i > 0; --i)
    {
        int j = RandomManager::next_int(0, i);
        swap(potential_edges[i], potential_edges[j]);
    }
    
    for (int i = 0; i < remaining_edges && i < (int)potential_edges.size(); ++i)
    {
        GeneratedEdge e;
        e.u = potential_edges[i].first;
        e.v = potential_edges[i].second;
        e.label = 0;
        edges.push_back(e);
    }
    
    vector<int> label_pool;
    
    for (int label = 1; label <= l; ++label)
    {
        label_pool.push_back(label);
    }
    
    while ((int)label_pool.size() < m)
    {
        label_pool.push_back(RandomManager::next_int(1, l));
    }
    
    for (int i = label_pool.size() - 1; i > 0; --i)
    {
        int j = RandomManager::next_int(0, i);
        swap(label_pool[i], label_pool[j]);
    }
    
    for (size_t i = 0; i < edges.size(); ++i)
    {
        edges[i].label = label_pool[i];
    }
    
    ofstream file(filepath);
    if (!file.is_open())
    {
        cerr << "Erro ao criar arquivo: " << filepath << endl;
        return;
    }
    
    for (int i = 0; i < n; ++i)
    {
        file << i << "\n";
    }
    
    for (const auto &e : edges)
    {
        file << e.u << " " << e.v << " " << e.label << "\n";
    }
    
    file.close();
}

int run_full_generation(const string &output_dir)
{
    try {
        fs::create_directories(output_dir);
    } catch (const exception &e) {
        cerr << "Nao foi possivel criar o diretorio " << output_dir << ": " << e.what() << endl;
        return 0;
    }
    
    int total_files = 0;
    
    for (const auto &set : PARAMETER_SETS)
    {
        for (int n : set.values_n)
        {
            for (double l_mult : set.multipliers_l)
            {
                int l = static_cast<int>(l_mult * n);
                for (double d : set.densities_d)
                {
                    int m = static_cast<int>((d * (n - 1) * n) / 2.0);
                    
                    stringstream ss;
                    ss << output_dir << "/scenario_n" << n << "_l" << l << "_d" << fixed << setprecision(2) << d << "_instance01.txt";
                    string filepath = ss.str();
                    
                    generate_instance(n, l, m, filepath);
                    total_files++;
                }
            }
        }
    }
    
    return total_files;
}
