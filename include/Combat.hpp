#pragma once
#include "Player.hpp"
#include "Enemy.hpp"
#include <random>
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>

namespace hx {

// 战斗动作类型
enum class CombatAction {
    ATTACK,     // 普通攻击
    SKILL,      // 技能
    INVENTORY,  // 使用物品
    DEFEND,     // 防御
    TALK,       // 交谈
    FLEE        // 逃跑
};

// 技能信息
struct Skill {
    std::string name;
    std::string description;
    int mp_cost;
    int cooldown;
    int current_cooldown;
    std::function<void(Player&, Enemy&, std::string&)> effect;
};

class CombatSystem {
public:
    explicit CombatSystem(unsigned seed = std::random_device{}());
    // 连接游戏状态（用于支线计数等）
    void setGameState(class GameState* gs) { game_state_ = gs; }
    
    // 主要战斗函数
    bool fight(Player& player, Enemy& enemy, std::string& log);
    
    // 战斗选项处理
    bool processPlayerAction(Player& player, Enemy& enemy, CombatAction action, 
                           const std::string& target, std::string& log);
    
    // 技能系统
    void addSkill(const std::string& skill_name, const Skill& skill);
    std::vector<Skill> getAvailableSkills(const Player& player) const;
    bool useSkill(Player& player, Enemy& enemy, const std::string& skill_name, std::string& log);
    
    // 伤害计算
    int calculatePhysicalDamage(int atk, int def, double random_factor = 1.0);
    int calculatePhysicalDamage(const Player& player, const Enemy& enemy, double random_factor = 1.0);
    int calculateSkillDamage(const Skill& skill, const Player& player, const Enemy& enemy);
    
    // 状态效果更新
    int updateCombatStatuses(Player& player, Enemy& enemy);
    
    // 逃跑判定
    bool attemptFlee(const Player& player, const Enemy& enemy);

private:
    std::mt19937 rng_;
    GameState* game_state_{nullptr};
    
    // 技能库
    std::unordered_map<std::string, Skill> skills_;
    
    // 工具函数
    int rollPercent();
    double rollRandomFactor();
    bool rollHit(int attacker_spd, int defender_spd);
    
    // 预设技能
    void initializeSkills();
    
    // 技能实现
    void skillKnowledgeTheft(Player& player, Enemy& enemy, std::string& log);
    void skillDeadlineRush(Player& player, Enemy& enemy, std::string& log);
    void skillCodeReview(Player& player, Enemy& enemy, std::string& log);
    void skillDebugMode(Player& player, Enemy& enemy, std::string& log);
    void skillAlgorithmOptimization(Player& player, Enemy& enemy, std::string& log);
};

} // namespace hx
