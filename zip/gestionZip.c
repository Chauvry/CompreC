#include "gestionZip.h"

#define DEFAULT_PASSWORD "default_password"


/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////               Ouverture ZIP                      /////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

struct zip* open_zip_file(const char* zip_file_path) {
    // Ouvre le fichier ZIP en mode lecture seule
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
    // Obtenir le nombre de fichiers dans l'archive ZIP
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
    int close_result = zip_close(zip_file);
    if (close_result == -1) {
        printf("Failed to close the zip file.\n");
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////               EXTRACT FUNCTION                   /////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////


int extract_zip(const char* zip_path, const char* selected_file, const char* password) {
    // Ouvrir le fichier zip
    struct zip* archive = open_zip_file(zip_path);
    if (!archive) {
        printf("Impossible d'ouvrir le fichier zip.\n");
        return -1;
    }
    
    // Extraire le fichier sélectionné
    struct zip_file* file = zip_fopen_encrypted(archive, selected_file, 0, password);
    if (!file) {
        printf("Impossible d'extraire le fichier %s.\n", selected_file);
        zip_close(archive);
        return -1;
    }
    
    // Lire et écrire le contenu du fichier
    char buffer[1024];
    FILE* output = fopen(selected_file, "wb");
    if (!output) {
        printf("Impossible de créer le fichier de sortie.\n");
        zip_fclose(file);
        zip_close(archive);
        return -1;
    }
    
    int read_bytes;
    while ((read_bytes = zip_fread(file, buffer, sizeof(buffer))) > 0) {
        fwrite(buffer, read_bytes, 1, output);
    }
    
    // Fermer les fichiers et libérer les ressources
    fclose(output);
    zip_fclose(file);
    zip_close(archive);
    
    printf("Extraction réussie du fichier %s.\n", selected_file);
    return 0;
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
//////////////////               AFFICHAGE ONLY                     /////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

void affichage_only(const char* zip_file_path) {
    struct zip* zip_file = open_zip_file(zip_file_path); // Ouverture du fichier ZIP
    if (!zip_file) {
        return;
    }

    int num_files = get_num_files(zip_file); // Récupération du nombre de fichiers dans le ZIP
    if (num_files == -1) {
        close_zip_file(zip_file);
        return;
    }

    int highlight = 1; // Option sélectionnée (mise en surbrillance)
    int c;
    int done = 0; // Indicateur de fin du menu

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
            const char* file_name = get_file_name(zip_file, i); // Récupération du nom du fichier
            struct zip_stat file_info;
            zip_stat_init(&file_info);
            zip_stat(zip_file, file_name, 0, &file_info); // Obtention des informations sur le fichier

            if (i + 2 == highlight) {
                attron(A_REVERSE); // Mettre en surbrillance l'option sélectionnée
            }
            printw("%s", file_name);

            if (file_info.valid & ZIP_STAT_SIZE) {
                printw(" (%ld bytes)", (long)file_info.size); // Affichage de la taille du fichier si disponible
            }

            if (is_directory(zip_file, (zip_uint64_t)file_name)) {
                printw(" [Dossier]"); // Indication de dossier si le fichier est un répertoire
            } else {
                printw(" [Fichier]"); // Indication de fichier si le fichier n'est pas un répertoire
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
                    done = 1; // Sortir de la boucle
                } else {
                    // Sélection d'un fichier/dossier (non implémenté dans cette version)
                }
                break;
        }
    }

    close_zip_file(zip_file); // Fermeture du fichier ZIP
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////       AFFICHAGE EXTRACTION SANS MDP                 ////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

void affichage_extraction_sans_password(const char* zip_file_path) {
    struct zip* zip_file = open_zip_file(zip_file_path); // Ouverture du fichier ZIP
    if (!zip_file) {
        return;
    }

    int num_files = get_num_files(zip_file); // Récupération du nombre de fichiers dans le ZIP
    if (num_files == -1) {
        close_zip_file(zip_file);
        return;
    }


    int highlight = 1; // Option sélectionnée (mise en surbrillance)
    int c;
    int extraction_done = 0; // Indicateur d'extraction terminée

    initscr();  // Initialiser l'interface utilisateur
    keypad(stdscr, TRUE);  // Activer les touches spéciales

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
            const char* file_name = get_file_name(zip_file, i); // Récupération du nom du fichier
            if (!file_name) {
                printw("Erreur : Impossible d'obtenir le nom du fichier dans le ZIP.\n");
                zip_close(zip_file);
                endwin();  // Fermer l'interface utilisateur
                return;
            }
            
            
            
            if (i + 2 == highlight) {
                attron(A_REVERSE); // Mettre en surbrillance l'option sélectionnée
            }
            printw("%s\n", file_name);
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
                    const char* selected_file = zip_get_name(zip_file, highlight - 2, 0);
                    if (!selected_file) {
                        printw("Erreur : Impossible d'obtenir le nom du fichier sélectionné.\n");
                        zip_close(zip_file);
                        endwin();  // Fermer l'interface utilisateur
                        return;}
                    else{
                        clear();
                            extraction_sans_password(zip_file, zip_file_path, selected_file); // Extraction du fichier sélectionné

                            // Attendre la saisie d'une touche pour continuer
                            printw("\nAppuyez sur une touche pour continuer...");
                            refresh();
                            getch();
                    }  
                    break;
                }
    }


    }

    close_zip_file(zip_file);
    endwin();  // Fermer l'interface utilisateur
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////               AFFICHAGE EXTRACTION AVEC PWD            ///////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

