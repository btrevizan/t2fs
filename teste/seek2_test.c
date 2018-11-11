#include "../include/t2fs.h"
#include "teste.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  start_test("seek2");

  char buffer[8];

  int ret_code = seek2(0, 0);
  assert("Se o handle nao foi aberto, retorna um erro", ret_code != 0);

  // Cria um arquivo com conteúdo
  FILE2 handle = create2("file");
  write2(handle, "conteudo", 8);

  ret_code = seek2(handle, -2);
  assert("Se o offset for menor que -1, retorna um erro", ret_code != 0);

  ret_code = seek2(handle, 9);
  assert("Se o offset for maior que o tamanho do arquivo, retorna um erro", 
         ret_code != 0);

  ret_code = seek2(handle, 3);
  assert("Em caso de sucesso, retorna 0", ret_code == 0);

  read2(handle, buffer, 2);
  assert("Deve reposicionar o current pointer corretamente", 
         strncmp(buffer, "te", 2) == 0);
  
  seek2(handle, -1);
  ret_code = read2(handle, buffer, 1);
  assert("Se o offset for -1, posiciona o current pointer no final do "
         "arquivo", ret_code == 0);
  
  close2(handle);
  ret_code = seek2(handle, 0);
  assert("Se o handle ja foi fechado, retorna um erro", ret_code != 0);

  end_test();

  return 0;
}