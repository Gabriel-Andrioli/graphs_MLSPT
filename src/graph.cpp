#include "graph.hpp"
#include <fstream>
#include <sstream>

using namespace std;

// Construtores e destrutores
Node::Node(int id)
{
    this->id = id;
    this->visited = false;
}

Graph::Graph()
{
}

// Retorna ponteiro para nó a partir do id
Node *Graph::find_node(int id) const
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

// Limpa o flag de visitado de todos os nós
void Graph::clear_visited()
{
    for (Node *node : nodes)
    {
        node->visited = false;
    }
}

// Procura indice de um nó na lista de adjacência de outro
int Graph::find_neighbor_index(int u, int v) const
{
    Node *node_u = find_node(u);
    if (!node_u)
        return -1; // nó u não existente

    for (int i = 0; i < (int)node_u->neighbors.size(); i++)
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
    for (i = 0; i < (int)nodes.size(); i++)
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
            }
        }
    }

    // deleta o no e remove do vetor
    delete target;
    nodes.erase(nodes.begin() + i);

    node_map.erase(id); // remove do mapa
    return true;
}

bool Graph::add_edge(int u, int v, int label)
{
    // localiza os nós pelos ids usando find_node
    Node *node_u = find_node(u);
    Node *node_v = find_node(v);
    if (!node_u || !node_v)
        return false;

    // verifica se a aresta ja existe
    if (has_edge(u, v))
        return false;

    // insere a aresta u -> v e v -> u (não direcionado)
    node_u->neighbors.push_back(Edge(node_v, label));
    node_v->neighbors.push_back(Edge(node_u, label));

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

    // remove v -> u (como é não direcionado, deve existir v -> u)
    int index_vu = find_neighbor_index(v, u);
    if (index_vu != -1)
    {
        node_v->neighbors.erase(node_v->neighbors.begin() + index_vu);
    }

    return true;
}

bool Graph::has_edge(int u, int v) const
{
    return find_neighbor_index(u, v) != -1;
}

void Graph::print() const
{
    if (nodes.empty())
    {
        cout << "(grafo vazio)\n";
        return;
    }

    // Primeiro: imprime todos os vertices, um por linha
    for (Node *node : nodes)
    {
        cout << node->id << "\n";
    }

    // Depois: imprime todas as arestas, uma por linha no formato u v label
    for (Node *node : nodes)
    {
        for (const Edge &edge : node->neighbors)
        {
            // Para grafos nao direcionados, imprime cada aresta uma unica vez
            // (apenas quando node->id < edge.target->id, evitando o par inverso)
            if (node->id < edge.target->id)
            {
                cout << node->id << " " << edge.target->id << " " << edge.label << "\n";
            }
        }
    }
}

int Graph::get_vertices_count() const
{
    return (int)nodes.size();
}

int Graph::degree(int id) const
{
    Node *n = find_node(id);
    if (!n)
        return -1; // vertice nao encontrado
    return (int)n->neighbors.size();
}

vector<int> Graph::get_neighbors(int id) const
{
    vector<int> result;
    Node *n = find_node(id);
    if (n)
    {
        for (const Edge &e : n->neighbors)
        {
            result.push_back(e.target->id);
        }
    }
    return result;
}

bool Graph::are_adjacent(int u, int v) const
{
    return has_edge(u, v);
}

int Graph::get_edge_label(int u, int v) const
{
    int idx = find_neighbor_index(u, v);
    if (idx == -1)
        return -1; // aresta não encontrada
    Node *node_u = find_node(u);
    return node_u->neighbors[idx].label;
}

// Leitura de arquivo
bool Graph::readFromFile(const string &filename)
{
    string filepath = "../graphFiles/" + filename;
    ifstream file(filepath);

    if (!file.is_open())
    {
        cerr << "Erro ao abrir arquivo: " << filepath << endl;
        return false;
    }

    string line;
    while (getline(file, line))
    {
        // Ignora linhas vazias
        if (line.empty())
            continue;

        // Remove espaços em branco no início e fim
        size_t first = line.find_first_not_of(' ');
        size_t last = line.find_last_not_of(' ');
        if (first == string::npos || last == string::npos) // linha vazia
            continue;
        line = line.substr(first, (last - first) + 1);

        // Verifica se a linha contém apenas um número (vértice)
        // ou dois/três números separados por espaço (aresta)
        stringstream ss(line);
        int u, v = -1;
        ss >> u;

        if (ss.fail())
        {
            // Linha inválida, ignora
            continue;
        }

        // Tenta ler o segundo número
        if (ss >> v)
        {
            // Linha contém dois ou três números: aresta u - v (com rótulo opcional)
            int label;
            if (ss >> label)
            {
                // Linha contém três números: aresta u - v com rótulo
                if (!add_edge(u, v, label))
                {
                    cerr << "Aviso: Não foi possível adicionar aresta " << u << " - " << v
                         << " (rótulo " << label << ")" << endl;
                }
            }
            else
            {
                // Linha contém dois números: aresta sem rótulo explícito (rótulo padrão = 0)
                if (!add_edge(u, v, 0))
                {
                    cerr << "Aviso: Não foi possível adicionar aresta " << u << " - " << v << endl;
                }
            }
        }
        else
        {
            // Linha contém apenas um número: vértice
            if (add_vertex(u) == -1)
            {
                cerr << "Aviso: Não foi possível adicionar vértice " << u << " (id já existe).\n";
            }
        }
    }

    file.close();
    return true;
}