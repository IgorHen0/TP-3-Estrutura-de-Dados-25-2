#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
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

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Erro: Informe o arquivo de entrada.\n");
        return 1;
    }

    //Leitura e Agregação
    CadastroLogradouros *cad = criar_cadastro(10000); 
    
    if (!ler_arquivo_entrada(cad, argv[1])) {
        fprintf(stderr, "Falha ao ler arquivo.\n");
        return 1;
    }
    calcular_centroides(cad);

    // Indexação
    NoPalavra *raiz = NULL;
    criar_indice_palavras(cad, &raiz);

    // Processamento de Consultas
    processar_consultas(cad, raiz, argv[1]);

    // Limpeza
    liberar_arvore(raiz);
    destruir_cadastro(cad);

    return 0;
}