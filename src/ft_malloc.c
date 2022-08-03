/*
// function call to help make implementation portable by asking
// OS for the size of memory page
int ft_getpagesize(void) {
return ((int)getpagesize());
}

// get_size_from_tag function extracts the size from a block's tag, either header or footer
int get_size_from_tag(int header_or_footer){
int size;

size = header_or_footer & BLOCK_SIZE_MASK;
return (size);
}


// get_prev_footer function will return the footer of the previous block to
// the one passed as parameter
int get_prevblock_footer(void *block){
int *header;

header = block;
return (*header - sizeof(int));
}

// get_prevblock_size function returns the size of the previous block
int get_prevblock_size(void *block) {
int prev_footer_tag;
int size;

prev_footer_tag = get_prevblock_footer(block);
size = get_size_from_tag(prev_footer_tag);
return (size);
}


int get_block_size(void *block) {
int header_tag;
int size;

header_tag = get_block_header(block);
size = get_size_from_tag(header_tag);
return (size);
}

void *get_block_payload(void *ptr) {
return (ptr + sizeof(int));
}

//
int is_allocated_from_block(void *block) {
int header_tag;

header_tag = get_block_header(block);
return(is_allocated_from_tag(header_tag));
}

 *
 * */
#include "ft_malloc.h"
#include <stdio.h>

#define BLOCK_ALLOC_MASK 1
#define BLOCK_SIZE_MASK -2
#define ALIGNMENT 8
#define TAG_SIZE 4

// allocated_list is a list of headers of allocated blocks
char memory[23836];

int *brkk;

int memory_size = 23836;

// ARGUMENTS:
// 1. address at which block is located in memory
// 2. header of that block
//
// BEHAVIOUR:
// Stores the header BYTE at that address in memory
static void store_int(int addr, int header_of_block) {
  int mask = 0xff;

  memory[addr] =  (unsigned char)(header_of_block >> 24);
  memory[addr+1] = (unsigned char)((header_of_block >> 16) & mask);
  memory[addr+2] = (unsigned char)((header_of_block >>8 & mask ));
  memory[addr+3] = (unsigned char)(header_of_block & mask);
  return ;
}

// ARGUMENT:
// address of the header of a given block
//
// BEHAVIOUR:
// returns the header BYTE at that address
static int load_int(int addr) {
  int i;

  i = memory[addr] << 24;
  i |= memory[addr+1] << 16;
  i |= memory[addr+2] << 8;
  i |= memory[addr+3];
  return i;
}

// ARGUMENT:
// Offset address of a block. This address points at the payload, not the header
//
// BEHAVIOUR:
// Returns the address of the header of that block.
static int get_block_header(int offset) {
  return load_int(offset - TAG_SIZE);
}

// is_allocated function returns dedicated allocation bit from a given block tag, either header or footer.
static int is_allocated(int header_or_footer) {
  return (header_or_footer & BLOCK_ALLOC_MASK);
}

// get_size_from_tag function extracts the size from a block's tag, either header or footer
static int get_size(int header_or_footer){
  int size;

  size = header_or_footer & BLOCK_SIZE_MASK;
  return (size);
}

void print_tag(int tag, int i)
{
  int size, allocbit;

  size = get_size(tag);
  allocbit = is_allocated(tag);

  printf("Header at address %d:\nSIZE: %d\nALLOCATED: %d\n\n", i, size, allocbit);
  return;
}


// sets a tag with given size and allocation bit at 1
static int set_tag(int size) {
  int tag;

  tag = size | 1;
  return (tag);
}

//
static int align(int size) {
  return (size + 7) / 8 * 8;
}

// temporary function to handle memory pages allocation
static int ft_sbrk(intptr_t increment){
  int *old;

  old = brkk;
  brkk = brkk + increment;

  return *old;
}

static int get_free_block(int freelist, int wantedsize) {
  int tag;
  int size;

  if (freelist + wantedsize >= memory_size) {
    return (-1);
  }
  tag = load_int(freelist);
  size = get_size(tag);
  if (tag == 0) {
    return freelist;
  } else if (!is_allocated(tag)) {
    if (size >= wantedsize) {
      return freelist;
    }
  }
  return (get_free_block(freelist + size, wantedsize));
}

void ft_free(int ptr){
  int addr;

  addr = ptr - TAG_SIZE;
  memory[addr] &= 0xfffe;
}

int ft_malloc(int size) {
  int block_size;
  int blockstart;

  if (size <= 0)
    return (-1);

  block_size = align(size);
  // look for an available free block
  // of adequate size
  blockstart = get_free_block(0, block_size);
  // if none is found
  if (blockstart < 0) {
    // enlarge BRK, allocate block
    blockstart = ft_sbrk(block_size);
  }
  // store header tag, at address of allocated block
  store_int(blockstart, set_tag(block_size));
  return (blockstart + TAG_SIZE);
}

void *ft_realloc(void *ptr, int size);
