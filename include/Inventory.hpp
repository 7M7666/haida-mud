// 这是背包系统的头文件
// 作者：大一学生
// 功能：定义玩家的背包系统，管理物品的存储和数量

#pragma once
#include <unordered_map>  // 哈希映射
#include <string>         // 字符串
#include <vector>         // 向量容器
#include "Item.hpp"       // 物品类

namespace hx {
// 背包类
// 功能：管理玩家的物品存储，包括添加、删除、查询等功能
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
