#include <cstdlib>
#include <iostream>
#include <fstream>
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
  const char * homeDir = getenv("HOME");
  if (!homeDir)
  {
    std::cerr << "Could not get access to environment variable HOME" << std::endl;
    return 1;
  }
  std::ofstream logfile(std::string(homeDir) + "/.clangd-proxy.log", std::ios_base::app);
  if (!logfile.is_open())
  {
    std::cerr << "Could not open log file!" << std::endl;
    return 1;
  }

  logfile << "printing arguments:";
  for (int i = 0; i < argc; i++)
  {
    logfile << argv[i] << " ";
  }
  logfile << std::endl;

  // Create pipes for parent and child process to communicate
  // read from 0 write to 1
  int pipeToProg[2];
  int pipeToProxy[2];

  if (pipe(pipeToProg))
  {
    std::cerr << "failed to create pipe to clangd" << std::endl;
    return 1;
  }
  if (pipe(pipeToProxy))
  {
    std::cerr << "failed to create pipe to proxy" << std::endl;
    return 1;
  }

  switch (pid_t pid = fork())
  {
    case 0: // Child process (clangd)
    {
      dup2(pipeToProg[0], STDIN_FILENO);
      dup2(pipeToProxy[1], STDOUT_FILENO);
      dup2(pipeToProxy[1], STDERR_FILENO);

      // Close unused ends
      close(pipeToProg[1]);
      close(pipeToProxy[0]);

      logfile.close();

      execl("/usr/bin/clangd", "clangd", (char*)NULL);
      std::cerr << "Could not execute clangd in child" << std::endl;
      return 0;
    }
    default: // Parent process (proxy)
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
          logfile << "received: " <<  buffer << std::endl;

        // Read data from stdio
        size_t bytes_read_input = read(STDIN_FILENO, input, size);
        if (bytes_read_input != -1)
          logfile << "input: " << input << std::endl;

        if (bytes_read_clangd == -1 && bytes_read_input == -1)
          usleep(50000); // Sleep for 0.05 seconds
      }

      int status;
      kill(pid, SIGKILL); //send SIGKILL signal to the child process
      waitpid(pid, &status, 0);
      logfile.close();
      return 0;
    }
    case -1:
    {
      std::cerr << "Could not fork" << std::endl;
      return 1;
    }
  }

  return 0;
}
