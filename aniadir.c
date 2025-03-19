if (strcmp(args[i], ">>") == 0) {
    int fd = open(args[i + 1], O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd == -1) {
        perror("Error abriendo archivo");
        return 1;
    }
    dup2(fd, STDOUT_FILENO);
    close(fd);
    args[i] = NULL; // Eliminar ">>" y el nombre del archivo de args
}
