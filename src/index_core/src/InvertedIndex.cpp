#include "index/InvertedIndex.hpp"
#include <algorithm>
#include <cctype>

namespace idx
{

void InvertedIndex::add(Document doc)
{
    size_t id = doc.getIndex();
    auto words = tokenize(doc.getText());

    // Считаем сколько раз каждое слово встречается в этом документе
    for (const auto& word : words)
        index[word][id]++;

    // Перемещаем документ в хранилище — id и text уже прочитаны выше, поэтому move безопасен
    docs.emplace(id, std::move(doc));
}

void InvertedIndex::remove(size_t docId)
{
    if (docs.find(docId) == docs.end())
        return;

    // Проходим по всем словам и убираем упоминания удаляемого документа.
    // Если слово больше не встречается ни в одном документе — удаляем его из индекса тоже.
    for (auto it = index.begin(); it != index.end();)
    {
        it->second.erase(docId);
        if (it->second.empty())
            it = index.erase(it); // свинка пеппа
        else
            ++it;
    }

    docs.erase(docId);
}

bool InvertedIndex::has(size_t docId) const
{
    return docs.find(docId) != docs.end();
}

std::vector<size_t> InvertedIndex::search(const std::string& word) const
{
    auto it = index.find(toLower(word));
    if (it == index.end())
        return {};

    // Собираем только ID документов (без количества вхождений)
    std::vector<size_t> result;
    for (const auto& [docId, count] : it->second)
        result.push_back(docId);

    return result;
}

size_t InvertedIndex::wordCount(const std::string& word, size_t docId) const
{
    auto wordIt = index.find(toLower(word));
    if (wordIt == index.end())
        return 0;

    auto docIt = wordIt->second.find(docId);
    if (docIt == wordIt->second.end())
        return 0;

    return docIt->second;
}

std::vector<std::string> InvertedIndex::tokenize(const std::string& text)
{
    std::vector<std::string> words;
    std::string word;

    // Читаем символ за символом: буквы накапливаем в слово, всё остальное — разделитель.
    for (char ch : text)
    {
        auto c = static_cast<unsigned char>(ch);
        if (std::isalpha(c))
            word += static_cast<char>(std::tolower(c));
        else if (!word.empty())
        {
            words.push_back(word);
            word.clear();
        }
    }

    // Последнее слово — если текст заканчивается буквой, а не разделителем
    if (!word.empty())
        words.push_back(word);

    return words;
}

std::string InvertedIndex::toLower(const std::string& word)
{
    std::string result = word;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return result;
}

} // namespace idx
