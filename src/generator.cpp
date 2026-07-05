#include <iostream>
#include "generator_core.hpp"
#include "random_manager.hpp"

using namespace std;

// No executável principal, deixamos a const root com o comportamento normal, 
// ou chamamos get_project_root() / "data".
// O original escrevia em "data", então vamos resolver com get_project_root() 
// para garantir que ele crie na raiz, assim como fazemos na varredura.

int main(int argc, char *argv[])
{
    // Inicializa a semente global a partir dos argumentos ou hora do sistema
    RandomManager::initialize(argc, argv);
    
    cout << "Iniciando a geracao de instancias do MLSTP...\n";
    
    string output_dir = get_project_root() + "/data";
    
    int total_files = run_full_generation(output_dir);
    
    if (total_files > 0) {
        cout << "Geracao concluida! Total de " << total_files << " arquivos de instancias gerados na pasta 'data'.\n";
    } else {
        cout << "Falha na geracao das instancias.\n";
    }
    
    return 0;
}
