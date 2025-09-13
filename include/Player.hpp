// 玩家类的头文件
// 定义玩家类，包括属性、装备、背包、任务等功能

#pragma once
#include "Entity.hpp"      // 实体基类
#include "Inventory.hpp"   // 背包系统
#include "Item.hpp"        // 物品系统
#include <vector>          // 向量容器
#include <memory>          // 智能指针
#include <string>          // 字符串
#include <unordered_map>   // 哈希映射

namespace hx {
// 简单物品结构体
struct SimpleItem { 
    std::string id;
    std::string name;
    int count;
};

enum class QuestStatus {
    NOT_STARTED,
    IN_PROGRESS,
    COMPLETED,
    FAILED
};

struct QuestInfo {
    std::string id;                           // 任务ID
    std::string name;                         // 任务名称
    std::string description;                  // 任务描述
    QuestStatus status;                       // 任务状态
    std::vector<std::string> objectives;      // 任务目标列表
    std::vector<std::string> rewards;         // 任务奖励列表
};

class Player : public Entity {
public:
    explicit Player(std::string name="无名学子");
    int level() const { return level_; }
    int xp() const { return xp_; }
    int coins() const { return coins_; }
    void addXP(int amount);
    void addCoins(int amount);
    bool spendCoins(int amount);
    Inventory& inventory() { return *inventory_; }
    
    // 装备系统
    Equipment& equipment() { return equipment_; }
    const Equipment& equipment() const { return equipment_; }
    bool equipItem(const std::string& item_id);
    bool unequipItem(EquipmentSlot slot);
    void updateAttributesFromEquipment();
    
    // 死亡复活系统
    void onDeathPenalty();
    bool hasRevivalScroll() const;
    bool useRevivalScroll();
    void respawnAtLibrary();
    
    // 好感度系统
    int getNPCFavor(const std::string& npc_name) const;
    void addNPCFavor(const std::string& npc_name, int amount);
    void setNPCFavor(const std::string& npc_name, int favor);
    std::unordered_map<std::string, int> getAllFavors() const { return npc_favors_; }
    
    // 任务系统
    void startQuest(const std::string& quest_id, const std::string& quest_name, 
                   const std::string& description, const std::vector<std::string>& objectives);
    void completeQuest(const std::string& quest_id);
    void failQuest(const std::string& quest_id);
    QuestStatus getQuestStatus(const std::string& quest_id) const;
    std::vector<QuestInfo> getActiveQuests() const;
    std::vector<QuestInfo> getAllQuests() const;
    
    // 结局系统
    enum class Ending {
        NONE,
        ACADEMIC_SUCCESS,    // 学业有成
        GUARDIAN,           // 秘境守护者
        BALANCED_WALKER,    // 平衡行者
        NORMAL_RETURN,      // 普通回归
        FAILURE            // 失败
    };
    
    Ending calculateEnding() const;
    std::string getEndingDescription(Ending ending) const;
    
    // 文心潭失败计数
    void incrementWenxinFailures() { wenxin_failures_++; }
    int getWenxinFailures() const { return wenxin_failures_; }
    
    // 等级提升
    void checkLevelUp();
    int getXPNeededForNextLevel() const;
    
    // 物品使用
    bool useItem(const std::string& item_id);
    
    const std::string& getName() const { return name_; }
    void setName(const std::string& n) { name_ = n; }
    void setAttr(const Attributes& a) { attr_ = a; }
    void setLevel(int lv) { level_ = lv; }
    void setXP(int x) { xp_ = x; }
    void setCoin(int c) { coins_ = c; }
    std::vector<SimpleItem> simpleInventory() const;
    void setInventory(const std::vector<SimpleItem>& items);
    
    void setNPCFavors(const std::unordered_map<std::string, int>& favors) { npc_favors_ = favors; }
    
    void setQuests(const std::unordered_map<std::string, QuestInfo>& quests) { quests_ = quests; }
    std::unordered_map<std::string, QuestInfo> getQuests() const { return quests_; }
    
    void setWenxinFailures(int failures) { wenxin_failures_ = failures; }

private:
    int level_{1};
    int xp_{0};
    int coins_{50};
    std::unique_ptr<Inventory> inventory_;
    Equipment equipment_;
    
    std::unordered_map<std::string, int> npc_favors_;
    std::unordered_map<std::string, QuestInfo> quests_;
    int wenxin_failures_{0};
    
    static const std::string REVIVAL_SCROLL_ID;
    static const std::string LIBRARY_LOCATION_ID;
};
} // namespace hx
