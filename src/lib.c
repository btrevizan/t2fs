#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "../include/t2fs.h"
#include "../include/helpers.h"
#include "../include/apidisk.h"

#define CURRENT_DIR_SIZE 1024

static char first_run = 1;
static struct t2fs_superbloco superblock;
static struct fcb open_files[N_OPEN_FILES];
static struct fcb open_dirs[1];
static char current_dir[CURRENT_DIR_SIZE];
static unsigned int *fat;
static unsigned int CLUSTER_SIZE;

int identify2 (char *name, int size) {
	if(first_run == 1) {
        if(t2fs_init() < 0) return -1;
    }

	strncpy (name, "Bernardo Trevizan - 00285638\nEduarda Trindade - 00274709\nGabriel Haggstrom - 00228552", size);
	return 0;
}


FILE2 create2 (char *filename) {
    if(first_run == 1) {
        if(t2fs_init() < 0) return -1;
    }

    int i = get_free_handle();
    if(i < 0) return -1; // N_FILES_OPEN opened

    if(exists(filename) == 1) {
        i = open2(filename);
        if(is_handle_valid(i) < 0) return -1;

        truncate2(i);
        return i;
    }

    struct fcb file;
    file.dir_entry.record.TypeVal = TYPEVAL_REGULAR;
    if(create_file(filename, &file) < 0) return -1;

    open_files[i] = file;
    return i;
}


int delete2 (char *filename) {
    if(first_run == 1) {
        if(t2fs_init() < 0) return -1;
    }

    struct directory_entry dir_entry;

    if(resolve_path(filename, &dir_entry, NULL, 0) < 0) return -1;
    if(is_file(&dir_entry) < 0 || is_link(&dir_entry) < 0) return -1;

    return delete_file(&dir_entry);
}


FILE2 open2 (char *filename) {
    if(first_run == 1) {
        if(t2fs_init() < 0) return -1;
    }

    int i = get_free_handle();
    if(i < 0) return -1; // N_FILES_OPEN opened

    struct fcb file;
    if(get_file(filename, &file) < 0) return -1;

    open_files[i] = file;
    return i;
}


int close2 (FILE2 handle) {
    if(first_run == 1) {
        if(t2fs_init() < 0) return -1;
    }

	if(is_handle_valid(handle) < 0) return -1;
    if(update_on_disk(&open_files[handle].dir_entry) < 0) return -1;

	open_files[handle].is_valid = 0;
	return 0;
}


int read2 (FILE2 handle, char *buffer, int size) {
    if(first_run == 1) {
        if(t2fs_init() < 0) return -1;
    }

    if(is_handle_valid(handle) < 0) return -1;

    struct fcb file = open_files[handle];
    int result = read_file(&file, buffer, size);

    open_files[handle] = file;
    return result;
}


int write2 (FILE2 handle, char *buffer, int size) {
    if(first_run == 1) {
        if(t2fs_init() < 0) return -1;
    }

    if(is_handle_valid(handle) < 0) return -1;

    struct fcb file = open_files[handle];
    int result = write_file(&file, buffer, size);

    open_files[handle] = file;
    return result;
}


