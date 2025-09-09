// 这是存档读档系统的实现文件
// 作者：大一学生
// 功能：实现游戏的存档和读档功能，保存和恢复游戏状态

#include "SaveLoad.hpp"  // 存档读档头文件
#include <fstream>        // 文件流
#include <iostream>       // 输入输出流

namespace hx {

// ---------------- 工具函数 ----------------
// 用于文件读写的辅助函数
static void writeString(std::ofstream& out, const std::string& s){ 
    size_t n = s.size(); 
    out.write((char*)&n, sizeof(n)); 
    out.write(s.data(), n); 
}

static bool readString(std::ifstream& in, std::string& s){ 
    size_t n = 0; 
    if(!in.read((char*)&n, sizeof(n))) return false; 
    s.resize(n); 
    if(!in.read(&s[0], n)) return false; 
    return true; 
}

// ---------------- Attributes 序列化 ----------------
static void writeAttributes(std::ofstream& out, const Attributes& a) {
    out.write((char*)&a.hp, sizeof(a.hp));
    out.write((char*)&a.max_hp, sizeof(a.max_hp));
    out.write((char*)&a.atk, sizeof(a.atk));
    out.write((char*)&a.def_, sizeof(a.def_));
    out.write((char*)&a.spd, sizeof(a.spd));

    out.write((char*)&a.available_points, sizeof(a.available_points));
    out.write((char*)&a.total_hp_points, sizeof(a.total_hp_points));
    out.write((char*)&a.total_atk_points, sizeof(a.total_atk_points));
    out.write((char*)&a.total_def_points, sizeof(a.total_def_points));
    out.write((char*)&a.total_spd_points, sizeof(a.total_spd_points));

    // active_statuses
    size_t sz = a.active_statuses.size();
    out.write((char*)&sz, sizeof(sz));
    for (auto& [effect, info] : a.active_statuses) {
        out.write((char*)&effect, sizeof(effect));
        writeString(out, info.name);
        out.write((char*)&info.duration, sizeof(info.duration));
        writeString(out, info.description);
    }
}

static bool readAttributes(std::ifstream& in, Attributes& a) {
    if (!in.read((char*)&a.hp, sizeof(a.hp))) return false;
    if (!in.read((char*)&a.max_hp, sizeof(a.max_hp))) return false;
    if (!in.read((char*)&a.atk, sizeof(a.atk))) return false;
    if (!in.read((char*)&a.def_, sizeof(a.def_))) return false;
    if (!in.read((char*)&a.spd, sizeof(a.spd))) return false;

    if (!in.read((char*)&a.available_points, sizeof(a.available_points))) return false;
    if (!in.read((char*)&a.total_hp_points, sizeof(a.total_hp_points))) return false;
    if (!in.read((char*)&a.total_atk_points, sizeof(a.total_atk_points))) return false;
    if (!in.read((char*)&a.total_def_points, sizeof(a.total_def_points))) return false;
    if (!in.read((char*)&a.total_spd_points, sizeof(a.total_spd_points))) return false;

    // active_statuses
    size_t sz;
    if (!in.read((char*)&sz, sizeof(sz))) return false;
    a.active_statuses.clear();
    for (size_t i = 0; i < sz; ++i) {
        StatusEffect effect;
        if (!in.read((char*)&effect, sizeof(effect))) return false;
        std::string name, desc;
        int duration;
        if (!readString(in, name)) return false;
        if (!in.read((char*)&duration, sizeof(duration))) return false;
        if (!readString(in, desc)) return false;

        a.active_statuses[effect] = {effect, name, duration, desc};
    }
    return true;
}

// ---------------- Item 序列化 ----------------
static void writeItem(std::ofstream& out, const Item& item) {
    writeString(out, item.id);
    writeString(out, item.name);
    writeString(out, item.description);
    out.write((char*)&item.type, sizeof(item.type));
    out.write((char*)&item.equip_type, sizeof(item.equip_type));
    out.write((char*)&item.equip_slot, sizeof(item.equip_slot));
    out.write((char*)&item.quality, sizeof(item.quality));
    out.write((char*)&item.atk_delta, sizeof(item.atk_delta));
    out.write((char*)&item.def_delta, sizeof(item.def_delta));
    out.write((char*)&item.spd_delta, sizeof(item.spd_delta));
    out.write((char*)&item.hp_delta, sizeof(item.hp_delta));
    out.write((char*)&item.price, sizeof(item.price));
    out.write((char*)&item.count, sizeof(item.count));
    out.write((char*)&item.max_stack, sizeof(item.max_stack));
    writeString(out, item.effect_description);
    writeString(out, item.effect_type);
    writeString(out, item.effect_target);
    out.write((char*)&item.effect_value, sizeof(item.effect_value));
    out.write((char*)&item.is_quest_item, sizeof(item.is_quest_item));
    out.write((char*)&item.is_tradeable, sizeof(item.is_tradeable));
    out.write((char*)&item.heal_amount, sizeof(item.heal_amount));
    out.write((char*)&item.mp_restore, sizeof(item.mp_restore));
    writeString(out, item.use_message);
    out.write((char*)&item.level_requirement, sizeof(item.level_requirement));
    out.write((char*)&item.favor_requirement, sizeof(item.favor_requirement));
}

static bool readItem(std::ifstream& in, Item& item) {
    if(!readString(in, item.id)) return false;
    if(!readString(in, item.name)) return false;
    if(!readString(in, item.description)) return false;
    if(!in.read((char*)&item.type, sizeof(item.type))) return false;
    if(!in.read((char*)&item.equip_type, sizeof(item.equip_type))) return false;
    if(!in.read((char*)&item.equip_slot, sizeof(item.equip_slot))) return false;
    if(!in.read((char*)&item.quality, sizeof(item.quality))) return false;
    if(!in.read((char*)&item.atk_delta, sizeof(item.atk_delta))) return false;
    if(!in.read((char*)&item.def_delta, sizeof(item.def_delta))) return false;
    if(!in.read((char*)&item.spd_delta, sizeof(item.spd_delta))) return false;
    if(!in.read((char*)&item.hp_delta, sizeof(item.hp_delta))) return false;
    if(!in.read((char*)&item.price, sizeof(item.price))) return false;
    if(!in.read((char*)&item.count, sizeof(item.count))) return false;
    if(!in.read((char*)&item.max_stack, sizeof(item.max_stack))) return false;
    if(!readString(in, item.effect_description)) return false;
    if(!readString(in, item.effect_type)) return false;
    if(!readString(in, item.effect_target)) return false;
    if(!in.read((char*)&item.effect_value, sizeof(item.effect_value))) return false;
    if(!in.read((char*)&item.is_quest_item, sizeof(item.is_quest_item))) return false;
    if(!in.read((char*)&item.is_tradeable, sizeof(item.is_tradeable))) return false;
    if(!in.read((char*)&item.heal_amount, sizeof(item.heal_amount))) return false;
    if(!in.read((char*)&item.mp_restore, sizeof(item.mp_restore))) return false;
    if(!readString(in, item.use_message)) return false;
    if(!in.read((char*)&item.level_requirement, sizeof(item.level_requirement))) return false;
    if(!in.read((char*)&item.favor_requirement, sizeof(item.favor_requirement))) return false;
    return true;
}

// ---------------- TaskReward 序列化 ----------------
static void writeTaskReward(std::ofstream& out, const TaskReward& reward) {
    writeString(out, reward.item_id);
    writeString(out, reward.item_name);
    out.write((char*)&reward.quantity, sizeof(reward.quantity));
    out.write((char*)&reward.exp_reward, sizeof(reward.exp_reward));
    out.write((char*)&reward.coin_reward, sizeof(reward.coin_reward));
    writeString(out, reward.description);
}

static bool readTaskReward(std::ifstream& in, TaskReward& reward) {
    if(!readString(in, reward.item_id)) return false;
    if(!readString(in, reward.item_name)) return false;
    if(!in.read((char*)&reward.quantity, sizeof(reward.quantity))) return false;
    if(!in.read((char*)&reward.exp_reward, sizeof(reward.exp_reward))) return false;
    if(!in.read((char*)&reward.coin_reward, sizeof(reward.coin_reward))) return false;
    if(!readString(in, reward.description)) return false;
    return true;
}

// ---------------- Task 序列化 ----------------
static void writeTask(std::ofstream& out, const Task& task) {
    writeString(out, task.getId());
    writeString(out, task.getName());
    writeString(out, task.getDescription());
    TaskType type = task.getType();
    TaskStatus status = task.getStatus();
    out.write((char*)&type, sizeof(type));
    out.write((char*)&status, sizeof(status));
    
    // 保存奖励
    auto rewards = task.getRewards();
    size_t rewardN = rewards.size();
    out.write((char*)&rewardN, sizeof(rewardN));
    for (const auto& reward : rewards) {
        writeTaskReward(out, reward);
    }
    
    // 保存目标
    auto objectives = task.getObjectives();
    size_t objN = objectives.size();
    out.write((char*)&objN, sizeof(objN));
    for (const auto& obj : objectives) {
        writeString(out, obj);
    }
}

static bool readTask(std::ifstream& in, Task& task) {
    std::string id, name, description;
    TaskType type;
    TaskStatus status;
    
    if(!readString(in, id) || !readString(in, name) || !readString(in, description) ||
       !in.read((char*)&type, sizeof(type)) || !in.read((char*)&status, sizeof(status))) {
        return false;
    }
    
    // 创建任务
    std::vector<TaskReward> rewards;
    size_t rewardN;
    if(!in.read((char*)&rewardN, sizeof(rewardN))) return false;
    for (size_t i = 0; i < rewardN; ++i) {
        TaskReward reward;
        if (!readTaskReward(in, reward)) return false;
        rewards.push_back(reward);
    }
    
    task = Task(id, name, description, type, rewards);
    task.setStatus(status);
    
    // 加载目标
    size_t objN;
    if(!in.read((char*)&objN, sizeof(objN))) return false;
    for (size_t i = 0; i < objN; ++i) {
        std::string obj;
        if (!readString(in, obj)) return false;
        task.addObjective(obj);
    }
    
    return true;
}

// ---------------- DialogueOption 序列化 ---------------- 
static void writeDialogueOption(std::ofstream& out, const DialogueOption& option) {
    writeString(out, option.text);
    writeString(out, option.next_dialogue_id);
    out.write((char*)&option.favor_change, sizeof(option.favor_change));
    writeString(out, option.requirement);
}

static bool readDialogueOption(std::ifstream& in, DialogueOption& option) {
    if(!readString(in, option.text)) return false;
    if(!readString(in, option.next_dialogue_id)) return false;
    if(!in.read((char*)&option.favor_change, sizeof(option.favor_change))) return false;
    if(!readString(in, option.requirement)) return false;
    return true;
}

// ---------------- DialogueNode 序列化 ---------------- 
static void writeDialogueNode(std::ofstream& out, const DialogueNode& node) {
    writeString(out, node.id);
    writeString(out, node.npc_text);
    out.write((char*)&node.is_shop, sizeof(node.is_shop));
    out.write((char*)&node.favor_requirement, sizeof(node.favor_requirement));
    out.write((char*)&node.is_visited, sizeof(node.is_visited));
    writeString(out, node.memory_key);
    
    // 保存选项
    size_t options_size = node.options.size();
    out.write((char*)&options_size, sizeof(options_size));
    for (const auto& option : node.options) {
        writeDialogueOption(out, option);
    }
}

static bool readDialogueNode(std::ifstream& in, DialogueNode& node) {
    if(!readString(in, node.id)) return false;
    if(!readString(in, node.npc_text)) return false;
    if(!in.read((char*)&node.is_shop, sizeof(node.is_shop))) return false;
    if(!in.read((char*)&node.favor_requirement, sizeof(node.favor_requirement))) return false;
    if(!in.read((char*)&node.is_visited, sizeof(node.is_visited))) return false;
    if(!readString(in, node.memory_key)) return false;
    
    // 加载选项
    size_t options_size;
    if(!in.read((char*)&options_size, sizeof(options_size))) return false;
    node.options.clear();
    for (size_t i = 0; i < options_size; ++i) {
        DialogueOption option;
        if (!readDialogueOption(in, option)) return false;
        node.options.push_back(option);
    }
    return true;
}

// ---------------- SaveLoad ----------------
bool SaveLoad::save(const GameState& state, const std::string& filename){ 
    std::ofstream out(filename, std::ios::binary); 
    if(!out) return false; 

    writeString(out, state.player.getName()); 

    // Attributes
    writeAttributes(out, state.player.attr());

    int lv = state.player.level(); 
    out.write((char*)&lv, sizeof(lv)); 

    int xp = state.player.xp(); 
    out.write((char*)&xp, sizeof(xp)); 

    int coins = state.player.coins(); 
    out.write((char*)&coins, sizeof(coins)); 

    writeString(out, state.current_loc); 

    // Inventory
    auto items = state.player.simpleInventory(); 
    size_t invN = items.size(); 
    out.write((char*)&invN, sizeof(invN)); 
    for(auto &it : items){ 
        writeString(out, it.id); 
        writeString(out, it.name); 
        out.write((char*)&it.count, sizeof(it.count)); 
    } 
    
    // 保存装备信息
    auto equipped_items = state.player.equipment().getEquippedItems();
    size_t equipN = equipped_items.size();
    out.write((char*)&equipN, sizeof(equipN));
    for (const auto& item : equipped_items) {
        writeItem(out, item);
    }
    
    // 保存NPC好感度
    auto npc_favors = state.player.getAllFavors();
    size_t favorN = npc_favors.size();
    out.write((char*)&favorN, sizeof(favorN));
    for (const auto& [npc_name, favor] : npc_favors) {
        writeString(out, npc_name);
        out.write((char*)&favor, sizeof(favor));
    }
    
    // 保存任务状态
    auto quests = state.player.getQuests();
    size_t questN = quests.size();
    out.write((char*)&questN, sizeof(questN));
    for (const auto& [quest_id, quest] : quests) {
        writeString(out, quest.id);
        writeString(out, quest.name);
        writeString(out, quest.description);
        out.write((char*)&quest.status, sizeof(quest.status));
        
        // 保存任务目标
        size_t objN = quest.objectives.size();
        out.write((char*)&objN, sizeof(objN));
        for (const auto& obj : quest.objectives) {
            writeString(out, obj);
        }
        
        // 保存任务奖励
        size_t rewardN = quest.rewards.size();
        out.write((char*)&rewardN, sizeof(rewardN));
        for (const auto& reward : quest.rewards) {
            writeString(out, reward);
        }
    }
    
    // 保存TaskManager状态
    auto tasks = state.task_manager.getAllTasksData();
    size_t taskN = tasks.size();
    out.write((char*)&taskN, sizeof(taskN));
    for (const auto& task : tasks) {
        writeTask(out, task);
    }
    
    // 保存游戏状态数据
    out.write((char*)&state.in_teaching_detail, sizeof(state.in_teaching_detail));
    out.write((char*)&state.wenxintan_intro_shown, sizeof(state.wenxintan_intro_shown));
    out.write((char*)&state.chapter4_shown, sizeof(state.chapter4_shown));
    
    // 保存主线与结局相关状态
    out.write((char*)&state.wenxintan_fail_streak, sizeof(state.wenxintan_fail_streak));
    out.write((char*)&state.key_i_obtained, sizeof(state.key_i_obtained));
    out.write((char*)&state.key_ii_obtained, sizeof(state.key_ii_obtained));
    out.write((char*)&state.key_iii_obtained, sizeof(state.key_iii_obtained));
    out.write((char*)&state.truth_reward_given, sizeof(state.truth_reward_given));
    
    // 保存支线统计
    out.write((char*)&state.failed_experiment_attack_count, sizeof(state.failed_experiment_attack_count));
    out.write((char*)&state.failed_experiment_kill_count, sizeof(state.failed_experiment_kill_count));
    out.write((char*)&state.s3_reward_given, sizeof(state.s3_reward_given));
    out.write((char*)&state.s4_reward_given, sizeof(state.s4_reward_given));
    out.write((char*)&state.math_difficulty_spirit_first_kill, sizeof(state.math_difficulty_spirit_first_kill));
    
    // 保存对话记忆系统
    size_t dialogue_memory_size = state.dialogue_memory.size();
    out.write((char*)&dialogue_memory_size, sizeof(dialogue_memory_size));
    for (const auto& [npc_name, choices] : state.dialogue_memory) {
        writeString(out, npc_name);
        size_t choices_size = choices.size();
        out.write((char*)&choices_size, sizeof(choices_size));
        for (const auto& choice : choices) {
            writeString(out, choice);
        }
    }
    
    // 保存回合计数器和商店系统
    out.write((char*)&state.turn_counter, sizeof(state.turn_counter));
    
    // 保存商店系统状态
    int revival_scroll_purchases = state.shop_system.getRevivalScrollPurchases();
    out.write((char*)&revival_scroll_purchases, sizeof(revival_scroll_purchases));
    
    // 保存怪物刷新系统
    size_t monster_spawns_size = state.monster_spawns.size();
    out.write((char*)&monster_spawns_size, sizeof(monster_spawns_size));
    for (const auto& spawn : state.monster_spawns) {
        writeString(out, spawn.location_id);
        writeString(out, spawn.monster_name);
        out.write((char*)&spawn.max_count, sizeof(spawn.max_count));
        out.write((char*)&spawn.current_count, sizeof(spawn.current_count));
        out.write((char*)&spawn.respawn_turns, sizeof(spawn.respawn_turns));
        out.write((char*)&spawn.turns_until_respawn, sizeof(spawn.turns_until_respawn));
        out.write((char*)&spawn.recommended_level, sizeof(spawn.recommended_level));
        out.write((char*)&spawn.challenge_count, sizeof(spawn.challenge_count));
        out.write((char*)&spawn.max_challenges, sizeof(spawn.max_challenges));
    }
    
    // 保存地图状态（NPC状态等）
    auto locations = state.map.allLocations();
    size_t locations_size = locations.size();
    out.write((char*)&locations_size, sizeof(locations_size));
    for (const auto& location : locations) {
        writeString(out, location.id);
        writeString(out, location.name);
        writeString(out, location.desc);
        out.write((char*)&location.coord.x, sizeof(location.coord.x));
        out.write((char*)&location.coord.y, sizeof(location.coord.y));
        
        // 保存出口
        size_t exits_size = location.exits.size();
        out.write((char*)&exits_size, sizeof(exits_size));
        for (const auto& exit : location.exits) {
            writeString(out, exit.label);
            writeString(out, exit.to);
        }
        
        // 保存NPC状态
        size_t npcs_size = location.npcs.size();
        out.write((char*)&npcs_size, sizeof(npcs_size));
        for (const auto& npc : location.npcs) {
            writeString(out, npc.name());
            writeString(out, npc.description());
            int favor = npc.getFavor();
            out.write((char*)&favor, sizeof(favor));
            bool given_reward = npc.hasGivenReward();
            out.write((char*)&given_reward, sizeof(given_reward));
            
            // 保存NPC的对话记忆
            auto visited_dialogues = npc.getVisitedDialogues();
            size_t visited_size = visited_dialogues.size();
            out.write((char*)&visited_size, sizeof(visited_size));
            for (const auto& dialogue : visited_dialogues) {
                writeString(out, dialogue);
            }
            
            auto memories = npc.getMemories();
            size_t memories_size = memories.size();
            out.write((char*)&memories_size, sizeof(memories_size));
            for (const auto& memory : memories) {
                writeString(out, memory);
            }
            
            auto chosen_options = npc.getChosenOptions();
            size_t chosen_size = chosen_options.size();
            out.write((char*)&chosen_size, sizeof(chosen_size));
            for (const auto& option : chosen_options) {
                writeString(out, option);
            }
            
            // 保存NPC的对话数据
            auto dialogues = npc.getDialogues();
            size_t dialogues_size = dialogues.size();
            out.write((char*)&dialogues_size, sizeof(dialogues_size));
            for (const auto& [dialogue_id, dialogue_node] : dialogues) {
                writeDialogueNode(out, dialogue_node);
            }
            
            // 保存默认对话ID
            writeString(out, npc.getDefaultDialogueId());
            
            // 保存对话流程
            auto dialogue_flow = npc.getDialogueFlow();
            size_t flow_size = dialogue_flow.size();
            out.write((char*)&flow_size, sizeof(flow_size));
            for (const auto& flow_id : dialogue_flow) {
                writeString(out, flow_id);
            }
        }
        
        // 保存商店物品（如果有的话）
        size_t shop_size = location.shop.size();
        out.write((char*)&shop_size, sizeof(shop_size));
        for (const auto& item : location.shop) {
            writeItem(out, item);
        }
    }
    
    return true; 
}

bool SaveLoad::load(GameState& state, const std::string& filename){ 
    std::ifstream in(filename, std::ios::binary); 
    if(!in) {
        std::cout << "无法打开存档文件: " << filename << std::endl;
        return false;
    } 

    std::string name; 
    if(!readString(in, name)) {
        std::cout << "读取玩家名称失败" << std::endl;
        return false;
    } 
    state.player.setName(name); 

    Attributes a; 
    if(!readAttributes(in, a)) {
        std::cout << "读取玩家属性失败" << std::endl;
        return false;
    } 
    state.player.setAttr(a); 

    int lv; 
    if(!in.read((char*)&lv, sizeof(lv))) {
        std::cout << "读取玩家等级失败" << std::endl;
        return false;
    }
    int xp; 
    if(!in.read((char*)&xp, sizeof(xp))) {
        std::cout << "读取玩家经验失败" << std::endl;
        return false;
    }
    int coins; 
    if(!in.read((char*)&coins, sizeof(coins))) {
        std::cout << "读取玩家金币失败" << std::endl;
        return false;
    }
    state.player.setLevel(lv); 
    state.player.setXP(xp); 
    state.player.setCoin(coins); 

    std::string loc; 
    if(!readString(in, loc)) {
        std::cout << "读取当前位置失败" << std::endl;
        return false;
    } 
    state.current_loc = loc; 

    size_t invN; 
    if(!in.read((char*)&invN, sizeof(invN))) {
        std::cout << "读取背包数量失败" << std::endl;
        return false;
    }
    std::vector<SimpleItem> items; 
    for(size_t i=0; i<invN; ++i){ 
        std::string id, name; 
        int cnt; 
        if(!readString(in, id)) return false; 
        if(!readString(in, name)) return false; 
        if(!in.read((char*)&cnt, sizeof(cnt))) return false; 
        items.push_back({id, name, cnt}); 
    } 
    state.player.setInventory(items); 
    
    // 加载装备信息
    size_t equipN;
    if(!in.read((char*)&equipN, sizeof(equipN))) {
        std::cout << "读取装备数量失败，使用默认值0" << std::endl;
        equipN = 0; // 默认值
    }
    std::vector<Item> equipped_items;
    for (size_t i = 0; i < equipN; ++i) {
        Item item;
        if (!readItem(in, item)) {
            std::cout << "读取装备信息失败，停止加载装备" << std::endl;
            break; // 读取失败，停止
        }
        equipped_items.push_back(item);
    }
    try {
        state.player.equipment().setEquippedItems(equipped_items);
    } catch (const std::exception& e) {
        std::cout << "设置装备信息时出错: " << e.what() << std::endl;
    }
    
    // 更新玩家属性（从装备计算）
    try {
        state.player.updateAttributesFromEquipment();
    } catch (const std::exception& e) {
        std::cout << "更新玩家属性时出错: " << e.what() << std::endl;
    }
    
    // 加载NPC好感度
    size_t favorN;
    if(!in.read((char*)&favorN, sizeof(favorN))) {
        favorN = 0; // 默认值
    }
    std::unordered_map<std::string, int> npc_favors;
    for (size_t i = 0; i < favorN; ++i) {
        std::string npc_name;
        int favor;
        if (!readString(in, npc_name) || !in.read((char*)&favor, sizeof(favor))) {
            break;
        }
        npc_favors[npc_name] = favor;
    }
    state.player.setNPCFavors(npc_favors);
    
    // 加载任务状态
    size_t questN;
    if(!in.read((char*)&questN, sizeof(questN))) {
        questN = 0; // 默认值
    }
    std::unordered_map<std::string, QuestInfo> quests;
    for (size_t i = 0; i < questN; ++i) {
        QuestInfo quest;
        if (!readString(in, quest.id) || !readString(in, quest.name) || 
            !readString(in, quest.description) || !in.read((char*)&quest.status, sizeof(quest.status))) {
            break;
        }
        
        // 加载任务目标
        size_t objN;
        if (!in.read((char*)&objN, sizeof(objN))) break;
        for (size_t j = 0; j < objN; ++j) {
            std::string obj;
            if (!readString(in, obj)) break;
            quest.objectives.push_back(obj);
        }
        
        // 加载任务奖励
        size_t rewardN;
        if (!in.read((char*)&rewardN, sizeof(rewardN))) break;
        for (size_t j = 0; j < rewardN; ++j) {
            std::string reward;
            if (!readString(in, reward)) break;
            quest.rewards.push_back(reward);
        }
        
        quests[quest.id] = quest;
    }
    state.player.setQuests(quests);
    
    // 加载TaskManager状态
    size_t taskN;
    if(!in.read((char*)&taskN, sizeof(taskN))) {
        taskN = 0; // 默认值
    }
    std::vector<Task> tasks;
    for (size_t i = 0; i < taskN; ++i) {
        Task task("", "", "", TaskType::MAIN, {});
        if (!readTask(in, task)) {
            break;
        }
        tasks.push_back(task);
    }
    state.task_manager.setAllTasksData(tasks);
    
    // 加载游戏状态数据
    if(!in.read((char*)&state.in_teaching_detail, sizeof(state.in_teaching_detail))) {
        state.in_teaching_detail = false; // 默认值
    }
    if(!in.read((char*)&state.wenxintan_intro_shown, sizeof(state.wenxintan_intro_shown))) {
        state.wenxintan_intro_shown = false; // 默认值
    }
    if(!in.read((char*)&state.chapter4_shown, sizeof(state.chapter4_shown))) {
        state.chapter4_shown = false; // 默认值
    }
    
    // 加载主线与结局相关状态
    if(!in.read((char*)&state.wenxintan_fail_streak, sizeof(state.wenxintan_fail_streak))) {
        state.wenxintan_fail_streak = 0; // 默认值
    }
    if(!in.read((char*)&state.key_i_obtained, sizeof(state.key_i_obtained))) {
        state.key_i_obtained = false; // 默认值
    }
    if(!in.read((char*)&state.key_ii_obtained, sizeof(state.key_ii_obtained))) {
        state.key_ii_obtained = false; // 默认值
    }
    if(!in.read((char*)&state.key_iii_obtained, sizeof(state.key_iii_obtained))) {
        state.key_iii_obtained = false; // 默认值
    }
    if(!in.read((char*)&state.truth_reward_given, sizeof(state.truth_reward_given))) {
        state.truth_reward_given = false; // 默认值
    }
    
    // 加载支线统计
    if(!in.read((char*)&state.failed_experiment_attack_count, sizeof(state.failed_experiment_attack_count))) {
        state.failed_experiment_attack_count = 0; // 默认值
    }
    if(!in.read((char*)&state.failed_experiment_kill_count, sizeof(state.failed_experiment_kill_count))) {
        state.failed_experiment_kill_count = 0; // 默认值
    }
    if(!in.read((char*)&state.s3_reward_given, sizeof(state.s3_reward_given))) {
        state.s3_reward_given = false; // 默认值
    }
    if(!in.read((char*)&state.s4_reward_given, sizeof(state.s4_reward_given))) {
        state.s4_reward_given = false; // 默认值
    }
    if(!in.read((char*)&state.math_difficulty_spirit_first_kill, sizeof(state.math_difficulty_spirit_first_kill))) {
        state.math_difficulty_spirit_first_kill = false; // 默认值
    }
    
    // 加载对话记忆系统
    size_t dialogue_memory_size;
    if(!in.read((char*)&dialogue_memory_size, sizeof(dialogue_memory_size))) {
        dialogue_memory_size = 0; // 默认值
    }
    state.dialogue_memory.clear();
    for (size_t i = 0; i < dialogue_memory_size; ++i) {
        std::string npc_name;
        if (!readString(in, npc_name)) break;
        size_t choices_size;
        if (!in.read((char*)&choices_size, sizeof(choices_size))) break;
        std::unordered_set<std::string> choices;
        for (size_t j = 0; j < choices_size; ++j) {
            std::string choice;
            if (!readString(in, choice)) break;
            choices.insert(choice);
        }
        state.dialogue_memory[npc_name] = choices;
    }
    
    // 加载回合计数器和商店系统
    if(!in.read((char*)&state.turn_counter, sizeof(state.turn_counter))) {
        state.turn_counter = 0; // 默认值
    }
    
    // 加载商店系统状态
    int revival_scroll_purchases;
    if(!in.read((char*)&revival_scroll_purchases, sizeof(revival_scroll_purchases))) {
        revival_scroll_purchases = 0; // 默认值
    }
    // 设置复活符购买次数
    for (int i = 0; i < revival_scroll_purchases; ++i) {
        state.shop_system.incrementRevivalScrollPurchases();
    }
    
    // 加载怪物刷新系统
    size_t monster_spawns_size;
    if(!in.read((char*)&monster_spawns_size, sizeof(monster_spawns_size))) {
        monster_spawns_size = 0; // 默认值
    }
    state.monster_spawns.clear();
    for (size_t i = 0; i < monster_spawns_size; ++i) {
        GameState::MonsterSpawnInfo spawn;
        if (!readString(in, spawn.location_id)) break;
        if (!readString(in, spawn.monster_name)) break;
        if (!in.read((char*)&spawn.max_count, sizeof(spawn.max_count))) break;
        if (!in.read((char*)&spawn.current_count, sizeof(spawn.current_count))) break;
        if (!in.read((char*)&spawn.respawn_turns, sizeof(spawn.respawn_turns))) break;
        if (!in.read((char*)&spawn.turns_until_respawn, sizeof(spawn.turns_until_respawn))) break;
        if (!in.read((char*)&spawn.recommended_level, sizeof(spawn.recommended_level))) break;
        
        // 尝试读取新增的字段，如果失败则使用默认值（向后兼容）
        if (!in.read((char*)&spawn.challenge_count, sizeof(spawn.challenge_count))) {
            spawn.challenge_count = 0; // 默认值
        }
        if (!in.read((char*)&spawn.max_challenges, sizeof(spawn.max_challenges))) {
            spawn.max_challenges = 3; // 默认值
        }
        
        state.monster_spawns.push_back(spawn);
    }
    
    // 只有在怪物刷新系统完全为空且无法从存档加载时才重新初始化
    // 这通常只会在新游戏或损坏的存档中发生
    if (state.monster_spawns.empty()) {
        std::cout << "警告：怪物刷新系统为空，使用默认配置重新初始化" << std::endl;
        
        // 体育馆 - 低等级区域 (Lv1-3) - 所有怪物5回合刷新
        state.monster_spawns.push_back({"gymnasium", "迷糊书虫", 2, 2, 5, 0, 1, 0, 3});
        state.monster_spawns.push_back({"gymnasium", "拖延小妖", 2, 2, 5, 0, 2, 0, 3});

        // 三六广场 - 中低等级区域 (Lv3-6) - 所有怪物5回合刷新
        state.monster_spawns.push_back({"plaza_36", "水波幻影", 3, 3, 5, 0, 3, 0, 3});
        state.monster_spawns.push_back({"plaza_36", "学业焦虑影", 3, 3, 5, 0, 4, 0, 3});

        // 荒废北操场 - 中等级区域 (Lv6-9) - 所有怪物5回合刷新
        state.monster_spawns.push_back({"north_playground", "夜行怠惰魔", 2, 2, 5, 0, 6, 0, 3});
        state.monster_spawns.push_back({"north_playground", "压力黑雾", 2, 2, 5, 0, 7, 0, 3});
        
        // 教学区详细地图 - 各层级（初始数量3，5回合刷新，每次刷新可挑战3次）
        state.monster_spawns.push_back({"teach_5", "高数难题精", 3, 3, 5, 0, 9, 0, 3});
        state.monster_spawns.push_back({"teach_7", "实验失败妖·群", 3, 3, 5, 0, 8, 0, 3});
        state.monster_spawns.push_back({"tree_space", "答辩紧张魔", 3, 3, 5, 0, 8, 0, 3});
        
        // 文心潭 - 高等级区域 (Lv9-15) - 所有怪物5回合刷新，挑战次数限制3次
        state.monster_spawns.push_back({"wenxintan", "文献综述怪", 1, 1, 5, 0, 12, 0, 3});
        state.monster_spawns.push_back({"wenxintan", "实验失败妖·复苏", 1, 1, 5, 0, 12, 0, 3});
        state.monster_spawns.push_back({"wenxintan", "答辩紧张魔·强化", 1, 1, 5, 0, 12, 0, 3});
    } else {
        std::cout << "成功加载怪物刷新系统，共 " << state.monster_spawns.size() << " 个怪物配置" << std::endl;
    }
    
    // 修复怪物刷新状态：如果还有挑战次数但current_count为0，则重置为1
    for (auto& spawn : state.monster_spawns) {
        if (spawn.challenge_count < spawn.max_challenges && spawn.current_count == 0) {
            spawn.current_count = 1; // 重置为1，表示怪物可用
        }
    }
    
    // 确保所有怪物都有正确的current_count（如果turns_until_respawn为0且challenge_count < max_challenges）
    for (auto& spawn : state.monster_spawns) {
        if (spawn.turns_until_respawn == 0 && spawn.challenge_count < spawn.max_challenges && spawn.current_count == 0) {
            spawn.current_count = spawn.max_count; // 重置为最大数量
        }
    }
    
    // 根据怪物刷新系统状态重新生成怪物到地图上
    // 注意：这里只处理从存档加载的怪物状态，不重新初始化
    for (const auto& spawn : state.monster_spawns) {
        if (spawn.current_count > 0) {
            auto* loc = state.map.get(spawn.location_id);
            if (loc) {
                // 检查怪物是否已经存在
                bool monster_exists = false;
                for (const auto& enemy : loc->enemies) {
                    if (enemy.name() == spawn.monster_name) {
                        monster_exists = true;
                        break;
                    }
                }
                
                if (!monster_exists) {
                    std::cout << "重新生成怪物: " << spawn.monster_name << " 在 " << spawn.location_id << std::endl;
                    // 根据怪物名称和位置创建怪物
                    if (spawn.location_id == "gymnasium") {
                        if (spawn.monster_name == "迷糊书虫") {
                            Enemy confused_bookworm(spawn.monster_name, Attributes{15, 15, 6, 6}, 3, 5);
                            confused_bookworm.addDropItem("health_potion", "生命药水", 1, 1, 0.05f);
                            confused_bookworm.addDropItem("power_fragment", "动力碎片", 1, 1, 0.50f);
                            loc->enemies.push_back(confused_bookworm);
                        } else if (spawn.monster_name == "拖延小妖") {
                            Enemy procrastination_demon(spawn.monster_name, Attributes{18, 18, 5, 5}, 5, 8);
                            procrastination_demon.addDropItem("health_potion", "生命药水", 1, 1, 0.08f);
                            procrastination_demon.addDropItem("power_fragment", "动力碎片", 1, 1, 0.50f);
                            loc->enemies.push_back(procrastination_demon);
                        }
                    } else if (spawn.location_id == "plaza_36") {
                        if (spawn.monster_name == "水波幻影") {
                            Enemy water_phantom(spawn.monster_name, Attributes{25, 25, 8, 12}, 10, 12);
                            water_phantom.addDropItem("health_potion", "生命药水", 1, 1, 0.08f);
                            loc->enemies.push_back(water_phantom);
                        } else if (spawn.monster_name == "学业焦虑影") {
                            Enemy academic_anxiety(spawn.monster_name, Attributes{30, 30, 9, 8}, 12, 15);
                            academic_anxiety.addDropItem("health_potion", "生命药水", 1, 1, 0.12f);
                            loc->enemies.push_back(academic_anxiety);
                        }
                    } else if (spawn.location_id == "north_playground") {
                        if (spawn.monster_name == "夜行怠惰魔") {
                            Enemy night_sloth_demon(spawn.monster_name, Attributes{55, 55, 14, 12}, 20, 25);
                            night_sloth_demon.setSpecialSkill("迟缓攻击", "攻击后50%概率对玩家施加迟缓(SPD-20%, 2回合)");
                            night_sloth_demon.setHasSlowSkill(true);
                            night_sloth_demon.addDropItem("health_potion", "生命药水", 1, 1, 0.10f);
                            night_sloth_demon.addDropItem("caffeine_elixir", "咖啡因灵液", 1, 1, 0.50f);
                            loc->enemies.push_back(night_sloth_demon);
                        } else if (spawn.monster_name == "压力黑雾") {
                            Enemy pressure_black_fog(spawn.monster_name, Attributes{65, 65, 12, 16}, 25, 30);
                            pressure_black_fog.setSpecialSkill("减速领域", "减速(全场SPD-15%, 2回合, 每3回合发动一次)，对玩家实施紧张效果");
                            pressure_black_fog.setHasTensionSkill(true);
                            pressure_black_fog.addDropItem("caffeine_elixir", "咖啡因灵液", 1, 1, 0.50f);
                            loc->enemies.push_back(pressure_black_fog);
                        }
                    } else if (spawn.location_id == "teach_5") {
                        if (spawn.monster_name == "高数难题精") {
                            Enemy math_difficulty_spirit(spawn.monster_name, Attributes{90, 90, 18, 12}, 50, 80);
                            math_difficulty_spirit.setSpecialSkill("专注弱点", "若攻击者处于专注状态，受到伤害+25%");
                            loc->enemies.push_back(math_difficulty_spirit);
                        }
                    } else if (spawn.location_id == "teach_7") {
                        if (spawn.monster_name == "实验失败妖·群") {
                            Enemy failed_experiment_group(spawn.monster_name, Attributes{135, 135, 45, 30}, 80, 120);
                            failed_experiment_group.setSpecialSkill("自爆机制", "每回合随机1只自爆，对玩家造成ATK×0.8真实伤害");
                            failed_experiment_group.setHasExplosionMechanic(true);
                            failed_experiment_group.setIsGroupEnemy(true, 3);
                            loc->enemies.push_back(failed_experiment_group);
                        }
                    } else if (spawn.location_id == "tree_space") {
                        if (spawn.monster_name == "答辩紧张魔") {
                            Enemy defense_anxiety_demon(spawn.monster_name, Attributes{80, 80, 22, 14}, 60, 100);
                            defense_anxiety_demon.setSpecialSkill("紧张施压", "每回合40%概率对玩家施加紧张(DEF-15%, 3回合)");
                            defense_anxiety_demon.setHasTensionSkill(true);
                            loc->enemies.push_back(defense_anxiety_demon);
                        }
                    } else if (spawn.location_id == "wenxintan") {
                        if (spawn.monster_name == "文献综述怪") {
                            Enemy lit_review(spawn.monster_name, Attributes{160, 160, 28, 12}, 70, 100);
                            lit_review.setSpecialSkill("阅读", "每3回合进入阅读状态，DEF+50%，持续2回合");
                            lit_review.addDropItem("wenxin_key_i", "文心秘钥·I", 1, 1, 1.0f);
                            loc->enemies.push_back(lit_review);
                        } else if (spawn.monster_name == "实验失败妖·复苏") {
                            Enemy failed_revive(spawn.monster_name, Attributes{140, 140, 30, 15}, 70, 100);
                            failed_revive.setSpecialSkill("召唤", "每3回合召唤1只实验失败妖，最多3只");
                            failed_revive.setHasSlowSkill(true);
                            failed_revive.addDropItem("wenxin_key_ii", "文心秘钥·II", 1, 1, 1.0f);
                            loc->enemies.push_back(failed_revive);
                        } else if (spawn.monster_name == "答辩紧张魔·强化") {
                            Enemy defense_anxiety_demon_enhanced(spawn.monster_name, Attributes{150, 150, 32, 18}, 80, 120);
                            defense_anxiety_demon_enhanced.setSpecialSkill("紧张施压", "每回合50%概率对玩家施加紧张(DEF-20%, 4回合)");
                            defense_anxiety_demon_enhanced.setHasTensionSkill(true);
                            defense_anxiety_demon_enhanced.addDropItem("wenxin_key_iii", "文心秘钥·III", 1, 1, 1.0f);
                            loc->enemies.push_back(defense_anxiety_demon_enhanced);
                        }
                    }
                }
            }
        }
    }
    
    // 加载地图状态（NPC状态等）
    size_t locations_size;
    if(!in.read((char*)&locations_size, sizeof(locations_size))) {
        locations_size = 0; // 默认值
    }
    
    // 不要清空地图数据，保留原有的怪物配置
    // state.map = Map(); // 重新初始化地图
    
    for (size_t i = 0; i < locations_size; ++i) {
        std::string location_id, location_name, location_desc;
        if (!readString(in, location_id) || !readString(in, location_name) || !readString(in, location_desc)) {
            break;
        }
        
        // 获取现有位置或创建新位置
        Location* location = state.map.get(location_id);
        if (!location) {
            // 如果位置不存在，创建新位置
            Location new_location;
            new_location.id = location_id;
            state.map.addLocation(new_location);
            location = state.map.get(location_id);
        }
        
        // 更新位置基本信息
        location->name = location_name;
        location->desc = location_desc;
        
        // 加载坐标
        if (!in.read((char*)&location->coord.x, sizeof(location->coord.x)) || 
            !in.read((char*)&location->coord.y, sizeof(location->coord.y))) {
            break;
        }
        
        // 加载出口
        size_t exits_size;
        if (!in.read((char*)&exits_size, sizeof(exits_size))) break;
        location->exits.clear(); // 清空现有出口
        for (size_t j = 0; j < exits_size; ++j) {
            std::string label, to;
            if (!readString(in, label) || !readString(in, to)) break;
            location->exits.push_back({label, to});
        }
        
        // 加载NPC状态
        size_t npcs_size;
        if (!in.read((char*)&npcs_size, sizeof(npcs_size))) break;
        location->npcs.clear(); // 清空现有NPC
        for (size_t j = 0; j < npcs_size; ++j) {
            std::string npc_name, npc_desc;
            if (!readString(in, npc_name) || !readString(in, npc_desc)) break;
            
            NPC npc(npc_name, npc_desc);
            
            // 加载NPC状态
            int favor;
            if (!in.read((char*)&favor, sizeof(favor))) break;
            npc.setFavor(favor);
            
            bool given_reward;
            if (!in.read((char*)&given_reward, sizeof(given_reward))) break;
            npc.setGivenReward(given_reward);
            
            // 加载对话记忆
            size_t visited_size;
            if (!in.read((char*)&visited_size, sizeof(visited_size))) break;
            std::unordered_set<std::string> visited_dialogues;
            for (size_t k = 0; k < visited_size; ++k) {
                std::string dialogue;
                if (!readString(in, dialogue)) break;
                visited_dialogues.insert(dialogue);
            }
            npc.setVisitedDialogues(visited_dialogues);
            
            // 加载记忆
            size_t memories_size;
            if (!in.read((char*)&memories_size, sizeof(memories_size))) break;
            std::unordered_set<std::string> memories;
            for (size_t k = 0; k < memories_size; ++k) {
                std::string memory;
                if (!readString(in, memory)) break;
                memories.insert(memory);
            }
            npc.setMemories(memories);
            
            // 加载已选择的选项
            size_t chosen_size;
            if (!in.read((char*)&chosen_size, sizeof(chosen_size))) break;
            std::unordered_set<std::string> chosen_options;
            for (size_t k = 0; k < chosen_size; ++k) {
                std::string option;
                if (!readString(in, option)) break;
                chosen_options.insert(option);
            }
            npc.setChosenOptions(chosen_options);
            
            // 加载NPC的对话数据
            size_t dialogues_size;
            if (!in.read((char*)&dialogues_size, sizeof(dialogues_size))) break;
            std::unordered_map<std::string, DialogueNode> dialogues;
            for (size_t k = 0; k < dialogues_size; ++k) {
                DialogueNode dialogue_node;
                if (!readDialogueNode(in, dialogue_node)) break;
                dialogues[dialogue_node.id] = dialogue_node;
            }
            npc.setDialogues(dialogues);
            
            // 加载默认对话ID
            std::string default_dialogue_id;
            if (!readString(in, default_dialogue_id)) break;
            npc.setDefaultDialogueId(default_dialogue_id);
            
            // 加载对话流程
            size_t flow_size;
            if (!in.read((char*)&flow_size, sizeof(flow_size))) break;
            std::vector<std::string> dialogue_flow;
            for (size_t k = 0; k < flow_size; ++k) {
                std::string flow_id;
                if (!readString(in, flow_id)) break;
                dialogue_flow.push_back(flow_id);
            }
            npc.setDialogueFlow(dialogue_flow);
            
            location->npcs.push_back(npc);
        }
        
        // 加载商店物品
        size_t shop_size;
        if (!in.read((char*)&shop_size, sizeof(shop_size))) break;
        location->shop.clear(); // 清空现有商店物品
        for (size_t j = 0; j < shop_size; ++j) {
            Item item;
            if (!readItem(in, item)) break;
            location->shop.push_back(item);
        }
        
        // 注意：不调用addLocation，因为我们已经更新了现有位置
        // 怪物数据应该被保留，因为我们在更新位置时没有清空enemies
    }
    
    return true; 
}

} // namespace hx