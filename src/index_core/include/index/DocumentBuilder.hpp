#pragma once
#include "index/Document.hpp"
#include <string>

namespace idx
{

class DocumentBuilder
{
  public:
    DocumentBuilder& setName(const std::string& name);
    DocumentBuilder& setText(const std::string& text);
    Document build(size_t id);

  private:
    std::string name_;
    std::string text_;
};

} // namespace idx
