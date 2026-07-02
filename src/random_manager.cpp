#include "random_manager.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;

int RandomManager::seed = 0;
bool RandomManager::initialized = false;

void RandomManager::initialize(int argc, char *argv[])
{
    if (initialized) return;

    if (argc > 1)
    {
        // Se houver argumento, tenta converter para inteiro
        int custom = atoi(argv[1]);
        if (custom != 0 || argv[1][0] == '0')
        {
            initialize(custom);
            return;
        }
    }

    // Inicializa com o tempo do sistema usando ctime
    initialize(static_cast<int>(time(NULL)));
}

void RandomManager::initialize(int custom_seed)
{
    if (initialized) return;

    seed = custom_seed;
    srand(static_cast<unsigned int>(seed));
    initialized = true;

    cout << "=========================================\n";
    cout << "Semente de Randomizacao: " << seed << "\n";
    cout << "=========================================\n\n";
}

int RandomManager::get_seed()
{
    return seed;
}

int RandomManager::next_int(int min, int max)
{
    if (max < min) return min;
    return min + rand() % (max - min + 1);
}
