#include "../include/t2fs.h"
#include "teste.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  start_test("close2");

  int ret_code = close2(0);
  // assert(Fechar arquivo não aberto?)
  
  int handle = create2("file");
  ret_code = close2(handle);
  assert("Em caso de sucesso, retorna 0", ret_code == 0);

  ret_code = close2(handle);
  // assert(Fechar arquivo já fechado?)

  end_test();

  return 0;
}