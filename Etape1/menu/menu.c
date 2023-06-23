#include "menu.h"

#define MAX_PATH_LENGTH 256
#define MAX_FILE_EXTENSIONS 5

int lireFichiersRepertoire(const char* repertoire, char*** fichiers) {
    DIR* dir = opendir(repertoire);
    if (!dir) {
        return -1; // Erreur : impossible d'ouvrir le répertoire
    }

    int num_files = 0;
    struct dirent* ent;

    // Compter le nombre de fichiers dans le répertoire
    while ((ent = readdir(dir)) != NULL) {
        if (ent->d_type == DT_REG) {
            num_files++;
        }
    }

    rewinddir(dir);

    // Allouer de la mémoire pour stocker les noms de fichiers
    *fichiers = (char**)malloc(num_files * sizeof(char*));

    int i = 0;
    while ((ent = readdir(dir)) != NULL) {
        if (ent->d_type == DT_REG) {
            (*fichiers)[i] = strdup(ent->d_name);
            i++;
        }
    }

    closedir(dir);

    return num_files;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////              MENU PRINCIPAL             ////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

void afficherMenu(const char* zip_file_path) {
    // Initialisation de ncurses
    initscr();
    keypad(stdscr, TRUE); // Activer la prise en charge des touches spéciales
    // noecho(); // Désactiver l'affichage des caractères saisis

    int choix = 0;
    int highlight = 1;
    int c;

    while (choix == 0) {
        // Effacer l'écran
        clear();

        // Afficher le menu principal
        printw("Menu principal :\n");
        if (highlight == 1) attron(A_REVERSE); // Mettre en surbrillance l'option sélectionnée
        printw("1. Afficher le contenu du ZIP\n");
        attroff(A_REVERSE); // Annuler la surbrillance
        if (highlight == 2) attron(A_REVERSE);
        printw("2. Extraire un contenu ciblé\n");
        attroff(A_REVERSE);
        if (highlight == 3) attron(A_REVERSE);
        printw("3. Ajouter un fichier local\n");
        attroff(A_REVERSE);
        if (highlight == 4) attron(A_REVERSE);
        printw("4. Quitter\n");
        attroff(A_REVERSE);
        refresh();

        // Lire l'entrée de l'utilisateur
        c = getch();
        switch (c) {
            case KEY_UP:
                if (highlight > 1) highlight--;
                break;
            case KEY_DOWN:
                if (highlight < 4) highlight++;
                break;
            case 10: // Touche Enter
                choix = highlight;
                break;
        }
    }

    switch (choix) {
        case 1:
            clear();
            // Afficher le contenu du ZIP
            affichage_only(zip_file_path);

            afficherMenu(zip_file_path); // Réafficher le menu principal
            break;
        case 2:
            clear();
            sousMenuExtraction(zip_file_path);

            // Attendre la saisie d'une touche pour revenir au menu principal

            afficherMenu(zip_file_path); // Réafficher le menu principal
            break;
        case 3:
            clear();
            sousMenuAjouterFichierLocal(zip_file_path);

            afficherMenu(zip_file_path); // Réafficher le menu principal
            break;
        case 4:
            clear();
            // Quitter le programme
            printw("Au revoir !\n");
            refresh();
            break;
    }
    // Restaurer les paramètres par défaut de ncurses et quitter
    endwin();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////          MENU SECONDAIRE EXTRACTION      ///////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

void sousMenuExtraction(const char* zip_file) {
    int choix;
    int highlight = 1;
    int c;

    while (1) {
        choix = 0;
        clear();

        printw("--- Sous-menu d'extraction ---\n");
        if (highlight == 1) attron(A_REVERSE);
        printw("1. Extraire sans mot de passe\n");
        attroff(A_REVERSE);
        if (highlight == 2) attron(A_REVERSE);
        printw("2. Extraire avec mots de passe connus\n");
        attroff(A_REVERSE);
        if (highlight == 3) attron(A_REVERSE);
        printw("3. Extraire par brute force\n");
        attroff(A_REVERSE);
        if (highlight == 4) attron(A_REVERSE);
        printw("4. Retour\n");
        attroff(A_REVERSE);
        refresh();

        c = getch();
        switch (c) {
            case KEY_UP:
                if (highlight > 1) highlight--;
                break;
            case KEY_DOWN:
                if (highlight < 4) highlight++;
                break;
            case 10: // Touche Enter
                choix = highlight;
                break;
        }
        
        if (choix != 0) {
            clear();
            switch (choix) {
                case 1:
                    clear();
                    affichage_extraction_sans_password(zip_file);
                    break;
                case 2:
                    clear();
                    const char* password;
                    printw("Entrez le mot de passe : ");
                    echo();
                    scanw("%ms", &password);
                    noecho();
                    affichage_extraction_avec_password(zip_file, password);
                    free((void*)password);
                    break;
                case 3:
                    clear();
                    printw("Sous-menu Brute force\n");
                    printw("Sélectionnez le dictionnaire de brute force :\n");
                    refresh();

                    char* dictionnaire_path = malloc(256);  // Allouer de la mémoire pour stocker le chemin du dictionnaire
                    getcwd(dictionnaire_path, 256);  // Récupérer le répertoire de travail actuel

                    // Lire les fichiers du répertoire "dictionnaire/"
                    char** fichiers_dictionnaire;
                    int num_files = lireFichiersRepertoire("dictionnaire/", &fichiers_dictionnaire);

                    if (num_files < 0) {
                        printw("Erreur : Impossible de lire les fichiers du répertoire dictionnaire.\n");
                        free(dictionnaire_path);
                        break;
                    }

                    int highlight_brute_force = 1;
                    int done_brute_force = 0;

                    while (!done_brute_force) {
                        clear();

                        if (highlight_brute_force == 1) {
                            attron(A_REVERSE);
                        }
                        printw("Retour\n");
                        attroff(A_REVERSE);

                        for (int i = 0; i < num_files; i++) {
                            if (i + 2 == highlight_brute_force) {
                                attron(A_REVERSE);
                            }
                            printw("%s\n", fichiers_dictionnaire[i]);
                            attroff(A_REVERSE);
                        }

                        c = getch();

                        switch (c) {
                            case KEY_UP:
                                if (highlight_brute_force > 1) {
                                    highlight_brute_force--;
                                }
                                break;
                            case KEY_DOWN:
                                if (highlight_brute_force < num_files + 1) {
                                    highlight_brute_force++;
                                }
                                break;
                            case 10: // Touche Enter
                                if (highlight_brute_force == 1) {
                                    done_brute_force = 1;
                                } else {
                                    const char* dictionary_name = fichiers_dictionnaire[highlight_brute_force - 2];
                                    snprintf(dictionnaire_path + strlen(dictionnaire_path), 256 - strlen(dictionnaire_path), "/dictionnaire/%s", dictionary_name);
                                    clear();
                                    affichage_extraction_brute_force(zip_file, dictionnaire_path);
                                    done_brute_force = 1;
                                }
                                break;
                        }
                    }

                    free(dictionnaire_path);
                    for (int i = 0; i < num_files; i++) {
                        free(fichiers_dictionnaire[i]);
                    }
                    free(fichiers_dictionnaire);

                    break;
                case 4:
                    return;
            }
        }
    }

    printw("\nAppuyez sur une touche pour continuer...");
    refresh();
    getch();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////          MENU SECONDAIRE AJOUT           ///////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

void sousMenuAjouterFichierLocal(const char* zip_file_path) {
    clear();
    printw("Sous-menu Ajouter un fichier local :\n");

    int choix = 0;
    int highlight = 1;
    int c;

    while (choix == 0) {
        clear();

        // Afficher le sous-menu
        printw("Sous-menu Ajouter un fichier local :\n");
        if (highlight == 1) attron(A_REVERSE);
        printw("1. Fichier ZIP sans mot de passe\n");
        attroff(A_REVERSE);
        if (highlight == 2) attron(A_REVERSE);
        printw("2. Fichier ZIP avec mot de passe connu\n");
        attroff(A_REVERSE);
        if (highlight == 3) attron(A_REVERSE);
        printw("3. Fichier ZIP avec brute force\n");
        attroff(A_REVERSE);
        refresh();

        c = getch();
        switch (c) {
            case KEY_UP:
                if (highlight > 1) highlight--;
                break;
            case KEY_DOWN:
                if (highlight < 3) highlight++;
                break;
            case 10: // Touche Enter
                choix = highlight;
                break;
        }
    }

    switch (choix) {
        case 1:
            clear();
            printw("Sous-menu Ajouter un fichier au ZIP\n");
            printw("Sélectionnez le fichier à ajouter :\n");
            refresh();

            char* fichierAjout_path = malloc(256);  // Allouer de la mémoire pour stocker le chemin du fichier à ajouter
            getcwd(fichierAjout_path, 256);  // Récupérer le répertoire de travail actuel

            // Lire les fichiers du répertoire "fichierAjout/"
            char** fichiers_fichierAjout;
            int num_files = lireFichiersRepertoire("fichierAjout/", &fichiers_fichierAjout);

            if (num_files < 0) {
                printw("Erreur : Impossible de lire les fichiers du répertoire fichierAjout.\n");
                free(fichierAjout_path);
                break;
            }

            int highlight_ajout = 1;
            int done_ajout = 0;

            while (!done_ajout) {
                clear();

                if (highlight_ajout == 1) {
                    attron(A_REVERSE);
                }
                printw("Retour\n");
                attroff(A_REVERSE);

                for (int i = 0; i < num_files; i++) {
                    if (i + 2 == highlight_ajout) {
                        attron(A_REVERSE);
                    }
                    printw("%s\n", fichiers_fichierAjout[i]);
                    attroff(A_REVERSE);
                }

                refresh();

                c = getch();

                switch (c) {
                    case KEY_UP:
                        if (highlight_ajout > 1) {
                            highlight_ajout--;
                        }
                        break;
                    case KEY_DOWN:
                        if (highlight_ajout < num_files + 1) {
                            highlight_ajout++;
                        }
                        break;
                    case 10: // Touche Enter
                        if (highlight_ajout == 1) {
                            done_ajout = 1;
                        } else {
                            const char* file_name = fichiers_fichierAjout[highlight_ajout - 2];
                            snprintf(fichierAjout_path + strlen(fichierAjout_path), 256 - strlen(fichierAjout_path), "/fichierAjout/%s", file_name);
                            clear();
                            inclure_fichier_local_zip(zip_file_path, fichierAjout_path);
                            done_ajout = 1;
                        }
                        break;
                }
            }

            free(fichierAjout_path);
            for (int i = 0; i < num_files; i++) {
                free(fichiers_fichierAjout[i]);
            }
            free(fichiers_fichierAjout);

            // Ajouter un arrêt pour empêcher de passer au point de rupture
            getch();
            break;

        case 2:
            clear();
            // Ajouter un fichier ZIP avec mot de passe connu
            // Code pour ajouter un fichier ZIP avec mot de passe connu ici
            printw("Fichier ZIP avec mot de passe connu ajouté.\n");
            refresh();
            getch(); // Attendre la saisie d'une touche pour revenir au sous-menu
            sousMenuAjouterFichierLocal(zip_file_path); // Réafficher le sous-menu
            break;
        case 3:
            clear();
            // Ajouter un fichier ZIP avec brute force
            // Code pour ajouter un fichier ZIP avec brute force ici
            printw("Fichier ZIP avec brute force ajouté.\n");
            refresh();
            getch(); // Attendre la saisie d'une touche pour revenir au sous-menu
            sousMenuAjouterFichierLocal(zip_file_path); // Réafficher le sous-menu
            break;
    }
}