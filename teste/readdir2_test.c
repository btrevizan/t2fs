#include "../include/t2fs.h"
#include "teste.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  start_test("readdir2");

  DIRENT2 entry;

  mkdir2("dir");

  DIR2 dir_handle = opendir2("dir");
  int ret_code = readdir2(dir_handle, &entry);
  assert("Em caso de sucesso, retorna 0", ret_code == 0);

  assert("Deve fazer a leitura corretamente",
         strcmp(entry.name, ".") == 0 &&
         entry.fileType == TYPEVAL_DIRETORIO &&
         entry.fileSize == 256 * 4);
  
  readdir2(dir_handle, &entry);
  assert("Deve avancar o current entry",
         strcmp(entry.name, "..") == 0);
  
  ret_code = readdir2(dir_handle, &entry);
  assert("Se chegou no fim da lista de entradas, deve retornar um erro", 
         ret_code != 0);

  end_test();

  return 0;
}