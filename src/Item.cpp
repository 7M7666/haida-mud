#include "Item.hpp"
#include <sstream>
#include <algorithm>

namespace hx {

// 静态方法实现
Item Item::createConsumable(const std::string& id, const std::string& name, 
                           const std::string& description, int heal, int price) {
    Item item;
    item.id = id;
    item.name = name;
    item.description = description;
    item.type = ItemType::CONSUMABLE;
    item.heal_amount = heal;
    item.price = price;
    item.use_message = "使用了 " + name + "，恢复了 " + std::to_string(heal) + " 点生命值。";
    return item;
}

Item Item::createEquipment(const std::string& id, const std::string& name,
                          const std::string& description, EquipmentType equip_type,
                          EquipmentSlot equip_slot, int atk, int def, int spd, int hp, int price) {
    Item item;
    item.id = id;
    item.name = name;
    item.description = description;
    item.type = ItemType::EQUIPMENT;
    item.equip_type = equip_type;
    item.equip_slot = equip_slot;
    item.atk_delta = atk;
    item.def_delta = def;
    item.spd_delta = spd;
    item.hp_delta = hp;
    item.price = price;
    // 默认品质与无套装
    item.quality = EquipmentQuality::UNDERGRAD;
    item.set_name = "";
    return item;
}

Item Item::createQuestItem(const std::string& id, const std::string& name,
                          const std::string& description) {
    Item item;
    item.id = id;
    item.name = name;
    item.description = description;
    item.type = ItemType::QUEST;
    item.is_quest_item = true;
    item.is_tradeable = false;
    return item;
}

// 装备系统实现
Equipment::Equipment() = default;

bool Equipment::equipItem(const Item& item) {
    if (item.type != ItemType::EQUIPMENT) {
        return false;
    }
    
    // 对于饰品类型，可以装备到任意一个饰品槽位
    if (item.equip_type == EquipmentType::ACCESSORY) {
        // 优先装备到ACCESSORY1槽位，如果被占用则装备到ACCESSORY2槽位
        if (!isSlotOccupied(EquipmentSlot::ACCESSORY1)) {
            equipped_items_[EquipmentSlot::ACCESSORY1] = item;
            return true;
        } else if (!isSlotOccupied(EquipmentSlot::ACCESSORY2)) {
            equipped_items_[EquipmentSlot::ACCESSORY2] = item;
            return true;
        } else {
            // 两个饰品槽位都被占用，先卸下ACCESSORY1槽位的装备
            unequipItem(EquipmentSlot::ACCESSORY1);
            equipped_items_[EquipmentSlot::ACCESSORY1] = item;
            return true;
        }
    } else {
        // 对于非饰品装备，如果槽位被占用则先卸下
        if (isSlotOccupied(item.equip_slot)) {
            unequipItem(item.equip_slot);
        }
        equipped_items_[item.equip_slot] = item;
        return true;
    }
}

bool Equipment::unequipItem(EquipmentSlot slot) {
    auto it = equipped_items_.find(slot);
    if (it != equipped_items_.end()) {
        equipped_items_.erase(it);
        return true;
    }
    return false;
}

const Item* Equipment::getEquippedItem(EquipmentSlot slot) const {
    auto it = equipped_items_.find(slot);
    return it != equipped_items_.end() ? &it->second : nullptr;
}

int Equipment::getTotalATK() const {
    int total = 0;
    for (const auto& [slot, item] : equipped_items_) {
        total += item.atk_delta;
    }
    return total;
}

int Equipment::getTotalDEF() const {
    int total = 0;
    for (const auto& [slot, item] : equipped_items_) {
        total += item.def_delta;
    }
    return total;
}

int Equipment::getTotalSPD() const {
    int total = 0;
    for (const auto& [slot, item] : equipped_items_) {
        total += item.spd_delta;
    }
    return total;
}

int Equipment::getTotalHP() const {
    int total = 0;
    for (const auto& [slot, item] : equipped_items_) {
        total += item.hp_delta;
    }
    return total;
}

bool Equipment::isSlotOccupied(EquipmentSlot slot) const {
    return equipped_items_.find(slot) != equipped_items_.end();
}

std::vector<EquipmentSlot> Equipment::getOccupiedSlots() const {
    std::vector<EquipmentSlot> occupied;
    for (const auto& [slot, item] : equipped_items_) {
        occupied.push_back(slot);
    }
    return occupied;
}

std::string Equipment::getEquipmentInfo() const {
    std::ostringstream oss;
    oss << "装备信息：\n";
    
    std::string slot_names[] = {"武器", "护甲", "饰品1", "饰品2"};
    EquipmentSlot slots[] = {EquipmentSlot::WEAPON, EquipmentSlot::ARMOR, 
                            EquipmentSlot::ACCESSORY1, EquipmentSlot::ACCESSORY2};
    
    for (int i = 0; i < 4; ++i) {
        oss << slot_names[i] << ": ";
        const Item* item = getEquippedItem(slots[i]);
        if (item) {
            oss << getColoredItemName(*item);
            if (item->atk_delta > 0) oss << " ATK+" << item->atk_delta;
            if (item->def_delta > 0) oss << " DEF+" << item->def_delta;
            if (item->spd_delta > 0) oss << " SPD+" << item->spd_delta;
            if (item->hp_delta > 0) oss << " HP+" << item->hp_delta;
            if (!item->effect_description.empty()) {
                oss << " [" << item->effect_description << "]";
            }
        } else {
            oss << "空";
        }
        oss << "\n";
    }
    
    oss << "总加成: ATK+" << getTotalATK() 
        << " DEF+" << getTotalDEF() 
        << " SPD+" << getTotalSPD() 
        << " HP+" << getTotalHP();
    
    return oss.str();
}

std::vector<const Item*> Equipment::getItemsWithEffect(const std::string& effect_type) const {
    std::vector<const Item*> items;
    for (const auto& [slot, item] : equipped_items_) {
        if (item.effect_type == effect_type) {
            items.push_back(&item);
        }
    }
    return items;
}

float Equipment::getEffectValue(const std::string& effect_type, const std::string& target) const {
    float total_value = 1.0f;
    for (const auto& [slot, item] : equipped_items_) {
        if (item.effect_type == effect_type) {
            if (target.empty() || item.effect_target == target || item.effect_target == "all") {
                total_value *= item.effect_value;
            }
        }
    }
    return total_value;
}

bool Equipment::hasEffect(const std::string& effect_type, const std::string& target) const {
    for (const auto& [slot, item] : equipped_items_) {
        if (item.effect_type == effect_type) {
            if (target.empty() || item.effect_target == target || item.effect_target == "all") {
                return true;
            }
        }
    }
    return false;
}

int Equipment::countInSet(const std::string& set) const {
    if (set.empty()) return 0;
    int c = 0;
    for (const auto& [slot, item] : equipped_items_) {
        if (item.set_name == set) c++;
    }
    return c;
}

// 名称着色：本科=绿(\x1b[32m)，硕士=天蓝(\x1b[36m)，博士=红(\x1b[31m)，饰品=金(\x1b[33m)。仅为装备着色，其它类型原样返回。
std::string getColoredItemName(const Item& item) {
    if (item.type != ItemType::EQUIPMENT) return item.name;
#ifdef _WIN32
    // Windows 控制台在某些环境不支持ANSI，仍然输出ANSI以便IDE/终端支持；如需适配可后续加专用API
#endif
    const char* color = "\x1b[37m"; // 默认白
    switch (item.quality) {
        case EquipmentQuality::UNDERGRAD: color = "\x1b[32m"; break; // 绿
        case EquipmentQuality::MASTER:    color = "\x1b[36m"; break; // 天蓝
        case EquipmentQuality::DOCTOR:    color = "\x1b[31m"; break; // 红
        default: break;
    }
    
    // 饰品特殊处理：所有饰品都显示为金色
    if (item.equip_type == EquipmentType::ACCESSORY) {
        color = "\x1b[33m"; // 金色
    }
    
    return std::string(color) + item.name + "\x1b[0m";
}

std::string getEquipmentStats(const Item& item) {
    if (item.type != ItemType::EQUIPMENT) {
        return "";
    }
    
    std::stringstream ss;
    bool hasStats = false;
    
    // 基础属性加成
    if (item.atk_delta != 0) {
        ss << "ATK " << (item.atk_delta > 0 ? "+" : "") << item.atk_delta << " ";
        hasStats = true;
    }
    if (item.def_delta != 0) {
        ss << "DEF " << (item.def_delta > 0 ? "+" : "") << item.def_delta << " ";
        hasStats = true;
    }
    if (item.spd_delta != 0) {
        ss << "SPD " << (item.spd_delta > 0 ? "+" : "") << item.spd_delta << " ";
        hasStats = true;
    }
    if (item.hp_delta != 0) {
        ss << "HP " << (item.hp_delta > 0 ? "+" : "") << item.hp_delta << " ";
        hasStats = true;
    }
    // 特殊效果
    if (!item.effect_description.empty()) {
        if (hasStats) ss << "| ";
        ss << item.effect_description;
        hasStats = true;
    }
    
    return hasStats ? ss.str() : "无属性加成";
}

std::vector<Item> Equipment::getEquippedItems() const {
    std::vector<Item> items;
    for (const auto& [slot, item] : equipped_items_) {
        items.push_back(item);
    }
    return items;
}

// 格式化装备详细信息，用于装备后显示和背包查看
std::string formatEquipmentDetails(const Item& item) {
    if (item.type != ItemType::EQUIPMENT) {
        return item.description;
    }
    
    std::ostringstream oss;
    oss << item.description << "\n";
    
    // 显示装备效果加成
    bool hasEffects = false;
    if (item.atk_delta > 0) {
        oss << "攻击力: +" << item.atk_delta;
        hasEffects = true;
    }
    if (item.def_delta > 0) {
        if (hasEffects) oss << " ";
        oss << "防御力: +" << item.def_delta;
        hasEffects = true;
    }
    if (item.spd_delta > 0) {
        if (hasEffects) oss << " ";
        oss << "速度: +" << item.spd_delta;
        hasEffects = true;
    }
    if (item.hp_delta > 0) {
        if (hasEffects) oss << " ";
        oss << "生命值: +" << item.hp_delta;
        hasEffects = true;
    }
    
    if (hasEffects) {
        oss << "\n";
    }
    
    // 显示特殊效果
    if (!item.effect_description.empty()) {
        oss << "特殊效果: " << item.effect_description << "\n";
    }
    
    // 显示套装信息
    if (!item.set_name.empty()) {
        oss << "套装: " << item.set_name << "\n";
    }
    
    return oss.str();
}

void Equipment::setEquippedItems(const std::vector<Item>& items) {
    equipped_items_.clear();
    for (const auto& item : items) {
        // 检查是否是有效的装备槽位
        if (item.equip_slot == EquipmentSlot::WEAPON || 
            item.equip_slot == EquipmentSlot::ARMOR || 
            item.equip_slot == EquipmentSlot::ACCESSORY1 || 
            item.equip_slot == EquipmentSlot::ACCESSORY2) {
            equipped_items_[item.equip_slot] = item;
        }
    }
}

} // namespace hx
