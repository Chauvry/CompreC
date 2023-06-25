CompréC
Description

CompréC is a command-line tool written in C that allows you to manage zip files. It provides various features to manipulate and interact with encrypted zip archives.
Features

    Bruteforce Password: CompréC offers an option to attempt bruteforce attacks on password-protected zip files, both with and without a dictionary. Upon successful bruteforce, the discovered password will be displayed.
    Decrypt Zip File: You can decrypt a zip file using a specified password.
    Interactive Exploration: Once the file is decrypted, CompréC enables interactive exploration of the archive.
    File Extraction: You can request the extraction of a file from the zip archive, which will be saved to the local filesystem.
    File Inclusion: CompréC allows you to include a file in the currently explored zip archive by providing its path.
    Help: The program provides a comprehensive "help" section that explains how to use it effectively.

Restrictions

    The usage of the system command is strictly forbidden.

Dependencies

To handle zip archives, CompréC utilizes the libzip library.
For improved user experience, you are required to choose one of the following libraries to assist with the interface:

    getopt
    ncurses
    GTK
    SDL

Usage

Here is an example of the input/output using getopt:

To open the help menu: