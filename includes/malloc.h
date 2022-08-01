#pragma once

typedef unsigned long size_t;

void free(void *ptr);
void *malloc(size_t size);
void *realloc(void *ptr, size_t size);