int truncate2 (FILE2 handle) {
    if(first_run == 1) {
        if(t2fs_init() < 0) return -1;
    }

    if(is_handle_valid(handle) < 0) return -1;
    struct fcb file = open_files[handle];

    unsigned int removed_sectors = 1;
    if(file.current_byte_on_sector > 0)
        removed_sectors = 0;

    DWORD curr_cluster = file.current_physical_cluster;
    DWORD curr_sector = file.current_sector_on_cluster;
    DWORD curr_byte = file.current_byte_on_sector;
    DWORD cluster = curr_cluster;

    while(next_sector(&file) == 0) {
        removed_sectors++;

        if(cluster != file.current_physical_cluster) {
            free_cluster(cluster);
            cluster = file.current_physical_cluster;
        }
    }

    free_cluster(cluster);

    DWORD last_byte = get_last_byte(file.dir_entry.record.bytesFileSize);
    file.dir_entry.record.bytesFileSize -= ((removed_sectors - 1) * SECTOR_SIZE + last_byte + 1 - (256 - curr_byte));
    file.dir_entry.record.clustersFileSize -= removed_sectors / superblock.SectorsPerCluster;

    file.current_physical_cluster = curr_cluster;
    file.current_sector_on_cluster = curr_sector - 1;
    file.current_byte_on_sector = SECTOR_SIZE - 1;

    if(file.current_byte_on_sector == 0) {
        if(file.current_sector_on_cluster == 0) {
            if(file.current_physical_cluster == file.dir_entry.record.firstCluster) {
                // Pointer is in the beginning of the file
                file.dir_entry.record.bytesFileSize = 0;
                file.dir_entry.record.clustersFileSize = 1;
                file.current_physical_cluster = file.dir_entry.record.firstCluster;
                file.current_sector_on_cluster = 0;
                file.current_byte_on_sector = 0;
            } else {
                // Pointer is in the beginning of a cluster that is not the first cluster
                prev_cluster(&file);
                file.current_sector_on_cluster = superblock.SectorsPerCluster - 1;
            }
        }
    } else {
        // Pointer is in the middle of a sector
        file.current_sector_on_cluster = curr_sector;
        file.current_byte_on_sector = curr_byte - 1;
    }

    update_on_disk(&file.dir_entry);
    open_files[handle] = file;
    return 0;
}


int seek2 (FILE2 handle, DWORD offset) {
    if(first_run == 1) {
        if(t2fs_init() < 0) return -1;
    }

	if (is_handle_valid(handle) < 0) return -1;

	struct fcb file = open_files[handle];
	if (set_current_pointer(offset, &file) < 0) return -1;

	open_files[handle] = file;
	
	return 0;
}


int mkdir2 (char *pathname) {
    if(first_run == 1) {
        if(t2fs_init() < 0) return -1;
    }

    if(open_dirs[0].is_valid == 1) return -1;

    struct fcb file;
    file.dir_entry.record.TypeVal = TYPEVAL_DIRETORIO;

    unsigned int cluster = create_file(pathname, &file);
    if(cluster < 0) return -1;

    // Fill directory with invalid entries
    struct t2fs_record invalid_record;
    invalid_record.TypeVal = TYPEVAL_INVALIDO;

    unsigned int record_size = (unsigned int) sizeof(struct t2fs_record);
    unsigned int n_records = (superblock.SectorsPerCluster * SECTOR_SIZE) / record_size;
    unsigned int n_records_per_sector = n_records / superblock.SectorsPerCluster;
    unsigned char records[SECTOR_SIZE];

    for(int i = 0; i < n_records_per_sector; i++)
        memcpy((records + i * record_size), &invalid_record, record_size);

    unsigned int sector = get_current_physical_sector(&file);
    for(int i = 0; i < superblock.SectorsPerCluster; i++)
        write_sector(sector + i, records);

    // Create the current and parent entries
    struct directory_entry current;
    struct directory_entry parent;

    current.record.TypeVal = TYPEVAL_DIRETORIO;
    strcpy(current.record.name, ".");
    current.record.bytesFileSize = SECTOR_SIZE * superblock.SectorsPerCluster;
    current.record.clustersFileSize = 1;
    current.record.firstCluster = cluster;

    add_entry(&current, &file.dir_entry);

    // Get parent first cluster
    struct directory_entry entry;
    char *parent_pathname = (char *) malloc(strlen(pathname) + strlen("/.."));
    get_parent_filepath(pathname, parent_pathname);

    if(resolve_path(parent_pathname, &entry, NULL, 0) < 0) return -1;

    parent.record.TypeVal = TYPEVAL_DIRETORIO;
    strcpy(parent.record.name, "..");
    parent.record.bytesFileSize = SECTOR_SIZE * superblock.SectorsPerCluster;
    parent.record.clustersFileSize = 1;
    parent.record.firstCluster = entry.record.firstCluster;

    add_entry(&parent, &file.dir_entry);
    return 0;
}


