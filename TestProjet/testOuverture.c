#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <zip.h>

void browse_zip_file(const char* file_path) {
    struct zip* zip_file = zip_open(file_path, ZIP_RDONLY, NULL);
    if (!zip_file) {
        printf("Failed to open the zip file.\n");
        return;
    }

    int num_files = zip_get_num_entries(zip_file, 0);
    if (num_files == -1) {
        printf("Failed to get the number of files in the zip archive.\n");
        zip_close(zip_file);
        return;
    }

    printf("Files in the ZIP archive:\n");

    for (int i = 0; i < num_files; i++) {
        const char* file_name = zip_get_name(zip_file, i, 0);
        printf("%s\n", file_name);
    }

    zip_close(zip_file);
}

int main(int argc, char* argv[]) {
    int option;
    char* file_path = NULL;

    while ((option = getopt(argc, argv, "ho:")) != -1) {
        switch (option) {
            case 'h':
                // print_help();
                return 0;
            case 'o':
                file_path = optarg;
                break;
            default:
                printf("Invalid option. Use -h or --help to see the available options.\n");
                return 1;
        }
    }

    if (file_path == NULL) {
        printf("Missing file path. Use -h or --help to see the available options.\n");
        return 1;
    }

    browse_zip_file(file_path);

    return 0;
}
