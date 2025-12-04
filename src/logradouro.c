#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../include/logradouro.h"
#include "../include/endereco.h"
#include "../include/utils.h"

#define FATOR_CARGA_MAX 0.7

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
    // Expansão do vetor se necessário
    if (cad->tamanho >= cad->capacidade) {
        cad->capacidade *= 2;
        cad->vetor = (Logradouro**) realloc(cad->vetor, cad->capacidade * sizeof(Logradouro*));
    }
    // Nota: Para um TP robusto, a Hash também deveria ser redimensionada aqui se o fator de carga estourar.

    size_t idx = hash_function(idLog, cad->tamHash);
    size_t idxOriginal = idx;
    
    while (cad->tabelaHash[idx] != -1) {
        int indiceVetor = cad->tabelaHash[idx];
        Logradouro *l = cad->vetor[indiceVetor];
        
        if (l->idLog == idLog) {
            // Atualiza existente
            l->somaLat += lat;
            l->somaLong += lon;
            l->contador++;
            return;
        }
        
        idx = (idx + 1) % cad->tamHash;
        if (idx == idxOriginal) {
            // Tabela cheia (não deve acontecer se tamEstimado for bom ou se tiver redimensionamento)
            return; 
        }
    }

    // Cria Novo
    Logradouro *novo = (Logradouro*) malloc(sizeof(Logradouro));
    novo->idLog = idLog;
    novo->nome = strdup(nome); // Copia o nome puro
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

int ler_arquivo_entrada(CadastroLogradouros *cad, const char *caminhoArquivo) {
    FILE *f = fopen(caminhoArquivo, "r");
    if (!f) return 0;

    char linha[1024];
    
    // Lê N
    if (fgets(linha, sizeof(linha), f) == NULL) {
        fclose(f);
        return 0;
    }

    while (fgets(linha, sizeof(linha), f)) {
        char *limpa = trim(linha);
        if (strlen(limpa) == 0) continue;

        // Usa o TAD Endereco para parsear
        Endereco *end = criar_endereco_de_linha(limpa);
        
        if (end != NULL) {
            registrar_logradouro(cad, end->idLog, end->nomeLog, end->lat, end->lon);

            destruir_endereco(end);
        }
    }

    fclose(f);
    return 1;
}