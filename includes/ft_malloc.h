#pragma once

#include <stdlib.h>
#include <stdint.h>

int get_block_header(int block);
int get_block_footer_addr(int block);
int get_header_addr_from_footer_addr(int footer_addr);
int get_header_from_footer(int footer_addr);
int get_block_footer(int block);
void ft_free(int ptr);
int ft_malloc(int size);
void *ft_realloc(void *ptr, int size);
