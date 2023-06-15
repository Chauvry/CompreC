#include <stdio.h>
#include <zip.h>

int main() {
    const char* zip_file_path = "secretV2.zip";
    struct zip* zip_file = zip_open(zip_file_path, ZIP_RDONLY, NULL);
    if (!zip_file) {
        printf("Failed to open the zip file.\n");
        return 1;
    }

    int num_files = zip_get_num_entries(zip_file, 0);
    if (num_files == -1) {
        printf("Failed to get the number of files in the zip archive.\n");
        zip_close(zip_file);
        return 1;
    }

    printf("Files in the ZIP archive:\n");

    for (int i = 0; i < num_files; i++) {
        const char* file_name = zip_get_name(zip_file, i, 0);
        printf("%s\n", file_name);
    }

    zip_close(zip_file);

    return 0;
}
