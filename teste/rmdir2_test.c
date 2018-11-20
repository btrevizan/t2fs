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
  
  ret_code = rmdir2("dir1");
  assert("Deve retornar um erro se o diretorio nao esta vazio", ret_code != 0);

  mkdir2("empty_dir2");
  ret_code = rmdir2("empty_dir2");
  assert("Em caso de sucesso, retorna 0", ret_code == 0);

  // Buscar a entrada no diretório
  DIR2 dir_handle = opendir2(".");
  DIRENT2 dir_entry;

  int found = 0;
  while (readdir2(dir_handle, &dir_entry) == 0) {
    if (strcmp(dir_entry.name, "empty_dir2") == 0) {
      found = 1;
      break;
    }
  }
  closedir2(dir_handle);
  assert("A entrada no diretorio pai deve ser removida", !found);

  ret_code = rmdir2("file1.txt");
  assert("Deve retornar um erro se o filepath referencia um arquivo regular", 
         ret_code != 0);

  ret_code = rmdir2("link1");
  assert("Deve retornar um erro se o filepath referencia um link para "
         "arquivo regular", ret_code != 0);

  end_test();

  return 0;
}