# Project Group Number: 3

## Group Members: 
- Gianni Guadagno - guada028 
- Jackson Reichow - reich656 
- Tyler Larson - lars6653

## Testing Machine: 
- login03.cselabs.umn.edu

## Changes to Makefile or Existing Files: 
- None

## Contribution Plan:
- All: README, Bug Fix.
- Jackson: Implement portions for pngs, implement logging help with the worker threads.
- Tyler: Implement the worker threads.
- Gianni: Fully implement the Mutex locks and conditions, help with logs.

## Implementation plan for the worker threads:
- Create all the worker thread using pthread_create with the main thread.
- The worker thread will be given an ID and passed to the request queue
- Worker thread will run until no longer needed.

## Implementation plan for Mutex locks and Conditionals:
- The mutex lock will be implemented around the queue functions to make sure only one thread can access the queue at a time.
- All threads will lock the mutex lock before checking or modifying the queue, unlocks afterwards. 
- Condition Variable: Empty, threads will wait on the queue if it is empty. When a item is added they will be signaled.
- Condition Variable: Full, threads will wait on the queue if it is empty. When a item is removed they will be signaled.
