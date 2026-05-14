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
        return; // ошибка - транзакция неактивна или коммит уже сделан
    }

    size_t id = doc.getIndex();
    if (isInToAdd(id))
    {
        return; // ошибка - документ уже в списке на добавление, добавление дубликата некорректно
    }
    if (isInToRemove(id))
    {
        return; // ошибка - документ в списке на удаление, нет смысла добавлять
    }
    if (store->has(id)) // предполагается проверка на наличие документа в indexstore
    {
        return; // ошибка - документ уже в indexstore, добавление дубликата некорректно
    }

    to_add.push_back(std::move(doc));
    return;
}

void UpdateTransaction::remove(size_t id)
{
    if (committed)
    {
        return; // ошибка - транзакция неактивна или коммит уже сделан
    }

    if (isInToAdd(id))
    {
        return; // ошибка - документ уже в списке на добавление, нет смысла удалять
    }
    if (isInToRemove(id))
    {
        return; // ошибка - документ в списке на удаление, удалять нечего
    }
    if (!store->has(id)) // проверка на наличие документа в indexstore
    {
        return; // ошибка - документа нет в indexstore, удалять нечего
    }

    to_remove.push_back(id);
    retrun;
}

void UpdateTransaction::commit()
{
    if (committed)
    {
        return; // ошибка - транзакция неактивна или коммит уже сделан
    }

    // можно докинуть повторные проверки, но как будто это излишне

    for (auto& doc : to_add)
    {
        store->getIndex().add(std::move(doc)); // предполагается метод добавления элементов в indexstore
    }
    for (size_t id : to_remove)
    {
        store->getIndex().remove(id); // предполагается метод удаления элементов в indexstore
    }

    committed = true;
    to_add.clear();
    to_remove.clear();
    store->setTransactionActive(false); // предполагается переключене флага в indexstore, который бы запрещал операции
                                        // над индексом в ходе транзакции
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
        store->setTransactionActive(false); // предполагается переключене флага в indexstore, который бы запрещал операции
                                            // над индексом в ходе транзакции
        store = nullptr;
    }
    committed = false;
}

} // namespace idx