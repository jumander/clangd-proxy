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
      // Make clangd pipe non blocking
      fcntl(pipeToProxy[0], F_SETFL, O_NONBLOCK | fcntl(pipeToProxy[0], F_GETFL, 0));
      // Make stdio non blocking
      fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK | fcntl(STDIN_FILENO, F_GETFL, 0));

      // Close unused ends
      close(pipeToProg[0]);
      close(pipeToProxy[1]);

      int constexpr size = 4096;
      char buffer[size];
      char input[size];

      while (true)
      {
        // Read data from clangd
        size_t bytes_read_clangd = read(pipeToProxy[0], buffer, size);
        if (bytes_read_clangd != -1)
          printf("received[%zu]: %s\n", bytes_read_clangd, buffer);

        // Read data from stdio
        size_t bytes_read_input = read(STDIN_FILENO, input, size);
        if (bytes_read_input != -1)
          printf("input: %s\n", input);
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

