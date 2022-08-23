#include "ft_malloc.h"

#include <stdio.h>

void assert(int got, int expected) {

  if (expected != got){
    printf("Expected: %d\n", expected);
    printf("GOT     : %d\n", got);
    exit(1);
  }
  return;

}

int main(void) {

  // Testing malloc.
  // counting offset from header
  assert (ft_malloc(10), 4);
  assert (ft_malloc(10), 28);
  assert (ft_malloc(10), 52);
  assert (ft_malloc(0), -1);
  assert (ft_malloc(10), 76);

  // checking if finding correct header
  // 4 is offsetted address from
  // header of a block of size 24 + 1(alloc bit)
  // 24 because:
  // (size + TAG_SIZE + TAG_SIZE + 7) / 8 * 8
  // (size + 8 + 7) / 8 * 8
  // (10 + 8 + 7) / 8 * 8
  // 25 / 8 * 8
  // 3 * 8
  // 24 + 1 allocbit
  assert (get_block_header(4), 25);
  assert (get_block_header(28), 25);
  assert (get_block_header(52), 25);
  assert (get_block_header(76), 25);

  // Testing free behaviour.
  //
  ft_free(4);
  assert(get_block_header(4), 24);
  assert (ft_malloc(10), 4);
  assert(get_block_header(4), 25);
  ft_free(4);
  assert(get_block_header(4), 24);
  assert (ft_malloc(16), 4);
  assert (ft_malloc(10), 100);

  // Testing coalescing of free block,
  // with the next one
  ft_free(28);
  ft_free(4);
  assert (ft_malloc(20), 4);

  // Testing block footer addr retrieval
  // first block = size 32, so footer addr = 28
  assert(get_block_footer_addr(4), 28);
  // value of footer is 33, 32 + alloc bit
  assert(get_block_footer(4), 33);
  assert(get_header_addr_from_footer_addr(28), 0);
  assert(get_header_from_footer(28), 33);

  // Testing coalescing of free block
  // with previous one
  ft_free(4);
  ft_free(76);
  ft_free(52);
  assert(ft_malloc(40), 4);
  assert (get_block_header(4), 49);

  return 0;
}