int rmdir2 (char *pathname) {
    if(first_run == 1) {
        if(t2fs_init() < 0) return -1;
    }

    struct directory_entry entry;

    if(resolve_path(pathname, &entry, NULL, 0) < 0) return -1;
    if(!is_dir(&entry)) return -1;
    if(!is_empty(&entry)) return -1;
    if(is_linkf(&entry)) return -1;

    return delete_file(&entry);
}


int chdir2 (char *pathname) {
    if(first_run == 1) {
        if(t2fs_init() < 0) return -1;
    }

    struct directory_entry entry;
    if(resolve(pathname, &entry, current_dir, CURRENT_DIR_SIZE) < 0) return -1;
    if(!is_dir(&entry)) return -1;

    return 0;
}


int getcwd2 (char *pathname, int size) {
    if(first_run == 1) {
        if(t2fs_init() < 0) return -1;
    }

    if(strlen(current_dir) > size) return -1;
    strncpy(pathname, current_dir, size);

    return 0;
}


DIR2 opendir2 (char *pathname) {
    if(first_run == 1) {
        if(t2fs_init() < 0) return -1;
    }

    DIR2 handle = 0;

    if(open_dirs[handle].is_valid == 1) return -1;

    struct fcb file;
    if(get_file(pathname, &file) < 0) return -1;
    if(is_dir(&file.dir_entry) < 0) return -1;

    open_dirs[0] = file;
    return 0;
}


int readdir2 (DIR2 handle, DIRENT2 *dentry) {
    if(first_run == 1) {
        if(t2fs_init() < 0) return -1;
    }

    if(handle != 0) return -1;
    struct fcb file = open_dirs[handle];

    char *buffer = (char *) malloc(sizeof(struct t2fs_record));
    if(!buffer) return -1;

    if(read_file(&file, buffer, sizeof(struct t2fs_record)) < 0) return -1;

    struct t2fs_record record;

    memcpy(&record, buffer, sizeof(struct t2fs_record));

    strcpy(dentry->name, record.name);
    dentry->fileType = record.TypeVal;
    dentry->fileSize = record.bytesFileSize;

    return 0;
}


int closedir2 (DIR2 handle) {
    if(first_run == 1) {
        if(t2fs_init() < 0) return -1;
    }

	if (handle != 0) return -1;

	open_dirs[handle].is_valid = 0;
	return 0;
}


int ln2(char *linkname, char *filename) {
	if(first_run == 1) {
        if(t2fs_init() < 0) return -1;
    }
	
	if (strlen(filename) > CLUSTER_SIZE) return -1;

	struct fcb file;
	file.dir_entry.record.TypeVal = TYPEVAL_LINK;

	if (create_file(linkname, &file) < 0) return -1;
	if (write_file(&file, filename, (int)strlen(filename)) < 0) return -1;

	return 0;
}

// -------------------------------------------------------------------------------------------------
// HELPERS
// -------------------------------------------------------------------------------------------------

int t2fs_init() {
    if(load_superblock() < 0) return -1;
    if(load_fat() < 0) return -1;

    // Set current directory
    strcpy(current_dir, "/");

    // Define CLUSTER_SIZE
    CLUSTER_SIZE = superblock.SectorsPerCluster * SECTOR_SIZE;

    // Open an invalid dir
    open_dirs[0].is_valid = 0;

    first_run = 0;
    return 0;
}


int update_on_disk(struct directory_entry* entry) {
    struct t2fs_record entries[SECTOR_SIZE / sizeof(struct t2fs_record)];
    int entry_on_sector;

    if (read_sector(entry->sector, (unsigned char *) entries) < 0) return -1;

    entry_on_sector = entry->byte_on_sector / sizeof(struct t2fs_record);
    entries[entry_on_sector] = entry->record;

    if (write_sector(entry->sector, (unsigned char *) entries) < 0) return -1;

    return 0;
}


int resolve_link(const struct directory_entry* link_entry, struct directory_entry* resolved_entry, char *resolved_path, int size) {
    char path[CLUSTER_SIZE];
    struct fcb file;

    while (1) {
        create_fcb(link_entry, &file);
        read_file(&file, path, CLUSTER_SIZE);
        if (resolve_path(path, resolved_entry, resolved_path, size) < 0) {
            return -1;
        }
        if (!is_link(resolved_entry)) break;
        link_entry = resolved_entry;
    }

    return 0;
}


