#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/endereco.h"
#include "../include/utils.h"

Endereco* criar_endereco_de_linha(char *linha) {
    if (!linha || strlen(linha) == 0) return NULL;

    Endereco *e = (Endereco*) malloc(sizeof(Endereco));
    if (!e) return NULL;

    int count = 0;
    char **tokens = split(linha, ";", &count);

    if (count < 10) {
        free_split(tokens, count);
        free(e);
        return NULL;
    }

    e->idEnd = strdup(tokens[0]);
    e->idLog = atoi(tokens[1]);
    e->tipoLog = strdup(tokens[2]);
    e->nomeLog = strdup(tokens[3]);
    e->numero = atoi(tokens[4]);
    e->bairro = strdup(tokens[5]);
    e->regiao = strdup(tokens[6]);
    e->cep = atoi(tokens[7]);
    e->lat = atof(tokens[8]);
    e->lon = atof(tokens[9]);

    free_split(tokens, count);
    return e;
}

void destruir_endereco(Endereco *e) {
    if (e) {
        if(e->idEnd) free(e->idEnd);
        if(e->tipoLog) free(e->tipoLog);
        if(e->nomeLog) free(e->nomeLog);
        if(e->bairro) free(e->bairro);
        if(e->regiao) free(e->regiao);
        free(e);
    }
}