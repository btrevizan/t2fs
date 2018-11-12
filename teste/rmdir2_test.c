#include "../include/t2fs.h"
#include "teste.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  start_test("rmdir2");

  int ret_code = rmdir2(".");
  assert("Deve retornar um erro ao tentar remover a entrada .", ret_code != 0);

  ret_code = rmdir2("..");
  assert("Deve retornar um erro ao tentar remover a entrada ..", ret_code != 0);

  ret_code = rmdir2("/");
  assert("Deve retornar um erro ao tentar deletar o diretorio raiz", 
         ret_code != 0);
  
  ret_code = rmdir2("nao_existo");
  assert("Deve retornar um erro se o diretorio nao existe", ret_code != 0);
  
  mkdir2("dir");
  FILE2 handle = create2("dir/file");
  close2(handle);
  ret_code = rmdir2("dir");
  assert("Deve retornar um erro se o diretorio nao esta vazio", ret_code != 0);

  delete2("dir/file");
  ret_code = rmdir2("dir");
  assert("Em caso de sucesso, retorna 0", ret_code == 0);

  // Buscar a entrada no diretório
  DIR2 dir_handle = opendir2(".");
  DIRENT2 dir_entry;

  int found = 0;
  while (readdir2(dir_handle, &dir_entry) == 0) {
    if (strcmp(dir_entry.name, "dir") == 0) {
      found = 1;
      break;
    }
  }
  closedir2(dir_handle);
  assert("A entrada no diretorio pai deve ser removida", !found);

  handle = create2("file");
  close2(handle);
  ret_code = rmdir2("file");
  assert("Deve retornar um erro se o filepath referencia um arquivo regular", 
         ret_code != 0);
  
  ln2("link", "file");
  ret_code = rmdir2("link");
  assert("Deve retornar um erro se o filepath referencia um link para "
         "arquivo regular", ret_code != 0);

  end_test();

  return 0;
}