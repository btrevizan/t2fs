#include "../include/t2fs.h"
#include "teste.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  start_test("closedir2");

  DIR2 dir_handle = opendir2(".");
  int ret_code = closedir2(dir_handle);
  assert("Em caso de sucesso, retorna 0", ret_code == 0);

  end_test();

  return 0;
}