// 这是NPC系统的实现文件
// 作者：大一学生
// 功能：实现游戏中的NPC系统，包括对话、商店、好感度等

#include "NPC.hpp"    // NPC类头文件
#include <algorithm>   // 算法库

namespace hx {

// NPC类的构造函数
// 参数：name(NPC名称), description(NPC描述)
// 功能：初始化NPC的基本信息
NPC::NPC(std::string name, std::string description) 
    : name_(std::move(name)), description_(std::move(description)) {}

void NPC::addDialogue(const std::string& id, const DialogueNode& node) {
    dialogues_[id] = node;
}

const DialogueNode* NPC::getDialogue(const std::string& id) const {
    auto it = dialogues_.find(id);
    return it != dialogues_.end() ? &it->second : nullptr;
}

// 好感度系统
void NPC::addFavor(int amount) {
    favor_ += amount;
    favor_ = std::max(0, std::min(100, favor_));
}

void NPC::setFavor(int favor) {
    favor_ = std::max(0, std::min(100, favor));
}

// 商店系统
void NPC::addShopItem(const ShopItem& item) {
    shop_items_.push_back(item);
}

std::vector<ShopItem> NPC::getAvailableItems(int player_favor) const {
    std::vector<ShopItem> available;
    for (const auto& item : shop_items_) {
        if (player_favor >= item.favor_requirement) {
            available.push_back(item);
        }
    }
    return available;
}

// 对话条件检查
bool NPC::canAccessDialogue(const std::string& dialogue_id, int player_favor) const {
    auto it = dialogues_.find(dialogue_id);
    if (it == dialogues_.end()) return false;
    
    return player_favor >= it->second.favor_requirement;
}

bool NPC::canChooseOption(const DialogueOption& option, int player_favor, 
                         const std::unordered_map<std::string, int>& player_inventory) const {
    // 检查好感度要求
    if (option.favor_change < 0 && player_favor + option.favor_change < 0) {
        return false;
    }
    
    // 检查物品要求
    if (!option.requirement.empty()) {
        // 简单的物品检查（可以根据需要扩展）
        auto it = player_inventory.find(option.requirement);
        if (it == player_inventory.end() || it->second <= 0) {
            return false;
        }
    }
    
    return true;
}

// 对话记忆系统实现
void NPC::markDialogueVisited(const std::string& dialogue_id) {
    visited_dialogues_.insert(dialogue_id);
    auto it = dialogues_.find(dialogue_id);
    if (it != dialogues_.end()) {
        it->second.is_visited = true;
    }
}

bool NPC::hasVisitedDialogue(const std::string& dialogue_id) const {
    return visited_dialogues_.find(dialogue_id) != visited_dialogues_.end();
}

bool NPC::hasMemory(const std::string& memory_key) const {
    return memories_.find(memory_key) != memories_.end();
}

void NPC::addMemory(const std::string& memory_key) {
    memories_.insert(memory_key);
}

void NPC::clearMemory() {
    memories_.clear();
    visited_dialogues_.clear();
}

// 对话选项记忆系统实现
void NPC::markOptionChosen(const std::string& option_key) {
    chosen_options_.insert(option_key);
}

bool NPC::hasChosenOption(const std::string& option_key) const {
    return chosen_options_.find(option_key) != chosen_options_.end();
}

void NPC::clearOptionMemory() {
    chosen_options_.clear();
}

// 获取线性对话流程
std::string NPC::getNextDialogueId(const std::string& current_id) const {
    auto it = std::find(dialogue_flow_.begin(), dialogue_flow_.end(), current_id);
    if (it != dialogue_flow_.end() && it + 1 != dialogue_flow_.end()) {
        return *(it + 1);
    }
    return ""; // 没有下一个对话
}

void NPC::setDialogueFlow(const std::vector<std::string>& flow) {
    dialogue_flow_ = flow;
}

} // namespace hx