void affichage_extraction_avec_password(const char* zip_file_path, const char* password) {
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
            attron(A_REVERSE); // Mettre en surbrillance l'option sélectionnée
        }
        printw("Retour\n");
        attroff(A_REVERSE); // Annuler la surbrillance

        for (int i = 0; i < num_files; i++) {
            const char* file_name = zip_get_name(zip_file, i, 0);
            if (!file_name) {
                printw("Erreur : Impossible d'obtenir le nom du fichier dans le ZIP.\n");
                zip_close(zip_file);
                endwin();  // Fermer l'interface utilisateur
                return;
            }

            if (i + 2 == highlight) {
                attron(A_REVERSE); // Mettre en surbrillance l'option sélectionnée
            }
            printw("%s\n", file_name);
            attroff(A_REVERSE); // Annuler la surbrillance
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
                    done = 1; // Option "Retour" sélectionnée
                } else {
                    const char* selected_file = zip_get_name(zip_file, highlight - 2, 0);
                    if (!selected_file) {
                        printw("Erreur : Impossible d'obtenir le nom du fichier sélectionné.\n");
                        zip_close(zip_file);
                        endwin();  // Fermer l'interface utilisateur
                        return;
                    }

                    clear();

                    // Appeler la fonction d'extraction avec mot de passe
                    int extraction_result = extract_zip(zip_file_path, selected_file, password);

                    if (extraction_result != 0) {
                        printw("Erreur lors de l'extraction du fichier %s.\n", selected_file);
                    } else {
                        printw("Extraction réussie du fichier %s.\n", selected_file);
                    }

                    printw("\nAppuyez sur une touche pour continuer...");
                    refresh();
                    getch();
                }
                break;
        }
    }

    close_zip_file(zip_file);
    endwin();  // Fermer l'interface utilisateur
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////               AFFICHAGE EXTRACTION AVEC BF             ///////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

void affichage_extraction_brute_force(const char* zip_file_path, const char* dictionary_path) {
    struct zip* zip_file = open_zip_file(zip_file_path);  // Ouvre le fichier ZIP
    if (!zip_file) {
        printw("Erreur : Impossible d'ouvrir le fichier ZIP.\n");
        return;
    }
 
    int num_files = zip_get_num_entries(zip_file, 0);  // Obtient le nombre de fichiers dans le ZIP
    if (num_files < 0) {
        printw("Erreur : Impossible d'obtenir le nombre de fichiers dans le ZIP.\n");
        zip_close(zip_file);
        return;
    }

    int highlight = 1;  // Indice de mise en surbrillance de l'option actuelle
    int c;  // Variable pour stocker la saisie de l'utilisateur
    int done = 0;  // Indicateur pour terminer la boucle

    initscr();  // Initialise l'interface utilisateur
    keypad(stdscr, TRUE);  // Active les touches spéciales

    while (!done) {
        clear();

        if (highlight == 1) {
            attron(A_REVERSE);
        }
        printw("Retour\n");
        attroff(A_REVERSE);

        for (int i = 0; i < num_files; i++) {
            const char* file_name = zip_get_name(zip_file, i, 0);  // Obtient le nom du fichier dans le ZIP
            if (!file_name) {
                printw("Erreur : Impossible d'obtenir le nom du fichier dans le ZIP.\n");
                zip_close(zip_file);
                endwin();  // Ferme l'interface utilisateur
                return;
            }

            if (i + 2 == highlight) {
                attron(A_REVERSE);
            }
            printw("%s\n", file_name);
            attroff(A_REVERSE);
        }

        c = getch();  // Attend la saisie de l'utilisateur

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
                    done = 1;  // Quitte la boucle si "Retour" est sélectionné
                } else {
                    const char* selected_file = zip_get_name(zip_file, highlight - 2, 0);  // Obtient le nom du fichier sélectionné
                    if (!selected_file) {
                        printw("Erreur : Impossible d'obtenir le nom du fichier sélectionné.\n");
                        zip_close(zip_file);
                        endwin();  // Ferme l'interface utilisateur
                        return;
                    }

                    clear();

                    extraction_brute_force(zip_file, zip_file_path, selected_file, dictionary_path);  // Effectue l'extraction brute force

                    printw("\nAppuyez sur une touche pour continuer...");
                    refresh();
                    getch();
                }
                break;
        }
    }

    close_zip_file(zip_file);  // Ferme le fichier ZIP
    endwin();  // Ferme l'interface utilisateur
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////               EXTRACTION SANS MDP                /////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

