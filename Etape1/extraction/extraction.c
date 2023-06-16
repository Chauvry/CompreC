#include <string.h>

void extraireAvecMotDePasse(struct zip* zip_file, const char* zip_file_path, const char* file_name, const char* password) {
    // Vérifier si le fichier existe dans le ZIP
    int file_index = get_file_index(zip_file, file_name);
    if (file_index == -1) {
        printw("Le fichier spécifié n'existe pas dans le ZIP.\n");
        return;
    }

    // Ouvrir le fichier dans le ZIP
    struct zip_file* zip_file_entry = zip_fopen_index(zip_file, file_index, ZIP_FL_ENC_GUESS);
    if (!zip_file_entry) {
        printw("Erreur lors de l'ouverture du fichier dans le ZIP.\n");
        return;
    }

    // Créer le chemin de destination pour l'extraction
    char dest_path[256];
    snprintf(dest_path, sizeof(dest_path), "%s/%s", get_current_directory(), file_name);

    // Extraire le fichier avec mot de passe
    int extract_result = zip_fextract(zip_file_entry, dest_path, ZIP_FL_OVERWRITE | ZIP_FL_ENC_GUESS, password);
    if (extract_result != 0) {
        printw("Erreur lors de l'extraction du fichier.\n");
    } else {
        printw("Le fichier a été extrait avec succès.\n");
    }

    // Fermer le fichier dans le ZIP
    zip_fclose(zip_file_entry);
}

void affichageZIPpwd(const char* zip_file_path) {
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
                    // Sélection d'un fichier/dossier
                    const char* selected_file = get_file_name(zip_file, highlight - 2);
                    clear();

                    // Demander le mot de passe à l'utilisateur
                    char password[100];
                    printw("Entrez le mot de passe pour extraire le fichier : ");
                    echo();
                    getstr(password);
                    noecho();

                    // Extraire le fichier avec mot de passe
                    extraireAvecMotDePasse(zip_file, zip_file_path, selected_file, password);

                    // Attendre la saisie d'une touche pour continuer
                    printw("\nAppuyez sur une touche pour continuer...");
                    refresh();
                    getch();
                }
                break;
        }
    }

    close_zip_file(zip_file);
}
