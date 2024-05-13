#pragma once
#include <string>
#include <optional>
#include <functional>

namespace lsp_proxy {

  using HandleMessageCallback = std::function<std::optional<std::string>(std::string &)>;

  /* Reads data from an input file descriptor in the form of base protocol.
   * It process it before sending it sending it out to an output file descriptor.
  */
  class BaseProcessor
  {
    public:
      BaseProcessor(int inFD, int outFD, int bufferSize=16384);
      ~BaseProcessor();
      void setMessageCallback(HandleMessageCallback callback);

      bool readPipe();

    protected:
      virtual std::optional<std::string> process(std::string & message);

    private:
      void writePipe(std::string const & message);

      int m_inFD;
      int m_outFD;
      int m_bufferSize;
      char * m_buffer = nullptr;

      std::string m_message;
      HandleMessageCallback m_callback;
  };

}