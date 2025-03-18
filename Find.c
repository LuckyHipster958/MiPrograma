#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

void find_file(const char *dir_path, const char *filename) {
    struct dirent *entry;
    struct stat file_stat;
    char full_path[1024];

    DIR *dir = opendir(dir_path);
    if (!dir) {
        perror("Error al abrir directorio");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        // Ignorar "." y ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);

        if (stat(full_path, &file_stat) == 0) {
            if (S_ISREG(file_stat.st_mode)) { // Si es un archivo
                if (strcmp(entry->d_name, filename) == 0)
                    printf("Encontrado: %s\n", full_path);
            } else if (S_ISDIR(file_stat.st_mode)) { // Si es un directorio, buscar recursivamente
                find_file(full_path, filename);
            }
        }
    }

    closedir(dir);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <directorio> <archivo_a_buscar>\n", argv[0]);
        return 1;
    }

    find_file(argv[1], argv[2]);
    return 0;
}
