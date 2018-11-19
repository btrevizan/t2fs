#include "../include/t2fs.h"
#include "teste.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  start_test("chdir2");

  int ret_code = chdir2("/nao/existo");
  assert("Se o filepath nao existe, retorna um erro", ret_code != 0);

  ret_code = chdir2("file1.txt");
  assert("Se o filepath referencia um arquivo regular, retorna um erro", 
         ret_code != 0);

  ret_code = chdir2("link1");
  assert("Se o filepath referencia um link para arquivo regular, retorna "
         "um erro", ret_code != 0);

  ret_code = chdir2("dir1");
  assert("Em caso de sucesso, retorna 0", ret_code == 0);

  char cwd[5];
  getcwd2(cwd, 5);
  assert("Se o filepath referencia um diretorio, deve seguir entrar "
         "no diretorio", strcmp(cwd, "/dir1") == 0);

  chdir2("..");
  chdir2("dir_link");
  getcwd2(cwd, 5);
  assert("Se o filepath referencia um link para um diretorio, deve seguir "
         "o link", strcmp(cwd, "/dir1") == 0);

  end_test();

  return 0;
}