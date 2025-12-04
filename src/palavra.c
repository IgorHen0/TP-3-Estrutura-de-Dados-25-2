#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/palavra.h"

NoPalavra* criar_no_palavra(char *palavra, int id) {
    NoPalavra *novo = (NoPalavra*) malloc(sizeof(NoPalavra));
    if (!novo) {
        fprintf(stderr, "Erro de alocacao no no da arvore.\n");
        exit(EXIT_FAILURE);
    }

    // Aloca e copia a string da palavra
    novo->palavra = strdup(palavra);
    if (!novo->palavra) {
        free(novo);
        return NULL;
    }

    // Cria o vetor de IDs e adiciona o primeiro ID
    novo->ids = vector_create(2);
    vector_push_back(novo->ids, id);

    novo->esq = NULL;
    novo->dir = NULL;

    return novo;
}

NoPalavra* inserir_palavra(NoPalavra *raiz, char *palavra, int id) {
    // Caso base: árvore vazia ou folha encontrada -> insere novo nó
    if (raiz == NULL) {
        return criar_no_palavra(palavra, id);
    }

    int cmp = strcmp(palavra, raiz->palavra);

    if (cmp == 0) {
        // Palavra encontrada: apenas adiciona o novo ID à lista existente
        vector_push_back(raiz->ids, id);
    } else if (cmp < 0) {
        // Palavra menor que a atual -> vai para esquerda
        raiz->esq = inserir_palavra(raiz->esq, palavra, id);
    } else {
        // Palavra maior que a atual -> vai para direita
        raiz->dir = inserir_palavra(raiz->dir, palavra, id);
    }

    return raiz;
}

NoPalavra* buscar_palavra(NoPalavra *raiz, char *palavra) {
    if (raiz == NULL) {
        return NULL; // Não encontrado
    }

    int cmp = strcmp(palavra, raiz->palavra);

    if (cmp == 0) {
        return raiz; // Encontrado
    } else if (cmp < 0) {
        return buscar_palavra(raiz->esq, palavra); // Busca na esquerda
    } else {
        return buscar_palavra(raiz->dir, palavra); // Busca na direita
    }
}

void liberar_arvore(NoPalavra *raiz) {
    if (raiz != NULL) {
        liberar_arvore(raiz->esq);
        liberar_arvore(raiz->dir);
        
        // Libera os recursos do nó atual
        free(raiz->palavra);
        vector_free(raiz->ids); // Função do utils.h
        free(raiz);
    }
}

void imprimir_arvore(NoPalavra *raiz) {
    if (raiz != NULL) {
        imprimir_arvore(raiz->esq);
        
        printf("Palavra: %s | IDs: ", raiz->palavra);
        for (size_t i = 0; i < raiz->ids->size; i++) {
            printf("%d ", raiz->ids->data[i]);
        }
        printf("\n");

        imprimir_arvore(raiz->dir);
    }
}