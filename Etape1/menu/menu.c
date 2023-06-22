#include "menu.h"



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
        printw("3. Quitter\n");
        attroff(A_REVERSE);
        refresh();

        // Lire l'entrée de l'utilisateur
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
            // Afficher le contenu du ZIP
            affichage_only(zip_file_path);

            afficherMenu(zip_file_path); // Réafficher le menu principal
            break;
        case 2:
            sousMenuExtraction(zip_file_path);

            // Attendre la saisie d'une touche pour revenir au menu principal

            afficherMenu(zip_file_path); // Réafficher le menu principal
            break;
        case 3:
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
////////////////////////              MENU SECONDAIRE             ///////////////////////////////////
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
                                    // printw("\nAppuyez sur une touche pour continuer...");
                                    refresh();
                                    getch();
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
