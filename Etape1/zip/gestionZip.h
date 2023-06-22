#ifndef GESTIONZIP_H
#define GESTIONZIP_H

#include <stdio.h>
#include <zip.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <zlib.h>
#include <openssl/evp.h>
/**
 * Ouvre le fichier ZIP.
 *
 * zip_file_path : Chemin du fichier ZIP à ouvrir.
 *
 * Retourne une structure zip* représentant le fichier ZIP ou NULL en cas d'erreur.
 */
struct zip* open_zip_file(const char* zip_file_path);

/**
 * Ferme le fichier ZIP.
 *
 * zip_file : Structure zip* du fichier ZIP à fermer.
 */
void close_zip_file(struct zip* zip_file);

/**
 * Vérifie si un élément dans le fichier ZIP est un répertoire.
 *
 * archive : Structure zip* du fichier ZIP.
 * index : Index de l'élément à vérifier.
 *
 * Retourne 1 si l'élément est un répertoire, 0 sinon.
 */
int is_directory(struct zip* archive, zip_uint64_t index);

/**
 * Récupère les informations sur un fichier dans le fichier ZIP.
 *
 * zip_file : Structure zip* du fichier ZIP.
 * file_name : Nom du fichier à inspecter.
 * file_stat : Structure zip_stat dans laquelle les informations seront stockées.
 *
 * Retourne 0 en cas de succès, -1 en cas d'erreur.
 */
int get_file_stat(struct zip* zip_file, const char* file_name, struct zip_stat* file_stat);

/**
 * Récupère le nombre de fichiers dans le fichier ZIP.
 *
 * zip_file : Structure zip* du fichier ZIP.
 *
 * Retourne le nombre de fichiers ou -1 en cas d'erreur.
 */
int get_num_files(struct zip* zip_file);

/**
 * Récupère le nom d'un fichier dans le fichier ZIP.
 *
 * zip_file : Structure zip* du fichier ZIP.
 * index : Index du fichier.
 *
 * Retourne le nom du fichier ou NULL en cas d'erreur.
 */
const char* get_file_name(struct zip* zip_file, int index);

/**
 * Affichage du menu pour l'extraction de fichiers du fichier ZIP sans mot de passe.
 *
 * zip_file_path : Chemin du fichier ZIP.
 */
void affichage_only(const char* zip_file_path);

/**
 * Affiche le contenu du fichier ZIP dans l'interface utilisateur sans mot de passe.
 *
 * zip_file_path : Chemin du fichier ZIP à afficher.
 */
void affichage_extraction_sans_password(const char* zip_file_path);

/**
 * Extrait le contenu d'un fichier du fichier ZIP sans mot de passe.
 *
 * zip_file : Structure zip* du fichier ZIP.
 * zip_file_path : Chemin du fichier ZIP.
 * selected_file : Nom du fichier à extraire.
 */
void extraction_sans_password(struct zip* zip_file, const char* zip_file_path, const char* selected_file);

/**
 * Affichage du menu pour l'extraction de fichiers avec mot de passe du fichier ZIP.
 *
 * zip_file_path : Chemin du fichier ZIP.
 * password : Mot de passe pour l'extraction.
 */
void affichage_extraction_avec_password(const char* zip_file_path, const char* password);

/**
 * Extrait un fichier avec un mot de passe du fichier ZIP.
 *
 * zip_file : Structure zip* du fichier ZIP.
 * zip_file_path : Chemin du fichier ZIP.
 * file_name : Nom du fichier à extraire.
 * password : Mot de passe pour l'extraction.
 */
void extraction_avec_password(struct zip* zip_file, const char* zip_file_path, const char* file_name, const char* password);

/**
 * Affichage du menu pour l'extraction de fichiers avec méthode brute force du fichier ZIP.
 *
 * zip_file_path : Chemin du fichier ZIP.
 * dictionary_path : Chemin du dictionnaire pour la méthode brute force.
 */
void affichage_extraction_brute_force(const char* zip_file_path, const char* dictionary_path);

/**
 * Extrait un fichier avec méthode brute force du fichier ZIP.
 *
 * zip_file : Structure zip* du fichier ZIP.
 * zip_file_path : Chemin du fichier ZIP.
 * selected_file : Nom du fichier à extraire.
 * dictionary_path : Chemin du dictionnaire pour la méthode brute force.
 */
void extraction_brute_force(struct zip* zip_file, const char* zip_file_path, const char* selected_file, const char* dictionary_path);


#endif /* GESTIONZIP_H */
