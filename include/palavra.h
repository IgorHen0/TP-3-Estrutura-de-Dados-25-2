#ifndef PALAVRA_H
#define PALAVRA_H

#include "utils.h"

// Estrutura do Nó da Árvore Binária de Busca
typedef struct NoPalavra {
    char *palavra;
    IntVector *ids;
    struct NoPalavra *esq;
    struct NoPalavra *dir;
} NoPalavra;

// Funções do TAD Palavra
NoPalavra* criar_no_palavra(char *palavra, int id);
NoPalavra* inserir_palavra(NoPalavra *raiz, char *palavra, int id);
NoPalavra* buscar_palavra(NoPalavra *raiz, char *palavra);
void liberar_arvore(NoPalavra *raiz);
void imprimir_arvore(NoPalavra *raiz);

#endif