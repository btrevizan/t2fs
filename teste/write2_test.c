#include "../include/t2fs.h"
#include "teste.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  start_test("write2");

  char buffer[12];

  int ret_code = write2(0, "conteudo", 8);
  assert("Se o handle nao foi aberto, retorna um erro", ret_code < 0);

  int handle = create2("file");
  ret_code = write2(handle, "conteudo", 8);
  assert("Deve retornar o numero de bytes escrito", ret_code == 8);

  int handle2 = open2("file");
  read2(handle2, buffer, 8);
  assert("Deve fazer a escrita corretamente", 
         strncmp(buffer, "conteudo", 8) == 0);

  write2(handle, "mais", 4);
  read2(handle2, buffer, 4);
  assert("Deve avancar o current pointer", strncmp(buffer, "mais", 4) == 0);

  close2(handle2);

  // Obter a entrada no diretório
  DIR2 dir_handle = opendir2(".");
  DIRENT2 dir_entry;

  while (readdir2(dir_handle, &dir_entry) == 0) {
    if (strcmp(dir_entry.name, "file") == 0) {
      break;
    }
  }
  closedir2(dir_handle);
  assert("Se a escrita aumenta o tamanho do arquivo, o tamanho do "
         "arquivo indicado na entrada do diretorio deve aumentar", 
         dir_entry.fileSize == 12);
  
  seek2(handle, 0);
  write2(handle, "teste", 5);
  seek2(handle, 0);
  read2(handle, buffer, 12);
  assert("A sobrescrita de dados deve funcionar corretamente", 
         strncmp(buffer, "testeudomais", 12) == 0);
  
  // Obter a entrada no diretório
  dir_handle = opendir2(".");

  while (readdir2(dir_handle, &dir_entry) == 0) {
    if (strcmp(dir_entry.name, "file") == 0) {
      break;
    }
  }
  closedir2(dir_handle);
  assert("Se a escrita nao aumenta o tamanho do arquivo, o tamanho do "
         "arquivo indicado na entrada do diretorio nao deve aumentar", 
         dir_entry.fileSize == 12);
  
  close2(handle);
  ret_code = write2(handle, "conteudo", 8);
  assert("Se o handle ja foi fechado, retorna um erro", ret_code < 0);

  end_test();

  return 0;
}