# Como compilar e executar o projeto com CMake?

## Passo 1 - Instalar o CMake
### Linux (Ubuntu/Debian)
```bash
sudo apt update
sudo apt install cmake build-essential
```

### Windows
1. Baixe e instale o CMake: https://cmake.org/download/
   - Marque a opção "Add CMake to system PATH"
2. Instale o Visual Studio Community ou algum compilador de C++

## Passo 2 - Compilar

### Linux (ou DevContainer)
```bash
mkdir build # Se for a primeira vez compilando
cd build
cmake ..
make
```

### Windows (PowerShell)
```powershell
mkdir build # Se for a primeira vez compilando
cd build
cmake ..
cmake --build .
```

### Windows (se você quiser compilar usando gcc ou g++ sem ter nada do Visual Studio (não é o vscode))
```powershell
mkdir build # Se for a primeira vez compilando
cd build
cmake -G "MinGW Makefiles" -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc ..
cmake --build .
```

## Passo 3 - Executar
Procure o executável `GraphsConnectedComponents` na pasta `build` e execute-o. 

### Linux (ou DevContainer)
```bash
./build/GraphsConnectedComponents
```

### Windows
```cmd
.\build\GraphsConnectedComponents.exe
```

## Estrutura do Projeto
```
meu-projeto/
├── src/           # Código fonte
├── include/       # Headers
├── graphFiles/    # Arquivos de grafos
├── build/         # Arquivos de build
└── CMakeLists.txt # Configuração CMake
```
