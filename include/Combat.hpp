// 战斗系统的头文件
// 定义战斗系统，包括技能、伤害计算、战斗逻辑等

#pragma once
#include "Player.hpp"     // 玩家类
#include "Enemy.hpp"      // 敌人类
#include <random>         // 随机数生成
#include <string>         // 字符串
#include <vector>         // 向量容器
#include <functional>     // 函数对象
#include <unordered_map>  // 哈希映射

namespace hx {

// 战斗动作类型枚举
// 定义玩家在战斗中可以执行的动作
enum class CombatAction {
    ATTACK,     // 普通攻击
    SKILL,      // 使用技能
    INVENTORY,  // 使用物品
    DEFEND,     // 防御
    TALK,       // 交谈
    FLEE        // 逃跑
};

// 技能信息结构体
// 存储技能的所有信息
struct Skill {
    std::string name;                                                    // 技能名称
    std::string description;                                             // 技能描述
    int mp_cost;                                                         // 魔法消耗
    int cooldown;                                                        // 冷却时间
    int current_cooldown;                                                // 当前冷却时间
    std::function<void(Player&, Enemy&, std::string&)> effect;          // 技能效果函数
};

class CombatSystem {
public:
    explicit CombatSystem(unsigned seed = std::random_device{}());
    void setGameState(class GameState* gs) { game_state_ = gs; }
    
    bool fight(Player& player, Enemy& enemy, std::string& log);
    bool processPlayerAction(Player& player, Enemy& enemy, CombatAction action, 
                           const std::string& target, std::string& log);
    
    void addSkill(const std::string& skill_name, const Skill& skill);
    std::vector<Skill> getAvailableSkills(const Player& player) const;
    bool useSkill(Player& player, Enemy& enemy, const std::string& skill_name, std::string& log);
    
    int calculatePhysicalDamage(int atk, int def, double random_factor = 1.0);
    int calculatePhysicalDamage(const Player& player, const Enemy& enemy, double random_factor = 1.0);
    int calculateSkillDamage(const Skill& skill, const Player& player, const Enemy& enemy);
    
    int updateCombatStatuses(Player& player, Enemy& enemy);
    bool attemptFlee(const Player& player, const Enemy& enemy);

private:
    std::mt19937 rng_;
    GameState* game_state_{nullptr};
    
    std::unordered_map<std::string, Skill> skills_;
    
    int rollPercent();
    double rollRandomFactor();
    bool rollHit(int attacker_spd, int defender_spd);
    
    void initializeSkills();
    
    void skillKnowledgeTheft(Player& player, Enemy& enemy, std::string& log);
    void skillDeadlineRush(Player& player, Enemy& enemy, std::string& log);
    void skillCodeReview(Player& player, Enemy& enemy, std::string& log);
    void skillDebugMode(Player& player, Enemy& enemy, std::string& log);
    void skillAlgorithmOptimization(Player& player, Enemy& enemy, std::string& log);
};

} // namespace hx
