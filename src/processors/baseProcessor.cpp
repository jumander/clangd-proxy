#include "baseProcessor.hpp"
#include <fcntl.h>
#include <iostream>
#include <sys/wait.h>

namespace lsp_proxy {

  BaseProcessor::BaseProcessor(int inFD, int outFD, int bufferSize)
   : m_inFD(inFD), m_outFD(outFD), m_bufferSize(bufferSize)
  {
    // Make input pipe non-blocking
    fcntl(inFD, F_SETFL, O_NONBLOCK | fcntl(inFD, F_GETFL, 0));

    m_buffer = new char[bufferSize];
  }

  BaseProcessor::~BaseProcessor()
  {
    if (m_buffer)
      delete [] m_buffer;
  }

  void BaseProcessor::setMessageCallback(HandleMessageCallback callback)
  {
    m_callback = callback;
  }

  bool BaseProcessor::readPipe()
  {
    ssize_t bytes_read = read(m_inFD, m_buffer, m_bufferSize);
    if (bytes_read < 1)
      return false;
    m_message.append(m_buffer, bytes_read);

    size_t headerPos = 0;
    size_t contentLength;
    while (true)
    {
      size_t termPos = m_message.find("\r\n", headerPos);
      if (termPos == -1)
        break;
      else // Found terminator
      {
        auto const headerString = m_message.substr(headerPos, termPos);

        size_t lengthPos = headerString.find("Content-Length");
        size_t typePos = headerString.find("Content-Type");
        if (lengthPos != -1) // content length
        {
          auto const contentLengthStr = headerString.substr(headerPos + lengthPos + 15, termPos);
          contentLength = std::stoi(contentLengthStr);
        }
        if (typePos != -1)
          std::cout << "FOUND CONTENT TYPE HEADER" << std::endl;

        headerPos = termPos + 2;

        if (lengthPos == -1 && lengthPos == -1) // End of header
        {
          if (m_message.size() >= headerPos + contentLength) // Read content
          {
            auto content = m_message.substr(headerPos, contentLength);
            if (auto processedMessage = process(content))
              writePipe(*processedMessage);
            m_message = m_message.substr(headerPos + contentLength);
            headerPos = 0;
          }
          else
            break;
        }
      }
    }

    return true;
  }

  void BaseProcessor::writePipe(std::string const & content)
  {
    std::string header = "Content-Length: " + std::to_string(content.size()) + "\r\n\r\n";
    std::string message = header + content;
    write(m_outFD, message.c_str(), message.size());
  }

  std::optional<std::string> BaseProcessor::process(std::string & message)
  {
    if (m_callback)
      return m_callback(message);
    else
      return message;
  }

}