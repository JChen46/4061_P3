#include <stdio.h>
#include <stdlib.h>

/*
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
*/

typedef struct Node {
	int val;
} Node;

typedef struct Queue {
	int size, capacity, front, rear;
	Node *n;
} Queue;

void initQueue(Queue *q, int capacity) {
	q->n = malloc(capacity * sizeof(Node));
	q->capacity = capacity;
	q->size = 0;
	q->front = 0;
	q->rear = 0;
}

void enqueue(Queue *q, Node n) {
	q->n[q->rear] = n;
	q->rear = (q->rear + 1) % q->capacity;
	q->size++;
}

Node dequeue(Queue *q) {
	Node returnNode = q->n[q->front];
	q->front = (q->front + 1) % q->capacity;
	q->size--;
	return returnNode;
}

int isQueueFull(Queue q) {
	return q.size == q.capacity;
}

int isQueueEmpty(Queue q) {
	return q.size == 0;
}

void printQueue(Queue q) {
	printf("--- print ---\n");
	printf("size: %d\n", q.size);
	for (int i = q.front; i != q.rear; i = (i + 1) % q.capacity) {
		printf("%d: %d\n", i, q.n[i].val);
	}
	printf("-------------\n");
}

int main() {
	Queue q;
	initQueue(&q, 4);
	printQueue(q);
	Node n1 = {10};
	enqueue(&q, n1);
	printQueue(q);
	n1.val = 9;
	Node n2 = {25};
	Node n3 = {26};
	Node n4 = {27};
	enqueue(&q, n2);
	enqueue(&q, n3);
	enqueue(&q, n4);
	printf("full? %d\n", isQueueFull(q));
	printQueue(q);
	dequeue(&q);
	printQueue(q);
	dequeue(&q);
	printQueue(q);
	dequeue(&q);
	printQueue(q);
	dequeue(&q);
	printQueue(q);
	printf("empty? %d\n", isQueueEmpty(q));
	return 0;
}
