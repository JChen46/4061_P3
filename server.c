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
#include <unistd.h>
#include "util.h"
#include <stdbool.h>
#include <signal.h>
#include <limits.h>

#define MAX_THREADS 100
#define MAX_QLEN 100
#define MAX_CE 100
#define INVALID -1
#define BUFF_SIZE 1024

/*
  THE CODE STRUCTURE GIVEN BELOW IS JUST A SUGESTION. FEEL FREE TO MODIFY AS NEEDED
*/
int cache_size;
int cache_counter = 0;
int request_counter = 0;
int lru_counter = 1;
// structs:
typedef struct request {
	int fd;
	int id;
	char request[BUFF_SIZE];
} request_t;

typedef struct request_queue {
	int size, capacity, front, rear;
	request_t *arr;
} request_queue_t;

typedef struct cache_entry {
	int len;
	char request[BUFF_SIZE];
	char *content;
	int lru;
} cache_entry_t;

/* ************************************ Queue methods ********************************/
// prints queue off in a list listing off size, then each element's fd and request string
void printQueue(request_queue_t q) {
	printf("--- print ---\n");
	printf("size: %d\n", q.size);
	int i;
	for (i = q.front; i != q.rear; i = (i + 1) % q.capacity) {
		printf("%d: %d, %s\n", i, q.arr[i].fd, q.arr[i].request);
	}
	printf("-------------\n");
}

// allocates entire queue's memory sets capacity, and 
void initQueue(request_queue_t *q, int capacity) {
	q->arr = malloc(capacity * sizeof(request_t));
	q->capacity = capacity;
	q->size = 0;
	q->front = 0;
	q->rear = 0;
}

void deleteQueue(request_queue_t *q) {
	free(q->arr);
	q = NULL;
}

void enqueue(request_queue_t *q, request_t r) {
	request_counter++;
	memcpy(&q->arr[q->rear], &r, sizeof(request_t));
	q->rear = (q->rear + 1) % q->capacity;
	q->size++;
}

request_t dequeue(request_queue_t *q) {
	request_t returnNode = q->arr[q->front];
	q->front = (q->front + 1) % q->capacity;
	q->size--;
	return returnNode;
}

int isQueueFull(request_queue_t q) {
	return q.size == q.capacity;
}

int isQueueEmpty(request_queue_t q) {
	return q.size == 0;
}
/**********************************************************************************/

// globals:
static request_queue_t req_q;
static pthread_mutex_t req_q_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t req_q_free_slot = PTHREAD_COND_INITIALIZER;
static pthread_cond_t req_q_full_slot = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t cache_mutex = PTHREAD_MUTEX_INITIALIZER;
static cache_entry_t *cache;
static FILE *fp;

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
	int i;
	for (i = 0; i < cache_size; i++){
		if (strcmp(cache[i].request, "") != 0 && strcmp(request, cache[i].request) == 0){
			return i;
		}
	}
	return -1;
}

// Function to add the request and its file content into the cache
void addIntoCache(char *request, char *content, int len){
  // It should add the request at an index according to the cache replacement policy
  // Make sure to allocate/free memeory when adding or replacing cache entries
	/*free(cache[cache_counter].content);
	cache[cache_counter].content = malloc(len);
	cache[cache_counter].len = len;
	strcpy(cache[cache_counter].request, request);
	memcpy(cache[cache_counter].content, content, len);
	cache_counter = (cache_counter + 1) % cache_size;*/
	int evict_lru = INT_MAX;
	int evict_line = 0;
	int i;
	for (i = 0; i < cache_size; i++){
		if (cache[i].lru < evict_lru){
			evict_line = i;
			evict_lru = cache[i].lru;
		}
	}
	cache[evict_line].lru = lru_counter++;
	free(cache[evict_line].content);
	cache[evict_line].content = malloc(len);
	cache[evict_line].len = len;
	strcpy(cache[evict_line].request, request);
	memcpy(cache[evict_line].content, content, len);
}

// clear the memory allocated to the cache
void deleteCache(){
  int i;
	for (i = 0; i < cache_size; i++){
		free(cache[i].content);
	}
	free(cache);
}

// Function to initialize the cache
void initCache(int size){
  // Allocating memory and initializing the cache array
	cache = malloc(sizeof(cache_entry_t) * size);
	int i;
	for (i = 0; i < size; i++){
		cache[i].lru = 0;
	}
	cache_size = size;
}

// removes leading slash
void removeLeadingSlash(char *in_request, char *request) {
	size_t str_len = strlen(in_request) + 1;
	int i;
	for (i = 1; i < str_len; i++) {
		request[i - 1] = in_request[i];
	}
}

