#include <stdio.h>
#include "zip/gestionZip.h"
#include "menu/menu.h"

int main() {
    const char* zip_file_path = "attrapemoi.zip";

    afficherMenu(zip_file_path);

    return 0;
}
