//Author: Matt Burris
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>

void printUse() {
  fprintf(stderr, "Usage: connect <program1> : <program2>\n");
  exit(1);
}

int main(int argc, char *argv[]) {
  
  if (argc < 4) printUse();
  
  int pipeIndex = 0; //argument index of the pipe symbol ':'
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], ":") == 0) {
      pipeIndex = i;
      break;
    }
  }
  
  if (pipeIndex == 0) printUse(); //no colon found
  
  if (pipeIndex == 1) printUse(); //colon cannot be first argument
  
  if (pipeIndex == argc - 1) printUse(); //colon cannot be last argument
  
  char **write = calloc(sizeof(char *), (pipeIndex)); //initialize write and read arrays
  char **read = calloc(sizeof(char *), (argc - pipeIndex) + 2); //while setting every index to null
  
  for (int i = 1; i < pipeIndex; i++) { //assign everything before the colon to write array
    write[i - 1] = argv[i]; 
  }
  
  for (int i = pipeIndex + 1; i < argc; i++) { //assign everything after the colon to read array
    read[i - pipeIndex - 1] = argv[i];
  }
  
  int fd[2];
  int status;
  pipe(fd);
  
  if (fork ()) {
    close(fd[0]); // close read end
    dup2(fd[1], STDOUT_FILENO);
    close(fd[1]);
    execvp(write[0], write);
    wait(&status);
  } else {
    close(fd[1]); //close write end
    dup2(fd[0], STDIN_FILENO);
    close(fd[0]);
    execvp(read[0], read);
  }
  
  return 0;
}