void extraction_sans_password(struct zip* zip_file, const char* zip_file_path, const char* selected_file) {
    struct zip_file* file; // Pointeur vers le fichier à extraire
    struct zip_stat file_info; // Structure pour stocker les informations sur le fichier

    int index = zip_name_locate(zip_file, selected_file, 0); // Recherche de l'index du fichier dans le fichier ZIP
    if (index < 0) {
        printw("Le contenu '%s' n'a pas été trouvé dans le fichier ZIP.\n", selected_file);
        return;
    }

    if (zip_stat_index(zip_file, index, 0, &file_info) < 0) { // Obtention des informations sur le fichier
        printw("Impossible d'obtenir les informations sur le contenu '%s'.\n", selected_file);
        return;
    }

    char buffer[1024]; // Tampon pour stocker les données du fichier extrait
    int length;

    file = zip_fopen_index(zip_file, index, 0); // Ouverture du fichier dans le fichier ZIP
    if (!file) {
        printw("Erreur lors de l'ouverture du contenu '%s' dans le fichier ZIP.\n", selected_file);
        return;
    }

    int output_fd = open(selected_file, O_WRONLY | O_CREAT | O_TRUNC, 0644); // Création du fichier de sortie
    if (output_fd < 0) {
        printw("Erreur lors de la création du fichier de sortie pour le contenu '%s'.\n", selected_file);
        zip_fclose(file);
        return;
    }

    while ((length = zip_fread(file, buffer, sizeof(buffer))) > 0) { // Lecture et écriture du contenu du fichier extrait
        if (write(output_fd, buffer, length) < 0) {
            printw("Erreur lors de l'écriture du contenu '%s' extrait.\n", selected_file);
            close(output_fd);
            zip_fclose(file);
            return;
        }
    }

    close(output_fd);
    zip_fclose(file); // Fermeture du fichier ZIP

    printw("Extraction du contenu '%s' terminée.\n", selected_file);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////                EXTRACTION AVEC BF             ////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////


void extraction_brute_force(struct zip* zip_file, const char* zip_file_path, const char* selected_file, const char* dictionary_path) {
    struct zip_stat file_info;

    if (!zip_file) {
        printw("Erreur : Impossible d'ouvrir le fichier ZIP.\n");
        return;
    }

    zip_uint64_t index = zip_name_locate(zip_file, selected_file, 0);
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

    FILE* dictionary_file = fopen(dictionary_path, "r");
    if (!dictionary_file) {
        printw("Erreur : Impossible d'ouvrir le fichier de dictionnaire.\n");
        return;
    }


    char *password =malloc(sizeof(char*)*11);
    size_t g=10;
    while (getline(&password,&g,dictionary_file)!=0) {
        int i =0;       
        while(password[i] != '\n' && password[i]){
            i++;

        }
        password[i] ='\0';
        if(!extract_zip(zip_file_path, selected_file, password))
        {
            fclose(dictionary_file);
            printw("Extraction du contenu '%s' terminée avec le mot de passe : %s\n", selected_file, password);
            
            return;
        }

    }
    

    // Si aucun mot de passe du dictionnaire ne correspond, afficher un message d'erreur
    printw("Erreur : Aucun mot de passe du dictionnaire ne correspond au fichier spécifié.\n");
    fclose(dictionary_file);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////               AJOUT FICHIER LOCAL             ////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////


int inclure_fichier_local_zip(const char* zip_file_path, const char* fichier_path) {
   // Ouvrir le fichier ZIP en mode lecture/écriture
    zip_t* zip = zip_open(zip_file_path, ZIP_CREATE | ZIP_EXCL, NULL);

    if (zip == NULL) {
        printf("Erreur : Impossible d'ouvrir le fichier ZIP.\n");
        return -1;
    }

    // Lire le contenu du fichier à ajouter
    FILE* fichier = fopen(fichier_path, "rb");
    if (fichier == NULL) {
        printf("Erreur : Impossible de lire le fichier à ajouter.\n");
        zip_close(zip);
        return -1;
    }

    fseek(fichier, 0, SEEK_END);
    long taille_fichier = ftell(fichier);
    fseek(fichier, 0, SEEK_SET);

    char* contenu_fichier = malloc(taille_fichier);
    fread(contenu_fichier, 1, taille_fichier, fichier);

    fclose(fichier);

    // Ajouter le fichier au fichier ZIP
    zip_source_t* source = zip_source_buffer(zip, contenu_fichier, taille_fichier, 0);
    if (source == NULL || zip_file_add(zip, fichier_path, source, 0) < 0) {
        printf("Erreur : Impossible d'ajouter le fichier au fichier ZIP.\n");
        zip_source_free(source);
        zip_close(zip);
        free(contenu_fichier);
        return -1;
    }

    // Fermer le fichier ZIP
    zip_close(zip);
    free(contenu_fichier);

    return 0;
}
