#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/time.h>
#include <time.h>
#include "util.h"
#include <stdbool.h>

#define MAX_THREADS 100
#define MAX_queue_len 100
#define MAX_CE 100
#define INVALID -1
#define BUFF_SIZE 1024

/*
  THE CODE STRUCTURE GIVEN BELOW IS JUST A SUGESTION. FEEL FREE TO MODIFY AS NEEDED
*/

// structs:
typedef struct request {
	int fd;
	char *request;
	struct request *next;
} request_t;

typedef struct request_queue {
	int size;
	int capacity;
	request_t *front;
	request_t *rear;
} request_queue_t;

typedef struct cache_entry {
	int len;
	char *request;
	char *content;
} cache_entry_t;

/* ************************************ Queue methods ********************************/
void initQueue(request_queue_t *q, int capacity) {
	q->size = 0;
	q->capacity = capacity;
	q->front = NULL;
	q->rear = NULL;
}

void enqueue(request_queue_t *q, request_t *r) {
	printf("--- enqueueing ---\n");
	if (q->rear == NULL) {
		q->front = q->rear = r;
	}
	else {
		q->rear->next = r;
		q->rear = r;
	}
	q->size++;
}

request_t* dequeue(request_queue_t *q) {
	printf("--- dequeueing ---\n");
	if (q->front == NULL) {
		return NULL;
	}
	else {
		request_t *returnRequest = q->front;
		q->front = q->front->next;
		q->size--;
		return returnRequest;
	}
}

int isQueueFull(request_queue_t q) {
	return q.size == q.capacity;
}

void printQueue(request_queue_t q) {
	printf("--- print ---\n");
	printf("size: %d\n", q.size);
	request_t *ptr = q.front;
	int i = 0;
	while (ptr != NULL) {
		printf("%d: %d\n", i, ptr->request);
		ptr = ptr->next;
		i++;
	}
	printf("-------------\n");
}
/**********************************************************************************/

// globals:
request_queue_t req_q;
static pthread_mutex_t req_q_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t req_q_free_slot = PTHREAD_COND_INITIALIZER;

/* ************************ Dynamic Pool Code ***********************************/
// Extra Credit: This function implements the policy to change the worker thread pool dynamically
// depending on the number of requests
void * dynamic_pool_size_update(void *arg) {
  while(1) {
    // Run at regular intervals
    // Increase / decrease dynamically based on your policy
  }
}
/**********************************************************************************/

/* ************************************ Cache Code ********************************/

// Function to check whether the given request is present in cache
int getCacheIndex(char *request){
  /// return the index if the request is present in the cache
}

// Function to add the request and its file content into the cache
void addIntoCache(char *mybuf, char *memory , int memory_size){
  // It should add the request at an index according to the cache replacement policy
  // Make sure to allocate/free memeory when adding or replacing cache entries
}

// clear the memory allocated to the cache
void deleteCache(){
  // De-allocate/free the cache memory
}

// Function to initialize the cache
void initCache(){
  // Allocating memory and initializing the cache array
}

// Function to open and read the file from the disk into the memory
// Add necessary arguments as needed
int readFromDisk(/*necessary arguments*/) {
  // Open and read the contents of file given the request
}

/**********************************************************************************/

/* ************************************ Utilities ********************************/
// Function to get the content type from the request
char* getContentType(char * mybuf) {
  // Should return the content type based on the file type in the request
  // (See Section 5 in Project description for more details)
}

// This function returns the current time in milliseconds
int getCurrentTimeInMills() {
  struct timeval curr_time;
  gettimeofday(&curr_time, NULL);
  return curr_time.tv_usec;
}

/**********************************************************************************/

// Function to receive the request from the client and add to the queue
void * dispatch(void *arg) {
	char buf[BUFF_SIZE];
	while (1) {

		// Accept client connection
		int fd;
		if ((fd = accept_connection()) < 0) {
			//ignore
		}
		else {
			// Get request from the client
			if (get_request(fd, buf) != 0) {
				printf("couldn't handle request for %s", buf);
			}
			else {
				// Add the request into the queue
				pthread_mutex_lock(&req_q_mutex);
					if (isQueueFull(req_q)) {
						pthread_cond_wait(&req_q_free_slot, &req_q_mutex);
					}
					request_t temp = {fd, buf};
					enqueue(&req_q, &temp);
					printQueue(req_q);
					//maybe signal worker
				pthread_mutex_unlock(&req_q_mutex);
			}
		}
	}
	return NULL;
}

/**********************************************************************************/

// Function to retrieve the request from the queue, process it and then return a result to the client
void * worker(void *arg) {

   while (1) {

    // Start recording time

    // Get the request from the queue

    // Get the data from the disk or the cache

    // Stop recording the time

    // Log the request into the file and terminal

    // return the result
  }
  return NULL;
}

/**********************************************************************************/

int main(int argc, char **argv) {

  // Error check on number of arguments
  if(argc != 8){
    printf("usage: %s port path num_dispatcher num_workers dynamic_flag queue_length cache_size\n", argv[0]);
    return -1;
  }

  // Get the input args
	char *in_port, *in_path, *in_num_dispatcher, *in_num_workers, *in_dynamic_flag, *in_queue_length, *in_cache_size;
	int port, num_dispatcher, num_workers, dynamic_flag, queue_length, cache_size;
	char *path; //not sure if this is right
	in_port = argv[1];
	in_path = argv[2];
	in_num_dispatcher = argv[3];
	in_num_workers = argv[4];
	in_dynamic_flag = argv[5];
	in_queue_length = argv[6];
	in_cache_size = argv[7];

  // Perform error checks on the input arguments
	if ((port = atoi(in_port)) == 0) {
		printf("bad port parameter: %s\n", in_port);
		return -1;
	}
	struct stat test_path_stat;
	stat(in_path, &test_path_stat);
	if (S_ISDIR(test_path_stat.st_mode) == 0) {
		printf("bad directory path: %s\n", in_path);
		return -1;
	}
	if ((num_dispatcher = atoi(in_num_dispatcher)) == 0) {
		printf("bad num_dispatcher: %s\n", in_num_dispatcher);
		return -1;
	}
	else if (num_dispatcher > MAX_THREADS) {
		printf("bad num_dispatcher: %d (max dispatch threads %d)\n", num_dispatcher, MAX_THREADS);
		return -1;
	}

	// more checks here
	
	init(port);

  // Change the current working directory to server root directory

  // Start the server and initialize cache
	initQueue(&req_q, queue_length);

  // Create dispatcher and worker threads
	pthread_t dispatch_threads[num_dispatcher];
	for (int i = 0; i < num_dispatcher; i++) {
		if (pthread_create(&dispatch_threads[i], NULL, dispatch, NULL)) {
			printf("error creating dispatcher thread %d\n", i);
			return -1;
		}
		if (pthread_join(dispatch_threads[i], NULL)) {
			printf("error joining dispatcher thread %d\n", i);
			return -1;
		}
	}

	pthread_t worker_threads[num_workers];
	for (int i = 0; i < num_workers; i++) {
		if (pthread_create(&worker_threads[i], NULL, worker, NULL)) {
			printf("error creating worker thread %d\n", i);
			return -1;
		}
		if (pthread_join(worker_threads[i], NULL)) {
			printf("error joining worker thread %d\n", i);
			return -1;
		}
	}

  // Clean up
  return 0;
}
