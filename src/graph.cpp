#include "graph.hpp"

using namespace std;

// Construtores e destrutores
Node::Node(int id)
{
    this->id = id;
    this->visited = false;
    this->in_degree = 0;
    this->out_degree = 0;
}

Graph::Graph(bool directed, bool weighted)
{
    this->directed = directed;
    this->weighted = weighted;
}

Node *Graph::find_node(int id)
{
    for (Node *n : nodes)
    {
        if (n->id == id)
        {
            return n;
        }
    }
    return nullptr;
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

int Graph::find_neighbor_index(int u, int v)
{
    Node *node_u = find_node(u);
    if (!node_u)
        return -1; // nó u não existente

    for (int i = 0; i < node_u->neighbors.size(); i++)
    {
        if (node_u->neighbors[i].target->id == v)
        {
            return i;
        }
    }
    return -1;
}

int Graph::add_vertex(int id)
{
    // verifica se já existe um nó com este id
    if (find_node(id))
        return -1; // id já existe

    nodes.push_back(new Node(id));
    return id;
}

bool Graph::remove_vertex(int id)
{
    Node *target = nullptr;
    int i;
    for (i = 0; i < nodes.size(); i++)
    {
        if (nodes[i]->id == id)
        {
            target = nodes[i];
            break;
        }
    }
    if (!target)
        return false;

    // remove arestas que apontam para este no em todos os outros nos
    for (Node *n : nodes)
    {
        if (n->id == id) // propria lista do no que será removido
            continue;

        for (int j = (int)n->neighbors.size() - 1; j >= 0; j--)
        {
            if (n->neighbors[j].target->id == id)
            {
                n->neighbors.erase(n->neighbors.begin() + j);
                // ajusta graus
                n->out_degree--;
                target->in_degree--;
            }
        }
    }

    // deleta o no e remove do vetor
    delete target;
    nodes.erase(nodes.begin() + i);
    return true;
}

bool Graph::add_edge(int u, int v, int weight)
{
    // localiza os nós pelos ids usando find_node
    Node *node_u = find_node(u);
    Node *node_v = find_node(v);
    if (!node_u || !node_v)
        return false;

    // verifica se a aresta ja existe
    if (has_edge(u, v))
        return false;

    if (!this->weighted)
    {
        weight = 1;
    }

    // insere a aresta u -> v
    node_u->neighbors.push_back(Edge(node_v, weight));
    node_u->out_degree++;
    node_v->in_degree++;

    // se nao direcionado, insere tambem v -> u
    if (!directed)
    {
        node_v->neighbors.push_back(Edge(node_u, weight));
        node_v->out_degree++;
        node_u->in_degree++;
    }

    return true;
}

bool Graph::remove_edge(int u, int v)
{
    // localiza os nos pelos ids
    Node *node_u = find_node(u);
    Node *node_v = find_node(v);
    if (!node_u || !node_v)
        return false;

    int index_uv = find_neighbor_index(u, v);
    if (index_uv == -1)
        return false; // não existe aresta entre os nos

    // remove u -> v
    node_u->neighbors.erase(node_u->neighbors.begin() + index_uv);
    node_u->out_degree--;
    node_v->in_degree--;

    // se nao for direcionado, remove v -> u
    if (!directed)
    {
        int index_vu = find_neighbor_index(v, u);
        if (index_vu != -1)
        {
            node_v->neighbors.erase(node_v->neighbors.begin() + index_vu);
            node_v->out_degree--;
            node_u->in_degree--;
        }
    }

    return true;
}

bool Graph::has_edge(int u, int v)
{
    return find_neighbor_index(u, v) != -1;
}

bool Graph::set_edge_weight(int u, int v, int weight)
{
    if (!this->weighted)
    {
        cout << "O grafo não é ponderado. Essa operação (set_edge_weight) não é permitida.\n";
        return false;
    }
    int i = find_neighbor_index(u, v);
    if (i == -1)
        return false; // não existe aresta entre os nos

    // localiza o no u
    for (Node *n : nodes)
    {
        if (n->id == u)
        {
            n->neighbors[i].weight = weight;
            break;
        }
    }

    // se nao direcionado, atualiza tambem o peso da aresta inversa
    if (!directed)
    {
        int i_rev = find_neighbor_index(v, u);
        if (i_rev != -1)
        {
            for (Node *n : nodes)
            {
                if (n->id == v)
                {
                    n->neighbors[i_rev].weight = weight;
                    break;
                }
            }
        }
    }

    return true;
}

void Graph::print()
{
    if (nodes.empty())
    {
        cout << "(grafo vazio)\n";
        return;
    }

    cout << (directed ? "Grafo Direcionado" : "Grafo Nao Direcionado");
    if (weighted)
        cout << " (ponderado)";
    cout << "\n";

    for (Node *node : nodes)
    {
        cout << node->id << ": ";
        for (size_t i = 0; i < node->neighbors.size(); i++)
        {
            if (weighted)
            {
                cout << node->neighbors[i].target->id
                     << "(" << node->neighbors[i].weight << ") ";
            }
            else
            {
                cout << node->neighbors[i].target->id << " ";
            }
        }
        cout << "\n";
    }
}

int Graph::degree(int id)
{
    Node *n = find_node(id);
    if (!n)
        return -1; // vertice nao encontrado
    if (directed)
        return n->in_degree + n->out_degree;
    else
        return (int)n->neighbors.size();
}

vector<int> Graph::get_neighbors(int id)
{
    vector<int> result;
    Node *n = find_node(id);
    if (n)
    {
        for (Edge &e : n->neighbors)
        {
            result.push_back(e.target->id);
        }
    }
    return result;
}

bool Graph::are_adjacent(int u, int v)
{
    return has_edge(u, v);
}

void Graph::dfs_recursive(int start)
{
    // encontra o nó de partida usando find_node
    Node *start_node = find_node(start);
    if (!start_node)
    {
        cout << "Vertice " << start << " nao encontrado.\n";
        return;
    }

    clear_visited();
    dfs_recursive_helper(start_node);
    cout << "\n";
}

void Graph::dfs_recursive_helper(Node *node)
{
    node->visited = true;
    cout << node->id << " ";

    for (Edge &e : node->neighbors)
    {
        if (!e.target->visited)
        {
            dfs_recursive_helper(e.target);
        }
    }
}
