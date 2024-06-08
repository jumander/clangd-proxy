#pragma once
#include "processors/baseProcessor.hpp"
#include <nlohmann/json.hpp>

namespace lsp_proxy {

  struct MessageRecord
  {
    int id;
    std::string method;
  };

  class ClangdProxy
  {
    public:
      ClangdProxy(int inClientFD, int outClientFD, int inServerFD, int outServerFD);

      bool readPipes();

      /// Returns whether or not the message was handled
      virtual bool handleMessage(nlohmann::json const & message);
      /// Returns whether or not the response was handled
      virtual bool handleResponse(nlohmann::json const & response);

    private:
      /// Message sent from server to client
      void handleServerMessage(std::string & message);
      /// Message sent from client to server
      void handleClientMessage(std::string & message);

      std::optional<MessageRecord> getRecord(nlohmann::json const & message);

    private:
      BaseProcessor m_clientToServer;
      BaseProcessor m_serverToClient;
      std::map<int, MessageRecord> m_clientRecord; // Record of messages from client to server
      std::map<int, MessageRecord> m_serverRecord; // Record of messages from server to client
  };
}