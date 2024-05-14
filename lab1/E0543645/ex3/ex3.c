/*************************************
* Lab 1 Exercise 3
* Name: Nicholas Sun
* Student No: A0216509B
* Lab Group: B04
*************************************/

#include <stdio.h>
#include <stdlib.h>

#include "function_pointers.h"
#include "node.h"

// The runner is empty now! Modify it to fulfill the requirements of the
// exercise. You can use ex2.c as a template

// DO NOT initialize the func_list array in this file. All initialization
// logic for func_list should go into function_pointers.c.

// Macros
#define SUM_LIST 0
#define INSERT_AT 1
#define DELETE_AT 2
#define SEARCH_LIST 3
#define REVERSE_LIST 4
#define RESET_LIST 5
#define LIST_LEN 6
#define MAP 7

void run_instruction(list* lst, int instr, char str[]);
void print_list(list* lst);
void print_index(int index);

int main(int argc, char *argv[]) { 
    if (argc != 2) {
        fprintf(stderr, "Error: expecting 1 argument, %d found\n", argc - 1);
        exit(1);
    }
    list* lst = malloc(sizeof(list));
    lst->head = NULL;

    // We read in the file name provided as argument
    //int instr;
    FILE* fp;

    fp = fopen(argv[1], "r"); //from ./ex3 A > B, reads A
    if (NULL == fp) {
        printf("file can't be opened \n");
       return 0;
    }
    int instr;

    char str[1500000];
    
    while (fgets(str, 1500000, fp)) {
        sscanf(str, "%d ", &instr);
        if (instr < 0) {
            break;
        }
        run_instruction(lst, instr, str);
        instr = -1;
    }
    fclose(fp);
 
    reset_list(lst);
    free(lst);
}

// Takes an instruction enum and runs the corresponding function
// We assume input always has the right format (no input validation on runner)
void run_instruction(list* lst, int instr, char str[]) {
    int index, data, element, function_index;
    switch (instr) {
    case SUM_LIST:
        sum_list(lst);
        break;
    case INSERT_AT:
        sscanf(str, "%d %d %d ", &instr, &index, &data);
        insert_node_at(lst, index, data);
        //print_list(lst);
        break;
    case DELETE_AT:
        sscanf(str, "%d %d ", &instr, &index);
        delete_node_at(lst, index);
        break;
    case SEARCH_LIST:
        //printf("why\n");
        sscanf(str, "%d %d ", &instr, &element);
        int ind = search_list(lst, element);
        print_index(ind);
        break;
    case REVERSE_LIST:
        reverse_list(lst);
        break;
    case RESET_LIST:
        reset_list(lst);
        break;
    case LIST_LEN:
        list_len(lst);
        break;
    case MAP:
        sscanf(str, "%d %d ", &instr, &function_index);
        int (*f)(int);
        f = func_list[function_index];
        map(lst, f);
        break;
    default:
        break;
    }
}

// Prints out the whole list in a single line
void print_list(list* lst) {
    if (lst->head == NULL) {
        printf("[ ]\n");
        return;
    }

    printf("[ ");
    node* curr = lst->head;
    do {
        printf("%d ", curr->data);
        curr = curr->next;
    } while (curr != NULL);
    printf("]\n");
}

//Print index
void print_index(int index) {
    if (index == -2) {
        printf("{}\n");
    } else {
        printf("{%d}\n", index);
    }
}