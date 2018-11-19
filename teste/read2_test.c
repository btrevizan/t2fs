#include "../include/t2fs.h"
#include "teste.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  start_test("read2");

  char buffer[8];

  int ret_code = read2(0, buffer, 8);
  assert("Se o handle nao foi aberto, retorna um erro", ret_code < 0);

  FILE2 handle = open2("file1.txt");
  ret_code = read2(handle, buffer, 3);
  assert("Deve retornar o numero de bytes lido", ret_code == 3);
  assert("Deve fazer a leitura corretamente", strncmp(buffer, "Ess", 3) == 0);

  read2(handle, buffer, 2);
  assert("Deve avancar o current pointer", strncmp(buffer, "e ", 2) == 0);

  ret_code = read2(handle, buffer, 60);
  assert("Se chegou no fim do arquivo antes de ler size bytes, o numero "
         "de bytes efetivamente lido deve ser retornado", ret_code == 50);
  
  close2(handle);
  ret_code = read2(handle, buffer, 8);
  assert("Se o handle ja foi fechado, retorna um erro", ret_code < 0);

  end_test();

  return 0;
}