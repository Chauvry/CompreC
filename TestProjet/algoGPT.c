#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <zip.h>
#include <ncurses.h>
#include <string.h>

void print_help() {
    printf("Usage: ./zip [OPTION]... [FILE]...\n");
    printf("Options:\n");
    printf(" -h, --help\t\tShow this help\n");
    printf(" -o, --open\t\tOpen a zip file for browsing\n");
    printf(" -b, --bruteforce\tTry to bruteforce the password\n");
    printf(" -d, --dictionary=FILE\tTry to bruteforce the password with a dictionary\n");
    printf(" -p, --password=PASSWORD\tUse this password\n");
    printf(" -e, --extract=FILE\tExtract this file\n");
    printf(" -i, --include=FILE\tInclude this file\n");
}

void browse_zip_file(const char* file_path, const char* password) {
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

    const char* file_name = NULL;  // Déplacer la déclaration ici

    initscr();  // Initialiser ncurses
    noecho();   // Désactiver l'affichage automatique des entrées clavier
    keypad(stdscr, TRUE); // Activer la prise en charge des touches spéciales

    int quit = 0;
    int selected_index = 0;

    while (!quit) {
        clear();  // Effacer l'écran

        // Reste du code...
    }

    endwin();  // Terminer ncurses
    zip_close(zip_file);
}

void bruteforce_zip_password(const char* file_path, const char* dictionary_path) {
    FILE* dictionary_file = fopen(dictionary_path, "r");
    if (!dictionary_file) {
        printf("Failed to open the dictionary file.\n");
        return;
    }

    char password[100];
    while (fgets(password, sizeof(password), dictionary_file)) {
        // Supprimer le saut de ligne à la fin du mot de passe
        int length = strlen(password);
        if (length > 0 && password[length - 1] == '\n') {
            password[length - 1] = '\0';
        }

        struct zip* zip_file = zip_open(file_path, 0, NULL);
        if (!zip_file) {
            printf("Failed to open the zip file.\n");
            fclose(dictionary_file);
            return;
        }

        int result = zip_set_default_password(zip_file, password);
        if (result == 0) {
            printf("Password found: %s\n", password);
            zip_close(zip_file);
            fclose(dictionary_file);
            return;
        }

        zip_close(zip_file);
    }

    printf("Password not found in the dictionary.\n");
    fclose(dictionary_file);
}

int main(int argc, char* argv[]) {
    int option;
    char* file_path = NULL;
    char* password = NULL;
    char* dictionary_path = NULL;

    while ((option = getopt(argc, argv, "ho:b:d:p:e:i:")) != -1) {
        switch (option) {
            case 'h':
                print_help();
                return 0;
            case 'o':
                file_path = optarg;
                break;
            case 'b':
                // Ignore this option for now
                break;
            case 'd':
                dictionary_path = optarg;
                break;
            case 'p':
                password = optarg;
                break;
            case 'e':
                // Ignore this option for now
                break;
            case 'i':
                dictionary_path = optarg;
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

    if (password != NULL && dictionary_path != NULL) {
        printf("Invalid options. Please use either -p or -d, not both.\n");
        return 1;
    }

    if (password != NULL) {
        browse_zip_file(file_path, password);
    } else if (dictionary_path != NULL) {
        bruteforce_zip_password(file_path, dictionary_path);
    } else {
        printf("No password or dictionary specified. Use -h or --help to see the available options.\n");
        return 1;
    }

    return 0;
}
