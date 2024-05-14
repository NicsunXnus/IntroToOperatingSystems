/**
 * CS2106 AY22/23 Semester 1 - Lab 2
 *
 * This file contains function definitions. Your implementation should go in
 * this file.
 */


#include "myshell.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>

struct PCBTable processes[50];
int numberOfProcesses;
int exitedP;
int runningP;
int terminatedP;
void my_init(void) {
    // Initialize what you need here
    numberOfProcesses = 0;
    exitedP=0;
    runningP=0;
    terminatedP = 0;
}
void helper_for_Chaining(size_t num_tokens, char **tokens) {
    // First check if the program is executable
    if (access(*tokens,X_OK) == -1 && strcmp(*tokens,"info") != 0 && strcmp(*tokens,"wait") != 0 && strcmp(*tokens,"terminate") != 0) {
        fprintf(stderr, "%s not found\n",*tokens);
        return;
    }
    
    //wait command
    if (strcmp(*tokens,"wait") == 0) {
        if (num_tokens != 3) {
            fprintf(stderr, "Wrong Command\n");
            return;
        }
        int waitPid = atoi(*(tokens + 1));
        for (int i = 0; i < numberOfProcesses + 1; i++) {
            if (processes[i].pid == waitPid) {
                if (processes[i].status == 2 ) { //running 
                    int status;          
                    waitpid(processes[i].pid,&status,0);
                    if (WIFEXITED(status)) {
                        processes[i].status = 1;
                        processes[i].exitCode = WEXITSTATUS(status);                    
                    }
                    break;
                }
            }
        }
        return;
    }
    
    //terminate command
    if (strcmp(*tokens,"terminate") == 0) {
        if (num_tokens != 3) {
            fprintf(stderr, "Wrong Command\n");
            return;
        }
        int terPid = atoi(*(tokens + 1));
        for (int i = 0; i < numberOfProcesses + 1; i++) {
            if (processes[i].pid == terPid) {
                if (processes[i].status == 2) { //running  
                    kill(processes[i].pid,SIGTERM);
                    processes[i].status = 3;
                    processes[i].exitCode = -1;
                    terminatedP++; runningP--;          
                    break;
                }
            }
        }
        return;
    }

    //Exit any finished running processes if any
    for (int i = 0; i < numberOfProcesses + 1; i++) {
        int status;        
        if (processes[i].status == 2 ) {
            waitpid(processes[i].pid,&status,WNOHANG);
            if (WIFEXITED(status)) {
                processes[i].status = 1;
                processes[i].exitCode = WEXITSTATUS(status);
                exitedP++; runningP--;
            }
        }
        if (processes[i].status == 3) {
            waitpid(processes[i].pid,&status,WNOHANG);
            if (WIFSIGNALED(status)) {
                processes[i].status = 1;
                if (WIFEXITED(status)) processes[i].exitCode = WEXITSTATUS(status);
                else processes[i].exitCode = WTERMSIG(status);
                exitedP++; terminatedP--;
            }
        }
    }
    // Info command
    if (strcmp(*tokens,"info") == 0) {

        char* option = *(tokens+1);
        if (strcmp(option,"0") != 0 && strcmp(option,"1") != 0 && strcmp(option, "2") != 0 && strcmp(option,"3") != 0) {
            fprintf(stderr, "Wrong Command\n");
            return;
        }
        if (strcmp(option,"0") == 0) {
            int status;
            for (int i = 0; i < numberOfProcesses; i++) {
                struct PCBTable p = processes[i];
                //4: stopped, 3:Terminating, 2: Running, 1:Exiting
                if (p.status == 1) {
                    printf("[%d] Exited %d\n",p.pid,p.exitCode);
                } else if (p.status == 2) {
                    printf("[%d] Running\n",p.pid);
                } else {
                    printf("[%d] Terminating\n",p.pid);
                }
            }
        } else if (strcmp(option,"1") == 0) {
            printf("Total exited processes: %d\n", exitedP);
        } else if  (strcmp(option,"2") == 0){
            printf("Total running processes: %d\n",runningP);
        } else {
            printf("Total terminating processes: %d\n", terminatedP);
        }
        return;
    }
    char* lastPtr = *(tokens + num_tokens-2);

    if (strcmp(lastPtr ,"&") == 0) {
        *(tokens + num_tokens - 2) = NULL;
    }

    // If executable, fork a child process
    pid_t pid = fork();
    if (pid == 0) { //child process
        //execute program
        if (num_tokens == 2) { // no args
            execv(*tokens, NULL);
        } else {
            execv(*tokens, tokens);
        }
        perror("execv");
        exit(1);
    } else { //parent
        if (strcmp(lastPtr ,"&") == 0)  { //Run in background if command is of type <program> <args...> &
            runningP++;
            processes[numberOfProcesses].pid = pid;
            processes[numberOfProcesses].status = 2;
            processes[numberOfProcesses].exitCode = -1;
            printf("Child [%d] in background\n", pid);
        }
        else { //wait for child process to finish
            int status;
            waitpid(pid,&status,0);
            if (WIFEXITED(status)) {
                processes[numberOfProcesses].pid = pid;
                processes[numberOfProcesses].status = 1;
                processes[numberOfProcesses].exitCode = WEXITSTATUS(status);
                exitedP++;
            }     
        }
        numberOfProcesses++;        
    }

}
void my_process_command(size_t num_tokens, char **tokens) {
    // Your code here, refer to the lab document for a description of the
    // arguments
    helper_for_Chaining(num_tokens,tokens);
    /*const char s[2] = ";";
    char* arr[5];
    int i = 0;
    while (num_tokens >= 1) {
        if (num_tokens == 1) {
            arr[i] = NULL;
            helper_for_Chaining(sizeof(arr)/sizeof(arr[0]),arr);

        } else if (strcmp(*tokens,s) == 0) {
            arr[i] = NULL;
            helper_for_Chaining(sizeof(arr)/sizeof(arr[0]),arr);
            i =0; //reset
            memset(arr, 0, sizeof(arr));
            tokens++;
            num_tokens--;
        } else {
            arr[i++] = *tokens; tokens++; num_tokens--;
        }
    }*/
}

void my_quit(void) {
    // Clean up function, called after "quit" is entered as a user command
    for (int i = 0; i < numberOfProcesses; i++) {
        if (processes[i].status == 2) {
            kill(processes[i].pid,SIGTERM);
            printf("Killing [%d]",processes[i].pid);
        }
    }
    
    printf("\nGoodbye\n");
}
