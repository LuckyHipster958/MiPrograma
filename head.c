#include <stdio.h>
#include <stdlib.h>

void head_command(const char *filename, int n) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("No se puede abrir el archivo");
        return;
    }

    char line[1024];
    int count = 0;

    while (fgets(line, sizeof(line), file) != NULL) {
        printf("%s", line);
        count++;
        if (count == n) {
            break;
        }
    }

    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc < 2 || argc > 3) {
        fprintf(stderr, "Uso: %s <archivo> [número de líneas]\n", argv[0]);
        return 1;
    }

    int lines = 10;  // Valor predeterminado
    if (argc == 3) {
        lines = atoi(argv[2]);
        if (lines <= 0) {
            fprintf(stderr, "Número de líneas no válido\n");
            return 1;
        }
    }

    head_command(argv[1], lines);
    return 0;
}
