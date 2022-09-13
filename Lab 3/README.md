# Hash Hash Hash

This code makes 2 hash table implementations safe to use concurrently; one version uses a single mutex, and the other version uses multiple mutexes.

## Building

Run 'make' to build the program.

## Running

I ran './hash-table-tester -t 8 -s 60000', and the output is:
Generation: 80,392 usec
Hash table base: 1,672,782 usec
  - 0 missing
Hash table v1: 1,987,858 usec
  - 0 missing
Hash table v2: 550,268 usec
  - 0 missing

## First Implementation

My first implementation strategy was locking the entire body of the hash_table_v1_add_entry() function. This is correct because only 1 thread can access the hash table and add an entry at a time. All other threads must wait until the currently running thread is done executing, before they can add entries to the hash table. This ensures that no values are missing from the final hash table. 

### Performance

First, I ran './hash-table-tester -t 8 -s 60000', and the output is:
Generation: 80,392 usec
Hash table base: 1,672,782 usec
  - 0 missing
Hash table v1: 1,987,858 usec
  - 0 missing
Hash table v2: 550,268 usec
  - 0 missing

Then, I ran './hash-table-tester -t 4 -s 120000', and the output is:
Generation: 82,440 usec
Hash table base: 1,869,061 usec
  - 0 missing
Hash table v1: 2,451,400 usec
  - 0 missing
Hash table v2: 485,087 usec
  - 0 missing

I locked the mutex at the very beginning of the hash_table_v1_add_entry() method, before the 3 lines of code where the threads read the hash table entry, list head, and list entry. This means that these 3 lines are included within the critical section, so only 1 thread can read them at a time. Threads can't execute these 3 lines concurrently in v1, making it slower than the base implementation. Even with constant workload, v1 runs slower when there are more threads, since more overhead is introduced.


## Second Implementation

Describe your second implementation strategy here (the one with a multiple
mutexes). Argue why your strategy is correct.

My second implementation strategy uses one lock for each hash table entry. I created an array of mutexes of size HASH_TABLE_CAPACITY, and I found the appropriate index with: 'bernstein_hash(key) % HASH_TABLE_CAPACITY'. I locked the corresponding mutex after the 3 lines that get the hash table entry, list head, and list entry. By doing this, I allowed the threads to read these lines concurrently. I also only locked a specific hash table entry, which enables a thread to concurrently access a different entry so long as its respective lock isn't being held by another thread. Otherwise, the thread must wait until the lock for that entry is released, before adding a list entry to that hash table entry. This ensures no values are missing because multiple threads aren't allowed to access the same hash table entry at the same time.

### Performance

Report the relative speedup with number of threads equal to the number of
physical cores on your machine (at least 4). Note again that the amount of work
(`-t` times `-s`) should remain constant. Report the speedup relative to the
base hash table implementation. Explain the difference between this
implementation and your first, which respect why you get a performance increase.

Then, I ran './hash-table-tester -t 4 -s 120000', and the output is:
Generation: 82,440 usec
Hash table base: 1,869,061 usec
  - 0 missing
Hash table v1: 2,451,400 usec
  - 0 missing
Hash table v2: 485,087 usec
  - 0 missing

The runtime of v2 falls in the range: base_time/num_cores < v2 < base_time/(num_cores/2). This is a speedup by a factor of between 2 and 4, compared to the base implementation. I ran the tester on the Linux server, which is 4 cores. Because I allowed threads to concurrently get the hash table entry, list head, and list entry, as well as concurrently access different hash table entries (but not the same entry), there is a significant speedup compared to the base implementation and v1. v1 adds list entries sequentially, while v2 adds them in parallel so long as no 2 threads add a list entry to the same hash table entry.

## Cleaning up

Run 'make clean' to get rid of the object and executable files.
