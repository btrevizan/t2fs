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
static char *current_dir;
static unsigned int *fat;
static unsigned int CLUSTER_SIZE;

int identify2 (char *name, int size) {
	if(first_run == 1)
	    if(t2fs_init() < 0) return -1;

	strncpy (name, "Bernardo Trevizan - 00285638\nEduarda Trindade - 00274709\nGabriel Haggstrom - 00228552", size);
	return 0;
}

FILE2 create2 (char *filename) {
    if(first_run == 1)
        if(t2fs_init() < 0) return -1;

    int i = get_free_handle();
    if(i < 0) return -1; // N_FILES_OPEN opened

    if(exists(filename) == 0) {
        i = open2(filename);
        if(is_handle_valid(i) < 0) return -1;

        truncate2(i);
        return i;
    }

    struct fcb file;
    file.dir_entry.record.TypeVal = TYPEVAL_REGULAR;
    if(create(filename, &file) < 0) return -1;

    open_files[i] = file;
    return i;
}



int delete2 (char *filename) {
    if(first_run == 1)
        if(t2fs_init() < 0) return -1;

    struct directory_entry dir_entry;

    if(resolve_path(filename, &dir_entry) < 0) return -1;
    if(is_file(&dir_entry) < 0 || is_link(&dir_entry) < 0) return -1;

    return delete_file(&dir_entry);
}



FILE2 open2 (char *filename) {
    if(first_run == 1)
        if(t2fs_init() < 0) return -1;

    int i = get_free_handle();
    if(i < 0) return -1; // N_FILES_OPEN opened

    struct fcb file;
    if(get_file(filename, &file) < 0) return -1;

    open_files[i] = file;
    return i;
}

int close2 (FILE2 handle) {
    if(first_run == 1)
        if(t2fs_init() < 0) return -1;

	if(is_handle_valid(handle) < 0) return -1;
    if(update_on_disc(&open_files[handle].dir_entry) < 0) return -1;

	open_files[handle].is_valid = 0;
	return 0;
}

int read2 (FILE2 handle, char *buffer, int size) {
    if(first_run == 1)
        if(t2fs_init() < 0) return -1;

    if(is_handle_valid(handle) < 0) return -1;

    struct fcb file = open_files[handle];
    if(size > file.dir_entry.record.bytesFileSize) return -1;

    unsigned int sector = get_current_physical_sector(&file);
    unsigned char aux_buffer[SECTOR_SIZE];

    // Read current sector and adjust strncpy to start on current pointer
    if(read_sector(sector, aux_buffer) < 0) return -1;

    unsigned int n = SECTOR_SIZE - file.current_byte_on_sector;
    if(n > size) n = size;

    unsigned int bytes_read = n;
    strncpy(buffer, (const char *)(aux_buffer + file.current_byte_on_sector), n);

    // Read the rest, sector by sector
    while(bytes_read < size) {
        if(read_sector(sector, aux_buffer) < 0) break;

        sector++;
        if(sector % superblock.SectorsPerCluster == 0) {

            // EOF
            if(next_cluster(&file) < 0) {
                n = file.dir_entry.record.bytesFileSize - ((file.dir_entry.record.bytesFileSize / SECTOR_SIZE) * SECTOR_SIZE);
                strncpy((buffer + bytes_read), (const char *)aux_buffer, n);
                bytes_read += n;
                sector--;
                break;
            }

            sector = get_current_physical_sector(&file);
        }

        n = SECTOR_SIZE;
        if(bytes_read + n > size) n = size - bytes_read;

        strncpy((buffer + bytes_read), (const char *)aux_buffer, n);
        bytes_read += n;

        if(n != SECTOR_SIZE) break;
    }

    file.current_byte_on_sector = n % SECTOR_SIZE;
    file.current_sector_on_cluster = get_current_logical_sector(&file, sector);
    strncpy(file.current_sector_data, (const char *)aux_buffer, n);
    file.num_bytes_read = n;

    open_files[handle] = file;
    return bytes_read;

}



int write2 (FILE2 handle, char *buffer, int size) {
    if(first_run == 1)
        if(t2fs_init() < 0) return -1;

	return -1;
}



int truncate2 (FILE2 handle) {
    if(first_run == 1)
        if(t2fs_init() < 0) return -1;

	return -1;
}



