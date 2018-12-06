#include <stdio.h>

typedef struct Node {
	int val;
	struct Node *next;
} Node;

typedef struct Queue {
	int size;
	int capacity;
	Node *front;
	Node *rear;
} Queue;

void initQueue(Queue *q) {
	q->size = 0;
	q->capacity = 0;
	q->front = NULL;
	q->rear = NULL;
}

void enqueue(Queue *q, Node *n) {
	printf("--- enqueueing ---\n");
	if (q->rear == NULL) {
		q->front = q->rear = n;
	}
	else {
		q->rear->next = n;
		q->rear = n;
	}
	q->size++;
}

Node* dequeue(Queue *q) {
	printf("--- dequeueing ---\n");
	if (q->front == NULL) {
		return NULL;
	}
	else {
		Node *returnNode = q->front;
		q->front = q->front->next;
		q->size--;
		return returnNode;
	}
}

void printQueue(Queue *q) {
	printf("--- print ---\n");
	printf("size: %d\n", q->size);
	Node *ptr = q->front;
	int i = 0;
	while (ptr != NULL) {
		printf("%d: %d\n", i, ptr->val);
		ptr = ptr->next;
		i++;
	}
	printf("-------------\n");
}

int main() {
	Queue q;
	initQueue(&q);
	printQueue(&q);
	Node n1 = {10, NULL};
	enqueue(&q, &n1);
	printQueue(&q);
	n1.val = 9;
	Node n2 = {25, NULL};
	Node n3 = {26, NULL};
	Node n4 = {27, NULL};
	enqueue(&q, &n2);
	enqueue(&q, &n3);
	enqueue(&q, &n4);
	printQueue(&q);
	dequeue(&q);
	printQueue(&q);
	dequeue(&q);
	printQueue(&q);
	dequeue(&q);
	printQueue(&q);
	dequeue(&q);
	printQueue(&q);
	dequeue(&q);
	printQueue(&q);
	return 0;
}
