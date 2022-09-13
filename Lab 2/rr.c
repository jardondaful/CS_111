#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/queue.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

typedef uint32_t u32;
typedef int32_t i32;

struct process {
  u32 pid;
  u32 arrival_time;
  u32 burst_time;

  TAILQ_ENTRY(process) pointers;

  /* Additional fields here */
  u32 time_left;
  bool has_run;
  u32 wait_time;
  u32 response_time;
  u32 p_num;
  bool added;
  /* End of "Additional fields here" */
};

TAILQ_HEAD(process_list, process);

u32 next_int(const char **data, const char *data_end) {
  u32 current = 0;
  bool started = false;
  while (*data != data_end) {
    char c = **data;

    if (c < 0x30 || c > 0x39) {
      if (started) {
	return current;
      }
    }
    else {
      if (!started) {
	current = (c - 0x30);
	started = true;
      }
      else {
	current *= 10;
	current += (c - 0x30);
      }
    }

    ++(*data);
  }

  printf("Reached end of file while looking for another integer\n");
  exit(EINVAL);
}

u32 next_int_from_c_str(const char *data) {
  char c;
  u32 i = 0;
  u32 current = 0;
  bool started = false;
  while ((c = data[i++])) {
    if (c < 0x30 || c > 0x39) {
      exit(EINVAL);
    }
    if (!started) {
      current = (c - 0x30);
      started = true;
    }
    else {
      current *= 10;
      current += (c - 0x30);
    }
  }
  return current;
}

void init_processes(const char *path,
                    struct process **process_data,
                    u32 *process_size)
{
  int fd = open(path, O_RDONLY);
  if (fd == -1) {
    int err = errno;
    perror("open");
    exit(err);
  }

  struct stat st;
  if (fstat(fd, &st) == -1) {
    int err = errno;
    perror("stat");
    exit(err);
  }

  u32 size = st.st_size;
  const char *data_start = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (data_start == MAP_FAILED) {
    int err = errno;
    perror("mmap");
    exit(err);
  }

  const char *data_end = data_start + size;
  const char *data = data_start;
  

  *process_size = next_int(&data, data_end);

  *process_data = calloc(sizeof(struct process), *process_size);
  if (*process_data == NULL) {
    int err = errno;
    perror("calloc");
    exit(err);
  }

  for (u32 i = 0; i < *process_size; ++i) {
    (*process_data)[i].pid = next_int(&data, data_end);
    (*process_data)[i].arrival_time = next_int(&data, data_end);
    (*process_data)[i].burst_time = next_int(&data, data_end);
  }
  
  munmap((void *)data, size);
  close(fd);
}

int main(int argc, char *argv[])
{
  if (argc != 3) {
    return EINVAL;
  }
  struct process *data;
  u32 size;
  init_processes(argv[1], &data, &size);

  u32 quantum_length = next_int_from_c_str(argv[2]);

  struct process_list list;
  TAILQ_INIT(&list);

  u32 total_waiting_time = 0;
  u32 total_response_time = 0;

  /* Your code here */

  struct process *p;
  for (u32 i = 0; i < size; i++) {
    p = &data[i];
    p->time_left = p->burst_time;
    p->has_run = false;
    p->p_num = i+1;
    p->added = false;
  }

  u32 t = 0;
  u32 quantum_elapsed = 0;
  bool finished = false;
  bool processes_left;
  
  while (!finished) {

    //push new arrivals to end
    for (u32 i = 0; i < size; i++) {
      p = &data[i];
      if (p->arrival_time == t && !p->added) {
	TAILQ_INSERT_TAIL(&list, p, pointers);
	p->wait_time = 0;
	p->response_time = 0;
	p->added = true;

      }
    }
    if (TAILQ_EMPTY(&list)) {

      //check if any processes are left
      processes_left = false;
      for (u32 i = 0; i < size; i++) {
	struct process *q = &data[i];
	if (q->arrival_time > t) {

	  t++;
	  processes_left = true;
	  
	  continue;
	}
      }
      
      if (!processes_left) {
	finished = true;
      }
    }
    
    else {

      p = TAILQ_FIRST(&list);
      if (!p->has_run) {
	p->has_run = true;
      }
      
      //at end of time slice
      if (quantum_elapsed == quantum_length || p->time_left <= 0) {
	quantum_elapsed = 0;

	//if process is completed
	if (p->time_left <= 0) {
	  TAILQ_REMOVE(&list, p, pointers);
	  total_waiting_time += p->wait_time;
	  total_response_time += p->response_time;
	}
	else {

	  //move to end of queue
	  TAILQ_REMOVE(&list, p, pointers);
	  TAILQ_INSERT_TAIL(&list, p, pointers);
	}
      }
      else {

	//for each queued process
	struct process *p_next = TAILQ_NEXT(p, pointers);	
	if (p_next != NULL) {

	  //don't update wait time for current process
	  TAILQ_REMOVE(&list, p, pointers);

	  //update wait and response times for queued processes
	  TAILQ_FOREACH(p_next, &list, pointers) {
	    p_next->wait_time++;	    
	    if (!p_next->has_run) {
	      p_next->response_time++;
	    }
	  }
	  TAILQ_INSERT_HEAD(&list, p, pointers);
	}
	
	t++;
	quantum_elapsed++;
	p->time_left--;
      }
    }
  }
  
  /* End of "Your code here" */

  printf("Average waiting time: %.2f\n", (float) total_waiting_time / (float) size);
  printf("Average response time: %.2f\n", (float) total_response_time / (float) size);

  free(data);
  return 0;
}
