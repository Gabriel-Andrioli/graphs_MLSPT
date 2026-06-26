#ifndef RANDOM_MANAGER_HPP
#define RANDOM_MANAGER_HPP

class RandomManager
{
private:
    static int seed;
    static bool initialized;

public:
    // Inicializa a semente a partir dos argumentos de linha de comando ou do tempo (ctime)
    static void initialize(int argc, char *argv[]);

    // Inicializa com uma semente fixa
    static void initialize(int custom_seed);

    // Retorna a semente utilizada
    static int get_seed();

    // Retorna um inteiro aleatório no intervalo [min, max] (inclusive) usando rand()
    static int next_int(int min, int max);
};

#endif // RANDOM_MANAGER_HPP
