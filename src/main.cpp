#include <iostream>
#include <limits>
#include "graph.hpp"

using namespace std;

pair<bool, bool> configure_graph()
{
    char directed_char, weighted_char;
    cout << "=== CONFIGURACAO DO GRAFO ===\n";

    do
    {
        cout << "Grafo direcionado? (S/N): ";
        cin >> directed_char;
    } while (toupper(directed_char) != 'S' && toupper(directed_char) != 'N');

    do
    {
        cout << "Grafo ponderado? (S/N): ";
        cin >> weighted_char;
    } while (toupper(weighted_char) != 'S' && toupper(weighted_char) != 'N');

    bool directed = (toupper(directed_char) == 'S');
    bool weighted = (toupper(weighted_char) == 'S');

    cout << "\n--- Configuracao definida ---\n";
    cout << "Direcionado: " << (directed ? "SIM" : "NAO") << endl;
    cout << "Ponderado: " << (weighted ? "SIM" : "NAO") << endl;
    cout << "----------------------------\n";

    return {directed, weighted};
}

void initial_menu(Graph &graph)
{
    char option;

    do
    {
        cout << "\n=== MENU INICIAL ===\n";
        cout << "0 - Criar grafo vazio\n";
        cout << "1 - Carregar grafo de arquivo\n";
        cout << "X - Sair\n";
        cout << "Escolha: ";
        cin >> option;

        option = toupper(option);

        if (option == '0')
        {
            cout << "\nGrafo criado com configuracao inicial.\n";
            break;
        }
        else if (option == '1')
        {
            string filename;
            cout << "Digite o nome do arquivo (na pasta graphFiles/): ";
            cin.ignore();
            getline(cin, filename);

            // Remover possíveis espaços e garantir que o arquivo está em graphFiles
            // Por enquanto, apenas imprimimos que carregaríamos o arquivo
            cout << "Carregando grafo do arquivo graphFiles/" << filename << "...\n";
            graph.readFromFile(filename);
            cout << "Grafo carregado com sucesso!\n";
            cout << "Visao do grafo atualizado com os dados do arquivo:\n";
            graph.print();
            break;
        }
        else if (option == 'X')
        {
            cout << "Saindo do programa...\n";
            exit(0);
        }
        else
        {
            cout << "Opcao invalida! Tente novamente.\n";
        }
    } while (true);
}

void main_menu(Graph &graph)
{
    char option;

    do
    {
        cout << "\n=== MENU PRINCIPAL ===\n";
        cout << "0 - Mostrar grafo\n";
        cout << "1 - Adicionar vertice\n";
        cout << "2 - Remover vertice\n";
        cout << "3 - Adicionar aresta\n";
        cout << "4 - Remover aresta\n";
        cout << "5 - Verificar existencia de aresta\n";
        cout << "6 - Alterar peso de aresta (apenas grafos ponderados)\n";
        cout << "7 - Calcular grau de um vertice\n";
        cout << "8 - Listar vizinhos de um vertice\n";
        cout << "9 - Verificar se dois vertices sao adjacentes\n";
        cout << "C - Componentes Conexas\n";
        cout << "X - Sair\n";
        cout << "Escolha: ";
        cin >> option;

        option = toupper(option);

        if (option == '0')
        {
            graph.print();
        }
        else if (option == '1')
        {
            int id;
            cout << "ID do novo vertice: ";
            cin >> id;

            if (graph.add_vertex(id) == -1)
            {
                cout << "Falha ao adicionar vertice (id ja existe).\n";
            }
            else
            {
                cout << "Vertice " << id << " adicionado com sucesso!\n";
            }
        }
        else if (option == '2')
        {
            int id;
            cout << "ID do vertice a remover: ";
            cin >> id;

            if (graph.remove_vertex(id))
            {
                cout << "Vertice " << id << " removido com sucesso!\n";
            }
            else
            {
                cout << "Falha ao remover vertice (vertice nao encontrado).\n";
            }
        }
        else if (option == '3')
        {
            int u, v, weight = 1;
            cout << "ID do vertice u: ";
            cin >> u;
            cout << "ID do vertice v: ";
            cin >> v;

            if (graph.is_directed() && graph.is_weighted())
            {
                cout << "Peso da aresta (padrao=1): ";
                cin >> weight;
            }

            if (graph.add_edge(u, v, weight))
            {
                cout << "Aresta adicionada com sucesso!\n";
            }
            else
            {
                cout << "Falha ao adicionar aresta (vertices inexistentes ou aresta ja existe).\n";
            }
        }
        else if (option == '4')
        {
            int u, v;
            cout << "ID do vertice u: ";
            cin >> u;
            cout << "ID do vertice v: ";
            cin >> v;

            if (graph.remove_edge(u, v))
            {
                cout << "Aresta removida com sucesso!\n";
            }
            else
            {
                cout << "Falha ao remover aresta (aresta nao existe).\n";
            }
        }
        else if (option == '5')
        {
            int u, v;
            cout << "ID do vertice u: ";
            cin >> u;
            cout << "ID do vertice v: ";
            cin >> v;

            if (graph.has_edge(u, v))
            {
                cout << "Existe aresta entre " << u << " e " << v << ".\n";
            }
            else
            {
                cout << "Nao existe aresta entre " << u << " e " << v << ".\n";
            }
        }
        else if (option == '6')
        {
            if (!graph.is_weighted())
            {
                cout << "Este grafo nao e ponderado. Operacao nao permitida.\n";
                continue;
            }

            int u, v, weight;
            cout << "ID do vertice u: ";
            cin >> u;
            cout << "ID do vertice v: ";
            cin >> v;
            cout << "Novo peso: ";
            cin >> weight;

            if (graph.set_edge_weight(u, v, weight))
            {
                cout << "Peso da aresta alterado com sucesso!\n";
            }
            else
            {
                cout << "Falha ao alterar peso (aresta nao existe).\n";
            }
        }
        else if (option == '7')
        {
            int id;
            cout << "ID do vertice: ";
            cin >> id;

            int deg = graph.degree(id);
            if (deg == -1)
            {
                cout << "Vertice nao encontrado.\n";
            }
            else
            {
                cout << "Grau do vertice " << id << ": " << deg << endl;
            }
        }
        else if (option == '8')
        {
            int id;
            cout << "ID do vertice: ";
            cin >> id;

            vector<int> neighbors = graph.get_neighbors(id);
            if (neighbors.empty())
            {
                cout << "O vertice " << id << " nao possui vizinhos.\n";
            }
            else
            {
                cout << "Vizinhos do vertice " << id << ": ";
                for (int n : neighbors)
                {
                    cout << n << " ";
                }
                cout << endl;
            }
        }
        else if (option == '9')
        {
            int u, v;
            cout << "ID do vertice u: ";
            cin >> u;
            cout << "ID do vertice v: ";
            cin >> v;

            if (graph.are_adjacent(u, v))
            {
                cout << u << " e " << v << " sao adjacentes.\n";
            }
            else
            {
                cout << u << " e " << v << " nao sao adjacentes.\n";
            }
        }
        else if (option == 'C' || option == 'c')
        {
            graph.print_connected_components();
        }
        else if (option == 'X')
        {
            cout << "Saindo do programa...\n";
            exit(0);
        }
        else
        {
            cout << "Opcao invalida! Tente novamente.\n";
        }
    } while (true);
}

int main(int argc, char *argv[])
{
    // Configuração inicial do grafo
    auto [directed, weighted] = configure_graph();

    // Cria o grafo com as configurações
    Graph graph(directed, weighted);

    initial_menu(graph);
    main_menu(graph);

    return 0;
}