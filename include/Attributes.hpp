// 这是属性系统的头文件
// 作者：大一学生
// 功能：定义角色的属性系统，包括基础属性和状态效果

#pragma once
#include <cstdint>        // 整数类型
#include <string>         // 字符串
#include <vector>         // 向量容器
#include <unordered_map>  // 哈希映射

namespace hx {

// 状态效果枚举
// 功能：定义游戏中所有可能的状态效果
enum class StatusEffect {
    NONE = 0,     // 无状态
    TENSION,      // 紧张：ATK+20%, DEF-10%
    INSPIRATION,  // 鼓舞：ATK+15%, SPD+10%
    SLOW,         // 迟缓：SPD-20%
    FOCUS,        // 专注：ATK+10%, 命中率+15%
    SHIELD        // 护盾：DEF+30%
};

// 状态效果信息
struct StatusInfo {
    StatusEffect type;
    std::string name;
    int duration;
    std::string description;
};

struct Attributes {
    int hp{60};
    int max_hp{60};
    int atk{10};
    int def_{10};
    int spd{10};
    
    // 属性点系统
    int available_points{0};
    int total_hp_points{0};
    int total_atk_points{0};
    int total_def_points{0};
    int total_spd_points{0};

    // 状态效果
    std::unordered_map<StatusEffect, StatusInfo> active_statuses;

    // 基础方法
    void addPoints(int hp_delta, int atk_delta, int def_delta, int spd_delta);
    std::string toString() const;
    
    // 状态效果管理
    void addStatus(StatusEffect effect, int duration);
    void removeStatus(StatusEffect effect);
    void updateStatuses();
    bool hasStatus(StatusEffect effect) const;
    int getStatusDuration(StatusEffect effect) const;
    
    // 属性计算（考虑状态效果）
    int getEffectiveATK() const;
    int getEffectiveDEF() const;
    int getEffectiveSPD() const;
    
    // 属性点分配
    bool allocatePoint(const std::string& stat);
    void addAvailablePoints(int points);
    
    // 等级提升
    void levelUp();
    
    // 恢复
    void heal(int amount);
    void healToFull();
    
    // 状态效果描述
    std::string getStatusDescription() const;
};

// 状态效果工具函数
StatusInfo createStatusInfo(StatusEffect effect, int duration);
std::string getStatusName(StatusEffect effect);
std::string getStatusDescription(StatusEffect effect);

} // namespace hx
