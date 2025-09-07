#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include "Item.hpp"

namespace hx {
class Inventory {
public:
    void add(const Item& item, int qty = 1);
    bool remove(const std::string& id, int qty = 1);
    int quantity(const std::string& id) const;
    std::vector<Item> list() const;
    std::vector<Item> rawList() const { return list(); }
    std::vector<Item> asSimpleItems() const;
    void setFromSimple(const std::vector<Item>& items);
private:
    std::unordered_map<std::string, std::pair<Item,int>> data_;
};
} // namespace hx
