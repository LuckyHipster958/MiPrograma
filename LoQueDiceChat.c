#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

char error_message[] = "An error has occurred\n";

void ejecutacomando(char *texto) {
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
        return;
    }

    // Tokenizar la línea para obtener los argumentos y detectar ">"
    char *comando = strtok(texto, " ");
    while (comando != NULL) {
        if (strcmp(comando, ">") == 0) {
            redirigir_salida = true;
            comando = strtok(NULL, " ");
            if (comando == NULL || strtok(NULL, " ") != NULL) {
                fprintf(stderr, "%s", error_message);
                return;
            }
            archivo_salida = comando;
            break;
        } else {
            argayu[i++] = comando;
        }
        comando = strtok(NULL, " ");
    }
    argayu[i] = NULL;

    if (argayu[0] == NULL) return;

    // Crear proceso hijo
    if (fork() == 0) { // Proceso hijo
        if (redirigir_salida) {
            int fd = open(archivo_salida, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0) {
                fprintf(stderr, "%s", error_message);
                exit(1);
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }
        execvp(argayu[0], argayu);
        fprintf(stderr, "%s", error_message);
        exit(1);
    }
}

void ejecutacomandos(char *texto) {
    char *comando;
    char *resto = texto;
    pid_t pids[128]; // Almacena los pids de los procesos hijos
    int count = 0;

    while ((comando = strsep(&resto, "&")) != NULL) {
        if (strlen(comando) == 0) continue;

        pid_t pid = fork();
        if (pid == 0) { // Proceso hijo
            ejecutacomando(comando);
            exit(0);
        } else if (pid < 0) {
            fprintf(stderr, "%s", error_message);
        } else {
            pids[count++] = pid;
        }
    }

    // Esperar a todos los hijos
    for (int i = 0; i < count; i++) {
        waitpid(pids[i], NULL, 0);
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
            if (nread == -1) break;

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

