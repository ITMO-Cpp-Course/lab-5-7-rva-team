#include "src/index_core/include/index/Result.hpp"

namespace idx
{

const IndexError IndexError::None(0, "Success");
const IndexError IndexError::DocumentAlreadyExists(1, "Document already exists");
const IndexError IndexError::DocumentNotFound(2, "Document not found");
const IndexError IndexError::InvalidDocumentId(3, "Invalid document ID");
const IndexError IndexError::InvalidDocumentContent(4, "Invalid document content");
const IndexError IndexError::TransactionAlreadyActive(5, "Transaction already active");
const IndexError IndexError::TransactionNotActive(6, "No active transaction");
const IndexError IndexError::IndexCorrupted(7, "Index is corrupted");
const IndexError IndexError::InternalError(8, "Internal error");

std::string errorToString(const IndexError& error)
{
    if (error == IndexError::None)
    {
        return "Success";
    }
    else if (error == IndexError::DocumentAlreadyExists)
    {
        return "Document already exists";
    }
    else if (error == IndexError::DocumentNotFound)
    {
        return "Document not found";
    }
    else if (error == IndexError::InvalidDocumentId)
    {
        return "Invalid document ID";
    }
    else if (error == IndexError::InvalidDocumentContent)
    {
        return "Invalid document content";
    }
    else if (error == IndexError::TransactionAlreadyActive)
    {
        return "Transaction already active";
    }
    else if (error == IndexError::TransactionNotActive)
    {
        return "No active transaction";
    }
    else if (error == IndexError::IndexCorrupted)
    {
        return "Index is corrupted";
    }
    else if (error == IndexError::InternalError)
    {
        return "Internal error";
    }
    else
    {
        return "Unknown error";
    }
}

} // namespace idx