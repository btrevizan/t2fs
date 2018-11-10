#include "../include/t2fs.h"
#include "teste.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    FILE2 handle;

    start_test("create2");

    mkdir2("dir");
    handle = create2("dir");
    assert("Deve retornar um erro se o filename referencia um diretorio", 
           handle < 0);
    


    ln2("dir_link", "dir");
    handle = create2("dir_link");
    assert("Deve retornar um erro se o filename referencia um link para "
           "diretorio", handle < 0);
    


    char name[51] = "olha_meu_nome_tem_50_caracteres_pode_contar.tar.gz";
    handle = create2(name);
    assert("Deve aceitar nome de tamanho 50", handle >= 0);



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

    assert("Deve ser criada uma entrada no diretorio com o nome fornecido", 
           found);

    assert("A entrada de diretório criada deve ser do tipo regular", 
           found && dir_entry.fileType == TYPEVAL_REGULAR);

    assert("A entrada de diretório criada deve ter 0 bytes de tamanho", 
           found && dir_entry.fileSize == 0);



    write2(handle, "conteudo", 8);
    close2(handle);
    handle = create2(name);
    seek2(handle, 0);
    char buffer[8];
    int bytes_read = read2(handle, buffer, 8);

    assert("Se o arquivo ja existe, seu conteúdo eh removido", bytes_read == 0);



    // Obter a entrada no diretório
    dir_handle = opendir2(".");

    found = 0;
    while (readdir2(dir_handle, &dir_entry) == 0) {
      if (strcmp(dir_entry.name, name) == 0) {
        found = 1;
        break;
      }
    }

    closedir2(dir_handle);

    assert("Se o arquivo já existe, ele assume o tamanho de 0 bytes", 
           dir_entry.fileSize == 0);



    close2(handle);
    ln2("link", "file.txt");
    handle = create2("link");
    close2(handle);

    // Obter a entrada no diretório
    dir_handle = opendir2(".");

    int found_link = 0;
    int found_file = 0;
    while (readdir2(dir_handle, &dir_entry) == 0) {

      if (strcmp(dir_entry.name, "link") == 0 
          && dir_entry.fileType == TYPEVAL_LINK) {
        found_link = 1;
      }
      else if (strcmp(dir_entry.name, "file.txt") == 0 
               && dir_entry.fileType == TYPEVAL_REGULAR) {
        found_file = 1;
      }

      if (found_file && found_link) break;
    }

    closedir2(dir_handle);

    assert("Se o filename referencia um link, o alvo do link eh afetado, "
           "e nao o link", found_file && found_link);


    int sectorsPerCluster = 4; // Assumindo que seja este o valor correto.
    int max_dir_entries = 4 * sectorsPerCluster;
    for (int i = 0; i < max_dir_entries + 1; i++) {
      sprintf(name, "%d", i);
      handle = create2(name);
    }

    assert("Deve retornar um erro ao tentar criar mais entradas de "
           "diretório do que o suportado", handle < 0);

    end_test();
    return 0;
}
