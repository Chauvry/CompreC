#include "menu.h"

void extraireSansMotDePasse(const char* zip_file, const char* entry_name) {
    // Code pour extraire le contenu sans mot de passe
    printf("Extraction du contenu '%s' sans mot de passe...\n", entry_name);
    // Code d'extraction du contenu sans mot de passe
}

void extraireAvecMotsDePasseConnus(const char* zip_file, const char* entry_name) {
    // Code pour extraire le contenu avec des mots de passe connus
    printf("Extraction du contenu '%s' avec des mots de passe connus...\n", entry_name);
    // Code d'extraction du contenu avec des mots de passe connus
}

void extraireParBruteforce(const char* zip_file, const char* entry_name) {
    // Code pour extraire le contenu par bruteforce
    printf("Extraction du contenu '%s' par bruteforce...\n", entry_name);
    // Code d'extraction du contenu par bruteforce
}

void sousMenuExtraction(const char* zip_file) {
    int choix;
    int highlight = 1;
    int c;

    while (1) {
        choix = 0;
        // Effacer l'écran
        clear();

        // Afficher le sous-menu d'extraction
        printw("--- Sous-menu d'extraction ---\n");
        if (highlight == 1) attron(A_REVERSE); // Mettre en surbrillance l'option sélectionnée
        printw("1. Extraire sans mot de passe\n");
        attroff(A_REVERSE); // Annuler la surbrillance
        if (highlight == 2) attron(A_REVERSE);
        printw("2. Extraire avec mots de passe connus\n");
        attroff(A_REVERSE);
        if (highlight == 3) attron(A_REVERSE);
        printw("3. Extraire par bruteforce\n");
        attroff(A_REVERSE);
        if (highlight == 4) attron(A_REVERSE);
        printw("4. Retour\n");
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

        if (choix != 0) {
            clear();
            switch (choix) {
                case 1:
                    clear();
                    // Afficher le contenu du ZIP
                    afficherContenuZIP(zip_file);


                    break;
                case 2:
                    extraireAvecMotsDePasseConnus(zip_file, NULL);
                    break;
                case 3:
                    extraireParBruteforce(zip_file, NULL);
                    break;
                case 4:
                    return;
            }

        }

    }

    // Attendre la saisie d'une touche pour revenir au sous-menu
    printw("\nAppuyez sur une touche pour continuer...");
    refresh();
    getch();
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
            affichageZIP(zip_file_path);

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