#include "../include/t2fs.h"
#include "teste.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  start_test("opendir2");

  mkdir2("dir");

  DIR2 dir_handle = opendir2("dir");
  assert("O handle deve ser nao-negativo", dir_handle >= 0);

  DIRENT2 entry;
  readdir2(dir_handle, &entry);
  assert("O current entry deve ser colocado na primeira posição valida", 
         strcmp(entry.name, ".") == 0);

  end_test();

  return 0;
}