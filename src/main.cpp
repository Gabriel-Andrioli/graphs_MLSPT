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

void test_all()
{
    cout << "\n============================================\n";
    cout << "      TESTE DE TODAS AS FUNCIONALIDADES\n";
    cout << "============================================\n\n";

    Graph graph(true, true);

    cout << "\n--- Teste: adicionar vertices ---\n";
    cout << "Adicionando vertices 1, 2, 3, 4, 5, 6, 7\n";
    graph.add_vertex(1);
    graph.add_vertex(2);
    graph.add_vertex(3);
    graph.add_vertex(4);
    graph.add_vertex(5);
    graph.add_vertex(6);
    graph.add_vertex(7);
    cout << "Tentativa de adicionar vertice 1 novamente: ";
    if (graph.add_vertex(1) == -1)
        cout << "FALHA (correto - id ja existe)\n\n";
    else
        cout << "SUCESSO (erro - id ja existe)\n\n";

    cout << "\n--- Teste: adicionar arestas ---\n";
    cout << "Adicionando arestas: 1->2 (peso 10), 1->3 (peso 20), 2->4 (peso 30), 3->4 (peso 40), 4->5 (peso 50), 6->7 (peso 60)\n";
    graph.add_edge(1, 2, 10);
    graph.add_edge(1, 3, 20);
    graph.add_edge(2, 4, 30);
    graph.add_edge(3, 4, 40);
    graph.add_edge(4, 5, 50);
    graph.add_edge(6, 7, 60);
    cout << "Tentativa de adicionar aresta 1->2 novamente (deve falhar): ";
    if (!graph.add_edge(1, 2))
        cout << "FALHA (correto - aresta ja existe)\n\n";
    else
        cout << "SUCESSO (erro - aresta ja existe)\n\n";

    cout << "Tentativa de adicionar aresta com vertice inexistente 1->99 (deve falhar): ";
    if (!graph.add_edge(1, 99))
        cout << "FALHA (correto - vertice nao existe)\n\n";
    else
        cout << "SUCESSO (erro - vertice nao existe)\n\n";

    cout << "\n--- Teste: exibir grafo ---\n";
    graph.print();

    cout << "\n--- Teste: verificar existencia de aresta ---\n";
    cout << "Existe aresta 1->2? " << (graph.has_edge(1, 2) ? "SIM" : "NAO") << " (Resposta esperada: SIM)\n";
    cout << "Existe aresta 2->1? " << (graph.has_edge(2, 1) ? "SIM" : "NAO") << " (Resposta esperada: NAO)\n";
    cout << "Existe aresta 1->99? " << (graph.has_edge(1, 99) ? "SIM" : "NAO") << " (Resposta esperada: NAO)\n";

    cout << "\n--- Teste: alterar peso de aresta ---\n";
    cout << "Alterando peso da aresta 1->2 para 99\n";
    graph.set_edge_weight(1, 2, 99);
    cout << "Verificando se tem aresta apos a alteracao: " << (graph.has_edge(1, 2) ? "SIM" : "NAO") << endl;
    cout << "Tentativa de alterar peso de aresta inexistente 2->1 (deve falhar): ";
    if (!graph.set_edge_weight(2, 1, 50))
        cout << "FALHA (correto - aresta nao existe)\n\n";
    else
        cout << "SUCESSO (erro - aresta nao existe)\n\n";

    cout << "\n--- Teste: calcular grau de um vertice ---\n";
    cout << "Grau do vertice 1: " << graph.degree(1) << " (esperado: 2 - arestas 1->2 e 1->3)\n";
    cout << "Grau do vertice 4: " << graph.degree(4) << " (esperado: 3 - arestas 2->4, 4->5 e 3->4)\n";
    cout << "Grau do vertice 5: " << graph.degree(5) << " (esperado: 1 - aresta 4->5)\n";
    cout << "Grau do vertice 99 (inexistente): " << graph.degree(99) << " (deve retornar -1)\n";

    cout << "\n--- Teste: obter vizinhos de um vertice ---\n";
    vector<int> vizinhos1 = graph.get_neighbors(1);
    cout << "Vizinhos do vertice 1: ";
    for (int v : vizinhos1)
        cout << v << " ";
    cout << "(esperado: 2 3)\n";

    vector<int> vizinhos5 = graph.get_neighbors(5);
    cout << "Vizinhos do vertice 5: ";
    for (int v : vizinhos5)
        cout << v << " ";
    cout << "(esperado: lista vazia)\n";

    cout << "\n--- Teste: verificar adjacencia ---\n";
    cout << "1 e 2 sao adjacentes? " << (graph.are_adjacent(1, 2) ? "SIM" : "NAO") << " (esperado: SIM - aresta 1->2 existe)\n";
    cout << "2 e 1 sao adjacentes? " << (graph.are_adjacent(2, 1) ? "SIM" : "NAO") << " (esperado: NAO - aresta 2->1 nao existe)\n";
    cout << "4 e 5 sao adjacentes? " << (graph.are_adjacent(4, 5) ? "SIM" : "NAO") << " (esperado: SIM - aresta 4->5 existe)\n";

    cout << "\n--- Teste: componentes conexas ---\n";
    cout << "Componentes conexas do grafo atual:\n";
    graph.print_connected_components();

    cout << "\n--- Teste: remover aresta ---\n";
    cout << "Removendo aresta 1->3\n";
    graph.remove_edge(1, 3);
    cout << "Existe aresta 1->3 apos remocao? " << (graph.has_edge(1, 3) ? "SIM (erro!)" : "NAO (correto)") << endl;
    cout << "Tentativa de remover aresta inexistente 1->3 novamente (deve falhar): ";
    if (!graph.remove_edge(1, 3))
        cout << "FALHA (correto - aresta ja foi removida)\n\n";
    else
        cout << "SUCESSO (erro - aresta ja foi removida)\n\n";

    cout << "\n--- Grafo apos remocao da aresta 1->3 ---\n";
    graph.print();

    cout << "\n--- Teste: remover vertice ---\n";
    cout << "Removendo vertice 5\n";
    graph.remove_vertex(5);
    cout << "Existe aresta 4->5 apos remocao do vertice 5? " << (graph.has_edge(4, 5) ? "SIM (erro!)" : "NAO (correto)") << endl;
    cout << "Grau do vertice 4 apos remocao: " << graph.degree(4) << " (esperado: 2 - so restam 2->4 e 3->4)\n";
    cout << "Tentativa de remover vertice 99 (inexistente): ";
    if (!graph.remove_vertex(99))
        cout << "FALHA (correto - vertice nao existe)\n\n";
    else
        cout << "SUCESSO (erro - vertice nao existe)\n\n";

    cout << "\n--- Grafo final ---\n";
    graph.print();

    cout << "\n--- Componentes conexas finais ---\n";
    graph.print_connected_components();

    cout << "\n============================================\n";
    cout << "               TESTE CONCLUIDO!             \n";
    cout << "============================================\n";
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
        cout << "T - Testar todas as funcionalidades (test_all)\n";
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
        else if (option == 'T')
        {
            test_all();
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