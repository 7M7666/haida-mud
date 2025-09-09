// 这是商店系统的实现文件
// 作者：大一学生
// 功能：实现游戏中的商店系统，包括物品池管理和商店功能

#include "ShopSystem.hpp"    // 商店系统头文件
#include "ItemDefinitions.hpp"  // 物品定义头文件
#include <iostream>            // 输入输出流
#include <algorithm>           // 算法库
#include <set>                 // 集合容器

namespace hx {

// 商店系统的构造函数
// 功能：初始化商店系统，设置物品池和随机数生成器
ShopSystem::ShopSystem() 
    : revival_scroll_purchases_(0), gen_(rd_()) {  // 初始化复活符购买次数和随机数生成器
    initializeItemPool();  // 初始化物品池
}

void ShopSystem::initializeItemPool() {
    initializeEquipmentPool();
    initializeConsumablePool();
}

void ShopSystem::initializeEquipmentPool() {
    // 本科级装备
    equipment_pool_.push_back({"wu_jing_ball", "吴京篮球", "形意合一，攻守兼备。", 200, EquipmentQuality::UNDERGRAD, false, 0, 0});
    equipment_pool_.push_back({"bed_quilt", "床上的被子", "最是温柔被窝中。", 200, EquipmentQuality::UNDERGRAD, false, 0, 0});
    
    // 饰品类装备（统一300金币以上，金色显示）
    equipment_pool_.push_back({"steel_spoon_amulet", "钢勺护符", "食堂选择的奖励，能提供额外保护。", 300, EquipmentQuality::UNDERGRAD, false, 0, 0});
    equipment_pool_.push_back({"lab_notebook", "实验记录本", "抗挫试炼的奖励，记录着实验心得。", 300, EquipmentQuality::UNDERGRAD, false, 0, 0});
    equipment_pool_.push_back({"safety_goggles", "护目镜", "实验楼的隐藏奖励，能保护眼睛。", 300, EquipmentQuality::UNDERGRAD, false, 0, 0});
    equipment_pool_.push_back({"speech_words_amulet", "演讲之词", "表达试炼的奖励，能提升表达能力。", 300, EquipmentQuality::UNDERGRAD, false, 0, 0});
    equipment_pool_.push_back({"hnu_badge_amulet", "海纳百川校徽", "海纳百川，有容乃大。", 300, EquipmentQuality::UNDERGRAD, false, 0, 0});
    
    // 硕士级装备
    equipment_pool_.push_back({"second_hand_guitar", "二手吉他", "一曲肝肠断，输出即情绪。", 400, EquipmentQuality::MASTER, false, 0, 0});
    equipment_pool_.push_back({"bachelor_robe", "学士袍", "雍容华贵，学养自显。", 400, EquipmentQuality::MASTER, false, 0, 0});
    equipment_pool_.push_back({"psionic_armor", "灵能护甲", "灵能护体，固若金汤。", 400, EquipmentQuality::MASTER, false, 0, 0});
    
    // 博士级装备
    equipment_pool_.push_back({"phd_thesis", "博士大论文", "厚重如砖，砸谁谁懵。", 600, EquipmentQuality::DOCTOR, false, 0, 0});
    equipment_pool_.push_back({"lab_coat", "实验服", "严谨至上，安全第一。", 600, EquipmentQuality::DOCTOR, false, 0, 0});
    
    // 高级饰品类装备（600金币以上，金色显示）
    equipment_pool_.push_back({"ecard_amulet", "海大e卡通", "一卡在手，畅行校园。", 600, EquipmentQuality::UNDERGRAD, false, 0, 0});
    equipment_pool_.push_back({"seat_all_lib_amulet", "全图书馆占座物品", "今日座位，舍我其谁。", 600, EquipmentQuality::UNDERGRAD, false, 0, 0});
}

void ShopSystem::initializeConsumablePool() {
    // 消耗品池（生命药水必定出现，复活符限购）
    consumable_pool_.push_back({"health_potion", "生命药水", "恢复30点生命值", 30, EquipmentQuality::UNDERGRAD, true, 0, 0});
    consumable_pool_.push_back({"revival_scroll", "复活符", "死亡时无惩罚复活", 300, EquipmentQuality::UNDERGRAD, true, 2, 0});
    consumable_pool_.push_back({"caffeine_elixir", "咖啡因灵液", "获得专注：下一次攻击必中", 150, EquipmentQuality::UNDERGRAD, false, 0, 0});
}

bool ShopSystem::shouldRefresh(int turn_counter) const {
    return turn_counter % 5 == 0;
}

void ShopSystem::refreshShop(std::vector<Item>& shop_items, int turn_counter) {
    shop_items.clear();
    
    // 1. 生命药水（固定，30金币）
    Item health_potion = Item::createConsumable("health_potion", "生命药水", "恢复30点生命值", 30, 30);
    shop_items.push_back(health_potion);
    
    // 2. 复活符（固定，300金币，全局最多购买2次；买满后换成装备）
    if (canPurchaseRevivalScroll()) {
        Item revival_scroll = Item::createConsumable("revival_scroll", "复活符", "死亡时无惩罚复活", 0, 300);
        revival_scroll.is_quest_item = true;
        shop_items.push_back(revival_scroll);
    }
    
    // 3-4. 随机装备（确保不重复）
    std::set<std::string> used_items;
    
    // 第3个物品：随机装备
    Item random_equip1 = generateRandomEquipment();
    used_items.insert(random_equip1.id);
    shop_items.push_back(random_equip1);
    
    // 第4个物品：随机装备（不能与第3个重复）
    Item random_equip2;
    int attempts = 0;
    do {
        random_equip2 = generateRandomEquipment();
        attempts++;
    } while (used_items.find(random_equip2.id) != used_items.end() && attempts < 10);
    
    // 如果尝试10次后仍有重复，使用默认装备
    if (attempts >= 10) {
        random_equip2 = Item::createEquipment("student_uniform", "普通学子服", "基础装备", 
            EquipmentType::ARMOR, EquipmentSlot::ARMOR, 0, 2, 0, 10, 200);
        random_equip2.quality = EquipmentQuality::UNDERGRAD;
    }
    shop_items.push_back(random_equip2);
    
    // 如果复活符已买满，添加第5个随机装备
    if (!canPurchaseRevivalScroll()) {
        Item random_equip3;
        attempts = 0;
        do {
            random_equip3 = generateRandomEquipment();
            attempts++;
        } while (used_items.find(random_equip3.id) != used_items.end() && attempts < 10);
        
        // 如果尝试10次后仍有重复，使用默认装备
        if (attempts >= 10) {
            random_equip3 = Item::createEquipment("bamboo_notes", "竹简笔记", "基础装备", 
                EquipmentType::ACCESSORY, EquipmentSlot::ACCESSORY1, 0, 0, 0, 0, 200);
            random_equip3.quality = EquipmentQuality::UNDERGRAD;
        }
        shop_items.push_back(random_equip3);
    }
    
    // 商店刷新提示已移至全局界面
}

int ShopSystem::getEquipmentPrice(EquipmentQuality quality) const {
    switch (quality) {
        case EquipmentQuality::UNDERGRAD:
            return 200;
        case EquipmentQuality::MASTER:
            return 400;
        case EquipmentQuality::DOCTOR:
            return 600;
        default:
            return 200;
    }
}

Item ShopSystem::generateRandomEquipment() const {
    // 从装备池中随机选择
    ShopItemPool selected = selectRandomFromPool(equipment_pool_);
    return createItem(selected);
}

ShopItemPool ShopSystem::selectRandomFromPool(const std::vector<ShopItemPool>& pool) const {
    if (pool.empty()) {
        // 返回默认物品
        return {"student_uniform", "普通学子服", "基础装备", 200, EquipmentQuality::UNDERGRAD, false, 0, 0};
    }
    
    std::uniform_int_distribution<> dis(0, pool.size() - 1);
    return pool[dis(gen_)];
}

Item ShopSystem::createItem(const ShopItemPool& pool_item) const {
    // 使用统一的物品定义系统
    return ItemDefinitions::createItemById(pool_item.id);
}

Item ShopSystem::createItemFromId(const std::string& item_id) const {
    // 使用统一的物品定义系统
    return ItemDefinitions::createItemById(item_id);
}

} // namespace hx
