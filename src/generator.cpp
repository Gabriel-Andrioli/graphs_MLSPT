#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include "random_manager.hpp"

namespace fs = std::filesystem;
using namespace std;

struct ParameterSet {
    vector<int> values_n;
    vector<double> multipliers_l;
    vector<double> densities_d;
};

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

const int NUM_INSTANCES_PER_SCENARIO = 1;
const string OUTPUT_ROOT_DIR = "data";

struct GeneratedEdge {
    int u;
    int v;
    int label;
};
    
// Gera um grafo conexo rotulado e salva no caminho especificado
void generate_instance(int n, int l, int m, const string &filepath)
{
    vector<GeneratedEdge> edges;
    
    // Passo 1: Construir árvore geradora para garantir conectividade
    vector<int> connected;
    vector<int> unconnected;
    
    connected.push_back(0);
    for (int i = 1; i < n; ++i)
    {
        unconnected.push_back(i);
    }
    
    // Conjunto de chaves de arestas já inseridas para evitar duplicatas: u*n + v (onde u < v)
    // Usamos um vetor simples bool ou set para verificar de forma simples
    vector<vector<bool>> edge_exists(n, vector<bool>(n, false));

    while (!unconnected.empty())
    {
        // Escolhe um nó conectado aleatório
        int conn_idx = RandomManager::next_int(0, connected.size() - 1);
        int u = connected[conn_idx];
        
        // Escolhe um nó não conectado aleatório
        int unconn_idx = RandomManager::next_int(0, unconnected.size() - 1);
        int v = unconnected[unconn_idx];
        
        // Adiciona a aresta à árvore
        GeneratedEdge e;
        e.u = min(u, v);
        e.v = max(u, v);
        e.label = 0; // será definido no Passo 3
        edges.push_back(e);
        
        edge_exists[e.u][e.v] = true;
        
        // Move o nó para a lista de conectados
        connected.push_back(v);
        unconnected.erase(unconnected.begin() + unconn_idx);
    }
    
    // Passo 2: Adicionar as m - (n-1) arestas restantes aleatoriamente
    int remaining_edges = m - (n - 1);
    
    // Lista de todas as possíveis arestas que ainda não existem
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
    
    // Embaralha as arestas potenciais para selecionar de forma aleatória
    // Como estamos usando rand(), podemos fazer um Fisher-Yates shuffle simples
    for (int i = potential_edges.size() - 1; i > 0; --i)
    {
        int j = RandomManager::next_int(0, i);
        swap(potential_edges[i], potential_edges[j]);
    }
    
    // Adiciona as arestas necessárias da lista embaralhada
    for (int i = 0; i < remaining_edges && i < (int)potential_edges.size(); ++i)
    {
        GeneratedEdge e;
        e.u = potential_edges[i].first;
        e.v = potential_edges[i].second;
        e.label = 0;
        edges.push_back(e);
    }
    
    // Passo 3: Atribuição de Rótulos (Garantindo que todos os l rótulos aparecem)
    // Rótulos serão de 1 a l
    vector<int> label_pool;
    
    // Primeiro garante que cada rótulo aparece pelo menos uma vez
    for (int label = 1; label <= l; ++label)
    {
        label_pool.push_back(label);
    }
    
    // Preenche o restante até m com rótulos aleatórios de 1 a l
    while ((int)label_pool.size() < m)
    {
        label_pool.push_back(RandomManager::next_int(1, l));
    }
    
    // Embaralha o pool de rótulos para distribuir aleatoriamente entre as arestas
    for (int i = label_pool.size() - 1; i > 0; --i)
    {
        int j = RandomManager::next_int(0, i);
        swap(label_pool[i], label_pool[j]);
    }
    
    // Atribui os rótulos embaralhados às arestas
    for (size_t i = 0; i < edges.size(); ++i)
    {
        edges[i].label = label_pool[i];
    }
    
    // Passo 4: Salvar a Instância no Arquivo
    ofstream file(filepath);
    if (!file.is_open())
    {
        cerr << "Erro ao criar arquivo: " << filepath << endl;
        return;
    }
    
    // Escreve os vértices de 0 a n-1, um por linha
    for (int i = 0; i < n; ++i)
    {
        file << i << "\n";
    }
    
    // Escreve as arestas no formato: u v label
    for (const auto &e : edges)
    {
        file << e.u << " " << e.v << " " << e.label << "\n";
    }
    
    file.close();
}

int main(int argc, char *argv[])
{
    // Inicializa a semente global a partir dos argumentos ou hora do sistema
    RandomManager::initialize(argc, argv);
    
    cout << "Iniciando a geracao de instancias do MLSTP...\n";
    
    // Garante que o diretório root existe
    try {
        fs::create_directories(OUTPUT_ROOT_DIR);
    } catch (const exception &e) {
        cerr << "Nao foi possivel criar o diretorio " << OUTPUT_ROOT_DIR << ": " << e.what() << endl;
        return 1;
    }
    
    int total_files = 0;
    
    // Loop por todos os conjuntos de parâmetros
    for (const auto &set : PARAMETER_SETS)
    {
        for (int n : set.values_n)
        {
            for (double l_mult : set.multipliers_l)
            {
                int l = static_cast<int>(l_mult * n);
                for (double d : set.densities_d)
                {
                    // Calcula m = (d * (n - 1) * n) / 2
                    int m = static_cast<int>((d * (n - 1) * n) / 2.0);
                    
                    // Cria o nome do arquivo diretamente na pasta OUTPUT_ROOT_DIR
                    // Nome do arquivo: scenario_n<n>_l<l>_d<d>_instance01.txt
                    stringstream ss;
                    ss << OUTPUT_ROOT_DIR << "/scenario_n" << n << "_l" << l << "_d" << fixed << setprecision(2) << d << "_instance01.txt";
                    string filepath = ss.str();
                    
                    generate_instance(n, l, m, filepath);
                    total_files++;
                }
            }
        }
    }
    
    cout << "Geracao concluida! Total de " << total_files << " arquivos de instancias gerados na pasta '" << OUTPUT_ROOT_DIR << "'.\n";
    
    return 0;
}
