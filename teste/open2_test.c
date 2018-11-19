#include "../include/t2fs.h"
#include "teste.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  start_test("open2");

  FILE2 handle[10];

  handle[0] = open2("nao_existo");
  assert("Se o arquivo nao existe, retorna um erro", handle[0] < 0);

  handle[0] = open2(".");
  assert("Deve retornar um erro se o filename referencia um diretorio", 
         handle[0] < 0);

  handle[0] = open2("file1.txt");
  assert("O handle deve ser nao-negativo", handle[0] >= 0);

  handle[1] = open2("file1.txt");
  assert("Deve ser possivel abrir o mesmo arquivo mais de uma vez", 
         handle[1] >= 0);
  
  char buffer[8];
  write2(handle[0], "conteudo", 8);
  read2(handle[1], buffer, 8);
  assert("Ao abrir o mesmo arquivo mais de uma vez, cada handle deve ter "
         "seu proprio current pointer associado e independente um do outro", 
         strcmp(buffer, "conteudo") == 0);
  
  int failed = 0;
  for (int i = 2; i < 10; i++) {
    handle[i] = open2("file1.txt");
    if (handle[i] < 0) {
      failed = 1;
      break;
    }
  }
  assert("Deve ser possivel abrir ate 10 arquivos simultaneamente", !failed);

  for (int i = 0; i < 10; i++) {
    close2(handle[i]);
  }

  buffer[0] = '\0'; // buffer = ""
  handle[0] = open2("file1.txt");
  read2(handle[0], buffer, 8);
  assert("O current pointer deve ser colocado na posicao 0", 
         strcmp(buffer, "conteudo") == 0);
  
  close2(handle[0]);

  buffer[0] = '\0'; // buffer = ""
  handle[0] = open2("link1");
  read2(handle[0], buffer, 8);
  assert("Se o filename referencia um link para um arquivo regular, o "
         "arquivo apontado pelo link deve ser aberto", 
         strcmp(buffer, "Esse eh ") == 0);
  
  close2(handle[0]);
  
  handle[0] = open2("dir_link");
  assert("Se o filename referencia um link para um diretorio, deve retornar "
         "um erro", handle[0] < 0);

  end_test();

  return 0;
}