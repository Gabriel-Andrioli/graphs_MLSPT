# Primeira Ideia de Algoritmo Heurístico para o MLSTP

## 1. Resumo da Abordagem

O algoritmo constrói uma árvore geradora de forma iterativa (semelhante/inspirada no Algoritmo de Prim). A escolha da próxima aresta a ser inserida na árvore é guiada por uma **Fila de Prioridade de Labels**, que é atualizada dinamicamente. A heurística prioriza maximizar o reuso de rótulos já presentes na solução parcial e, em caso de novos rótulos, prioriza aqueles que são mais frequentes (recorrentes) na topologia global do grafo.

## 2. Estruturas de Dados Utilizadas

* **`Visitados` (Set):** Conjunto de vértices que já foram conectados à árvore geradora.
* **`S` (Lista de Arestas):** A solução parcial, contendo as arestas que formam a árvore geradora.
* **`Usados`:** Estrutura de dados com os rótulos (labels) que já foram incluídos em alguma aresta da solução `S`.
* **`Mapa de Recorrência` (Dicionário/HashMap):** Armazena a contagem total de aparições de cada rótulo no grafo original.
* **`Fila de Prioridade` (Lista Ordenada):** Lista de todos os rótulos disponíveis ordenada por critérios de prioridade para ditar a próxima escolha.
* **`Candidatos`:** Estrutura de dados com as arestas que podem ser expandidas (caminhos que podemos tomar durante o algoritmo no estado vigente).

## 3. Regras de Prioridade para Rótulos

A Fila de Prioridade ordena os rótulos do mais atrativo (início da fila) para o menos atrativo (fim da fila), seguindo rigorosamente esta ordem de critérios:

1. **Critério Primário (Reuso):** Rótulos que já pertencem ao conjunto `Usados` têm prioridade máxima.
2. **Critério Secundário (Frequência):** Caso os rótulos comparados tenham o mesmo status no Critério 1, a prioridade é dada àquele com maior valor no `Mapa de Recorrência`.
3. **Critério Terciário (Desempate):** Ordem alfabética ou arbitrária consistente (provavelmente ordem de inserção ou o que facilite a implementação).

## 4. Passo a Passo do Algoritmo

### Fase 1: Pré-processamento

1. **Identificar Arestas Incontornáveis:**
* Varra os vértices do grafo em busca de vértices de grau de label 1, vértices mono-label.
* As arestas que conectam esses vértices são obrigatórias para qualquer árvore geradora válida.


2. **Mapear a Recorrência:**
* Varra todas as arestas do grafo e conte a frequência de cada rótulo, preenchendo o `Mapa de Recorrência`.


### Fase 2: Inicialização

1. **Inserir as Arestas Incontornáveis na Solução:**
* Adicione as arestas incontornáveis identificadas na Fase 1 ao conjunto `S`.
* Adicione os vértices dessas arestas ao conjunto `Visitados`.
* Adicione os rótulos dessas arestas ao conjunto `Usados`.
* *(Caso o grafo não possua vértices de grau 1, inicialize a árvore escolhendo uma aresta aleatória pertencente ao rótulo de maior recorrência. Se existerem vértices de grau 1, comece a árvore por algum vértice que se liga a um dos vértices de grau 1, ou seja, que participa de uma aresta incontornável, mas que não é o vértice isolado).*


2. **Montar a Fila de Prioridade Inicial:**
* Ordene todos os rótulos do grafo aplicando as **Regras de Prioridade** estabelecidas acima.


### Fase 3: Loop de Expansão (Construção Gulosa)

Repita os passos abaixo até que o tamanho do conjunto `Visitados` seja igual ao número total de vértices do grafo (todos foram alcançados):

1. **Mapear Fronteira (Definição de Opções):**
* Encontre todas as arestas candidatas. Uma aresta candidata é aquela que conecta um vértice que **está** em `Visitados` a um vértice que **não está** em `Visitados`.


2. **Seleção da Melhor Aresta:**
* Olhe para os rótulos de todas as arestas candidatas levantadas no passo anterior.
* Consulte a `Fila de Prioridade` e selecione a aresta cujo rótulo aparece **primeiro** (mais ao topo) na fila.


3. **Atualização de Estado:**
* Adicione a aresta escolhida a `S`.
* Adicione o novo vértice alcançado a `Visitados`.


4. **Atualização Dinâmica das Prioridades:**
* Verifique se o rótulo da aresta recém-adicionada é inédito (não estava em `Usados`).
* Se for inédito, adicione-o a `Usados`.
* **Reordene a `Fila de Prioridade**`: Como esse rótulo agora é um rótulo "Usado" (Critério 1), ele deve saltar para o topo da fila (junto com os outros já usados, ordenados entre si pela recorrência do Critério 2).



### Fase 4: Finalização

* O loop se encerra quando todos os vértices estiverem em `Visitados`.
* A solução do problema será o conjunto de arestas `S`, e o custo heurístico será o tamanho do conjunto `Usados`.

---