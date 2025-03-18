#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void rmdir_command(const char *dir_name) {
    if (rmdir(dir_name) == -1) {
        perror("rmdir");
    } else {
        printf("Directorio '%s' eliminado con éxito.\n", dir_name);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <nombre_directorio>\n", argv[0]);
        return 1;
    }

    rmdir_command(argv[1]);
    return 0;
}
