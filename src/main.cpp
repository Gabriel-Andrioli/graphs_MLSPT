#include <iostream>
#include <limits>
#include <algorithm>
#include "graph.hpp"
#include "random_manager.hpp"
#include "solver.hpp"

using namespace std;

void heapify(vector<pair<int, int>>& arr, int n, int i)
{
    int extremum = i; 
    int left = 2 * i + 1; // filho esquerdo
    int right = 2 * i + 2; // filho direito

    // Usando uma funcao auxiliar (lambda) para comparar qual deve subir para a raiz.
    auto is_less = [](const pair<int, int>& a, const pair<int, int>& b) {
        if (a.second != b.second)
            return a.second < b.second;
        return a.first < b.first; 
    };

    if (left < n && is_less(arr[left], arr[extremum]))
        extremum = left;

    if (right < n && is_less(arr[right], arr[extremum]))
        extremum = right;

    if (extremum != i)
    {
        swap(arr[i], arr[extremum]);
        heapify(arr, n, extremum);
    }
}

void heapSort(vector<pair<int, int>>& arr)
{
    int n = arr.size();

    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(arr, n, i);

    for (int i = n - 1; i > 0; i--)
    {
        swap(arr[0], arr[i]);
        heapify(arr, i, 0);
    }
}
// --------------------------------------------------------

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
            cout << "Digite o nome do arquivo dentro da pasta data (ex: scenario_.../instance_x.txt): ";
            cin.ignore();
            getline(cin, filename);

            cout << "Carregando grafo do arquivo " << filename << "...\n";
            if (graph.readFromFile(filename))
            {
                cout << "Grafo carregado com sucesso!\n";
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
        cout << "1 - Carregar novo grafo (outro arquivo)\n";
        cout << "2 - Editar grafo\n";
        cout << "3 - Consultar rotulo de aresta\n";
        cout << "4 - Obter quantidade de vertices\n";
        cout << "5 - Obter frequencias globais de rotulos\n";
        cout << "6 - Executar Algoritmo Guloso (Fases 1 a 4)\n";
        cout << "7 - Executar Algoritmo Guloso Randomizado\n";
        cout << "8 - Executar Algoritmo Guloso Randomizado Reativo\n";
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
            string filename;
            cout << "Digite o nome do arquivo dentro da pasta data (ex: scenario_.../instance_x.txt): ";
            cin.ignore();
            getline(cin, filename);

            cout << "Limpando o grafo atual...\n";
            graph.clear();

            cout << "Carregando grafo do arquivo " << filename << "...\n";
            if (graph.readFromFile(filename))
            {
                cout << "Grafo carregado com sucesso!\n";
            }
        }
        else if (option == '2')
        {
            char edit_option;
            do
            {
                cout << "\n=== EDITAR GRAFO ===\n";
                cout << "1 - Adicionar vertice\n";
                cout << "2 - Remover vertice\n";
                cout << "3 - Adicionar aresta\n";
                cout << "4 - Remover aresta\n";
                cout << "B - Voltar ao menu principal\n";
                cout << "Escolha: ";
                cin >> edit_option;

                edit_option = toupper(edit_option);

                if (edit_option == '1')
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
                else if (edit_option == '2')
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
                else if (edit_option == '3')
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
                else if (edit_option == '4')
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
                else if (edit_option == 'B')
                {
                    break;
                }
                else
                {
                    cout << "Opcao invalida! Tente novamente.\n";
                }
            } while (true);
        }
        else if (option == '3')
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
        else if (option == '4')
        {
            cout << "Quantidade total de vertices: " << graph.get_vertices_count() << endl;
        }
        else if (option == '5')
        {
            cout << "Frequencias globais de rotulos (Ordenados):\n";
            const auto& freqs = graph.get_label_frequencies();
            if (freqs.empty())
            {
                cout << "(nenhum rotulo registrado)\n";
            }
            else
            {
                vector<pair<int, int>> sorted_freqs(freqs.begin(), freqs.end());
                heapSort(sorted_freqs);

                for (const auto& pair : sorted_freqs)
                {
                    cout << "  Rotulo " << pair.first << ": " << pair.second << " ocorrencia(s)\n";
                }
            }
        }
        else if (option == '6')
        {
            MLSPTSolver solver(graph);
            solver.solve();

            bool success = (solver.get_visited_vertices().size() == graph.get_vertices_count());

            cout << "\n=== RESULTADO DO ALGORITMO GULOSO ===\n";
            cout << "Status: " << (success ? "SUCESSO" : "FALHA") << "\n";
            cout << "Quantidade de rotulos usados: " << solver.get_used_labels().size() << "\n";
            
            cout << "Lista de rotulos usados: ";
            for (int label : solver.get_used_labels())
            {
                cout << label << " ";
            }
            cout << "\n";

            cout << "Fila final de prioridade: ";
            for (int label : solver.get_label_priority_queue())
            {
                cout << label << " ";
            }
            cout << "\n";

            if (success)
            {
                char show_graph;
                cout << "Quer ver o grafo solucao final (arvore)? (S/N): ";
                cin >> show_graph;
                show_graph = toupper(show_graph);

                if (show_graph == 'S')
                {
                    cout << "Solucao Final:\n";
                    Graph solution_graph;
                    vector<int> visited_sorted(solver.get_visited_vertices().begin(), solver.get_visited_vertices().end());
                    sort(visited_sorted.begin(), visited_sorted.end());
                    for (int v : visited_sorted)
                    {
                        solution_graph.add_vertex(v);
                    }
                    for (const auto& edge : solver.get_selected_edges())
                    {
                        solution_graph.add_edge(edge.first, edge.second, graph.get_edge_label(edge.first, edge.second));
                    }
                    solution_graph.print();
                }
            }
            else
            {
                cout << "Solucao Final:\n";
                cout << "(sem solucao - grafo desconexo)\n";
            }
            cout << "=====================================\n";
        }
        else if (option == '7')
        {
            double alpha;
            int iterations;
            cout << "Digite o valor de alfa (entre 0.0 e 1.0): ";
            cin >> alpha;
            cout << "Digite o numero de iteracoes (maior que 0): ";
            cin >> iterations;

            if (alpha < 0.0 || alpha > 1.0 || iterations <= 0)
            {
                cout << "[Erro] Parametros invalidos.\n";
            }
            else
            {
                MLSPTSolver solver(graph);
                solver.solve_randomized(alpha, iterations);

                bool success = (solver.get_visited_vertices().size() == graph.get_vertices_count());

                cout << "\n=== RESULTADO DO ALGORITMO GULOSO RANDOMIZADO ===\n";
                cout << "Status: " << (success ? "SUCESSO" : "FALHA") << "\n";
                cout << "Quantidade de rotulos usados: " << solver.get_used_labels().size() << "\n";
                
                cout << "Lista de rotulos usados: ";
                for (int label : solver.get_used_labels())
                {
                    cout << label << " ";
                }
                cout << "\n";

                cout << "Fila final de prioridade: ";
                for (int label : solver.get_label_priority_queue())
                {
                    cout << label << " ";
                }
                cout << "\n";

                if (success)
                {
                    char show_graph;
                    cout << "Quer ver o grafo solucao final (arvore)? (S/N): ";
                    cin >> show_graph;
                    show_graph = toupper(show_graph);

                    if (show_graph == 'S')
                    {
                        cout << "Solucao Final:\n";
                        Graph solution_graph;
                        vector<int> visited_sorted(solver.get_visited_vertices().begin(), solver.get_visited_vertices().end());
                        sort(visited_sorted.begin(), visited_sorted.end());
                        for (int v : visited_sorted)
                        {
                            solution_graph.add_vertex(v);
                        }
                        for (const auto& edge : solver.get_selected_edges())
                        {
                            solution_graph.add_edge(edge.first, edge.second, graph.get_edge_label(edge.first, edge.second));
                        }
                        solution_graph.print();
                    }
                }
                else
                {
                    cout << "Solucao Final:\n";
                    cout << "(sem solucao - grafo desconexo)\n";
                }
                cout << "=================================================\n";
            }
        }
        else if (option == '8')
        {
            int iterations, block_size;
            double delta;
            cout << "Digite o numero de iteracoes (maior que 0): ";
            cin >> iterations;
            cout << "Digite o tamanho do bloco (maior que 0)\n";
            cout << "(Nota: numero de iteracoes antes de reavaliar as probabilidades dos alfas): ";
            cin >> block_size;
            cout << "Digite o valor de delta para reatividade\n";
            cout << "(Nota: delta > 1 aumenta intensificacao, delta < 1 aumenta exploracao): ";
            cin >> delta;

            if (iterations <= 0 || block_size <= 0)
            {
                cout << "[Erro] Parametros invalidos.\n";
            }
            else
            {
                vector<double> alphas = {0.0, 0.2, 0.4, 0.6, 0.8, 1.0};
                MLSPTSolver solver(graph);
                solver.solve_reactive(iterations, block_size, delta, alphas);

                bool success = (solver.get_visited_vertices().size() == graph.get_vertices_count());

                cout << "\n=== RESULTADO DO ALGORITMO GULOSO RANDOMIZADO REATIVO ===\n";
                cout << "Status: " << (success ? "SUCESSO" : "FALHA") << "\n";
                if (success)
                {
                    cout << "Melhor alfa encontrado: " << solver.get_best_alpha() << "\n";
                }
                cout << "Quantidade de rotulos usados: " << solver.get_used_labels().size() << "\n";
                
                cout << "Lista de rotulos usados: ";
                for (int label : solver.get_used_labels())
                {
                    cout << label << " ";
                }
                cout << "\n";

                cout << "Fila final de prioridade: ";
                for (int label : solver.get_label_priority_queue())
                {
                    cout << label << " ";
                }
                cout << "\n";

                if (success)
                {
                    char show_graph;
                    cout << "Quer ver o grafo solucao final (arvore)? (S/N): ";
                    cin >> show_graph;
                    show_graph = toupper(show_graph);

                    if (show_graph == 'S')
                    {
                        cout << "Solucao Final:\n";
                        Graph solution_graph;
                        vector<int> visited_sorted(solver.get_visited_vertices().begin(), solver.get_visited_vertices().end());
                        sort(visited_sorted.begin(), visited_sorted.end());
                        for (int v : visited_sorted)
                        {
                            solution_graph.add_vertex(v);
                        }
                        for (const auto& edge : solver.get_selected_edges())
                        {
                            solution_graph.add_edge(edge.first, edge.second, graph.get_edge_label(edge.first, edge.second));
                        }
                        solution_graph.print();
                    }
                }
                else
                {
                    cout << "Solucao Final:\n";
                    cout << "(sem solucao - grafo desconexo)\n";
                }
                cout << "=================================================\n";
            }
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