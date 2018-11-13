#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "../include/t2fs.h"
#include "../include/helpers.h"
#include "../include/apidisk.h"

static char first_run = 1;
static struct t2fs_superbloco superblock;
static struct fcb open_files[N_OPEN_FILES];
static struct fcb open_dirs[1];
static unsigned int *fat;
static unsigned int CLUSTER_SIZE;

int identify2 (char *name, int size) {
	if(first_run == 1) t2fs_init();
	return -1;
}



FILE2 create2 (char *filename) {
	return -1;
}



int delete2 (char *filename) {
	return -1;
}



FILE2 open2 (char *filename) {
	return -1;
}



int close2 (FILE2 handle) {
	return -1;
}



int read2 (FILE2 handle, char *buffer, int size) {
	return -1;
}



int write2 (FILE2 handle, char *buffer, int size) {
	return -1;
}



int truncate2 (FILE2 handle) {
	return -1;
}



int seek2 (FILE2 handle, DWORD offset) {
	return -1;
}



int mkdir2 (char *pathname) {
	return -1;
}



int rmdir2 (char *pathname) {
	return -1;
}



int chdir2 (char *pathname) {
	return -1;
}



int getcwd2 (char *pathname, int size) {
	return -1;
}



DIR2 opendir2 (char *pathname) {
	return -1;
}



int readdir2 (DIR2 handle, DIRENT2 *dentry) {
	return -1;
}



int closedir2 (DIR2 handle) {
	return -1;
}



int ln2(char *linkname, char *filename) {
	return -1;
}

// -------------------------------------------------------------------------------------------------
// FUNÇÕES AUXILIARES
// -------------------------------------------------------------------------------------------------
//int t2fs_init();
//int update_on_disc(struct directory_entry* entry);
//int resolve_link(const struct directory_entry* link_entry, char* resolved_path, int size);
//int resolve_path(char* path, struct directory_entry* entry);
//int delete_file(struct directory_entry *entry);
//int is_empty(const struct directory_entry *file);
//int create(char *filename, fcb *file);

int is_handle_valid(int handle) {
    if(handle < 0) return -1;
    if(handle >= N_OPEN_FILES) return -1;
    return 0;
}

BYTE file_type(const struct directory_entry *file) {
    return file->record.TypeVal;
}

int is_file(const struct directory_entry *file) {
    return file_type(file) == TYPEVAL_REGULAR;
}

int is_dir(const struct directory_entry *file) {
    return file_type(file) == TYPEVAL_DIRETORIO;
}

int is_link(const struct directory_entry *file) {
    return file_type(file) == TYPEVAL_LINK;
}


int is_linkf(const struct directory_entry *file) {
    if(is_link(file) < 0) return -1;

    struct directory_entry entry;
    char resolved_path[100];

    if(resolve_link(file, resolved_path, 100) < 0) return -1;
    if(resolve_path(resolved_path, &entry) < 0) return -1;

    return is_file(&entry);
}

int is_linkd(const struct directory_entry *file) {
    if(is_link(file) < 0) return -1;

    struct directory_entry entry;
    char resolved_path[100];

    if(resolve_link(file, resolved_path, 100) < 0) return -1;
    if(resolve_path(resolved_path, &entry) < 0) return -1;

    return is_dir(&entry);
}

int exists(char *filepath) {
    struct directory_entry entry;
    return resolve_path(filepath, &entry);
}

int get_free_handle() {
    for(int i = 0; i < N_OPEN_FILES; i++) {
       if(open_files[i].is_valid == 0)
           return i;
    }

   return -1;
}


FILE2 insert(struct fcb file) {
   int i = get_free_handle();
   open_files[i] = file;
   return i;
}

int set_current_pointer(DWORD offset, struct fcb *file) {
    if(offset > file->dir_entry.record.bytesFileSize) return -1;

    if(offset == -1)
        offset = file->dir_entry.record.bytesFileSize;

    set_current_physical_cluster(offset, file);
    set_current_sector_on_cluster(offset, file);
    return -1;
}

int get_file(char *filename, struct fcb *file) {
    struct directory_entry entry;

    if(resolve_path(filename, &entry) < 0) return -1;

    file->dir_entry = entry;
    file->current_physical_cluster = entry.record.firstCluster;
    file->current_sector_on_cluster = 0;
    file->current_byte_on_sector = 0;
    file->num_bytes_read = 0;
    file->is_valid = 1;

    return 0;
}

DWORD get_current_physical_sector(const struct fcb *file) {
    return (file->current_physical_cluster - 1) * superblock.SectorsPerCluster + file->current_sector_on_cluster;
}

DWORD get_current_logical_sector(const struct fcb *file, DWORD sector) {
    return sector - (file->current_physical_cluster - 1) * superblock.SectorsPerCluster;
}

int next_cluster(struct fcb *file) {
    if(file->current_physical_cluster == 0) return -1;   // reserved
    if(file->current_physical_cluster == 1) return -1;   // reserved

    unsigned int next = fat[file->current_physical_cluster];

    if(next == BAD_CLUSTER) return -1;
    if(next == END_OF_FILE) return -1;

    file->current_physical_cluster = next;
    file->current_sector_on_cluster = 0;
    file->current_byte_on_sector = 0;
    return 0;
}

int set_current_physical_cluster(DWORD offset, struct fcb *file) {
    file->current_physical_cluster = file->dir_entry.record.firstCluster;

    for(int i = 0; i < floor(offset / CLUSTER_SIZE); i++)
        if(next_cluster(file) < 0) return -1;

    return 0;
}

int set_current_sector_on_cluster(DWORD offset, struct fcb *file) {
    int leftover = offset - floor((float)offset / CLUSTER_SIZE) * CLUSTER_SIZE;
    file->current_sector_on_cluster = floor(leftover / SECTOR_SIZE + 1) - 1;
    file->current_byte_on_sector = leftover;
    return 0;
}