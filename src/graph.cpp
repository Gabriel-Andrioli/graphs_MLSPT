#include "graph.hpp"

using namespace std;

Node::Node(int id) : id(id), visited(false), in_degree(0), out_degree(0) {}

Graph::Graph(int n) : num_vertices(n)
{
    for (int i = 0; i < n; i++)
    {
        nodes.push_back(new Node(i));
    }
}

Graph::~Graph()
{
    for (Node *node : nodes)
    {
        delete node;
    }
}

void Graph::clear_visited()
{
    for (Node *node : nodes)
    {
        node->visited = false;
    }
}

void Graph::add_edge(int u, int v, bool directed)
{
    nodes[u]->neighbors.push_back(nodes[v]);
    nodes[u]->out_degree++;
    nodes[v]->in_degree++;

    if (!directed)
    {
        nodes[v]->neighbors.push_back(nodes[u]);
        nodes[v]->out_degree++;
        nodes[u]->in_degree++;
    }
}

void Graph::print()
{
    for (Node *node : nodes)
    {
        cout << node->id << ": ";
        for (Node *neighbor : node->neighbors)
        {
            cout << neighbor->id << " ";
        }
        cout << "\n";
    }
}
// ------------------------------------
// EXEMPLO:
// Busca em profundidade (recursiva)
// ------------------------------------

void Graph::dfs_recursive(int start)
{
    clear_visited();
    dfs_recursive_helper(nodes[start]);
}

void Graph::dfs_recursive_helper(Node *node)
{
    node->visited = true;
    cout << node->id << " ";

    for (Node *neighbor : node->neighbors)
    {
        if (!neighbor->visited)
        {
            dfs_recursive_helper(neighbor);
        }
    }
}