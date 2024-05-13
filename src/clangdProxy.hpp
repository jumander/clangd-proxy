#pragma once
#include "processors/baseProcessor.hpp"

namespace lsp_proxy {

  class ClangdProxy
  {
    public:
      ClangdProxy(int inClientFD, int outClientFD, int inServerFD, int outServerFD);

      bool readPipes();

      std::optional<std::string> processMessage(std::string & message);
      std::optional<std::string> processResponse(std::string & message);

    private:
      std::optional<std::string> processServerMessage(std::string & message);
      std::optional<std::string> processClientMessage(std::string & message);


    private:
      BaseProcessor m_clientToServer;
      BaseProcessor m_serverToClient;
  };
}