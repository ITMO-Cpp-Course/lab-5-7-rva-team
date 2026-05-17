#pragma once
#include "index/InvertedIndex.hpp"
#include "index/Result.hpp"
#include <memory>
#include <string>
#include <vector>

namespace idx
{

class UpdateTransaction;

// add() и remove() работают ТОЛЬКО внутри открытой транзакции.
// search() доступен всегда.
class IndexStore
{
  public:
    IndexStore();
    ~IndexStore(); // определён в .cpp — там доступен полный тип UpdateTransaction

    IndexStore(const IndexStore&) = delete;
    IndexStore& operator=(const IndexStore&) = delete;
    IndexStore(IndexStore&& other) = delete;
    IndexStore& operator=(IndexStore&& other) = delete;

    friend class UpdateTransaction;

    // Поставить документ в очередь на добавление (требует активной транзакции)
    Result<void> add(Document doc);

    // Поставить документ в очередь на удаление (требует активной транзакции)
    Result<void> remove(size_t id);

    // Найти документы по слову. Всегда доступен, не требует транзакции.
    Result<std::vector<size_t>> search(const std::string& word) const;

    // Открыть транзакцию. Ошибка: уже активна.
    Result<void> beginTransaction();

    // Применить все изменения транзакции к индексу. Ошибка: транзакция не открыта.
    Result<void> commitTransaction();

  private:
    InvertedIndex index_;
    bool transactionActive_ = false;
    std::unique_ptr<UpdateTransaction> transaction_; // создаётся при beginTransaction

    // Методы для UpdateTransaction
    bool has(size_t id) const;
    InvertedIndex& getIndex();
    void setTransactionActive(bool active);
};

} // namespace idx
