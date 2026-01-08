// Tool to track inactive (#if/#ifdef-skipped) code blocks and surface them to LLVM IR.
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/Attr.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Basic/Version.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Lex/Lexer.h"
#include "clang/Lex/PPCallbacks.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;
using namespace clang::tooling;

namespace {
struct InactiveBlock {
  SourceRange Range;
  std::string Condition;
  std::string File;
  std::string Content;  // Actual inactive code content
  unsigned BeginLine = 0;
  unsigned BeginCol = 0;
  unsigned EndLine = 0;
  unsigned EndCol = 0;
};

class InactiveCodePPCallbacks : public PPCallbacks {
public:
  InactiveCodePPCallbacks(const SourceManager &SM, const LangOptions &LangOpts,
                          std::vector<InactiveBlock> &Blocks)
      : SM(SM), LangOpts(LangOpts), Blocks(Blocks) {}

#if CLANG_VERSION_MAJOR >= 19
  SourceRange Skipped(SourceRange Range,
                      const Token &ConditionToken) override {
    handleSkipped(Range, ConditionToken);
    return Range;
  }
#else
  void SourceRangeSkipped(SourceRange Range, SourceLocation) override {
    Token Dummy;
    handleSkipped(Range, Dummy);
  }
#endif

private:
  const SourceManager &SM;
  const LangOptions &LangOpts;
  std::vector<InactiveBlock> &Blocks;

  void handleSkipped(SourceRange Range, const Token &CondTok) {
    InactiveBlock Block;
    Block.Range = Range;

    const SourceLocation FileBegin = SM.getFileLoc(Range.getBegin());
    const SourceLocation FileEnd = SM.getFileLoc(Range.getEnd());

    if (!SM.isWrittenInMainFile(FileBegin))
      return; // avoid rewriting system headers
    const PresumedLoc PBegin = SM.getPresumedLoc(FileBegin);
    const PresumedLoc PEnd = SM.getPresumedLoc(FileEnd);

    if (PBegin.isValid()) {
      Block.File = PBegin.getFilename();
      Block.BeginLine = PBegin.getLine();
      Block.BeginCol = PBegin.getColumn();
    }
    if (PEnd.isValid()) {
      Block.EndLine = PEnd.getLine();
      Block.EndCol = PEnd.getColumn();
    }

    // Extract the actual inactive code content
    SmallString<4096> CodeContent;
    const char *StartPtr = SM.getCharacterData(Range.getBegin());
    const char *EndPtr = SM.getCharacterData(Range.getEnd());
    if (StartPtr && EndPtr && EndPtr >= StartPtr) {
      CodeContent.append(StartPtr, EndPtr);
    }
    Block.Content = CodeContent.str().str();
    
    // Sanitize content for annotation
    std::string Sanitized;
    for (size_t i = 0; i < Block.Content.length() && i < 200; ++i) {
      char c = Block.Content[i];
      // Keep only safe characters for annotation
      if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || 
          (c >= '0' && c <= '9') || c == '(' || c == ')' || c == '{' || 
          c == '}' || c == '=' || c == '+' || c == '-' || c == '*' || 
          c == '/' || c == '<' || c == '>' || c == ';' || c == ',' || 
          c == ':' || c == '_') {
        Sanitized += c;
      } else if (c == '\n' || c == '\r' || c == '\t' || c == ' ') {
        if (Sanitized.empty() || Sanitized.back() != '_') {
          Sanitized += '_';
        }
      }
    }
    if (Block.Content.length() > 200) {
      Sanitized += "...";
    }
    Block.Content = Sanitized;

    if (CondTok.is(tok::identifier) || CondTok.is(tok::kw_if) ||
        CondTok.is(tok::hash) || CondTok.is(tok::unknown) ||
        CondTok.isAnyIdentifier()) {
      SmallString<64> Spelling;
      Spelling = Lexer::getSpelling(CondTok, SM, LangOpts);
      Block.Condition = Spelling.str().str();
    } else if (CondTok.is(tok::eof)) {
      Block.Condition = "<no-condition>";
    }

    Blocks.push_back(std::move(Block));
  }
};

class FunctionCollector : public RecursiveASTVisitor<FunctionCollector> {
public:
  explicit FunctionCollector(SourceManager &SM) : SM(SM) {}

  bool VisitFunctionDecl(FunctionDecl *FD) {
    if (FD && FD->hasBody()) {
      Functions.push_back(FD);
    }
    return true;
  }

  const std::vector<FunctionDecl *> &getFunctions() const { return Functions; }

private:
  SourceManager &SM;
  std::vector<FunctionDecl *> Functions;
};

