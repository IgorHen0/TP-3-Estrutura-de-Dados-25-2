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

// Função para redimensionar a tabela hash e manter a performance O(1)
void redimensionar_hash(CadastroLogradouros *cad) {
    size_t novoTamHash = cad->tamHash * 2;
    int *novaTabela = (int*) malloc(novoTamHash * sizeof(int));
    
    // Inicializa nova tabela com -1
    for (size_t i = 0; i < novoTamHash; i++) {
        novaTabela[i] = -1;
    }

    // Rehashing: Reinsere todos os índices existentes na nova tabela
    for (size_t i = 0; i < cad->tamanho; i++) {
        int idLog = cad->vetor[i]->idLog;
        size_t idx = hash_function(idLog, novoTamHash);
        
        // Tratamento de colisão (Linear Probing) na nova tabela
        while (novaTabela[idx] != -1) {
            idx = (idx + 1) % novoTamHash;
        }
        novaTabela[idx] = (int)i; // Mapeia para o índice no vetor denso
    }

    // Libera a tabela antiga e atualiza o cadastro
    free(cad->tabelaHash);
    cad->tabelaHash = novaTabela;
    cad->tamHash = novoTamHash;
}

void registrar_logradouro(CadastroLogradouros *cad, int idLog, char *nome, double lat, double lon) {
    // Busca primeiro para ver se já existe (usando a hash atual)
    size_t idx = hash_function(idLog, cad->tamHash);
    size_t idxOriginal = idx;
    
    while (cad->tabelaHash[idx] != -1) {
        int indiceVetor = cad->tabelaHash[idx];
        Logradouro *l = cad->vetor[indiceVetor];
        
        if (l->idLog == idLog) {
            // Encontrou: atualiza média e retorna
            l->somaLat += lat;
            l->somaLong += lon;
            l->contador++;
            return;
        }
        idx = (idx + 1) % cad->tamHash;
        if (idx == idxOriginal) break; // Volta completa
    }

    // Se não encontrou, precisa inserir. Verifica capacidade.
    if (cad->tamanho >= cad->capacidade) {
        cad->capacidade *= 2;
        Logradouro **tempVetor = (Logradouro**) realloc(cad->vetor, cad->capacidade * sizeof(Logradouro*));
        if (!tempVetor) {
            fprintf(stderr, "Erro fatal: Falha ao redimensionar vetor de logradouros.\n");
            exit(EXIT_FAILURE);
        }
        cad->vetor = tempVetor;

        // Implementação da Robustez: Redimensiona a Hash também
        redimensionar_hash(cad);
        
        // Recalcula o índice para o novo tamanho de hash, pois ele mudou
        idx = hash_function(idLog, cad->tamHash);
        while (cad->tabelaHash[idx] != -1) {
            idx = (idx + 1) % cad->tamHash;
        }
    }

    // Criação do novo logradouro
    Logradouro *novo = (Logradouro*) malloc(sizeof(Logradouro));
    if (!novo) {
        fprintf(stderr, "Erro de alocacao de logradouro.\n");
        exit(EXIT_FAILURE);
    }
    novo->idLog = idLog;
    novo->nome = strdup(nome);
    novo->somaLat = lat;
    novo->somaLong = lon;
    novo->contador = 1;
    novo->latMedia = 0.0;
    novo->longMedia = 0.0;

    // Inserção nas estruturas
    int novoIndice = cad->tamanho;
    cad->vetor[novoIndice] = novo;
    cad->tamanho++;
    
    // Usa o idx calculado (seja o original ou o novo após resize)
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