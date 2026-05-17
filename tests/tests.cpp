#include "index/DocumentBuilder.hpp"
#include "index/IndexStore.hpp"
#include "index/InvertedIndex.hpp"
#include "index/UpdateTransaction.hpp"
#include <algorithm>
#include <catch2/catch_all.hpp>

using namespace idx;

// ── DocumentBuilder ───────────────────────────────────────────────────────────

TEST_CASE("DocumentBuilder creates a document with correct fields", "[builder]")
{
    Document doc = DocumentBuilder{}.setName("notes").setText("Hello world").build(42);

    REQUIRE(doc.getIndex() == 42);
    REQUIRE(doc.getName() == "notes");
    REQUIRE(doc.getText() == "Hello world");
}

// ── InvertedIndex: добавление и поиск ────────────────────────────────────────

TEST_CASE("InvertedIndex: search finds documents containing the word", "[inverted_index]")
{
    InvertedIndex inv;
    inv.add(DocumentBuilder{}.setName("a").setText("cat sat on mat").build(1));
    inv.add(DocumentBuilder{}.setName("b").setText("cat in the hat").build(2));
    inv.add(DocumentBuilder{}.setName("c").setText("dog ran away").build(3));

    auto results = inv.search("cat");

    REQUIRE(results.size() == 2);
    REQUIRE(std::find(results.begin(), results.end(), 1) != results.end());
    REQUIRE(std::find(results.begin(), results.end(), 2) != results.end());
}

TEST_CASE("InvertedIndex: search returns empty when word is absent", "[inverted_index]")
{
    InvertedIndex inv;
    inv.add(DocumentBuilder{}.setName("a").setText("hello world").build(1));

    REQUIRE(inv.search("missing").empty());
}

TEST_CASE("InvertedIndex: search is case-insensitive", "[inverted_index]")
{
    InvertedIndex inv;
    inv.add(DocumentBuilder{}.setName("a").setText("Hello World").build(1));

    REQUIRE(inv.search("hello").size() == 1);
    REQUIRE(inv.search("HELLO").size() == 1);
    REQUIRE(inv.search("Hello").size() == 1);
}

// ── InvertedIndex: подсчёт вхождений ─────────────────────────────────────────

TEST_CASE("InvertedIndex: wordCount returns correct number of occurrences", "[inverted_index]")
{
    InvertedIndex inv;
    inv.add(DocumentBuilder{}.setName("a").setText("the cat sat on the mat the cat").build(1));

    REQUIRE(inv.wordCount("the", 1) == 3);
    REQUIRE(inv.wordCount("cat", 1) == 2);
    REQUIRE(inv.wordCount("sat", 1) == 1);
}

TEST_CASE("InvertedIndex: wordCount returns 0 for absent word", "[inverted_index]")
{
    InvertedIndex inv;
    inv.add(DocumentBuilder{}.setName("a").setText("hello world").build(1));

    REQUIRE(inv.wordCount("dog", 1) == 0);
}

TEST_CASE("InvertedIndex: wordCount returns 0 for absent document", "[inverted_index]")
{
    InvertedIndex inv;
    inv.add(DocumentBuilder{}.setName("a").setText("hello").build(1));

    REQUIRE(inv.wordCount("hello", 999) == 0);
}

// ── InvertedIndex: удаление ───────────────────────────────────────────────────

TEST_CASE("InvertedIndex: remove deletes document from search results", "[inverted_index]")
{
    InvertedIndex inv;
    inv.add(DocumentBuilder{}.setName("a").setText("cat sat").build(1));
    inv.add(DocumentBuilder{}.setName("b").setText("cat ran").build(2));

    inv.remove(1);

    auto results = inv.search("cat");
    REQUIRE(results.size() == 1);
    REQUIRE(results[0] == 2);
}

TEST_CASE("InvertedIndex: remove non-existent document does not crash", "[inverted_index]")
{
    InvertedIndex inv;
    REQUIRE_NOTHROW(inv.remove(999));
}

TEST_CASE("InvertedIndex: search returns empty after all documents removed", "[inverted_index]")
{
    InvertedIndex inv;
    inv.add(DocumentBuilder{}.setName("a").setText("only word").build(1));
    inv.remove(1);

    REQUIRE(inv.search("only").empty());
}

// ── IndexStore: search (доступен без транзакции) ──────────────────────────────

TEST_CASE("IndexStore: search works without a transaction", "[index_store]")
{
    IndexStore store;

    auto result = store.search("cat");

    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error() == IndexError::WordNotFound);
}