// get size of file to choose size of buffer when reading file
int sizeOfFile(char *path) {
	struct stat stat_buf;
	if (stat(path, &stat_buf) != 0) {
		return -1;
	}
	else {
		return stat_buf.st_size;
	}
}

// Function to open and read the file from the disk into the memory
// Add necessary arguments as needed
int readFromDisk(char *request, char *buf, int size) {
  // Open and read the contents of file given the request
	int fd = open(request, O_RDONLY);
	int read_size;
	if ((read_size = read(fd, buf, size)) == -1) {
		return 0;
	}
	else {
		return 1;
	}
}

/**********************************************************************************/

/* ************************************ Utilities ********************************/
// Function to get the content type from the request
char* getContentType(char * mybuf) {
  // Should return the content type based on the file type in the request
  // (See Section 5 in Project description for more details)
	char *temp = strrchr(mybuf,'.');
	if (temp == NULL){
		return "text/plain";
	}
	if (strcmp(temp,".html") == 0 || strcmp(temp,".htm") == 0){
		return "text/html";
	}
	if (strcmp(temp,".jpg") == 0){
		return "image/jpeg";
	}
	if (strcmp(temp,".gif") == 0){
		return "image/gif";
	}
	return "text/plain";
}

// This function returns the current time in milliseconds
long int getCurrentTimeInMicro() {
  struct timeval curr_time;
  gettimeofday(&curr_time, NULL);
  return curr_time.tv_usec;
}

/**********************************************************************************/

// Function to receive the request from the client and add to the queue
void * dispatch(void *arg) {
	while (1) {

		// Accept client connection
		int fd;
		if ((fd = accept_connection()) < 0) {
			//ignore
		}
		else {
			char buf[BUFF_SIZE];
			// Get request from the client
			if (get_request(fd, buf) != 0) {
				printf("couldn't handle request for \"%s\"\n", buf);
			}
			else {
				//printf("got request: %s\n", buf);
				request_t temp;
				temp.fd = fd;
				removeLeadingSlash(buf, temp.request);
				// Add the request into the queue
				pthread_mutex_lock(&req_q_mutex);
					if (isQueueFull(req_q)) {
						pthread_cond_wait(&req_q_free_slot, &req_q_mutex);
					}
					temp.id = request_counter;
					enqueue(&req_q, temp);
					pthread_cond_signal(&req_q_full_slot);
				pthread_mutex_unlock(&req_q_mutex);
			}
		}
	}
	return NULL;
}

/**********************************************************************************/

// Function to retrieve the request from the queue, process it and then return a result to the client
void * worker(void *arg) {
	int thread_id = *((int*)arg);
	fp = fopen("webserver_log", "a");
	if (fp == NULL) {
		printf("error opening webserver_log file: %d\n", errno);
	}

  while (1) {
		long int startTime;
    // Get the request from the queue
		request_t req;
		pthread_mutex_lock(&req_q_mutex);
			if (isQueueEmpty(req_q)) {
				pthread_cond_wait(&req_q_full_slot, &req_q_mutex);
			}
			startTime = getCurrentTimeInMicro();
			req = dequeue(&req_q);
			pthread_cond_signal(&req_q_free_slot);
		pthread_mutex_unlock(&req_q_mutex);

    // Get the data from the disk or the cache
		char christmas[6];
		pthread_mutex_lock(&cache_mutex);
		int cache_index = getCacheIndex(req.request);
		pthread_mutex_unlock(&cache_mutex);
		int size;
		int error = 0;
		char error_buf[BUFF_SIZE];
		if (cache_index != -1) {
			strcpy(christmas, "HIT");
			size = cache[cache_index].len;
			if (return_result(req.fd, getContentType(req.request), cache[cache_index].content, size)) {
				continue;
			}
		}
		else {
			if ((size = sizeOfFile(req.request)) == -1) {
				printf("couldn't find file: \"%s\"\n", req.request);
				strcpy(error_buf, "couldn't find file.");
				error = 1;
				return_error(req.fd, error_buf);
			}
			else {
				char buf[size];
				if (readFromDisk(req.request, buf, size) == 0) {
					printf("error reading from disk file: %s\n", req.request);
					strcpy(error_buf, "error reading from disk.");
					error = 1;
					return_error(req.fd, error_buf);
				}
				else {
					strcpy(christmas, "MISS");
					return_result(req.fd, getContentType(req.request), buf, size);
					pthread_mutex_lock(&cache_mutex);
					addIntoCache(req.request, buf, size);
					pthread_mutex_unlock(&cache_mutex);
				}
			}
		}

    // Stop recording the time
		long int endTime = getCurrentTimeInMicro();

    // Log the request into the file and terminal
		if (error) {
			printf("[%d][%d][%d][%s][%s][%ldus][ERROR]\n", thread_id, req.id, req.fd, req.request, error_buf, endTime - startTime);
			fprintf(fp, "[%d][%d][%d][%s][%s][%ldus][ERROR]\n", thread_id, req.id, req.fd, req.request, error_buf, endTime - startTime);
		}
		else {
			printf("[%d][%d][%d][%s][%d][%ldus][%s]\n", thread_id, req.id, req.fd, req.request, size, endTime - startTime, christmas);
			fprintf(fp, "[%d][%d][%d][%s][%d][%ldus][%s]\n", thread_id, req.id, req.fd, req.request, size, endTime - startTime, christmas);
		}

    // return the result
  }
  return NULL;
}
/**********************************************************************************/

void terminate(int signo){
	deleteQueue(&req_q);
	printf("\n--queue deleted--\n");
	deleteCache(cache);
	fclose(fp);
	printf("--cache deleted--\n--process termination successful--\n");
	exit(0);
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
		printf("invalid port parameter: %s\n", in_port);
		return -1;
	}
  else if (port < 1025 || port > 65535){
    printf("port number out of range (1025 - 65535): %d", port);
    return -1;
  }
	struct stat test_path_stat;
	stat(in_path, &test_path_stat);
	if (S_ISDIR(test_path_stat.st_mode) == 0) {
		printf("bad directory path: %s\n", in_path);
		return -1;
	}
	if ((num_dispatcher = atoi(in_num_dispatcher)) == 0) {
		printf("invalid num_dispatcher parameter: %s\n", in_num_dispatcher);
		return -1;
	}
	else if (num_dispatcher > MAX_THREADS || num_dispatcher < 0) {
		printf("num_dispatcher out of range (1 - %d): %d\n", MAX_THREADS, num_dispatcher);
		return -1;
	}
  if ((num_workers = atoi(in_num_workers)) == 0) {
		printf("invalid num_workers parameter: %s\n", in_num_workers);
		return -1;
	}
	else if (num_workers > MAX_THREADS || num_workers < 0) {
		printf("num_workers out of range (1 - %d): %d\n", MAX_THREADS, num_workers);
		return -1;
	}
  if (strcmp(in_dynamic_flag, "0") != 0 || (dynamic_flag = atoi(in_dynamic_flag)) != 0){
    printf("invalid dynamic flag (default: 0): %s\n", in_dynamic_flag);
    return -1;
  }
  if ((queue_length = atoi(in_queue_length)) == 0){
    printf("invalid queue_length parameter: %s\n", in_queue_length);
    return -1;
  }
  else if (queue_length > MAX_QLEN || queue_length < 0){
    printf("queue_length out of range (1 - %d): %d\n", MAX_QLEN, queue_length);
    return -1;
  }
  if ((cache_size = atoi(in_cache_size)) == 0){
    printf("invalid cache_size parameter: %s\n", in_cache_size);
    return -1;
  }
  else if (cache_size > MAX_QLEN || cache_size < 0){
    printf("cache_size out of range (1 - %d): %d\n", MAX_CE, cache_size);
    return -1;
  }

	printf("starting server...\n");

	init(port);

  // Change the current working directory to server root directory
	if (chdir(in_path) != 0) {
		printf("invalid path: %s\n", in_path);
		return -1;
	}

  // Start the server and initialize cache
	initQueue(&req_q, queue_length);
	initCache(cache_size);

  // Create dispatcher and worker threads
	pthread_t dispatch_threads[num_dispatcher];
	int i;
	for (i = 0; i < num_dispatcher; i++) {
		if (pthread_create(&dispatch_threads[i], NULL, dispatch, (void*)&i)) {
			printf("error creating dispatcher thread %d\n", i);
			return -1;
		}
		if (pthread_detach(dispatch_threads[i])) {
			printf("error joining dispatcher thread %d\n", i);
			return -1;
		}
	}

	pthread_t worker_threads[num_workers];
	for (i = 0; i < num_workers; i++) {
		if (pthread_create(&worker_threads[i], NULL, worker, (void*)&i)) {
			printf("error creating worker thread %d\n", i);
			return -1;
		}
		if (pthread_detach(worker_threads[i])) {
			printf("error joining worker thread %d\n", i);
			return -1;
		}
	}

	//set ^c to clean up then exit process
	struct sigaction act;
	act.sa_handler = terminate;
	sigfillset(&act.sa_mask);
	sigaction(SIGINT, &act, NULL);

	printf("server started on port %d\n", port);

	//keep blocking until ^c, then process cleans up and exits
	while(1);

	//never reached
  return 0;
}
