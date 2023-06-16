#include "gestionZip.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////               Ouverture ZIP                      /////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

struct zip* open_zip_file(const char* zip_file_path) {
    struct zip* zip_file = zip_open(zip_file_path, ZIP_RDONLY, NULL);
    if (!zip_file) {
        printf("Failed to open the zip file.\n");
        return NULL;
    }

    return zip_file;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////               VERIF DOSSIER                      /////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
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

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////               COMPTE LE NB FICHIER               /////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

int get_num_files(struct zip* zip_file) {
    int num_files = zip_get_num_entries(zip_file, 0);
    if (num_files == -1) {
        printf("Failed to get the number of files in the zip archive.\n");
        return -1;
    }

    return num_files;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////               RÉCUP LE NOM DU FICHIER            /////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

const char* get_file_name(struct zip* zip_file, int index) {
    return zip_get_name(zip_file, index, 0);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////               FERMETURE ZIP                      /////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

void close_zip_file(struct zip* zip_file) {
    zip_close(zip_file);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////               EXTRACTION SANS MDP                /////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

void extraireContenuZIP(struct zip* zip_file, const char* zip_file_path, const char* selected_file) {
    struct zip_file* file;
    struct zip_stat file_info;

    int index = zip_name_locate(zip_file, selected_file, 0);
    if (index < 0) {
        printw("Le contenu '%s' n'a pas été trouvé dans le fichier ZIP.\n", selected_file);
        return;
    }

    if (zip_stat_index(zip_file, index, 0, &file_info) < 0) {
        printw("Impossible d'obtenir les informations sur le contenu '%s'.\n", selected_file);
        return;
    }

    char buffer[1024];
    int length;

    file = zip_fopen_index(zip_file, index, 0);
    if (!file) {
        printw("Erreur lors de l'ouverture du contenu '%s' dans le fichier ZIP.\n", selected_file);
        return;
    }
    

    int output_fd = open(selected_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (output_fd < 0) {
        printw("Erreur lors de la création du fichier de sortie pour le contenu '%s'.\n", selected_file);
        zip_fclose(file);
        return;
    }

    while ((length = zip_fread(file, buffer, sizeof(buffer))) > 0) {
        if (write(output_fd, buffer, length) < 0) {
            printw("Erreur lors de l'écriture du contenu '%s' extrait.\n", selected_file);
            close(output_fd);
            zip_fclose(file);
            return;
        }
    }

    close(output_fd);
    zip_fclose(file);

    printw("Extraction du contenu '%s' terminée.\n", selected_file);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////               RECUP STAT                         /////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

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

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
///////       AFFICHAGE CONTENU ZIP POUR EXTRACTION SANS MDP                 ////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

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

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////               AFFICHAGE ONLY                     /////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

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

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////               EXTRACTION AVEC PWD               /////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

void extraireAvecMotDePasse(struct zip* zip_file, const char* zip_file_path, const char* selected_file, const char* password) {
    struct zip_file* file;
    struct zip_stat file_info;

    if (!zip_file) {
        printw("Erreur : Impossible d'ouvrir le fichier ZIP.\n");
        return;
    }

    int index = zip_name_locate(zip_file, selected_file, 0);
    if (index < 0) {
        printw("Erreur : Le fichier spécifié n'a pas été trouvé dans le ZIP.\n");
        return;
    }

    if (zip_stat_index(zip_file, index, 0, &file_info) < 0) {
        printw("Erreur : Impossible d'obtenir les informations sur le fichier spécifié.\n");
        return;
    }

    char buffer[1024];
    int length;


    file = zip_fopen_index_encrypted(zip_file, index, 0, password);
    if (!file) {
        printw("Erreur : Impossible d'ouvrir le fichier à extraire avec le mot de passe spécifié.\n");
        return;
    }

    int output_fd = open(selected_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (output_fd < 0) {
        printw("Erreur lors de la création du fichier de sortie pour le contenu '%s'.\n", selected_file);
        zip_fclose(file);
        return;
    }

    while ((length = zip_fread(file, buffer, sizeof(buffer))) > 0) {
        if (write(output_fd, buffer, length) < 0) {
            printw("Erreur lors de l'écriture du contenu '%s' extrait.\n", selected_file);
            close(output_fd);
            zip_fclose(file);
            return;
        }
    }

    close(output_fd);
    zip_fclose(file);

    printw("Extraction du contenu '%s' terminée.\n", selected_file);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////               AFFICHAGE EXTRACTION AVEC PWD            ///////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

void affichageZIPpwd(const char* zip_file_path, const char* password) {
    struct zip* zip_file = open_zip_file(zip_file_path);
    if (!zip_file) {
        printw("Erreur : Impossible d'ouvrir le fichier ZIP.\n");
        return;
    }
 
    int num_files = zip_get_num_entries(zip_file, 0);
    if (num_files < 0) {
        printw("Erreur : Impossible d'obtenir le nombre de fichiers dans le ZIP.\n");
        zip_close(zip_file);
        return;
    }

    int highlight = 1;
    int c;
    int done = 0;

    initscr();  // Initialiser l'interface utilisateur
    keypad(stdscr, TRUE);  // Activer les touches spéciales

    while (!done) {
        clear();

        if (highlight == 1) {
            attron(A_REVERSE);
        }
        printw("Retour\n");
        attroff(A_REVERSE);

        for (int i = 0; i < num_files; i++) {
            const char* file_name = zip_get_name(zip_file, i, 0);
            if (!file_name) {
                printw("Erreur : Impossible d'obtenir le nom du fichier dans le ZIP.\n");
                zip_close(zip_file);
                endwin();  // Fermer l'interface utilisateur
                return;
            }

            if (i + 2 == highlight) {
                attron(A_REVERSE);
            }
            printw("%s\n", file_name);
            attroff(A_REVERSE);
        }

        c = getch();

        switch (c) {
            case KEY_UP:
                if (highlight > 1) {
                    highlight--;
                }
                break;
            case KEY_DOWN:
                if (highlight < num_files + 1) {
                    highlight++;
                }
                break;
            case 10:  // Touche Enter
                if (highlight == 1) {
                    done = 1;
                } else {
                    const char* selected_file = zip_get_name(zip_file, highlight - 2, 0);
                    if (!selected_file) {
                        printw("Erreur : Impossible d'obtenir le nom du fichier sélectionné.\n");
                        zip_close(zip_file);
                        endwin();  // Fermer l'interface utilisateur
                        return;
                    }

                    clear();

                    extraireAvecMotDePasse(zip_file, zip_file_path,selected_file, password);

                    printw("\nAppuyez sur une touche pour continuer...");
                    refresh();
                    getch();
                }
                break;
        }
    }

    zip_close(zip_file);
    endwin();  // Fermer l'interface utilisateur
}