int resolve_path(char* path, struct directory_entry* resolved_entry, char *resolved_path, int size) {
    char *path_copy;
    char *name;
    int i;

    if (resolved_path != NULL) {
        resolved_path[0] = '\0';
    }

    if (path[0] == '/') {
        // Caminho absoluto
        // Cria uma entry correspondente à raiz
        resolved_entry->record.TypeVal = TYPEVAL_DIRETORIO;
        resolved_entry->record.bytesFileSize = CLUSTER_SIZE;
        resolved_entry->record.clustersFileSize = 1;
        resolved_entry->record.firstCluster = superblock.RootDirCluster;
    }
    else {
        // Caminho relativo
        resolve_path(current_dir, resolved_entry, resolved_path, size);
    }

    path_copy = malloc(strlen(path) + 1);
    strcpy(path_copy, path);
    path = path_copy;

    name = strtok(path, "/");

    while (name) {
        if (search_entry(name, resolved_entry, resolved_entry) < 0) return -1;

        if (resolved_path != NULL) {
            i = strlen(resolved_path);

            if (strcmp(name, "..") == 0) {
                // Remove o último componente
                while (resolved_path[i] != '/') i--;
                resolved_path[i] = '\0';
            }
            else if (strcmp(name, ".") != 0) {
                if (i + strlen(name) + 2 > size) return -1;

                strcat(resolved_path, "/");
                strcat(resolved_path, name);
            }
        }

        name = strtok(NULL, "/");
        if (name == NULL) return 0;

        if (is_link(resolved_entry)) {
            if (resolve_link(resolved_entry, resolved_entry, resolved_path, size) < 0) return -1;
        }

        if (!is_dir(resolved_entry)) return -1;
    }

    return 0;
}


int load_superblock() {
    if(read_sector(0, (unsigned char *) (&superblock)) < 0) return -1;
    return 0;
}


int load_fat() {
    unsigned int fat_size = fat_bytes_size();
    unsigned int fat_n_sectors = fat_size / SECTOR_SIZE;
    unsigned int sector = superblock.pFATSectorStart;
    unsigned int i = 0;

    fat = malloc(fat_size);
    if(!fat) return -1;

    while(fat_n_sectors > 0) {
        if(read_sector(sector, (unsigned char *) fat + (SECTOR_SIZE * i)) < 0) return -1;

        i++;
        sector++;
        fat_n_sectors--;
    }

    return 0;
}


int save_fat(unsigned int cluster) {
    unsigned int sector = superblock.pFATSectorStart + (cluster / (SECTOR_SIZE / 4));
    if(write_sector(sector, (unsigned char *)(fat + sector * SECTOR_SIZE)) < 0) return -1;
    return 0;
}


int fat_bytes_size() {
    return ((superblock.NofSectors - superblock.DataSectorStart) / superblock.SectorsPerCluster) * 4;
}


int resolve(char *path, struct directory_entry *entry, char *resolved_path, int size) {
    if(resolve_path(path, entry, resolved_path, size) < 0) return -1;

    if(is_link(entry)) {
        struct directory_entry resolved_entry;
        if(resolve_link((struct directory_entry *) entry, &resolved_entry, resolved_path, size) < 0) return -1;
        *(entry) = resolved_entry;
    }

    return 0;
}


