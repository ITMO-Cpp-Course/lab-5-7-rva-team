#include "index/Result.hpp"

namespace idx
{

std::string errorToString(IndexError error)
{
    switch (error)
    {
    case IndexError::None:
        return "Success";
    case IndexError::DocumentAlreadyExists:
        return "Document already exists";
    case IndexError::DocumentNotFound:
        return "Document not found";
    case IndexError::InvalidDocumentId:
        return "Invalid document ID";
    case IndexError::InvalidDocumentContent:
        return "Invalid document content";
    case IndexError::TransactionAlreadyActive:
        return "Transaction already active";
    case IndexError::TransactionNotActive:
        return "No active transaction";
    case IndexError::IndexCorrupted:
        return "Index is corrupted";
    case IndexError::InternalError:
        return "Internal error";
    }
    return "Unknown error";
}

} // namespace idx
