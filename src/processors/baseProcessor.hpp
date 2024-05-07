#pragma once
#include <string>

namespace lsp_proxy {

  /* Reads data from an input file descriptor in the form of base protocol.
   * It process it before sending it sending it out to an output file descriptor.
  */
  class BaseProcessor
  {
    public:
      BaseProcessor(int inFD, int outFD, int bufferSize=16384);
      ~BaseProcessor();

      bool readPipe();

      virtual std::string process(std::string const & message);

    private:
      void writePipe(std::string const & message);

      int m_inFD;
      int m_outFD;
      int m_bufferSize;
      char * m_buffer = nullptr;

      std::string m_message;
  };

}