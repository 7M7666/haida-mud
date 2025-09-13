// 游戏的主要文件，写游戏逻辑的地方
// 海大修仙秘这个游戏的所有主要功能都在这里

#include "Game.hpp"        // 游戏类的头文件
#include "SaveLoad.hpp"     // 存档读档功能
#include "ItemDefinitions.hpp"  // 物品定义
#include <iostream>         // 输入输出流
#include <cstdlib>          // 标准库函数
#include <algorithm>        // 算法库
#include <limits>           // 数值限制
#include <sstream>          // 字符串流
#include <numeric>          // 数值算法
#include <set>              // 集合容器

namespace hx {
// 快速创建地点的辅助函数
static Location mk(const std::string& id, const std::string& name, const std::string& desc, int x, int y) { 
    Location loc; 
    loc.id = id; 
    loc.name = name; 
    loc.desc = desc; 
    loc.coord = {x, y}; 
    return loc; 
}

// 构造函数
Game::Game() { 
    setupWorld();
    combat_.setGameState(&state_);
}

// 显示游戏标题
void Game::printBanner() const { 
    std::cout << "\n=== 海大修仙秘：文心潭秘录 ===\n"; 
    std::cout << "输入 help 查看指令。\n"; 
}

// 世界设置函数现在在GameWorld.cpp中实现

// 查看当前地点的信息
// 会显示NPC、敌人、地图等所有东西
void Game::look() const{
    // 找到当前所在的位置
    const auto* loc = state_.map.get(state_.current_loc);
    // 如果找不到位置就报错
    if(!loc){ std::cout<<"未知地点。\n"; return; }
    
    // 第四章之后就不显示位置信息了
    if (state_.chapter4_shown) {
        return;
    }
    
    // 显示地点名称和描述
    std::cout << "\n📍 【" << loc->name << "】\n";
    std::cout << loc->desc << "\n";
    
    // 显示NPC
    if(!loc->npcs.empty()){ 
        std::cout << "npc：\n";
        // 把每个NPC的名字都显示出来
        for(const auto& npc:loc->npcs) {
            std::cout << "   • " << npc.name() << "\n";
        }
    }
    
    // 显示敌人
    if(!loc->enemies.empty()){ 
        std::cout << "敌人：\n";
        // 把每个敌人都显示出来
        for(auto &en:loc->enemies) {
            // 看看这个敌人能不能打
            bool can_fight = canSpawnMonster(state_.current_loc, en.name());
            std::cout << "   • " << formatMonsterName(en);
            // 显示能不能挑战
            if (!can_fight) {
                std::cout << "（不可挑战）";
            } else {
                std::cout << "（可挑战）";
            }
            std::cout << "\n";
        }
    }
    
    // 显示可以做什么操作
    showEnhancedOperations();
    
    // 显示地图
    std::cout << "\n🗺️ 地图导航\n";
    
    // 根据位置显示不同的地图
    if(state_.in_teaching_detail) {
        // 在教学区就显示教学区地图
        renderTeachingDetailMap();
    } else {
        // 不在教学区就显示主地图
        renderMainMap();
    }
}

// 显示主地图
void Game::renderMainMap() const {
    std::cout << state_.map.renderMainMap(state_.current_loc);
}

// 显示教学区地图
void Game::renderTeachingDetailMap() const {
    std::cout << state_.map.renderTeachingDetailMap(state_.current_loc);
}

// 显示增强版主地图
void Game::renderEnhancedMainMap() const {
    std::cout << state_.map.renderEnhancedMainMap(state_.current_loc);
}

// 显示增强版教学区地图
void Game::renderEnhancedTeachingDetailMap() const {
    std::cout << state_.map.renderEnhancedTeachingDetailMap(state_.current_loc);
}

void Game::showAtmosphereDescription(const std::string& locationId) const {
    if(locationId == "library") {
        std::cout << "古老的书籍散发着墨香，静谧中仿佛能听到知识的低语。";
    } else if(locationId == "gymnasium") {
        std::cout << "空旷的场地回响着脚步声，空气中弥漫着汗水和努力的味道。";
    } else if(locationId == "canteen") {
        std::cout << "食物的香气与嘈杂的人声交织，这里是校园最有人气的地方。";
    } else if(locationId == "teaching_area") {
        std::cout << "教学楼群庄严肃穆，每一扇窗户都透出求知的渴望。";
    } else if(locationId == "wenxintan") {
        std::cout << "潭水幽深如镜，倒映着天空的云彩，神秘而宁静。";
    } else if(locationId == "jiuzhutan") {
        std::cout << "九根石柱环绕，每一根都散发着不同学科的气息。";
    } else if(locationId == "teach_5") {
        std::cout << "走廊里试卷飞舞，每一道题都化作幻影，考验着智慧。";
    } else if(locationId == "teach_7") {
        std::cout << "实验器材散落一地，失败的实验化作怪物，考验着坚韧。";
    } else if(locationId == "tree_space") {
        std::cout << "古树参天，树荫下辩论声此起彼伏，考验着表达能力。";
    } else {
        std::cout << "空气中弥漫着未知的气息，等待着探索者的到来。";
    }
}

void Game::showEnhancedOperations() const {
    const auto* loc = state_.map.get(state_.current_loc);
    if(!loc) return;
    
    std::cout << "\n🎮 操作指南\n";
    
    // 交互操作
    if(!loc->npcs.empty()) {
        std::cout << "💬 对话：\n";
        std::cout << "   📝 talk/对话 - 与NPC交谈\n";
    }
    
    if(!loc->enemies.empty()) {
        std::cout << "⚔️ 战斗：\n";
        std::cout << "   🗡️ fight/战斗 - 开始战斗\n";
    }
    
    // 系统操作
    std::cout << "📋 系统：\n";
    std::cout << "   📊 stats - 查看属性    🎒 inv - 查看背包\n";
    std::cout << "   📋 task - 查看任务     ❓ help - 帮助\n";
}

void Game::showSmartActions() const {
    const auto* loc = state_.map.get(state_.current_loc);
    if(!loc) return;
    
    std::cout << "\n🎮 操作: ";
    
    // 显示核心操作选项
    if(!loc->exits.empty()) {
        std::cout << "移动(";
        for(size_t i = 0; i < loc->exits.size(); ++i) {
            if(i > 0) std::cout << "/";
            std::cout << loc->exits[i].label;
        }
        std::cout << ") ";
    }
    
    if(!loc->npcs.empty()) {
        std::cout << "对话(talk) ";
    }
    
    if(!loc->enemies.empty()) {
        std::cout << "战斗(fight) ";
    }
    
    std::cout << "其他(stats/inv/task/help)\n";
}

void Game::handlePlayerDeath() {
    // 检查是否有复活符
    if(state_.player.hasRevivalScroll()) {
        // 使用复活符，免惩罚
        state_.player.useRevivalScroll();
        std::cout<<"【复活符】你使用了复活符，免除了死亡惩罚！\n";
        std::cout<<"【复活符】你的生命值已完全恢复。\n";
    } else {
        // 没有复活符，执行死亡惩罚
        state_.player.onDeathPenalty();
        std::cout<<"【死亡惩罚】等级-1，金币-10%，生命值已恢复。\n";
    }
    
    // 无论是否有复活符，都传送到秘境图书馆
    std::cout<<"你被传送回了秘境图书馆。\n";
    state_.current_loc = "library";
    state_.in_teaching_detail = false; // 确保回到主地图模式
    look();
}

// 处理文心潭三战后的钥匙与失败计数
static void onWenxinBossDefeated(GameState& state, const std::string& boss_name) {
    if (boss_name == "文献综述怪") state.key_i_obtained = true;
    if (boss_name == "实验失败妖·复苏") state.key_ii_obtained = true;
    if (boss_name == "答辩紧张魔·强化") state.key_iii_obtained = true;
    state.wenxintan_fail_streak = 0;
}

static void onWenxinFail(GameState& state) {
    state.wenxintan_fail_streak += 1;
}

// 美化的第四章过渡界面
void Game::triggerChapter4Transition() {
    state_.chapter4_shown = true;

    // 清屏效果
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "重大事件触发\n";
    std::cout << std::string(60, '=') << "\n\n";

    // 章节标题
    std::cout << "第四章：真相·秘境之源\n";
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";

    // 整合的剧情描述
    std::cout << "水镜觉醒\n";
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    std::cout << "三把秘钥合而为一，水面剧烈震荡，最终凝成一面清澈的水镜。历代学子的身影浮现：\n";
    std::cout << "焦灼的夜晚，灯光下的书影婆娑；堆积如山的资料，压得人喘不过气；失败后的迷茫，\n";
    std::cout << "徘徊在十字路口。你忽然明白，秘境并非囚笼，而是将无形压力化为可见心魔的'练功房'。\n";
    std::cout << "你看见自己一路走来的痕迹：面对、理解、拆解、克服。水镜最后显现《文心潭秘录》的\n";
    std::cout << "封页：如此，你已可做出最终抉择。\n\n";
    
    std::cout << "输入 'ending' 进行结局判定\n";
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";
    
    std::cout << "成就达成：文心三钥集齐者\n";
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
}

void Game::openShop(const std::string& npc_name) {
    auto* loc = state_.map.get(state_.current_loc);
    if(!loc) return;
    
    const NPC* npc = loc->findNPC(npc_name);
    if(!npc) return;
    
    // 检查是否需要刷新商店（每5个回合）或商店为空
    if (state_.shop_system.shouldRefresh(state_.turn_counter) || loc->shop.empty()) {
        // 检查是否已经刷新过（避免重复刷新）
        static int last_refresh_turn = -1;
        if (last_refresh_turn != state_.turn_counter) {
            // 刷新商店物品
            state_.shop_system.refreshShop(loc->shop, state_.turn_counter);
            last_refresh_turn = state_.turn_counter;
        }
    }
    
    // 商店主循环
    while(true) {
        std::cout<<"\n"<<std::string(50,'=')<<"\n";
        std::cout<<"🛒 "<<npc_name<<" 的商店\n";
        std::cout<<std::string(50,'=')<<"\n";
        std::cout<<"金币: " << state_.player.coins() << "\n\n";
        
        if(loc->shop.empty()) {
            std::cout<<"商店暂时没有商品。\n";
            std::cout<<"\n输入 'back' 返回： ";
            std::string input;
            std::getline(std::cin, input);
            if(input == "back") return;
            continue;
        }
        
        for(size_t i = 0; i < loc->shop.size(); ++i) {
            const auto& item = loc->shop[i];
            std::string display_name = item.name;
            // 为装备类型的ShopItem着色
            if (item.type == ItemType::EQUIPMENT) {
                Item temp_item;
                temp_item.name = item.name;
                temp_item.quality = item.quality;
                temp_item.type = ItemType::EQUIPMENT;
                // 检查是否为饰品，设置正确的装备类型
                if (item.id == "steel_spoon_amulet" || item.id == "speech_words_amulet" || 
                    item.id == "goggles_amulet" || item.id == "hnu_badge_amulet" || 
                    item.id == "ecard_amulet" || item.id == "seat_all_lib_amulet") {
                    temp_item.equip_type = EquipmentType::ACCESSORY;
                }
                display_name = getColoredItemName(temp_item);
            }
            // 紧凑一行：编号 名称 [品质色] 价格/折扣/限购
            std::cout<<(i+1)<<". "<<display_name;
            std::cout<<"  - "<<item.price<<"金币";
            
            // 复活符显示剩余购买次数
            if(item.id == "revival_scroll") {
                int remaining = 2 - state_.shop_system.getRevivalScrollPurchases();
                std::cout<<" [剩余购买次数: " << remaining << "]";
            }
            
            // 若未来扩展限购字段，可在此处输出
            if(item.favor_requirement > 0) std::cout<<"  需要好感:"<<item.favor_requirement;
            std::cout<<"\n";
        }
        // 添加第5个选项：我不买了
        std::cout<<(loc->shop.size()+1)<<". 我不买了\n";
        std::cout<<"\n输入数字购买；输入 '详情 <编号>' 查看描述；'sell' 出售装备（10金币/件）： ";
        std::string input;
        std::getline(std::cin, input);
        
        if(input.rfind("详情 ",0)==0){
            try{
                int idx = std::stoi(input.substr(7));
                if(idx>0 && idx<=static_cast<int>(loc->shop.size())){
                    const auto& it = loc->shop[idx-1];
                    std::cout<<"\n"<<std::string(50,'-')<<"\n";
                    std::cout<<it.name<<"："<<it.description<<"\n";
                    std::cout<<std::string(50,'-')<<"\n";
                }
            }catch(...){ }
            continue; // 查看详情后继续商店循环
        }
        
        if(input == "sell") {
            // 出售装备：列出背包中的装备并选择出售
            auto items = state_.player.inventory().list();
            std::vector<Item> equipments;
            for (const auto& it : items) {
                if (it.type == ItemType::EQUIPMENT) equipments.push_back(it);
            }
            if (equipments.empty()) {
                std::cout<<"你没有可出售的装备。\n";
                continue;
            }
            std::cout<<"可出售的装备：\n";
            for (size_t i = 0; i < equipments.size(); ++i) {
                std::cout<< (i+1) << ". " << getColoredItemName(equipments[i]) << " x" << equipments[i].count << "\n";
            }
            std::cout<<"输入编号出售（每件10金币），或输入 'cancel' 取消：";
            std::string sellInput;
            std::getline(std::cin, sellInput);
            if (sellInput == "cancel") continue;
            try {
                int idx = std::stoi(sellInput);
                if (idx > 0 && idx <= static_cast<int>(equipments.size())) {
                    const Item& chosen = equipments[idx-1];
                    if (state_.player.inventory().remove(chosen.id, 1)) {
                        state_.player.addCoins(10);
                        std::cout<<"回收了 "<< chosen.name <<"，获得10金币。\n";
                    } else {
                        std::cout<<"出售失败。\n";
                    }
                } else {
                    std::cout<<"无效选择。\n";
                }
            } catch(...) {
                std::cout<<"无效输入。\n";
            }
            continue;
        }
        
        // 购买逻辑
        try {
            int choice = std::stoi(input);
            // 检查是否选择了"我不买了"选项
            if(choice == static_cast<int>(loc->shop.size()) + 1) {
                std::cout<<"好的，欢迎下次再来！\n";
                return;
            }
            if(choice > 0 && choice <= static_cast<int>(loc->shop.size())) {
                const auto& item = loc->shop[choice - 1];
                
                if(item.id == "revival_scroll") {
                    // 复活符最多购买2次：检查商店系统的购买次数
                    if (!state_.shop_system.canPurchaseRevivalScroll()) {
                        std::cout<<"复活符已达购买上限（2）。\n";
                        continue;
                    }
                }
                int final_price = item.price;
                if (state_.player.equipment().hasEffect("shop_discount")) {
                    final_price = std::max(1, (int)std::floor(item.price * state_.player.equipment().getEffectValue("shop_discount")));
                }
                if(state_.player.spendCoins(final_price)) {
                    // 创建物品并添加到背包
                    Item shop_item;
                    
                    // 根据物品类型创建对应的Item
                    if (item.type == ItemType::EQUIPMENT) {
                        // 装备类型，使用ShopSystem的createItem方法
                        shop_item = state_.shop_system.createItemFromId(item.id);
                        shop_item.price = final_price; // 使用最终价格（考虑折扣）
                    } else {
                        // 消耗品类型
                        shop_item.id = item.id;
                        shop_item.name = item.name;
                        shop_item.description = item.description;
                        shop_item.price = final_price;
                        shop_item.type = ItemType::CONSUMABLE;
                        
                        // 根据物品ID设置特殊属性
                        if(item.id == "health_potion") {
                            shop_item.heal_amount = 30;
                            shop_item.use_message = "使用了生命药水，恢复了30点生命值。";
                        } else if(item.id == "revival_scroll") {
                            shop_item.is_quest_item = true;
                            // 更新复活符购买次数
                            state_.shop_system.incrementRevivalScrollPurchases();
                        } else if(item.id == "caffeine_elixir") {
                            shop_item.use_message = "你饮下了咖啡因灵液。";
                        }
                    }
                    
                    state_.player.inventory().add(shop_item, 1);
                    std::cout<<"购买了 "<<item.name<<"！\n";
                } else {
                    std::cout<<"金币不足！\n";
                }
            } else {
                std::cout<<"无效选择。\n";
            }
        } catch(...) {
            std::cout<<"无效输入。\n";
        }
    } // 结束商店主循环
}

void Game::showQuest(const std::string& npc_name) {
    auto* loc = state_.map.get(state_.current_loc);
    if(!loc) return;
    
    const NPC* npc = loc->findNPC(npc_name);
    if(!npc || !npc->hasQuest()) return;
    
    std::cout<<"\n=== "<<npc_name<<" 的任务 ===\n";
    std::cout<<"任务ID: "<<npc->getQuestId()<<"\n";
    
    // 这里可以根据任务ID显示具体的任务信息
    // 暂时显示通用信息
    std::cout<<"任务详情请与NPC对话了解。\n";
}

void Game::handleSpecialRewards(const std::string& npc_name, const std::string& dialogue_id, 
                               int choice, const NPC* npc) {
    // 钱道然的商店访问已移至对话系统主逻辑中处理
    
    // 林清漪的特殊处理
    if(npc_name == "林清漪") {
        // 装备奖励
        if(dialogue_id == "equipment" && choice == 1) {
            if(!npc->hasGivenReward()) {
                // 给予初始装备
                Item student_uniform = Item::createEquipment("student_uniform", "普通学子服(DEF+5,HP+15)", 
                    "海大计算机学院的标准校服，虽然普通但很实用。", 
                    EquipmentType::ARMOR, EquipmentSlot::ARMOR, 0, 5, 0, 15, 50);
                Item bamboo_notes = Item::createEquipment("bamboo_notes", "竹简笔记(ATK+5,SPD+2)", 
                    "古老的竹简，记载着学习心得。", 
                    EquipmentType::WEAPON, EquipmentSlot::WEAPON, 5, 0, 2, 0, 30);
                
                state_.player.inventory().add(student_uniform, 1);
                state_.player.inventory().add(bamboo_notes, 1);
                std::cout<<"【获得：普通学子服(DEF+5,HP+15) x1，竹简笔记(ATK+5,SPD+2) x1】\n";
                
                // 第一次给装备时增加好感度
                state_.player.addNPCFavor(npc_name, 10);
                std::cout<<"【好感度 +10】\n";
                
                // 标记已给过奖励
                const_cast<NPC*>(npc)->setGivenReward(true);
            } else {
                std::cout<<"【林清漪】\"这些装备我已经给过你了，要好好珍惜哦。\"\n";
            }
        }
        // 第一次询问信息的好感度奖励
        else if(dialogue_id == "welcome" && choice == 1) {
            // 检查是否已经询问过信息（通过检查当前好感度是否为0）
            int current_favor = state_.player.getNPCFavor(npc_name);
            if(current_favor == 0) {
                // 第一次询问信息，增加好感度
                state_.player.addNPCFavor(npc_name, 5);
                std::cout<<"【好感度 +5】\n";
            }
        }
    }
    
    // 苏小萌的钢勺护符奖励
    if(npc_name == "苏小萌" && (dialogue_id == "spicy_choice" || dialogue_id == "rice_choice" || dialogue_id == "random_choice") && choice == 1) {
        if(!npc->hasGivenReward()) {
            Item steel_spoon = Item::createEquipment("steel_spoon", "钢勺护符", 
                "食堂选择的奖励，能提供额外保护。", 
                EquipmentType::ARMOR, EquipmentSlot::ARMOR, 0, 20, 0, 0, 80);
            steel_spoon.effect_description = "对实验楼怪物伤害 ×1.3";
            steel_spoon.effect_type = "damage_multiplier";
            steel_spoon.effect_target = "失败实验体";
            steel_spoon.effect_value = 1.3f;
            
            state_.player.inventory().add(steel_spoon, 1);
            std::cout<<"【获得：钢勺护符 x1】\n";
            
            // 标记已给过奖励
            const_cast<NPC*>(npc)->setGivenReward(true);
        } else {
            std::cout<<"【苏小萌】\"护符已经给你了，要好好使用哦！\"\n";
        }
    }
    
    // 陆天宇的负重护腕奖励
    if(npc_name == "陆天宇" && dialogue_id == "accept_quest" && choice == 1) {
        if(!npc->hasGivenReward()) {
            // 给予负重护腕
            Item weight_bracelet = Item::createEquipment("weight_bracelet", "负重护腕", 
                "毅力试炼的奖励，能增强体魄。", 
                EquipmentType::ACCESSORY, EquipmentSlot::ACCESSORY2, 0, 10, 0, 5, 90);
            weight_bracelet.description = "毅力试炼的奖励，能增强体魄。戴上它，你感觉自己的力量增加了。";
            
            state_.player.inventory().add(weight_bracelet, 1);
            std::cout<<"【获得：负重护腕 x1】\n";
            
            // 标记已给过奖励
            const_cast<NPC*>(npc)->setGivenReward(true);
        } else {
            std::cout<<"【陆天宇】\"护腕已经给你了，要好好使用哦！\"\n";
        }
    }
}

void Game::showOpeningStory() {
    // 清屏效果和游戏标题
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "海大修仙秘：文心潭秘录\n";
    std::cout << std::string(60, '=') << "\n\n";

    // 章节标题
    std::cout << "第一章：缘起·古籍现世\n";
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";

    // 场景描述
    std::cout << "场景：海大图书馆古籍区\n";
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";

    // 剧情描述
    std::cout << "期末考试周，你为了准备《海洋文化概论》的论文，来到图书馆古籍区查找资料。\n\n";

    std::cout << "在书架角落，你意外发现一本蓝色封皮、线装订的古籍——《文心潭秘录》。\n\n";

    std::cout << "当你翻开书页时，书中突然散发出柔和的光芒，周围的景象开始扭曲变化……\n\n";

    // 交互提示
    std::cout << "输入 '翻阅古籍' 继续...\n";
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    
    std::string input;
    while(true) {
        std::cout << "\n> ";
        std::getline(std::cin, input);
        if(input == "翻阅古籍") {
            break;
        } else {
            std::cout << "请输入 '翻阅古籍' 继续剧情。\n";
        }
    }
    
    // 章节标题
    std::cout << "第二章：初识·秘境指引\n";
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";

    // 场景描述
    std::cout << "场景：秘境图书馆\n";
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";

    // 剧情描述
    std::cout << "光芒散去，一位身着青衫的学姐出现，自称林清漪。\n";
    std::cout << "她解释这里是文心秘境，学业压力具象化为心魔，必须修炼才能找到回归现实的方法。\n\n";

    // NPC对话
    std::cout << "【林清漪】\"欢迎来到文心秘境，这里是学业与灵魂的投影。\"\n";
    std::cout << "\"你需要自由探索五个区域，击败心魔收集修为，最终挑战文心潭。\"\n";
    std::cout << "\"现在，让我为你准备一些装备吧。\"\n\n";

    // 交互提示
    std::cout << "按回车键开始你的秘境之旅...\n";
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    std::getline(std::cin, input);
    
    // 新手引导
    std::cout << "\n新手引导 - 文心秘境生存指南\n";
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";

    std::cout << "基础操作：\n";
    std::cout << "   • 输入 'look' 查看当前位置和可用操作\n";
    std::cout << "   • 输入 'help' 查看所有可用指令\n";
    std::cout << "   • 输入 'stats' 查看你的属性和等级\n";
    std::cout << "   • 输入 'inv' 查看背包中的物品\n";
    std::cout << "   • 输入 'task' 查看当前任务和目标\n\n";

    std::cout << "移动系统：\n";
    std::cout << "   • 使用 w/a/s/d 快速移动\n";
    std::cout << "   • 或输入方向名称（如'东'、'西'、'南'、'北'）\n";
    std::cout << "   • 输入 'map' 查看当前地图\n\n";

    std::cout << "对话系统：\n";
    std::cout << "   • 输入 'talk' 打开对话菜单\n";
    std::cout << "   • 或直接输入 'talk <NPC名>' 与特定NPC对话\n";
    std::cout << "   • 每个NPC都有独特的背景故事和任务\n\n";

    std::cout << "⚔️ 战斗系统：\n";
    std::cout << "   • 输入 'fight' 打开战斗菜单\n";
    std::cout << "   • 或直接输入 'fight <敌人名>' 挑战特定敌人\n";
    std::cout << "   • 击败敌人获得经验值、金币和装备\n\n";

    std::cout << "游戏目标：\n";
    std::cout << "   • 完成三个试炼：智力、毅力、表达\n";
    std::cout << "   • 提升等级至9级以上\n";
    std::cout << "   • 收集硕士品质装备\n";
    std::cout << "   • 挑战文心潭最终试炼\n";
    std::cout << "   • 集齐三把文心秘钥揭开真相\n\n";

    std::cout << "重要提示：\n";
    std::cout << "   • 与林清漪对话了解游戏背景和获得新手装备\n";
    std::cout << "   • 在钱道然那里购买生命药水和复活符\n";
    std::cout << "   • 完成苏小萌和陆天宇的任务获得特殊装备\n";
    std::cout << "   • 输入 'help' 可随时查看指令帮助\n";
    std::cout << "   • 死亡有惩罚，建议购买复活符\n\n";

    std::cout << "开始你的秘境之旅吧！\n";
    std::cout << std::string(60, '=') << "\n\n";
    
    // 自动进入游戏主循环
    look();
}

void Game::printCombatSummary(const Enemy& enemy, int old_xp, int old_coins, int old_level) {
    std::cout << "\n" << std::string(40, '=') << "\n";
    std::cout << "⚔️ 战斗小结\n";
    std::cout << std::string(40, '=') << "\n";
    
    // 当前血量显示
    std::cout << "❤️ 当前血量: " << state_.player.attr().hp << "/" << state_.player.attr().max_hp << "\n";
    
    // 经验值和升级进度
    std::cout << "📈 经验值: " << state_.player.xp() << " (还需 " << state_.player.getXPNeededForNextLevel() << " 升级)\n";
    
    // 等级变化
    if (state_.player.level() > old_level) {
        std::cout << "⭐ 等级提升: " << old_level << " → " << state_.player.level() << "\n";
    }
    
    // 任务进度
    std::cout << "📋 任务进度: ";
    bool has_task_progress = false;
    if (state_.task_manager.hasActiveTask("S2_动力碎片")) {
        int qty = state_.player.inventory().quantity("power_fragment");
        std::cout << "动力碎片 " << qty << "/3";
        has_task_progress = true;
    }
    if (state_.task_manager.hasActiveTask("S3_实验失败妖")) {
        if (has_task_progress) std::cout << ", ";
        std::cout << "实验失败妖 " << state_.failed_experiment_kill_count << "/11";
        has_task_progress = true;
    }
    if (!has_task_progress) std::cout << "无活跃任务";
    std::cout << "\n";
    
    
    std::cout << std::string(40, '=') << "\n\n";
}

void Game::handleCombatVictory(const Enemy& enemy, int old_xp, int old_coins, int old_level) {
    // 战斗结束后清除所有负面状态
    state_.player.attr().removeStatus(StatusEffect::TENSION);
    state_.player.attr().removeStatus(StatusEffect::SLOW);
    std::cout << "【状态清除】战斗结束，所有负面状态已清除。\n";
    
    // 计算经验值惩罚
    int exp_penalty = calculateExperiencePenalty(enemy);
    int actual_xp = (enemy.xpReward() * exp_penalty) / 100;
    
    // 战斗胜利奖励（应用经验值惩罚）
    state_.player.addXP(actual_xp);
    state_.player.addCoins(enemy.coinReward());
    std::cout<<"获得经验值"<<actual_xp<<"，金币"<<enemy.coinReward()<<"。\n";
    
    // 增加回合计数器
    state_.turn_counter++;
    
    // 显示经验值奖励/惩罚信息
    if (exp_penalty > 100) {
        std::cout << "【越级奖励】由于挑战高等级怪物，获得经验值增加至 " << exp_penalty << "%\n";
    } else if (exp_penalty < 100) {
        std::cout << "【等级惩罚】由于挑战低等级怪物，获得经验值减少至 " << exp_penalty << "%\n";
    }
    
    // 怪物被击败，更新刷新状态
    onMonsterDefeated(state_.current_loc, enemy.name());
    
    // 处理掉落物品
    processEnemyDrops(enemy);
    
    // 打印战斗小结
    printCombatSummary(enemy, old_xp, old_coins, old_level);
    
    // 若有S2任务，实时更新进度
    if (state_.task_manager.hasActiveTask("S2_动力碎片")) {
        int qty = state_.player.inventory().quantity("power_fragment");
        if (auto* tk = state_.task_manager.getTask("S2_动力碎片")) {
            tk->setObjective(0, std::string("收集动力碎片：") + std::to_string(qty) + "/3");
            std::cout<<"【任务进度】动力碎片："<<qty<<"/3\n";
        }
    }

    // 如果在文心潭并击败三战之一，标记钥匙
    if (state_.current_loc == "wenxintan") {
        onWenxinBossDefeated(state_, enemy.name());
        // 集齐三把钥匙后一次性通关奖励和第四章触发
        if (!state_.truth_reward_given && state_.key_i_obtained && state_.key_ii_obtained && state_.key_iii_obtained) {
            state_.player.addXP(300);
            state_.player.addCoins(200);
            state_.truth_reward_given = true;

            // 延迟一下，让玩家看到奖励信息
            std::cout<<"【通关奖励】获得大量经验300与金币200！\n";
            std::cout<<"【称号】获得称号：荣誉博士！\n";
            std::cout<<"恭喜通关！！\n\n";

            // 自动触发第四章
            triggerChapter4Transition();
        }
    }

    // S3任务进度：若为实验失败妖，更新任务与进度显示
    if (enemy.name().find("实验失败妖") != std::string::npos) {
        // 更新新任务系统的进度
        if (auto* tk = state_.task_manager.getTask("side_lab_challenge")) {
            if (tk->getStatus() == TaskStatus::IN_PROGRESS) {
                // 更新任务目标：击败实验失败妖
                tk->setObjective(2, "击败实验失败妖 ✓");
            }
        }
        std::cout << "【任务进度】实验失败妖：击败 " << state_.failed_experiment_kill_count << "/11，ATK " << state_.player.attr().getEffectiveATK() << "（需≥30）\n";
    }

    // 首次击败高数难题精奖励：给予启智笔
    if (enemy.name().find("高数难题精") != std::string::npos && !state_.math_difficulty_spirit_first_kill) {
        state_.math_difficulty_spirit_first_kill = true;
        Item wisdom_pen = Item::createEquipment("wisdom_pen", "启智笔", 
            "智力试炼的奖励，能提升思维敏捷度。", 
            EquipmentType::WEAPON, EquipmentSlot::WEAPON, 6, 0, 0, 0, 120);
        wisdom_pen.quality = EquipmentQuality::MASTER;
        wisdom_pen.effect_type = "damage_multiplier";
        wisdom_pen.effect_target = "difficult_problem";
        wisdom_pen.effect_value = 1.15f;
        wisdom_pen.effect_description = "对难题类敌人额外造成15%伤害";
        state_.player.inventory().add(wisdom_pen, 1);
        std::cout << "【智力试炼完成】首次击败高数难题精！获得启智笔×1！\n";
        std::cout << "【装备效果】启智笔：ATK+6，对难题类敌人额外造成15%伤害\n";
    }

    // S3奖励判定：击败实验失败妖>10且ATK≥30且未发放
    if (!state_.s3_reward_given && state_.failed_experiment_kill_count>10 && state_.player.attr().getEffectiveATK()>=30) {
        state_.s3_reward_given = true;
        Item goggles = Item::createEquipment("goggles","护目镜","视野更清晰，行动更敏捷。",EquipmentType::ACCESSORY,EquipmentSlot::ACCESSORY2,0,0,6,0,0);
        state_.player.inventory().add(goggles,1);
        state_.player.addNPCFavor("林清漪",20);
        std::cout<<"【S3完成】长期与实验失败妖交手让你收获良多。获得护目镜×1。林清漪好感+20。\n";
        
        // 完成新任务系统的任务
        if (auto* tk = state_.task_manager.getTask("side_lab_challenge")) {
            tk->setObjective(3, "获得实验服 ✓");
            state_.task_manager.completeTask("side_lab_challenge");
        }
    }

    // S4奖励判定入口：树下空间Boss击败后，如持启智笔，触发答题
    if (state_.current_loc == "tree_space" && !state_.s4_reward_given && (enemy.name()=="答辩紧张魔" || enemy.name()=="答辩紧张魔·强化")) {
        // 更新新任务系统的进度
        if (auto* tk = state_.task_manager.getTask("side_debate_challenge")) {
            if (tk->getStatus() == TaskStatus::IN_PROGRESS) {
                tk->setObjective(1, "击败答辩紧张魔 ✓");
            }
        }
        
        bool has_wisdom_pen=false; auto inv = state_.player.inventory().asSimpleItems();
        for (auto &it: inv) if (it.id=="wisdom_pen" && it.count>0) { has_wisdom_pen=true; break; }
        if (has_wisdom_pen) {
            int correct=0; std::string a;
            std::cout<<"【对话挑战】请回答以下问题（3题，答对≥2通过）：\n";
            std::cout<<"1. 演讲时缓解紧张的有效方式是？\n A) 大声喝止对方\n B) 深呼吸并放慢语速\n C) 避免目光接触\n> ";
            std::getline(std::cin,a); if(a=="B"||a=="b") correct++;
            std::cout<<"2. 回答问题时最重要的是？\n A) 语速越快越好\n B) 逻辑清晰、层次分明\n C) 使用大量术语\n> ";
            std::getline(std::cin,a); if(a=="B"||a=="b") correct++;
            std::cout<<"3. 面对质疑，正确做法是？\n A) 反驳并否定\n B) 情绪化回应\n C) 接纳问题、给出证据与解释\n> ";
            std::getline(std::cin,a); if(a=="C"||a=="c") correct++;
            if (correct>=2) {
                state_.s4_reward_given = true;
                Item fan = Item::createEquipment("debate_fan","辩锋羽扇","锋芒毕露，言辞更有力。",EquipmentType::WEAPON,EquipmentSlot::WEAPON,15,0,6,0,350);
                fan.quality = EquipmentQuality::DOCTOR;
                fan.effect_type = "damage_multiplier";
                fan.effect_target = "答辩紧张魔·强化";
                fan.effect_value = 1.3f;
                fan.effect_description = "对答辩紧张魔·强化造成1.3倍伤害";
                state_.player.inventory().add(fan,1);
                state_.player.addNPCFavor("林清漪",20);
                std::cout<<"【S4完成】你顺利通过对话挑战，获得辩锋羽扇×1。林清漪好感+20。\n";
                
                // 完成新任务系统的任务
                if (auto* tk = state_.task_manager.getTask("side_debate_challenge")) {
                    tk->setObjective(2, "回答3个问题 ✓");
                    tk->setObjective(3, "答对2题以上 ✓");
                    tk->setObjective(4, "获得辩峰羽扇 ✓");
                    state_.task_manager.completeTask("side_debate_challenge");
                }
            } else {
                std::cout<<"很遗憾，本次挑战未通过，可稍后再试。\n";
            }
        }
    }
    
    // 战斗结束后显示当前位置信息（第四章之后不显示）
    if (!state_.chapter4_shown) {
        std::cout << "\n" << std::string(50, '=') << "\n";
        std::cout << "📍 当前位置信息\n";
        std::cout << std::string(50, '=') << "\n";
        
        // 显示当前位置基本信息
        auto* loc = state_.map.get(state_.current_loc);
        if (loc) {
            std::cout << "📍 位置: " << loc->name << "\n";
            std::cout << "📝 描述: " << loc->desc << "\n";
            
            // 显示可前往的地点
            if (!loc->exits.empty()) {
                std::cout << "🚪 可前往: ";
                for (size_t i = 0; i < loc->exits.size(); ++i) {
                    if (i > 0) std::cout << ", ";
                    std::cout << loc->exits[i].label;
                }
                std::cout << "\n";
            }
            
            // 显示NPC
            if (!loc->npcs.empty()) {
                std::cout << "👥 NPC: ";
                for (size_t i = 0; i < loc->npcs.size(); ++i) {
                    if (i > 0) std::cout << ", ";
                    std::cout << loc->npcs[i].name();
                }
                std::cout << "\n";
            }
            
            // 显示怪物状态 - 基于monster_spawns系统
            bool has_monsters = false;
            for (const auto& spawn : state_.monster_spawns) {
                if (spawn.location_id == state_.current_loc) {
                    if (!has_monsters) {
                        std::cout << "👹 怪物状态:\n";
                        has_monsters = true;
                    }
                    
                    // 检查是否还有挑战次数
                    int remaining_challenges = spawn.max_challenges - spawn.challenge_count;
                    bool can_fight = canSpawnMonster(state_.current_loc, spawn.monster_name);
                    
                    // 获取怪物等级用于显示
                    int monster_level = 1; // 默认等级
                    if (spawn.monster_name == "迷糊书虫") monster_level = 1;
                    else if (spawn.monster_name == "拖延小妖") monster_level = 2;
                    else if (spawn.monster_name == "水波幻影") monster_level = 3;
                    else if (spawn.monster_name == "学业焦虑影") monster_level = 4;
                    else if (spawn.monster_name == "夜行怠惰魔") monster_level = 6;
                    else if (spawn.monster_name == "压力黑雾") monster_level = 7;
                    else if (spawn.monster_name == "实验失败妖·群") monster_level = 8;
                    else if (spawn.monster_name == "高数难题精") monster_level = 9;
                    else if (spawn.monster_name == "答辩紧张魔") monster_level = 8;
                    else if (spawn.monster_name == "文献综述怪") monster_level = 12;
                    else if (spawn.monster_name == "实验失败妖·复苏") monster_level = 12;
                    else if (spawn.monster_name == "答辩紧张魔·强化") monster_level = 12;
                    
                    // 计算难度提示
                    int player_level = state_.player.level();
                    int level_diff = player_level - monster_level;
                    std::string difficulty_hint = "";
                    if (level_diff >= 3) {
                        difficulty_hint = " (轻松)";
                    } else if (level_diff >= 1) {
                        difficulty_hint = " (简单)";
                    } else if (level_diff == 0) {
                        difficulty_hint = " (相当)";
                    } else if (level_diff >= -1) {
                        difficulty_hint = " (困难)";
                    } else if (level_diff >= -3) {
                        difficulty_hint = " (危险)";
                    } else {
                        difficulty_hint = " (极危)";
                    }
                    
                    std::cout << "   • Lv" << monster_level << " " << spawn.monster_name << difficulty_hint;
                    
                    if (spawn.turns_until_respawn > 0) {
                        std::cout << " (刷新中，还需 " << spawn.turns_until_respawn << " 回合)";
                    } else if (remaining_challenges > 0) {
                        std::cout << " (剩余 " << remaining_challenges << " 次挑战)";
                    } else {
                        std::cout << " (暂时不可挑战，需等待刷新)";
                    }
                    std::cout << "\n";
                }
            }
            
            if (!has_monsters) {
                std::cout << "👹 怪物: 无\n";
            }
            
            // 显示商店刷新信息（在顶部位置信息区域）
            if (state_.shop_system.shouldRefresh(state_.turn_counter)) {
                std::cout << "\033[34m【商店刷新】钱道然的商店更新了新的货物！\033[0m\n";
            }
        }
        
        std::cout << std::string(50, '=') << "\n";
    }
}

void Game::showContextualHelp() {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "📖 文心秘境指令帮助\n";
    std::cout << std::string(60, '=') << "\n\n";
    
    // 基础指令
    std::cout << "🔹 基础操作：\n";
    std::cout << "  look - 查看当前位置和可用操作\n";
    std::cout << "  stats - 查看角色属性和等级\n";
    std::cout << "  inv - 查看背包中的物品\n";
    std::cout << "  task - 查看当前任务和目标\n";
    std::cout << "  map - 查看当前地图\n";
    std::cout << "  help - 显示此帮助信息\n\n";
    
    // 移动指令
    std::cout << "🔹 移动系统：\n";
    std::cout << "  w/a/s/d - 快速方向移动（北/西/南/东）\n";
    std::cout << "  enter - 进入教学区详细地图（仅在教学区有效）\n";
    std::cout << "  exit - 退出教学区详细地图（仅在九珠坛有效）\n\n";
    
    // 交互指令
    std::cout << "🔹 交互系统：\n";
    std::cout << "  talk/对话 - 打开对话菜单（列出本地NPC，支持数字选择）\n";
    std::cout << "  fight/战斗 - 打开战斗菜单（列出本地敌人，支持数字选择）\n";
    std::cout << "  monsters/怪物信息 - 查看怪物刷新信息\n\n";
    
    // 装备指令
    std::cout << "🔹 装备系统：\n";
    std::cout << "  equip/装备 - 打开装备菜单（列出可装备物品，支持数字选择）\n";
    std::cout << "  unequip/卸下 - 打开卸装菜单（列出已装备物品，支持数字选择）\n";
    std::cout << "  use <物品名> - 使用消耗品（如生命药水）\n\n";
    
    // 商店指令
    std::cout << "🔹 商店系统：\n";
    std::cout << "  sell - 出售装备（10金币/件）\n\n";
    
    // 属性分配
    if (state_.player.attr().available_points > 0) {
        std::cout << "🔹 属性分配：\n";
        std::cout << "  allocate <属性> [数量] - 分配属性点\n";
        std::cout << "  可用属性：hp(生命), atk(攻击), def(防御), spd(速度)\n";
        std::cout << "  示例：allocate atk 3 或 allocate hp 2\n\n";
    }
    
    // 系统指令
    std::cout << "🔹 系统指令：\n";
    std::cout << "  save - 保存游戏进度\n";
    std::cout << "  load - 加载游戏进度\n";
    std::cout << "  quit/q - 退出游戏\n\n";
    
    // 装备品质说明
    std::cout << "🔹 装备品质：\n";
    std::cout << "  \x1b[32m本科\x1b[0m - 基础装备，适合新手(ง •_•)ง\n";
    std::cout << "  \x1b[34m硕士\x1b[0m - 高级装备，需要一定实力获得(๑•̀ㅂ•́)و✧\n";
    std::cout << "  \x1b[31m博士\x1b[0m - 顶级装备，只有强者才能驾驭，或者有钱b（￣▽￣）d　\n";
    std::cout << "  \x1b[33m饰品\x1b[0m - 特殊装备，提供独特效果，显示为黄色文字(★ω★)\n\n";
    
    // 特殊帮助
    std::cout << "🔹 特殊帮助：\n";
    std::cout << "  help combat - 查看战斗系统帮助\n";
    std::cout << "  help shop - 查看商店系统帮助\n";
    std::cout << "  help task - 查看任务系统帮助\n\n";
    
    // 当前进度提示
    std::cout << "🔹 当前进度：\n";
    if (state_.player.level() < 5) {
        std::cout << "  阶段：新手阶段\n";
        std::cout << "  目标：与林清漪对话获得新手装备，完成苏小萌和陆天宇的任务\n";
        std::cout << "  建议：在体育馆击败迷糊书虫和拖延小妖练级\n";
    } else if (state_.player.level() < 9) {
        std::cout << "  阶段：进阶阶段\n";
        std::cout << "  目标：提升等级至9级，收集硕士品质装备\n";
        std::cout << "  建议：进入教学区详细地图，完成智力试炼\n";
    } else if (!state_.key_i_obtained || !state_.key_ii_obtained || !state_.key_iii_obtained) {
        std::cout << "  阶段：高级阶段\n";
        std::cout << "  目标：前往文心潭，集齐三把秘钥\n";
        std::cout << "  提示：文心潭需要等级≥9且至少两件硕士品质装备\n";
    } else {
        std::cout << "  阶段：最终阶段\n";
        std::cout << "  目标：输入 'ending' 查看结局\n";
        std::cout << "  恭喜：已完成所有试炼，可以查看结局了！\n";
    }
    
    std::cout << "\n" << std::string(60, '=') << "\n";
}

void Game::processEnemyDrops(const Enemy& enemy) {
    auto roll = [](){ return (std::rand() % 100) + 1; };
    
    // 教学区子地图专用掉落系统
    if (state_.in_teaching_detail) {
        // 清空原有掉落：不处理普通掉落物品，仅处理装备掉落
        
        // 第一类：本科级装备（掉落概率25%）
        if (roll() <= 25) {
            // 本科级武器池：仅含【吴京篮球】（属性：ATK +4，SPD +2）
            std::vector<std::string> undergrad_weapons = {"wu_jing_ball"};
            // 本科级护甲池：含2件装备，分别为【床上的被子】（属性：DEF +7，每回合恢复3%最大HP）、【普通学子服】（属性：DEF +2，HP +10）
            std::vector<std::string> undergrad_armor = {"bed_quilt", "student_uniform"};
            
            // 随机选择武器或护甲
            if (roll() <= 50) {
                // 选择武器
                std::string equip_id = undergrad_weapons[std::rand() % undergrad_weapons.size()];
                Item equip;
                if (equip_id == "wu_jing_ball") {
                    equip = ItemDefinitions::createWuJingBall();
                } else {
                    equip = Item::createEquipment(equip_id, "未知武器", "教学区掉落的装备", 
                        EquipmentType::WEAPON, EquipmentSlot::WEAPON, 0, 0, 0, 0, 0);
                    equip.quality = EquipmentQuality::UNDERGRAD;
                }
                state_.player.inventory().add(equip, 1);
                std::cout << "【掉落】获得 " << getColoredItemName(equip) << "！\n";
            } else {
                // 选择护甲
                std::string equip_id = undergrad_armor[std::rand() % undergrad_armor.size()];
                Item equip;
                if (equip_id == "bed_quilt") {
                    equip = ItemDefinitions::createBedQuilt();
                } else if (equip_id == "student_uniform") {
                    equip = Item::createEquipment(equip_id, "普通学子服", "教学区掉落的装备", 
                        EquipmentType::ARMOR, EquipmentSlot::ARMOR, 0, 2, 0, 10, 0);
                    equip.quality = EquipmentQuality::UNDERGRAD;
                } else {
                    equip = Item::createEquipment(equip_id, "未知护甲", "教学区掉落的装备", 
                        EquipmentType::ARMOR, EquipmentSlot::ARMOR, 0, 0, 0, 0, 0);
                    equip.quality = EquipmentQuality::UNDERGRAD;
                }
                state_.player.inventory().add(equip, 1);
                std::cout << "【掉落】获得 " << getColoredItemName(equip) << "！\n";
            }
        }
        // 第二类：硕士级装备（掉落概率15%，累积40%）
        else if (roll() <= 40) {
            // 硕士级武器池：含2件装备，分别为【启智笔】（属性：ATK +6，对"难题类"敌人额外造成15%伤害（乘法计算））、【二手吉他】（属性：ATK +7，攻击时30%概率触发"鼓舞"效果（自身ATK +15%，持续2回合））
            std::vector<std::string> master_weapons = {"wisdom_pen", "second_hand_guitar"};
            // 硕士级护甲池：含2件装备，分别为【学士袍】（属性：全属性 +10%）、【灵能护甲】（属性：DEF +6，HP +20，获得被动"护盾"（每3回合自动套1次30%减伤盾））
            std::vector<std::string> master_armor = {"bachelor_robe", "psionic_armor"};
            
            if (roll() <= 50) {
                // 选择武器
                std::string equip_id = master_weapons[std::rand() % master_weapons.size()];
                Item equip;
                if (equip_id == "wisdom_pen") {
                    equip = ItemDefinitions::createWisdomPen();
                } else if (equip_id == "second_hand_guitar") {
                    equip = ItemDefinitions::createSecondHandGuitar();
                } else {
                    equip = Item::createEquipment(equip_id, "未知武器", "教学区掉落的装备", 
                        EquipmentType::WEAPON, EquipmentSlot::WEAPON, 0, 0, 0, 0, 0);
                    equip.quality = EquipmentQuality::MASTER;
                }
                state_.player.inventory().add(equip, 1);
                std::cout << "【掉落】获得 " << getColoredItemName(equip) << "！\n";
            } else {
                // 选择护甲
                std::string equip_id = master_armor[std::rand() % master_armor.size()];
                Item equip;
                if (equip_id == "bachelor_robe") {
                    equip = ItemDefinitions::createBachelorRobe();
                } else if (equip_id == "psionic_armor") {
                    equip = ItemDefinitions::createPsionicArmor();
                } else {
                    equip = Item::createEquipment(equip_id, "未知护甲", "教学区掉落的装备", 
                        EquipmentType::ARMOR, EquipmentSlot::ARMOR, 0, 0, 0, 0, 0);
                    equip.quality = EquipmentQuality::MASTER;
                }
                state_.player.inventory().add(equip, 1);
                std::cout << "【掉落】获得 " << getColoredItemName(equip) << "！\n";
            }
        }
        // 第三类：博士级装备（掉落概率8%，累积48%）
        else if (roll() <= 48) {
            // 博士级武器池：仅含【博士大论文】（属性：ATK +15，SPD +2）
            std::vector<std::string> doctor_weapons = {"phd_thesis"};
            // 博士级护甲池：含2件装备，分别为【实验服】（属性：DEF +10，HP +35）、【负重护腕】（属性：ATK +3，DEF +9，HP +30）
            std::vector<std::string> doctor_armor = {"lab_coat", "weight_bracelet"};
            
            if (roll() <= 50) {
                // 选择武器
                std::string equip_id = doctor_weapons[std::rand() % doctor_weapons.size()];
                Item equip;
                if (equip_id == "phd_thesis") {
                    equip = ItemDefinitions::createPhdThesis();
                } else {
                    equip = Item::createEquipment(equip_id, "未知武器", "教学区掉落的装备", 
                        EquipmentType::WEAPON, EquipmentSlot::WEAPON, 0, 0, 0, 0, 0);
                    equip.quality = EquipmentQuality::DOCTOR;
                }
                state_.player.inventory().add(equip, 1);
                std::cout << "【掉落】获得 " << getColoredItemName(equip) << "！\n";
            } else {
                // 选择护甲
                std::string equip_id = doctor_armor[std::rand() % doctor_armor.size()];
                Item equip;
                if (equip_id == "lab_coat") {
                    equip = ItemDefinitions::createLabCoat();
                } else if (equip_id == "weight_bracelet") {
                    equip = Item::createEquipment(equip_id, "负重护腕", "教学区掉落的装备", 
                        EquipmentType::ARMOR, EquipmentSlot::ARMOR, 3, 9, 0, 30, 0);
                    equip.quality = EquipmentQuality::DOCTOR;
                } else {
                    equip = Item::createEquipment(equip_id, "未知护甲", "教学区掉落的装备", 
                        EquipmentType::ARMOR, EquipmentSlot::ARMOR, 0, 0, 0, 0, 0);
                    equip.quality = EquipmentQuality::DOCTOR;
                }
                state_.player.inventory().add(equip, 1);
                std::cout << "【掉落】获得 " << getColoredItemName(equip) << "！\n";
            }
        }
        // 第四类：饰品类装备（掉落概率10%，累积58%）
        else if (roll() <= 58) {
            // 饰品类装备池：含6件装备
            std::vector<std::string> accessories = {
                "steel_spoon_amulet",    // 【钢勺护符】：对"实验失败妖"系敌人最终伤害×1.3（乘法计算），ATK +1
                "speech_words_amulet",   // 【演讲之词】：战斗开始自动获得"鼓舞"效果（ATK +15%，持续3回合），ATK +1
                "goggles_amulet",        // 【护目镜】：SPD +6，闪避率额外 +5%
                "hnu_badge_amulet",      // 【海纳百川校徽】：全属性 +3，战斗开始获得"专注"效果（持续1回合）
                "ecard_amulet",          // 【海大e卡通】：商店购物享受10%折扣
                "seat_all_lib_amulet"    // 【全图书馆占座物品】：SPD +5，首回合必定先攻
            };
            std::string acc_id = accessories[std::rand() % accessories.size()];
            
            // 饰品可以装备到任意槽位，让装备系统自动选择
            Item accessory;
            if (acc_id == "steel_spoon_amulet") {
                accessory = ItemDefinitions::createSteelSpoonAmulet();
            } else if (acc_id == "speech_words_amulet") {
                accessory = ItemDefinitions::createSpeechWordsAmulet();
            } else if (acc_id == "goggles_amulet") {
                accessory = ItemDefinitions::createGogglesAmulet();
            } else if (acc_id == "hnu_badge_amulet") {
                accessory = ItemDefinitions::createHnuBadgeAmulet();
            } else if (acc_id == "ecard_amulet") {
                accessory = ItemDefinitions::createEcardAmulet();
            } else if (acc_id == "seat_all_lib_amulet") {
                accessory = ItemDefinitions::createSeatAllLibAmulet();
            } else {
                accessory = Item::createEquipment(acc_id, "未知饰品", "教学区掉落的饰品", 
                    EquipmentType::ACCESSORY, EquipmentSlot::ACCESSORY1, 0, 0, 0, 0, 0);
            }
            state_.player.inventory().add(accessory, 1);
            std::cout << "【掉落】获得 " << getColoredItemName(accessory) << "！\n";
        }
        // 未触发任何一类时则无装备掉落
    }
    
    // 无论是否在教学区详细地图，都处理普通掉落物品
    for (const auto& drop : enemy.getDropItems()) {
        if (roll() <= (int)(drop.drop_rate * 100)) {
            int quantity = drop.min_quantity + (std::rand() % (drop.max_quantity - drop.min_quantity + 1));
            
            // 创建掉落物品
            Item drop_item;
            
            // 根据物品ID使用正确的创建方法
            if (drop.item_id == "health_potion") {
                drop_item = Item::createConsumable("health_potion", "生命药水", "恢复30点生命值", 30, 30);
            } else if (drop.item_id == "caffeine_elixir") {
                drop_item = Item::createConsumable("caffeine_elixir", "咖啡因灵液", "获得专注：下一次攻击必中", 0, 150);
            } else if (drop.item_id == "power_fragment") {
                drop_item = Item::createQuestItem("power_fragment", "动力碎片", "可以用来修理训练装置");
            } else {
                // 默认创建普通消耗品
                drop_item.id = drop.item_id;
                drop_item.name = drop.item_name;
                drop_item.type = ItemType::CONSUMABLE;
            }
            
            state_.player.inventory().add(drop_item, quantity);
            std::cout << "【掉落】获得 " << drop.item_name << " x" << quantity << "！\n";
        }
    }
}

void Game::move(const std::string& label){ // label is the exit label exactly
    auto* loc = state_.map.get(state_.current_loc);
    if(!loc){ std::cout<<"当前地点不存在。\n"; return; }
    const Exit* ex = nullptr;
    for(auto &e: loc->exits) if(e.label==label) ex=&e;
    if(!ex){ std::cout<<"此方向无法直接通行（需要沿已有连接行走）。\n"; return; }
    
    // 清屏功能
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
    
    if(ex->to == "enter_teaching") {
        // 进入教学区详细地图
        state_.in_teaching_detail = true;
        state_.current_loc = "jiuzhutan"; // 初始位置在九珠坛
        std::cout << "\n=== 进入教学区详细地图 ===\n";
        look();
    } else if (ex->to == "exit_teaching") {
        // 退出教学区，回到主地图
        state_.in_teaching_detail = false;
        state_.current_loc = "teaching_area"; // 回到教学区
        std::cout << "\n=== 返回主地图 ===\n";
        look();
    } else if (ex->to == "wenxintan") {
        // 进入文心潭前的条件判定
        std::cout << "\n—— 文心潭进入条件判定 ——\n";
        std::cout << "需要：Lv≥9 且 至少两件装备品质≥硕士\n";
        int high_quality_count = 0;
        for (auto slot : state_.player.equipment().getOccupiedSlots()) {
            const Item* it = state_.player.equipment().getEquippedItem(slot);
            if (it && (it->quality == EquipmentQuality::MASTER || it->quality == EquipmentQuality::DOCTOR)) high_quality_count++;
        }
        bool cond_level = state_.player.level() >= 9;
        bool cond_equip = high_quality_count >= 2;
        std::cout << "当前Lv: " << state_.player.level() << (cond_level?" ✓":" ✗") << "\n";
        std::cout << "高品质装备件数(≥硕士): " << high_quality_count << (cond_equip?" ✓":" ✗") << "\n";
        if (!cond_level || !cond_equip) {
            std::cout << "未满足进入条件，无法进入文心潭。\n";
            return;
        }
        state_.current_loc = ex->to;
        std::cout << "\n=== 进入文心潭 ===\n";
        if (!state_.wenxintan_intro_shown) {
            state_.wenxintan_intro_shown = true;

            // 章节标题
            std::cout << "第三章：核心·文心潭试炼（终极考验）\n";
            std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";

            // 场景描述
            std::cout << "场景：文心潭\n";
            std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";

            // 剧情描述
            std::cout << "潭水如镜，轻波涟漪，倒映出你走过的每一段路。\n";
            std::cout << "随着你靠近，水面上逐渐浮现出三道凝实的影子——它们是此处失衡的根源：\n\n";

            // BOSS介绍
            std::cout << "试炼之敌\n";
            std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
            std::cout << "① 文献综述怪：海量资料化作铠甲，阅读即护身，难以击破。\n";
            std::cout << "② 实验失败妖·复苏：不断召唤失败的回声，以数量压垮意志。\n";
            std::cout << "③ 答辩紧张魔·强化：言辞如刃，情绪波动使其愈战愈狂。\n\n";

            // 任务目标
            std::cout << "试炼目标\n";
            std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
            std::cout << "只有依次击败它们，集齐三把【文心秘钥】，才能让文心潭回归平衡。\n";
            std::cout << "⚔️ 你握紧了手中的装备，深吸一口气，迈入最后的修行。\n\n";

            // 成就提示
            std::cout << "击败所有心魔后，将自动开启第四章！\n";
            std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
        }
        look();
    } else {
        state_.current_loc = ex->to;
        // 更新怪物刷新状态
        updateMonsterSpawns();
        look();
    }
}

void Game::talk(const std::string& npc_name) {
    auto* loc = state_.map.get(state_.current_loc);
    if(!loc) {
        std::cout<<"未知地点。\n"; 
        return;
    }
    
    NPC* npc = const_cast<NPC*>(loc->findNPC(npc_name));
    if(!npc) {
        std::cout<<"这里没有名为 "<<npc_name<<" 的NPC。\n";
        return;
    }
    
    // 清屏功能 - 让对话界面更清晰
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
    
    std::string current_dialogue_id = npc->defaultDialogue();
    // 基于任务状态的起始分支（让对话逻辑更清晰）
    if (npc_name == "林清漪") {
        // 林清漪使用新的主菜单系统
        current_dialogue_id = "main_menu";
    } else if (npc_name == "钱道然") {
        // 钱道然也使用主菜单系统
        current_dialogue_id = "main_menu";
    } else if (npc_name == "苏小萌") {
        if (auto* tk = state_.task_manager.getTask("side_canteen_choice")) {
            if (tk->getStatus() == TaskStatus::COMPLETED) {
                current_dialogue_id = "s1_chat"; // 完成后显示支线任务3承接内容
            } else if (tk->getStatus() == TaskStatus::IN_PROGRESS) {
                // 检查是否已经选择过食物（通过好感度标记）
                bool has_chosen_food = state_.player.getNPCFavor("苏小萌") > 0;
                if (has_chosen_food) {
                    // 已经选择过食物，直接进入咖啡因灵液需求阶段
                    current_dialogue_id = "s1_after_pick";
                } else {
                    // 还没有选择食物，进入选择界面
                    current_dialogue_id = "s1_choose";
                }
            } else {
                current_dialogue_id = "welcome"; // 默认首次欢迎
            }
        } else {
            current_dialogue_id = "welcome"; // 未接取任务
        }
    } else if (npc_name == "陆天宇") {
        if (auto* tk = state_.task_manager.getTask("side_gym_fragments")) {
            if (tk->getStatus() == TaskStatus::COMPLETED) {
                current_dialogue_id = "s2_done"; // 完成后显示支线任务4承接内容
            } else if (tk->getStatus() == TaskStatus::IN_PROGRESS) {
                current_dialogue_id = "s2_turnin"; // 进行中→直接进入交付界面
            } else {
                current_dialogue_id = "welcome"; // 未接取任务
            }
        } else {
            current_dialogue_id = "welcome"; // 未接取任务
        }
    } else if (npc_name == "钱道然") {
        // 钱道然使用主菜单系统
        current_dialogue_id = "main_menu";
    }
    int player_favor = state_.player.getNPCFavor(npc_name);
    
    // 改进的对话界面布局
    // 为钱道然的主菜单提供简洁显示，跳过完整的对话界面
    if (!(npc_name == "钱道然" && current_dialogue_id == "main_menu")) {
        std::cout << std::string(60, '=') << "\n";
        std::cout << "💬 与 " << npc_name << " 对话\n";
        std::cout << std::string(60, '=') << "\n";
        std::cout << "💡 输入 'help' 查看对话命令 | 'clear' 清屏 | 'back' 退出\n";
        std::cout << std::string(60, '-') << "\n";
    }
    
    while(true) {
        const DialogueNode* node = npc->getDialogue(current_dialogue_id);
        if(!node) {
            // 尝试使用默认对话ID
            if (current_dialogue_id != npc->defaultDialogue() && !npc->defaultDialogue().empty()) {
                current_dialogue_id = npc->defaultDialogue();
                continue;
            }
            
            // 如果还是没有对话，尝试使用第一个可用的对话
            if (!npc->getDialogues().empty()) {
                current_dialogue_id = npc->getDialogues().begin()->first;
                continue;
            }
            
            // 如果仍然没有对话，显示错误信息
            std::cout<<"对话错误：找不到可用的对话内容。\n";
            // 对话错误时仍允许用户输入命令
            std::cout << "💡 输入 'help' 查看对话命令 | 'clear' 清屏 | 'back' 退出\n";
            std::cout << ">";
            std::string input;
            std::getline(std::cin, input);
            
            if(input == "back") {
                std::cout << "\n🎭 对话结束。\n";
                std::cout << std::string(60, '=') << "\n";
                look();
                break;
            } else if(input == "help") {
                std::cout << "\n" << std::string(60, '=') << "\n";
                std::cout << "📖 对话命令帮助\n";
                std::cout << std::string(60, '=') << "\n";
                std::cout << "数字 - 选择对应选项\n";
                std::cout << "task/t - 查看任务\n";
                std::cout << "clear - 清屏刷新界面\n";
                std::cout << "reset_dialogue - 重置对话记忆（调试用）\n";
                std::cout << "back - 退出对话\n";
                std::cout << std::string(60, '=') << "\n";
                continue;
            } else if(input == "clear") {
                #ifdef _WIN32
                    system("cls");
                #else
                    system("clear");
                #endif
                std::cout << std::string(60, '=') << "\n";
                std::cout << "💬 与 " << npc_name << " 对话\n";
                std::cout << "❤️  好感度: " << player_favor << "\n";
                std::cout << std::string(60, '=') << "\n";
                std::cout << "💡 输入 'help' 查看对话命令 | 'clear' 清屏 | 'back' 退出\n";
                std::cout << std::string(60, '-') << "\n";
                continue;
            } else if(input == "task" || input == "t") {
                state_.task_manager.showTaskList();
                continue;
            } else {
                std::cout << "无效命令，请输入 'back' 退出对话。\n";
                continue;
            }
        }
        
        // 为钱道然的主菜单提供简洁显示
        if (npc_name == "钱道然" && current_dialogue_id == "main_menu") {
            // 跳过完整的对话界面，直接显示主菜单内容
            std::cout << "\n" << std::string(60, '-') << "\n";
            std::cout << "【" << npc->name() << "】\n";
            std::cout << node->npc_text << "\n";
            std::cout << std::string(60, '-') << "\n";
        } else {
            // 检查好感度要求
            if(player_favor < node->favor_requirement) {
                std::cout << "\n" << std::string(60, '-') << "\n";
                std::cout << "【" << npc->name() << "】\"我们还不够熟悉，需要好感度 " << node->favor_requirement << " 才能继续这个话题。\"\n";
                std::cout << std::string(60, '-') << "\n";
                current_dialogue_id = "welcome";
                continue;
            }
            
            // 标记对话已访问（用于记忆系统）
            const_cast<NPC*>(npc)->markDialogueVisited(current_dialogue_id);
            
            // 检查记忆系统，避免重复信息
            if (!node->memory_key.empty() && npc->hasMemory(node->memory_key)) {
                // 如果已经访问过这个记忆，可以显示简化版本或跳过
                std::cout << "\n" << std::string(60, '-') << "\n";
                std::cout << "【" << npc->name() << "】\n";
                std::cout << "（" << npc->name() << "点了点头）\n\n我们之前已经讨论过这个话题了。有什么其他需要帮助的吗？\n";
                std::cout << std::string(60, '-') << "\n";
            } else {
                // 改进的对话文本显示
                std::cout << "\n" << std::string(60, '-') << "\n";
                std::cout << "【" << npc->name() << "】\n";
                std::cout << node->npc_text << "\n";
                std::cout << std::string(60, '-') << "\n";
                
                // 添加记忆
                if (!node->memory_key.empty()) {
                    const_cast<NPC*>(npc)->addMemory(node->memory_key);
                }
            }
        }
        
        if(node->options.empty()) {
            std::cout << "\n🎭 对话结束。\n";
            std::cout << std::string(60, '=') << "\n";
            look(); // 显示当前地点信息
            break;
        }
        
        // 改进的选项显示
        std::cout << "\n📋 选择回复：\n";
        
        // 声明可用选项向量
        std::vector<size_t> available_options;
        
        // 为林清漪和钱道然实现特殊的动态选项显示
        if ((npc_name == "林清漪" || npc_name == "钱道然") && current_dialogue_id == "main_menu") {
            if (npc_name == "钱道然") {
                // 钱道然选项始终完整显示，不跳转
                available_options.resize(node->options.size());
                std::iota(available_options.begin(), available_options.end(), 0);
            } else {
                // 林清漪的主菜单动态选项显示
                for(size_t i = 0; i < node->options.size(); ++i) {
                    const auto& option = node->options[i];
                    // 检查是否已经选择过这个选项（使用requirement字段作为选项标识）
                    if (!option.requirement.empty() && npc->hasChosenOption(option.requirement)) {
                        continue; // 跳过已选择的选项
                    }
                    available_options.push_back(i);
                }
                
                // 如果所有选项都被选择完了，跳转到试炼询问
                if (available_options.empty()) {
                    current_dialogue_id = "trial_inquiry";
                    continue; // 重新开始对话循环
                }
            }
        } else {
            // 其他NPC的常规选项显示
            for(size_t i = 0; i < node->options.size(); ++i) {
                const auto& option = node->options[i];
                // 检查是否已经选择过这个选项
                std::string memory_key = npc_name + "_" + current_dialogue_id + "_" + option.text;
                if (state_.dialogue_memory[npc_name].find(memory_key) == state_.dialogue_memory[npc_name].end()) {
                    available_options.push_back(i);
                }
            }
            
            // 如果没有可用选项，显示所有选项
            if (available_options.empty()) {
                available_options.resize(node->options.size());
                std::iota(available_options.begin(), available_options.end(), 0);
            }
        }
        
        // 显示可用选项，重新编号
        for(size_t i = 0; i < available_options.size(); ++i) {
            size_t option_index = available_options[i];
            const auto& option = node->options[option_index];
            std::cout << "   " << (i+1) << ". " << option.text;
            if(option.favor_change != 0) {
                std::cout << " (好感度" << (option.favor_change > 0 ? "+" : "") << option.favor_change << ")";
            }
            std::cout << "\n";
        }
        
        // 显示特殊命令
        if(npc->hasQuest()) {

        }
        
        std::cout << "\n" << std::string(60, '-') << "\n";
        std::cout << "请选择 (输入数字或命令): ";
        std::string input;
        std::getline(std::cin, input);
        

        
        if(input == "help") {
            std::cout << "\n" << std::string(60, '=') << "\n";
            std::cout << "📖 对话命令帮助\n";
            std::cout << std::string(60, '=') << "\n";
            std::cout << "数字 - 选择对应选项\n";
            std::cout << "task/t - 查看任务\n";
            std::cout << "clear - 清屏刷新界面\n";
            std::cout << "reset_dialogue - 重置对话记忆（调试用）\n";
            std::cout << "back - 退出对话\n";
            std::cout << std::string(60, '=') << "\n";
            continue;
        }
        
        if(input == "task" || input == "t") {
            state_.task_manager.showTaskList();
            continue;
        }
        
        if(input == "clear") {
            // 清屏功能
            #ifdef _WIN32
                system("cls");
            #else
                system("clear");
            #endif
            // 重新显示对话界面头部
            std::cout << std::string(60, '=') << "\n";
            std::cout << "💬 与 " << npc_name << " 对话\n";
            std::cout << "❤️  好感度: " << player_favor << "\n";
            std::cout << std::string(60, '=') << "\n";
            std::cout << "💡 输入 'help' 查看对话命令 | 'clear' 清屏 | 'back' 退出\n";
            std::cout << std::string(60, '-') << "\n";
            continue;
        }
        
        if(input == "reset_dialogue") {
            // 重置对话记忆（调试用）
            state_.dialogue_memory[npc_name].clear();
            std::cout << "对话记忆已重置。\n";
            continue;
        }
        
        if(input == "back") {
            std::cout << "\n🎭 对话结束。\n";
            std::cout << std::string(60, '=') << "\n";
            look();
            break;
        }
        

        
        try {
            int choice = std::stoi(input);
            if(choice > 0 && choice <= static_cast<int>(available_options.size())) {
                size_t option_index = available_options[choice - 1];
                const auto& option = node->options[option_index];
                
                // 记录已选择的选项
                std::string memory_key = npc_name + "_" + current_dialogue_id + "_" + option.text;
                state_.dialogue_memory[npc_name].insert(memory_key);
                
                // 为林清漪和钱道然记录选项选择（用于动态选项显示）
                if ((npc_name == "林清漪" || npc_name == "钱道然") && current_dialogue_id == "main_menu" && !option.requirement.empty()) {
                    const_cast<NPC*>(npc)->markOptionChosen(option.requirement);
                }
                
                // 检查选项条件（为林清漪和钱道然的主菜单跳过requirement检查，因为requirement用作选项标识符）
                // 如果对话ID为空或者是main_menu，都跳过条件检查
                bool should_check_conditions = !((npc_name == "林清漪" || npc_name == "钱道然") && 
                                                (current_dialogue_id == "main_menu" || current_dialogue_id.empty()));
                
                if (should_check_conditions) {
                    auto inventory_items = state_.player.inventory().asSimpleItems();
                    std::unordered_map<std::string, int> inventory_map;
                    for (const auto& item : inventory_items) {
                        inventory_map[item.id] = item.count;
                    }
                    bool can_choose = npc->canChooseOption(option, player_favor, inventory_map);
                    if(!can_choose) {
                        std::cout<<"条件不满足，无法选择此选项。\n";
                        continue;
                    }
                }
                
                // 增加好感度
                if(option.favor_change != 0) {
                    state_.player.addNPCFavor(npc_name, option.favor_change);
                    player_favor = state_.player.getNPCFavor(npc_name); // 更新本地好感度
                    if(option.favor_change > 0) {
                        std::cout<<"【好感度 +" << option.favor_change << "】\n";
                    } else if(option.favor_change < 0) {
                        std::cout<<"【好感度 " << option.favor_change << "】\n";
                    }
                }
                
                // 执行特殊行动
                if(option.action) {
                    option.action();
                }
                
                // 处理特殊奖励逻辑
                handleSpecialRewards(npc_name, current_dialogue_id, choice, npc);
                
                // 特殊处理商店功能
                if(option.next_dialogue_id == "shop" && npc_name == "钱道然") {
                    // 直接调用商店系统
                    openShop(npc_name);
                    // 商店返回后，直接退出对话，避免重复显示主菜单
                    std::cout << "\n🎭 对话结束。\n";
                    std::cout << std::string(60, '=') << "\n";
                    look();
                    return;
                }
                
                
                
                // 移动到下一个对话
                if(option.next_dialogue_id == "exit") {
                    std::cout<<"\n🎭 对话结束。\n";
                    std::cout << std::string(60, '=') << "\n";
                    look(); // 显示当前地点信息
                    return; // 直接退出对话函数
                } else {
                    // 检查苏小萌的咖啡因灵液对话特殊情况
                    if (npc_name == "苏小萌" && current_dialogue_id == "s1_after_pick" && option.next_dialogue_id == "s1_give") {
                        // 检查是否没有咖啡因灵液
                        if (state_.dialogue_memory[npc_name].find("no_caffeine_elixir") != state_.dialogue_memory[npc_name].end()) {
                            // 清除标志并跳转到没有物品的对话
                            state_.dialogue_memory[npc_name].erase("no_caffeine_elixir");
                            current_dialogue_id = "s1_no_elixir_response";
                        } else {
                            current_dialogue_id = option.next_dialogue_id;
                        }
                    }
                    // 检查陆天宇的动力碎片对话特殊情况
                    else if (npc_name == "陆天宇" && current_dialogue_id == "s2_turnin" && option.next_dialogue_id == "s2_check_fragments") {
                        // 根据记忆标志决定跳转到哪个对话
                        if (state_.dialogue_memory[npc_name].find("has_enough_fragments") != state_.dialogue_memory[npc_name].end()) {
                            // 有足够的碎片，直接跳转到完成对话
                            current_dialogue_id = "s2_done";
                            // 执行奖励逻辑
                            state_.player.inventory().remove("power_fragment",3);
                            Item wrist = ItemDefinitions::createWeightBracelet();
                            wrist.price = 0; // 奖励物品免费
                            state_.player.inventory().add(wrist,1);
                            state_.player.addNPCFavor("林清漪",20);
                            std::cout<<"【S2完成】你交付了3个动力碎片，获得负重护腕×1。林清漪好感+20。\n";
                            if (auto* tk = state_.task_manager.getTask("side_gym_fragments")) { tk->complete(); }
                            // 清除记忆标志
                            state_.dialogue_memory[npc_name].erase("has_enough_fragments");
                        } else if (state_.dialogue_memory[npc_name].find("not_enough_fragments") != state_.dialogue_memory[npc_name].end()) {
                            // 没有足够的碎片，跳转到检查对话
                            current_dialogue_id = "s2_check_fragments";
                            // 更新对话内容显示当前数量
                            int current_fragments = state_.player.inventory().quantity("power_fragment");
                            std::cout << "（他数了数你手中的动力碎片）\n\n目前你有 " << current_fragments << " 个动力碎片，还需要 " << (3 - current_fragments) << " 个。\n\n";
                            // 清除记忆标志
                            state_.dialogue_memory[npc_name].erase("not_enough_fragments");
                        } else {
                            current_dialogue_id = option.next_dialogue_id;
                        }
                    }
                // 为林清漪和钱道然实现特殊的对话跳转逻辑
                else if ((npc_name == "林清漪" || npc_name == "钱道然") && current_dialogue_id == "main_menu" && option.next_dialogue_id != "main_menu") {
                    // 主菜单的选项选择后，先跳转到对应对话，然后自动回到主菜单
                    current_dialogue_id = option.next_dialogue_id;
                } else if (npc_name == "林清漪" && option.next_dialogue_id == "main_menu") {
                    // 林清漪的子对话返回主菜单
                    current_dialogue_id = "main_menu";
                } else if (npc_name == "钱道然" && option.next_dialogue_id == "main_menu") {
                    // 钱道然的子对话返回主菜单
                    current_dialogue_id = "main_menu";
                } else {
                    current_dialogue_id = option.next_dialogue_id;
                }
                }
            } else {
                std::cout<<"无效选择，请重新输入。\n";
            }
        } catch(...) {
            std::cout<<"无效输入，请重新输入。\n";
        }
    }
}

// 免参数对话入口：列出当前位置NPC并支持数字/模糊匹配
void Game::talkAuto() {
    auto* loc = state_.map.get(state_.current_loc);
    if(!loc) { std::cout<<"未知地点。\n"; return; }
    if (loc->npcs.empty()) { std::cout<<"这里没有可以交谈的NPC。\n"; return; }

    // 若只有一个NPC，直接进入
    if (loc->npcs.size()==1) { talk(loc->npcs[0].name()); return; }

    std::cout << "\n可交谈的NPC：\n";
    for(size_t i=0;i<loc->npcs.size();++i){
        std::cout << "  " << (i+1) << ". " << loc->npcs[i].name() << " - " << loc->npcs[i].description() << "\n";
    }
    std::cout << "输入编号或NPC名字（back返回）：";
    std::string sel; std::getline(std::cin, sel);
    if (sel=="back") return;
    // 数字选择
    try{
        int idx = std::stoi(sel);
        if (idx>0 && idx<=static_cast<int>(loc->npcs.size())) { talk(loc->npcs[idx-1].name()); return; }
    }catch(...){ }
    // 模糊匹配
    for(const auto& n: loc->npcs){ if (n.name().find(sel)!=std::string::npos) { talk(n.name()); return; } }
    std::cout << "未找到匹配的NPC。\n";
}

// 无参数战斗选择
void Game::fightAuto() {
    auto* loc = state_.map.get(state_.current_loc);
    if(!loc) { std::cout<<"未知地点。\n"; return; }
    
    // 检查当前地点是否有可战斗的怪物
    std::vector<std::string> available_monsters;
    for (const auto& spawn : state_.monster_spawns) {
        if (spawn.location_id == state_.current_loc && canSpawnMonster(state_.current_loc, spawn.monster_name)) {
            available_monsters.push_back(spawn.monster_name);
        }
    }
    
    if (available_monsters.empty()) { 
        std::cout<<"这里没有可以战斗的敌人。\n"; 
        return; 
    }
    
    // 记录战斗前的状态
    int old_xp = state_.player.xp();
    int old_coins = state_.player.coins();
    int old_level = state_.player.level();
    
    if (available_monsters.size()==1) {
        // 只有一个敌人，直接战斗
        std::string monster_name = available_monsters[0];
        Enemy en = createMonsterByName(monster_name);
        
        // 清屏功能 - 让战斗界面更清晰
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
        
        // 显示战斗开始信息
        std::cout << "\n" << std::string(50, '=') << "\n";
        std::cout << "⚔️ 战斗开始！\n";
        std::cout << "挑战目标: " << formatMonsterName(en) << "\n";
        std::cout << "你的等级: Lv" << state_.player.level() << "\n";
        std::cout << std::string(50, '=') << "\n";
        
        std::string log;
        if(combat_.fight(state_.player,en,log)){
            std::cout<<log; 
            handleCombatVictory(en, old_xp, old_coins, old_level);
        } else { 
            std::cout<<log; 
            handlePlayerDeath(); 
        }
        return;
    }
    
    // 多个敌人，显示选择菜单
    std::cout << "\n可挑战的敌人：\n";
    for(size_t i=0;i<available_monsters.size();++i){ 
        std::string monster_name = available_monsters[i];
        Enemy temp_en = createMonsterByName(monster_name);
        std::cout<<"  "<<(i+1)<<". "<<formatMonsterName(temp_en)<<"\n"; 
    }
    
    std::cout<<"输入编号（back返回）："; 
    std::string sel; 
    std::getline(std::cin, sel); 
    if(sel=="back") return; 
    
    try{ 
        int idx=std::stoi(sel); 
        if(idx>0 && idx<=static_cast<int>(available_monsters.size())){
            std::string monster_name = available_monsters[idx-1];
            Enemy en = createMonsterByName(monster_name);
            
            // 清屏功能 - 让战斗界面更清晰
            #ifdef _WIN32
                system("cls");
            #else
                system("clear");
            #endif
            
            // 显示战斗开始信息
            std::cout << "\n" << std::string(50, '=') << "\n";
            std::cout << "⚔️ 战斗开始！\n";
            std::cout << "挑战目标: " << formatMonsterName(en) << "\n";
            std::cout << "你的等级: Lv" << state_.player.level() << "\n";
            std::cout << std::string(50, '=') << "\n";
            
            std::string log; 
            if(combat_.fight(state_.player,en,log)){
                std::cout<<log; 
                handleCombatVictory(en, old_xp, old_coins, old_level);
            } else { 
                std::cout<<log; 
                handlePlayerDeath(); 
            }
            return; 
        }
    }catch(...){ }
    std::cout<<"无效选择。\n";
}

// 无参数装备选择
void Game::equipAuto() {
    auto items = state_.player.inventory().list();
    std::vector<Item> equippables;
    
    // 获取当前已装备的物品ID列表
    std::set<std::string> equipped_ids;
    auto equipped_items = state_.player.equipment().getEquippedItems();
    for (const auto& item : equipped_items) {
        equipped_ids.insert(item.id);
    }
    
    // 过滤掉已装备的物品
    for (const auto& it: items) {
        if (it.type == ItemType::EQUIPMENT && equipped_ids.find(it.id) == equipped_ids.end()) {
            equippables.push_back(it);
        }
    }
    
    if (equippables.empty()) { std::cout<<"没有可装备的物品。\n"; return; }
    std::cout<<"\n可装备的物品：\n"; for(size_t i=0;i<equippables.size();++i){ std::cout<<"  "<<(i+1)<<". "<<getColoredItemName(equippables[i])<<"\n"; }
    std::cout<<"输入编号（back返回）："; std::string sel; std::getline(std::cin, sel); if(sel=="back") return; 
    try{ int idx=std::stoi(sel); if(idx>0 && idx<=static_cast<int>(equippables.size())){
        if(state_.player.equipItem(equippables[idx-1].name)) {
            // 装备成功，显示装备详细信息
            std::cout<<"装备了 " << getColoredItemName(equippables[idx-1]) << "！\n";
            std::cout << "\n" << std::string(40, '-') << "\n";
            std::cout << "📋 装备详情：\n";
            std::cout << formatEquipmentDetails(equippables[idx-1]);
            std::cout << std::string(40, '-') << "\n";
        } else {
            std::cout<<"无法装备这个物品。\n";
        }
        return; }
    }catch(...){ }
    std::cout<<"无效选择。\n";
}

// 无参数卸下选择
void Game::unequipAuto() {
    std::vector<std::pair<std::string, EquipmentSlot>> slots;
    const auto& eq = state_.player.equipment();
    if (eq.getEquippedItem(EquipmentSlot::WEAPON))  slots.push_back({"武器", EquipmentSlot::WEAPON});
    if (eq.getEquippedItem(EquipmentSlot::ARMOR))   slots.push_back({"护甲", EquipmentSlot::ARMOR});
    if (eq.getEquippedItem(EquipmentSlot::ACCESSORY1)) slots.push_back({"饰品1", EquipmentSlot::ACCESSORY1});
    if (eq.getEquippedItem(EquipmentSlot::ACCESSORY2)) slots.push_back({"饰品2", EquipmentSlot::ACCESSORY2});
    if (slots.empty()) { std::cout<<"当前没有可卸下的装备。\n"; return; }
    std::cout<<"\n可卸下：\n"; for(size_t i=0;i<slots.size();++i){ std::cout<<"  "<<(i+1)<<". "<<slots[i].first<<"\n"; }
    std::cout<<"输入编号（back返回）："; std::string sel; std::getline(std::cin, sel); if(sel=="back") return; 
    try{ int idx=std::stoi(sel); if(idx>0 && idx<=static_cast<int>(slots.size())){
        if(state_.player.unequipItem(slots[idx-1].second)) std::cout<<"卸下了装备。\n"; else std::cout<<"该槽位没有装备。\n"; return; }
    }catch(...){ }
    std::cout<<"无效选择。\n";
}

void Game::run(){ 
    printBanner(); 
    showOpeningStory();
    std::string line;
    while(true){ 
        // 更新玩家状态效果持续时间
        state_.player.attr().updateStatuses();
        
        std::cout<<"\n> "; 
        if(!std::getline(std::cin,line)) break; 
        if(line=="quit" || line=="q"){ 
            std::cout<<"游戏结束。\n"; 
            break; 
        }
        else if(line=="exit") {
            // exit命令只在九珠坛有效，用于退出教学区
            if(state_.current_loc == "jiuzhutan" && state_.in_teaching_detail) {
                // 退出教学区，回到主地图
                state_.in_teaching_detail = false;
                state_.current_loc = "teaching_area";
                std::cout << "\n=== 退出教学区，回到主地图 ===\n";
                look();
            } else {
                std::cout<<"exit命令只在九珠坛有效，用于退出教学区。\n";
                std::cout<<"要退出游戏，请使用 quit 或 q 命令。\n";
            }
        }
        else if(line=="help" || line=="h" || line=="?"){ 
            showContextualHelp();
        }
        else if(line=="help combat"){ 
            std::cout<<"\n"<<std::string(50,'=')<<"\n";
            std::cout<<"⚔️ 战斗帮助\n"<<std::string(50,'=')<<"\n";
            std::cout<<" - 命中与闪避受 SPD 影响；专注=必中；鼓舞=ATK+15%\n";
            std::cout<<" - 敌人可能施加‘迟缓/紧张’，留意提示\n";
            std::cout<<" - 装备特效：演讲之词(开场鼓舞)、护目镜(额外闪避)、被子(回合恢复)\n";
            std::cout<<" - 学霸两件套：武器与护甲同品质 → 本科套装+10%，硕士套装+15%，博士套装+20%\n";
        }
        else if(line=="help shop"){ 
            std::cout<<"\n"<<std::string(50,'=')<<"\n";
            std::cout<<"🛒 商店帮助\n"<<std::string(50,'=')<<"\n";
            std::cout<<" - buy <物品名> 购买；sell 出售装备(10金币/件)\n";
            std::cout<<" - e卡通享受9折；复活符限购2张\n";
            std::cout<<" - 可输入 ‘详情 <编号>’ 查看描述\n";
        }
        else if(line=="help task"){ 
            std::cout<<"\n"<<std::string(50,'=')<<"\n";
            std::cout<<"📝 任务帮助\n"<<std::string(50,'=')<<"\n";
            std::cout<<" - task 查看任务列表；task <任务名> 查看详情\n";
            std::cout<<" - 某些任务目标会实时更新进度(如S3击败次数)\n";
        }
        else if(line=="ending"){
            std::cout<<"\n" << std::string(60, '=') << "\n";
            std::cout<<"🌟 结局判定·命运的十字路口 🌟\n";
            std::cout << std::string(60, '=') << "\n";
            bool cleared = state_.truth_reward_given && state_.key_i_obtained && state_.key_ii_obtained && state_.key_iii_obtained;
            int favor = state_.player.getNPCFavor("林清漪");
            bool has_two_items = 0;
            {
                // 检查是否持有【启智笔】【护目镜】【辩锋羽扇】任意两件（包括背包和装备）
                int count=0; 
                
                // 检查背包中的物品
                auto inv = state_.player.inventory().asSimpleItems();
                for (auto &it: inv){
                    if (it.id=="wisdom_pen"||it.id=="goggles"||it.id=="debate_fan") count += (it.count>0);
                }
                
                // 检查已装备的物品
                auto equipped_items = state_.player.equipment().getEquippedItems();
                for (const auto& item: equipped_items){
                    if (item.id=="wisdom_pen"||item.id=="goggles"||item.id=="debate_fan") count += 1;
                }
                
                has_two_items = count>=2;
            }
            if(!cleared){
                std::cout<<"❌ 尚未完成文心潭主线，无法结局判定。\n";
                std::cout<<"💡 提示：需要集齐三把文心秘钥才能开启结局判定。\n";
            } else if (state_.wenxintan_fail_streak>=3) {
                std::cout<<"\n" << std::string(50, '=') << "\n";
                std::cout<<"💔 结局E：迷失的旅人\n";
                std::cout << std::string(50, '=') << "\n";
                std::cout<<"在屡次战斗失败后，你的精神过于疲惫，最终被秘境排斥而出。\n\n";
                std::cout<<"水镜中的倒影开始模糊，那些曾经清晰的目标变得遥不可及。\n";
                std::cout<<"你感到一阵眩晕，再次睁开眼时，发现自己正坐在图书馆的桌前。\n";
                std::cout<<"桌上空空如也，没有《文心潭秘录》，也没有任何痕迹证明刚才的经历。\n\n";
                std::cout<<"回归现实后，你发现自己对学习的信心受到了打击，成绩反而有所下滑。\n";
                std::cout<<"那些在秘境中获得的勇气和智慧，仿佛从未存在过。\n";
                std::cout<<"你开始怀疑，是否真的有过那样一段奇妙的旅程。\n\n";
                std::cout<<"也许，有些机会只有一次。有些成长，需要更多的坚持。\n";
                std::cout<<"但请记住，失败不是终点，而是重新开始的起点。\n";
                std::cout<<"\n🎮 恭喜通关！！\n";
            } else if(cleared && favor>=50 && has_two_items && state_.player.level()>=10){
                std::cout<<"\n" << std::string(50, '=') << "\n";
                std::cout<<"⚖️ 结局C：平衡行者（隐藏结局）\n";
                std::cout << std::string(50, '=') << "\n";
                std::cout<<"你看着手中的《文心潭秘录》，心中有了一个大胆的想法。\n";
                std::cout<<"'也许...我可以找到一种平衡。'你喃喃自语。\n\n";
                std::cout<<"你深吸一口气，将秘录的力量一分为二：\n";
                std::cout<<"一半留在秘境，维持这个特殊空间的运转；\n";
                std::cout<<"一半融入自己的身体，带回现实世界。\n\n";
                std::cout<<"瞬间，你感受到两股力量在体内交织，既强大又和谐。\n";
                std::cout<<"你明白，真正的智慧不是选择其中一方，而是找到平衡点。\n\n";
                std::cout<<"回到现实后，你白天学习、夜晚修炼，创立了'学业互助社'。\n";
                std::cout<<"你用自己的经历和智慧，帮助那些还在为学业焦虑的同学们。\n";
                std::cout<<"你告诉他们，学习不是负担，而是成长的过程。\n\n";
                std::cout<<"渐渐地，你成为了海大校园的传奇人物。\n";
                std::cout<<"同学们都说，和你聊天后，学习变得不再那么困难。\n";
                std::cout<<"你明白，这是秘境给你的最好礼物——帮助他人的能力。\n\n";
                std::cout<<"多年后，当你站在毕业典礼的讲台上时，\n";
                std::cout<<"你看着台下那些充满希望的年轻面孔，心中涌起无限感慨。\n";
                std::cout<<"你知道，你的故事将会激励更多的人，去面对自己的心魔，\n";
                std::cout<<"去追求真正的成长。\n";
                std::cout<<"\n🎮 恭喜通关！！\n";
                std::cout<<"\n📋 结局判定条件：等级≥10级 + 林清漪好感度≥50 + 拥有特殊装备≥2件 + 完成文心潭主线\n";
                std::cout<<"   💡 特殊装备：启智笔、护目镜、辩锋羽扇（包括已装备的）\n";
            } else if(state_.player.level()>=12 && cleared && favor>=60){
                std::cout<<"\n" << std::string(50, '=') << "\n";
                std::cout<<"🌟 结局B：秘境守护者\n";
                std::cout << std::string(50, '=') << "\n";
                std::cout<<"你凝视着水镜中林清漪的身影，心中涌起一股暖流。\n";
                std::cout<<"'我想留下来。'你轻声说道，'我想帮助更多的人。'\n\n";
                std::cout<<"林清漪的眼中闪过一丝欣慰，她缓缓点头：\n";
                std::cout<<"'很好，你终于明白了秘境的真正意义。这里需要的不是强大的力量，\n";
                std::cout<<"而是一颗愿意帮助他人的心。'\n\n";
                std::cout<<"你接过林清漪手中的《文心潭秘录》，感受到其中蕴含的无穷智慧。\n";
                std::cout<<"从此刻起，你成为了新的秘境守护者。\n\n";
                std::cout<<"日复一日，你在水镜前诉说过来人的经验，看见他们重拾自信。\n";
                std::cout<<"你见证了无数个学子的成长：从迷茫到坚定，从恐惧到勇敢。\n";
                std::cout<<"每一个成功走出秘境的人，都带着新的希望回到现实。\n\n";
                std::cout<<"偶尔你也望见现实里的同学们毕业、远行——你知道，这同样是有意义的选择。\n";
                std::cout<<"你明白，真正的成长不是逃避现实，而是在现实中找到自己的价值。\n\n";
                std::cout<<"岁月如流水，你在这个特殊的空间里，成为了无数人生命中的指路明灯。\n";
                std::cout<<"虽然你无法回到现实，但你知道，你的存在让这个世界变得更加美好。\n";
                std::cout<<"\n🎮 恭喜通关！！\n";
                std::cout<<"\n📋 结局判定条件：等级≥12级 + 林清漪好感度≥60 + 完成文心潭主线\n";
            } else if(state_.player.level()>=12 && cleared && favor<60){
                std::cout<<"\n" << std::string(50, '=') << "\n";
                std::cout<<"🎓 结局A：学业有成（回归现实）\n";
                std::cout << std::string(50, '=') << "\n";
                std::cout<<"你深吸一口气，将三把秘钥合而为一。\n";
                std::cout<<"瞬间，文心潭的水面爆发出耀眼的光芒，一道通往现实的光门缓缓开启。\n\n";
                std::cout<<"你踏入光门，回到现实的图书馆。秘录化作流光融入身体，\n";
                std::cout<<"你感受到一股暖流在体内流淌，那是知识的力量，是成长的印记。\n\n";
                std::cout<<"此后学习渐入佳境，难点迎刃而解。期末佳绩，名列前茅。\n";
                std::cout<<"同学们都惊讶于你的变化，但你明白，这不是'开挂'，\n";
                std::cout<<"而是你在秘境磨砺后的水到渠成。\n\n";
                std::cout<<"每当夜深人静时，你偶尔会想起那段奇妙的经历，\n";
                std::cout<<"想起那些与你并肩作战的伙伴，想起那些被击败的心魔。\n";
                std::cout<<"你知道，那些经历已经成为了你人生中最宝贵的财富。\n\n";
                std::cout<<"毕业那天，你站在海大的校园里，看着那些还在为学业焦虑的学弟学妹们，\n";
                std::cout<<"心中涌起一股暖流。你决定，要将这份力量传递下去。\n";
                std::cout<<"\n🎮 恭喜通关！！\n";
                std::cout<<"\n📋 结局判定条件：等级≥12级 + 林清漪好感度<60 + 完成文心潭主线\n";
            } else if (cleared){
                std::cout<<"\n" << std::string(50, '=') << "\n";
                std::cout<<"🌅 结局D：普通回归\n";
                std::cout << std::string(50, '=') << "\n";
                std::cout<<"你完成了文心潭的试炼，但心中仍有些许遗憾。\n";
                std::cout<<"你明白，自己还没有完全准备好面对更大的挑战。\n\n";
                std::cout<<"你返回现实，保留了部分收获。学习有所提升，但并非腾飞。\n";
                std::cout<<"你偶尔会想起那段奇妙经历，但记忆如梦，渐行渐远。\n\n";
                std::cout<<"不过，你并没有完全忘记。\n";
                std::cout<<"每当遇到困难时，你总会想起在秘境中学到的那些道理：\n";
                std::cout<<"面对恐惧，理解问题，拆解困难，最终克服。\n\n";
                std::cout<<"虽然你的成长没有那么显著，但你明白，\n";
                std::cout<<"真正的成长往往是在潜移默化中发生的。\n";
                std::cout<<"也许，下一次机会来临时，你会做得更好。\n\n";
                std::cout<<"毕竟，人生不是一场游戏，而是一段漫长的旅程。\n";
                std::cout<<"每一个选择，每一次尝试，都是成长的一部分。\n";
                std::cout<<"\n🎮 恭喜通关！！\n";
                std::cout<<"\n📋 结局判定条件：完成文心潭主线 + 其他情况（未满足上述特殊条件）\n";
            }
            
            std::cout << "\n" << std::string(60, '=') << "\n";
            std::cout << "感谢您体验《文心潭秘录》的冒险之旅！\n";
            std::cout << "愿您在现实的学习生活中，也能像在秘境中一样勇敢前行。\n";
            std::cout << std::string(60, '=') << "\n";
        }
        else if(line=="look" || line=="l" || line=="查看" || line=="看" || line=="观察") look();
        else if(line=="w" || line=="a" || line=="s" || line=="d") {
            // WASD移动系统 - 严格检查方向连接
            std::string direction;
            if(line=="w") direction = "北";
            else if(line=="a") direction = "西";
            else if(line=="s") direction = "南";
            else if(line=="d") direction = "东";
            
            // 检查当前地点是否有该方向的出口
            auto* loc = state_.map.get(state_.current_loc);
            if(!loc) {
                std::cout<<"当前地点不存在。\n";
                continue;
            }
            
            bool has_exit = false;
            for(const auto& exit : loc->exits) {
                if(exit.label == direction) {
                    has_exit = true;
                    break;
                }
            }
            
            if(has_exit) {
                move(direction);
            } else {
                std::cout<<"无法移动。\n";
            }
        }
        else if(line=="talk" || line=="对话") {
            talkAuto();
        }
        
        else if(line=="fight" || line=="战斗" || line=="挑战") {
            fightAuto();
        }
        else if(line=="monsters" || line=="怪物信息" || line=="刷新信息") {
            showMonsterSpawnInfo();
        }
        else if(line.rfind("fight ",0)==0){ 
            std::string target = line.substr(6); 
            auto* loc = state_.map.get(state_.current_loc); 
            if(!loc){ 
                std::cout<<"未知地点\n"; 
                continue;
            } 
            bool found=false; 
            for(auto en : loc->enemies){ 
                if(en.name()==target){ 
                    found=true; 
                    
                    // 记录战斗前状态
                    int old_xp = state_.player.xp();
                    int old_coins = state_.player.coins();
                    int old_level = state_.player.level();
                    
                    std::string log; 
                    // 检查是否可以战斗（怪物数量限制）
                    if (!canSpawnMonster(state_.current_loc, en.name())) {
                        std::cout << "【提示】" << formatMonsterName(en) << " 暂时不在这个区域，需要等待刷新。\n";
                        std::cout << "输入 'monsters' 查看怪物刷新信息。\n";
                        break;
                    }
                    
                    // 清屏功能 - 让战斗界面更清晰
                    #ifdef _WIN32
                        system("cls");
                    #else
                        system("clear");
                    #endif
                    
                    // 显示战斗开始信息
                    std::cout << "\n" << std::string(50, '=') << "\n";
                    std::cout << "⚔️ 战斗开始！\n";
                    std::cout << "挑战目标: " << formatMonsterName(en) << "\n";
                    std::cout << "你的等级: Lv" << state_.player.level() << "\n";
                    std::cout << std::string(50, '=') << "\n";
                    
                    if(combat_.fight(state_.player,en,log)) {
                        std::cout<<log;
                        handleCombatVictory(en, old_xp, old_coins, old_level);
                    } else {
                        std::cout<<log;
                        // 战斗失败，执行死亡惩罚
                        handlePlayerDeath();
                        if (state_.current_loc == "wenxintan") {
                            onWenxinFail(state_);
                        }
                    }
                    break; 
                } 
            } 
            if(!found) std::cout<<"这里没有这个敌人。\n"; 
        }
        else if(line.rfind("挑战",0)==0) {
            // 处理中文战斗指令 "挑战XXX"
            std::string target = line.substr(2); // 跳过"挑战"
            auto* loc = state_.map.get(state_.current_loc); 
            if(!loc){ 
                std::cout<<"未知地点\n"; 
                continue;
            } 
            bool found=false; 
            for(auto en : loc->enemies){ 
                if(en.name()==target){ 
                    found=true; 
                    
                    // 记录战斗前状态
                    int old_xp = state_.player.xp();
                    int old_coins = state_.player.coins();
                    int old_level = state_.player.level();
                    
                    std::string log; 
                    // 检查是否可以战斗（怪物数量限制）
                    if (!canSpawnMonster(state_.current_loc, en.name())) {
                        std::cout << "【提示】" << formatMonsterName(en) << " 暂时不在这个区域，需要等待刷新。\n";
                        std::cout << "输入 'monsters' 查看怪物刷新信息。\n";
                        break;
                    }
                    
                    // 清屏功能 - 让战斗界面更清晰
                    #ifdef _WIN32
                        system("cls");
                    #else
                        system("clear");
                    #endif
                    
                    // 显示战斗开始信息
                    std::cout << "\n" << std::string(50, '=') << "\n";
                    std::cout << "⚔️ 战斗开始！\n";
                    std::cout << "挑战目标: " << formatMonsterName(en) << "\n";
                    std::cout << "你的等级: Lv" << state_.player.level() << "\n";
                    std::cout << std::string(50, '=') << "\n";
                    
                    if(combat_.fight(state_.player,en,log)) {
                        std::cout<<log;
                        handleCombatVictory(en, old_xp, old_coins, old_level);
                    } else {
                        std::cout<<log;
                        // 战斗失败，执行死亡惩罚
                        handlePlayerDeath();
                        if (state_.current_loc == "wenxintan") {
                            onWenxinFail(state_);
                        }
                    }
                    break; 
                } 
            } 
            if(!found) std::cout<<"这里没有这个敌人。\n"; 
        }
        else if(line.rfind("buy ",0)==0) {
            std::string item_name = line.substr(4);
            auto* loc = state_.map.get(state_.current_loc);
            if(!loc) {
                std::cout<<"未知地点\n";
                continue;
            }
            bool found = false;
            for(const auto& item : loc->shop) {
                if(item.name == item_name) {
                    found = true;
                    if(state_.player.spendCoins(item.price)) {
                        state_.player.inventory().add(item, 1);
                        std::cout<<"购买了 " << item.name << "。\n";
                    } else {
                        std::cout<<"金币不足。\n";
                    }
                    break;
                }
            }
            if(!found) std::cout<<"商店中没有这个物品。\n";
        }
        else if(line.rfind("use ",0)==0) {
            std::string item_name = line.substr(4);
            if(!state_.player.useItem(item_name)) {
                std::cout<<"无法使用这个物品。\n";
            }
        }
        else if(line=="equip" || line=="装备") {
            equipAuto();
        }
        else if(line.rfind("equip ",0)==0) {
            std::string item_name = line.substr(6);
            // 查找物品以获取颜色信息
            auto items = state_.player.inventory().list();
            Item* found_item = nullptr;
            for (const auto& item : items) {
                if (item.name.find(item_name) != std::string::npos || 
                    item_name.find(item.name) != std::string::npos ||
                    item.id == item_name) {
                    found_item = const_cast<Item*>(&item);
                    break;
                }
            }
            
            if(state_.player.equipItem(item_name)) {
                if (found_item) {
                    std::cout<<"装备了 " << getColoredItemName(*found_item) << "。\n";
                    // 显示装备详细信息
                    std::cout << "\n" << std::string(40, '-') << "\n";
                    std::cout << "📋 装备详情：\n";
                    std::cout << formatEquipmentDetails(*found_item);
                    std::cout << std::string(40, '-') << "\n";
                } else {
                    std::cout<<"装备了 " << item_name << "。\n";
                }
            } else {
                std::cout<<"无法装备这个物品。\n";
            }
        }
        else if(line=="unequip" || line=="卸下") {
            unequipAuto();
        }
        else if(line.rfind("unequip ",0)==0) {
            std::string slot_name = line.substr(8);
            EquipmentSlot slot;
            if(slot_name == "weapon" || slot_name == "武器") slot = EquipmentSlot::WEAPON;
            else if(slot_name == "armor" || slot_name == "护甲" || slot_name == "防具") slot = EquipmentSlot::ARMOR;
            else if(slot_name == "accessory1" || slot_name == "饰品1" || slot_name == "饰品一") slot = EquipmentSlot::ACCESSORY1;
            else if(slot_name == "accessory2" || slot_name == "饰品2" || slot_name == "饰品二") slot = EquipmentSlot::ACCESSORY2;
            else {
                std::cout<<"无效的装备槽位。请使用: 武器/护甲/饰品1/饰品2 或 weapon/armor/accessory1/accessory2\n";
                continue;
            }
            if(state_.player.unequipItem(slot)) {
                std::cout<<"卸下了装备。\n";
            } else {
                std::cout<<"该槽位没有装备。\n";
            }
        }
        else if(line=="save"){ 
            // 检查是否已通关
            if (state_.truth_reward_given) {
                std::cout<<"通关后无法存档\n";
            } else if(SaveLoad::save(state_)) {
                std::cout<<"存档成功。\n"; 
            } else {
                std::cout<<"存档失败。\n"; 
            }
        }
        else if(line=="load"){ 
            if(SaveLoad::load(state_)) { 
                std::cout<<"读档成功。\n"; 
                // 确保怪物刷新系统被正确初始化
                if (state_.monster_spawns.empty()) {
                    initializeMonsterSpawns();
                }
                // 重新初始化NPC对话内容，确保对话系统正常工作
                // 这不会覆盖已保存的对话状态（如visited_dialogues_, memories_等）
                initializeNPCDialogues();
                look(); 
            } else std::cout<<"读档失败。\n"; 
        }
        else if(line=="stats" || line=="s" || line=="属性" || line=="状态"){ 
            auto &p = state_.player; 
            std::cout << "\n" << std::string(50, '=') << "\n";
            std::cout << "📊 角色属性\n";
            std::cout << std::string(50, '=') << "\n";
            std::cout << "等级: " << p.level() << " | XP: " << p.xp() << " | 金币: " << p.coins() << "\n";
            std::cout << "生命: " << p.attr().hp << "/" << p.attr().max_hp << "\n";
            std::cout << "攻击: " << p.attr().atk << " | 防御: " << p.attr().def_ << " | 速度: " << p.attr().spd << "\n";
            if(p.attr().available_points > 0) {
                std::cout << "未分配属性点: " << p.attr().available_points << "\n";
                std::cout << "可用属性：hp(生命), atk(攻击), def(防御), spd(速度)\n";
                std::cout << "示例：allocate hp 1 或 allocate atk 2\n";
            }
            std::cout << "\n装备信息：\n" << p.equipment().getEquipmentInfo() << "\n";
            std::cout << std::string(50, '=') << "\n";
        }
        else if(line=="inv" || line=="i" || line=="背包" || line=="物品"){ 
            auto items = state_.player.inventory().list(); 
            
            // 获取当前已装备的物品ID列表
            std::set<std::string> equipped_ids;
            auto equipped_items = state_.player.equipment().getEquippedItems();
            for (const auto& item : equipped_items) {
                equipped_ids.insert(item.id);
            }
            
            // 过滤掉已装备的物品
            std::vector<Item> unequipped_items;
            for (const auto& it : items) {
                if (it.type != ItemType::EQUIPMENT || equipped_ids.find(it.id) == equipped_ids.end()) {
                    unequipped_items.push_back(it);
                }
            }
            
            if(unequipped_items.empty()) {
                std::cout << "\n" << std::string(50, '=') << "\n";
                std::cout << "🎒 背包\n";
                std::cout << std::string(50, '=') << "\n";
                std::cout << "背包是空的。\n";
                std::cout << std::string(50, '=') << "\n";
            } else { 
                std::cout << "\n" << std::string(50, '=') << "\n";
                std::cout << "🎒 背包\n";
                std::cout << std::string(50, '=') << "\n";
                for(auto &it:unequipped_items) {
                    std::string name = it.type==ItemType::EQUIPMENT ? getColoredItemName(it) : it.name;
                    std::string type_icon = it.type==ItemType::EQUIPMENT ? "⚔️ " : 
                                          it.type==ItemType::CONSUMABLE ? "🧪 " : "📋 ";
                    std::cout << "   " << type_icon << name << " x" << it.count;
                    
                    // 如果是装备，显示简要属性
                    if (it.type == ItemType::EQUIPMENT) {
                        std::cout << " (";
                        bool first = true;
                        if (it.atk_delta > 0) {
                            if (!first) std::cout << ", ";
                            std::cout << "ATK+" << it.atk_delta;
                            first = false;
                        }
                        if (it.def_delta > 0) {
                            if (!first) std::cout << ", ";
                            std::cout << "DEF+" << it.def_delta;
                            first = false;
                        }
                        if (it.spd_delta > 0) {
                            if (!first) std::cout << ", ";
                            std::cout << "SPD+" << it.spd_delta;
                            first = false;
                        }
                        if (it.hp_delta > 0) {
                            if (!first) std::cout << ", ";
                            std::cout << "HP+" << it.hp_delta;
                            first = false;
                        }
                        std::cout << ")";
                    }
                    std::cout << "\n";
                }
                std::cout << std::string(50, '=') << "\n";
            } 
        }

        else if(line=="task" || line=="t" || line=="任务" || line=="任务列表") {
            state_.task_manager.showTaskList(state_.player);
        }
        else if(line.rfind("task ",0)==0) {
            std::string task_name = line.substr(5);
            state_.task_manager.showTaskDetails(task_name);
        }
        else if(line=="map") {
            if(state_.in_teaching_detail) {
                renderTeachingDetailMap();
            } else {
                renderMainMap();
            }
        }
        else if(line.rfind("allocate ",0)==0) {
            std::string params = line.substr(9);
            std::istringstream iss(params);
            std::string stat;
            int amount = 1; // 默认分配1点
            
            iss >> stat;
            if (iss >> amount) {
                // 如果提供了数量参数
            }
            
            if (amount <= 0) {
                std::cout<<"分配数量必须大于0！\n";
                continue;
            }
            
            if (amount > state_.player.attr().available_points) {
                std::cout<<"可用属性点不足！需要 " << amount << " 点，但只有 " << state_.player.attr().available_points << " 点。\n";
                continue;
            }
            
            bool success = true;
            for (int i = 0; i < amount; ++i) {
                if (!state_.player.attr().allocatePoint(stat)) {
                    success = false;
                    break;
                }
            }
            
            if (success) {
                std::cout<<"✅ 成功分配 " << amount << " 点属性到 " << stat << "！\n";
                std::cout<<"📊 当前属性：" << state_.player.attr().toString() << "\n";
                if (state_.player.attr().available_points > 0) {
                    std::cout<<"💡 还有 " << state_.player.attr().available_points << " 点属性可分配，继续使用 allocate 指令\n";
                } else {
                    std::cout<<"🎉 所有属性点已分配完毕！\n";
                }
            } else {
                std::cout<<"❌ 分配失败！请检查属性名称是否正确 (hp/atk/def/spd)\n";
            }
        }
        else if(line=="enter") {
            // 进入教学区详细地图
            auto* loc = state_.map.get(state_.current_loc);
            if(!loc) {
                std::cout<<"当前地点不存在。\n";
                continue;
            }
            
            // 检查是否在教学区
            if(state_.current_loc == "teaching_area") {
                state_.in_teaching_detail = true;
                state_.current_loc = "jiuzhutan"; // 初始位置在九珠坛
                std::cout << "\n=== 进入教学区详细地图 ===\n";
                look();
            } else {
                std::cout<<"这里无法进入教学区。\n";
            }
        }
        else if(line=="exit") {
            // 退出教学区详细地图
            if(state_.in_teaching_detail) {
                state_.in_teaching_detail = false;
                state_.current_loc = "teaching_area"; // 回到教学区
                std::cout << "\n=== 返回主地图 ===\n";
                look();
            } else {
                std::cout<<"你不在教学区详细地图中。\n";
            }
        }
        else { 
            // 智能提示系统
            std::cout << "❌ 未知指令: " << line << "\n";
            std::cout << "💡 建议：\n";
            
            // 检查是否是常见的拼写错误或相似命令
            if (line.find("look") != std::string::npos || line.find("查看") != std::string::npos || line.find("看") != std::string::npos) {
                std::cout << "   尝试输入 'look' 或 '查看' 查看当前位置\n";
            } else if (line.find("stats") != std::string::npos || line.find("属性") != std::string::npos || line.find("状态") != std::string::npos) {
                std::cout << "   尝试输入 'stats' 或 '属性' 查看角色信息\n";
            } else if (line.find("inv") != std::string::npos || line.find("背包") != std::string::npos || line.find("物品") != std::string::npos) {
                std::cout << "   尝试输入 'inv' 或 '背包' 查看物品\n";
            } else if (line.find("task") != std::string::npos || line.find("任务") != std::string::npos) {
                std::cout << "   尝试输入 'task' 或 '任务' 查看任务\n";
            } else if (line.find("help") != std::string::npos || line.find("帮助") != std::string::npos) {
                std::cout << "   尝试输入 'help' 或 '帮助' 查看帮助\n";
            } else if (line.find("talk") != std::string::npos || line.find("对话") != std::string::npos) {
                std::cout << "   尝试输入 talk\n";
            } else if (line.find("fight") != std::string::npos || line.find("战斗") != std::string::npos || line.find("挑战") != std::string::npos) {
                std::cout << "   尝试输入 fight\n";
            } else {
                std::cout << "   输入 'help' 查看所有可用指令\n";
                std::cout << "   输入 'look' 查看当前位置和可用操作\n";
                std::cout << "   常用指令: 查看(look), 属性(stats), 背包(inv), 任务(task)\n";
            }
        }
    }
}

// 怪物管理系统实现
void Game::initializeMonsterSpawns() {
    // 清空现有数据
    state_.monster_spawns.clear();
    
    // 体育馆 - 低等级区域 (Lv1-3) - 所有怪物5回合刷新
    state_.monster_spawns.push_back({"gymnasium", "迷糊书虫", 2, 2, 5, 0, 1, 0, 3});
    state_.monster_spawns.push_back({"gymnasium", "拖延小妖", 2, 2, 5, 0, 2, 0, 3});

    // 三六广场 - 中低等级区域 (Lv3-6) - 所有怪物5回合刷新
    state_.monster_spawns.push_back({"plaza_36", "水波幻影", 3, 3, 5, 0, 3, 0, 3});
    state_.monster_spawns.push_back({"plaza_36", "学业焦虑影", 3, 3, 5, 0, 4, 0, 3});

    // 荒废北操场 - 中等级区域 (Lv6-9) - 所有怪物5回合刷新
    state_.monster_spawns.push_back({"north_playground", "夜行怠惰魔", 2, 2, 5, 0, 6, 0, 3});
    state_.monster_spawns.push_back({"north_playground", "压力黑雾", 2, 2, 5, 0, 7, 0, 3});
    
    // 教学区详细地图 - 各层级（初始数量3，5回合刷新，每次刷新可挑战3次）
    state_.monster_spawns.push_back({"teach_5", "高数难题精", 3, 3, 5, 0, 9, 0, 3});
    state_.monster_spawns.push_back({"teach_7", "实验失败妖·群", 3, 3, 5, 0, 8, 0, 3});
    state_.monster_spawns.push_back({"tree_space", "答辩紧张魔", 3, 3, 5, 0, 8, 0, 3});
    
    // 文心潭 - 高等级区域 (Lv9-15) - 所有怪物5回合刷新，挑战次数限制3次
    state_.monster_spawns.push_back({"wenxintan", "文献综述怪", 1, 1, 5, 0, 12, 0, 3});
    state_.monster_spawns.push_back({"wenxintan", "实验失败妖·复苏", 1, 1, 5, 0, 12, 0, 3});
    state_.monster_spawns.push_back({"wenxintan", "答辩紧张魔·强化", 1, 1, 5, 0, 12, 0, 3});
}

void Game::updateMonsterSpawns() {
    for (auto& spawn : state_.monster_spawns) {
        if (spawn.turns_until_respawn > 0) {
            spawn.turns_until_respawn--;
            if (spawn.turns_until_respawn == 0) {
                // 统一处理：所有怪物都刷新到最大数量并重新添加怪物
                if (spawn.current_count < spawn.max_count) {
                    // 重新添加怪物到地图
                    auto* loc = state_.map.get(spawn.location_id);
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
                            // 根据位置和怪物名称重新创建怪物
                            if (spawn.location_id == "teach_5") {
                                Enemy math_difficulty_spirit(spawn.monster_name, Attributes{90, 90, 18, 12}, 50, 80);
                                math_difficulty_spirit.setSpecialSkill("专注弱点", "若攻击者处于专注状态，受到伤害+25%");
                                loc->enemies.push_back(math_difficulty_spirit);
                            } else if (spawn.location_id == "teach_7") {
                                Enemy failed_experiment_group(spawn.monster_name, Attributes{135, 135, 45, 30}, 80, 120);
                                failed_experiment_group.setSpecialSkill("自爆机制", "每回合随机1只自爆，对玩家造成ATK×0.8真实伤害");
                                failed_experiment_group.setHasExplosionMechanic(true);
                                failed_experiment_group.setIsGroupEnemy(true, 3);
                                loc->enemies.push_back(failed_experiment_group);
                            } else if (spawn.location_id == "tree_space") {
                                Enemy defense_anxiety_demon(spawn.monster_name, Attributes{80, 80, 22, 14}, 60, 100);
                                defense_anxiety_demon.setSpecialSkill("紧张施压", "每回合40%概率对玩家施加紧张(DEF-15%, 3回合)");
                                defense_anxiety_demon.setHasTensionSkill(true);
                                loc->enemies.push_back(defense_anxiety_demon);
                            } else {
                                // 其他区域的怪物，根据名称创建
                                if (spawn.monster_name == "迷糊书虫") {
                                    Enemy confused_bookworm(spawn.monster_name, Attributes{25, 25, 8, 5}, 15, 25);
                                    confused_bookworm.addDropItem("health_potion", "生命药水", 1, 1, 0.05f);
                                    confused_bookworm.addDropItem("power_fragment", "动力碎片", 1, 1, 0.50f);
                                    loc->enemies.push_back(confused_bookworm);
                                } else if (spawn.monster_name == "拖延小妖") {
                                    Enemy procrastination_goblin(spawn.monster_name, Attributes{30, 30, 10, 6}, 20, 30);
                                    procrastination_goblin.addDropItem("health_potion", "生命药水", 1, 1, 0.08f);
                                    procrastination_goblin.addDropItem("power_fragment", "动力碎片", 1, 1, 0.50f);
                                    loc->enemies.push_back(procrastination_goblin);
                                } else if (spawn.monster_name == "水波幻影") {
                                    Enemy water_wave_phantom(spawn.monster_name, Attributes{40, 40, 12, 8}, 25, 40);
                                    loc->enemies.push_back(water_wave_phantom);
                                } else if (spawn.monster_name == "学业焦虑影") {
                                    Enemy academic_anxiety_shadow(spawn.monster_name, Attributes{50, 50, 14, 10}, 30, 50);
                                    loc->enemies.push_back(academic_anxiety_shadow);
                                } else if (spawn.monster_name == "夜行怠惰魔") {
                                    Enemy night_laziness_demon(spawn.monster_name, Attributes{70, 70, 16, 12}, 40, 70);
                                    night_laziness_demon.setSpecialSkill("迟缓攻击", "攻击后50%概率对玩家施加迟缓(SPD-20%, 2回合)");
                                    night_laziness_demon.setHasSlowSkill(true);
                                    night_laziness_demon.addDropItem("health_potion", "生命药水", 1, 1, 0.10f);
                                    night_laziness_demon.addDropItem("caffeine_elixir", "咖啡因灵液", 1, 1, 0.50f);
                                    loc->enemies.push_back(night_laziness_demon);
                                } else if (spawn.monster_name == "压力黑雾") {
                                    Enemy stress_black_mist(spawn.monster_name, Attributes{80, 80, 18, 14}, 50, 80);
                                    stress_black_mist.setSpecialSkill("减速领域", "减速(全场SPD-15%, 2回合, 每3回合发动一次)，对玩家实施紧张效果");
                                    stress_black_mist.setHasTensionSkill(true);
                                    stress_black_mist.addDropItem("caffeine_elixir", "咖啡因灵液", 1, 1, 0.50f);
                                    loc->enemies.push_back(stress_black_mist);
                                } else if (spawn.monster_name == "文献综述怪") {
                                    Enemy literature_review_monster(spawn.monster_name, Attributes{280, 280, 50, 25}, 100, 150);
                                    literature_review_monster.setSpecialSkill("阅读", "每3回合进入阅读状态，DEF+50%，持续2回合");
                                    literature_review_monster.addDropItem("wenxin_key_i", "文心秘钥·I", 1, 1, 1.0f);
                                    loc->enemies.push_back(literature_review_monster);
                                } else if (spawn.monster_name == "实验失败妖·复苏") {
                                    Enemy failed_experiment_revive(spawn.monster_name, Attributes{260, 260, 55, 30}, 100, 150);
                                    failed_experiment_revive.setSpecialSkill("召唤", "每3回合召唤1只实验失败妖，最多3只");
                                    failed_experiment_revive.setHasSlowSkill(true);
                                    failed_experiment_revive.addDropItem("wenxin_key_ii", "文心秘钥·II", 1, 1, 1.0f);
                                    loc->enemies.push_back(failed_experiment_revive);
                                } else if (spawn.monster_name == "答辩紧张魔·强化") {
                                    Enemy defense_anxiety_demon_enhanced(spawn.monster_name, Attributes{300, 300, 60, 35}, 120, 180);
                                    defense_anxiety_demon_enhanced.setSpecialSkill("紧张施压", "每回合50%概率对玩家施加紧张(DEF-20%, 4回合)");
                                    defense_anxiety_demon_enhanced.setHasTensionSkill(true);
                                    defense_anxiety_demon_enhanced.addDropItem("wenxin_key_iii", "文心秘钥·III", 1, 1, 1.0f);
                                    loc->enemies.push_back(defense_anxiety_demon_enhanced);
                                }
                            }
                        }
                    }

                    spawn.current_count = spawn.max_count; // 重置为最大数量
                    spawn.challenge_count = 0; // 重置挑战次数计数器
                    std::cout << "\033[31m【怪物刷新】" << spawn.monster_name << " 在 " << spawn.location_id << " 重新出现了！\033[0m\n";
                }
            }
        }
    }
}

