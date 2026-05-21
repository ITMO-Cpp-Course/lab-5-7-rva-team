#include "index/UpdateTransaction.hpp"
#include "index/IndexStore.hpp"

namespace idx
{

UpdateTransaction::UpdateTransaction(IndexStore& store_) : store{&store_} {}

UpdateTransaction::~UpdateTransaction()
{
    if (!committed)
        rollback();
}

Result<void> UpdateTransaction::add(Document doc)
{
    if (committed)
        return std::unexpected(IndexError::TransactionCommitted);

    size_t id = doc.getIndex();
    if (isInToAdd(id))
        return std::unexpected(IndexError::DocumentAlreadyInAdditionList);
    if (isInToRemove(id))
        return std::unexpected(IndexError::DocumentAlreadyInRemovalList);
    if (store->has(id))
        return std::unexpected(IndexError::DocumentAlreadyExists);

    to_add.push_back(std::move(doc));
    return {};
}

Result<void> UpdateTransaction::remove(size_t id)
{
    if (committed)
        return std::unexpected(IndexError::TransactionCommitted);

    if (isInToAdd(id))
        return std::unexpected(IndexError::DocumentAlreadyInAdditionList);
    if (isInToRemove(id))
        return std::unexpected(IndexError::DocumentAlreadyInRemovalList);
    if (!store->has(id))
        return std::unexpected(IndexError::DocumentNotFound);

    to_remove.push_back(id);
    return {};
}

Result<void> UpdateTransaction::commit()
{
    if (committed)
        return std::unexpected(IndexError::TransactionCommitted);

    for (auto& doc : to_add)
        store->getIndex().add(std::move(doc));

    for (size_t id : to_remove)
        store->getIndex().remove(id);

    committed = true;
    to_add.clear();
    to_remove.clear();
    store->setTransactionActive(false);
    store = nullptr;
    return {};
}

bool UpdateTransaction::isInToAdd(size_t id) const
{
    for (const auto& doc : to_add)
    {
        if (doc.getIndex() == id)
            return true;
    }
    return false;
}

bool UpdateTransaction::isInToRemove(size_t id) const
{
    for (size_t removedId : to_remove)
    {
        if (removedId == id)
            return true;
    }
    return false;
}

void UpdateTransaction::rollback()
{
    to_add.clear();
    to_remove.clear();
    if (store)
    {
        store->setTransactionActive(false);
        store = nullptr;
    }
    committed = false;
}

} // namespace idx
