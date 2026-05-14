#pragma once

#include "index/Document.hpp"
#include "index/IndexStore.hpp"
#include "index/UpdateTransaction.hpp"
#include <vector>

namespace idx
{

class UpdateTransaction
{
  public:
    UpdateTransaction(IndexStore& store_);
    ~UpdateTransaction();

    // запрет копирования и перемещения (возможно придется вернуть перемещение)
    UpdateTransaction(const UpdateTransaction&) = delete;
    UpdateTransaction& operator=(const UpdateTransaction&) = delete;
    UpdateTransaction(UpdateTransaction&& other) = delete;
    UpdateTransaction& operator=(UpdateTransaction&& other) = delete;

    void add(Document doc);
    void remove(size_t id);
    void commit();

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