bool Game::canSpawnMonster(const std::string& location_id, const std::string& monster_name) const {
    for (const auto& spawn : state_.monster_spawns) {
        if (spawn.location_id == location_id && spawn.monster_name == monster_name) {
            // 检查怪物是否可用：当前数量大于0，挑战次数未达上限，且不在刷新倒计时中
            return spawn.current_count > 0 && spawn.challenge_count < spawn.max_challenges && spawn.turns_until_respawn == 0;
        }
    }
    return true; // 如果没找到配置，默认允许
}

int Game::getAvailableMonsterCount(const std::string& location_id, const std::string& monster_name) const {
    for (const auto& spawn : state_.monster_spawns) {
        if (spawn.location_id == location_id && spawn.monster_name == monster_name) {
            return spawn.current_count;
        }
    }
    return -1; // 如果没找到配置，返回-1表示无限制
}

void Game::onMonsterDefeated(const std::string& location_id, const std::string& monster_name) {
    for (auto& spawn : state_.monster_spawns) {
        if (spawn.location_id == location_id && spawn.monster_name == monster_name) {
            spawn.challenge_count++; // 增加挑战次数计数器

            // 检查是否达到最大挑战次数
            if (spawn.challenge_count >= spawn.max_challenges) {
                // 达到最大挑战次数，减少当前数量并开始刷新倒计时
                spawn.current_count--; // 只有在达到最大挑战次数时才减少当前数量
                
                auto* loc = state_.map.get(location_id);
                if (loc) {
                    // 移除所有匹配名称的怪物
                    loc->enemies.erase(
                        std::remove_if(loc->enemies.begin(), loc->enemies.end(),
                            [&monster_name](const Enemy& enemy) {
                                return enemy.name() == monster_name;
                            }),
                        loc->enemies.end()
                    );
                }

                // 设置重生倒计时
                spawn.turns_until_respawn = spawn.respawn_turns;
                
                // 显示怪物消失和刷新信息
                std::cout << "【怪物消失】" << monster_name << " 已暂时离开这个区域。\n";
                std::cout << "【挑战限制】本次刷新周期内已挑战 " << spawn.challenge_count << " 次，需等待 " << spawn.respawn_turns << " 回合后刷新。\n";
            } else {
                // 还有挑战次数剩余，不减少current_count，怪物继续可用
                // 显示剩余挑战次数信息
                int remaining_challenges = spawn.max_challenges - spawn.challenge_count;
                std::cout << "【挑战剩余】" << monster_name << " 还有 " << remaining_challenges << " 次挑战机会。\n";
            }
            break;
        }
    }
}

