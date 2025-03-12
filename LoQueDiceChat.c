#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

char error_message[] = "An error has occurred\n";

void ejecutacomandos(char* texto) {
    char *argayu[12] = {NULL};
    int i = 0;

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

    // Tokenizar la línea para obtener los argumentos
    char *comando = strtok(texto, " ");
    while (comando != NULL) {
        argayu[i++] = comando;
        comando = strtok(NULL, " ");
    }
    argayu[i] = NULL;

    // Si no hay comando, salir
    if (argayu[0] == NULL) return;
    char *otrayuda=argayu[1];
    if (otrayuda==NULL||(access(otrayuda, F_OK )== 0)||otrayuda[0]=='-'||otrayuda[0]=='/'||otrayuda[0]=='>'){
    if(otrayuda[0]=='>'){
	    if(argayu[2]!=NULL){
	    FILE *fich=fopen(argayu[2],"w");
	    int fd=fileno(fich);
	    dup2(fd, STDOUT_FILENO); 
	    pid_t pid = fork();
    		if (pid == 0) { // Proceso hijo
        	execvp(argayu[0], argayu);
        	fprintf(stderr, "%s", error_message); // Solo se imprime si execvp falla
        	exit(1);
    	} else if (pid > 0) { // Proceso padre
       	 int status;
		waitpid(pid, &status, 0);
		if(WIFEXITED(status)&&WEXITSTATUS(status)!=0){
			exit(0);
			}
    		} else {
        	fprintf(stderr, "%s", error_message);
    		}
	    }else{
	    fprintf(stderr, "%s",error_message);
	    }
    }else{
    pid_t pid = fork();
    if (pid == 0) { // Proceso hijo
        execvp(argayu[0], argayu);
        fprintf(stderr, "%s", error_message); // Solo se imprime si execvp falla
        exit(1);
    } else if (pid > 0) { // Proceso padre
        int status;
	waitpid(pid, &status, 0);
	if(WIFEXITED(status)&&WEXITSTATUS(status)!=0){
		exit(0);
	}
    } else {
        fprintf(stderr, "%s", error_message);
    }
    }
    }else {
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
            exit(0);
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
        exit(0);
    }

    return 0;
}