int create_file(char *filename, struct fcb *file) {
    struct directory_entry entry;
    if(resolve_path(filename, &entry, NULL, 0) < 0) return -1;
    if(exists(filename)) return -1;

    unsigned int cluster = alloc_cluster();
    if(cluster < 0) return -1;

    char *name = (char *) malloc(MAX_FILE_NAME_SIZE);
    get_file_name(filename, name);

    strncpy(file->dir_entry.record.name, name, strlen(file->dir_entry.record.name));
    file->dir_entry.record.bytesFileSize = 0;
    file->dir_entry.record.clustersFileSize = 1;
    file->dir_entry.record.firstCluster = cluster;

    struct fcb parent;
    char *parent_pathname = (char *) malloc(strlen(filename) + strlen("/.."));
    get_parent_filepath(filename, parent_pathname);

    if(get_file(parent_pathname, &parent) < 0) return -1;
    if(add_entry(&file->dir_entry, &parent.dir_entry) < 0) return -1;

    file->is_valid = 1;
    file->current_physical_cluster = cluster;
    file->current_sector_on_cluster = 0;
    file->current_byte_on_sector = 0;

    file->dir_entry.sector = get_current_physical_sector(file);
    file->dir_entry.byte_on_sector = 0;

    return cluster;
}


int read_file(struct fcb *file, char *buffer, int size) {
    unsigned int sector = get_current_physical_sector(file);
    unsigned char aux_buffer[SECTOR_SIZE];

    // Read current sector and adjust strncpy to start on current pointer
    if(read_sector(sector, aux_buffer) < 0) return -1;

    unsigned int n = SECTOR_SIZE - file->current_byte_on_sector;
    if(n > size) n = size;

    unsigned int bytes_read = n;
    strncpy(buffer, (const char *)(aux_buffer + file->current_byte_on_sector), n);

    // Read the rest, sector by sector
    while(bytes_read < size) {
        if(next_sector(file) < 0) { // EOF
            n = get_last_byte(file->dir_entry.record.bytesFileSize);
            strncpy((buffer + bytes_read), (const char *)aux_buffer, n);
            bytes_read += n;
            break;
        }

        sector = get_current_physical_sector(file);
        if(read_sector(sector, aux_buffer) < 0) break;

        n = SECTOR_SIZE;
        if(bytes_read + n > size) n = size - bytes_read;

        strncpy((buffer + bytes_read), (const char *)aux_buffer, n);
        bytes_read += n;

        if(n != SECTOR_SIZE) break;
    }

    file->current_byte_on_sector = n % SECTOR_SIZE;
    strncpy(file->current_sector_data, (const char *)aux_buffer, n);
    file->num_bytes_read = n;

    return bytes_read;
}


int write_file(struct fcb *file, char *buffer, int size) {
    unsigned int sector = get_current_physical_sector(file);
    unsigned char aux_buffer[SECTOR_SIZE];

    if(read_sector(sector, aux_buffer) < 0) return -1;

    unsigned int n = SECTOR_SIZE - file->current_byte_on_sector;
    if(n > size) n = size;

    unsigned int bytes_written = n;

    strncpy((char *) (aux_buffer + file->current_byte_on_sector), (const char *)buffer, n);
    if(write_sector(sector, aux_buffer) < 0) return -1;

    // Write the rest, sector by sector
    while(bytes_written < size) {
        if(next_sector(file) < 0) { // EOF
            if(add_cluster(file) < 0) break;
        } else {
            sector = get_current_physical_sector(file);

            n = SECTOR_SIZE;
            if(bytes_written + n > size) {
                n = size - bytes_written;
                if(read_sector(sector, aux_buffer) < 0) break;
            }

            strncpy((char *)aux_buffer, (const char *)(buffer + bytes_written), n);
            if(write_sector(sector, aux_buffer) < 0) break;

            bytes_written += n;
            if(n != SECTOR_SIZE) break;
        }
    }

    file->current_byte_on_sector = n % SECTOR_SIZE;
    return bytes_written;
}


int delete_file(struct directory_entry *entry) {
    if(remove_entry(entry) < 0) return -1;

    unsigned int cluster = entry->record.firstCluster;
    unsigned int prev_cluster;

    while(fat[cluster] != END_OF_FILE) {
        prev_cluster = cluster;
        cluster = fat[cluster];

        dealloc_cluster(prev_cluster);
    }

    dealloc_cluster(cluster);
    return 0;
}


int get_file(char *filename, struct fcb *file) {
    struct directory_entry entry;
    if(resolve(filename, &entry, NULL, 0) < 0) return -1;
    return create_fcb(&entry, file);
}


