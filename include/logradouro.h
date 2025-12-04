#ifndef LOGRADOURO_H
#define LOGRADOURO_H

#include <stdio.h>

// Estrutura que representa um Logradouro Único
typedef struct {
    int idLog;
    char *nome;
    double somaLat;
    double somaLong;
    int contador;
    double latMedia;
    double longMedia;
} Logradouro;

// Estrutura para gerenciar a coleção de logradouros
typedef struct {
    Logradouro **vetor;      
    size_t tamanho;          
    size_t capacidade;       
    int *tabelaHash;         
    size_t tamHash;          
} CadastroLogradouros;

CadastroLogradouros* criar_cadastro(size_t tamEstimado);
void registrar_logradouro(CadastroLogradouros *cad, int idLog, char *nome, double lat, double lon);
void calcular_centroides(CadastroLogradouros *cad);
void destruir_cadastro(CadastroLogradouros *cad);
int ler_arquivo_entrada(CadastroLogradouros *cad, FILE *entrada);

#endif