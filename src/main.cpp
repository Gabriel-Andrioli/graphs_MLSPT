#include <iostream>
#include <limits>
#include "graph.hpp"
#include "random_manager.hpp"
#include "solver.hpp"

using namespace std;

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
            cout << "Digite o caminho do arquivo a partir da pasta data/ (ex: scenario_.../instance_x.txt): ";
            cin.ignore();
            getline(cin, filename);

            cout << "Carregando grafo do arquivo data/" << filename << "...\n";
            if (graph.readFromFile(filename))
            {
                cout << "Grafo carregado com sucesso!\n";
                cout << "Visao do grafo atualizado com os dados do arquivo:\n";
                graph.print();
            }
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
        cout << "6 - Consultar rotulo de aresta\n";
        cout << "7 - Calcular grau de um vertice\n";
        cout << "8 - Listar vizinhos de um vertice\n";
        cout << "9 - Verificar se dois vertices sao adjacentes\n";
        cout << "V - Obter quantidade de vertices\n";
        cout << "F - Obter frequencias globais de rotulos\n";
        cout << "M - Verificar se um vertice e mono-rotulo (mono-label)\n";
        cout << "P - Executar Algoritmo Guloso (Fases 1 a 4)\n";
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
            int u, v, label = 0;
            cout << "ID do vertice u: ";
            cin >> u;
            cout << "ID do vertice v: ";
            cin >> v;
            cout << "Rotulo da aresta (padrao=0): ";
            cin >> label;

            if (graph.add_edge(u, v, label))
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
            int u, v;
            cout << "ID do vertice u: ";
            cin >> u;
            cout << "ID do vertice v: ";
            cin >> v;

            int label = graph.get_edge_label(u, v);
            if (label == -1)
            {
                cout << "Aresta nao existe.\n";
            }
            else
            {
                cout << "O rotulo da aresta entre " << u << " e " << v << " e: " << label << endl;
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
        else if (option == 'V' || option == 'v')
        {
            cout << "Quantidade total de vertices: " << graph.get_vertices_count() << endl;
        }
        else if (option == 'F' || option == 'f')
        {
            cout << "Frequencias globais de rotulos:\n";
            const auto& freqs = graph.get_label_frequencies();
            if (freqs.empty())
            {
                cout << "(nenhum rotulo registrado)\n";
            }
            else
            {
                for (const auto& pair : freqs)
                {
                    cout << "  Rotulo " << pair.first << ": " << pair.second << " ocorrencia(s)\n";
                }
            }
        }
        else if (option == 'M' || option == 'm')
        {
            int id;
            cout << "ID do vertice: ";
            cin >> id;

            if (graph.is_vertex_mono_label(id))
            {
                cout << "O vertice " << id << " e mono-rotulo (mono-label)!\n";
            }
            else
            {
                cout << "O vertice " << id << " NAO e mono-rotulo.\n";
            }
        }
        else if (option == 'P' || option == 'p')
        {
            cout << "\n=== EXECUCAO DO SOLVER - ALGORITMO GULOSO (FASES 1 A 4) ===\n";
            MLSPTSolver solver(graph);
            solver.solve(); // Executa pré-processamento, inicialização, expansão e finalização
            
            cout << "\n--- FASE 1: ELEMENTOS INCONTORNAVEIS ---\n";
            cout << "Arestas Incontornaveis encontradas:\n";
            const auto& edges = solver.get_unavoidable_edges();
            if (edges.empty())
            {
                cout << "  (nenhuma aresta incontornavel)\n";
            }
            else
            {
                for (const auto& edge : edges)
                {
                    cout << "  Aresta: " << edge.first << " - " << edge.second 
                         << " (rotulo: " << graph.get_edge_label(edge.first, edge.second) << ")\n";
                }
            }

            cout << "Rotulos Incontornaveis encontrados:\n";
            const auto& labels = solver.get_unavoidable_labels();
            if (labels.empty())
            {
                cout << "  (nenhum rotulo incontornavel)\n";
            }
            else
            {
                for (int label : labels)
                {
                    cout << "  Rotulo: " << label << "\n";
                }
            }

            cout << "\n--- FASES 2, 3 e 4: RESULTADO FINAL DA SOLUCAO ---\n";
            cout << "Arestas na Arvore Geradora final (S):\n";
            const auto& sel_edges = solver.get_selected_edges();
            if (sel_edges.empty())
            {
                cout << "  (solucao vazia)\n";
            }
            else
            {
                for (const auto& edge : sel_edges)
                {
                    cout << "  Aresta: " << edge.first << " - " << edge.second 
                         << " (rotulo: " << graph.get_edge_label(edge.first, edge.second) << ")\n";
                }
            }

            cout << "Vertices em Visitados (" << solver.get_visited_vertices().size() << "):\n  ";
            const auto& visited = solver.get_visited_vertices();
            for (int v : visited)
            {
                cout << v << " ";
            }
            cout << "\n";

            cout << "Custo da Solucao (tamanho de Usados): " << solver.get_used_labels().size() << "\n";
            cout << "Rotulos em Usados:\n  ";
            const auto& used = solver.get_used_labels();
            for (int l : used)
            {
                cout << l << " ";
            }
            cout << "\n";

            cout << "Fila de Prioridades de Rotulos (Ordenada):\n  ";
            const auto& pq = solver.get_label_priority_queue();
            for (int l : pq)
            {
                cout << l << " ";
            }
            cout << "\n============================================\n";
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
    // Inicializa a semente global uma única vez
    RandomManager::initialize(argc, argv);

    // Cria o grafo simplificado para o MLSTP
    Graph graph;

    initial_menu(graph);
    main_menu(graph);

    return 0;
}