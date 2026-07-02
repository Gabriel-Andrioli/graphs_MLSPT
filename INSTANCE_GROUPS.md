# Grupos de Instâncias para o MLSTP

Este arquivo define os grupos de parâmetros utilizados para a geração de instâncias de teste do Problema da Árvore Geradora com Número Mínimo de Rótulos (MLSTP).

Os cenários são caracterizados pelas seguintes variáveis:
- **$n$**: Número de vértices no grafo.
- **$l$**: Número de rótulos (labels) disponíveis ($l = \text{multiplicador} \times n$).
- **$d$**: Densidade do grafo, usada para determinar o número total de arestas $m = \lfloor \frac{d \cdot n(n-1)}{2} \rfloor$.

Para evitar redundância de dados nas análises e manter a unicidade das instâncias geradas, os casos em que os cenários coincidem entre os grupos foram unificados (por exemplo, removendo a duplicação de $n=50, l=50$ do Grupo 2 de Cerulli, já que já está contido no Grupo 1 de Cerulli).

---

## 1. Grupo 1 de Cerulli (2005)
Instâncias pequenas onde o número de rótulos é exatamente igual ao número de vértices.

- **Vértices ($n$):** 20, 30, 40, 50
- **Rótulos ($l$):** $l = n$ (multiplicador = 1.0)
- **Densidades ($d$):** 0.2, 0.5, 0.8
- **Quantidade de instâncias por cenário:** 1
- **Total de cenários:** 12

| $n$ | Multiplicador de $l$ | $l$ | Densidades ($d$) | Arestas ($m$) |
|---|---|---|---|---|
| 20 | 1.0 | 20 | 0.2, 0.5, 0.8 | 38, 95, 152 |
| 30 | 1.0 | 30 | 0.2, 0.5, 0.8 | 87, 217, 348 |
| 40 | 1.0 | 40 | 0.2, 0.5, 0.8 | 156, 390, 624 |
| 50 | 1.0 | 50 | 0.2, 0.5, 0.8 | 245, 612, 980 |

---

## 2. Grupo 2 de Cerulli (2005)
Instâncias de médio porte com variações na proporção do número de rótulos em relação aos vértices. 

> [!NOTE]
> O cenário com $n=50$ e multiplicador $1.0$ ($l=50$) foi omitido deste grupo para evitar colisão com o mesmo cenário já coberto pelo Grupo 1 de Cerulli.

- **Vértices ($n$):** 50, 100
- **Rótulos ($l$):** $l \in \{0.25n, 0.5n, n, 1.25n\}$
- **Densidades ($d$):** 0.2, 0.5, 0.8
- **Quantidade de instâncias por cenário:** 1
- **Total de cenários:** 21 (9 para $n=50$, 12 para $n=100$)

| $n$ | Multiplicador de $l$ | $l$ | Densidades ($d$) | Arestas ($m$) |
|---|---|---|---|---|
| 50 | 0.25, 0.5, 1.25 | 12, 25, 62 | 0.2, 0.5, 0.8 | 245, 612, 980 |
| 100 | 0.25, 0.5, 1.0, 1.25 | 25, 50, 100, 125 | 0.2, 0.5, 0.8 | 990, 2475, 3960 |

---

## 3. Grupo de Consoli (2015)
Instâncias de grande porte com maior granularidade de rótulos.

- **Vértices ($n$):** 500, 1000
- **Rótulos ($l$):** $l \in \{0.25n, 0.5n, 0.75n, n, 1.25n\}$
- **Densidades ($d$):** 0.2, 0.5, 0.8
- **Quantidade de instâncias por cenário:** 1
- **Total de cenários:** 30

| $n$ | Multiplicador de $l$ | $l$ | Densidades ($d$) | Arestas ($m$) |
|---|---|---|---|---|
| 500 | 0.25, 0.5, 0.75, 1.0, 1.25 | 125, 250, 375, 500, 625 | 0.2, 0.5, 0.8 | 24950, 62375, 99800 |
| 1000 | 0.25, 0.5, 0.75, 1.0, 1.25 | 250, 500, 750, 1000, 1250 | 0.2, 0.5, 0.8 | 99900, 249750, 399600 |
