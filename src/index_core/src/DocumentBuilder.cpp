#include "index/DocumentBuilder.hpp"

namespace idx
{

DocumentBuilder& DocumentBuilder::setName(const std::string& name)
{
    name_ = name;
    return *this;
}

DocumentBuilder& DocumentBuilder::setText(const std::string& text)
{
    text_ = text;
    return *this;
}

Document DocumentBuilder::build(size_t id)
{
    return Document{id, name_, text_};
}

} // namespace idx
