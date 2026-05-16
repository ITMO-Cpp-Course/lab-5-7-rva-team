#pragma once
#include "index/Document.hpp"
#include "index/Result.hpp"
#include <vector>

namespace idx
{

class IndexStore; // forward declaration — полный include не нужен, используем только указатель

class UpdateTransaction
{
  public:
    UpdateTransaction(IndexStore& store_);
    ~UpdateTransaction();

    // prohibited copying and moving (might wanna put moving back though, idk)
    UpdateTransaction(const UpdateTransaction&) = delete;
    UpdateTransaction& operator=(const UpdateTransaction&) = delete;
    UpdateTransaction(UpdateTransaction&& other) = delete;
    UpdateTransaction& operator=(UpdateTransaction&& other) = delete;

    Result<void> add(Document doc);
    Result<void> remove(size_t id);
    Result<void> commit();

  private:
    IndexStore* store;
    std::vector<Document> to_add;
    std::vector<size_t> to_remove;

    bool committed = false;

    bool isInToAdd(size_t id) const;
    bool isInToRemove(size_t id) const;
    void rollback();
};

} // namespace idx
