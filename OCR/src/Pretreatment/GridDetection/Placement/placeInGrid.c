#include "placeInGrid.h"
#include <err.h>

void swap(struct Node* p1, struct Node* p2) {
	/*
	 * list -> linked list
	 * we swap p1 and p2 !! p1 > p2
	 * p1, p2 index of the values to switch
	 */
	struct Shape* t = p1->data;
	p1->data = p2->data;
	p2->data = t;
}

void sort_cluster(struct Node* lst) {
	for (struct Node* i = lst; i != NULL; i = i->next) {
		for (struct Node* j = i->next; j != NULL; j = j->next) {
			if (j->data->Cx < i->data->Cx) {
				swap(j, i);
			}
		}
	}
}



void place(Node*** list, int size, int* clusters_size) {
    /*
	 * Sort the list inside the cluster list
	 * list -> array of all the cluster 
	 * size -> length of list
	 */
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < clusters_size[i]; j++) {
			if (list[i][j] != NULL) {
				sort_cluster(list[i][j]);
			}
		}
	}
}



void find_grid(Node*** list, int size, int* clusters_size) {
	for (int i = 0; i < size; i++) {
		int a = LenNode(&list[i][0]);
		int b = a;
		int j = 0;
		while (j < clusters_size[i] && a == b) {
			b = LenNode(&list[i][j]);
			j++;
		}
		if (j >= clusters_size[i]) {
			Node** t = list[0];
			list[0] = list[i];
			list[i] = t;
			break;
		}
	}
}
