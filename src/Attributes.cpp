#include "Attributes.hpp"
#include "Utilities.hpp"
#include <sstream>
#include <algorithm>
#include <iostream>

namespace hx {

void Attributes::addPoints(int hp_delta, int atk_delta, int def_delta, int spd_delta) {
    max_hp += hp_delta;
    hp = std::min(hp, max_hp); // 确保HP不超过最大值
    atk += atk_delta;
    def_ += def_delta;
    spd += spd_delta;
}

std::string Attributes::toString() const {
    std::ostringstream oss;
    oss << "HP: " << hp << "/" << max_hp 
        << " ATK: " << getEffectiveATK() << "(" << atk << ")"
        << " DEF: " << getEffectiveDEF() << "(" << def_ << ")"
        << " SPD: " << getEffectiveSPD() << "(" << spd << ")";
    
    if (available_points > 0) {
        oss << " 可用属性点: " << available_points;
    }
    
    if (!active_statuses.empty()) {
        oss << "\n状态: " << getStatusDescription();
    }
    
    return oss.str();
}

// 状态效果管理
void Attributes::addStatus(StatusEffect effect, int duration) {
    if (effect == StatusEffect::NONE) return;
    
    StatusInfo info = createStatusInfo(effect, duration);
    active_statuses[effect] = info;
}

void Attributes::removeStatus(StatusEffect effect) {
    active_statuses.erase(effect);
}

void Attributes::updateStatuses() {
    std::vector<StatusEffect> to_remove;
    
    for (auto& [effect, info] : active_statuses) {
        info.duration--;
        if (info.duration <= 0) {
            to_remove.push_back(effect);
        }
    }
    
    for (auto effect : to_remove) {
        removeStatus(effect);
    }
}

bool Attributes::hasStatus(StatusEffect effect) const {
    return active_statuses.find(effect) != active_statuses.end();
}

int Attributes::getStatusDuration(StatusEffect effect) const {
    auto it = active_statuses.find(effect);
    return it != active_statuses.end() ? it->second.duration : 0;
}

// 属性计算（考虑状态效果）
int Attributes::getEffectiveATK() const {
    int effective_atk = atk;
    
    if (hasStatus(StatusEffect::TENSION)) {
        effective_atk = static_cast<int>(effective_atk * 1.2);
    }
    if (hasStatus(StatusEffect::INSPIRATION)) {
        effective_atk = static_cast<int>(effective_atk * 1.15);
    }
    
    return effective_atk;
}

int Attributes::getEffectiveDEF() const {
    int effective_def = def_;
    
    if (hasStatus(StatusEffect::TENSION)) {
        effective_def = static_cast<int>(effective_def * 0.85);
    }
    if (hasStatus(StatusEffect::SHIELD)) {
        effective_def = static_cast<int>(effective_def * 1.3);
    }
    
    return effective_def;
}

int Attributes::getEffectiveSPD() const {
    int effective_spd = spd;
    
    if (hasStatus(StatusEffect::INSPIRATION)) {
        effective_spd = static_cast<int>(effective_spd * 1.1);
    }
    if (hasStatus(StatusEffect::SLOW)) {
        effective_spd = static_cast<int>(effective_spd * 0.8);
    }
    
    return effective_spd;
}

// 属性点分配
bool Attributes::allocatePoint(const std::string& stat) {
    if (available_points <= 0) return false;
    
    if (stat == "hp" || stat == "HP") {
        total_hp_points++;
        max_hp += 5;
        hp += 5;
    } else if (stat == "atk" || stat == "ATK") {
        total_atk_points++;
        atk += 2;
    } else if (stat == "def" || stat == "DEF") {
        total_def_points++;
        def_ += 2;
    } else if (stat == "spd" || stat == "SPD") {
        total_spd_points++;
        spd += 2;
    } else {
        return false;
    }
    
    available_points--;
    return true;
}

void Attributes::addAvailablePoints(int points) {
    available_points += points;
}

// 等级提升
void Attributes::levelUp() {
    max_hp += 10;
    hp = max_hp; // 升级时回满血
    atk += 2;
    def_ += 2;
    spd += 2;
    available_points += 5;
}

// 恢复
void Attributes::heal(int amount) {
    hp = std::min(hp + amount, max_hp);
}

void Attributes::healToFull() {
    hp = max_hp;
}

// 状态效果描述
std::string Attributes::getStatusDescription() const {
    if (active_statuses.empty()) return "";
    
    std::ostringstream oss;
    for (const auto& [effect, info] : active_statuses) {
        if (!oss.str().empty()) oss << ", ";
        oss << info.name << "(" << info.duration << "回合)";
    }
    return oss.str();
}

// 状态效果工具函数
StatusInfo createStatusInfo(StatusEffect effect, int duration) {
    StatusInfo info;
    info.type = effect;
    info.name = getStatusName(effect);
    info.duration = duration;
    info.description = getStatusDescription(effect);
    return info;
}

std::string getStatusName(StatusEffect effect) {
    switch (effect) {
        case StatusEffect::TENSION: return "紧张";
        case StatusEffect::INSPIRATION: return "鼓舞";
        case StatusEffect::SLOW: return "迟缓";
        case StatusEffect::FOCUS: return "专注";
        case StatusEffect::SHIELD: return "护盾";
        default: return "无";
    }
}

std::string getStatusDescription(StatusEffect effect) {
    switch (effect) {
        case StatusEffect::TENSION: return "ATK+20%, DEF-10%";
        case StatusEffect::INSPIRATION: return "ATK+15%, SPD+10%";
        case StatusEffect::SLOW: return "SPD-20%";
        case StatusEffect::FOCUS: return "ATK+10%, 命中率+15%";
        case StatusEffect::SHIELD: return "DEF+30%";
        default: return "";
    }
}

} // namespace hx
