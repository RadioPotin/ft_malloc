#pragma once

#include <stdlib.h>
#include <stdint.h>

size_t ft_sbrk(intptr_t increment);

void free(void *ptr);
int ft_malloc(size_t size);
void *realloc(void *ptr, size_t size);
