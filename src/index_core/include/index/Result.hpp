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
    DocumentAlreadyInRemovalList,
    DocumentAlreadyInAdditionList,
    InvalidDocumentId,
    InvalidDocumentContent,
    TransactionAlreadyActive,
    TransactionNotActive,
    TransactionCommitted,
    IndexCorrupted,
    InternalError
};

std::string errorToString(IndexError error);

template <typename T> using Result = std::expected<T, IndexError>;

} // namespace idx
