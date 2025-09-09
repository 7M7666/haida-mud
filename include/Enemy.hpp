// 这是敌人类的头文件
// 作者：大一学生
// 功能：定义游戏中的敌人类，包括怪物属性和特殊技能

#pragma once
#include "Entity.hpp"      // 实体基类
#include "Attributes.hpp"  // 属性系统
#include <vector>          // 向量容器
#include <string>          // 字符串
#include <functional>      // 函数对象

namespace hx {
// 敌人类
// 功能：继承自Entity，表示游戏中的敌人和怪物
class Enemy : public Entity {
public:
    Enemy(std::string name, Attributes attr, int coin_reward, int xp_reward);
    int coinReward() const { return coin_reward_; }
    int xpReward() const { return xp_reward_; }
    
    // 获取怪物等级（基于属性估算）
    int getLevel() const;
    
    // 特殊技能相关
    void setSpecialSkill(const std::string& skill_name, const std::string& description);
    bool hasSpecialSkill() const { return !special_skill_.empty(); }
    const std::string& getSpecialSkill() const { return special_skill_; }
    const std::string& getSpecialSkillDescription() const { return special_skill_description_; }
    
    // 掉落物品相关
    struct DropItem {
        std::string item_id;
        std::string item_name;
        int min_quantity;
        int max_quantity;
        float drop_rate; // 0.0-1.0
    };
    
    void addDropItem(const std::string& item_id, const std::string& item_name, 
                     int min_qty, int max_qty, float rate);
    const std::vector<DropItem>& getDropItems() const { return drop_items_; }
    
    // 特殊效果标记
    void setHasSlowSkill(bool has) { has_slow_skill_ = has; }
    bool hasSlowSkill() const { return has_slow_skill_; }
    
    void setHasTensionSkill(bool has) { has_tension_skill_ = has; }
    bool hasTensionSkill() const { return has_tension_skill_; }
    
    void setHasExplosionMechanic(bool has) { has_explosion_mechanic_ = has; }
    bool hasExplosionMechanic() const { return has_explosion_mechanic_; }
    
    void setIsGroupEnemy(bool is_group, int count = 1) { 
        is_group_enemy_ = is_group; 
        group_count_ = count; 
    }
    bool isGroupEnemy() const { return is_group_enemy_; }
    int getGroupCount() const { return group_count_; }
    
private:
    int coin_reward_{0};
    int xp_reward_{0};
    std::string special_skill_;
    std::string special_skill_description_;
    std::vector<DropItem> drop_items_;
    
    // 特殊效果标记
    bool has_slow_skill_{false};
    bool has_tension_skill_{false};
    bool has_explosion_mechanic_{false};
    bool is_group_enemy_{false};
    int group_count_{1};
};
} // namespace hx
