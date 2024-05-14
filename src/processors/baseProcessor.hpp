#pragma once
#include <string>
#include <optional>
#include <functional>

namespace lsp_proxy {

  using HandleMessageCallback = std::function<void(std::string &)>;

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
      void writePipe(std::string const & message);

    protected:
      virtual void process(std::string & message);

    private:
      int m_inFD;
      int m_outFD;
      int m_bufferSize;
      char * m_buffer = nullptr;

      std::string m_message;
      HandleMessageCallback m_callback;
  };

}