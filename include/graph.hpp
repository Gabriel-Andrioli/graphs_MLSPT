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
    int label; // Rótulo (label) para o MLSTP
    Edge(Node *target, int label)
    {
        this->target = target;
        this->label = label;
    };
};

class Node
{
public:
    int id;                 // id do nó
    bool visited;           // flag de visitado
    vector<Edge> neighbors; // lista de adjacências
    Node(int id);
};

class Graph
{
private:
    vector<Node *> nodes;                // vetor de ponteiros para todos os nos do grafo
    unordered_map<int, Node *> node_map; // mapa para acesso O(1) a nós por id
    void clear_visited();                // limpa o flag visitado de todos os nos
    Node *find_node(int id) const;       // procura no vetor de ponteiros de nós o nó com id informado

public:
    Graph();
    ~Graph();

    int add_vertex(int id);                           // 1. Inserir vértice
    bool remove_vertex(int id);                       // 2. Remover vértice
    bool add_edge(int u, int v, int label = 0);       // 3. Inserir aresta com rótulo
    bool remove_edge(int u, int v);                   // 4. Remover aresta
    bool has_edge(int u, int v) const;                // 5. Verificar existência de aresta
    void print() const;                               // 6. Exibir o grafo

    int get_vertices_count() const;                   // Retorna o número de vértices
    int degree(int id) const;                         // 7. Calcular grau de um vértice
    vector<int> get_neighbors(int id) const;          // 8. Listar vizinhos de um vértice
    bool are_adjacent(int u, int v) const;            // 9. Verificar se dois vértices são adjacentes
    int get_edge_label(int u, int v) const;           // Retorna o rótulo da aresta entre u e v

    // Leitura de arquivo
    bool readFromFile(const string &filename);

private:
    int find_neighbor_index(int u, int v) const; // procura indice de um nó na lista de adjacência de outro
};

#endif

