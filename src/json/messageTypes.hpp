#pragma once
#include <string>
#include <vector>
#include <tuple>
#include <variant>
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

  struct SwitchSourceHeaderParams : public TextDocument
  {
  };

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

  struct DocumentLinkParams : public SymbolContainer
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

  struct DocumentHighlightParams
  {
    TextDocument textDocument;
    Position position;
    std::optional<std::string> workDoneToken;
    std::optional<std::string> partialResultToken;
  };

  inline void to_json(nlohmann::json & j, const DocumentHighlightParams & o) {
    j = nlohmann::json({{"textDocument", o.textDocument}, {"position", o.position}});
    if (o.workDoneToken) j["workDoneToken"] = *o.workDoneToken;
    if (o.partialResultToken) j["partialResultToken"] = *o.partialResultToken;
  }

  inline void from_json(const nlohmann::json & j, DocumentHighlightParams & o) {
    j.at("textDocument").get_to(o.textDocument);
    j.at("position").get_to(o.position);
    if (j.contains("workDoneToken")) o.workDoneToken = j.at("workDoneToken").get<std::string>();
    if (j.contains("partialResultToken")) o.partialResultToken = j.at("partialResultToken").get<std::string>();
  }
  struct CompletionContext
  {
    int triggerKind;
    std::optional<std::string> triggerCharacter;
  };

  inline void to_json(nlohmann::json & j, const CompletionContext & o) {
    j = nlohmann::json({{"triggerKind", o.triggerKind}});
    if (o.triggerCharacter) j["triggerCharacter"] = *o.triggerCharacter;
  }

  inline void from_json(const nlohmann::json & j, CompletionContext & o) {
    j.at("triggerKind").get_to(o.triggerKind);
    if (j.contains("triggerCharacter")) o.triggerCharacter = j.at("triggerCharacter").get<std::string>();
  }

  struct CompletionParams
  {
    std::optional<CompletionContext> context;
    TextDocument textDocument;
    Position position;
    std::optional<std::string> workDoneToken;
    std::optional<std::string> partialResultToken;
  };

  inline void to_json(nlohmann::json & j, const CompletionParams & o) {
    j = nlohmann::json({{"textDocument", o.textDocument}, {"position", o.position}});
    if (o.context) j["context"] = *o.context;
    if (o.workDoneToken) j["workDoneToken"] = *o.workDoneToken;
    if (o.partialResultToken) j["partialResultToken"] = *o.partialResultToken;
  }

  inline void from_json(const nlohmann::json & j, CompletionParams & o) {
    j.at("textDocument").get_to(o.textDocument);
    j.at("position").get_to(o.position);
    if (j.contains("context")) o.context = j.at("context").get<CompletionContext>();
    if (j.contains("workDoneToken")) o.workDoneToken = j.at("workDoneToken").get<std::string>();
    if (j.contains("partialResultToken")) o.partialResultToken = j.at("partialResultToken").get<std::string>();
  }

  struct CodeActionContext
  {
    std::vector<Diagnostic> diagnostics;
    std::optional<std::vector<std::string>> only;
    std::optional<int> triggerKind;
  };

  inline void to_json(nlohmann::json & j, const CodeActionContext & o) {
    j = nlohmann::json({{"diagnostics", o.diagnostics}});
    if (o.only) j["only"] = *o.only;
    if (o.triggerKind) j["triggerKind"] = *o.triggerKind;
  }

  inline void from_json(const nlohmann::json & j, CodeActionContext & o) {
    j.at("diagnostics").get_to(o.diagnostics);
    if (j.contains("only")) o.only = j.at("only").get<std::vector<std::string>>();
    if (j.contains("triggerKind")) o.triggerKind = j.at("triggerKind").get<int>();
  }

  struct CodeActionParams
  {
    TextDocument textDocument;
    Range range;
    CodeActionContext context;
    std::optional<std::string> workDoneToken;
    std::optional<std::string> partialResultToken;
  };

  inline void to_json(nlohmann::json & j, const CodeActionParams & o) {
    j = nlohmann::json({{"textDocument", o.textDocument}, {"range", o.range}, {"context", o.context}});
    if (o.workDoneToken) j["workDoneToken"] = *o.workDoneToken;
    if (o.partialResultToken) j["partialResultToken"] = *o.partialResultToken;
  }

  inline void from_json(const nlohmann::json & j, CodeActionParams & o) {
    j.at("textDocument").get_to(o.textDocument);
    j.at("range").get_to(o.range);
    j.at("context").get_to(o.context);
    if (j.contains("workDoneToken")) o.workDoneToken = j.at("workDoneToken").get<std::string>();
    if (j.contains("partialResultToken")) o.partialResultToken = j.at("partialResultToken").get<std::string>();
  }

  struct ParameterInformation
  {
    std::variant<std::string, std::tuple<int, int>> label;
    std::optional<std::string> documentation;
  };

  inline void to_json(nlohmann::json & j, const ParameterInformation & o) {
    j = nlohmann::json({});
    if (std::holds_alternative<std::string>(o.label))
      j["label"] = std::get<std::string>(o.label);
    else
      j["label"] = std::get<std::tuple<int, int>>(o.label);
    if (o.documentation) j["documentation"] = *o.documentation;
  }

  inline void from_json(const nlohmann::json & j, ParameterInformation & o) {
    if (j["label"].is_string())
      o.label = j.at("label").get<std::string>();
    else
      o.label = j.at("label").get<std::tuple<int, int>>();
    if (j.contains("documentation")) o.documentation = j.at("documentation").get<std::string>();
  }

  struct SignatureInformation
  {
    std::string label;
    std::optional<std::string> documentation;
    std::optional<std::vector<ParameterInformation>> parameters;
    std::optional<uint> activeParameter;
  };

  inline void to_json(nlohmann::json & j, const SignatureInformation & o) {
    j = nlohmann::json({{"label", o.label}});
    if (o.documentation) j["documentation"] = *o.documentation;
    if (o.parameters) j["parameters"] = *o.parameters;
    if (o.activeParameter) j["activeParameter"] = *o.activeParameter;
  }

  inline void from_json(const nlohmann::json & j, SignatureInformation & o) {
    j.at("label").get_to(o.label);
    if (j.contains("documentation")) o.documentation = j.at("documentation").get<std::string>();
    if (j.contains("parameters")) o.parameters = j.at("parameters").get<std::vector<ParameterInformation>>();
    if (j.contains("activeParameter")) o.activeParameter = j.at("activeParameter").get<uint>();
  }

  struct SignatureHelp
  {
    std::vector<SignatureInformation> signatures;
    std::optional<uint> activeSignature;
    std::optional<uint> activeParameter;
  };

  inline void to_json(nlohmann::json & j, const SignatureHelp & o) {
    j = nlohmann::json({{"signatures", o.signatures}});
    if (o.activeSignature) j["activeSignature"] = *o.activeSignature;
    if (o.activeParameter) j["activeParameter"] = *o.activeParameter;
  }

  inline void from_json(const nlohmann::json & j, SignatureHelp & o) {
    j.at("signatures").get_to(o.signatures);
    if (j.contains("activeSignature")) o.activeSignature = j.at("activeSignature").get<uint>();
    if (j.contains("activeParameter")) o.activeParameter = j.at("activeParameter").get<uint>();
  }

  struct SignatureHelpContext
  {
    int triggerKind;
    std::optional<std::string> triggerCharacter;
    bool isRetrigger;
    std::optional<SignatureHelp> activeSignatureHelp;
  };

  inline void to_json(nlohmann::json & j, const SignatureHelpContext & o) {
    j = nlohmann::json({{"triggerKind", o.triggerKind}, {"isRetrigger", o.isRetrigger}});
    if (o.triggerCharacter) j["triggerCharacter"] = *o.triggerCharacter;
    if (o.activeSignatureHelp) j["activeSignatureHelp"] = *o.activeSignatureHelp;
  }

  inline void from_json(const nlohmann::json & j, SignatureHelpContext & o) {
    j.at("triggerKind").get_to(o.triggerKind);
    j.at("isRetrigger").get_to(o.isRetrigger);
    if (j.contains("triggerCharacter")) o.triggerCharacter = j.at("triggerCharacter").get<std::string>();
    if (j.contains("activeSignatureHelp")) o.activeSignatureHelp = j.at("activeSignatureHelp").get<SignatureHelp>();
  }

  struct SignatureHelpParams
  {
    std::optional<SignatureHelpContext> context;
    TextDocument textDocument;
    Position position;
    std::optional<std::string> workDoneToken;
  };

  inline void to_json(nlohmann::json & j, const SignatureHelpParams & o) {
    j = nlohmann::json({{"textDocument", o.textDocument}, {"position", o.position}});
    if (o.context) j["context"] = *o.context;
    if (o.workDoneToken) j["workDoneToken"] = *o.workDoneToken;
  }

  inline void from_json(const nlohmann::json & j, SignatureHelpParams & o) {
    j.at("textDocument").get_to(o.textDocument);
    j.at("position").get_to(o.position);
    if (j.contains("context")) o.context = j.at("context").get<SignatureHelpContext>();
    if (j.contains("workDoneToken")) o.workDoneToken = j.at("workDoneToken").get<std::string>();
  }
}