#include "index/UpdateTransaction.hpp"
#include "index/IndexStore.hpp"

namespace idx
{

UpdateTransaction::UpdateTransaction(IndexStore& store_) : store{&store_} {}

UpdateTransaction::~UpdateTransaction()
{
    if (!committed)
    {
        rollback();
    }
}

void UpdateTransaction::add(Document doc)
{
    if (committed)
        return;

    size_t id = doc.getIndex();
    if (isInToAdd(id))
        return;
    if (isInToRemove(id))
        return;
    if (store->has(id))
        return;

    to_add.push_back(std::move(doc));
}

void UpdateTransaction::remove(size_t id)
{
    if (committed)
        return;

    if (isInToAdd(id))
        return;
    if (isInToRemove(id))
        return;
    if (!store->has(id))
        return;

    to_remove.push_back(id);
}

void UpdateTransaction::commit()
{
    if (committed)
        return;

    for (auto& doc : to_add)
        store->getIndex().add(std::move(doc));

    for (size_t id : to_remove)
        store->getIndex().remove(id);

    committed = true;
    to_add.clear();
    to_remove.clear();
    store->setTransactionActive(false);
    store = nullptr;
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
