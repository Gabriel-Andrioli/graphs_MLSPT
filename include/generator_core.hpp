#ifndef GENERATOR_CORE_HPP
#define GENERATOR_CORE_HPP

#include <string>
#include <vector>

struct ParameterSet {
    std::vector<int> values_n;
    std::vector<double> multipliers_l;
    std::vector<double> densities_d;
};

// Declaração do conjunto de parâmetros
extern const std::vector<ParameterSet> PARAMETER_SETS;

struct GeneratedEdge {
    int u;
    int v;
    int label;
};

// Encontra dinamicamente a raiz do projeto procurando por CMakeLists.txt ou .gitignore
std::string get_project_root();

// Gera um grafo conexo rotulado e salva no caminho especificado
void generate_instance(int n, int l, int m, const std::string &filepath);

// Roda a geracao completa baseada nos PARAMETER_SETS na pasta especificada
int run_full_generation(const std::string &output_dir);

#endif
