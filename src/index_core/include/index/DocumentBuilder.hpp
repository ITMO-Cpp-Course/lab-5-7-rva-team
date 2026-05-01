#pragma once
#include <string>
#include "index/Document.hpp"

namespace idx
{

// Вспомогательный класс для удобного создания Document.
// Методы возвращают *this, поэтому можно писать цепочкой:
//   DocumentBuilder{}.setName("doc").setText("hello world").build(1)
class DocumentBuilder
{
  public:
    DocumentBuilder& setName(const std::string& name);
    DocumentBuilder& setText(const std::string& text);

    // Создать документ с указанным id. id должен быть уникальным в пределах индекса.
    Document build(size_t id);

  private:
    std::string name_;
    std::string text_;
};

} // namespace idx

