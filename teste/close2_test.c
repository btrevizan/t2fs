#include "../include/t2fs.h"
#include "teste.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  start_test("close2");
  
  FILE2 handle = open2("file1.txt");
  int ret_code = close2(handle);
  assert("Em caso de sucesso, retorna 0", ret_code == 0);

  end_test();

  return 0;
}