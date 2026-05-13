#pragma once
#include <string>

namespace idx
{

class Document
{
  public:
    Document(size_t idx_, const std::string& name_, const std::string& text_);
    ~Document() = default;

    Document(const Document&) = delete;
    Document& operator=(const Document&) = delete;

    Document(Document&& other) noexcept;
    Document& operator=(Document&& other) noexcept;

    size_t getIndex() const;
    const std::string& getName() const;
    const std::string& getText() const;

  private:
    size_t idx;
    std::string name;
    std::string text;
};

} // namespace idx

