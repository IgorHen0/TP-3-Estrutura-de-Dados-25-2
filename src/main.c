#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "../include/logradouro.h"
#include "../include/palavra.h"
#include "../include/utils.h"
#include "../include/consulta.h"

void criar_indice_palavras(CadastroLogradouros *cad, NoPalavra **raiz) {
    for (size_t i = 0; i < cad->tamanho; i++) {
        Logradouro *l = cad->vetor[i];
        char *copiaNome = strdup(l->nome);
        if (!copiaNome) continue;

        int qtdPalavras = 0;
        char **tokens = split(copiaNome, " ", &qtdPalavras);

        for (int j = 0; j < qtdPalavras; j++) {
            *raiz = inserir_palavra(*raiz, tokens[j], l->idLog);
        }

        free_split(tokens, qtdPalavras);
        free(copiaNome);
    }
}

int main() {
    
    // MEDIR TEMPO DE CARGA (Leitura + Hash + Centróides + Indexação)
    clock_t inicio_carga = clock();
    
    CadastroLogradouros *cad = criar_cadastro(10000); 
    if (!ler_arquivo_entrada(cad, stdin)) return 1;
    calcular_centroides(cad);
    NoPalavra *raiz = NULL;
    criar_indice_palavras(cad, &raiz); // Indexação
    
    clock_t fim_carga = clock();
    double tempo_carga = (double)(fim_carga - inicio_carga) / CLOCKS_PER_SEC;

    // MEDIR TEMPO DE CONSULTA
    clock_t inicio_consulta = clock();
    
    processar_consultas(cad, raiz, stdin);
    
    clock_t fim_consulta = clock();
    double tempo_consulta = (double)(fim_consulta - inicio_consulta) / CLOCKS_PER_SEC;

    // Imprimir para stderr para não sujar a saída padrão do VPL
    fprintf(stderr, "Tempo Carga: %f s\n", tempo_carga);
    fprintf(stderr, "Tempo Consulta: %f s\n", tempo_consulta);

    // Limpeza
    liberar_arvore(raiz);
    destruir_cadastro(cad);

    return 0;
}