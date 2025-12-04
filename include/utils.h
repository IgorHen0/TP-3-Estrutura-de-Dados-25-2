#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>

typedef struct {
    int *data;
    size_t size;
    size_t capacity;
} IntVector;

// Funções do Vetor
IntVector* vector_create(size_t initial_capacity);
void vector_push_back(IntVector *vec, int value);
void vector_free(IntVector *vec);
void vector_sort(IntVector *vec); // Útil para a interseção futura

char* trim(char *str);

char** split(char *str, const char *delim, int *count);

void free_split(char **tokens, int count);

#endif