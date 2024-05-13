#include "clangdProxy.hpp"
#include <iostream>

namespace lsp_proxy {

  ClangdProxy::ClangdProxy(int inClientFD, int outClientFD, int inServerFD, int outServerFD)
    : m_clientToServer(BaseProcessor(inClientFD, outClientFD)), m_serverToClient(BaseProcessor(inServerFD, outServerFD))
  {
    // Set callbacks
    m_clientToServer.setMessageCallback([this](std::string & message){
      return processServerMessage(message);
    });
    m_serverToClient.setMessageCallback([this](std::string & message){
      return processClientMessage(message);
    });
  }

  bool ClangdProxy::readPipes()
  {
    bool changed = false;

    changed |= m_clientToServer.readPipe();

    changed |= m_serverToClient.readPipe();

    return changed;
  }

  std::optional<std::string> ClangdProxy::processServerMessage(std::string & message)
  {
    std::cout << "M to server" << std::endl;
    return processMessage(message);
  }

  std::optional<std::string> ClangdProxy::processClientMessage(std::string & message)
  {
    std::cout << "M to client" << std::endl;
    return processMessage(message);
  }

  std::optional<std::string> ClangdProxy::processMessage(std::string & message)
  {
    return message;
  }

  std::optional<std::string> ClangdProxy::processResponse(std::string & response)
  {
    return response;
  }
}