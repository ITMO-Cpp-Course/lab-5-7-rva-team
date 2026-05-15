#pragma once
#include "index/InvertedIndex.hpp"
#include "index/Result.hpp"
#include <string>
#include <vector>

namespace idx
{

class UpdateTransaction; // forward declaration — полное определение не нужно

// Управляющий слой над InvertedIndex.
// Все публичные операции возвращают Result<T> — явно сигнализируя об ошибках.
// Пока активна транзакция, прямые add/remove заблокированы.
class IndexStore
{
  public:
    // Добавить документ. Ошибка: документ уже есть, или активна транзакция.
    Result<void> add(Document doc);

    // Удалить документ. Ошибка: документ не найден, или активна транзакция.
    Result<void> remove(size_t id);

    // Найти документы по слову. Возвращает пустой список если ничего не найдено.
    Result<std::vector<size_t>> search(const std::string& word) const;

    // Начать транзакцию. После этого создай UpdateTransaction(store).
    // Ошибка: транзакция уже активна.
    Result<void> beginTransaction();

    // Методы для UpdateTransaction — не вызывай напрямую из прикладного кода
    bool has(size_t id) const;
    InvertedIndex& getIndex();
    void setTransactionActive(bool active);

  private:
    InvertedIndex index_;
    bool transactionActive_ = false;
};

} // namespace idx
