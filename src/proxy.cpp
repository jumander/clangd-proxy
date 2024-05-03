#include "proxy.hpp"
#include <sys/wait.h>
#include <sys/prctl.h>
#include <fcntl.h>
#include <iostream>

namespace lsp_proxy {


  bool createClangdProxy(const char * clangdPath, Descriptors & stdDescriptors, Descriptors & proxyDescriptors, pid_t & childPID)
  {
    // Create pipes for parent and child process to communicate
    // read from 0 write to 1
    int pipeToProg[2];
    int pipeToProxy[2];
    int pipeToProxyInfo[2];

    if (pipe(pipeToProg))
    {
      std::cout << "failed to create pipe to clangd" << std::endl;
      return false;
    }
    if (pipe(pipeToProxy))
    {
      std::cout << "failed to create pipe to proxy" << std::endl;
      return false;
    }
    if (pipe(pipeToProxyInfo))
    {
      std::cout << "failed to create pipe to proxy" << std::endl;
      return false;
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
        close(pipeToProg[1]);
        close(pipeToProxy[0]);
        close(pipeToProxyInfo[0]);

        execl(clangdPath, "clangd", (char*)NULL);
        std::cout << "Could not execute clangd in child" << std::endl;
        return false;
      }
      default: // Parent process (proxy)
      {
        std::cout << "proxy process created" << std::endl;
        // Make stdio non blocking
        fcntl(stdDescriptors.in, F_SETFL, O_NONBLOCK | fcntl(stdDescriptors.in, F_GETFL, 0));
        // Make clangd pipe non blocking
        fcntl(pipeToProxy[0], F_SETFL, O_NONBLOCK | fcntl(pipeToProxy[0], F_GETFL, 0));
        fcntl(pipeToProxyInfo[0], F_SETFL, O_NONBLOCK | fcntl(pipeToProxyInfo[0], F_GETFL, 0));

        // Close unused ends
        close(pipeToProg[0]);
        close(pipeToProxy[1]);
        close(pipeToProxyInfo[1]);

        childPID = pid;
        proxyDescriptors.in = pipeToProg[1];
        proxyDescriptors.out = pipeToProxy[0];
        proxyDescriptors.info = pipeToProxyInfo[0];
        return true;
      }
      case -1:
      {
        std::cout << "Could not fork" << std::endl;
        return false;
      }
    }
    return true;
  }
}