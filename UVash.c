#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "string.h"
#include "unistd.h"
#include "sys/wait.h"
char path[1024];
char error_message[30]="An error has occurred\n";
void ejecutacomandos(ssize_t nread, char* texto){
	char *argayu[12] = {NULL};
	int i=0;
	if(texto[nread-1]=='\n'){
		texto[nread-1]='\0';	
	}
	if(texto[0]=='c' && texto[1]=='d'){
		char *comando;
		while(texto!=NULL){
			comando=strsep(&texto, " ");
			argayu[i++]=comando;
		}
		if(argayu[1]==NULL){
			fprintf(stderr, "%s", error_message);
			exit(0);
		}else if(argayu[2]==NULL){
			chdir(argayu[1]);
		}else{
		fprintf(stderr, "%s", error_message);
		exit(0);
	}

	}else{
	while(texto!=NULL){
		char *argumento = strsep(&texto, " ");
		if (argumento==NULL){
			break;
		}
		argayu[i++]=argumento;
	}
	argayu[i]=NULL;
	snprintf(path, sizeof(path), "/bin/%s",texto);
	pid_t pid=fork();
	if(pid==0){
		execvp(argayu[0], argayu);
	}else if(pid>0){
		wait(NULL);
	}else{
		fprintf(stderr, "%s", error_message);
		exit(0);
	}
	}		
}
int main (int argc, char *argv[]){
	if(argc==1){
		char* texto=NULL;
		size_t line=0;
		ssize_t nread;
		int argcount=0;
		char* ayudanueva[12] = {NULL};
		while(true){
			printf("UVash> ");
			nread=getline(&texto,&line,stdin);
			if(texto[nread-1]=='\n'){
				texto[nread-1]='\0';	
			}
			char *aux=strdup(texto);
			char *comando;	
			while(aux!=NULL){
			comando=strsep(&aux, " ");
			ayudanueva[argcount++]=comando;
		}
			if(strcmp(ayudanueva[0],"exit")==0){
			if(ayudanueva[1]!=NULL){
			fprintf(stderr, "%s", error_message);
			exit(0);
		}
				free(texto);
				return 0;
			}
			ejecutacomandos(nread, texto);	
	}	}else if(argc==2){
		FILE* fichero;
		fichero=fopen(argv[1],"r");
		if(fichero){
			char* texto;
			size_t line=0;
			ssize_t nread;	
			int argcount=0;
			char* ayudanueva[12] = {NULL};
			while((nread=getline(&texto,&line,fichero))!=-1){
				if(texto[nread-1]=='\n'){
				texto[nread-1]='\0';	
			}char *aux=strdup(texto);
			char *comando;	
			while(aux!=NULL){
			comando=strsep(&aux, " ");
			ayudanueva[argcount++]=comando;
		}
			if(strcmp(ayudanueva[0],"exit")==0){
			if(ayudanueva[1]!=NULL){
			fprintf(stderr, "%s", error_message);
			exit(0);
		}
				free(texto);	return 0;
			}ejecutacomandos(nread,texto);
			}
		}	
	}else{
		fprintf(stderr, " %s",error_message);
		exit(0);
	}
}
