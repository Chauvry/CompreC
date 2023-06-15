#ifndef GESTIONZIP
#define GESTIONZIP

#include <stdio.h>
#include <zip.h>

struct zip* open_zip_file(const char* zip_file_path);
int get_num_files(struct zip* zip_file);
const char* get_file_name(struct zip* zip_file, int index);
void close_zip_file(struct zip* zip_file);

#endif /* GESTIONZIP */
