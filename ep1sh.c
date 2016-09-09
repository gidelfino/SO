#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/stat.h>
#include <sys/wait.h>


#define MAX_SIZE 	1024

int main()
{
	int  i, status;
    char path[MAX_SIZE], prompt[MAX_SIZE];
	char *input, *token, *comnd;
    char *param[MAX_SIZE];
	pid_t pid;

	input = NULL;

	/* Inicio do shell */
    while (1) {
    	if (getcwd(path, sizeof(path)) != NULL) {
    		sprintf(prompt, "(%s): ", path);
    		input = readline(prompt);

    		/* Se o input nao for vazio, adiciona-lo ao historico */
			if (input != NULL) {
				if (strcmp(input, "") == 0) continue;
				add_history(input);

				/* Tokenizacao do input entre comando e parametros */
				token = strtok(input, " ");
				comnd = token;
				param[0] = token;
				for (i = 1; token != NULL; i++) {
					token = strtok(NULL, " ");
					param[i] = token;
				}
			}
			else perror("readline()");
		}
 		else perror("getcwd()");

 		/* id -u */
 		if (strcmp(comnd, "id") == 0) {
 			if (strcmp(param[1], "-u") == 0)
 				printf("%d\n", (int)getuid());		
 		}

 		/* chmod <modo numerico> <arquivo no diretorio atual> */
 		else if (strcmp(comnd, "chmod") == 0) {
 			if (chmod(param[2], strtol(param[1], NULL, 8)) != 0)
 				perror("chmod()");
 		}

 		/* /bin/ls -1    /bin/date    ./ep1 <argumentos do EP1> */
 		else                      
 			switch (pid = fork()) {
				case -1: /* Erro */
					perror("fork()");
					exit(EXIT_FAILURE);
				case 0:  /* Processo filho */
					status = execve(comnd, param, 0);
					exit(status);
				default: /* Processo pai */
					if ((waitpid(pid, &status, 0)) < 0) {
						perror("waitpid()");
						exit(EXIT_FAILURE);
					}
			}
		
		if (input != NULL) free(input);
	}
	return 0;
}
