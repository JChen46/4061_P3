/* CSci 4061 Fall 2018 Project 3
 * Name: John Chen, Raymond He, Chris Roelof 
 * X500: chen4787, hexxx863, roelo035 */

How to compile and run:
To compile run 'make'.
Run './web_server port path num_dispatcher num-workers dynamic_flag queue_length cache_size' in one command window.
An example would be: './web_server 9000 testing 100 100 0 100 100'
In the other command window, run the command 'wget 127.0.0.1:9000/FILE'
An example would be: 'wget 127.0.0.1:9000/image/gif/12.gif'

How the program works:
The purpose of this program is to create a multi-threaded web server through the usage of pthreads. 
By using dispatcher threads, the program repeatedly accepts an incoming connection, read the connection, and then place it into a queue.
The worker threads retrieves request and sends the result back to the client. We used CVs to synchronize the request queue.

Caching Mechanism:
We also implemented a cache system that stores the requests and its file content. For this we used an array of memory allocated entries
that uses the counter 'cache_counter' to keep track of the current position in the cache array. The cache is utilized in the worker threads,
where the thread first checks if the request is already present in the cache, logging a HIT or MISS based on the result. If not in the cache,
then the data is retrieved from the disk instead. We also implemented a LRU replacement policy 

Dynamic worker thread pool size:
Did not do

Contribution of each group member:
Chris - implemented queue and dequeue system, as well as the dispatch and worker threads, error checking, performance analysis
Raymond - implemented cache system, and dispatch and worker threads, terminator function, code commenting
John - implemented content checking, dispatch and worker threads, error checking, project readme
Project was mainly worked on together in meetings, and most of the progress was made with the participation of all group members