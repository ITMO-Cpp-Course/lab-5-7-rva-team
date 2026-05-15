#include "index/DocumentBuilder.hpp"
#include "index/IndexStore.hpp"
#include "index/InvertedIndex.hpp"
#include "index/UpdateTransaction.hpp"
#include <algorithm>
#include <catch2/catch_all.hpp>

using namespace idx;

// ── DocumentBuilder ──────────────────────────────────────────────────────────

TEST_CASE("DocumentBuilder creates a document with correct fields", "[builder]")
{
    Document doc = DocumentBuilder{}.setName("notes").setText("Hello world").build(42);

    REQUIRE(doc.getIndex() == 42);
    REQUIRE(doc.getName() == "notes");
    REQUIRE(doc.getText() == "Hello world");
}

// ── InvertedIndex: добавление и поиск ────────────────────────────────────────

TEST_CASE("search finds documents that contain the word", "[inverted_index]")
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

TEST_CASE("search returns empty when word is not in index", "[inverted_index]")
{
    InvertedIndex inv;
    inv.add(DocumentBuilder{}.setName("a").setText("hello world").build(1));

    REQUIRE(inv.search("missing").empty());
}

TEST_CASE("search is case-insensitive", "[inverted_index]")
{
    InvertedIndex inv;
    inv.add(DocumentBuilder{}.setName("a").setText("Hello World").build(1));

    REQUIRE(inv.search("hello").size() == 1);
    REQUIRE(inv.search("HELLO").size() == 1);
    REQUIRE(inv.search("Hello").size() == 1);
}

// ── InvertedIndex: подсчёт вхождений ────────────────────────────────────────

TEST_CASE("wordCount returns correct number of occurrences", "[inverted_index]")
{
    InvertedIndex inv;
    inv.add(DocumentBuilder{}.setName("a").setText("the cat sat on the mat the cat").build(1));

    REQUIRE(inv.wordCount("the", 1) == 3);
    REQUIRE(inv.wordCount("cat", 1) == 2);
    REQUIRE(inv.wordCount("sat", 1) == 1);
}

TEST_CASE("wordCount returns 0 for absent word", "[inverted_index]")
{
    InvertedIndex inv;
    inv.add(DocumentBuilder{}.setName("a").setText("hello world").build(1));

    REQUIRE(inv.wordCount("dog", 1) == 0);
}

TEST_CASE("wordCount returns 0 for absent document", "[inverted_index]")
{
    InvertedIndex inv;
    inv.add(DocumentBuilder{}.setName("a").setText("hello").build(1));

    REQUIRE(inv.wordCount("hello", 999) == 0);
}

// ── InvertedIndex: удаление ──────────────────────────────────────────────────

TEST_CASE("remove deletes document from search results", "[inverted_index]")
{
    InvertedIndex inv;
    inv.add(DocumentBuilder{}.setName("a").setText("cat sat").build(1));
    inv.add(DocumentBuilder{}.setName("b").setText("cat ran").build(2));

    inv.remove(1);

    auto results = inv.search("cat");
    REQUIRE(results.size() == 1);
    REQUIRE(results[0] == 2);
}

TEST_CASE("remove non-existent document does not crash", "[inverted_index]")
{
    InvertedIndex inv;
    REQUIRE_NOTHROW(inv.remove(999));
}

TEST_CASE("after removing all documents search returns empty", "[inverted_index]")
{
    InvertedIndex inv;
    inv.add(DocumentBuilder{}.setName("a").setText("only word").build(1));
    inv.remove(1);

    REQUIRE(inv.search("only").empty());
}

// ── IndexStore: базовые операции ─────────────────────────────────────────────

TEST_CASE("IndexStore: add and search document", "[index_store]")
{
    IndexStore store;
    auto result = store.add(DocumentBuilder{}.setName("a").setText("cat sat").build(1));

    REQUIRE(result.has_value());
    REQUIRE(store.search("cat")->size() == 1);
}

TEST_CASE("IndexStore: add duplicate returns DocumentAlreadyExists", "[index_store]")
{
    IndexStore store;
    store.add(DocumentBuilder{}.setName("a").setText("cat").build(1));

    auto result = store.add(DocumentBuilder{}.setName("b").setText("dog").build(1));

    REQUIRE(!result.has_value());
    REQUIRE(result.error() == IndexError::DocumentAlreadyExists);
}

TEST_CASE("IndexStore: remove existing document", "[index_store]")
{
    IndexStore store;
    store.add(DocumentBuilder{}.setName("a").setText("cat sat").build(1));

    auto result = store.remove(1);

    REQUIRE(result.has_value());
    REQUIRE(store.search("cat")->empty());
}

TEST_CASE("IndexStore: remove non-existent returns DocumentNotFound", "[index_store]")
{
    IndexStore store;

    auto result = store.remove(999);

    REQUIRE(!result.has_value());
    REQUIRE(result.error() == IndexError::DocumentNotFound);
}

// ── UpdateTransaction: commit ─────────────────────────────────────────────────

TEST_CASE("UpdateTransaction: commit applies all staged changes", "[transaction]")
{
    IndexStore store;
    store.add(DocumentBuilder{}.setName("a").setText("cat sat").build(1));

    store.beginTransaction();
    {
        UpdateTransaction tx(store);
        tx.add(DocumentBuilder{}.setName("b").setText("dog ran").build(2));
        tx.remove(1);
        tx.commit();
    }

    REQUIRE(store.search("dog")->size() == 1);
    REQUIRE(store.search("cat")->empty());
}

// ── UpdateTransaction: rollback ───────────────────────────────────────────────

TEST_CASE("UpdateTransaction: rollback on scope exit leaves state unchanged", "[transaction]")
{
    IndexStore store;
    store.add(DocumentBuilder{}.setName("a").setText("cat sat").build(1));

    store.beginTransaction();
    {
        UpdateTransaction tx(store);
        tx.add(DocumentBuilder{}.setName("b").setText("dog ran").build(2));
        tx.remove(1);
        // нет commit() — деструктор вызовет rollback()
    }

    REQUIRE(store.search("cat")->size() == 1);
    REQUIRE(store.search("dog")->empty());
}

// ── IndexStore: блокировка во время транзакции ────────────────────────────────

TEST_CASE("IndexStore: direct add blocked during active transaction", "[transaction]")
{
    IndexStore store;
    store.beginTransaction();

    auto result = store.add(DocumentBuilder{}.setName("a").setText("cat").build(1));

    REQUIRE(!result.has_value());
    REQUIRE(result.error() == IndexError::TransactionAlreadyActive);
}

TEST_CASE("IndexStore: beginTransaction returns error if already active", "[transaction]")
{
    IndexStore store;
    store.beginTransaction();

    auto result = store.beginTransaction();

    REQUIRE(!result.has_value());
    REQUIRE(result.error() == IndexError::TransactionAlreadyActive);
}

TEST_CASE("IndexStore: direct operations available again after transaction ends", "[transaction]")
{
    IndexStore store;

    store.beginTransaction();
    {
        UpdateTransaction tx(store);
        tx.commit();
    }

    auto result = store.add(DocumentBuilder{}.setName("a").setText("cat").build(1));
    REQUIRE(result.has_value());
}
