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
  printf("printing arguments:\n");
  for (int i = 0; i < argc; i++)
  {
    printf("%s", argv[i]);
    printf(" ");
  }
  printf("\n");
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

      execl("/usr/bin/clangd", "clangd", (char*)NULL);
      printf("Could not execute clangd in child\n");
      return 0;
    }
    default:
    {
      // Make read pipe non blocking
      fcntl(pipeToProxy[0], F_SETFL, O_NONBLOCK | fcntl(pipeToProxy[0], F_GETFL, 0));

      // Close unused ends
      close(pipeToProg[0]);
      close(pipeToProxy[1]);

      int constexpr size = 4096;
      char buffer[size];

      while (true)
      {
        int bytes_read = read(pipeToProxy[0], buffer, size);
        if (bytes_read == -1)
          continue;
        buffer[bytes_read] = 0;
        printf("received[%d]: %s\n", bytes_read, buffer);
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

