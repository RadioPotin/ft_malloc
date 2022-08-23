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
// address of a TAG (footer or header) of a given block
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
  return (header_or_footer & BLOCK_SIZE_MASK);
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
// Header/footer of a block
//
// BEHAVIOUR:
// Marks block as FREE
static int unset_tag(int header) {
  return (header & 0xfffe);
}

// ARGUMENT:
// Header/footer of a block
//
// BEHAVIOUR:
// Marks block as allocated
static int set_tag(int header) {
  return (header | 1);
}

// ARGUMENT:
// Address of a block FOOTER.
//
// BEHAVIOUR:
// Returns the address of the header of that block.
int get_header_addr_from_footer_addr(int addr_of_footer)
{
  int size;
  int footer;
  int addr_of_header;

  footer = load_int(addr_of_footer);
  size = get_size(footer);
  addr_of_header = addr_of_footer - (size - TAG_SIZE);
  return addr_of_header;
}

// ARGUMENT:
// Address of a block FOOTER.
//
// BEHAVIOUR:
// Returns the header of that block.
int get_header_from_footer(int addr_of_footer)
{
  int addr_of_header;

  addr_of_header = get_header_addr_from_footer_addr(addr_of_footer);
  return load_int(addr_of_header);
}

// ARGUMENT:
// Offset address of a block.
// This address points to the payload, not the header
//
// BEHAVIOUR:
// Returns the header of that block.
int get_block_header(int offset) {
  return load_int(offset - TAG_SIZE);
}

// ARGUMENT:
// Offset address of a block.
// This address points to the payload, not the header
//
// BEHAVIOUR:
// Returns the address of the footer of that block.
int get_block_footer_addr(int offset) {
  int header;
  int header_addr;
  int size;
  int footer_address;

  header_addr = offset - TAG_SIZE;
  header = load_int(header_addr);
  size = get_size(header);
  footer_address = (header_addr + size) - TAG_SIZE;
  return footer_address;
}

// ARGUMENT:
// Offset address of a block.
// This address points to the payload, not the header
//
// BEHAVIOUR:
// Returns the value of the footer of block given as arg.
int get_block_footer(int offset)
{
  int footer_addr;

  footer_addr = get_block_footer_addr(offset);
  return (load_int(footer_addr));
}

// ARGUMENT:
// Size required by user
//
// Behaviour:
// Return a new size aligned on a multiple of 8.
// If Size is already a multiple,
//      returns (size + headersize)
// Else
//      return the next greater multiple (with header AND footer sizes considered)
static int align(int size) {
  return (size + TAG_SIZE + TAG_SIZE + 7) / 8 * 8;
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
  int tmp_ptr;
  int newsize;

  if (list + wantedsize >= memory_size) {
    return (-1);
  }
  tag = load_int(list);
  size = get_size(tag);
  if (tag == 0) {
    return list;
  } else if (!is_allocated(tag)) {
    if (size >= wantedsize) {
      // go to next block and check if it's of size 0
      // If so,
      // put a header at that address with relevant
      // information on the remaining room of free block
      tmp_ptr = list + wantedsize;
      tag = load_int(tmp_ptr);
      if (!get_size(tag)) {
        newsize = size - wantedsize;
        store_int(tmp_ptr, newsize);
      }
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
// TODO
// Coalesce with previous block
void ft_free(int ptr) {
  int addr_of_next_header;
  int next_header;
  int next_size;

  int addr_of_current_header;
  int addr_of_current_footer;
  int current_header;
  int current_size;

  int addr_of_prev_header;
  int addr_of_prev_footer;
  int prev_footer;

  addr_of_current_header = ptr - TAG_SIZE;
  addr_of_current_footer = get_block_footer_addr(ptr);

  current_header = load_int(addr_of_current_header);
  if (!ptr || !is_allocated(current_header))
    return ;

  current_size = get_size(current_header);
  addr_of_next_header = addr_of_current_header + current_size;
  // COALESCE WITH RIGHT SIDE BLOCK
  if (addr_of_next_header < memory_size) {
    next_header = load_int(addr_of_next_header);
    if (!is_allocated(next_header)) {
      next_size = get_size(next_header);
      current_header += next_size;
    }
  }
  store_int(addr_of_current_header, unset_tag(current_header));
  store_int(addr_of_current_footer, unset_tag(current_header));

  addr_of_prev_footer = (addr_of_current_header - TAG_SIZE);
  // COALESCE WITH LEFT SIDE BLOCK
  if (addr_of_prev_footer > 0)
  {
    prev_footer = load_int(addr_of_prev_footer);
    if (!is_allocated(prev_footer)) {
      prev_footer += current_header;
    }
    store_int(addr_of_prev_footer, prev_footer);
    addr_of_prev_header = get_header_addr_from_footer_addr(addr_of_prev_footer);
    store_int(addr_of_prev_header, prev_footer);
    return ;
  }
  return ;
}

int ft_malloc(int size) {
  int header;
  int header_addr;
  int footer_addr;

  if (size <= 0)
    return (-1);

  header = align(size);
  // look for an available free block
  // of adequate size
  header_addr = get_free_block(0, header);
  // if none is found
  if (header_addr < 0) {
    // enlarge BRK, allocate block
    header_addr = ft_sbrk(header);
  }
  // store header, at address of allocated block
  store_int(header_addr, set_tag(header));
  footer_addr = get_block_footer_addr(header_addr + TAG_SIZE);
  store_int(footer_addr, set_tag(header));
  return (header_addr + TAG_SIZE);
}

int ft_realloc(int ptr, int size) {
  int header_addr;
  int header;
  int block_size;
  int payload_size;
  int new_ptr;
  int new_size;

  header_addr = ptr - TAG_SIZE;
  header = load_int(header_addr);
  block_size = get_size(header);
  payload_size = block_size - (TAG_SIZE * 2);
  // man realloc(3) states:
  // if  size  is  equal to zero, and ptr is not NULL,
  // then the call is equivalent to  free(ptr)
  // (this behavior  is  nonportable;  see NOTES)
  if (ptr && !size) {
    ft_free(ptr);
  } else if (!ptr && size) {
    // man realloc(3) states:
    // If ptr is NULL,
    // then the call is equivalent to malloc(size),
    // for all values of  size;
    return (ft_malloc(size));
  }
  // check if ptr is bound to the current range of heap
  if (ptr && ptr < memory_size) {
    // given ptr payload is big enough
    if (size <= payload_size) {
      return ptr;
    } else {
      // realloc
      new_size = align(size);
      // change header of block
      // change footer of block
      // split block

      ft_free(ptr);
      return new_ptr;
    }
    // call sbrk here probably
    //
  }
  return -1;
}
