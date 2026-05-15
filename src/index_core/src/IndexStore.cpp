#include "index/IndexStore.hpp"

namespace idx
{

Result<void> IndexStore::add(Document doc)
{
    if (transactionActive_)
        return std::unexpected(IndexError::TransactionAlreadyActive);

    if (index_.has(doc.getIndex()))
        return std::unexpected(IndexError::DocumentAlreadyExists);

    index_.add(std::move(doc));
    return {};
}

Result<void> IndexStore::remove(size_t id)
{
    if (transactionActive_)
        return std::unexpected(IndexError::TransactionAlreadyActive);

    if (!index_.has(id))
        return std::unexpected(IndexError::DocumentNotFound);

    index_.remove(id);
    return {};
}

Result<std::vector<size_t>> IndexStore::search(const std::string& word) const
{
    return index_.search(word);
}

Result<void> IndexStore::beginTransaction()
{
    if (transactionActive_)
        return std::unexpected(IndexError::TransactionAlreadyActive);

    transactionActive_ = true;
    return {};
}

bool IndexStore::has(size_t id) const
{
    return index_.has(id);
}

InvertedIndex& IndexStore::getIndex()
{
    return index_;
}

void IndexStore::setTransactionActive(bool active)
{
    transactionActive_ = active;
}

} // namespace idx
