#pragma once
#include "Entity.hpp"
#include "Inventory.hpp"
#include "Item.hpp"
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

namespace hx {
struct SimpleItem { std::string id; std::string name; int count; };

// 任务状态
enum class QuestStatus {
    NOT_STARTED,
    IN_PROGRESS,
    COMPLETED,
    FAILED
};

// 任务信息
struct QuestInfo {
    std::string id;
    std::string name;
    std::string description;
    QuestStatus status;
    std::vector<std::string> objectives;
    std::vector<std::string> rewards;
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
    
    // setters/getters used by SaveLoad
    const std::string& getName() const { return name_; }
    void setName(const std::string& n) { name_ = n; }
    void setAttr(const Attributes& a) { attr_ = a; }
    void setLevel(int lv) { level_ = lv; }
    void setXP(int x) { xp_ = x; }
    void setCoin(int c) { coins_ = c; }
    std::vector<SimpleItem> simpleInventory() const;
    void setInventory(const std::vector<SimpleItem>& items);
    
    // 好感度保存/加载
    void setNPCFavors(const std::unordered_map<std::string, int>& favors) { npc_favors_ = favors; }
    
    // 任务保存/加载
    void setQuests(const std::unordered_map<std::string, QuestInfo>& quests) { quests_ = quests; }
    std::unordered_map<std::string, QuestInfo> getQuests() const { return quests_; }
    
    // 文心潭失败次数保存/加载
    void setWenxinFailures(int failures) { wenxin_failures_ = failures; }

private:
    int level_{1};
    int xp_{0};
    int coins_{50};
    std::unique_ptr<Inventory> inventory_;
    Equipment equipment_;
    
    // 好感度系统
    std::unordered_map<std::string, int> npc_favors_;
    
    // 任务系统
    std::unordered_map<std::string, QuestInfo> quests_;
    
    // 文心潭失败计数
    int wenxin_failures_{0};
    
    // 复活符ID
    static const std::string REVIVAL_SCROLL_ID;
    static const std::string LIBRARY_LOCATION_ID;
};
} // namespace hx
