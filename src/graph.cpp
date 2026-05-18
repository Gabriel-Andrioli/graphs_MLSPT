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

// Getters para directed e weighted
bool Graph::is_directed() const
{
    return directed;
}

bool Graph::is_weighted() const
{
    return weighted;
}

Node *Graph::find_node(int id)
{
    auto it = node_map.find(id);
    if (it != node_map.end())
        return it->second;
    return nullptr;
}

Graph::~Graph()
{
    for (Node *node : nodes)
    {
        delete node;
    }
    node_map.clear(); // limpa o mapa
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

    Node *novo = new Node(id);
    nodes.push_back(novo);
    node_map[id] = novo; // adiciona ao mapa
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

    node_map.erase(id); // remove do mapa
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

void Graph::dfs_collect(Node *node, vector<int> &component,
                        const vector<vector<int>> &ghostNodeNeighbors,
                        const unordered_map<int, int> &node_index)
{
    node->visited = true;
    component.push_back(node->id);

    // Percorre vizinhos diretos (arestas forward)
    for (Edge &e : node->neighbors)
    {
        if (!e.target->visited)
        {
            dfs_collect(e.target, component, ghostNodeNeighbors, node_index);
        }
    }

    // Se houver ghostNodeNeighbors (apenas para grafos direcionados),
    // percorre os vizinhos reversos (arcos que chegam neste no)
    if (!ghostNodeNeighbors.empty())
    {
        int i = node_index.at(node->id);
        for (int rev_id : ghostNodeNeighbors[i])
        {
            Node *rev_node = find_node(rev_id);
            if (rev_node && !rev_node->visited)
            {
                dfs_collect(rev_node, component, ghostNodeNeighbors, node_index);
            }
        }
    }
}

vector<vector<int>> Graph::connected_components()
{
    vector<vector<int>> components;
    clear_visited();

    // Mapeamento de id do no para indice no vetor nodes (para acesso O(1) ao ghostNodeNeighbors)
    unordered_map<int, int> node_index;
    for (int i = 0; i < (int)nodes.size(); i++)
    {
        node_index[nodes[i]->id] = i;
    }

    // Constroi lista de adjacencia invertida apenas para grafos direcionados
    // ghostNodeNeighbors[v] contem todos os ids u tais que existe arco u -> v
    vector<vector<int>> ghostNodeNeighbors;
    if (directed)
    {
        ghostNodeNeighbors.resize(nodes.size());
        for (Node *u : nodes)
        {
            for (Edge &e : u->neighbors)
            {
                int v_idx = node_index[e.target->id];
                ghostNodeNeighbors[v_idx].push_back(u->id);
            }
        }
    }

    // Percorre todos os vertices, executando DFS a partir de cada no nao visitado
    for (Node *node : nodes)
    {
        if (!node->visited)
        {
            vector<int> component;
            dfs_collect(node, component, ghostNodeNeighbors, node_index);
            components.push_back(component);
        }
    }

    return components;
}

void Graph::print_connected_components()
{
    vector<vector<int>> components = connected_components();

    cout << "\n====================================\n";
    cout << "Componentes Conexas\n";
    cout << "====================================\n";
    cout << "Total de componentes: " << components.size() << "\n\n";

    for (int i = 0; i < (int)components.size(); i++)
    {
        cout << "Componente " << i + 1 << " (tamanho " << components[i].size() << "): ";
        for (int j = 0; j < (int)components[i].size(); j++)
        {
            cout << components[i][j];
            if (j < (int)components[i].size() - 1)
                cout << ", ";
        }
        cout << "\n";
    }
    cout << "====================================\n";
}
