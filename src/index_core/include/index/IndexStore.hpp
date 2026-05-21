#pragma once
#include "index/InvertedIndex.hpp"
#include "index/Result.hpp"
#include <memory>
#include <string>
#include <vector>

namespace idx
{

class UpdateTransaction; // forward declaration — полный include только в .cpp

// Управляющий слой над InvertedIndex с транзакционным API.
// add() и remove() работают ТОЛЬКО внутри открытой транзакции.
// search() доступен всегда.
class IndexStore
{
  public:
    IndexStore();
    ~IndexStore(); // определён в .cpp — там доступен полный тип UpdateTransaction

    // Поставить документ в очередь на добавление (требует активной транзакции)
    Result<void> add(Document doc);

    // Поставить документ в очередь на удаление (требует активной транзакции)
    Result<void> remove(size_t id);

    // Найти документы по слову. Всегда доступен, не требует транзакции.
    std::vector<size_t> search(const std::string& word) const;

    // Открыть транзакцию. Ошибка: уже активна.
    Result<void> beginTransaction();

    // Применить все изменения транзакции к индексу. Ошибка: транзакция не открыта.
    Result<void> commitTransaction();

    // Методы для UpdateTransaction — не вызывай напрямую из прикладного кода
    bool has(size_t id) const;
    InvertedIndex& getIndex();
    void setTransactionActive(bool active);

  private:
    InvertedIndex index_;
    bool transactionActive_ = false;
    std::unique_ptr<UpdateTransaction> transaction_; // создаётся при beginTransaction
};

} // namespace idx
