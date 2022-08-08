/*
// function call to help make implementation portable by asking
// OS for the size of memory page
int ft_getpagesize(void) {
return ((int)getpagesize());
}
*/

#include "ft_malloc.h"
#include <stdio.h>

#define BLOCK_ALLOC_MASK 1
#define BLOCK_SIZE_MASK -2
#define ALIGNMENT 8
#define HEADER_SIZE 4

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
int get_block_header(int offset) {
  return load_int(offset - HEADER_SIZE);
}

// ARGUMENT:
// header or the footer of a block
//
// BEHAVIOUR:
// Returns 0 if not allocated
// Returns 1 if allocated
static int is_allocated(int header_or_footer) {
  return (header_or_footer & BLOCK_ALLOC_MASK);
}

// ARGUMENT:
//  header or the footer of a block
//
// BEHAVIOUR:
// Returns the size of a block
static int get_size(int header_or_footer){
  int size;

  size = header_or_footer & BLOCK_SIZE_MASK;
  return (size);
}

// ARGUMENT
// 1. Tag of a block
// 2. Address of Tag
//
// DEBUG FUNCT
void print_header(int tag, int i)
{
  int size, allocbit;

  size = get_size(tag);
  allocbit = is_allocated(tag);

  printf("Header at address %d:\nSIZE: %d\nALLOCATED: %d\n\n", i, size, allocbit);
  return;
}

// ARGUMENT:
// Header of a block
//
// BEHAVIOUR:
// Marks block as allocated
static int unset_tag(int header) {
  return (header & 0xfffe);
}

// ARGUMENT:
// Header of a block
//
// BEHAVIOUR:
// Marks block as allocated
static int set_tag(int header) {
  return (header | 1);
}

// ARGUMENT:
// Size required by user
//
// Behaviour:
// Return a new size aligned on a multiple of 8.
// If Size is already a multiple,
//      returns (size + headersize)
// Else
//      return the next greater multiple (with header size considered)
static int align(int size) {
  return (size + HEADER_SIZE + 7) / 8 * 8;
}

// ARUGMENT:
// Takes an increment of the current pointer to the total memory available
//
// BEHAVIOUR:
// Increments pointer to total memory available
// Returns the previous pointer before incrementation
//
// temporary function to handle memory pages allocation
static int ft_sbrk(intptr_t increment){
  int *old;

  old = brkk;
  brkk = brkk + increment;
  return *old;
}

// ARGUMENTS:
// 1. Takes a pointer to a list of header block
// 2. Size needed for allocation of new block
//
// BEHAVIOUR:
// Return -1 is no block is no free block is found
// Otherwise returns address of header of a adequate block
static int get_free_block(int list, int wantedsize) {
  int tag;
  int size;

  if (list + wantedsize >= memory_size) {
    return (-1);
  }
  tag = load_int(list);
  size = get_size(tag);
  if (tag == 0) {
    return list;
  } else if (!is_allocated(tag)) {
    if (size >= wantedsize) {
      // TODO
      // Create new block after allocated one
      // with newsize=(size-wantedsize)
      return list;
    }
  }
  return (get_free_block(list + size, wantedsize));
}

// ARGUMENT:
// Pointer to free
//
// BEHAVIOUR:
// Frees the block given as argument,
// Coalesces it with the previous and next free block if possible
// (in order to limit fragmentation)
void ft_free(int ptr){
  int addr_of_next_header;
  int next_header;
  int next_size;
  int addr_of_header;
  int header;
  int size;

  if (!ptr)
    return ;
  // get addr of header
  addr_of_header = ptr - HEADER_SIZE;
  // load header
  header = load_int(addr_of_header);
  // make header free
  header = unset_tag(header);
  // get size of given header
  size = get_size(header);
  // get addr of next header after current
  addr_of_next_header = addr_of_header + size;
  if (addr_of_next_header < memory_size) {
    // load next header
    next_header = load_int(addr_of_next_header);
    if (!is_allocated(next_header)) {
      // if next header is also free
      // get its size for coalescing to first one
      next_size = get_size(next_header);
      // update size of first header
      header += next_size;
    }
  }
  store_int(addr_of_header, header);
}

int ft_malloc(int size) {
  int header;
  int blockstart;

  if (size <= 0)
    return (-1);

  header = align(size);
  // look for an available free block
  // of adequate size
  blockstart = get_free_block(0, header);
  // if none is found
  if (blockstart < 0) {
    // enlarge BRK, allocate block
    blockstart = ft_sbrk(header);
  }
  // store header, at address of allocated block
  store_int(blockstart, set_tag(header));
  return (blockstart + HEADER_SIZE);
}

void *ft_realloc(void *ptr, int size);
