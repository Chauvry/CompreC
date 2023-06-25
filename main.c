#include <stdio.h>
#include "zip/gestionZip.h"
#include "menu/menu.h"

int main(int argc, char *argv[]) {

    //Récupération du fichierZIP en argument
    const char* zip_file_path = argv[1];   

    afficherMenu(zip_file_path);

    return 0;
}
