#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

char error_message[] = "An error has occurred\n";

void ejecutar_comando(char *comando, bool redirigir_salida, char *archivo_salida) {
    char *argayu[12] = {NULL};
    int i = 0;
    char *token = strtok(comando, " ");

    while (token != NULL) {
        argayu[i++] = token;
        token = strtok(NULL, " ");
    }
    argayu[i] = NULL;

    if (argayu[0] == NULL) return; // No hay comando

    pid_t pid = fork();
    if (pid == 0) { // Proceso hijo
        if (redirigir_salida) {
            int fd = open(archivo_salida, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0) {
                perror("Error al abrir archivo de salida");
                exit(1);
            }
            dup2(fd, STDOUT_FILENO); // Redirigir salida estándar al archivo
            close(fd);
        }

        execvp(argayu[0], argayu);
        perror("Error al ejecutar comando"); // Si execvp falla
        exit(1);
    } else if (pid < 0) {
        perror("Error al crear proceso hijo");
        exit(1);
    }
}

void ejecutacomandos(char *texto) {
    char *comando = strtok(texto, "&");
    int cuentamper = 0;
    bool redirigir_salida = false;
    char *archivo_salida = NULL;

    while (comando != NULL) {
        cuentamper++;

        // Verificar si hay redirección
        char *redir_out = strstr(comando, ">");
        if (redir_out != NULL) {
            redirigir_salida = true;
            *redir_out = '\0'; // Eliminar ">" de la cadena
            comando = strtok(NULL, " ");
            if (comando != NULL) {
                archivo_salida = comando;
            }
        }

        // Crear un proceso hijo para ejecutar el comando
        ejecutar_comando(comando, redirigir_salida, archivo_salida);

        // Resetear valores para el siguiente comando
        redirigir_salida = false;
        archivo_salida = NULL;

        // Pasar al siguiente comando
        comando = strtok(NULL, "&");
    }

    // Esperar a que todos los procesos hijos terminen
    for (int i = 0; i < cuentamper; i++) {
        wait(NULL);
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

