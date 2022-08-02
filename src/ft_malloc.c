#include "ft_malloc.h"

#define BLOCK_ALLOC_MASK 1
#define BLOCK_SIZE_MASK (-2)
#define ALIGNMENT 8
#define TAG_SIZE 4

// block_classes[] will help organise free block lists by allocation size
// Classifying blocks by number of words required for allocation(size_t)

// free_list is a list of headers of free blocks
size_t *free_list;

size_t memory[238367];

void *brkk;

size_t align(size_t size) {
  return (size + 7) / 8 * 8;
}

// temporary function to handle memory pages allocation
size_t ft_sbrk(intptr_t increment){
  void *old;

  old = brkk;
  brkk = brkk + increment;
  return (size_t)old;
}

// function call to help make implementation portable by asking
// OS for the size of memory page
/*
   size_t ft_getpagesize(void) {
   return ((size_t)getpagesize());
   }
   */

// get_size_from_tag function extracts the size from a block's tag, either header or footer
size_t get_size_from_tag(size_t header_or_footer){
  size_t size;

  size = header_or_footer & BLOCK_SIZE_MASK;
  return (size);
}

// is_allocated function either returns dedicated allocation bit from a given block tag, either header or footer.
size_t is_allocated_from_tag(size_t header_or_footer) {
  return (header_or_footer & BLOCK_ALLOC_MASK);
}

// get_prev_footer function will return the footer of the previous block to
// the one passed as parameter
size_t get_prevblock_footer(void *block){
  size_t *header;

  header = block;
  return (*header - sizeof(size_t));
}

// get_prevblock_size function returns the size of the previous block
size_t get_prevblock_size(void *block) {
  size_t prev_footer_tag;
  size_t size;

  prev_footer_tag = get_prevblock_footer(block);
  size = get_size_from_tag(prev_footer_tag);
  return (size);
}

// get_header function will return the header of a block passed as parameter
size_t get_block_header(void *block) {
  size_t *header;

  header = block;
  return *header;
}

size_t get_block_size(void *block) {
  size_t header_tag;
  size_t size;

  header_tag = get_block_header(block);
  size = get_size_from_tag(header_tag);
  return (size);
}

void *get_block_payload(void *ptr) {
  return (ptr + sizeof(size_t));
}

//
size_t is_allocated_from_block(void *block) {
  size_t header_tag;

  header_tag = get_block_header(block);
  return(is_allocated_from_tag(header_tag));
}

size_t set_tag(size_t size) {
  size_t tag;

  tag = size | 1;
  return (tag);
}



void free(void *ptr);
void *realloc(void *ptr, size_t size);

int ft_malloc(size_t size) {

  if (size <= 0)
    return (-1);


  // Test for available free blocks
  // TODO
  //
  // enlarge BRK
  size_t block_size = align(size);
  size_t blockstart= ft_sbrk(block_size);
  memory[blockstart] = set_tag(block_size);

  return (blockstart + TAG_SIZE);

  return (-1);
}
