#include <stdio.h>
#include <string.h>
#include <dirent.h>

int file_exists_in_dir(const char *dir_path, const char *substring) {
    struct dirent *entry;
    DIR *dir = opendir(dir_path);

    if (!dir) {
        perror("Error al abrir directorio");
        return 0; // Retorna 0 si no se pudo abrir el directorio
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strstr(entry->d_name, substring) != NULL) { // Si el nombre contiene la subcadena
            printf("Encontrado: %s\n", entry->d_name);
            closedir(dir);
            return 1; // Retorna 1 si encuentra al menos un archivo
        }
    }

    closedir(dir);
    return 0; // No encontrado
}

int main(int argc char *argv){
if(argc==3){
DIR *directorio=opendir(argv[1]);
if(directorio){
  file_exists_in_dir(directorio,argv[2]);
}else{
fprintf("No es un directorio válido");
}
}else{
fprintf("Error");
exit(1);
}
}
