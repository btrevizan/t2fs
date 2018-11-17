#include "../include/t2fs.h"
#include "teste.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  start_test("getcwd2");

  char cwd[2];

  int ret_code = getcwd2(cwd, 2);
  assert("Retorna o caminho correto", strcmp(cwd, "/") == 0);

  assert("Em caso de sucesso, retorna 0", ret_code == 0);

  end_test();

  return 0;
}