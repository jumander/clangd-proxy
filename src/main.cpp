#include <iostream>
#include <fstream>
#include <sys/wait.h>
#include <sys/prctl.h>
#include <fcntl.h>
#include <unistd.h>
#include "proxy.hpp"

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

  lsp_proxy::Descriptors stdDescriptors;
  stdDescriptors.in = STDIN_FILENO;
  stdDescriptors.out = stdoutFD;
  stdDescriptors.info = STDERR_FILENO;
  lsp_proxy::Descriptors proxyDescriptors;
  pid_t clangdPID;

  if (!lsp_proxy::createClangdProxy("/usr/bin/clangd", stdDescriptors, proxyDescriptors, clangdPID))
    return 1;

  int constexpr size = 8192;
  char input[size + 1];
  char output[size + 1];
  char info[size + 1];

  while (true)
  {
    // Handle stdio
    ssize_t bytes_read_input = read(stdDescriptors.in, input, size);
    if (bytes_read_input > 0) {
      input[bytes_read_input] = '\0'; // Null terminate string
      // std::cout << "Read " << bytes_read_input << " bytes from input" << std::endl;
      // std::cout << "FROM EDITOR: " << input << std::endl;
      write(proxyDescriptors.in, input, bytes_read_input);
    }

    // Handle stdout
    ssize_t bytes_read_clangd = read(proxyDescriptors.out, output, size);
    if (bytes_read_clangd > 0)
    {
      output[bytes_read_clangd] = '\0'; // Null terminate string
      // std::cout << "Read " << bytes_read_clangd << " bytes from clangd" << std::endl;
      // std::cout << "FROM CLANGD: " << output << std::endl;
      write(stdDescriptors.out, output, bytes_read_clangd);
    }

    // Handle stderr
    ssize_t bytes_read_clangd_err = read(proxyDescriptors.info, info, size);
    if (bytes_read_clangd_err > 0)
    {
      info[bytes_read_clangd_err] = '\0'; // Null terminate string
      std::cout << info << std::endl;
      // std::cout << "Read " << bytes_read_clangd << " bytes from clangd" << std::endl;
      // std::cout << "FROM CLANGD INFO: " << info << std::endl;
      write(stdDescriptors.info, info, bytes_read_clangd_err);
    }

    if (bytes_read_clangd <= 0 && bytes_read_input <= 0)
      usleep(50000); // Sleep for 0.05 seconds
  }
  int status;
  kill(clangdPID, SIGKILL); //send SIGKILL signal to the child process
  waitpid(clangdPID, &status, 0);
  return 0;
}
