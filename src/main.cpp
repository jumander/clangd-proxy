#include <iostream>
#include <fstream>
#include <sys/wait.h>
#include <sys/prctl.h>
#include <fcntl.h>
#include "nlohmann/json.hpp"

int main(int argc, char* argv[])
{
  nlohmann::json ex1 = nlohmann::json::parse(R"(
    {
      "pi": 3.141,
      "happy": true
    }
  )");
  std::cout << ex1["pi"] << std::endl;

  std::cout << "launching clangd-proxy\n";
  const char * homeDir = getenv("HOME");
  if (!homeDir)
  {
    std::cerr << "Could not get access to environment variable HOME" << std::endl;
    return 1;
  }
  std::ofstream logfile(std::string(homeDir) + "/.clangd-proxy.log");//, std::ios_base::app);
  if (!logfile.is_open())
  {
    std::cerr << "Could not open log file!" << std::endl;
    return 1;
  }

  logfile << "New session: ";
  for (int i = 0; i < argc; i++)
  {
    logfile << argv[i] << " ";
  }
  logfile << std::endl;

  // Create pipes for parent and child process to communicate
  // read from 0 write to 1
  int pipeToProg[2];
  int pipeToProxy[2];
  int pipeToProxyInfo[2];

  if (pipe(pipeToProg))
  {
    logfile << "failed to create pipe to clangd" << std::endl;
    return 1;
  }
  if (pipe(pipeToProxy))
  {
    logfile << "failed to create pipe to proxy" << std::endl;
    return 1;
  }
  if (pipe(pipeToProxyInfo))
  {
    logfile << "failed to create pipe to proxy" << std::endl;
    return 1;
  }

  switch (pid_t pid = fork())
  {
    case 0: // Child process (clangd)
    {
      logfile << "clangd process created" << std::endl;
      dup2(pipeToProg[0], STDIN_FILENO);
      dup2(pipeToProxy[1], STDOUT_FILENO);
      dup2(pipeToProxyInfo[1], STDERR_FILENO);

      // Close unused ends
      close(pipeToProg[1]);
      close(pipeToProxy[0]);
      close(pipeToProxyInfo[0]);

      execl("/usr/bin/clangd", "clangd", (char*)NULL);
      logfile << "Could not execute clangd in child" << std::endl;
      return 0;
    }
    default: // Parent process (proxy)
    {
      logfile << "proxy process created" << std::endl;
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
        size_t bytes_read_input = read(STDIN_FILENO, input, size);
        if (bytes_read_input != -1)
        {
          input[bytes_read_input] = '\0'; // Null terminate string
          // logfile << "Read " << bytes_read_input << " bytes from input" << std::endl;
          // logfile << "FROM EDITOR: " << input << std::endl;
          write(pipeToProg[1], input, bytes_read_input);
        }

        // Handle stdout
        size_t bytes_read_clangd = read(pipeToProxy[0], output, size);
        if (bytes_read_clangd != -1)
        {
          output[bytes_read_clangd] = '\0'; // Null terminate string
          // logfile << "Read " << bytes_read_clangd << " bytes from clangd" << std::endl;
          // logfile << "FROM CLANGD: " << output << std::endl;
          write(STDOUT_FILENO, output, bytes_read_clangd);
        }

        // Handle stderr
        size_t bytes_read_clangd_err = read(pipeToProxyInfo[0], info, size);
        if (bytes_read_clangd_err != -1)
        {
          info[bytes_read_clangd_err] = '\0'; // Null terminate string
          logfile << info << std::endl;
          // logfile << "Read " << bytes_read_clangd << " bytes from clangd" << std::endl;
          // logfile << "FROM CLANGD: " << info << std::endl;
          write(STDERR_FILENO, info, bytes_read_clangd_err);
        }

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
      logfile << "Could not fork" << std::endl;
      return 1;
    }
  }

  return 0;
}