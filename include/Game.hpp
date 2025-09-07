#pragma once
#include <string>
#include "GameState.hpp"
#include "Map.hpp"
#include "Player.hpp"
#include "Quest.hpp"
#include "Combat.hpp"
#include "Command.hpp"

namespace hx {
class Game {
public:
    Game();
    void run();
    GameState& state() { return state_; }
    CombatSystem& combat() { return combat_; }
    
    // NPC对话初始化（用于测试和修复）
    void initializeNPCDialogues(); // 为现有NPC添加对话内容
private:
    GameState state_{};
    CombatSystem combat_{};
    CommandRouter router_{};
    bool in_teaching_detail_ = false; // 标记是否在教学区详细地图中
    
    void setupWorld();
    void createLocations();
    void createNPCs();
    void createItems();
    void createTasks();
    void printBanner() const;
    void look() const;
    void move(const std::string& label);
    void talk(const std::string& npc_name);
    void switchToTeachingDetail(); // 切换到教学区详细地图
    void switchToMainMap(); // 切换回主地图
    void renderTeachingDetailMap() const; // 渲染教学区详细地图
    void renderMainMap() const; // 渲染主地图
    void handlePlayerDeath(); // 处理玩家死亡逻辑
    void openShop(const std::string& npc_name); // 打开商店
    void showQuest(const std::string& npc_name); // 显示任务
    void handleSpecialRewards(const std::string& npc_name, const std::string& dialogue_id, 
                             int choice, const NPC* npc); // 处理特殊奖励
    void showOpeningStory(); // 显示开场剧情
    void processEnemyDrops(const Enemy& enemy); // 处理敌人掉落物品
    void showContextualHelp(); // 显示上下文相关帮助
    void showSmartActions() const; // 显示智能操作提示
    void showAtmosphereDescription(const std::string& locationId) const; // 显示氛围描述
    void showEnhancedOperations() const; // 显示增强版操作指南
    void renderEnhancedMainMap() const; // 渲染增强版主地图
    void renderEnhancedTeachingDetailMap() const; // 渲染增强版教学区地图
    void printCombatSummary(const Enemy& enemy, int old_xp, int old_coins, int old_level); // 打印战斗小结
    void handleCombatVictory(const Enemy& enemy, int old_xp, int old_coins, int old_level); // 统一的战斗胜利处理
    void triggerChapter4Transition(); // 触发第四章过渡界面
    void talkAuto(); // 无参数对话：弹出可选NPC菜单/模糊匹配
    void fightAuto(); // 无参数战斗：列出敌人并选择
    void equipAuto(); // 无参数装备：列出可装备物品
    void unequipAuto(); // 无参数卸下：列出可卸下槽位
    
    // 怪物管理系统
    void initializeMonsterSpawns(); // 初始化怪物刷新信息
    void updateMonsterSpawns(); // 更新怪物刷新状态
    bool canSpawnMonster(const std::string& location_id, const std::string& monster_name) const; // 检查是否可以生成怪物
    int getAvailableMonsterCount(const std::string& location_id, const std::string& monster_name) const; // 获取可用怪物数量
    void onMonsterDefeated(const std::string& location_id, const std::string& monster_name); // 怪物被击败时的处理
    int calculateExperiencePenalty(const Enemy& enemy); // 计算经验值惩罚
    void showMonsterSpawnInfo() const; // 显示怪物刷新信息
    std::string formatMonsterName(const Enemy& enemy) const; // 格式化怪物显示名称
    Enemy createMonsterByName(const std::string& monster_name) const; // 根据怪物名称创建怪物实例
    
    // NPC对话初始化（私有实现）
    void initializeLinQingyiDialogues(NPC& npc); // 初始化林清漪对话
    void initializeQianDaoranDialogues(NPC& npc); // 初始化钱道然对话
    void initializeLuTianyuDialogues(NPC& npc); // 初始化陆天宇对话
    void initializeSuXiaomengDialogues(NPC& npc); // 初始化苏小萌对话
};
} // namespace hx
