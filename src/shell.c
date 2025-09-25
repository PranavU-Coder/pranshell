// ALL THE LIBRARIES USED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>     
#include <sys/wait.h>   
#include <sys/types.h> 

// DEFINITIONS

#define PRAN_RL_BUFSIZE 1024
#define PRAN_TOK_BUFSIZE 64
#define PRAN_TOK_DELIM " \t\r\n\a"

// ADDING BUILT-IN SHELL COMMANDS 

int pran_cd(char **args);
int pran_help(char **args);
int pran_exit(char **args);
 
char *builtin_str [] = {
  "cd",
  "help",
  "exit"
};

int (*builtin_func[]) (char **) = {
  &pran_cd,
  &pran_help,
  &pran_exit
};

int pran_num_builtins(){
  return sizeof (builtin_str)/sizeof (char *);
}

int pran_cd(char **args){
  if(args[1] ==  NULL){
    fprintf(stderr,"pran: expected argument to \"cd\" \n");
  } else {
    if (chdir(args[1])!=0){
      perror("pran");
    }
  }
  return 1;
}

int pran_help(char **args){
  int i;
  printf("Pranav Unni's Shell.");
  printf("Type program names and arguments, and then hit enter.\n");
  printf("The following are built in:\n");

  for (i = 0; i < pran_num_builtins(); i++) {
    printf("  %s\n", builtin_str[i]);
  }

  printf("use 'man' command to know more about how to use a certain program -> man grep to know more about how grep works"); 
  return 1;
}

int pran_exit(char **args){
  return 0;
}

// A SIMPLER APPROACH IS USING GETLINE() BUT IT IS NOT A TRIVIAL SOLUTION

char *pran_read_line(void){
    
    int bufsize = PRAN_RL_BUFSIZE;
    int position = 0;
    char *buffer = malloc(sizeof(char)*bufsize);
    int c;

    if(!buffer){
        printf("Memory Allocation Failed , Please Try Again :< ");
        exit(EXIT_FAILURE);
    }

    while(1){

        c = getchar();

        // IN THE CASE EOF IS REACHED , REPLACE IT WITH A NULL CHARACTER AND PROCEED

        if(c == EOF || c == '\n'){

            buffer[position]='\0';
            return buffer;

        } else {

            buffer[position] = c;
        
        }

        position++;

        // IN THE CASE BUFFER HAS BEEN EXCEEDED , REALLOCATE AND PROCEED

        if(position>=bufsize){
            
            bufsize += PRAN_RL_BUFSIZE;
            buffer = realloc(buffer,bufsize);

            // SANITY CHECK

            if(!buffer){
                printf("Memory Allocation Failed , Please Try Again :< ");
                exit(EXIT_FAILURE);
            }
        }
    }
}

char **pran_split_line(char *line){

    int bufsize = PRAN_TOK_BUFSIZE;
    int position = 0;

    char **tokens = malloc(sizeof(char*)*bufsize);
    char *token;

    if(!tokens){
        printf("Memory Allocation Failed , Please Try Again :< ");
        exit(EXIT_FAILURE);     
    }

    token = strtok(line, PRAN_TOK_DELIM);

    while(token != NULL){

        tokens[position] = token;
        position++;

        // AGAIN HANDLING EDGE CASES

        if(position >= bufsize){
            bufsize += PRAN_TOK_BUFSIZE;
            tokens = realloc(tokens,bufsize*(sizeof(char*)));

            if(!tokens){
                printf("Memory Allocation Failed , Please Try Again :< ");
                exit(EXIT_FAILURE);     
            }  
        }

        token = strtok(NULL, PRAN_TOK_DELIM);

    }

    tokens[position] = NULL;
    return tokens;
}

int pran_launch(char **args){

    pid_t pid , wpid;
    int status;

    pid = fork();

    if(pid == 0){

        // CHILD PROCESS

        if(execvp(args[0],args) == -1){
            perror("pran");
        }

        exit(EXIT_FAILURE);

    } else if (pid<0){

        // FORKING ERROR

        perror("pran");

    } else {

        do {
            wpid = waitpid(pid, &status , WUNTRACED);
        } while(!WIFEXITED(status) && !WIFESIGNALED(status));
    }

    return 1;
}


int pran_execute(char **args){

  int i;

  if(args[0]==NULL){
    return 1;
  }

  for(i=0;i<pran_num_builtins();i++){
    if(strcmp(args[0],builtin_str[i]) == 0) {
      return (* builtin_func[i])(args);
    }
  }

  return pran_launch(args);
}

void pran_loop(void){
    
    char *line;
    char **args;
    int status;

    do {
        printf("-> ");
        line = pran_read_line();
        args = pran_split_line(line);
        status = pran_execute(args);

        free(line);
        free(args);

    } while(status);
}

// MAIN PROGRAM

int main(int argc, char **argv){

    // COMMAND LOOP

    pran_loop();

    // SHUTDOWN OR CLEANUP

    return EXIT_SUCCESS;
}
