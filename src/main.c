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

int main() {
    // Inicializa Cadastro
    CadastroLogradouros *cad = criar_cadastro(10000); 
    
    // A função lê N endereços e para.
    if (!ler_arquivo_entrada(cad, stdin)) {
        return 1;
    }
    calcular_centroides(cad);

    // Indexação
    NoPalavra *raiz = NULL;
    criar_indice_palavras(cad, &raiz);

    // Processamento de Consultas via STDIN
    processar_consultas(cad, raiz, stdin);

    // Limpeza
    liberar_arvore(raiz);
    destruir_cadastro(cad);

    return 0;
}