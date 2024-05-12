#pragma once
#include <unistd.h>

namespace lsp_proxy {

  struct Descriptors
  {
    int in;
    int out;
    int info;
  };

  /* Forks the process, spawning clangd in the child process.
   * sets up pipes so that parent process can end up as a proxy.
   * Returns file descriptors of stdin, stdout and info for the proxy.
   */
  bool createClangdProxy(const char * clangdPath, Descriptors & stdDescriptors, Descriptors & proxyDescriptors, pid_t & childPID);
}