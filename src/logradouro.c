#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../include/logradouro.h"
#include "../include/endereco.h"
#include "../include/utils.h"

// Funções Auxiliares da Hash
size_t hash_function(int key, size_t tam) {
    return (unsigned long)key % tam;
}

CadastroLogradouros* criar_cadastro(size_t tamEstimado) {
    CadastroLogradouros *cad = (CadastroLogradouros*) malloc(sizeof(CadastroLogradouros));
    cad->capacidade = tamEstimado > 0 ? tamEstimado : 100;
    cad->tamanho = 0;
    cad->vetor = (Logradouro**) malloc(cad->capacidade * sizeof(Logradouro*));
    cad->tamHash = (size_t)(cad->capacidade * 2.0);
    cad->tabelaHash = (int*) malloc(cad->tamHash * sizeof(int));
    for(size_t i = 0; i < cad->tamHash; i++) cad->tabelaHash[i] = -1;
    return cad;
}

void registrar_logradouro(CadastroLogradouros *cad, int idLog, char *nome, double lat, double lon) {
    if (cad->tamanho >= cad->capacidade) {
        cad->capacidade *= 2;
        cad->vetor = (Logradouro**) realloc(cad->vetor, cad->capacidade * sizeof(Logradouro*));
        // Nota: Idealmente redimensionaria a Hash aqui também
    }

    size_t idx = hash_function(idLog, cad->tamHash);
    size_t idxOriginal = idx;
    
    while (cad->tabelaHash[idx] != -1) {
        int indiceVetor = cad->tabelaHash[idx];
        Logradouro *l = cad->vetor[indiceVetor];
        
        if (l->idLog == idLog) {
            l->somaLat += lat;
            l->somaLong += lon;
            l->contador++;
            return;
        }
        idx = (idx + 1) % cad->tamHash;
        if (idx == idxOriginal) return; 
    }

    Logradouro *novo = (Logradouro*) malloc(sizeof(Logradouro));
    novo->idLog = idLog;
    novo->nome = strdup(nome);
    novo->somaLat = lat;
    novo->somaLong = lon;
    novo->contador = 1;
    novo->latMedia = 0.0;
    novo->longMedia = 0.0;

    int novoIndice = cad->tamanho;
    cad->vetor[novoIndice] = novo;
    cad->tamanho++;
    cad->tabelaHash[idx] = novoIndice;
}

void calcular_centroides(CadastroLogradouros *cad) {
    for (size_t i = 0; i < cad->tamanho; i++) {
        Logradouro *l = cad->vetor[i];
        if (l->contador > 0) {
            l->latMedia = l->somaLat / l->contador;
            l->longMedia = l->somaLong / l->contador;
        }
    }
}

void destruir_cadastro(CadastroLogradouros *cad) {
    if (cad) {
        for (size_t i = 0; i < cad->tamanho; i++) {
            free(cad->vetor[i]->nome);
            free(cad->vetor[i]);
        }
        free(cad->vetor);
        free(cad->tabelaHash);
        free(cad);
    }
}

// Leitura do Arquivo
int ler_arquivo_entrada(CadastroLogradouros *cad, FILE *entrada) {
    if (!entrada) return 0;

    char linha[1024];
    
    // Ler N (número de endereços)
    if (fgets(linha, sizeof(linha), entrada) == NULL) {
        return 0;
    }
    int qtdEnderecos = atoi(linha);
    int lidos = 0;

    // Loop para ler EXATAMENTE 'qtdEnderecos' válidos
    while (lidos < qtdEnderecos && fgets(linha, sizeof(linha), entrada)) {
        char *limpa = trim(linha);
        if (strlen(limpa) == 0) continue; // Linhas em branco não contam

        Endereco *end = criar_endereco_de_linha(limpa);
        
        if (end != NULL) {
            registrar_logradouro(cad, end->idLog, end->nomeLog, end->lat, end->lon);
            destruir_endereco(end);
            lidos++; // Contabiliza apenas se processou um endereço
        }
    }
    
    // Não fechamos o arquivo, pois ele será usado na próxima etapa
    return 1;
}