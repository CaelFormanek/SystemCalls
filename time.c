#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

int main(int argc, char** argv)
{
  /* check if the user entered correct number of args */
  if (argc != 2)
  {
    printf("You must enter 1 command after %s\n", argv[0]);
    exit(1);
  }
  
  /* size of the shared memory object */ 
  const int SIZE = 4096;
  /* name of shared memory object */
  const char* name = "shm_object";
  /* create a file descriptor for the shared memory */
  int shm_fd;
  /* pointer to shared memory */
  void* shm_ptr;

  /* creating the actual shared memory */
  shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
  /* change the size of the shared memory */
  ftruncate(shm_fd, SIZE);
  /* shm_ptr maps to shm_fd */
  shm_ptr = mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);

  /* create a child process */
  pid_t pid;
  pid = fork();

  /* forking failed */
  if (pid < 0)
  {
    printf("Failure when forking\n");
    exit(1);
  }
  /* on the child process */
  else if (pid == 0)
  {
    /* get the time right before executing */
    struct timeval start;
    gettimeofday(&start,NULL);
    int startTime = start.tv_usec;

    /* put start time in shared memory */
    memcpy(shm_ptr, &startTime, sizeof(int));

    /* execute linux command */
    char* linux_command = argv[1];
    system(linux_command);
  }
  /* on the parent process */
  else
  {
    int stat_val;

    /* wait for child to end */
    wait(&stat_val);
    
    /* get time at which child completes process */
    struct timeval end;
    gettimeofday(&end,NULL);
    int endTime = end.tv_usec;

    /* get the start time from shared memory and unlink shared memory */
    int startingTime = *(int*)shm_ptr;
    shm_unlink(name);

    /* get the difference */
    float difference = endTime - startingTime;
    printf("Elapsed time in microseconds: %f\n", difference);
  }
}