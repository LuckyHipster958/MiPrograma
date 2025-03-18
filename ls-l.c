#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

void print_permissions(mode_t mode) {
    // Permisos en forma de "rwxr-xr-x"
    char perms[10] = "---------";
    
    // Usuario: lectura, escritura, ejecución
    if (mode & S_IRUSR) perms[0] = 'r';  // Propietario: lectura
    if (mode & S_IWUSR) perms[1] = 'w';  // Propietario: escritura
    if (mode & S_IXUSR) perms[2] = 'x';  // Propietario: ejecución

    // Grupo: lectura, escritura, ejecución
    if (mode & S_IRGRP) perms[3] = 'r';  // Grupo: lectura
    if (mode & S_IWGRP) perms[4] = 'w';  // Grupo: escritura
    if (mode & S_IXGRP) perms[5] = 'x';  // Grupo: ejecución

    // Otros: lectura, escritura, ejecución
    if (mode & S_IROTH) perms[6] = 'r';  // Otros: lectura
    if (mode & S_IWOTH) perms[7] = 'w';  // Otros: escritura
    if (mode & S_IXOTH) perms[8] = 'x';  // Otros: ejecución

    // Mostrar los permisos
    printf("%s ", perms);
}

void list_directory(const char *dir_path) {
    DIR *dir = opendir(dir_path);
    if (!dir) {
        perror("No se pudo abrir el directorio");
        return;
    }

    struct dirent *entry;
    struct stat file_stat;
    char full_path[1024];

    // Leer cada entrada en el directorio
    while ((entry = readdir(dir)) != NULL) {
        // Ignorar "." y ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);
        
        // Obtener los detalles del archivo
        if (stat(full_path, &file_stat) == 0) {
            // Mostrar los permisos
            print_permissions(file_stat.st_mode);
            
            // Mostrar el nombre del archivo
            printf("%s\n", entry->d_name);
        }
    }

    closedir(dir);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <directorio>\n", argv[0]);
        return 1;
    }

    list_directory(argv[1]);
    return 0;
}
