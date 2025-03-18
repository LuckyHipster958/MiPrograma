#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>

void mkdir_command(const char *dir_name) {
    // Crear el directorio
    if (mkdir(dir_name, 0755) == -1) {
        perror("mkdir");  // Si falla, muestra el error
    } else {
        printf("Directorio '%s' creado con éxito.\n", dir_name);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <nombre_directorio>\n", argv[0]);
        return 1;
    }

    mkdir_command(argv[1]); // Llamada a la función para crear el directorio
    return 0;
}
