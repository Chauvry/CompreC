#include "gestionZip.h"

struct zip* open_zip_file(const char* zip_file_path) {
    struct zip* zip_file = zip_open(zip_file_path, ZIP_RDONLY, NULL);
    if (!zip_file) {
        printf("Failed to open the zip file.\n");
        return NULL;
    }

    return zip_file;
}


int is_directory(struct zip* archive, zip_uint64_t index) {
    struct zip_stat file_info;
    int flags = 0; // Peut être défini à 0 ou à d'autres options selon vos besoins

    if (zip_stat_index(archive, index, flags, &file_info) == -1) {
        // Gestion de l'erreur lors de la récupération des informations du fichier
        return 0; // Ou une autre valeur pour indiquer une erreur
    }

    const char* file_name = zip_get_name(archive, index, flags);

    // Vérifier si le dernier caractère du nom est un '/'
    size_t name_length = strlen(file_name);
    if (name_length > 0 && file_name[name_length - 1] == '/') {
        return 1; // Le fichier est un répertoire
    }

    return 0; // Le fichier n'est pas un répertoire
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

void extraireContenuZIP(struct zip* zip_file, const char* zip_file_path, const char* entry_name) {
    struct zip_file* file;
    struct zip_stat file_info;

    if (!zip_file) {
        return;
    }

    int index = zip_name_locate(zip_file, entry_name, 0);
    if (index < 0) {
        printw("Le contenu '%s' n'a pas été trouvé dans le fichier ZIP.\n", entry_name);
        return;
    }

    if (zip_stat_index(zip_file, index, 0, &file_info) < 0) {
        printw("Impossible d'obtenir les informations sur le contenu '%s'.\n", entry_name);
        return;
    }

    char buffer[1024];
    int length;

    file = zip_fopen_index(zip_file, index, 0);
    if (!file) {
        printw("Erreur lors de l'ouverture du contenu '%s' dans le fichier ZIP.\n", entry_name);
        return;
    }
    

    int output_fd = open(entry_name, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (output_fd < 0) {
        printw("Erreur lors de la création du fichier de sortie pour le contenu '%s'.\n", entry_name);
        zip_fclose(file);
        return;
    }

    while ((length = zip_fread(file, buffer, sizeof(buffer))) > 0) {
        if (write(output_fd, buffer, length) < 0) {
            printw("Erreur lors de l'écriture du contenu '%s' extrait.\n", entry_name);
            close(output_fd);
            zip_fclose(file);
            return;
        }
    }

    close(output_fd);
    zip_fclose(file);

    printw("Extraction du contenu '%s' terminée.\n", entry_name);
}

int get_file_stat(struct zip* zip_file, const char* file_name, struct zip_stat* file_stat) {
    // Recherche de l'index du fichier dans le fichier ZIP
    int file_index = zip_name_locate(zip_file, file_name, 0);
    if (file_index < 0) {
        // Le fichier n'a pas été trouvé
        return 0;
    }

    // Obtention des informations de statut du fichier
    if (zip_stat_index(zip_file, file_index, ZIP_STAT_SIZE | ZIP_STAT_MTIME, file_stat) != 0) {
        // Erreur lors de l'obtention des informations de statut
        return 0;
    }

    return 1;
}



void afficherContenuZIP(const char* zip_file_path) {
    struct zip* zip_file = open_zip_file(zip_file_path);
    if (!zip_file) {
        return;
    }

    int num_files = get_num_files(zip_file);
    if (num_files == -1) {
        close_zip_file(zip_file);
        return;
    }

    printw("Contenu du fichier ZIP :\n");

    int highlight = 1;
    int c;
    const char* selected_file = NULL;
    int extraction_done = 0;

    while (!extraction_done) {
        // Effacer l'écran
        clear();

        // Afficher l'option "Retour"
        if (highlight == 1) {
            attron(A_REVERSE); // Mettre en surbrillance l'option sélectionnée
        }
        printw("Retour\n");
        attroff(A_REVERSE); // Annuler la surbrillance

        // Afficher le contenu du ZIP
        for (int i = 0; i < num_files; i++) {
            const char* file_name = get_file_name(zip_file, i);
            struct zip_stat file_info;
            zip_stat_init(&file_info);
            zip_stat(zip_file, file_name, 0, &file_info);

            if (i + 2 == highlight) {
                attron(A_REVERSE); // Mettre en surbrillance l'option sélectionnée
            }
            printw("%s", file_name);

            if (file_info.valid & ZIP_STAT_SIZE) {
                printw(" (%ld bytes)", (long)file_info.size);
            }

            if (is_directory(zip_file, selected_file)) {
                printw(" [Dossier]");
            } else {
                printw(" [Fichier]");
            }
            printw("\n");
            attroff(A_REVERSE); // Annuler la surbrillance
        }

        // Lire l'entrée de l'utilisateur
        c = getch();
        switch (c) {
            case KEY_UP:
                if (highlight > 1) {
                    highlight--;
                }
                break;
            case KEY_DOWN:
                if (highlight < num_files + 2) {
                    highlight++;
                }
                break;
            case 10: // Touche Enter
                if (highlight == 1) {
                    // Option "Retour" sélectionnée
                    extraction_done = 1;
                } else {
                    selected_file = get_file_name(zip_file, highlight - 2);
                    extraction_done = 1; // Extraction effectuée, sortir de la boucle
                }
                break;
        }
    }

    if (selected_file) {
        extraireContenuZIP(zip_file, zip_file_path, selected_file);

        // Attendre la saisie d'une touche pour continuer
        printw("\nAppuyez sur une touche pour continuer...");
        refresh();
        getch();
    }

    // close_zip_file(zip_file);
}

void affichageZIP(const char* zip_file_path) {
    struct zip* zip_file = open_zip_file(zip_file_path);
    if (!zip_file) {
        return;
    }

    int num_files = get_num_files(zip_file);
    if (num_files == -1) {
        close_zip_file(zip_file);
        return;
    }

    int highlight = 1;
    int c;
    int done = 0;

    while (!done) {
        // Effacer l'écran
        clear();

        // Afficher l'option "Retour"
        if (highlight == 1) {
            attron(A_REVERSE); // Mettre en surbrillance l'option sélectionnée
        }
        printw("Retour\n");
        attroff(A_REVERSE); // Annuler la surbrillance

        // Afficher le contenu du ZIP
        for (int i = 0; i < num_files; i++) {
            const char* file_name = get_file_name(zip_file, i);
            struct zip_stat file_info;
            zip_stat_init(&file_info);
            zip_stat(zip_file, file_name, 0, &file_info);

            if (i + 2 == highlight) {
                attron(A_REVERSE); // Mettre en surbrillance l'option sélectionnée
            }
            printw("%s", file_name);

            if (file_info.valid & ZIP_STAT_SIZE) {
                printw(" (%ld bytes)", (long)file_info.size);
            }

            if (is_directory(zip_file, file_name)) {
                printw(" [Dossier]");
            } else {
                printw(" [Fichier]");
            }
            printw("\n");
            attroff(A_REVERSE); // Annuler la surbrillance
        }

        // Lire l'entrée de l'utilisateur
        c = getch();
        switch (c) {
            case KEY_UP:
                if (highlight > 1) {
                    highlight--;
                }
                break;
            case KEY_DOWN:
                if (highlight < num_files + 2) {
                    highlight++;
                }
                break;
            case 10: // Touche Enter
                if (highlight == 1) {
                    // Option "Retour" sélectionnée
                    done = 1;
                } else {
                    // Sélection d'un fichier/dossier (non implémenté dans cette version)
                }
                break;
        }
    }

    // close_zip_file(zip_file);
}
