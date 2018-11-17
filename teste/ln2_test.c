#include "../include/t2fs.h"
#include "teste.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  start_test("ln2");

  int ret_code = ln2("link", "file");
  assert("Em caso de sucesso, retorna 0", ret_code == 0);

  // Obter a entrada no diretório
  DIR2 dir_handle = opendir2(".");
  DIRENT2 dir_entry;

  int found = 0;
  while (readdir2(dir_handle, &dir_entry) == 0) {
    if (strcmp(dir_entry.name, "link") == 0) {
      found = 1;
      break;
    }
  }

  assert("Deve ser criada uma entrada no diretorio com o nome fornecido, "
         "do tipo link, e com tamanho igual ao tamanho de 1 cluster",
         found &&
         dir_entry.fileType == TYPEVAL_LINK &&
         dir_entry.fileSize == 256 * 4);

  closedir2(dir_handle);

  end_test();

  return 0;
}