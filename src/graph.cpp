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

bool Node::is_mono_label() const
{
    return label_counts.size() == 1;
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

void Graph::clear()
{
    for (Node *node : nodes)
    {
        delete node;
    }
    nodes.clear();
    node_map.clear();
    label_frequencies.clear();
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
    Node *target = find_node(id);
    if (!target)
        return false;

    // Para cada vizinho do nó a ser removido, removemos a aresta reversa e atualizamos as estatísticas
    for (const Edge &e : target->neighbors)
    {
        Node *neighbor = e.target;
        int label = e.label;

        // Decrementa contadores locais do vizinho
        neighbor->label_counts[label]--;
        if (neighbor->label_counts[label] == 0)
        {
            neighbor->label_counts.erase(label);
        }

        // Decrementa a recorrência global
        label_frequencies[label]--;
        if (label_frequencies[label] == 0)
        {
            label_frequencies.erase(label);
        }

        // Remove a aresta do vizinho que aponta para o nó deletado
        for (int j = (int)neighbor->neighbors.size() - 1; j >= 0; j--)
        {
            if (neighbor->neighbors[j].target->id == id)
            {
                neighbor->neighbors.erase(neighbor->neighbors.begin() + j);
            }
        }
    }

    // Localiza e remove o nó do vetor nodes
    int i;
    for (i = 0; i < (int)nodes.size(); i++)
    {
        if (nodes[i]->id == id)
        {
            break;
        }
    }
    delete target;
    nodes.erase(nodes.begin() + i);
    node_map.erase(id);

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

    // Atualiza contadores locais dos nós
    node_u->label_counts[label]++;
    node_v->label_counts[label]++;

    // Atualiza contadores globais do grafo
    label_frequencies[label]++;

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

    int label = node_u->neighbors[index_uv].label;

    // remove u -> v
    node_u->neighbors.erase(node_u->neighbors.begin() + index_uv);

    // remove v -> u (como é não direcionado, deve existir v -> u)
    int index_vu = find_neighbor_index(v, u);
    if (index_vu != -1)
    {
        node_v->neighbors.erase(node_v->neighbors.begin() + index_vu);
    }

    // Decrementa contadores locais
    node_u->label_counts[label]--;
    if (node_u->label_counts[label] == 0)
        node_u->label_counts.erase(label);

    node_v->label_counts[label]--;
    if (node_v->label_counts[label] == 0)
        node_v->label_counts.erase(label);

    // Decrementa contador global
    label_frequencies[label]--;
    if (label_frequencies[label] == 0)
        label_frequencies.erase(label);

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
    // Tenta diferentes caminhos comuns dependendo de onde o executável foi chamado
    string possible_paths[] = {
        filename,                  // Se o usuário passar o caminho completo ou executar da pasta data/
        "data/" + filename,        // Se executar da raiz do projeto (graphs_MLSPT/)
        "../data/" + filename,     // Se executar da pasta build/ (comum no Linux)
        "../../data/" + filename   // Se executar da pasta build/Debug/ (comum no Windows/Visual Studio)
    };

    ifstream file;
    string filepath_used = "";

    for (const string& p : possible_paths) {
        file.open(p);
        if (file.is_open()) {
            filepath_used = p;
            break;
        }
    }

    if (!file.is_open())
    {
        cerr << "Erro ao abrir arquivo em nenhum dos caminhos previstos para: " << filename << endl;
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

int Graph::get_label_frequency(int label) const
{
    auto it = label_frequencies.find(label);
    if (it != label_frequencies.end())
        return it->second;
    return 0;
}

const unordered_map<int, int>& Graph::get_label_frequencies() const
{
    return label_frequencies;
}

bool Graph::is_vertex_mono_label(int id) const
{
    Node *n = find_node(id);
    if (n)
        return n->is_mono_label();
    return false;
}