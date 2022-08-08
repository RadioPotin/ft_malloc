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

  // Testing malloc. first arg is address that malloc returns, second is address
  //
  // counting offset from header
  assert (ft_malloc(10), 4);
  assert (ft_malloc(10), 20);
  assert (ft_malloc(10), 36);
  assert (ft_malloc(0), -1);
  assert (ft_malloc(10), 52);
  // checking if finding correct header
  // 4 is offsetted address from header of a block of size 16 + 1(alloc bit)
  assert (get_block_header(4), 17);
  assert (get_block_header(20), 17);
  assert (get_block_header(36), 17);
  assert (get_block_header(52), 17);

  // Testing free behaviour.
  //
  ft_free(4);
  assert(get_block_header(4), 16);
  assert (ft_malloc(10), 4);
  assert(get_block_header(4), 17);
  ft_free(4);
  assert(get_block_header(4), 16);
  assert (ft_malloc(16), 68);
  assert (ft_malloc(10), 4);

  // Testing coalescing of free block, with the next one
  ft_free(20);
  ft_free(4);
  assert (ft_malloc(20), 4);
  return 0;
}
