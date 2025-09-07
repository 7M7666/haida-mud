#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include "Item.hpp"

namespace hx {

struct DialogueOption {
    std::string text;
    std::string next_dialogue_id;
    std::function<void()> action; // 可选的行动回调
    int favor_change{0}; // 好感度变化
    std::string requirement; // 需求条件（如物品、等级等）
};

struct DialogueNode {
    std::string id;
    std::string npc_text;
    std::vector<DialogueOption> options;
    bool is_shop{false}; // 是否显示商店选项
    int favor_requirement{0}; // 需要的好感度
    bool is_visited{false}; // 是否已经访问过
    std::string memory_key; // 记忆键，用于避免重复信息
};

// 商店物品
struct ShopItem {
    std::string id;
    std::string name;
    std::string description;
    int price;
    int stock;
    int favor_requirement{0}; // 需要的好感度
    EquipmentQuality quality{EquipmentQuality::UNDERGRAD}; // 装备品质
    
    // 构造函数
    ShopItem() = default;
    ShopItem(const std::string& id, const std::string& name, const std::string& description, 
             int price, int stock, int favor_requirement = 0, EquipmentQuality quality = EquipmentQuality::UNDERGRAD)
        : id(id), name(name), description(description), price(price), stock(stock), favor_requirement(favor_requirement), quality(quality) {}
};

class NPC {
public:
    NPC(std::string name, std::string description);
    
    void addDialogue(const std::string& id, const DialogueNode& node);
    const DialogueNode* getDialogue(const std::string& id) const;
    const std::string& name() const { return name_; }
    const std::string& description() const { return description_; }
    
    // 设置默认对话
    void setDefaultDialogue(const std::string& id) { default_dialogue_id_ = id; }
    const std::string& defaultDialogue() const { return default_dialogue_id_; }
    
    // 好感度系统
    int getFavor() const { return favor_; }
    void addFavor(int amount);
    void setFavor(int favor);
    
    // 商店系统
    void addShopItem(const ShopItem& item);
    std::vector<ShopItem> getAvailableItems(int player_favor) const;
    bool hasShop() const { return !shop_items_.empty(); }
    
    // 任务相关
    bool hasQuest() const { return !quest_id_.empty(); }
    const std::string& getQuestId() const { return quest_id_; }
    void setQuestId(const std::string& quest_id) { quest_id_ = quest_id; }
    
    // 奖励系统
    bool hasGivenReward() const { return given_reward_; }
    void setGivenReward(bool given) { given_reward_ = given; }
    
    // 对话条件检查
    bool canAccessDialogue(const std::string& dialogue_id, int player_favor) const;
    bool canChooseOption(const DialogueOption& option, int player_favor, 
                        const std::unordered_map<std::string, int>& player_inventory) const;
    
    // 对话记忆系统
    void markDialogueVisited(const std::string& dialogue_id);
    bool hasVisitedDialogue(const std::string& dialogue_id) const;
    bool hasMemory(const std::string& memory_key) const;
    void addMemory(const std::string& memory_key);
    void clearMemory();
    
    // 对话选项记忆系统
    void markOptionChosen(const std::string& option_key);
    bool hasChosenOption(const std::string& option_key) const;
    void clearOptionMemory();
    
    // 获取内部状态（用于保存/加载）
    const std::unordered_set<std::string>& getVisitedDialogues() const { return visited_dialogues_; }
    const std::unordered_set<std::string>& getMemories() const { return memories_; }
    const std::unordered_set<std::string>& getChosenOptions() const { return chosen_options_; }
    void setVisitedDialogues(const std::unordered_set<std::string>& dialogues) { visited_dialogues_ = dialogues; }
    void setMemories(const std::unordered_set<std::string>& memories) { memories_ = memories; }
    void setChosenOptions(const std::unordered_set<std::string>& options) { chosen_options_ = options; }
    
    // 获取和设置对话数据（用于保存/加载）
    const std::unordered_map<std::string, DialogueNode>& getDialogues() const { return dialogues_; }
    void setDialogues(const std::unordered_map<std::string, DialogueNode>& dialogues) { dialogues_ = dialogues; }
    const std::string& getDefaultDialogueId() const { return default_dialogue_id_; }
    void setDefaultDialogueId(const std::string& id) { default_dialogue_id_ = id; }
    
    // 获取线性对话流程
    std::string getNextDialogueId(const std::string& current_id) const;
    void setDialogueFlow(const std::vector<std::string>& flow);
    const std::vector<std::string>& getDialogueFlow() const { return dialogue_flow_; }
    
private:
    std::string name_;
    std::string description_;
    std::unordered_map<std::string, DialogueNode> dialogues_;
    std::string default_dialogue_id_{"main_menu"};
    
    // 好感度
    int favor_{0};
    
    // 商店物品
    std::vector<ShopItem> shop_items_;
    
    // 任务相关
    std::string quest_id_;
    
    // 奖励系统
    bool given_reward_{false};
    
    // 对话记忆系统
    std::unordered_set<std::string> visited_dialogues_;
    std::unordered_set<std::string> memories_;
    std::unordered_set<std::string> chosen_options_; // 已选择的对话选项
    std::vector<std::string> dialogue_flow_; // 线性对话流程
};

} // namespace hx
