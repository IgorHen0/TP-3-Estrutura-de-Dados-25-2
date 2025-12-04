#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/utils.h"

IntVector* vector_create(size_t initial_capacity) {
    IntVector *vec = (IntVector*) malloc(sizeof(IntVector));
    if (!vec) return NULL;

    vec->data = (int*) malloc(initial_capacity * sizeof(int));
    if (!vec->data) {
        free(vec);
        return NULL;
    }

    vec->size = 0;
    vec->capacity = initial_capacity;
    return vec;
}

void vector_push_back(IntVector *vec, int value) {
    if (vec->size >= vec->capacity) {
        size_t new_capacity = vec->capacity == 0 ? 4 : vec->capacity * 2;
        int *new_data = (int*) realloc(vec->data, new_capacity * sizeof(int));
        if (!new_data) {
            fprintf(stderr, "Erro de alocação de memória no vetor.\n");
            exit(EXIT_FAILURE);
        }
        vec->data = new_data;
        vec->capacity = new_capacity;
    }
    vec->data[vec->size++] = value;
}

void vector_free(IntVector *vec) {
    if (vec) {
        free(vec->data);
        free(vec);
    }
}

// Função auxiliar para trocar dois valores
void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Particiona o array para o Quick Sort
int partition(int arr[], int low, int high) {
    int pivot = arr[high]; // Pivô na última posição
    int i = (low - 1); 

    for (int j = low; j <= high - 1; j++) {
        // Se o elemento atual é menor que o pivô
        if (arr[j] < pivot) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}

// Função recursiva do Quick Sort
void quick_sort_recursive(int arr[], int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);

        quick_sort_recursive(arr, low, pi - 1);
        quick_sort_recursive(arr, pi + 1, high);
    }
}

// Função pública que chama o sort manual
void vector_sort(IntVector *vec) {
    if (vec->size > 1) {
        quick_sort_recursive(vec->data, 0, vec->size - 1);
    }
}

char* trim(char *str) {
    char *end;

    // Remove espaços do início
    while(isspace((unsigned char)*str)) str++;

    if(*str == 0) return str; // String vazia

    // Remove espaços do fim
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;

    // Adiciona o terminador nulo
    *(end + 1) = 0;

    return str;
}

char** split(char *str, const char *delim, int *count) {
    int capacity = 10;
    int size = 0;
    char **tokens = (char**) malloc(capacity * sizeof(char*));
    char *token;

    // Usa strtok para dividir. 
    token = strtok(str, delim);
    while (token != NULL) {
        if (size >= capacity) {
            capacity *= 2;
            tokens = (char**) realloc(tokens, capacity * sizeof(char*));
        }
        tokens[size++] = token;
        token = strtok(NULL, delim);
    }

    *count = size;
    return tokens;
}

void free_split(char **tokens, int count) {
    free(tokens);
}