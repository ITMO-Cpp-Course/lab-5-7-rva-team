#include "index/Document.hpp"

namespace idx
{

Document::Document(size_t idx_, const std::string& name_, const std::string& text_)
    : idx{idx_}, name{name_}, text{text_}
{
}

Document::Document(Document&& other) noexcept : idx{other.idx}, name{other.name}, text{other.text}
{
    other.idx = 0;
    other.name = "";
    other.text = "";
}

Document& Document::operator=(Document&& other) noexcept
{
    if (this != &other)
    {
        idx = other.idx;
        name = other.name;
        text = other.text;
        other.idx = 0;
        other.name = "";
        other.text = "";
    }

    return *this;
}

size_t Document::getIndex() const
{
    return idx;
}

const std::string& Document::getName() const
{
    return name;
}

const std::string& Document::getText() const
{
    return text;
}

} // namespace idx

