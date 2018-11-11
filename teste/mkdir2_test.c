#include "../include/t2fs.h"
#include "teste.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  start_test("mkdir2");

  char name[51] = "olha_sou_um_diretorio_e_meu_nome_tem_50_caracteres";
  int ret_code = mkdir2(name);
  assert("Deve aceitar nome de tamanho 50", ret_code == 0);

  ret_code = mkdir2(name);
  assert("Deve retornar um erro se o filepath ja esta em uso", ret_code != 0);

  // Obter a entrada no diretório
  DIR2 dir_handle = opendir2(".");
  DIRENT2 dir_entry;

  int found = 0;
  while (readdir2(dir_handle, &dir_entry) == 0) {
    if (strcmp(dir_entry.name, name) == 0) {
      found = 1;
      break;
    }
  }
  closedir2(dir_handle);

  assert("Deve ser criada uma entrada no diretorio pai com o nome fornecido", 
          found);
  assert("A entrada de diretorio criada deve ser do tipo diretorio", 
          found && dir_entry.fileType == TYPEVAL_DIRETORIO);
  assert("A entrada de diretorio criada deve ter o tamanho de 1 cluster", found 
         && dir_entry.fileSize == 256 * 4); // Assumindo cluster de 4 setores
  
  // Buscar as entradas . e ..
  dir_handle = opendir2(name);

  int found_dot = 0;
  int found_dot_dot = 0;
  while (readdir2(dir_handle, &dir_entry) == 0) {
    if (strcmp(dir_entry.name, ".") == 0 
        && dir_entry.fileType == TYPEVAL_DIRETORIO) {
      found_dot = 1;
    }
    else if (strcmp(dir_entry.name, "..") == 0 
              && dir_entry.fileType == TYPEVAL_DIRETORIO) {
      found_dot_dot = 1;
    }

    if (found_dot && found_dot_dot) break;
  }
  closedir2(dir_handle);

  assert("Devem ser criadas as entradas . e .., do tipo diretorio", 
         found_dot && found_dot_dot);
  
  int sectorsPerCluster = 4; // Assumindo que seja este o valor correto.
  int max_dir_entries = 4 * sectorsPerCluster;
  for (int i = 0; i < max_dir_entries + 1; i++) {
    sprintf(name, "%d", i);
    ret_code = mkdir2(name);
  }

  assert("Deve retornar um erro ao tentar criar mais entradas no "
         "diretorio pai do que o suportado", ret_code < 0);

  end_test();

  return 0;
}