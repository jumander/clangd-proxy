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
  };

  struct DidCloseParams : public TextDocumentContainer
  {
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
    j.at("textDocument").get_to(o.versionedTextDocument);
    o.contentChanges = j.at("contentChanges").get<std::vector<TextDocumentContentChangeEvent>>();
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

  struct CodeDescription
  {
    std::string href;
  };

  inline void to_json(nlohmann::json & j, const CodeDescription & o) {
    j = nlohmann::json({{"href", o.href}});
  }

  inline void from_json(const nlohmann::json & j, CodeDescription & o) {
    j.at("href").get_to(o.href);
  }

  struct DiagnosticRelatedInformation
  {
    Location location;
    std::string message;
  };

  inline void to_json(nlohmann::json & j, const DiagnosticRelatedInformation & o) {
    j = nlohmann::json({{"location", o.location}, {"message", o.message}});
  }

  inline void from_json(const nlohmann::json & j, DiagnosticRelatedInformation & o) {
    j.at("location").get_to(o.location);
    j.at("message").get_to(o.message);
  }

  struct Diagnostic
  {
    Range range;
    std::optional<int> severity;
    std::optional<std::string> code;
    std::optional<CodeDescription> codeDescription;
    std::optional<std::string> source;
    std::string message;
    std::optional<std::vector<int>> tags;
    std::optional<std::vector<DiagnosticRelatedInformation>> relatedInformation;
    std::optional<std::string> data;
  };

  inline void to_json(nlohmann::json & j, const Diagnostic & o) {
    j = nlohmann::json({{"range", o.range}, {"message", o.message}});
    if (o.severity) j["severity"] = *o.severity;
    if (o.code) j["code"] = *o.code;
    if (o.codeDescription) j["codeDescription"] = *o.codeDescription;
    if (o.source) j["source"] = *o.source;
    if (o.tags) j["tags"] = *o.tags;
    if (o.relatedInformation) j["relatedInformation"] = *o.relatedInformation;
    if (o.data) j["data"] = *o.data;
  }

  inline void from_json(const nlohmann::json & j, Diagnostic & o) {
    j.at("range").get_to(o.range);
    j.at("message").get_to(o.message);
    o.severity = j.contains("severity") ? j.at("severity").get<int>() : std::optional<int>{};
    o.code = j.contains("code") ? j.at("code").get<std::string>() : std::optional<std::string>{};
    o.codeDescription = j.contains("codeDescription") ? j.at("codeDescription").get<CodeDescription>() : std::optional<CodeDescription>{};
    o.source = j.contains("source") ? j.at("source").get<std::string>() : std::optional<std::string>{};
    if (j.contains("tags")) o.tags = j.at("tags").get<std::vector<int>>();
    if (j.contains("relatedInformation")) o.relatedInformation = j.at("relatedInformation").get<std::vector<DiagnosticRelatedInformation>>();
    if (j.contains("data")) o.data = j.at("data").get<std::string>();
  }


  struct PublishDiagnosticsParams
  {
    std::string uri;
    std::optional<int> version;
    std::vector<Diagnostic> diagnostics;
  };

  inline void to_json(nlohmann::json & j, const PublishDiagnosticsParams & o) {
    j = nlohmann::json({{"uri", o.uri}, {"diagnostics", o.diagnostics}});
    if (o.version) j["version"] = *o.version;
  }

  inline void from_json(const nlohmann::json & j, PublishDiagnosticsParams & o) {
    j.at("uri").get_to(o.uri);
    j.at("diagnostics").get_to(o.diagnostics);
    if (j.contains("version")) o.version = j.at("version").get<int>();
  }

  struct SymbolContainer
  {
    TextDocument textDocument;
    std::optional<std::string> workDoneToken;
    std::optional<std::string> partialResultToken;
  };

  inline void to_json(nlohmann::json & j, const SymbolContainer & o) {
    j = nlohmann::json({{"textDocument", o.textDocument}});
    if (o.workDoneToken) j["workDoneToken"] = *o.workDoneToken;
    if (o.partialResultToken) j["partialResultToken"] = *o.partialResultToken;
  }

  inline void from_json(const nlohmann::json & j, SymbolContainer & o) {
    j.at("textDocument").get_to(o.textDocument);
    if (j.contains("workDoneToken")) o.workDoneToken = j.at("workDoneToken").get<std::string>();
    if (j.contains("partialResultToken")) o.partialResultToken = j.at("partialResultToken").get<std::string>();
  }

  struct DocumentSymbolParams : public SymbolContainer
  {
  };

  struct SemanticTokensParams : public SymbolContainer
  {
  };

  struct SemanticTokensDeltaParams
  {
    TextDocument textDocument;
    std::string previousResultId;
    std::optional<std::string> workDoneToken;
    std::optional<std::string> partialResultToken;
  };

  inline void to_json(nlohmann::json & j, const SemanticTokensDeltaParams & o) {
    j = nlohmann::json({{"textDocument", o.textDocument}, {"previousResultId", o.previousResultId}});
    if (o.workDoneToken) j["workDoneToken"] = *o.workDoneToken;
    if (o.partialResultToken) j["partialResultToken"] = *o.partialResultToken;
  }

  inline void from_json(const nlohmann::json & j, SemanticTokensDeltaParams & o) {
    j.at("textDocument").get_to(o.textDocument);
    j.at("previousResultId").get_to(o.previousResultId);
    if (j.contains("workDoneToken")) o.workDoneToken = j.at("workDoneToken").get<std::string>();
    if (j.contains("partialResultToken")) o.partialResultToken = j.at("partialResultToken").get<std::string>();
  }
}