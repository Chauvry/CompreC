#include "menu.h"

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

        // Afficher le contenu du ZIP
        for (int i = 0; i < num_files; i++) {
            const char* file_name = get_file_name(zip_file, i);
            if (i + 1 == highlight) {
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
                if (highlight < num_files) {
                    highlight++;
                }
                break;
            case 10: // Touche Enter
                selected_file = get_file_name(zip_file, highlight - 1);
                extraction_done = 1; // Extraction effectuée, sortir de la boucle
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





void extraireSansMotDePasse(struct zip* zip_file, const char* entry_name) {
    // Code pour extraire le contenu sans mot de passe
    printf("Extraction du contenu '%s' sans mot de passe...\n", entry_name);
    // Code d'extraction du contenu sans mot de passe
}

void extraireAvecMotsDePasseConnus(struct zip* zip_file, const char* entry_name) {
    // Code pour extraire le contenu avec des mots de passe connus
    printf("Extraction du contenu '%s' avec des mots de passe connus...\n", entry_name);
    // Code d'extraction du contenu avec des mots de passe connus
}

void extraireParBruteforce(struct zip* zip_file, const char* entry_name) {
    // Code pour extraire le contenu par bruteforce
    printf("Extraction du contenu '%s' par bruteforce...\n", entry_name);
    // Code d'extraction du contenu par bruteforce
}

void sousMenuExtraction(struct zip* zip_file, const char* entry_name) {
    int choix;

    while (1) {
        printf("\n--- Sous-menu d'extraction ---\n");
        printf("1. Extraire sans mot de passe\n");
        printf("2. Extraire avec mots de passe connus\n");
        printf("3. Extraire par bruteforce\n");
        printf("4. Retour\n");
        printf("Choix : ");
        scanf("%d", &choix);
        printf("\n");

        switch (choix) {
            case 1:
                printf("\033[2J\033[1;1H");
                extraireSansMotDePasse(zip_file, entry_name);
                break;
            case 2:
                printw("\033[2J\033[1;1H");
                extraireAvecMotsDePasseConnus(zip_file, entry_name);
                break;
            case 3:
                printw("\033[2J\033[1;1H");
                extraireParBruteforce(zip_file, entry_name);
                break;
            case 4:
                printw("\033[2J\033[1;1H");
                return;
            default:
                printw("Choix invalide. Veuillez réessayer.\n");
                break;
        }
    }
}

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
            afficherContenuZIP(zip_file_path);

            afficherMenu(zip_file_path); // Réafficher le menu principal
            break;
        case 2:
            clear();
            // Appeler la fonction pour extraire un contenu ciblé
            // extraireContenuZIP(zip_file_path);

            // Attendre la saisie d'une touche pour revenir au menu principal
            printw("\nAppuyez sur une touche pour continuer...");
            refresh();
            getch();
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