int Game::calculateExperiencePenalty(const Enemy& enemy) {
    int player_level = state_.player.level();
    int enemy_level = enemy.getLevel(); // 使用Enemy类的getLevel()方法
    
    int level_diff = enemy_level - player_level;
    
    // 综合经验值机制：防止刷低等级 + 鼓励越级挑战
    if (level_diff == 0) {
        return 100; // 同等级，无奖励无惩罚
    } else if (level_diff > 0) {
        // 越级挑战：给奖励
        if (level_diff == 1) {
            return 120; // 越1级，20%奖励
        } else if (level_diff == 2) {
            return 150; // 越2级，50%奖励
        } else if (level_diff == 3) {
            return 180; // 越3级，80%奖励
        } else if (level_diff == 4) {
            return 200; // 越4级，100%奖励
        } else {
            return 250; // 越5级及以上，150%奖励！
        }
    } else {
        // 低等级怪物：重惩罚
        if (level_diff == -1) {
            return 50;  // 低1级，50%惩罚
        } else if (level_diff == -2) {
            return 25;  // 低2级，75%惩罚
        } else if (level_diff == -3) {
            return 10;  // 低3级，90%惩罚
        } else {
            return 5;   // 低4级及以上，95%惩罚
        }
    }
}

void Game::showMonsterSpawnInfo() const {
    std::cout << "\n" << std::string(50, '=') << "\n";
    std::cout << "🐉 怪物刷新信息\n";
    std::cout << std::string(50, '=') << "\n";

    for (const auto& spawn : state_.monster_spawns) {
        std::cout << "📍 " << spawn.location_id << " - " << spawn.monster_name << "\n";
        std::cout << "   当前数量: " << spawn.current_count << "/" << spawn.max_count << "\n";
        std::cout << "   推荐等级: " << spawn.recommended_level << "\n";
        std::cout << "   已挑战次数: " << spawn.challenge_count << "/" << spawn.max_challenges << "\n";

        if (spawn.turns_until_respawn > 0) {
            std::cout << "   重生倒计时: " << spawn.turns_until_respawn << " 回合\n";
        }
        std::cout << "\n";
    }
    std::cout << std::string(50, '=') << "\n";
}

