#pragma once
#include <string>
#include <cstdint>
#include <vector>
#include <unordered_map>

namespace hx {

// 装备槽位类型
enum class EquipmentSlot {
    WEAPON,     // 武器
    ARMOR,      // 护甲
    ACCESSORY1, // 饰品1
    ACCESSORY2  // 饰品2
};

// 物品类型
enum class ItemType {
    CONSUMABLE, // 消耗品
    EQUIPMENT,  // 装备
    QUEST,      // 任务物品
    MATERIAL    // 材料
};

// 装备类型
enum class EquipmentType {
    NONE,
    WEAPON,     // 武器
    ARMOR,      // 护甲
    ACCESSORY   // 饰品
};

// 装备品质
enum class EquipmentQuality {
    UNDERGRAD, // 本科
    MASTER,    // 硕士
    DOCTOR     // 博士
};

struct Item {
    std::string id;
    std::string name;
    std::string description;
    ItemType type{ItemType::CONSUMABLE};
    EquipmentType equip_type{EquipmentType::NONE};
    EquipmentSlot equip_slot{EquipmentSlot::WEAPON};
    EquipmentQuality quality{EquipmentQuality::UNDERGRAD};
    std::string set_name; // 套装名称（用于套装加成）
    
    // 属性加成
    int hp_delta{0};
    int atk_delta{0};
    int def_delta{0};
    int spd_delta{0};
    
    // 物品属性
    int price{0};
    int count{1};
    int max_stack{99};
    
    // 特殊效果
    std::string effect_description;
    std::string effect_type; // 效果类型：damage_multiplier, auto_inspiration, etc.
    std::string effect_target; // 效果目标：experiment_monsters, all_enemies, etc.
    float effect_value{1.0f}; // 效果数值
    bool is_quest_item{false};
    bool is_tradeable{true};
    
    // 使用效果
    int heal_amount{0};
    int mp_restore{0};
    std::string use_message;
    
    // 装备要求
    int level_requirement{0};
    int favor_requirement{0};
    
    // 创建预设物品的静态方法
    static Item createConsumable(const std::string& id, const std::string& name, 
                                const std::string& description, int heal, int price);
    static Item createEquipment(const std::string& id, const std::string& name,
                               const std::string& description, EquipmentType equip_type,
                               EquipmentSlot equip_slot, int atk, int def, int spd, int hp, int price);
    static Item createQuestItem(const std::string& id, const std::string& name,
                               const std::string& description);
};

// 装备系统
class Equipment {
public:
    Equipment();
    
    // 装备管理
    bool equipItem(const Item& item);
    bool unequipItem(EquipmentSlot slot);
    const Item* getEquippedItem(EquipmentSlot slot) const;
    
    // 属性计算
    int getTotalATK() const;
    int getTotalDEF() const;
    int getTotalSPD() const;
    int getTotalHP() const;
    
    // 装备状态
    bool isSlotOccupied(EquipmentSlot slot) const;
    std::vector<EquipmentSlot> getOccupiedSlots() const;
    
    // 装备信息
    std::string getEquipmentInfo() const;
    
    // 特殊效果查询
    std::vector<const Item*> getItemsWithEffect(const std::string& effect_type) const;
    float getEffectValue(const std::string& effect_type, const std::string& target = "") const;
    bool hasEffect(const std::string& effect_type, const std::string& target = "") const;
    int countInSet(const std::string& set) const;
    
    // 序列化方法
    std::vector<Item> getEquippedItems() const;
    void setEquippedItems(const std::vector<Item>& items);

private:
    std::unordered_map<EquipmentSlot, Item> equipped_items_;
};

// 名称着色工具（仅对装备根据品质着色）：本科=绿色，硕士=蓝色，博士=红色
std::string getColoredItemName(const Item& item);

// 格式化装备详细信息，用于装备后显示和背包查看
std::string formatEquipmentDetails(const Item& item);

} // namespace hx
