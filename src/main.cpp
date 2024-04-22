#include <iostream>
#include <fstream>
#include <sys/wait.h>
#include <sys/prctl.h>
#include <fcntl.h>

bool logToStdout(std::string logname, int * prevFD)
{
  const char * homeDir = getenv("HOME");
  if (!homeDir)
  {
    std::cerr << "Could not get access to environment variable HOME" << std::endl;
    return false;
  }
  FILE * logfile = fopen((std::string(homeDir) + "/" + logname).c_str(), "w");
  if (!logfile)
  {
    std::cerr << "Could not open log file!" << std::endl;
    return false;
  }

  int logfile_fd = fileno(logfile);
  *prevFD = dup(fileno(stdout));
  dup2(logfile_fd, fileno(stdout));

  return true;
}

int main(int argc, char* argv[])
{
  int stdoutFD;
  if (!logToStdout("/.clangd-proxy.log", &stdoutFD))
    return 1;
  std::cout << "launching clangd-proxy\n";

  for (int i = 0; i < argc; i++)
  {
    std::cout << argv[i] << " ";
  }
  std::cout << std::endl;

  // Create pipes for parent and child process to communicate
  // read from 0 write to 1
  int pipeToProg[2];
  int pipeToProxy[2];
  int pipeToProxyInfo[2];

  if (pipe(pipeToProg))
  {
    std::cout << "failed to create pipe to clangd" << std::endl;
    return 1;
  }
  if (pipe(pipeToProxy))
  {
    std::cout << "failed to create pipe to proxy" << std::endl;
    return 1;
  }
  if (pipe(pipeToProxyInfo))
  {
    std::cout << "failed to create pipe to proxy" << std::endl;
    return 1;
  }

  switch (pid_t pid = fork())
  {
    case 0: // Child process (clangd)
    {
      std::cout << "clangd process created" << std::endl;
      dup2(pipeToProg[0], STDIN_FILENO);
      dup2(pipeToProxy[1], STDOUT_FILENO);
      dup2(pipeToProxyInfo[1], STDERR_FILENO);

      // Close unused ends
      close(stdoutFD);
      close(pipeToProg[1]);
      close(pipeToProxy[0]);
      close(pipeToProxyInfo[0]);

      execl("/usr/bin/clangd", "clangd", (char*)NULL);
      std::cout << "Could not execute clangd in child" << std::endl;
      return 0;
    }
    default: // Parent process (proxy)
    {
      std::cout << "proxy process created" << std::endl;
      // Make stdio non blocking
      fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK | fcntl(STDIN_FILENO, F_GETFL, 0));
      // Make clangd pipe non blocking
      fcntl(pipeToProxy[0], F_SETFL, O_NONBLOCK | fcntl(pipeToProxy[0], F_GETFL, 0));
      fcntl(pipeToProxyInfo[0], F_SETFL, O_NONBLOCK | fcntl(pipeToProxyInfo[0], F_GETFL, 0));

      // Close unused ends
      close(pipeToProg[0]);
      close(pipeToProxy[1]);
      close(pipeToProxyInfo[1]);

      int constexpr size = 8192;
      char input[size + 1];
      char output[size + 1];
      char info[size + 1];

      while (true)
      {
        // Handle stdio
        ssize_t bytes_read_input = read(STDIN_FILENO, input, size);
        if (bytes_read_input > 0) {
          input[bytes_read_input] = '\0'; // Null terminate string
          // std::cout << "Read " << bytes_read_input << " bytes from input" << std::endl;
          // std::cout << "FROM EDITOR: " << input << std::endl;
          write(pipeToProg[1], input, bytes_read_input);
        }

        // Handle stdout
        ssize_t bytes_read_clangd = read(pipeToProxy[0], output, size);
        if (bytes_read_clangd > 0)
        {
          output[bytes_read_clangd] = '\0'; // Null terminate string
          // std::cout << "Read " << bytes_read_clangd << " bytes from clangd" << std::endl;
          // std::cout << "FROM CLANGD: " << output << std::endl;
          write(stdoutFD, output, bytes_read_clangd);
        }

        // Handle stderr
        ssize_t bytes_read_clangd_err = read(pipeToProxyInfo[0], info, size);
        if (bytes_read_clangd_err > 0)
        {
          info[bytes_read_clangd_err] = '\0'; // Null terminate string
          std::cout << info << std::endl;
          // std::cout << "Read " << bytes_read_clangd << " bytes from clangd" << std::endl;
          // std::cout << "FROM CLANGD: " << info << std::endl;
          write(STDERR_FILENO, info, bytes_read_clangd_err);
        }

        if (bytes_read_clangd <= 0 && bytes_read_input <= 0)
          usleep(50000); // Sleep for 0.05 seconds
      }

      int status;
      kill(pid, SIGKILL); //send SIGKILL signal to the child process
      waitpid(pid, &status, 0);
      return 0;
    }
    case -1:
    {
      std::cout << "Could not fork" << std::endl;
      return 1;
    }
  }

  return 0;
}
