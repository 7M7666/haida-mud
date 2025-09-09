// 这是游戏状态的头文件
// 作者：大一学生
// 功能：定义游戏的状态结构，包含所有游戏数据

#pragma once
#include "Player.hpp"     // 玩家类
#include "Map.hpp"        // 地图类
#include "Quest.hpp"      // 任务类
#include "Task.hpp"       // 任务管理器
#include "ShopSystem.hpp" // 商店系统
#include <unordered_map>  // 哈希映射
#include <unordered_set>  // 哈希集合

namespace hx { 
// 游戏状态结构体
// 功能：存储游戏的所有状态信息，包括玩家、地图、任务等
struct GameState { 
    std::string current_loc{"library"}; // 初始位置在图书馆
    bool in_teaching_detail = false; // 是否在教学区详细地图中
    Player player{}; 
    Map map; 
    QuestLog quests; 
    TaskManager task_manager; // 任务管理器
    // 主线与结局相关状态
    int wenxintan_fail_streak{0};
    bool key_i_obtained{false};
    bool key_ii_obtained{false};
    bool key_iii_obtained{false};
    bool truth_reward_given{false};
    bool wenxintan_intro_shown{false};
    bool chapter4_shown{false};
    // 支线统计
    int failed_experiment_attack_count{0};
    int failed_experiment_kill_count{0};
    bool s3_reward_given{false};
    bool s4_reward_given{false};
    bool math_difficulty_spirit_first_kill{false}; // 首次击败高数难题精标记
    
    // 对话记忆系统 - 记录已选择过的对话选项
    std::unordered_map<std::string, std::unordered_set<std::string>> dialogue_memory;
    
    // 回合计数器和商店系统
    int turn_counter{0}; // 游戏回合计数器
    ShopSystem shop_system; // 商店系统
    
    // 怪物刷新系统
    struct MonsterSpawnInfo {
        std::string location_id;
        std::string monster_name;
        int max_count;          // 最大数量
        int current_count;      // 当前数量
        int respawn_turns;      // 重生回合数
        int turns_until_respawn; // 距离重生的回合数
        int recommended_level;  // 推荐等级
        int challenge_count;    // 当前刷新周期内已挑战次数
        int max_challenges;     // 每次刷新周期内最大挑战次数
    };
    std::vector<MonsterSpawnInfo> monster_spawns;
}; 
}
