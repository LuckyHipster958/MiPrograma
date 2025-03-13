#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

char error_message[] = "An error has occurred\n";

// Función para ejecutar un solo comando
void ejecutar_comando(char *comando, bool redirigir_salida, char *archivo_salida) {
    char *argayu[12] = {NULL};
    int i = 0;
    
    // Tokenizar la línea del comando
    char *token = strtok(comando, " ");
    while (token != NULL) {
        argayu[i++] = token;
        token = strtok(NULL, " ");
    }
    argayu[i] = NULL;

    if (argayu[0] == NULL) return; // No hay comando

    pid_t pid = fork();
    if (pid == 0) { // Proceso hijo
        // Redirección de salida
        if (redirigir_salida) {
            int fd = open(archivo_salida, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0) {
                fprintf(stderr, "%s", error_message);
                exit(0);
            }
            dup2(fd, STDOUT_FILENO); // Redirigir salida estándar al archivo
            close(fd);
        }

        execvp(argayu[0], argayu);
        fprintf(stderr, "%s", error_message); // Si execvp falla
        exit(0);
    } else if (pid > 0) { // Proceso padre
        int status;
        waitpid(pid, &status, 0);
    } else {
        fprintf(stderr, "%s", error_message);
        exit(0);
    }
}

// Función para contar cuántos comandos paralelos hay (por los ampersands '&') y procesar los comandos
void cuenta_comandos(char *texto) {
    char *comando = strtok(texto, "&");
    int cuentamper = 0; // Contador de comandos paralelos
    bool redirigir_salida = false;
    char *archivo_salida = NULL;

    // Iterar sobre los comandos en paralelo
    while (comando != NULL) {
        cuentamper++;

        // Verificar redirección de salida
        char *redir_out = strstr(comando, ">");
        if (redir_out != NULL) {
            redirigir_salida = true;
            *redir_out = '\0'; // Eliminar el ">" para que el archivo quede después
            comando = strtok(NULL, " ");
            if (comando != NULL) {
                archivo_salida = comando;
            }
        }

        // Ejecutar el comando con la información recopilada
        ejecutar_comando(comando, redirigir_salida, archivo_salida);

        // Resetear los flags para el siguiente comando
        redirigir_salida = false;
        archivo_salida = NULL;
        comando = strtok(NULL, "&"); // Obtener el siguiente comando
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

            cuenta_comandos(texto);  // Llamar a cuenta_comandos para procesar la línea
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
            cuenta_comandos(texto);  // Llamar a cuenta_comandos para procesar la línea
        }

        free(texto);
        fclose(fichero);
    } else {
        fprintf(stderr, "%s", error_message);
        exit(1);
    }

    return 0;
}


