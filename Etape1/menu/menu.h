#ifndef MENU_H
#define MENU_H

#include "../zip/gestionZip.h"

// Affiche le contenu du fichier ZIP
void afficherContenuZIP(const char* zip_file_path);

// Extraction d'un contenu ciblé du fichier ZIP
void extraireContenuZIP(const char* zip_file_path);

// Sous-menu pour le choix du mode d'extraction (sans mot de passe, mots de passe connus, bruteforce)
void sousMenuExtraction(struct zip* zip_file, const char* entry_name);

// Fonction principale pour afficher le menu et gérer les options
void afficherMenu(const char* zip_file_path);

#endif
