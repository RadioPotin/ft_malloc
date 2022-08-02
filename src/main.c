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
  // BRK at 0
  //
  // ask for 10, 10 + 7 / 8 * 8 = 16
  // add
  //
  //
  //
  assert (ft_malloc(10), 4);
  assert (ft_malloc(10), 20);
  assert (ft_malloc(10), 36);
  assert (ft_malloc(0), -1);
  assert (ft_malloc(10), 52);
  return 0;
}
