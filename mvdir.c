#include <stdio.h>
#include <stdlib.h>

void mv_command(const char *source, const char *destination) {
    if (rename(source, destination) == -1) {
        perror("Error al mover archivo");
    } else {
        printf("Archivo '%s' movido a '%s'\n", source, destination);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <archivo_fuente> <archivo_destino>\n", argv[0]);
        return 1;
    }

    mv_command(argv[1], argv[2]);
    return 0;
}
