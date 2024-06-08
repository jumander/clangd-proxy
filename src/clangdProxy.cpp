#include "clangdProxy.hpp"
#include <iostream>
#include <set>

#include "json/messageTypes.hpp"

namespace lsp_proxy {

  ClangdProxy::ClangdProxy(int inClientFD, int outClientFD, int inServerFD, int outServerFD)
    : m_clientToServer(BaseProcessor(inClientFD, outClientFD)), m_serverToClient(BaseProcessor(inServerFD, outServerFD))
  {
    // Set callbacks
    m_clientToServer.setMessageCallback([this](std::string & message){
      return handleServerMessage(message);
    });
    m_serverToClient.setMessageCallback([this](std::string & message){
      return handleClientMessage(message);
    });
  }

  bool ClangdProxy::readPipes()
  {
    bool changed = false;

    changed |= m_clientToServer.readPipe();

    changed |= m_serverToClient.readPipe();

    return changed;
  }

  void ClangdProxy::handleServerMessage(std::string & message)
  {
    auto jsonMessage = nlohmann::json(message);
    if (auto const & record = getRecord(jsonMessage))
      m_serverRecord[record->id] = *record;

    std::cout << "M to server" << std::endl;
    if (!handleMessage(jsonMessage))
      m_clientToServer.writePipe(message);
  }

  void ClangdProxy::handleClientMessage(std::string & message)
  {
    auto jsonMessage = nlohmann::json(message);
    if (auto const & record = getRecord(jsonMessage))
      m_clientRecord[record->id] = *record;

    std::cout << "M to client" << std::endl;
    if (!handleMessage(jsonMessage))
      m_serverToClient.writePipe(message);
  }

  std::optional<MessageRecord> ClangdProxy::getRecord(nlohmann::json const & message)
  {
    if (message.contains("method") && message.contains("id"))
    {
      MessageRecord record;
      record.id = message["id"].get<int>();
      record.method = message["method"].get<std::string>();
      return record;
    }
    return std::nullopt;
  }

  bool ClangdProxy::handleMessage(nlohmann::json const & message)
  {
    if (message.contains("method"))
    {
      std::string method = message["method"];
      method.erase(remove(method.begin(), method.end(), '\"'), method.end());
      std::set<std::string> ignoredMethods = {"initialize", "initialized",
        "$/setTrace", "$/progress", "$/cancelRequest",
        "window/workDoneProgress/create", "workspace/semanticTokens/refresh",
        };

      auto const & params = message["params"];
      if (method == "textDocument/hover")
        assertType<HoverParams>(params);
      else if (method == "textDocument/didOpen")
        assertType<DidOpenParams>(params);
      else if (method == "textDocument/didSave")
        assertType<DidSaveParams>(params);
      else if (method == "textDocument/didClose")
        assertType<DidCloseParams>(params);
      else if (method == "textDocument/didChange")
        assertType<DidChangeParams>(params);
      else if (method == "textDocument/clangd.fileStatus")
        assertType<ClangdFileStatus>(params);
      else if (method == "textDocument/publishDiagnostics")
        assertType<PublishDiagnosticsParams>(params);
      else if (method == "textDocument/documentSymbol")
        assertType<DocumentSymbolParams>(params);
      else if (method == "textDocument/semanticTokens/full")
        assertType<SemanticTokensParams>(params);
      else if (method == "textDocument/semanticTokens/full/delta")
        assertType<SemanticTokensDeltaParams>(params);
      else if (method == "textDocument/documentHighlight")
        assertType<DocumentHighlightParams>(params);
      else if (method == "textDocument/documentLink")
        assertType<DocumentLinkParams>(params);
      else if (method == "textDocument/completion")
        assertType<CompletionParams>(params);
      else if (method == "textDocument/codeAction")
        assertType<CodeActionParams>(params);
      else if (method == "textDocument/switchSourceHeader")
        assertType<SwitchSourceHeaderParams>(params);
      else if (method == "textDocument/signatureHelp")
        assertType<SignatureHelpParams>(params);
      else if (method == "textDocument/prepareRename")
        assertType<PrepareRenameParams>(params);
      else if (method == "textDocument/rename")
        assertType<RenameParams>(params);
      else if (!ignoredMethods.contains(method))
      {
        std::cout << "unhandled method " << method << std::endl;
      }
      // if (method == "textDocument/signatureHelp")
      //   std::cout << params.dump() << std::endl;
    }
    else
      std::cout << "reply" << std::endl;

    return false;
  }

  bool ClangdProxy::handleResponse(nlohmann::json const & response)
  {
    return false;
  }
}