#include "Combat.hpp"
#include "GameState.hpp"
#include <sstream>
#include <algorithm>
#include <cmath>
#include <iostream>

namespace hx {
static int clamp(int v, int lo, int hi) { return std::max(lo, std::min(v, hi)); }
static double clamp(double v, double lo, double hi) { return std::max(lo, std::min(v, hi)); }

CombatSystem::CombatSystem(unsigned seed) : rng_(seed) {
    initializeSkills();
}

int CombatSystem::rollPercent() { 
    std::uniform_int_distribution<int> d(1,100); 
    return d(rng_); 
}

double CombatSystem::rollRandomFactor() {
    std::uniform_real_distribution<double> d(0.9, 1.1);
    return d(rng_);
}

bool CombatSystem::rollHit(int attacker_spd, int defender_spd) {
    int spd_diff = defender_spd - attacker_spd;
    int hit_chance = clamp(100 - (spd_diff * 2), 15, 95);
    return rollPercent() <= hit_chance;
}

// 主要战斗函数
bool CombatSystem::fight(Player& player, Enemy& enemy, std::string& log) {
    std::ostringstream L;
    auto& pa = player.attr();
    auto ea = enemy.attr(); // copy enemy attr (so we can mutate)

    // 隐式动态难度层级计算：0=新手 1=默认 2=熟练
    auto calcTier = [this,&player]() -> int {
        int tier = 1;
        if (!game_state_) return 1;
        int level = player.level();
        int qualityScore = 0;
        for (const auto& it : player.equipment().getEquippedItems()) {
            if (it.quality == EquipmentQuality::MASTER) qualityScore += 1;
            else if (it.quality == EquipmentQuality::DOCTOR) qualityScore += 2;
        }
        int keys = 0;
        keys += game_state_->key_i_obtained ? 1 : 0;
        keys += game_state_->key_ii_obtained ? 1 : 0;
        keys += game_state_->key_iii_obtained ? 1 : 0;
        if (level < 6 || (qualityScore <= 0 && level < 9)) tier = 0;
        if (level >= 10 || qualityScore >= 3 || keys >= 2) tier = 2;
        return tier;
    };
    int tier = calcTier();

    L << "遭遇敌人：" << enemy.name() << "\n";
    L << "战斗开始！\n";
    
    // 检查装备特殊效果
    if (player.equipment().hasEffect("auto_inspiration")) {
        player.attr().addStatus(StatusEffect::INSPIRATION, 3);
        L << "【演讲之词】战斗开始时，你获得了鼓舞状态！\n";
    }
    if (player.equipment().hasEffect("auto_focus")) {
        player.attr().addStatus(StatusEffect::FOCUS, 1);
        L << "【校徽】你在战斗开始进入专注状态。\n";
    }

    auto turn = 0;
    int reading_buff_turns = 0; // 文献综述怪阅读增防
    bool is_failed_revive = (enemy.name() == "实验失败妖·复苏");
    int summoned_minions = is_failed_revive ? (tier==0?2:3) : 0; // 层级0开场2只，其它3只
    int summon_cooldown = is_failed_revive ? 3 : 0;   // 每3回合+1
    while (pa.hp > 0 && ea.hp > 0) {
        ++turn; 
        L << "\n—— 回合 " << turn << " ——\n";
        L << "你的HP: " << pa.hp << "/" << pa.max_hp << " | " << enemy.name() << " HP: " << ea.hp << "/" << ea.max_hp << "\n";
        
        // 更新状态效果
        player.attr().updateStatuses(); // 更新玩家状态持续时间
        int heal_amount = updateCombatStatuses(player, enemy);
        if (heal_amount > 0) {
            L << "【床上的被子】恢复了 " << heal_amount << " 点生命值。\n";
        }
        // 文献综述怪：层级0每4回合，其它每3回合进入阅读，持续2回合
        if (enemy.name() == "文献综述怪") {
            int freq = (tier==0?4:3);
            if ((turn - 1) % freq == 0) {
                reading_buff_turns = 2;
                L << "【阅读】文献综述怪进入阅读状态，防御提升！\n";
            }
        }
        // 复苏Boss：召唤节律
        if (is_failed_revive) {
            summon_cooldown -= 1;
            if (summon_cooldown <= 0) {
                if (tier >= 1) {
                    summoned_minions += 1; // 层级0不再追加
                }
                summon_cooldown = 3;
                L << "【召唤】又有一只失败实验体小怪加入战场！（当前：" << summoned_minions << ")\n";
            }
        }
        
        // Determine order by SPD（首回合先攻加成）
        bool playerFirst = pa.getEffectiveSPD() >= ea.spd;
        if (turn == 1 && player.equipment().hasEffect("first_turn_priority")) {
            playerFirst = true;
        }
        
        if (playerFirst) {
            // 玩家行动
            if (pa.hp > 0) {
                // 动态防御修正（阅读+50%防御）与钢勺护符克制
                int enemy_def_for_calc = enemy.attr().def_;
                if (reading_buff_turns > 0) enemy_def_for_calc = (int)(enemy_def_for_calc * 1.5);
                double rf = rollRandomFactor();
                double base_damage = player.attr().getEffectiveATK() * rf;
                if (enemy.name().find("实验失败妖") != std::string::npos) {
                    // 钢勺护符：对实验失败妖×1.3
                    for (const auto& eq : player.equipment().getEquippedItems()) {
                        if (eq.id == "steel_spoon") { base_damage *= 1.3; break; }
                    }
                }
                int dmg = std::max(1, (int)(base_damage * (1.0 - (double)enemy_def_for_calc / (enemy_def_for_calc + 120.0))));
                // S3统计：攻击实验失败妖次数
                if (game_state_ && enemy.name().find("实验失败妖") != std::string::npos) {
                    game_state_->failed_experiment_attack_count++;
                }
                bool guaranteed = player.attr().hasStatus(StatusEffect::FOCUS);
                if (guaranteed || rollHit(pa.getEffectiveSPD(), ea.spd)) {
                    ea.hp -= dmg; 
                    L << "你命中，造成 " << dmg << " 伤害。\n";
                    if (guaranteed) {
                        player.attr().removeStatus(StatusEffect::FOCUS);
                    }
                    // 二手吉他：30%几率获得鼓舞2回合
                    if (player.equipment().hasEffect("on_attack_inspiration")) {
                        int chance = (int)(player.equipment().getEffectValue("on_attack_inspiration") * 100);
                        if (rollPercent() <= chance) {
                            player.attr().addStatus(StatusEffect::INSPIRATION, 2);
                            L << "音乐激励了你，你获得了鼓舞！\n";
                        }
                    }
                } else {
                    L << "你的攻击落空了。\n";
                }
            }
            
            // 敌人行动
            if (ea.hp > 0) {
                int enemy_atk_for_calc = ea.atk;
                double atkMul = (tier==0?0.9:(tier==2?1.1:1.0));
                enemy_atk_for_calc = (int)std::round(enemy_atk_for_calc * atkMul);
                if (enemy.name().find("答辩紧张魔") != std::string::npos && ea.hp * 2 < ea.max_hp) {
                    enemy_atk_for_calc = (int)(enemy_atk_for_calc * 1.3);
                }
                int dmg = calculatePhysicalDamage(enemy_atk_for_calc, pa.getEffectiveDEF());
                bool enemyHit = rollHit(ea.spd, pa.getEffectiveSPD());
                if (enemyHit && player.equipment().hasEffect("extra_evasion")) {
                    if (rollPercent() <= (int)(player.equipment().getEffectValue("extra_evasion") * 100)) {
                        enemyHit = false; // 额外闪避
                    }
                }
                if (enemyHit) {
                    pa.hp -= dmg; 
                    L << enemy.name() << " 命中，造成 " << dmg << " 伤害。\n";
                    
                    // 检查怪物特殊技能
                    {
                        int slowChance = (tier==0?30:(tier==2?70:50));
                        if (enemy.hasSlowSkill() && rollPercent() <= slowChance) {
                            player.attr().addStatus(StatusEffect::SLOW, 2);
                            L << "【迟缓攻击】你被施加了迟缓状态！\n";
                        }
                    }
                    // 答辩紧张魔：每回合60%施加紧张
                    if (enemy.name().find("答辩紧张魔") != std::string::npos) {
                        int tensionChance = (tier==2?70:60);
                        if (rollPercent() <= tensionChance) {
                            player.attr().addStatus(StatusEffect::TENSION, 3);
                            L << "【紧张施压】你被施加了紧张状态！\n";
                        }
                    } else if (enemy.hasTensionSkill()) {
                        int tChance = (tier==0?20:(tier==2?60:40));
                        if (rollPercent() <= tChance) {
                            player.attr().addStatus(StatusEffect::TENSION, 3);
                            L << "【紧张施压】你被施加了紧张状态！\n";
                        }
                    }
                // 小怪群追加攻击
                if (is_failed_revive && summoned_minions > 0 && pa.hp > 0) {
                    int total = 0;
                    for (int i=0;i<summoned_minions;i++) {
                        int minionAtk = (tier==0?18:(tier==2?22:20));
                        int md = calculatePhysicalDamage(minionAtk, pa.getEffectiveDEF());
                        // 钢勺护符降低小怪伤害
                        for (const auto& eq : player.equipment().getEquippedItems()) {
                            if (eq.id == "steel_spoon") { md = (int)(md * 0.7); break; }
                        }
                        total += md;
                    }
                    if (total>0) {
                        pa.hp -= total;
                        L << "【小怪群】失败实验体群造成追加伤害 " << total << "。\n";
                    }
                }
                } else {
                    L << enemy.name() << " 攻击落空了。\n";
                }
            }
        } else {
            // 敌人先行动
            if (ea.hp > 0) {
                int enemy_atk_for_calc = ea.atk;
                double atkMul = (tier==0?0.9:(tier==2?1.1:1.0));
                enemy_atk_for_calc = (int)std::round(enemy_atk_for_calc * atkMul);
                int dmg = calculatePhysicalDamage(enemy_atk_for_calc, pa.getEffectiveDEF());
                bool enemyHit = rollHit(ea.spd, pa.getEffectiveSPD());
                if (enemyHit && player.equipment().hasEffect("extra_evasion")) {
                    if (rollPercent() <= (int)(player.equipment().getEffectValue("extra_evasion") * 100)) {
                        enemyHit = false;
                    }
                }
                if (enemyHit) {
                    pa.hp -= dmg; 
                    L << enemy.name() << " 命中，造成 " << dmg << " 伤害。\n";
                    
                    // 检查怪物特殊技能
                    {
                        int slowChance = (tier==0?30:(tier==2?70:50));
                        if (enemy.hasSlowSkill() && rollPercent() <= slowChance) {
                            player.attr().addStatus(StatusEffect::SLOW, 2);
                            L << "【迟缓攻击】你被施加了迟缓状态！\n";
                        }
                    }
                    if (enemy.hasTensionSkill()) {
                        int tChance = (tier==0?20:(tier==2?60:40));
                        if (rollPercent() <= tChance) {
                            player.attr().addStatus(StatusEffect::TENSION, 3);
                            L << "【紧张施压】你被施加了紧张状态！\n";
                        }
                    }
                } else {
                    L << enemy.name() << " 攻击落空了。\n";
                }
            }
            
            // 玩家行动
            if (pa.hp > 0) {
                int enemy_def_for_calc2 = enemy.attr().def_;
                if (reading_buff_turns > 0) enemy_def_for_calc2 = (int)(enemy_def_for_calc2 * 1.5);
                double rf2 = rollRandomFactor();
                double base_damage2 = player.attr().getEffectiveATK() * rf2;
                if (enemy.name().find("实验失败妖") != std::string::npos) {
                    for (const auto& eq : player.equipment().getEquippedItems()) {
                        if (eq.id == "steel_spoon") { base_damage2 *= 1.3; break; }
                    }
                }
                int dmg = std::max(1, (int)(base_damage2 * (1.0 - (double)enemy_def_for_calc2 / (enemy_def_for_calc2 + 120.0))));
                bool guaranteed = player.attr().hasStatus(StatusEffect::FOCUS);
                if (guaranteed || rollHit(pa.getEffectiveSPD(), ea.spd)) {
                    ea.hp -= dmg; 
                    L << "你命中，造成 " << dmg << " 伤害。\n";
                    if (guaranteed) {
                        player.attr().removeStatus(StatusEffect::FOCUS);
                    }
                    if (player.equipment().hasEffect("on_attack_inspiration")) {
                        int chance = (int)(player.equipment().getEffectValue("on_attack_inspiration") * 100);
                        if (rollPercent() <= chance) {
                            player.attr().addStatus(StatusEffect::INSPIRATION, 2);
                            L << "音乐激励了你，你获得了鼓舞！\n";
                        }
                    }
                } else {
                    L << "你的攻击落空了。\n";
                }
            }
        }
        if (reading_buff_turns > 0) reading_buff_turns -= 1;
    }
    
    if (pa.hp <= 0) { 
        L << "\n你被击败了……\n"; 
        if (game_state_ && game_state_->current_loc == std::string("wenxintan")) {
            game_state_->wenxintan_fail_streak++;
            int s = game_state_->wenxintan_fail_streak;
            if (s == 1) {
                L << "水镜微颤，似在告诫：不必急于求成，先在教学区积累实力再来。\n";
            } else if (s == 2) {
                L << "你的倒影摇曳。心魔未除，学业未稳——再取两件\x1b[34m硕士\x1b[0m品质装备或提升等级吧。\n";
            } else if (s >= 3) {
                L << "秘境的波光渐冷。它在期待你以更完善的准备归来。\n";
            }
        }
        log = L.str(); 
        return false; 
    }
    
    L << "\n你击败了 " << enemy.name() << "！\n";
    
    // S3统计：击败实验失败妖数量
    if (game_state_ && enemy.name().find("实验失败妖") != std::string::npos) {
        game_state_->failed_experiment_kill_count++;
    }
    log = L.str(); 
    return true;
}

// 处理玩家行动
bool CombatSystem::processPlayerAction(Player& player, Enemy& enemy, CombatAction action, 
                                     const std::string& target, std::string& log) {
    std::ostringstream L;
    
    switch (action) {
        case CombatAction::ATTACK: {
            int dmg = calculatePhysicalDamage(player, enemy);
            bool guaranteed = player.attr().hasStatus(StatusEffect::FOCUS);
            if (guaranteed || rollHit(player.attr().getEffectiveSPD(), enemy.attr().spd)) {
                enemy.attr().hp -= dmg;
                L << "你命中，造成 " << dmg << " 伤害。\n";
                if (guaranteed) {
                    player.attr().removeStatus(StatusEffect::FOCUS);
                }
            } else {
                L << "你的攻击落空了。\n";
            }
            break;
        }
        case CombatAction::SKILL: {
            if (!useSkill(player, enemy, target, log)) {
                L << "技能使用失败。\n";
            }
            break;
        }
        case CombatAction::INVENTORY: {
            if (player.useItem(target)) {
                L << "你使用了 " << target << "。\n";
            } else {
                L << "物品使用失败。\n";
            }
            break;
        }
        case CombatAction::DEFEND: {
            // 防御状态会在状态系统中处理
            L << "你摆出防御姿态。\n";
            break;
        }
        case CombatAction::FLEE: {
            if (attemptFlee(player, enemy)) {
                L << "你成功逃脱了！\n";
                log = L.str();
                return true; // 逃跑成功
            } else {
                L << "逃跑失败！\n";
            }
            break;
        }
        default:
            L << "无效的行动。\n";
            break;
    }
    
    log = L.str();
    return false;
}

// 伤害计算
int CombatSystem::calculatePhysicalDamage(int atk, int def, double random_factor) {
    double base = atk * random_factor;
    double mitigation = base * (1.0 - (double)def / (def + 120.0));
    return std::max(1, (int)mitigation);
}

int CombatSystem::calculatePhysicalDamage(const Player& player, const Enemy& enemy, double random_factor) {
    int base_atk = player.attr().getEffectiveATK();
    int enemy_def = enemy.attr().def_;
    
    // 计算基础伤害
    double base_damage = base_atk * random_factor;
    
    // 应用装备特殊效果
    float damage_multiplier = player.equipment().getEffectValue("damage_multiplier", enemy.name());
    if (damage_multiplier > 1.0f) {
        base_damage *= damage_multiplier;
    }
    
    // 专注状态对高数难题精的伤害加成
    if (player.attr().hasStatus(StatusEffect::FOCUS) && enemy.name() == "高数难题精") {
        base_damage *= 1.25; // +25%伤害
    }
    
    // 计算防御减免
    double mitigation = base_damage * (1.0 - (double)enemy_def / (enemy_def + 120.0));
    
    return std::max(1, (int)mitigation);
}

// 技能伤害计算
int CombatSystem::calculateSkillDamage(const Skill& skill, const Player& player, const Enemy& enemy) {
    double base = player.attr().getEffectiveATK() * rollRandomFactor();
    double mitigation = base * (1.0 - (double)enemy.attr().def_ / (enemy.attr().def_ + 120.0));
    return std::max(1, (int)mitigation);
}

// 逃跑判定
bool CombatSystem::attemptFlee(const Player& player, const Enemy& enemy) {
    int player_spd = player.attr().getEffectiveSPD();
    int enemy_spd = enemy.attr().spd;
    double escape_chance = clamp(0.4 + (player_spd - enemy_spd) / 100.0, 0.1, 0.9);
    return rollPercent() <= (int)(escape_chance * 100);
}

// 更新战斗状态
int CombatSystem::updateCombatStatuses(Player& player, Enemy& enemy) {
    // 回合开始状态维护
    int heal_amount = 0;
    // 每回合回复（被子）
    if (player.equipment().hasEffect("per_turn_heal_percent")) {
        float p = player.equipment().getEffectValue("per_turn_heal_percent");
        int heal = std::max(1, (int)(player.attr().max_hp * p));
        int old_hp = player.attr().hp;
        player.attr().hp = std::min(player.attr().max_hp, player.attr().hp + heal);
        heal_amount = player.attr().hp - old_hp;
    }
    // 周期护盾（灵能护甲）：每3回合赋予1回合护盾
    static int turnCounter = 0;
    turnCounter++;
    if (player.equipment().hasEffect("periodic_shield") && turnCounter % 3 == 0) {
        player.attr().addStatus(StatusEffect::SHIELD, 1);
    }
    return heal_amount;
}

// 初始化技能
void CombatSystem::initializeSkills() {
    // 知识窃取技能
    Skill knowledge_theft;
    knowledge_theft.name = "知识窃取";
    knowledge_theft.description = "Base = ATK × Rand(0.8,0.9)，防御参数 = DEF+140，吸血50%。";
    knowledge_theft.mp_cost = 20;
    knowledge_theft.cooldown = 3;
    knowledge_theft.current_cooldown = 0;
    knowledge_theft.effect = [this](Player& player, Enemy& enemy, std::string& log) {
        skillKnowledgeTheft(player, enemy, log);
    };
    skills_["知识窃取"] = knowledge_theft;
    
    // Deadline冲刺技能
    Skill deadline_rush;
    deadline_rush.name = "Deadline冲刺";
    deadline_rush.description = "Base = ATK × Rand(0.9,1.3)，防御参数 = DEF+130，获得2回合连续行动但附带疲惫。";
    deadline_rush.mp_cost = 30;
    deadline_rush.cooldown = 5;
    deadline_rush.current_cooldown = 0;
    deadline_rush.effect = [this](Player& player, Enemy& enemy, std::string& log) {
        skillDeadlineRush(player, enemy, log);
    };
    skills_["Deadline冲刺"] = deadline_rush;
    
    // 代码审查技能
    Skill code_review;
    code_review.name = "代码审查";
    code_review.description = "仔细检查敌人的弱点，造成额外伤害。";
    code_review.mp_cost = 15;
    code_review.cooldown = 2;
    code_review.current_cooldown = 0;
    code_review.effect = [this](Player& player, Enemy& enemy, std::string& log) {
        skillCodeReview(player, enemy, log);
    };
    skills_["代码审查"] = code_review;
    
    // 调试模式技能
    Skill debug_mode;
    debug_mode.name = "调试模式";
    debug_mode.description = "进入调试状态，提升防御力。";
    debug_mode.mp_cost = 25;
    debug_mode.cooldown = 4;
    debug_mode.current_cooldown = 0;
    debug_mode.effect = [this](Player& player, Enemy& enemy, std::string& log) {
        skillDebugMode(player, enemy, log);
    };
    skills_["调试模式"] = debug_mode;
    
    // 算法优化技能
    Skill algorithm_optimization;
    algorithm_optimization.name = "算法优化";
    algorithm_optimization.description = "优化攻击算法，造成高额伤害。";
    algorithm_optimization.mp_cost = 40;
    algorithm_optimization.cooldown = 6;
    algorithm_optimization.current_cooldown = 0;
    algorithm_optimization.effect = [this](Player& player, Enemy& enemy, std::string& log) {
        skillAlgorithmOptimization(player, enemy, log);
    };
    skills_["算法优化"] = algorithm_optimization;
}

// 添加技能
void CombatSystem::addSkill(const std::string& skill_name, const Skill& skill) {
    skills_[skill_name] = skill;
}

// 获取可用技能
std::vector<Skill> CombatSystem::getAvailableSkills(const Player& player) const {
    std::vector<Skill> available;
    for (const auto& pair : skills_) {
        const Skill& skill = pair.second;
        if (skill.current_cooldown == 0) {
            available.push_back(skill);
        }
    }
    return available;
}

// 使用技能
bool CombatSystem::useSkill(Player& player, Enemy& enemy, const std::string& skill_name, std::string& log) {
    auto it = skills_.find(skill_name);
    if (it == skills_.end()) {
        log = "技能不存在。\n";
        return false;
    }
    
    Skill& skill = it->second;
    if (skill.current_cooldown > 0) {
        log = "技能还在冷却中。\n";
        return false;
    }
    
    // 执行技能效果
    skill.effect(player, enemy, log);
    
    // 设置冷却
    skill.current_cooldown = skill.cooldown;
    
    return true;
}

// 技能实现
void CombatSystem::skillKnowledgeTheft(Player& player, Enemy& enemy, std::string& log) {
    std::uniform_real_distribution<double> d(0.8, 0.9);
    double random_factor = d(rng_);
    
    double base = player.attr().getEffectiveATK() * random_factor;
    double mitigation = base * (1.0 - (double)enemy.attr().def_ / (enemy.attr().def_ + 140.0));
    int damage = std::max(1, (int)mitigation);
    
    enemy.attr().hp -= damage;
    
    // 吸血效果
    int heal = damage / 2;
    player.attr().hp = std::min(player.attr().max_hp, player.attr().hp + heal);
    
    log += "你使用知识窃取，造成 " + std::to_string(damage) + " 伤害，并恢复 " + std::to_string(heal) + " 生命值。\n";
}

void CombatSystem::skillDeadlineRush(Player& player, Enemy& enemy, std::string& log) {
    std::uniform_real_distribution<double> d(0.9, 1.3);
    double random_factor = d(rng_);
    
    double base = player.attr().getEffectiveATK() * random_factor;
    double mitigation = base * (1.0 - (double)enemy.attr().def_ / (enemy.attr().def_ + 130.0));
    int damage = std::max(1, (int)mitigation);
    
    enemy.attr().hp -= damage;
    
    log += "你使用Deadline冲刺，造成 " + std::to_string(damage) + " 伤害！\n";
    log += "你获得了2回合连续行动，但会感到疲惫。\n";
}

void CombatSystem::skillCodeReview(Player& player, Enemy& enemy, std::string& log) {
    int damage = calculatePhysicalDamage(player.attr().getEffectiveATK(), enemy.attr().def_, 1.2);
    enemy.attr().hp -= damage;
    
    log += "你使用代码审查，仔细分析了敌人的弱点，造成 " + std::to_string(damage) + " 伤害。\n";
}

void CombatSystem::skillDebugMode(Player& player, Enemy& enemy, std::string& log) {
    // 提升防御力
    log += "你进入调试模式，防御力暂时提升。\n";
}

void CombatSystem::skillAlgorithmOptimization(Player& player, Enemy& enemy, std::string& log) {
    int damage = calculatePhysicalDamage(player.attr().getEffectiveATK(), enemy.attr().def_, 1.5);
    enemy.attr().hp -= damage;
    
    log += "你使用算法优化，攻击效率大幅提升，造成 " + std::to_string(damage) + " 伤害！\n";
}

} // namespace hx
