#pragma once
#include "index/Document.hpp"
#include <string>
#include <unordered_map>
#include <vector>

namespace idx
{

// Основная структура поискового индекса.
// Хранит документы и позволяет быстро искать по словам.
class InvertedIndex
{
  public:
    // Добавить документ в индекс (передаётся по значению, внутри перемещается)
    void add(Document doc);

    // Удалить документ и все его слова из индекса
    void remove(size_t docId);

    // Проверить, существует ли документ с данным id
    bool has(size_t docId) const;

    // Вернуть ID всех документов, в которых встречается слово (регистр не важен)
    std::vector<size_t> search(const std::string& word) const;

    // Вернуть количество вхождений слова в конкретном документе (0 если нет)
    size_t wordCount(const std::string& word, size_t docId) const;

  private:
    // Хранилище документов: id -> Document
    std::unordered_map<size_t, Document> docs;

    // Инвертированный индекс: слово -> { id документа -> количество вхождений }
    std::unordered_map<std::string, std::unordered_map<size_t, size_t>> index;

    // Разбить текст на слова: убирает пунктуацию, приводит к lowercase
    static std::vector<std::string> tokenize(const std::string& text);

    // Привести строку к lowercase (используется при поиске)
    static std::string toLower(const std::string& word);
};

} // namespace idx
