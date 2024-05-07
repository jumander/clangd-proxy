#include <iostream>
#include <sys/wait.h>
#include <fcntl.h>
#include "proxy.hpp"
#include "processors/jsonProcessor.hpp"

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

  lsp_proxy::JSONProcessor toLSP(stdDescriptors.in, proxyDescriptors.in);
  lsp_proxy::JSONProcessor fromLSP(proxyDescriptors.out, stdDescriptors.out);

  // Make info pipe non-blocking
  fcntl(proxyDescriptors.info, F_SETFL, O_NONBLOCK | fcntl(proxyDescriptors.info, F_GETFL, 0));
  int constexpr infoBufferSize = 8192;
  char infoBuffer[infoBufferSize + 1];

  while (true)
  {
    bool changed = false;

    // Handle stdio
    changed |= toLSP.readPipe();

    // Handle stdout
    changed |= fromLSP.readPipe();

    // Handle info(stderr)
    ssize_t bytes_read_info = read(proxyDescriptors.info, infoBuffer, infoBufferSize);
    if (bytes_read_info > 0)
    {
      infoBuffer[bytes_read_info] = '\0'; // Null terminate string
      // std::cout << infoBuffer << std::endl;
      write(stdDescriptors.info, infoBuffer, bytes_read_info);
    }
    changed |= bytes_read_info > 0;

    if (!changed)
      usleep(50000); // Sleep for 0.05 seconds
  }


  int status;

  kill(clangdPID, SIGKILL); //send SIGKILL signal to the child process
  waitpid(clangdPID, &status, 0);
  return 0;
}
