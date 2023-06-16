#include "gestionZip.h"

struct zip* open_zip_file(const char* zip_file_path) {
    struct zip* zip_file = zip_open(zip_file_path, ZIP_RDONLY, NULL);
    if (!zip_file) {
        printf("Failed to open the zip file.\n");
        return NULL;
    }

    return zip_file;
}

int get_num_files(struct zip* zip_file) {
    int num_files = zip_get_num_entries(zip_file, 0);
    if (num_files == -1) {
        printf("Failed to get the number of files in the zip archive.\n");
        return -1;
    }

    return num_files;
}

const char* get_file_name(struct zip* zip_file, int index) {
    return zip_get_name(zip_file, index, 0);
}

void close_zip_file(struct zip* zip_file) {
    zip_close(zip_file);
}


