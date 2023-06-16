#ifndef MENU_H
#define MENU_H

#include "../zip/gestionZip.h"
#include <ncurses.h>


// Sous-menu pour le choix du mode d'extraction (sans mot de passe, mots de passe connus, bruteforce)
void sousMenuExtraction(const char* zip_file, const char* password);

// Fonction principale pour afficher le menu et gérer les options
void afficherMenu(const char* zip_file_path, const char* password);

#endif
