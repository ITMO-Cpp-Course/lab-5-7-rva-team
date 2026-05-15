#pragma once
#include <expected>
#include <string>

namespace idx
{

enum class IndexError
{
    None = 0,
    DocumentAlreadyExists,
    DocumentNotFound,
    InvalidDocumentId,
    InvalidDocumentContent,
    TransactionAlreadyActive,
    TransactionNotActive,
    IndexCorrupted,
    InternalError
};

std::string errorToString(IndexError error);

template <typename T> using Result = std::expected<T, IndexError>;

} // namespace idx