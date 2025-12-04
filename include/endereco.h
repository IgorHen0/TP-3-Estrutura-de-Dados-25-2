#ifndef ENDERECO_H
#define ENDERECO_H

typedef struct {
    char *idEnd;
    int idLog;
    char *tipoLog;
    char *nomeLog;
    int numero;
    char *bairro;
    char *regiao;
    int cep;
    double lat;
    double lon;
} Endereco;

Endereco* criar_endereco_de_linha(char *linha);
void destruir_endereco(Endereco *end);

#endif