int create_fcb(const struct directory_entry *entry, struct fcb *file) {
    file->dir_entry = *entry;
    file->current_physical_cluster = entry->record.firstCluster;
    file->current_sector_on_cluster = 0;
    file->current_byte_on_sector = 0;
    file->num_bytes_read = 0;
    strcpy(file->current_sector_data, (const char *)"");
    file->is_valid = 1;
    return 0;
}


int get_file_name(char *filepath, char *filename) {
    char *dir = strtok(filepath, "/");

    while(dir) {
        strcpy(filename, dir);
        dir = strtok(NULL, "/");
    }

    return 0;
}


int get_parent_filepath(char *filepath, char *parent_pathname) {
    strncpy(parent_pathname, filepath, strlen(filepath) - 1);
    strcat(parent_pathname, "/..");
    return 0;
}


int add_entry(struct directory_entry *entry, struct directory_entry *dir) {
    if(!is_dir(dir)) return -1;

    unsigned char buffer[SECTOR_SIZE];
    unsigned int sector = dir->sector;

    struct t2fs_record aux_record;
    unsigned int record_size = (unsigned int) sizeof(struct t2fs_record);

    for(int i = 0; i < superblock.SectorsPerCluster; i++) {
        read_sector(sector + i, buffer);

        for(int j = 0; j < SECTOR_SIZE / record_size; j++) {
            if(!(i == 0 && (j == 0 || j == 1))) {
                memcpy(&aux_record, (buffer + j * record_size), record_size);

                if(aux_record.TypeVal == TYPEVAL_INVALIDO) {
                    memcpy((buffer + j * record_size), &entry->record, record_size);

                    entry->sector = sector + i;
                    entry->byte_on_sector = j * record_size;

                    if(write_sector(sector + i, buffer) < 0) return -1;
                    return 0;
                }
            }
        }
    }

    return -1;
}


int remove_entry(struct directory_entry *entry) {
    unsigned char buffer[SECTOR_SIZE];
    if(read_sector(entry->sector, buffer) < 0) return -1;

    struct t2fs_record invalid;
    invalid.TypeVal = TYPEVAL_INVALIDO;

    memcpy((buffer + entry->byte_on_sector), &invalid, sizeof(struct t2fs_record));
    if(write_sector(entry->sector, buffer) < 0) return -1;

    return 0;
}


int search_entry(char *name, struct directory_entry *dir_entry, struct directory_entry *entry) {
    int num_entries = CLUSTER_SIZE / sizeof(struct t2fs_record);
    struct t2fs_record entries[num_entries];
    struct fcb dir_file;

    create_fcb(dir_entry, &dir_file);

    read_file(&dir_file, (char *) entries, CLUSTER_SIZE);

    int i;
    for (i = 0; i < num_entries; i++) {
        if (strcmp(entries[i].name, name) == 0) break;
    }
    if (i == num_entries) return -1;

    entry->record = entries[i];

    entry->sector = superblock.DataSectorStart + (dir_entry->record.firstCluster * superblock.SectorsPerCluster) + (i * sizeof(struct t2fs_record)) / SECTOR_SIZE;

    entry->byte_on_sector = (i * sizeof(struct t2fs_record)) % SECTOR_SIZE;

    return 0;
}


int is_handle_valid(int handle) {
    if(handle < 0) return -1;
    if(handle >= N_OPEN_FILES) return -1;
    return 0;
}


int get_free_handle() {
    for(int i = 0; i < N_OPEN_FILES; i++) {
        if(open_files[i].is_valid == 0)
            return i;
    }

    return -1;
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
    if(!is_link(file)) return -1;

    struct directory_entry entry;
    if(resolve_link(file, &entry, NULL, 0) < 0) return -1;
    return is_file(&entry);
}


int is_linkd(const struct directory_entry *file) {
    if(!is_link(file)) return -1;

    struct directory_entry entry;
    if(resolve_link(file, &entry, NULL, 0) < 0) return -1;
    return is_dir(&entry);
}