class InactiveCodeConsumer : public ASTConsumer {
public:
  InactiveCodeConsumer(ASTContext &Ctx, Rewriter &R,
                       std::vector<InactiveBlock> &Blocks)
      : Ctx(Ctx), SM(Ctx.getSourceManager()), RewriterRef(R), Blocks(Blocks) {}

  void HandleTranslationUnit(ASTContext &Context) override {
    FunctionCollector Collector(SM);
    Collector.TraverseDecl(Context.getTranslationUnitDecl());
    const auto &Funcs = Collector.getFunctions();

    unsigned MarkerIdx = 0;
    for (const auto &Block : Blocks) {
      const SourceLocation Begin = SM.getFileLoc(Block.Range.getBegin());
      const SourceLocation End = SM.getFileLoc(Block.Range.getEnd());
      const FileID BlockFile = SM.getFileID(Begin);

      FunctionDecl *Containing = nullptr;
      for (FunctionDecl *FD : Funcs) {
        if (!FD->getBody())
          continue;
        SourceRange BodyRange = FD->getBody()->getSourceRange();
        SourceLocation BodyBegin = SM.getFileLoc(BodyRange.getBegin());
        SourceLocation BodyEnd = SM.getFileLoc(BodyRange.getEnd());

        if (SM.getFileID(BodyBegin) != BlockFile)
          continue;

        const bool StartsAfterBegin =
            !SM.isBeforeInTranslationUnit(Begin, BodyBegin);
        const bool EndsBeforeEnd =
            !SM.isBeforeInTranslationUnit(BodyEnd, End);
        if (StartsAfterBegin && EndsBeforeEnd) {
          Containing = FD;
          break;
        }
      }

      std::string Payload;
      llvm::raw_string_ostream OS(Payload);
      OS << "inactive_block: " << Block.File << ':' << Block.BeginLine << ':'
         << Block.BeginCol << "-" << Block.EndLine << ':' << Block.EndCol;
      if (!Block.Condition.empty())
        OS << " condition=" << Block.Condition;
      // Add code content as annotation metadata
      if (!Block.Content.empty())
        OS << " code=" << Block.Content;
      OS.flush();

      if (Containing) {
        auto *Attr = AnnotateAttr::CreateImplicit(Ctx, Payload, nullptr, 0,
                   SourceRange(Begin, Begin));
        Containing->addAttr(Attr);
      }

      // Always inject a file-scope marker at the end of the file so the information
      // survives to IR and is easy to match with the original source.
      const std::string MarkerName =
          "__clang_inactive_marker_" + std::to_string(MarkerIdx++);
      std::string Stub;
      Stub += "__attribute__((annotate(\"" + Payload + "\"), used)) ";
      Stub += "static void " + MarkerName + "(void) { }\n";
      const SourceLocation EndLoc = SM.getLocForEndOfFile(BlockFile);
      RewriterRef.InsertTextBefore(EndLoc, Stub);
    }
  }

private:
  ASTContext &Ctx;
  SourceManager &SM;
  Rewriter &RewriterRef;
  std::vector<InactiveBlock> &Blocks;
};

class InactiveCodeAction : public ASTFrontendAction {
public:
  InactiveCodeAction() = default;

  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 StringRef InFile) override {
    RewriterRef.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
    return std::make_unique<InactiveCodeConsumer>(CI.getASTContext(),
                                                  RewriterRef, Blocks);
  }

  bool BeginSourceFileAction(CompilerInstance &CI) override {
    CI.getPreprocessor().addPPCallbacks(std::make_unique<InactiveCodePPCallbacks>(
        CI.getSourceManager(), CI.getLangOpts(), Blocks));
    return true;
  }

  void EndSourceFileAction() override {
    RewriterRef.overwriteChangedFiles();
  }

private:
  Rewriter RewriterRef;
  std::vector<InactiveBlock> Blocks;
};

static llvm::cl::OptionCategory ToolCategory("inactive-code-tracker");

} // namespace

int main(int argc, const char **argv) {
  auto ExpectedParser = CommonOptionsParser::create(argc, argv, ToolCategory);
  if (!ExpectedParser) {
    llvm::errs() << ExpectedParser.takeError();
    return 1;
  }
  CommonOptionsParser &OptionsParser = ExpectedParser.get();
  ClangTool Tool(OptionsParser.getCompilations(),
                 OptionsParser.getSourcePathList());

  return Tool.run(newFrontendActionFactory<InactiveCodeAction>().get());
}
