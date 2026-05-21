#include "index/IndexStore.hpp"
#include "index/UpdateTransaction.hpp"

namespace idx
{

// Конструктор и деструктор определены здесь, а не в .hpp —
// потому что здесь доступен полный тип UpdateTransaction (нужен unique_ptr для удаления)
IndexStore::IndexStore() = default;
IndexStore::~IndexStore() = default;

Result<void> IndexStore::add(Document doc)
{
    if (!transactionActive_)
        return std::unexpected(IndexError::TransactionNotActive);

    return transaction_->add(std::move(doc));
}

Result<void> IndexStore::remove(size_t id)
{
    if (!transactionActive_)
        return std::unexpected(IndexError::TransactionNotActive);

    return transaction_->remove(id);
}

std::vector<size_t> IndexStore::search(const std::string& word) const
{
    return index_.search(word);
}

Result<void> IndexStore::beginTransaction()
{
    if (transactionActive_)
        return std::unexpected(IndexError::TransactionAlreadyActive);

    // Создаём новую транзакцию на куче — она живёт до commit или до уничтожения IndexStore
    transaction_ = std::make_unique<UpdateTransaction>(*this);
    transactionActive_ = true;
    return {};
}

Result<void> IndexStore::commitTransaction()
{
    if (!transactionActive_)
        return std::unexpected(IndexError::TransactionNotActive);

    Result<void> res = transaction_->commit();
    // После commit() транзакция мертва — освобождаем память
    transaction_.reset();
    return res;
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
