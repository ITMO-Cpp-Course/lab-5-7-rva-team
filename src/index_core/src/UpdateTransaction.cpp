#pragma once

#include "index/UpdateTransaction.hpp"

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
    {
        return; // error - transaction not active
    }

    size_t id = doc.getIndex();
    if (isInToAdd(id))
    {
        return; // error - doc already in to_add list, no need to add twice
    }
    if (isInToRemove(id))
    {
        return; // error - doc in to_remove list, no need to add
    }
    if (store->has(id)) // supposed method for checking if the doc is in IndexStore
    {
        return; // error - doc already in IndexStore, no need to add twice
    }

    to_add.push_back(std::move(doc));
    return;
}

void UpdateTransaction::remove(size_t id)
{
    if (committed)
    {
        return; // error - transaction not active
    }

    if (isInToAdd(id))
    {
        return; // error - doc in to_add list, no need to remove
    }
    if (isInToRemove(id))
    {
        return; // error - doc already in to_remove list, no need to remove twice
    }
    if (!store->has(id)) // supposed method for checking if the doc is in IndexStore
    {
        return; // error - no doc in IndexStore, nothing to remove
    }

    to_remove.push_back(id);
    retrun;
}

void UpdateTransaction::commit()
{
    if (committed)
    {
        return; // error - transaction not active
    }

    // might wanna add more validity checks, but seems like overkill

    for (auto& doc : to_add)
    {
        store->getIndex().add(std::move(doc)); // supposed method for adding objects in IndexStore
    }
    for (size_t id : to_remove)
    {
        store->getIndex().remove(id); // supposed method for removing objects in IndexStore
    }

    committed = true;
    to_add.clear();
    to_remove.clear();
    store->setTransactionActive(
        false); // supposed flag switch in IndexStore, the flag should prohibit index changes during the transaction
    store = nullptr;
    return;
}

bool UpdateTransaction::isInToAdd(size_t id) const
{
    for (const auto& doc : to_add)
    {
        if (doc.getIndex() == id)
        {
            return true;
        }
    }

    return false;
}

bool UpdateTransaction::isInToRemove(size_t id) const
{
    for (const auto& doc : to_remove)
    {
        if (doc.getIndex() == id)
        {
            return true;
        }
    }

    return false;
}

void UpdateTransaction::rollback()
{
    to_add.clear();
    to_remove.clear();
    if (store)
    {
        store->setTransactionActive(
            false); // supposed flag switch in IndexStore, the flag should prohibit index changes during the transaction
        store = nullptr;
    }
    committed = false;
}

} // namespace idx