int seek2 (FILE2 handle, DWORD offset) {
    if(first_run == 1)
        if(t2fs_init() < 0) return -1;

	if (is_handle_valid(handle) < 0) return -1;

	struct fcb file = open_files[handle];
	if (set_current_pointer(offset, &file) < 0) return -1;

	open_files[handle] = file;
	
	return 0;
}



int mkdir2 (char *pathname) {
    if(first_run == 1)
        if(t2fs_init() < 0) return -1;

	return -1;
}



int rmdir2 (char *pathname) {
    if(first_run == 1)
        if(t2fs_init() < 0) return -1;

	return -1;
}



int chdir2 (char *pathname) {
    if(first_run == 1)
        if(t2fs_init() < 0) return -1;

	return -1;
}



int getcwd2 (char *pathname, int size) {
    if(first_run == 1)
        if(t2fs_init() < 0) return -1;

	return -1;
}



DIR2 opendir2 (char *pathname) {
    if(first_run == 1)
        if(t2fs_init() < 0) return -1;

	return -1;
}



int readdir2 (DIR2 handle, DIRENT2 *dentry) {
    if(first_run == 1)
        if(t2fs_init() < 0) return -1;

	return -1;
}



int closedir2 (DIR2 handle) {
    if(first_run == 1)
        if(t2fs_init() < 0) return -1;

	if (is_handle_valid(handle) < 0) return -1;

	open_dirs[handle].is_valid = 0;
	return 0;
}


int ln2(char *linkname, char *filename) {
	if(first_run == 1)
	    if(t2fs_init() < 0) return -1;
	
	if (strlen(filename) > CLUSTER_SIZE) return -1;

	struct fcb file;
	file.dir_entry.record.TypeVal = TYPEVAL_LINK;

	if (create(linkname, &file) < 0) return -1;
	if (write(&file, filename, (int)strlen(filename)) < 0) return -1;

	return 0;
}

// -------------------------------------------------------------------------------------------------
// HELPERS
// -------------------------------------------------------------------------------------------------
//int update_on_disc(struct directory_entry* entry);
//int resolve_link(const struct directory_entry* link_entry, char* resolved_path, int size);
//int resolve_path(char* path, struct directory_entry* entry);
//int delete_file(struct directory_entry *entry);
//int is_empty(const struct directory_entry *file);
//int create(char *filename, fcb *file);
//int write(struct fcb *file, char *content, int size);

int t2fs_init() {
    if(load_superblock() < 0) return -1;
    if(load_fat() < 0) return -1;

    // Set current directory
    current_dir = (char *) malloc(sizeof(char) * 2);
    if(!current_dir) return -1;
    strcpy(current_dir, "/");

    // Define CLUSTER_SIZE
    CLUSTER_SIZE = superblock.SectorsPerCluster * SECTOR_SIZE;

    first_run = 0;
    return 0;
}

int load_superblock() {
    unsigned char buffer[SECTOR_SIZE];
    if(read_sector(0, buffer) < 0) return -1;

    superblock = *((struct t2fs_superbloco*) buffer);
    return 0;
}

int load_fat() {
    unsigned int fat_size = (superblock.NofSectors / superblock.SectorsPerCluster) * 4;
    unsigned int fat_n_sectors = fat_size / SECTOR_SIZE;
    unsigned int sector = superblock.pFATSectorStart;
    unsigned char buffer[SECTOR_SIZE];
    unsigned int i = 0;

    fat = malloc(fat_size);
    if(!fat) return -1;

    while(fat_n_sectors > 0) {
        if(read_sector(sector, buffer) < 0) return -1;

        // TODO: fix the line above, because it is not right
        *(fat + (SECTOR_SIZE * i) / 4) = *(buffer);

        i++;
        sector++;
        fat_n_sectors--;
    }

    return 0;
}

int is_handle_valid(int handle) {
    if(handle < 0) return -1;
    if(handle >= N_OPEN_FILES) return -1;
    if(open_files[handle].is_valid < 0) return -1;
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

    for(int i = 0; i < (offset / CLUSTER_SIZE); i++)
        if(next_cluster(file) < 0) return -1;

    return 0;
}

int set_current_sector_on_cluster(DWORD offset, struct fcb *file) {
    DWORD leftover = offset - (offset / CLUSTER_SIZE) * CLUSTER_SIZE;
    file->current_sector_on_cluster = (leftover / SECTOR_SIZE + 1) - 1;
    file->current_byte_on_sector = leftover;
    return 0;
}