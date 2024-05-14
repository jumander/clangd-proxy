#pragma once
#include "processors/baseProcessor.hpp"

namespace lsp_proxy {

  class ClangdProxy
  {
    public:
      ClangdProxy(int inClientFD, int outClientFD, int inServerFD, int outServerFD);

      bool readPipes();

      /// Returns whether or not the message was handled
      virtual bool handleMessage(std::string & message);
      /// Returns whether or not the response was handled
      virtual bool handleResponse(std::string & response);

    private:
      void handleServerMessage(std::string & message);
      void handleClientMessage(std::string & message);


    private:
      BaseProcessor m_clientToServer;
      BaseProcessor m_serverToClient;
  };
}