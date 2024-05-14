/*************************************
* Lab 1 Exercise 3
* Name: Nicholas Sun Jun Yang
* Student No: A0217609B
* Lab Group: B04
*************************************/

#include "node.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Add in your implementation below to the respective functions
// Feel free to add any headers you deem fit (although you do not need to)


// Traverses list and returns the sum of the data values
// of every node in the list.
long sum_list(list *lst) {
	if (lst->head == NULL) {
		printf("%d\n", 0);
		return 0;
	}
	else {
		node* curr = lst->head;
		long sum = 0;
		while (curr != NULL) {
			sum += curr->data;
			curr = curr->next;
		}
		printf("%ld\n", sum);
		return sum;
	}
}

// Traverses list and returns the number of data values in the list.
int list_len(list *lst) {
	if (lst->head == NULL) {
		printf("%d\n", 0);
		return 0;
	}
	else {
		node* curr = lst->head;
		int len = 0;
		while (curr != NULL) {
			len += 1;
			curr = curr->next;
		}
		printf("%d\n", len);
		return len;
	}

}



// Inserts a new node with data value at index (counting from head
// starting at 0).
// Note: index is guaranteed to be valid.
void insert_node_at(list *lst, int index, int data) {
	if (lst->head == NULL) {

		if (index == 0) {
			node* inserted_node = malloc(sizeof(node));
			inserted_node->data = data;
			inserted_node->next = NULL;
			lst->head = inserted_node;
		}
	}
	else {
		if (index == 0) {
			node* new_node = malloc(sizeof(node));

			/*put in the data */
			new_node->data = data;

			/*Make next of new node as head*/
			new_node->next = lst->head;

			/*move the head to point to the new node*/
			lst->head = new_node;
		}
		else {
			int len = 0;
			node* prev_node = lst->head;
			while (prev_node->next != NULL) {
				if (index == len + 1) {
					break;
				}
				prev_node = prev_node->next;
				len++;
			}


			//allocate new node 
			node* new_node = malloc(sizeof(node));

			//put in the data
			new_node->data = data;

			new_node->next = prev_node->next;

			prev_node->next = new_node;
		}
	}
}

// Deletes node at index (counting from head starting from 0).
// Note: index is guarenteed to be valid.

void delete_node_at(list *lst, int index) {
	if (lst->head != NULL) {
		if (index == 0) {
			lst->head = lst->head->next;
		}
		else {
			node* prev;
			node* to_be_deleted;
			prev = lst->head;
			to_be_deleted = lst->head->next;

			for (int i = 0; i < index; i++) {
				if (i + 1 == index) {
					prev->next = to_be_deleted->next;
					free(to_be_deleted);
					break;
				}
				else {
					prev = prev->next;
					to_be_deleted = to_be_deleted->next;
					if (prev == NULL || to_be_deleted == NULL) { // position was greater than number of nodes in the list
						lst->head = NULL;
						break;
					}
				}
			}
		}
	}


}

// Search list by the given element.
// If element not present, return -1 else return the index. If lst is empty return -2.

int search_list(list *lst, int element) {
	if (lst->head == NULL) {
		return -2;
	}
	node* curr;
	curr = lst->head;
	int index = 0;
	while (curr != NULL) {
		if (curr->data == element) {
			return index;
		}
		curr = curr->next;
		index++;
	}
	return -1;
}

// Reverses the list with the last node becoming the first node.
void reverse_list(list *lst) {
	if (lst->head != NULL) {
		node* prev = NULL;
		node* curr = lst->head;
		node* next = NULL;
		while (curr != NULL) {
			next = curr->next;
			curr->next = prev;
			prev = curr;
			curr = next;
		}
		lst->head = prev;
	}
}

// Resets list to an empty state (no nodes) and frees
// any allocated memory in the process
void reset_list(list *lst) {
	node* temp;

	while (lst->head != NULL) {
		temp = lst->head;
		lst->head = lst->head->next;
		free(temp);
	}
	lst->head = NULL;
}



// Traverses list and applies func on data values of
// all elements in the list.
void map(list *lst, int (*func)(int)) {
	if (lst->head != NULL) {
		node* curr = lst->head;
		while (curr != NULL) {
			curr->data = func(curr->data);
			curr = curr->next;
		}
	}
}
	

