#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <vector>
#include <queue>
#include <stack>
#include <iostream>
#include <unordered_map>
#include <fstream>
#include <sstream>

using namespace std;

class Node;

struct Edge
{
    Node *target;
    float weight;
    Edge(Node *target, float weight)
    {
        this->target = target;
        this->weight = weight;
    };
};

class Node
{
public:
    int id;                 // id do nó
    bool visited;           // flag de visitado
    int in_degree;          // grau de entrada
    int out_degree;         // grau de saida
    vector<Edge> neighbors; // lista de adjacências
    Node(int id);
};

class Graph
{
private:
    vector<Node *> nodes;                // vetor de ponteiros para todos os nos do grafo
    unordered_map<int, Node *> node_map; // mapa para acesso O(1) a nós por id
    bool directed;                       // flag de direcionado
    bool weighted;                       // flag de ponderado
    void clear_visited();                // limpa o flag visitado de todos os nos
    Node *find_node(int id);             // procura no vetor de ponteiros de nós o nó com id informado

public:
    Graph(bool directed = false, bool weighted = false);
    ~Graph();

    // Getters para directed e weighted
    bool is_directed();
    bool is_weighted();

    int add_vertex(int id);                           // 1. Inserir vértice
    bool remove_vertex(int id);                       // 2. Remover vértice
    bool add_edge(int u, int v, float weight = 1);    // 3. Inserir aresta
    bool remove_edge(int u, int v);                   // 4. Remover aresta
    bool has_edge(int u, int v);                      // 5. Verificar existência de aresta
    bool set_edge_weight(int u, int v, float weight); // 6. Alterar peso de uma aresta
    void print();                                     // 7. Exibir o grafo

    int degree(int id);                // 8. Calcular grau de um vértice
    vector<int> get_neighbors(int id); // 9. Listar vizinhos de um vértice
    bool are_adjacent(int u, int v);   // 10. Verificar se dois vértices são adjacentes

    void dfs_recursive(int start);

    // Algoritmo de componentes conexas
    vector<vector<int>> connected_components();
    void print_connected_components();

    // Leitura de arquivo
    bool readFromFile(const string &filename);

private:
    void dfs_recursive_helper(Node *node);

    int find_neighbor_index(int u, int v); // procura indice de um nó na lista de adjacência de outro, informando sua posição/indice na lista

    // helpers para componentes conexas
    void dfs_collect(Node *node, vector<int> &component,
                     const vector<vector<int>> &reverse_adj,
                     const unordered_map<int, int> &node_index);
};

#endif
