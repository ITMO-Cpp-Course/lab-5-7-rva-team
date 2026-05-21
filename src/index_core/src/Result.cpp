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
    case IndexError::DocumentAlreadyInAdditionList:
        return "Document is already in queue for addition";
    case IndexError::DocumentAlreadyInRemovalList:
        return "Document is already in queue for removal";
    case IndexError::InvalidDocumentId:
        return "Invalid document ID";
    case IndexError::InvalidDocumentContent:
        return "Invalid document content";
    case IndexError::TransactionAlreadyActive:
        return "Transaction already active";
    case IndexError::TransactionNotActive:
        return "Transaction not active";
    case IndexError::TransactionCommitted:
        return "Transaction already committed";
    case IndexError::IndexCorrupted:
        return "Index is corrupted";
    case IndexError::WordNotFound:
        return "Word not found in index";
    case IndexError::InternalError:
        return "Internal error";
    }
    return "Unknown error";
}

} // namespace idx