// ── IndexStore: add и remove требуют транзакцию ───────────────────────────────

TEST_CASE("IndexStore: add without transaction returns TransactionNotActive", "[index_store]")
{
    IndexStore store;

    auto result = store.add(DocumentBuilder{}.setName("a").setText("cat").build(1));

    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error() == IndexError::TransactionNotActive);
}

TEST_CASE("IndexStore: remove without transaction returns TransactionNotActive", "[index_store]")
{
    IndexStore store;

    auto result = store.remove(1);

    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error() == IndexError::TransactionNotActive);
}

// ── IndexStore: beginTransaction ──────────────────────────────────────────────

TEST_CASE("IndexStore: beginTransaction succeeds when no transaction is active", "[index_store]")
{
    IndexStore store;

    REQUIRE(store.beginTransaction().has_value());
}

TEST_CASE("IndexStore: beginTransaction returns error if already active", "[index_store]")
{
    IndexStore store;
    auto tmp = store.beginTransaction();

    auto result = store.beginTransaction();

    REQUIRE(tmp.has_value());
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error() == IndexError::TransactionAlreadyActive);
}

// ── IndexStore: commitTransaction ─────────────────────────────────────────────

TEST_CASE("IndexStore: commitTransaction without beginTransaction returns error", "[index_store]")
{
    IndexStore store;

    auto result = store.commitTransaction();

    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error() == IndexError::TransactionNotActive);
}

TEST_CASE("IndexStore: commit applies added documents — search finds them", "[index_store]")
{
    IndexStore store;

    store.beginTransaction();
    store.add(DocumentBuilder{}.setName("a").setText("cat sat").build(1));
    store.add(DocumentBuilder{}.setName("b").setText("dog ran").build(2));
    store.commitTransaction();

    REQUIRE(store.search("cat").value().size() == 1);
    REQUIRE(store.search("dog").value().size() == 1);
}

TEST_CASE("IndexStore: commit applies removed documents — search no longer finds them", "[index_store]")
{
    IndexStore store;

    store.beginTransaction();
    store.add(DocumentBuilder{}.setName("a").setText("cat sat").build(1));
    store.commitTransaction();

    store.beginTransaction();
    store.remove(1);
    store.commitTransaction();

    auto result = store.search("cat");

    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error() == IndexError::WordNotFound);
}

TEST_CASE("IndexStore: uncommitted changes are not visible via search", "[index_store]")
{
    IndexStore store;

    store.beginTransaction();
    store.add(DocumentBuilder{}.setName("a").setText("cat sat").build(1));
    // нет commitTransaction — изменения ещё не применены

    auto result = store.search("cat");

    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error() == IndexError::WordNotFound);
}

TEST_CASE("IndexStore: after commit a new transaction can be started", "[index_store]")
{
    IndexStore store;

    store.beginTransaction();
    store.commitTransaction();

    auto result = store.beginTransaction();
    REQUIRE(result.has_value());
}

// ── UpdateTransaction: ошибки при дублировании ───────────────────────────────

TEST_CASE("UpdateTransaction: add same doc twice returns DocumentAlreadyInAdditionList", "[transaction]")
{
    IndexStore store;
    store.beginTransaction();

    store.add(DocumentBuilder{}.setName("a").setText("cat").build(1));
    auto result = store.add(DocumentBuilder{}.setName("b").setText("dog").build(1));

    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error() == IndexError::DocumentAlreadyInAdditionList);
}

TEST_CASE("UpdateTransaction: add doc that already exists returns DocumentAlreadyExists", "[transaction]")
{
    IndexStore store;

    store.beginTransaction();
    store.add(DocumentBuilder{}.setName("a").setText("cat").build(1));
    store.commitTransaction();

    store.beginTransaction();
    auto result = store.add(DocumentBuilder{}.setName("b").setText("dog").build(1));

    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error() == IndexError::DocumentAlreadyExists);
}

TEST_CASE("UpdateTransaction: remove same doc twice returns DocumentAlreadyInRemovalList", "[transaction]")
{
    IndexStore store;

    store.beginTransaction();
    store.add(DocumentBuilder{}.setName("a").setText("cat").build(1));
    store.commitTransaction();

    store.beginTransaction();
    store.remove(1);
    auto result = store.remove(1);

    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error() == IndexError::DocumentAlreadyInRemovalList);
}

TEST_CASE("UpdateTransaction: remove non-existent doc returns DocumentNotFound", "[transaction]")
{
    IndexStore store;
    store.beginTransaction();

    auto result = store.remove(999);

    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error() == IndexError::DocumentNotFound);
}
