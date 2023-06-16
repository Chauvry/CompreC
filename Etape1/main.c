#include <stdio.h>
#include "zip/gestionZip.h"
#include "menu/menu.h"

int main() {
    const char* zip_file_path = "attrapemoi.zip";
    const char* password = "password";

    afficherMenu(zip_file_path,password);

    return 0;
}