// 根据怪物名称创建怪物实例
Enemy Game::createMonsterByName(const std::string& monster_name) const {
    if (monster_name == "迷糊书虫") {
        Enemy confused_bookworm(monster_name, Attributes{35, 35, 12, 8}, 20, 30);
        confused_bookworm.addDropItem("health_potion", "生命药水", 1, 1, 0.05f);
        confused_bookworm.addDropItem("power_fragment", "动力碎片", 1, 1, 0.50f);
        return confused_bookworm;
    } else if (monster_name == "拖延小妖") {
        Enemy procrastination_demon(monster_name, Attributes{28, 28, 8, 7}, 8, 12);
        procrastination_demon.addDropItem("health_potion", "生命药水", 1, 1, 0.08f);
        procrastination_demon.addDropItem("power_fragment", "动力碎片", 1, 1, 0.50f);
        return procrastination_demon;
    } else if (monster_name == "水波幻影") {
        Enemy water_phantom(monster_name, Attributes{40, 40, 10, 6}, 15, 25);
        return water_phantom;
    } else if (monster_name == "学业焦虑影") {
        Enemy academic_anxiety(monster_name, Attributes{45, 45, 12, 8}, 20, 30);
        return academic_anxiety;
    } else if (monster_name == "夜行怠惰魔") {
        Enemy night_laziness_demon(monster_name, Attributes{55, 55, 14, 10}, 30, 45);
        night_laziness_demon.setSpecialSkill("迟缓攻击", "攻击后50%概率对玩家施加迟缓(SPD-20%, 2回合)");
        night_laziness_demon.setHasSlowSkill(true);
        night_laziness_demon.addDropItem("health_potion", "生命药水", 1, 1, 0.10f);
        night_laziness_demon.addDropItem("caffeine_elixir", "咖啡因灵液", 1, 1, 0.50f);
        return night_laziness_demon;
    } else if (monster_name == "压力黑雾") {
        Enemy stress_black_mist(monster_name, Attributes{65, 65, 16, 12}, 35, 50);
        stress_black_mist.setSpecialSkill("减速领域", "减速(全场SPD-15%, 2回合, 每3回合发动一次)，对玩家实施紧张效果");
        stress_black_mist.setHasTensionSkill(true);
        stress_black_mist.addDropItem("caffeine_elixir", "咖啡因灵液", 1, 1, 0.50f);
        return stress_black_mist;
    } else if (monster_name == "高数难题精") {
        Enemy math_difficulty_spirit(monster_name, Attributes{130, 130, 28, 18}, 70, 100);
        math_difficulty_spirit.setSpecialSkill("专注弱点", "若攻击者处于专注状态，受到伤害+25%");
        return math_difficulty_spirit;
    } else if (monster_name == "实验失败妖·群") {
        Enemy failed_experiment_group(monster_name, Attributes{180, 180, 60, 40}, 100, 150);
        failed_experiment_group.setSpecialSkill("自爆机制", "每回合随机1只自爆，对玩家造成ATK×0.8真实伤害");
        failed_experiment_group.setHasExplosionMechanic(true);
        failed_experiment_group.setIsGroupEnemy(true, 3);
        return failed_experiment_group;
    } else if (monster_name == "答辩紧张魔") {
        Enemy defense_anxiety_demon(monster_name, Attributes{120, 120, 32, 20}, 80, 120);
        defense_anxiety_demon.setSpecialSkill("紧张施压", "每回合40%概率对玩家施加紧张(DEF-15%, 3回合)");
        defense_anxiety_demon.setHasTensionSkill(true);
        return defense_anxiety_demon;
    } else if (monster_name == "文献综述怪") {
        Enemy lit_review(monster_name, Attributes{280, 280, 50, 25}, 100, 150);
        lit_review.setSpecialSkill("阅读", "每3回合进入阅读状态，DEF+50%，持续2回合");
        lit_review.addDropItem("wenxin_key_i", "文心秘钥·I", 1, 1, 1.0f);
        return lit_review;
    } else if (monster_name == "实验失败妖·复苏") {
        Enemy failed_experiment_revive(monster_name, Attributes{260, 260, 55, 30}, 100, 150);
        failed_experiment_revive.setSpecialSkill("召唤", "每3回合召唤1只实验失败妖，最多3只");
        failed_experiment_revive.setHasSlowSkill(true);
        failed_experiment_revive.addDropItem("wenxin_key_ii", "文心秘钥·II", 1, 1, 1.0f);
        return failed_experiment_revive;
    } else if (monster_name == "答辩紧张魔·强化") {
        Enemy defense_anxiety_demon_enhanced(monster_name, Attributes{300, 300, 60, 35}, 120, 180);
        defense_anxiety_demon_enhanced.setSpecialSkill("紧张施压", "每回合50%概率对玩家施加紧张(DEF-20%, 4回合)");
        defense_anxiety_demon_enhanced.setHasTensionSkill(true);
        defense_anxiety_demon_enhanced.addDropItem("wenxin_key_iii", "文心秘钥·III", 1, 1, 1.0f);
        return defense_anxiety_demon_enhanced;
    }
    
    // 默认怪物
    Enemy default_monster(monster_name, Attributes{20, 20, 5, 3}, 10, 15);
    return default_monster;
}

// 格式化怪物显示名称，包含等级和难度提示
std::string Game::formatMonsterName(const Enemy& enemy) const {
    int monster_level = enemy.getLevel();
    int player_level = state_.player.level();
    int level_diff = player_level - monster_level;
    
    std::string level_info = "Lv" + std::to_string(monster_level);
    
    // 根据等级差添加难度提示
    std::string difficulty_hint = "";
    if (level_diff >= 3) {
        difficulty_hint = " (轻松)";
    } else if (level_diff >= 1) {
        difficulty_hint = " (简单)";
    } else if (level_diff == 0) {
        difficulty_hint = " (相当)";
    } else if (level_diff >= -1) {
        difficulty_hint = " (困难)";
    } else if (level_diff >= -3) {
        difficulty_hint = " (危险)";
    } else {
        difficulty_hint = " (极危)";
    }
    
    return level_info + " " + enemy.name() + difficulty_hint;
}

} // namespace hx