int is_empty(const struct directory_entry *file) {
    if(is_dir(file) < 0) return -1;

    struct t2fs_record record;
    unsigned int record_size = (unsigned int) sizeof(struct t2fs_record);
    unsigned int sector = file->sector;
    unsigned char buffer[SECTOR_SIZE];

    for(int i = 0; i < superblock.SectorsPerCluster; i++) {
        read_sector(sector + i, buffer);

        for(int j = 0; j < SECTOR_SIZE / record_size; j++) {
            if(!(i == 0 && (j == 0 || j == 1))) {
                memcpy(&record, (buffer + j * record_size), record_size);
                if(record.TypeVal == TYPEVAL_DIRETORIO || record.TypeVal == TYPEVAL_REGULAR || record.TypeVal == TYPEVAL_LINK)
                    return 0;
            }
        }
    }

    return 1;
}


int exists(char *filepath) {
    struct directory_entry entry;
    if(resolve_path(filepath, &entry, NULL, 0) == 0) return 1;
    return 0;
}


int next_sector(struct fcb *file) {
    DWORD sector = file->current_sector_on_cluster;

    sector++;
    if(sector % superblock.SectorsPerCluster == 0)
        if(next_cluster(file) < 0) return -1; // EOF

    file->current_sector_on_cluster = sector;
    return 0;
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


int prev_cluster(struct fcb *file) {
    DWORD last_cluster_visited;
    DWORD curr_cluster = file->current_physical_cluster;
    file->current_physical_cluster = file->dir_entry.record.firstCluster;

    while(file->current_physical_cluster != curr_cluster) {
        last_cluster_visited = file->current_physical_cluster;
        next_cluster(file);
    }

    file->current_physical_cluster = last_cluster_visited;
    file->current_sector_on_cluster = 0;
    file->current_byte_on_sector = 0;

    return 0;
}


int free_cluster(DWORD cluster) {
    int n_fat_entries = superblock.NofSectors / superblock.SectorsPerCluster;

    if(cluster < 0) return -1;
    if(cluster >= n_fat_entries) return -1;

    fat[cluster] = FREE_CLUSTER;
    return 0;
}


unsigned int alloc_cluster() {
    unsigned int cluster = 0;
    int n_clusters = fat_bytes_size() / 4;

    while(fat[cluster] != FREE_CLUSTER && cluster < n_clusters)
        cluster++;

    if(cluster == n_clusters) return -1;

    fat[cluster] = END_OF_FILE;

    if(save_fat(cluster) < 0) return -1;
    return cluster;
}


int dealloc_cluster(unsigned int cluster) {
    if(fat[cluster] == BAD_CLUSTER) return -1;
    fat[cluster] = FREE_CLUSTER;
    return save_fat(cluster);
}


unsigned int add_cluster(struct fcb *file) {
    unsigned int cluster = file->current_physical_cluster;
    while(fat[cluster] != END_OF_FILE) cluster = fat[cluster];

    unsigned int new_cluster = alloc_cluster();
    if(new_cluster < 0) return -1;

    fat[cluster] = new_cluster;

    if(save_fat(cluster) < 0) return -1;
    return new_cluster;
}

int set_current_pointer(DWORD offset, struct fcb *file) {
    if(offset > file->dir_entry.record.bytesFileSize) return -1;

    if(offset == -1)
        offset = file->dir_entry.record.bytesFileSize;

    set_current_physical_cluster(offset, file);
    set_current_sector_on_cluster(offset, file);
    return -1;
}


DWORD get_current_physical_sector(const struct fcb *file) {
    return superblock.DataSectorStart + file->current_physical_cluster * superblock.SectorsPerCluster + file->current_sector_on_cluster;
}


DWORD get_current_logical_sector(const struct fcb *file, DWORD sector) {
    return sector - (superblock.DataSectorStart + file->current_physical_cluster * superblock.SectorsPerCluster);
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


DWORD get_last_byte(DWORD file_size) {
    DWORD last_byte = (file_size - (file_size / SECTOR_SIZE) * SECTOR_SIZE) - 1;
    if(last_byte == -1) last_byte = SECTOR_SIZE - 1;
    return last_byte;
}
