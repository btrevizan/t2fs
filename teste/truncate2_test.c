#include "../include/t2fs.h"
#include "teste.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  start_test("truncate2");

  char buffer[8];

  int ret_code = truncate2(0);
  assert("Se o handle nao foi aberto, retorna um erro", ret_code != 0);

  FILE2 handle = open2("file1.txt");
  seek2(handle, 5);
  ret_code = truncate2(handle);
  assert("Em caso de sucesso, retorna 0", ret_code == 0);

  ret_code = read2(handle, buffer, 1);
  assert("O current pointer devera ficar no final do arquivo", ret_code == 0);

  seek2(handle, 0);
  int bytes_read = read2(handle, buffer, 8);
  assert("Deve realizar o truncamento corretamente", 
         bytes_read == 5 && strncmp(buffer, "Esse ", 5) == 0);
  
  // Obter a entrada no diretório
  DIR2 dir_handle = opendir2(".");
  DIRENT2 dir_entry;

  while (readdir2(dir_handle, &dir_entry) == 0) {
    if (strcmp(dir_entry.name, "file1.txt") == 0) {
      break;
    }
  }
  closedir2(dir_handle);
  assert("O tamanho do arquivo indicado na entrada do diretorio devera "
         "ficar igual a CP", dir_entry.fileSize == 5);

  close2(handle);
  ret_code = truncate2(handle);
  assert("Se o handle ja foi fechado, retorna um erro", ret_code != 0);

  end_test();

  return 0;
}