#pragma once
#include "baseProcessor.hpp"

namespace lsp_proxy {

  /* Parses json from the input of a pipe, processes it and writes it to the output of the pipe.
  */
  class JSONProcessor : public BaseProcessor
  {
    public:
      JSONProcessor(int inFD, int outFD, int bufferSize=16384) : BaseProcessor(inFD, outFD, bufferSize) {};
      ~JSONProcessor() {};

      std::string process(std::string const & message) override;
  };

}