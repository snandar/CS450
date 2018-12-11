/* THIS CODE IS MY OWN WORK, IT WAS WRITTEN WITHOUT CONSULTING
A TUTOR OR CODE WRITTEN BY OTHER STUDENTS - NANDAR SOE */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <memory.h>
#include <ctype.h>

int main(int argc, char **argv) {

    int afd[2]; //fd for piping process A
    int bfd[2]; //fd for piping process B

    pipe(afd); //make pipe A
    pipe(bfd); //make pipe B

    pid_t apid, bpid; //identifies the first and second fork children

    FILE *fp; //for parse stdin
    FILE *stream; //for surpress stdin

    apid = fork(); //first child
    // printf("forked");
    if(apid == -1){
        printf("fork error");
    }
    else if(apid == 0){ //child = handle sort
        // printf("child");

        //close irrelevant pipes
        close(afd[1]); //close write on A
        close(bfd[0]); //close read on B

        //relevant pipes
        dup2(afd[0], STDIN_FILENO); //redirecting pipe A readint to stdin
        dup2(bfd[1], STDOUT_FILENO); //redirecting pipe B writing to stdout
        close(afd[0]);
        close(bfd[1]);

        execl("/usr/bin/sort", "sort", (char *) NULL);
    }

    bpid = fork(); //second child

    if(bpid == -1){
        printf("fork error");
    }
    else if(bpid == 0){ //child == surpress is handled here

        //Variable declaration
        char currentword[100];
        char nextword[100];
        int count = 1;

        //irrelevant pipes
        close(afd[0]);
        close(afd[1]);
        close(bfd[1]);

        //relevant pipes
        dup2(bfd[0], STDIN_FILENO); //redirect the input from stdin
        close(bfd[0]);
        //printf("\n");

        fgets(currentword, 100, stdin);
        while(fgets(nextword, 100, stdin)!= NULL){

            int length = strlen(currentword);
            // printf("Current Word: %s\n", currentword);
            // printf("Next Word:%s\n", nextword);

            // multiple of first word
            if(strcmp(currentword, nextword) == 0){
                count++;
            }
            else{
                
                if(length > 5 && length <= 35){
                    printf("Word: %s", currentword);
                    printf("Multiplicity: %-5d \n\n", count);
                    strcpy(currentword, nextword);
                    count = 1;
                }

                else if(length > 35){
                    printf("Word: %.*s\n", 35, currentword);
                    printf("Multiplicity: %-5d \n\n", count);
                    strcpy(currentword, nextword);
                    count = 1;
                }
                else{
                    strcpy(currentword, nextword);
                    count = 1;
                }
            }

        }

        int length = strlen(currentword);
        if(length > 5 && length <= 35){
            printf("Word: %s", currentword);
            printf("Multiplicity: %-5d \n\n", count);
        }
        else if(length > 35){
            printf("Word: %.*s\n", 35, currentword);
            printf("Multiplicity: %-5d \n\n", count);
        }

        exit(0);
    }
    else{ //parent handles parsing
        // printf("parent");

        //close irrelevant pipes
        close(bfd[0]);
        close(bfd[1]);
        close(afd[0]);

        char c;

        //open a stream for writing into
        FILE *parse_stream = fdopen(afd[1], "w");

        //get each character from stdin until EOF
        while((c=fgetc(stdin)) != EOF){
            c = tolower(c);
            if(isalpha(c)!=0){
                fputc(c, parse_stream);
            }
            else if(c == ' '||c == '\n'||isalpha(c)==0){
                fputc('\n', parse_stream);
            }
        }
        fclose(parse_stream);
        close(afd[1]);

        while(wait(NULL)>0){}
    }

    return 0;

}