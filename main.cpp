#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/prctl.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>

int main(int argc, char* argv[])
{
	printf("starting clangd\n");

	// Create pipes for parent and child process to communicate
	// read from 0 write to 1
	int pipeToProg[2];
	int pipeToProxy[2];

	if (pipe(pipeToProg))
	{
		printf("failed to create pipe to clangd\n");
		return 1;
	}
	if (pipe(pipeToProxy))
	{
		printf("failed to create pipe to proxy\n");
		return 1;
	}


	switch (pid_t pid = fork())
  {
    case 0:
    {
      dup2(pipeToProg[0], STDIN_FILENO);
      dup2(pipeToProxy[1], STDOUT_FILENO);
      dup2(pipeToProxy[1], STDERR_FILENO);

      // Close unused ends
      close(pipeToProg[1]);
      close(pipeToProxy[0]);

      execl("/usr/bin/tee", "tee", (char*)NULL);
      printf("Could not execute clangd in child\n");
      return 0;
    }
    default:
    {
      // Close unused ends
      close(pipeToProg[0]);
      close(pipeToProxy[1]);


      printf("parent hello\n");
      char toChild[] = "Sending this clangd";
      write(pipeToProg[1], toChild, strlen(toChild) + 1);

      // recieve message from child
      {
        printf("try to read\n");
        int constexpr size = 4096;
        char buffer[size];
        int bytes_read = read(pipeToProxy[0], buffer, size);
        printf("parent received: %s\n", buffer);
        printf("parent read %d bytes\n", bytes_read);
      }

      printf("done\n");

      int status;
      kill(pid, SIGKILL); //send SIGKILL signal to the child process
      waitpid(pid, &status, 0);
      return 0;
    }
    case -1:
    {
      printf("Could not fork\n");
      return 1;
    }
	}

  return 0;
}

