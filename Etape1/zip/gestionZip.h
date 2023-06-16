#ifndef GESTIONZIP
#define GESTIONZIP

#include <stdio.h>
#include <zip.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


struct zip* open_zip_file(const char* zip_file_path);
int is_directory(struct zip* archive, zip_uint64_t index);
int get_file_stat(struct zip* zip_file, const char* file_name, struct zip_stat* file_stat);
int get_num_files(struct zip* zip_file);
const char* get_file_name(struct zip* zip_file, int index);
void close_zip_file(struct zip* zip_file);
void extraireContenuZIP(struct zip* zip_file, const char* zip_file_path, const char* entry_name);
void afficherContenuZIP(const char* zip_file_path);

void affichageZIP(const char* zip_file_path);

#endif /* GESTIONZIP */
