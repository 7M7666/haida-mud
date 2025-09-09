#include "Player.hpp"
#include "Inventory.hpp"
#include <sstream>
#include <algorithm>
#include <iostream>

namespace hx {

// 静态常量定义
const std::string Player::REVIVAL_SCROLL_ID = "revival_scroll";
const std::string Player::LIBRARY_LOCATION_ID = "library";

Player::Player(std::string name) : Entity(name, Attributes{}) {
    inventory_ = std::make_unique<Inventory>();
}

void Player::addXP(int amount) {
    xp_ += amount;
    checkLevelUp();
}

void Player::addCoins(int amount) {
    coins_ += amount;
}

bool Player::spendCoins(int amount) {
    if (coins_ >= amount) {
        coins_ -= amount;
        return true;
    }
    return false;
}

// 装备系统
bool Player::equipItem(const std::string& item_name) {
    // 从背包中查找匹配的物品
    std::vector<Item> items = inventory_->list();
    Item* found_item = nullptr;
    std::string found_id;
    
    // 查找匹配的物品（支持部分匹配）
    for (const auto& item : items) {
        if (item.name.find(item_name) != std::string::npos || 
            item_name.find(item.name) != std::string::npos ||
            item.id == item_name) {
            found_item = const_cast<Item*>(&item);
            found_id = item.id;
            break;
        }
    }
    
    if (!found_item) {
        std::cout << "未找到物品: " << item_name << "\n";
        return false;
    }
    
    // 检查物品是否存在
    if (inventory_->quantity(found_id) < 1) {
        std::cout << "物品数量不足: " << found_item->name << "\n";
        return false;
    }
    
    // 检查是否为装备
    if (found_item->type != ItemType::EQUIPMENT) {
        std::cout << "该物品不是装备，无法装备: " << found_item->name << "\n";
        return false;
    }
    
    // 使用找到的物品信息
    Item item = *found_item;
    
    // 对于饰品，需要特殊处理槽位选择
    if (item.equip_type == EquipmentType::ACCESSORY) {
        // 检查哪个饰品槽位可用
        EquipmentSlot target_slot = EquipmentSlot::ACCESSORY1;
        if (equipment_.isSlotOccupied(EquipmentSlot::ACCESSORY1)) {
            if (equipment_.isSlotOccupied(EquipmentSlot::ACCESSORY2)) {
                // 两个槽位都被占用，智能选择：优先替换品质较低的装备
                const Item* item1 = equipment_.getEquippedItem(EquipmentSlot::ACCESSORY1);
                const Item* item2 = equipment_.getEquippedItem(EquipmentSlot::ACCESSORY2);
                
                // 比较品质：本科 < 硕士 < 博士
                bool replace_item1 = false;
                if (item1 && item2) {
                    if (item.quality > item1->quality) {
                        replace_item1 = true;
                    } else if (item.quality > item2->quality) {
                        replace_item1 = false;
                    } else {
                        // 品质相同，询问用户选择
                        std::cout << "两个饰品槽位都被占用，请选择要替换的槽位：\n";
                        std::cout << "1. 饰品1槽位 (" << getColoredItemName(*item1) << ")\n";
                        std::cout << "2. 饰品2槽位 (" << getColoredItemName(*item2) << ")\n";
                        std::cout << "请选择 (1/2): ";
                        std::string choice;
                        std::getline(std::cin, choice);
                        
                        if (choice == "1") {
                            replace_item1 = true;
                        } else if (choice == "2") {
                            replace_item1 = false;
                        } else {
                            std::cout << "无效选择，取消装备。\n";
                            return false;
                        }
                    }
                }
                
                target_slot = replace_item1 ? EquipmentSlot::ACCESSORY1 : EquipmentSlot::ACCESSORY2;
            } else {
                // 只有饰品1槽位被占用，使用饰品2槽位
                target_slot = EquipmentSlot::ACCESSORY2;
            }
        }
        
        // 如果目标槽位被占用，先卸下旧装备
        if (equipment_.isSlotOccupied(target_slot)) {
            const Item* old_item = equipment_.getEquippedItem(target_slot);
            if (old_item) {
                // 将旧装备放回背包
                inventory_->add(*old_item, 1);
                std::cout << "卸下了 " << getColoredItemName(*old_item) << " 并放回背包。\n";
            }
            equipment_.unequipItem(target_slot);
        }
        
        // 设置饰品的槽位
        item.equip_slot = target_slot;
        
        // 提示用户槽位选择
        if (target_slot == EquipmentSlot::ACCESSORY1) {
            std::cout << "将装备到饰品1槽位。\n";
        } else {
            std::cout << "将装备到饰品2槽位。\n";
        }
    } else {
        // 对于非饰品装备，检查槽位是否被占用
        if (equipment_.isSlotOccupied(item.equip_slot)) {
            const Item* old_item = equipment_.getEquippedItem(item.equip_slot);
            if (old_item) {
                // 将旧装备放回背包
                inventory_->add(*old_item, 1);
                std::cout << "卸下了 " << getColoredItemName(*old_item) << " 并放回背包。\n";
            }
            equipment_.unequipItem(item.equip_slot);
        }
    }
    
    if (equipment_.equipItem(item)) {
        inventory_->remove(found_id, 1);
        updateAttributesFromEquipment();
        
        // 装备成功信息由调用者显示
        
        // 学霸两件套触发提示：当武器与护甲为同一品质
        const Item* eq_weapon = equipment_.getEquippedItem(EquipmentSlot::WEAPON);
        const Item* eq_armor  = equipment_.getEquippedItem(EquipmentSlot::ARMOR);
        if (eq_weapon && eq_armor && eq_weapon->quality == eq_armor->quality) {
            Item qdemo; qdemo.type = ItemType::EQUIPMENT; qdemo.quality = eq_weapon->quality;
            qdemo.name = (eq_weapon->quality==EquipmentQuality::UNDERGRAD?"本科":(eq_weapon->quality==EquipmentQuality::MASTER?"硕士":"博士"));
            int bonus_percent = (eq_weapon->quality==EquipmentQuality::UNDERGRAD?10:(eq_weapon->quality==EquipmentQuality::MASTER?15:20));
            std::cout << "【套装】学霸两件套已触发（" << getColoredItemName(qdemo) << "）全属性+" << bonus_percent << "%\n";
        }
        return true;
    }
    
    return false;
}

bool Player::unequipItem(EquipmentSlot slot) {
    const Item* equipped_item = equipment_.getEquippedItem(slot);
    if (!equipped_item) {
        return false;
    }
    
    // 将装备放回背包
    inventory_->add(*equipped_item, 1);
    
    if (equipment_.unequipItem(slot)) {
        updateAttributesFromEquipment();
        return true;
    }
    
    return false;
}

void Player::updateAttributesFromEquipment() {
    // 重置基础属性（不包括装备加成）
    // 这里需要保存基础属性，暂时简化处理
    // 基础值：ATK=10, DEF=10, SPD=10, HPmax=60；随等级与加点线性成长
    attr().atk = 10 + (level_ - 1) * 2 + attr().total_atk_points * 2;
    attr().def_ = 10 + (level_ - 1) * 2 + attr().total_def_points * 2;
    attr().spd = 10 + (level_ - 1) * 2 + attr().total_spd_points * 2;
    attr().max_hp = 60 + (level_ - 1) * 10 + attr().total_hp_points * 5;
    
    // 添加装备属性
    attr().atk += equipment_.getTotalATK();
    attr().def_ += equipment_.getTotalDEF();
    attr().spd += equipment_.getTotalSPD();
    attr().max_hp += equipment_.getTotalHP();

    // 学霸两件套：当武器与护甲为同一品质时，根据品质给予不同加成
    // 本科套装+10%，硕士套装+15%，博士套装+20%
    const Item* eq_weapon = equipment_.getEquippedItem(EquipmentSlot::WEAPON);
    const Item* eq_armor  = equipment_.getEquippedItem(EquipmentSlot::ARMOR);
    if (eq_weapon && eq_armor && eq_weapon->quality == eq_armor->quality) {
        float bonus_multiplier = 1.0f;
        if (eq_weapon->quality == EquipmentQuality::UNDERGRAD) {
            bonus_multiplier = 1.1f; // 本科套装+10%
        } else if (eq_weapon->quality == EquipmentQuality::MASTER) {
            bonus_multiplier = 1.15f; // 硕士套装+15%
        } else if (eq_weapon->quality == EquipmentQuality::DOCTOR) {
            bonus_multiplier = 1.2f; // 博士套装+20%
        }
        
        attr().atk = static_cast<int>(attr().atk * bonus_multiplier);
        attr().def_ = static_cast<int>(attr().def_ * bonus_multiplier);
        attr().spd = static_cast<int>(attr().spd * bonus_multiplier);
        attr().max_hp = static_cast<int>(attr().max_hp * bonus_multiplier);
    }
    
    // 确保HP不超过最大值
    attr().hp = std::min(attr().hp, attr().max_hp);
}

// 物品使用
bool Player::useItem(const std::string& item_name) {
    // 从背包中查找匹配的物品
    std::vector<Item> items = inventory_->list();
    const Item* found_item = nullptr;
    std::string found_id;
    
    // 查找匹配的物品（支持部分匹配）
    for (const auto& item : items) {
        if (item.name.find(item_name) != std::string::npos || 
            item_name.find(item.name) != std::string::npos ||
            item.id == item_name) {
            found_item = &item;
            found_id = item.id;
            break;
        }
    }
    
    if (!found_item) {
        std::cout << "未找到物品: " << item_name << "\n";
        return false;
    }
    
    // 检查物品是否存在
    if (inventory_->quantity(found_id) < 1) {
        std::cout << "物品数量不足: " << found_item->name << "\n";
        return false;
    }
    
    // 检查是否为消耗品
    if (found_item->type != ItemType::CONSUMABLE) {
        std::cout << "该物品不是消耗品，无法使用: " << found_item->name << "\n";
        return false;
    }
    
    // 根据物品属性处理使用效果
    if (found_item->heal_amount > 0) {
        // 检查是否满血
        if (attr().hp >= attr().max_hp) {
            std::cout << "你的生命值已满，无法使用 " << found_item->name << "。\n";
            return false;
        }
        
        int old_hp = attr().hp;
        attr().heal(found_item->heal_amount);
        int actual_heal = attr().hp - old_hp;
        inventory_->remove(found_id, 1);
        
        if (!found_item->use_message.empty()) {
            std::cout << found_item->use_message << "\n";
        } else {
            std::cout << "使用了 " << found_item->name << "，恢复了 " << actual_heal << " 点生命值。\n";
        }
        // 显示当前生命值
        std::cout << "当前生命值: " << attr().hp << "/" << attr().max_hp << "\n";
        if (!found_item->description.empty()) {
            std::cout << "说明：" << found_item->description << "\n";
        }
        return true;
    }
    
    // 特殊物品处理
    if (found_id == "caffeine_elixir") {
        // 获得专注：下一次攻击必中（持续1回合，使用后即生效并在下一次攻击后移除）
        attr().addStatus(StatusEffect::FOCUS, 1);
        inventory_->remove(found_id, 1);
        std::cout << "你饮下了咖啡因灵液，精神前所未有地集中。下一次攻击将必中。\n";
        if (!found_item->description.empty()) {
            std::cout << "说明：" << found_item->description << "\n";
        }
        return true;
    }
    
    return false;
}

void Player::onDeathPenalty() {
    if (hasRevivalScroll()) {
        useRevivalScroll();
        return;
    }
    
    // 没有复活符，执行死亡惩罚
    respawnAtLibrary();
}

bool Player::hasRevivalScroll() const {
    return inventory_->quantity(REVIVAL_SCROLL_ID) >= 1;
}

bool Player::useRevivalScroll() {
    if (!hasRevivalScroll()) return false;
    
    inventory_->remove(REVIVAL_SCROLL_ID, 1);
    attr().healToFull();
    return true;
}

void Player::respawnAtLibrary() {
    // 等级-1（最小为1）
    if (level_ > 1) {
        level_--;
        // 重新计算属性（减去升级时的属性点）
        attr().addPoints(-10, -2, -2, -2);
        // 减去已分配的属性点
        attr().addPoints(-attr().total_hp_points * 5, 
                        -attr().total_atk_points * 2, 
                        -attr().total_def_points * 2, 
                        -attr().total_spd_points * 2);
        attr().total_hp_points = 0;
        attr().total_atk_points = 0;
        attr().total_def_points = 0;
        attr().total_spd_points = 0;
    }
    
    // 金币扣除10%（向下取整）
    int penalty = coins_ / 10;
    coins_ -= penalty;
    
    // HP回满
    attr().healToFull();
    
    // 传送回图书馆（这里需要游戏状态管理，暂时只记录）
    // 实际传送逻辑在Game类中处理
}

// 好感度系统
int Player::getNPCFavor(const std::string& npc_name) const {
    auto it = npc_favors_.find(npc_name);
    return it != npc_favors_.end() ? it->second : 0;
}

void Player::addNPCFavor(const std::string& npc_name, int amount) {
    npc_favors_[npc_name] += amount;
    // 限制好感度范围 0-100
    npc_favors_[npc_name] = std::max(0, std::min(100, npc_favors_[npc_name]));
}

void Player::setNPCFavor(const std::string& npc_name, int favor) {
    npc_favors_[npc_name] = std::max(0, std::min(100, favor));
}

// 任务系统
void Player::startQuest(const std::string& quest_id, const std::string& quest_name, 
                       const std::string& description, const std::vector<std::string>& objectives) {
    QuestInfo quest;
    quest.id = quest_id;
    quest.name = quest_name;
    quest.description = description;
    quest.status = QuestStatus::IN_PROGRESS;
    quest.objectives = objectives;
    
    quests_[quest_id] = quest;
}

void Player::completeQuest(const std::string& quest_id) {
    auto it = quests_.find(quest_id);
    if (it != quests_.end()) {
        it->second.status = QuestStatus::COMPLETED;
        std::cout << "\033[31m【任务完成】" << it->second.name << " 已完成！\033[0m\n";
    }
}

void Player::failQuest(const std::string& quest_id) {
    auto it = quests_.find(quest_id);
    if (it != quests_.end()) {
        it->second.status = QuestStatus::FAILED;
    }
}

QuestStatus Player::getQuestStatus(const std::string& quest_id) const {
    auto it = quests_.find(quest_id);
    return it != quests_.end() ? it->second.status : QuestStatus::NOT_STARTED;
}

std::vector<QuestInfo> Player::getActiveQuests() const {
    std::vector<QuestInfo> active;
    for (const auto& [id, quest] : quests_) {
        if (quest.status == QuestStatus::IN_PROGRESS) {
            active.push_back(quest);
        }
    }
    return active;
}

std::vector<QuestInfo> Player::getAllQuests() const {
    std::vector<QuestInfo> all;
    for (const auto& [id, quest] : quests_) {
        all.push_back(quest);
    }
    return all;
}

// 结局系统函数已在EndingSystem.cpp中实现

std::vector<SimpleItem> Player::simpleInventory() const {
    std::vector<SimpleItem> result;
    auto items = inventory_->asSimpleItems();
    for (const auto& item : items) {
        result.push_back({item.id, item.name, item.count});
    }
    return result;
}

void Player::setInventory(const std::vector<SimpleItem>& items) {
    // 清空背包并重新添加物品
    inventory_ = std::make_unique<Inventory>();
    for (const auto& item : items) {
        Item new_item;
        new_item.id = item.id;
        new_item.name = item.name;
        new_item.count = item.count;
        
        // 根据物品ID确定正确的物品类型和属性
        if (item.id == "student_uniform") {
            // 普通学子服 - 护甲装备
            new_item = Item::createEquipment("student_uniform", "普通学子服(DEF+5,HP+15)", 
                "海大计算机学院的标准校服，虽然普通但很实用。", 
                EquipmentType::ARMOR, EquipmentSlot::ARMOR, 0, 5, 0, 15, 50);
            new_item.quality = EquipmentQuality::UNDERGRAD;
            new_item.type = ItemType::EQUIPMENT;
        } else if (item.id == "bamboo_notes") {
            // 竹简笔记 - 武器装备
            new_item = Item::createEquipment("bamboo_notes", "竹简笔记(ATK+5,SPD+2)", 
                "古老的竹简，记载着学习心得。", 
                EquipmentType::WEAPON, EquipmentSlot::WEAPON, 5, 0, 2, 0, 30);
            new_item.quality = EquipmentQuality::UNDERGRAD;
            new_item.type = ItemType::EQUIPMENT;
        } else if (item.id == "wisdom_pen") {
            // 启智笔 - 武器装备
            new_item = Item::createEquipment("wisdom_pen", "启智笔", 
                "智力试炼的奖励，能提升思维敏捷度。", 
                EquipmentType::WEAPON, EquipmentSlot::WEAPON, 10, 5, 15, 0, 100);
            new_item.quality = EquipmentQuality::MASTER;
            new_item.type = ItemType::EQUIPMENT;
        } else if (item.id == "steel_spoon") {
            // 钢勺护符 - 护甲装备
            new_item = Item::createEquipment("steel_spoon", "钢勺护符", 
                "食堂选择的奖励，能提供额外保护。", 
                EquipmentType::ARMOR, EquipmentSlot::ARMOR, 0, 20, 0, 0, 80);
            new_item.effect_description = "对实验楼怪物伤害 ×1.3";
            new_item.effect_type = "damage_multiplier";
            new_item.effect_target = "失败实验体";
            new_item.effect_value = 1.3f;
            new_item.quality = EquipmentQuality::UNDERGRAD;
            new_item.type = ItemType::EQUIPMENT;
        } else if (item.id == "weight_bracelet") {
            // 负重护腕 - 饰品装备
            new_item = Item::createEquipment("weight_bracelet", "负重护腕", 
                "毅力试炼的奖励，能增强体魄。", 
                EquipmentType::ACCESSORY, EquipmentSlot::ACCESSORY2, 0, 10, 0, 5, 90);
            new_item.quality = EquipmentQuality::DOCTOR;
            new_item.type = ItemType::EQUIPMENT;
        } else if (item.id == "lab_notebook") {
            // 实验记录本 - 饰品装备
            new_item = Item::createEquipment("lab_notebook", "实验记录本", 
                "抗挫试炼的奖励，记录着实验心得。", 
                EquipmentType::ACCESSORY, EquipmentSlot::ACCESSORY1, 5, 5, 10, 0, 70);
            new_item.quality = EquipmentQuality::UNDERGRAD;
            new_item.type = ItemType::EQUIPMENT;
        } else if (item.id == "safety_goggles" || item.id == "goggles") {
            // 护目镜 - 饰品装备
            new_item = Item::createEquipment(item.id, "护目镜", 
                "实验楼的隐藏奖励，能保护眼睛。", 
                EquipmentType::ACCESSORY, EquipmentSlot::ACCESSORY2, 0, 0, 6, 0, 160);
            new_item.quality = EquipmentQuality::UNDERGRAD;
            new_item.type = ItemType::EQUIPMENT;
            new_item.effect_type = "extra_evasion"; 
            new_item.effect_value = 0.05f; 
            new_item.effect_description = "SPD +6，闪避率额外 +5%";
        } else if (item.id == "speech_words") {
            // 演讲之词 - 饰品装备
            new_item = Item::createEquipment("speech_words", "演讲之词", 
                "表达试炼的奖励，能提升表达能力。", 
                EquipmentType::ACCESSORY, EquipmentSlot::ACCESSORY1, 8, 0, 12, 0, 85);
            new_item.effect_description = "战斗开始时自动获得鼓舞状态";
            new_item.effect_type = "auto_inspiration";
            new_item.effect_target = "all";
            new_item.effect_value = 1.0f;
            new_item.quality = EquipmentQuality::UNDERGRAD;
            new_item.type = ItemType::EQUIPMENT;
        } else if (item.id == "debate_fan") {
            // 辩锋羽扇 - 武器装备
            new_item = Item::createEquipment("debate_fan", "辩锋羽扇", 
                "辩论厅的奖励，能增强辩论技巧。", 
                EquipmentType::WEAPON, EquipmentSlot::WEAPON, 15, 0, 6, 0, 350);
            new_item.quality = EquipmentQuality::DOCTOR;
            new_item.effect_type = "damage_multiplier";
            new_item.effect_target = "答辩紧张魔·强化";
            new_item.effect_value = 1.3f;
            new_item.effect_description = "对答辩紧张魔·强化造成1.3倍伤害";
            new_item.type = ItemType::EQUIPMENT;
        } else if (item.id == "health_potion") {
            // 生命药水 - 消耗品
            new_item = Item::createConsumable("health_potion", "生命药水", 
                "恢复生命值的药水。", 30, 30);
        } else if (item.id == "revival_scroll") {
            // 复活符 - 消耗品
            new_item = Item::createConsumable("revival_scroll", "复活符", 
                "能让你在死亡时无惩罚复活的符咒。", 0, 200);
            new_item.is_quest_item = true;
        } else if (item.id == "power_fragment") {
            // 动力碎片 - 任务物品
            new_item = Item::createQuestItem("power_fragment", "动力碎片", 
                "训练装置的动力碎片，需要收集三块。");
        } else if (item.id == "wenxin_key") {
            // 文心秘钥 - 任务物品
            new_item = Item::createQuestItem("wenxin_key", "文心秘钥", 
                "打开真相之门的钥匙。");
        } else if (item.id == "wenxin_key_ii") {
            // 文心秘钥·II - 任务物品
            new_item = Item::createQuestItem("wenxin_key_ii", "文心秘钥·II", 
                "打开真相之门的钥匙。");
        } else if (item.id == "wenxin_key_iii") {
            // 文心秘钥·III - 任务物品
            new_item = Item::createQuestItem("wenxin_key_iii", "文心秘钥·III", 
                "打开真相之门的钥匙。");
        } else if (item.id == "caffeine_elixir") {
            // 咖啡因灵液 - 消耗品
            new_item = Item::createConsumable("caffeine_elixir", "咖啡因灵液", 
                "获得专注：下一次攻击必中", 0, 150);
        } else if (item.id == "ecard_amulet") {
            // 海大e卡通 - 饰品装备
            new_item = Item::createEquipment("ecard_amulet", "海大e卡通", 
                "一卡在手，畅行校园。", 
                EquipmentType::ACCESSORY, EquipmentSlot::ACCESSORY2, 0, 0, 0, 0, 600);
            new_item.effect_type = "shop_discount"; 
            new_item.effect_value = 0.9f; 
            new_item.effect_description = "商店购物享受10%折扣";
            new_item.quality = EquipmentQuality::DOCTOR;
            new_item.type = ItemType::EQUIPMENT;
        } else if (item.id == "seat_all_lib_amulet") {
            // 全图书馆占座物品 - 饰品装备
            new_item = Item::createEquipment("seat_all_lib_amulet", "全图书馆占座物品", 
                "今日座位，舍我其谁。", 
                EquipmentType::ACCESSORY, EquipmentSlot::ACCESSORY2, 0, 0, 5, 0, 600);
            new_item.effect_type = "first_turn_priority"; 
            new_item.effect_value = 1.0f; 
            new_item.effect_description = "SPD +5，首回合必定先攻";
            new_item.quality = EquipmentQuality::DOCTOR;
            new_item.type = ItemType::EQUIPMENT;
        } else if (item.id == "phd_thesis") {
            // 博士大论文 - 武器装备
            new_item = Item::createEquipment("phd_thesis", "博士大论文", 
                "厚重如砖，砸谁谁懵。", 
                EquipmentType::WEAPON, EquipmentSlot::WEAPON, 15, 0, 2, 0, 320);
            new_item.quality = EquipmentQuality::DOCTOR;
            new_item.type = ItemType::EQUIPMENT;
        } else if (item.id == "bachelor_robe") {
            // 学士袍 - 护甲装备
            new_item = Item::createEquipment("bachelor_robe", "学士袍", 
                "雍容华贵，学养自显。", 
                EquipmentType::ARMOR, EquipmentSlot::ARMOR, 0, 0, 0, 0, 220);
            new_item.effect_type = "all_stats_multiplier"; 
            new_item.effect_value = 1.1f; 
            new_item.effect_description = "全属性 +10%";
            new_item.quality = EquipmentQuality::MASTER;
            new_item.type = ItemType::EQUIPMENT;
        } else if (item.id == "lab_coat") {
            // 实验服 - 护甲装备
            new_item = Item::createEquipment("lab_coat", "实验服", 
                "严谨至上，安全第一。", 
                EquipmentType::ARMOR, EquipmentSlot::ARMOR, 0, 10, 0, 35, 260);
            new_item.quality = EquipmentQuality::DOCTOR;
            new_item.type = ItemType::EQUIPMENT;
        } else if (item.id == "psionic_armor") {
            // 灵能护甲 - 护甲装备
            new_item = Item::createEquipment("psionic_armor", "灵能护甲", 
                "灵能护体，固若金汤。", 
                EquipmentType::ARMOR, EquipmentSlot::ARMOR, 0, 6, 0, 20, 230);
            new_item.effect_type = "periodic_shield"; 
            new_item.effect_value = 0.3f; 
            new_item.effect_description = "每3回合自动套1次30%减伤盾";
            new_item.quality = EquipmentQuality::MASTER;
            new_item.type = ItemType::EQUIPMENT;
        } else if (item.id == "bed_quilt") {
            // 床上的被子 - 护甲装备
            new_item = Item::createEquipment("bed_quilt", "床上的被子", 
                "最是温柔被窝中。", 
                EquipmentType::ARMOR, EquipmentSlot::ARMOR, 0, 7, 0, 0, 140);
            new_item.effect_type = "per_turn_heal_percent"; 
            new_item.effect_value = 0.03f; 
            new_item.effect_description = "每回合恢复3%最大HP";
            new_item.quality = EquipmentQuality::UNDERGRAD;
            new_item.type = ItemType::EQUIPMENT;
        } else if (item.id == "wu_jing_ball") {
            // 吴京篮球 - 武器装备
            new_item = Item::createEquipment("wu_jing_ball", "吴京篮球", 
                "形意合一，攻守兼备。", 
                EquipmentType::WEAPON, EquipmentSlot::WEAPON, 4, 0, 2, 0, 110);
            new_item.quality = EquipmentQuality::UNDERGRAD;
            new_item.type = ItemType::EQUIPMENT;
        } else if (item.id == "second_hand_guitar") {
            // 二手吉他 - 武器装备
            new_item = Item::createEquipment("second_hand_guitar", "二手吉他", 
                "一曲肝肠断，输出即情绪。", 
                EquipmentType::WEAPON, EquipmentSlot::WEAPON, 7, 0, 0, 0, 180);
            new_item.effect_type = "on_attack_inspiration";
            new_item.effect_value = 0.3f;
            new_item.effect_description = "攻击时30%概率触发鼓舞效果（自身ATK +15%，持续2回合）";
            new_item.quality = EquipmentQuality::MASTER;
            new_item.type = ItemType::EQUIPMENT;
        } else {
            // 其他物品，尝试根据名称推断类型
            if (item.name.find("药水") != std::string::npos || 
                item.name.find("符") != std::string::npos) {
                new_item.type = ItemType::CONSUMABLE;
            } else if (item.name.find("服") != std::string::npos || 
                       item.name.find("护甲") != std::string::npos ||
                       item.name.find("装备") != std::string::npos) {
                new_item.type = ItemType::EQUIPMENT;
                new_item.quality = EquipmentQuality::UNDERGRAD; // 默认为本科级
            } else {
                new_item.type = ItemType::QUEST; // 默认为任务物品
            }
        }
        
        new_item.count = item.count;
        inventory_->add(new_item, item.count);
    }
}

// 等级提升检查
void Player::checkLevelUp() {
    int required_xp = level_ * 100; // 每级需要 level * 100 经验
    while (xp_ >= required_xp) {
        xp_ -= required_xp;
        level_++;
        attr_.available_points += 2; // 每级获得2点属性点
        
        // 更新属性（包括装备加成）
        updateAttributesFromEquipment();
        
        // 升级时HP回复到上限
        attr_.hp = attr_.max_hp;
        
        std::cout << "【升级】等级提升至 " << level_ << "！HP已回复到上限。\n";
        std::cout << "【属性点】获得 " << attr_.available_points << " 点属性点可分配！\n";
        std::cout << "💡 使用 'allocate <属性> [数量]' 分配属性点\n";
        std::cout << "   可用属性：hp(生命), atk(攻击), def(防御), spd(速度)\n";
        std::cout << "   示例：allocate hp 1 或 allocate atk 2\n";
        
        required_xp = level_ * 100;
    }
}

// 获取下一级所需经验
int Player::getXPNeededForNextLevel() const {
    int required_xp = level_ * 100;
    return required_xp - xp_;
}

} // namespace hx
