# Project Group Number: 3

## Group Members: 
- Gianni Guadagno - guada028 
- Jackson Reichow - reich656 
- Tyler Larson - lars6653

## Testing Machine: 
- login03.cselabs.umn.edu

## Changes to Makefile or Existing Files: 
- None

## Member Contributions:
- Jackson: Completed logging function as well as the worker thread.
- Tyler: Implemented locking for Processing as well as outlined all of the conditions needed.
- Gianni: Implemented locking for Queue and main.
-Group Work: Completed the processing thread, README and bug fixed.

## Additional Assumptions:
- None

## Parallel Image Processing Design:
Recieve number of workers from input.
Creates a processing thread thread
	traverse image directory
		for img file in directory:
			lock(queue_lock)
			put request in the shared queue
			unlock(queue_lock)
	Signal(queue_filled_cond)
Create number workers of threads.
Each worker continues to dequeues request from queue until empty.
lock(processing_wait_lock)
Then applies the transformation
saves it to the output directory
unlock(processing_wait_lock)
Join threads and exit.

Difference from Intermediate:
Additional Lock and Conditional
Mutex Locks: queue_lock, processing_wait_lock
Conditionals: queue_has_request_cond, queue_filled_cond, workers_done_cond

## AI Help:
- Used to help with certain debugging on test cases. Revealed incorrect calls in error checking. 
