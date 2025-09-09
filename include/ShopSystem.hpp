// 这是商店系统的头文件
// 作者：大一学生
// 功能：定义游戏中的商店系统，包括物品池管理和商店功能

#pragma once
#include "Item.hpp"        // 物品类
#include "NPC.hpp"         // NPC类
#include <vector>          // 向量容器
#include <random>          // 随机数生成
#include <unordered_map>   // 哈希映射

namespace hx {

// 商店物品池中的物品定义
// 功能：定义商店中可以出售的物品
struct ShopItemPool {
    std::string id;
    std::string name;
    std::string description;
    int base_price;
    EquipmentQuality quality;
    bool is_guaranteed; // 是否必定出现（如生命药水）
    int max_purchases; // 最大购买次数（如复活符限购2次）
    int current_purchases; // 当前购买次数
};

class ShopSystem {
public:
    ShopSystem();
    
    // 初始化商店物品池
    void initializeItemPool();
    
    // 检查是否需要刷新商店（每5个回合）
    bool shouldRefresh(int turn_counter) const;
    
    // 刷新商店物品
    void refreshShop(std::vector<Item>& shop_items, int turn_counter);
    
    // 获取复活符已购买次数
    int getRevivalScrollPurchases() const { return revival_scroll_purchases_; }
    
    // 增加复活符购买次数
    void incrementRevivalScrollPurchases() { revival_scroll_purchases_++; }
    
    // 检查复活符是否可购买
    bool canPurchaseRevivalScroll() const { return revival_scroll_purchases_ < 2; }
    
    // 获取装备价格（根据品质）
    int getEquipmentPrice(EquipmentQuality quality) const;
    
    // 生成随机装备
    Item generateRandomEquipment() const;
    
    // 获取装备池中的所有装备
    const std::vector<ShopItemPool>& getEquipmentPool() const { return equipment_pool_; }
    
    // 根据ID创建Item（用于商店购买）
    Item createItemFromId(const std::string& item_id) const;

private:
    std::vector<ShopItemPool> equipment_pool_; // 装备池
    std::vector<ShopItemPool> consumable_pool_; // 消耗品池
    int revival_scroll_purchases_; // 复活符购买次数
    mutable std::random_device rd_;
    mutable std::mt19937 gen_;
    
    // 初始化装备池
    void initializeEquipmentPool();
    
    // 初始化消耗品池
    void initializeConsumablePool();
    
    // 从池中随机选择物品
    ShopItemPool selectRandomFromPool(const std::vector<ShopItemPool>& pool) const;
    
    // 创建Item
    Item createItem(const ShopItemPool& pool_item) const;
};

} // namespace hx
