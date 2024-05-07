#pragma once
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include "baseTypes.hpp"

namespace lsp_proxy {

  struct HoverParams
  {
    TextDocument textDocument;
    Position position;
  };

  inline void to_json(nlohmann::json & j, const HoverParams & o) {
    j = nlohmann::json({{"textDocument", o.textDocument}, {"position", o.position}});
  }

  inline void from_json(const nlohmann::json & j, HoverParams & o) {
    j.at("textDocument").get_to(o.textDocument);
    j.at("position").get_to(o.position);
  }

  struct DidOpenParams
  {
    TextDocumentItem textDocument;
  };

  inline void to_json(nlohmann::json & j, const DidOpenParams & o) {
    j = nlohmann::json({{"textDocument", o.textDocument}});
  }

  inline void from_json(const nlohmann::json & j, DidOpenParams & o) {
    j.at("textDocument").get_to(o.textDocument);
  }

  struct TextDocumentContainer
  {
    TextDocument textDocument;
  };

  inline void to_json(nlohmann::json & j, const TextDocumentContainer & o) {
    j = nlohmann::json({{"textDocument", o.textDocument}});
  }

  inline void from_json(const nlohmann::json & j, TextDocumentContainer & o) {
    j.at("textDocument").get_to(o.textDocument);
  }

  struct DidSaveParams : public TextDocumentContainer
  {
    TextDocument textDocument;
  };

  struct DidCloseParams : public TextDocumentContainer
  {
    TextDocument textDocument;
  };

  struct DidChangeParams
  {
    VersionedTextDocument versionedTextDocument;
    std::vector<TextDocumentContentChangeEvent> contentChanges;
  };


  inline void to_json(nlohmann::json & j, const DidChangeParams & o) {
    j = nlohmann::json({{"textDocument", o.versionedTextDocument}, {"contentChanges", o.contentChanges}});
  }


  inline void from_json(const nlohmann::json & j, DidChangeParams & o) {
    o.contentChanges.clear();
    j.at("textDocument").get_to(o.versionedTextDocument);
    for (auto const & change : j["contentChanges"])
      o.contentChanges.push_back(change);
  }

  struct ClangdFileStatus
  {
    std::string uri;
    std::string state;
  };

  inline void to_json(nlohmann::json & j, const ClangdFileStatus & o) {
    j = nlohmann::json({{"uri", o.uri}, {"state", o.state}});
  }

  inline void from_json(const nlohmann::json & j, ClangdFileStatus & o) {
    j.at("uri").get_to(o.uri);
    j.at("state").get_to(o.state);
  }

}