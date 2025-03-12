#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

char error_message[] = "An error has occurred\n";

void ejecutacomandos(char *texto) {
    char *argayu[12] = {NULL};
    int i = 0;
    bool redirigir_salida = false;
    char *archivo_salida = NULL;

    if (texto[strlen(texto) - 1] == '\n') {
        texto[strlen(texto) - 1] = '\0';
    }

    // Manejo especial del comando "cd"
    if (strncmp(texto, "cd", 2) == 0) {
        char *comando = strtok(texto, " ");
        while (comando != NULL) {
            argayu[i++] = comando;
            comando = strtok(NULL, " ");
        }

        if (argayu[1] == NULL || argayu[2] != NULL) {
            fprintf(stderr, "%s", error_message);
        } else {
            if (chdir(argayu[1]) != 0) {
                fprintf(stderr, "%s", error_message);
            }
        }
        return; // No seguir con execvp
    }

    // Tokenizar la línea para obtener los argumentos y detectar ">"
    char *comando = strtok(texto, " ");
    while (comando != NULL) {
        if (strcmp(comando, ">") == 0) {
            redirigir_salida = true;
            comando = strtok(NULL, " ");
            if (comando == NULL || strtok(NULL, " ") != NULL) { // Verificar que hay un solo archivo
                fprintf(stderr, "%s", error_message);
                return;
            }
            archivo_salida = comando;
            break;
        }
        argayu[i++] = comando;
        comando = strtok(NULL, " ");
    }
    argayu[i] = NULL;

    if (argayu[0] == NULL) return; // No hay comando

    // Crear proceso hijo para ejecutar el comando
    pid_t pid = fork();
    if (pid == 0) { // Proceso hijo
        if (redirigir_salida) {
            int fd = open(archivo_salida, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0) {
                fprintf(stderr, "%s", error_message);
                exit(1);
            }
            dup2(fd, STDOUT_FILENO); // Redirigir salida estándar al archivo
            close(fd);
        }

        execvp(argayu[0], argayu);
        fprintf(stderr, "%s", error_message); // Si execvp falla
        exit(1);
    } else if (pid > 0) { // Proceso padre
        int status;
        waitpid(pid, &status, 0);
    } else {
        fprintf(stderr, "%s", error_message);
    }
}

int main(int argc, char *argv[]) {
    if (argc == 1) { // Modo interactivo
        char *texto = NULL;
        size_t line = 0;
        ssize_t nread;

        while (true) {
            printf("UVash> ");
            fflush(stdout);

            nread = getline(&texto, &line, stdin);
            if (nread == -1) break; // Detectar EOF

            if (strcmp(texto, "exit\n") == 0) {
                free(texto);
                exit(0);
            }

            ejecutacomandos(texto);
        }
        free(texto);
    } else if (argc == 2) { // Modo batch (archivo de comandos)
        FILE *fichero = fopen(argv[1], "r");
        if (fichero == NULL) {
            fprintf(stderr, "%s", error_message);
            exit(1);
        }

        char *texto = NULL;
        size_t line = 0;
        ssize_t nread;
        while ((nread = getline(&texto, &line, fichero)) != -1) {
            if (strcmp(texto, "exit\n") == 0) {
                free(texto);
                fclose(fichero);
                exit(0);
            }
            ejecutacomandos(texto);
        }

        free(texto);
        fclose(fichero);
    } else {
        fprintf(stderr, "%s", error_message);
        exit(1);
    }

    return 0;
}
