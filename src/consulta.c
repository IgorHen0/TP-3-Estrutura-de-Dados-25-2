#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../include/consulta.h"
#include "../include/utils.h"

// Estrutura auxiliar para ordenação dos resultados
typedef struct {
    Logradouro *log;
    double distancia;
} ResultadoBusca;

// Função auxiliar para calcular distância Euclidiana
double dist_euclidiana(double x1, double y1, double x2, double y2) {
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

// Função auxiliar para interseção de dois vetores ordenados
IntVector* interseccao_vetores(IntVector *v1, IntVector *v2) {
    // Garante que ambos estejam ordenados para usar algoritmo O(N)
    vector_sort(v1);
    vector_sort(v2);

    // Cria vetor para resultado (tamanho máximo é o menor dos dois)
    size_t cap = v1->size < v2->size ? v1->size : v2->size;
    IntVector *res = vector_create(cap > 0 ? cap : 1);

    size_t i = 0, j = 0;
    while (i < v1->size && j < v2->size) {
        if (v1->data[i] < v2->data[j]) {
            i++;
        } else if (v1->data[i] > v2->data[j]) {
            j++;
        } else {
            // Iguais: adiciona ao resultado
            vector_push_back(res, v1->data[i]);
            i++;
            j++;
        }
    }
    return res;
}

// Comparador para o QuickSort dos resultados (menor distância primeiro)
int compare_resultados(const void *a, const void *b) {
    ResultadoBusca *r1 = (ResultadoBusca*)a;
    ResultadoBusca *r2 = (ResultadoBusca*)b;
    if (r1->distancia < r2->distancia) return -1;
    if (r1->distancia > r2->distancia) return 1;
    return 0; // Empate
}

// Adaptando o QuickSort do utils para struct ResultadoBusca
void quick_sort_resultados(ResultadoBusca *arr, int low, int high) {
    if (low < high) {
        double pivot = arr[high].distancia;
        int i = (low - 1);
        for (int j = low; j <= high - 1; j++) {
            if (arr[j].distancia < pivot) {
                i++;
                ResultadoBusca temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
            }
        }
        ResultadoBusca temp = arr[i + 1];
        arr[i + 1] = arr[high];
        arr[high] = temp;
        int pi = i + 1;

        quick_sort_resultados(arr, low, pi - 1);
        quick_sort_resultados(arr, pi + 1, high);
    }
}

void processar_consultas(CadastroLogradouros *cad, NoPalavra *raiz, FILE *entrada) {
    if (!entrada) return;

    char linha[1024];

    // O arquivo já deve estar posicionado na linha "M R" (após ler os endereços)
    if (!fgets(linha, sizeof(linha), entrada)) return;
    
    int numConsultas = 0, maxResultados = 0;
    char *l = trim(linha);
    int countTokens = 0;
    char **tokensHeader = split(l, " ", &countTokens);
    if (countTokens >= 2) {
        numConsultas = atoi(tokensHeader[0]);
        maxResultados = atoi(tokensHeader[1]);
    }
    free_split(tokensHeader, countTokens);

    printf("%d\n", numConsultas);

    // Loop de consultas
    for (int k = 0; k < numConsultas; k++) {
        if (!fgets(linha, sizeof(linha), entrada)) break;

        char *limpa = trim(linha);
        int qtdParts = 0;
        char **parts = split(limpa, ";", &qtdParts);

        if (qtdParts >= 4) {
            char *idConsulta = parts[0];
            char *termosBusca = parts[1];
            double latOrigem = atof(parts[2]);
            double longOrigem = atof(parts[3]);

            int qtdTermos = 0;
            char **palavras = split(termosBusca, " ", &qtdTermos);
            IntVector *resultadoIDs = NULL;

            for (int w = 0; w < qtdTermos; w++) {
                NoPalavra *no = buscar_palavra(raiz, palavras[w]);
                if (no == NULL) {
                    if (resultadoIDs) vector_free(resultadoIDs);
                    resultadoIDs = vector_create(0);
                    break;
                }
                if (w == 0) {
                    resultadoIDs = vector_create(no->ids->size);
                    for(size_t x=0; x < no->ids->size; x++) 
                        vector_push_back(resultadoIDs, no->ids->data[x]);
                } else {
                    IntVector *temp = interseccao_vetores(resultadoIDs, no->ids);
                    vector_free(resultadoIDs);
                    resultadoIDs = temp;
                }
                if (resultadoIDs->size == 0) break;
            }

            ResultadoBusca *listaFinal = (ResultadoBusca*) malloc(resultadoIDs->size * sizeof(ResultadoBusca));
            int countFinal = 0;

            for (size_t i = 0; i < resultadoIDs->size; i++) {
                int idBusca = resultadoIDs->data[i];
                // Reimplementando busca linear rápida na hash
                Logradouro *encontrado = NULL;
                size_t idxHash = (unsigned long)idBusca % cad->tamHash;
                size_t idxOriginal = idxHash;
                while (cad->tabelaHash[idxHash] != -1) {
                    Logradouro *cand = cad->vetor[cad->tabelaHash[idxHash]];
                    if (cand->idLog == idBusca) {
                        encontrado = cand;
                        break;
                    }
                    idxHash = (idxHash + 1) % cad->tamHash;
                    if (idxHash == idxOriginal) break;
                }

                if (encontrado) {
                    listaFinal[countFinal].log = encontrado;
                    listaFinal[countFinal].distancia = dist_euclidiana(latOrigem, longOrigem, encontrado->latMedia, encontrado->longMedia);
                    countFinal++;
                }
            }

            if (countFinal > 1) {
                quick_sort_resultados(listaFinal, 0, countFinal - 1);
            }

            int numParaImprimir = countFinal < maxResultados ? countFinal : maxResultados;
            printf("%s;%d\n", idConsulta, numParaImprimir);

            for (int i = 0; i < numParaImprimir; i++) {
                printf("%d;%s\n", listaFinal[i].log->idLog, listaFinal[i].log->nome);
            }

            free(listaFinal);
            if(resultadoIDs) vector_free(resultadoIDs);
            free_split(palavras, qtdTermos);
        }
        free_split(parts, qtdParts);
    }
}