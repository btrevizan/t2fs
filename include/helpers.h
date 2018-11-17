#include "t2fs.h"

#define N_OPEN_FILES 10
#define FREE_CLUSTER 0x00000000
#define BAD_CLUSTER 0xFFFFFFFE
#define END_OF_FILE 0xFFFFFFFF

struct directory_entry {
    struct t2fs_record record;
    DWORD sector;
    DWORD byte_on_sector;
};

struct fcb {
    struct directory_entry dir_entry;
    char is_valid;

    // current pointer broken into three components
    DWORD current_physical_cluster;     // 0 ... C - 1
    DWORD current_sector_on_cluster;    // 0 ... SectorsPerCluster - 1
    DWORD current_byte_on_sector;       // 0 ... SECTOR_SIZE - 1

    char current_sector_data[SECTOR_SIZE];     // Last read sector
    DWORD num_bytes_read;               // Number of bytes read from disc and copied to current_sector_data.
    // In the last sector of the file, it can be less than SECTOR_SIZE.
};

int t2fs_init();
int load_superblock();
int load_fat();

int fat_bytes_size();

int update_on_disk(struct directory_entry* entry);

int resolve_link(const struct directory_entry* link_entry, char* resolved_path, int size);
int resolve_path(char* path, struct directory_entry* entry);

int create_file(char *filename, struct fcb *file);
int read_file(struct fcb *file, char *buffer, int size);
int write_file(struct fcb *file, char *buffer, int size);
int delete_file(struct directory_entry *entry);
int get_file(char *filename, struct fcb *file);
int create_fcb(struct directory_entry *entry, struct fcb *file);
int get_file_name(char *filepath, char *filename);
int get_parent_filepath(char *filepath, char *parent_filepath);

int add_entry(struct t2fs_record *record, struct directory_entry *dir);
int remove_entry(struct directory_entry *entry);

int is_handle_valid(int handle);
int get_free_handle();

BYTE file_type(const struct directory_entry *file);
int is_file(const struct directory_entry *file);
int is_dir(const struct directory_entry *file);
int is_link(const struct directory_entry *file);
int is_linkf(const struct directory_entry *file);
int is_linkd(const struct directory_entry *file);
int is_empty(const struct directory_entry *file);
int exists(char *filepath);

int next_sector(struct fcb *file);
int next_cluster(struct fcb *file);
int prev_cluster(struct fcb *file);
int free_cluster(DWORD cluster);
unsigned int alloc_cluster();
unsigned int add_cluster(struct fcb *file);

int set_current_pointer(DWORD offset, struct fcb *file);
DWORD get_current_physical_sector(const struct fcb *file);
DWORD get_current_logical_sector(const struct fcb *file, DWORD sector);
int set_current_physical_cluster(DWORD offset, struct fcb *file);
int set_current_sector_on_cluster(DWORD offset, struct fcb *file);
DWORD get_last_byte(DWORD fileSize);