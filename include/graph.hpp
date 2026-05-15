#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <vector>
#include <queue>
#include <stack>
#include <iostream>

using namespace std;

class Node
{
public:
    int id;
    bool visited;
    int in_degree;
    int out_degree;
    int componentId;
    vector<Node *> neighbors;

    Node(int id);
};

class Graph
{
private:
    int num_vertices;
    vector<Node *> nodes;
    void clear_visited(); // limpa o flag visitado de todos os nos

public:
    Graph(int n);
    ~Graph();

    void add_edge(int u, int v, bool directed = false);
    void print(); // imprime o grafo

    void dfs_recursive(int start); // busca em profundidade (recursiva)

private:
    void dfs_recursive_helper(Node *node); // auxiliar recursiva da dfs
};

#endif