#include "../include/t2fs.h"
#include "teste.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  start_test("delete2");

  int ret_code = delete2("nao_existo");
  assert("Se o arquivo nao existe, retorna um erro", ret_code != 0);



  ret_code = delete2(".");
  assert("Se o filename referencia um diretorio, deve retornar um erro", 
         ret_code != 0);
  


  FILE2 handle = create2("file");
  close2(handle);
  ret_code = delete2("file");
  assert("Em caso de sucesso, retorna 0", ret_code == 0);

  // Buscar a entrada no diretório
  DIR2 dir_handle = opendir2(".");
  DIRENT2 dir_entry;

  int found = 0;
  while (readdir2(dir_handle, &dir_entry) == 0) {
    if (strcmp(dir_entry.name, "file") == 0) {
      found = 1;
      break;
    }
  }

  closedir2(dir_handle);

  assert("A entrada no diretório deve ser removida", !found);



  handle = create2("file");
  close2(handle);
  ln2("link", "file");
  delete2("link");

  // Obter a entrada no diretório
  dir_handle = opendir2(".");

  int found_link = 0;
  int found_file = 0;
  while (readdir2(dir_handle, &dir_entry) == 0) {
    if (strcmp(dir_entry.name, "link") == 0) {
      found_link = 1;
    }
    else if (strcmp(dir_entry.name, "file") == 0) {
      found_file = 1;
    }
  }

  closedir2(dir_handle);

  assert("Se o filename referencia um link, o link deve ser deletado, e nao "
         "o arquivo apontado pelo link", found_file && !found_link);

  end_test();

  return 0;
}