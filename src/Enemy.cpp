#include "Enemy.hpp"

namespace hx {
Enemy::Enemy(std::string name, Attributes attr, int coin_reward, int xp_reward)
: Entity(std::move(name), attr), coin_reward_(coin_reward), xp_reward_(xp_reward) {}

void Enemy::setSpecialSkill(const std::string& skill_name, const std::string& description) {
    special_skill_ = skill_name;
    special_skill_description_ = description;
}

void Enemy::addDropItem(const std::string& item_id, const std::string& item_name, 
                        int min_qty, int max_qty, float rate) {
    drop_items_.push_back({item_id, item_name, min_qty, max_qty, rate});
}

int Enemy::getLevel() const {
    // 基于怪物名称和属性估算等级
    const std::string& name = this->name();
    
    if (name.find("迷糊书虫") != std::string::npos) return 1;
    else if (name.find("拖延小妖") != std::string::npos) return 2;
    else if (name.find("水波幻影") != std::string::npos) return 3;
    else if (name.find("学业焦虑影") != std::string::npos) return 4;
    else if (name.find("夜行怠惰魔") != std::string::npos) return 6;
    else if (name.find("压力黑雾") != std::string::npos) return 7;
    else if (name.find("实验失败妖·复苏") != std::string::npos) return 12;
    else if (name.find("实验失败妖") != std::string::npos) return 8;
    else if (name.find("答辩紧张魔·强化") != std::string::npos) return 12;
    else if (name.find("答辩紧张魔") != std::string::npos) return 8;
    else if (name.find("高数难题精") != std::string::npos) return 9;
    else if (name.find("文献综述怪") != std::string::npos) return 12;
    
    // 如果没有匹配到具体名称，基于属性估算等级
    // 使用HP和ATK的平均值来估算等级
    int estimated_level = (attr().max_hp + attr().atk) / 20;
    return std::max(1, std::min(15, estimated_level)); // 限制在1-15级之间
}
} // namespace hx
