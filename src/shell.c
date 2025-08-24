// ALL THE LIBRARIES USED

#include<stdio.h>
#include<stdlib.h>
#include <unistd.h>     
#include <sys/wait.h>   
#include <sys/types.h> 

// DEFINITIONS

#define LSH_RL_BUFSIZE 1024
#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"


// A SIMPLER APPROACH IS USING GETLINE() BUT IT IS NOT A TRIVIAL SOLUTION

char *lsh_read_line(void){
    
    int bufsize = LSH_RL_BUFSIZE;
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

        if(c == 'EOF' || c == '\n'){

            buffer[position]='\0';
            return buffer;

        } else {

            buffer[position] = c;
        
        }

        position++;

        // IN THE CASE BUFFER HAS BEEN EXCEEDED , REALLOCATE AND PROCEED

        if(position>=bufsize){
            
            bufsize += LSH_RL_BUFSIZE;
            buffer = realloc(buffer,bufsize);

            // SANITY CHECK

            if(!buffer){
                printf("Memory Allocation Failed , Please Try Again :< ");
                exit(EXIT_FAILURE);
            }
        }
    }
}

char **lsh_split_line(char *line){

    int bufsize = LSH_TOK_BUFSIZE;
    int position = 0;

    char **tokens = malloc(sizeof(char*)*bufsize);
    char *token;

    if(!tokens){
        printf("Memory Allocation Failed , Please Try Again :< ");
        exit(EXIT_FAILURE);     
    }

    token = strtok(line, LSH_TOK_DELIM);

    while(token != NULL){

        tokens[position] = token;
        position++;

        // AGAIN HANDLING EDGE CASES

        if(position >= bufsize){
            bufsize += LSH_TOK_BUFSIZE;
            tokens = realloc(tokens,bufsize*(sizeof(char*)));

            if(!tokens){
                printf("Memory Allocation Failed , Please Try Again :< ");
                exit(EXIT_FAILURE);     
            }  
        }

        token = strtok(NULL, LSH_TOK_DELIM);

    }

    tokens[position] = NULL;
    return tokens;
}

int lsh_launch(char **args){

    pid_t pid , wpid;
    int status;

    pid = fork();

    if(pid == 0){

        // CHILD PROCESS

        if(execvp(args[0],args) == -1){
            perror("lsh");
        }

        exit(EXIT_FAILURE);

    } else if (pid<0){

        // FORKING ERROR

        perror("lsh");

    } else {

        do {
            wpid = waitpid(pid, &status , WUNTRACED);
        } while(!WIFEXITED(status) && !WIFESIGNALED(status));
    }

    return 1;
}

void lsh_loop(void){
    
    char *line;
    char **args;
    int status;

    do {
        printf("-> ");
        line = lsh_read_line();
        args = lsh_split_line(line);
        status = lsh_execute(args);

        free(line);
        free(args);

    } while(status);
}

// MAIN PROGRAM

int main(int argc, char **argv){

    // COMMAND LOOP

    lsh_loop();

    // SHUTDOWN OR CLEANUP

    return EXIT_SUCCESS;
}