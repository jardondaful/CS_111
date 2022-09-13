#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>

int one_cmd(const char *cmd);
int two_cmd(char *arr_args[]);
int pipe_func(int n_args, char *arr_args[]);

int main(int argc, char *argv[])
{
  switch (argc) {
    case 0:
    case 1:
      return EINVAL;
      exit(EINVAL);
    case 2:
      return one_cmd(argv[1]);
    case 3:
      return two_cmd(argv);
    default:
      return pipe_func(argc, argv);
  }
  return 0;
}

int one_cmd(const char *cmd)
{
  int status;
  int id = fork();
  if (id > 0) {
    wait(&status);
    if ( ! (WIFEXITED(status) && (WEXITSTATUS(status) == 0))) {
      return ECHILD;
      exit(ECHILD);
    }
  }
  else if (id == 0) {
    if (execlp(cmd, cmd, NULL) == -1) {
      return ENOEXEC;
      exit(ENOEXEC);
    }
  }
  else if (id < 0) {
    return ESRCH;
    exit(ESRCH);
  }
  return 0;
}


int two_cmd(char *arr_args[])
{
  int status;
  int fd[2];
  pipe(fd);

  for (int i = 1; i <= 2; i++) {
    int id = fork();
    
    if (i == 1) {
      if (id > 0) {  //parent
	close(fd[1]);
	wait(&status);
	if ( ! (WIFEXITED(status) && (WEXITSTATUS(status) == 0))) {
	  return ECHILD;
	  exit(ECHILD);
	}
      }
      else if (id == 0) {  //child
	dup2(fd[1], 1);
	close(fd[1]);
	close(fd[0]);
	if (execlp(arr_args[1], arr_args[1], NULL) == -1) {
	  return ENOEXEC;
	  exit(ENOEXEC);
	}
      }
      else if (id < 0) {
	return ESRCH;
	exit(ESRCH);
      }
    }

    if (i == 2) {
      if (id > 0) {  //parent
	close(fd[0]);
	wait(&status);
	if ( ! (WIFEXITED(status) && (WEXITSTATUS(status) == 0))) {
	  return ECHILD;
	  exit(ECHILD);
	}
      }
      else if (id == 0) {  //child
	dup2(fd[0], 0);
	close(fd[0]);
	if (execlp(arr_args[2], arr_args[2], NULL) == -1) {
	  return ENOEXEC;
	  exit(ENOEXEC);
	}
      }
      else if (id < 0) {
	return ESRCH;
	exit(ESRCH);
      }
    }
  }
  
  return 0;
}


int pipe_func(int n_args, char *arr_args[])
{
  int status;
  
  //first command
  int fd[7][2];
  pipe(fd[0]);
  int id = fork();

  if (id > 0) {  //parent
    close(fd[0][1]);
    wait(&status);
    if ( ! (WIFEXITED(status) && (WEXITSTATUS(status) == 0))) {
      return ECHILD;
      exit(ECHILD);
    }
  }
  else if (id == 0) {  //child
    dup2(fd[0][1], 1);
    close(fd[0][1]);
    close(fd[0][0]);
    if (execlp(arr_args[1], arr_args[1], NULL) == -1) {
      return ENOEXEC;
      exit(ENOEXEC);
    }
  }
  else if (id < 0) {
    return ESRCH;
    exit(ESRCH);
  }

  
  //middle commands
  
  int i = 2;
  for (; i < n_args - 1; i++) {
    if (i != 2) {
      close(fd[i-3][0]);
    }
    pipe(fd[i-1]);
    int id = fork();

    if (id > 0) {  //parent
      close(fd[i-1][1]);
      wait(&status);
      if ( ! (WIFEXITED(status) && (WEXITSTATUS(status) == 0))) {
	return ECHILD;
	exit(ECHILD);
      }
    }
    else if (id == 0) {  //child
      dup2(fd[i-2][0], 0);
      close(fd[i-2][0]);
      dup2(fd[i-1][1], 1);
      close(fd[i-1][1]);
      close(fd[i-1][0]);
      if (execlp(arr_args[i], arr_args[i], NULL) == -1) {
	return ENOEXEC;
	exit(ENOEXEC);
      }
    }
  }

  
  //last command
  
  close(fd[i-3][0]);
  id = fork();
  
  if (id > 0) {  //parent
    wait(&status);
    if ( ! (WIFEXITED(status) && (WEXITSTATUS(status) == 0))) {
      return ECHILD;
      exit(ECHILD);
    }
  }
  else if (id == 0) {  //child
    dup2(fd[i-2][0], 0);
    close(fd[i-2][0]);
    if (execlp(arr_args[i], arr_args[i], NULL) == -1) {
      return ENOEXEC;
      exit(ENOEXEC);
    }
  }
  else if (id < 0) {
    return ESRCH;
    exit(ESRCH);
  }
  
  return 0;
}
