#pragma once

#include <stdlib.h>
#include <stdint.h>

int ft_sbrk(intptr_t increment);
int get_block_header(int block);
void ft_free(int ptr);
int ft_malloc(int size);
void *ft_realloc(void *ptr, int size);
