// 这是游戏世界的实现文件
// 作者：大一学生
// 功能：实现游戏世界的创建和初始化，包括地点、NPC、物品、任务等

#include "Game.hpp"            // 游戏类头文件
#include "Location.hpp"        // 地点类头文件
#include "NPC.hpp"             // NPC类头文件
#include "Item.hpp"            // 物品类头文件
#include "ItemDefinitions.hpp" // 物品定义头文件
#include "Enemy.hpp"           // 敌人类头文件
#include "Attributes.hpp"      // 属性类头文件
#include <iostream>            // 输入输出流

namespace hx {

// 设置游戏世界
// 功能：初始化整个游戏世界，创建所有游戏元素
void Game::setupWorld() {
    // 创建地点 - 设置游戏中的所有地点
    createLocations();
    
    // 创建NPC - 设置游戏中的所有NPC
    createNPCs();
    
    // 初始化NPC对话 - 为NPC添加对话内容
    initializeNPCDialogues();
    
    // 创建物品 - 设置游戏中的所有物品
    createItems();
    
    // 创建任务 - 设置游戏中的所有任务
    createTasks();
    
    // 初始化怪物刷新系统 - 设置怪物的刷新机制
    initializeMonsterSpawns();
    
    // 设置初始位置 - 海大图书馆古籍区
    state_.current_loc = "library";
}

void Game::createLocations() {
    // 主地图 - 海大校园秘境
    Location library;
    library.id = "library";
    library.name = "秘境图书馆";
    library.desc = "高耸的书架直抵穹顶，空气中弥漫着墨香。林清漪立于中央，微笑注视着你。这里是安全区，也是你的重生点。";
    library.coord = {3, 2};
    library.exits = {
        {"东", "wenxintan"},
        {"西", "info_building"},
    };
    // 商店NPC将在createNPCs()中创建
    state_.map.addLocation(library);
    
    Location wenxintan;
    wenxintan.id = "wenxintan";
    wenxintan.name = "文心潭";
    wenxintan.desc = "潭水如镜，却映照出你内心的焦虑与恐惧。波纹中升起的，是你最深的心魔。这里是最终的试炼之地。";
    wenxintan.coord = {2, 2};
    wenxintan.exits = {
        {"西", "library"}
    };
    // 文心潭三战（Lv9-15）
    // 1) 文献综述怪（高防御）
    Enemy lit_review("文献综述怪", Attributes{160, 160, 28, 12}, 70, 100);
    lit_review.setSpecialSkill("阅读", "每3回合进入阅读状态，DEF+50%，持续2回合");
    lit_review.addDropItem("wenxin_key_i", "文心秘钥·I", 1, 1, 1.0f);
    wenxintan.enemies.push_back(lit_review);
    
    // 2) 实验失败妖·复苏（召唤小怪）
    Enemy failed_revive("实验失败妖·复苏", Attributes{140, 140, 30, 15}, 70, 100);
    failed_revive.setSpecialSkill("召唤", "回合开始召唤3只小怪，之后每3回合再召唤1只");
    failed_revive.addDropItem("wenxin_key_ii", "文心秘钥·II", 1, 1, 1.0f);
    wenxintan.enemies.push_back(failed_revive);
    
    // 3) 答辩紧张魔·强化
    Enemy defense_power("答辩紧张魔·强化", Attributes{140, 140, 35, 20}, 60, 100);
    defense_power.setSpecialSkill("爆发", "每回合60%施加紧张；HP<50%时ATK+30%");
    defense_power.addDropItem("wenxin_key_iii", "文心秘钥·III", 1, 1, 1.0f);
    wenxintan.enemies.push_back(defense_power);
    state_.map.addLocation(wenxintan);
    
    Location teaching_area;
    teaching_area.id = "teaching_area";
    teaching_area.name = "教学区";
    teaching_area.desc = "主要的教学区域，通往各个教学楼。";
    teaching_area.coord = {3, 1};
    teaching_area.exits = {
        {"东", "info_building"},
        {"西", "plaza_36"},
        {"进入教学区", "enter_teaching"}
    };
    state_.map.addLocation(teaching_area);
    
    Location info_building;
    info_building.id = "info_building";
    info_building.name = "信息楼";
    info_building.desc = "信息技术相关教学楼，实验设备齐全。";
    info_building.coord = {4, 2};
    info_building.exits = {
        {"东", "library"},
        {"西", "teaching_area"}
    };
    state_.map.addLocation(info_building);
    
    Location plaza_36;
    plaza_36.id = "plaza_36";
    plaza_36.name = "三六广场";
    plaza_36.desc = "校园中心广场，学生们聚集的地方。";
    plaza_36.coord = {2, 1};
    plaza_36.exits = {
        {"东", "teaching_area"},
        {"西", "gymnasium"}
    };
    // 添加敌人 - 三六广场（Lv3-6）
    Enemy water_phantom("水波幻影", Attributes{25, 25, 8, 12}, 10, 12);
    water_phantom.addDropItem("health_potion", "生命药水", 1, 1, 0.08f);
    plaza_36.enemies.push_back(water_phantom);
    
    Enemy academic_anxiety("学业焦虑影", Attributes{30, 30, 9, 8}, 12, 15);
    academic_anxiety.addDropItem("health_potion", "生命药水", 1, 1, 0.12f);
    plaza_36.enemies.push_back(academic_anxiety);
    state_.map.addLocation(plaza_36);
    
    Location gymnasium;
    gymnasium.id = "gymnasium";
    gymnasium.name = "体育馆";
    gymnasium.desc = "空旷的场馆回荡着口号。陆天宇在修理破旧的训练装置。这里是毅力试炼之地。";
    gymnasium.coord = {1, 1};
    gymnasium.exits = {
        {"东", "plaza_36"},
        {"西", "activity_center"}
    };
    // NPC陆天宇将在createNPCs()中创建
    // 添加敌人 - 体育馆（Lv1-3）
    Enemy confused_bookworm("迷糊书虫", Attributes{15, 15, 6, 6}, 3, 5);
    confused_bookworm.addDropItem("health_potion", "生命药水", 1, 1, 0.05f);
    confused_bookworm.addDropItem("power_fragment", "动力碎片", 1, 1, 0.50f);
    gymnasium.enemies.push_back(confused_bookworm);
    
    Enemy procrastination_demon("拖延小妖", Attributes{18, 18, 5, 5}, 5, 8);
    procrastination_demon.addDropItem("health_potion", "生命药水", 1, 1, 0.08f);
    procrastination_demon.addDropItem("power_fragment", "动力碎片", 1, 1, 0.50f);
    gymnasium.enemies.push_back(procrastination_demon);
    state_.map.addLocation(gymnasium);
    
    Location activity_center;
    activity_center.id = "activity_center";
    activity_center.name = "大学生活动中心";
    activity_center.desc = "学生活动的中心场所，各种社团活动在此举行。";
    activity_center.coord = {1, 0};
    activity_center.exits = {
        {"东", "gymnasium"},
        {"北", "north_playground"}
    };
    state_.map.addLocation(activity_center);
    
    Location canteen;
    canteen.id = "canteen";
    canteen.name = "食堂";
    canteen.desc = "香气与嘈杂交织，你看到苏小萌在烦恼选择。这里是选择支线任务的地点。";
    canteen.coord = {2, 0};
    canteen.exits = {
        {"西", "north_playground"}
    };
    // NPC苏小萌将在createNPCs()中创建
    state_.map.addLocation(canteen);
    
    Location north_playground;
    north_playground.id = "north_playground";
    north_playground.name = "荒废北操场";
    north_playground.desc = "废弃的操场，杂草丛生，偶尔有野猫出没。";
    north_playground.coord = {2, -1};
    north_playground.exits = {
        {"东", "canteen"},
        {"南", "activity_center"}
    };
    // 添加敌人 - 荒废北操场（Lv6-9）
    Enemy night_sloth_demon("夜行怠惰魔", Attributes{55, 55, 14, 12}, 20, 25);
    night_sloth_demon.setSpecialSkill("迟缓攻击", "攻击后50%概率对玩家施加迟缓(SPD-20%, 2回合)");
    night_sloth_demon.setHasSlowSkill(true);
    night_sloth_demon.addDropItem("health_potion", "生命药水", 1, 1, 0.10f);
    night_sloth_demon.addDropItem("caffeine_elixir", "咖啡因灵液", 1, 1, 0.50f);
    north_playground.enemies.push_back(night_sloth_demon);
    
    Enemy pressure_black_fog("压力黑雾", Attributes{65, 65, 12, 16}, 25, 30);
    pressure_black_fog.setSpecialSkill("减速领域", "减速(全场SPD-15%, 2回合, 每3回合发动一次)，对玩家实施紧张效果");
    pressure_black_fog.setHasTensionSkill(true);
    pressure_black_fog.addDropItem("caffeine_elixir", "咖啡因灵液", 1, 1, 0.50f);
    north_playground.enemies.push_back(pressure_black_fog);
    state_.map.addLocation(north_playground);
    
    // 教学区详细地图
    Location jiuzhutan;
    jiuzhutan.id = "jiuzhutan";
    jiuzhutan.name = "九珠坛";
    jiuzhutan.desc = "教学区的中心广场，九根石柱环绕，象征着九大学科。";
    jiuzhutan.coord = {0, 0};
    jiuzhutan.exits = {
        {"东", "teach_4"},
        {"西", "teach_3"},
        {"退出教学区", "exit_teaching"}
    };
    state_.map.addLocation(jiuzhutan);
    
    Location teach_2;
    teach_2.id = "teach_2";
    teach_2.name = "教学楼二区";
    teach_2.desc = "基础课程教学楼，新生们在这里学习基础知识。";
    teach_2.coord = {-2, 0};
    teach_2.exits = {
        {"东", "teach_3"}
    };
    state_.map.addLocation(teach_2);
    
    Location teach_3;
    teach_3.id = "teach_3";
    teach_3.name = "教学楼三区";
    teach_3.desc = "专业课程教学楼，学生们在这里深入学习专业知识。";
    teach_3.coord = {-1, 0};
    teach_3.exits = {
        {"东", "jiuzhutan"},
        {"西", "teach_2"},
        {"南", "teach_5"}
    };
    state_.map.addLocation(teach_3);
    
    Location teach_4;
    teach_4.id = "teach_4";
    teach_4.name = "教学楼四区";
    teach_4.desc = "实验课程教学楼，各种实验设备齐全。";
    teach_4.coord = {1, 0};
    teach_4.exits = {
        {"西", "jiuzhutan"}
    };
    state_.map.addLocation(teach_4);
    
    Location teach_5;
    teach_5.id = "teach_5";
    teach_5.name = "教学楼五区";
    teach_5.desc = "走廊里漂浮着试卷幻影，每一道题都化作幻影敌人扑来。这里是智力试炼之地。";
    teach_5.coord = {0, 1};
    teach_5.exits = {
        {"东", "teach_6"},
        {"北", "teach_3"}
    };
    // 添加智力试炼敌人 - 高数难题精（MiniBoss, Lv9）
    // 教学区怪物：单个实例，可挑战3次，打完后5回合刷新
    Enemy math_difficulty_spirit("高数难题精", Attributes{90, 90, 18, 12}, 50, 80);
    math_difficulty_spirit.setSpecialSkill("专注弱点", "若攻击者处于专注状态，受到伤害+25%");
    teach_5.enemies.push_back(math_difficulty_spirit);
    state_.map.addLocation(teach_5);
    
    Location teach_6;
    teach_6.id = "teach_6";
    teach_6.name = "教学楼六区";
    teach_6.desc = "高级课程教学楼，研究生们在这里进行深入研究。";
    teach_6.coord = {1, 1};
    teach_6.exits = {
        {"西", "teach_5"},
        {"东", "teach_7"},
        {"南", "tree_space"}
    };
    state_.map.addLocation(teach_6);
    
    Location teach_7;
    teach_7.id = "teach_7";
    teach_7.name = "教学楼七区";
    teach_7.desc = "实验楼：烧瓶与仪器碎片漂浮在空气中，失败的实验化作怪物冲来。这里是抗挫试炼之地。";
    teach_7.coord = {2, 1};
    teach_7.exits = {
        {"西", "teach_6"},
    };
    // 添加抗挫试炼敌人 - 实验失败妖·群（Lv6-8，3只一组）
    // 教学区怪物：单个实例，可挑战3次，打完后5回合刷新
    Enemy failed_experiment_group("实验失败妖·群", Attributes{135, 135, 45, 30}, 80, 120); // 3只一组，总HP=45*3=135
    failed_experiment_group.setSpecialSkill("自爆机制", "每回合随机1只自爆，对玩家造成ATK×0.8真实伤害");
    failed_experiment_group.setHasExplosionMechanic(true);
    failed_experiment_group.setIsGroupEnemy(true, 3);
    teach_7.enemies.push_back(failed_experiment_group);
    state_.map.addLocation(teach_7);
    
    Location tree_space;
    tree_space.id = "tree_space";
    tree_space.name = "树下空间";
    tree_space.desc = "大厅中空无一人，却有无数声音在你耳边争论，让你心神不宁。这里是表达试炼之地。";
    tree_space.coord = {2, 0};
    tree_space.exits = {
        {"北", "teach_6"}
    };
    // 添加表达试炼敌人 - 答辩紧张魔（Lv8-10）
    // 教学区怪物：单个实例，可挑战3次，打完后5回合刷新
    Enemy defense_anxiety_demon("答辩紧张魔", Attributes{80, 80, 22, 14}, 60, 100);
    defense_anxiety_demon.setSpecialSkill("紧张施压", "每回合40%概率对玩家施加紧张(DEF-15%, 3回合)");
    defense_anxiety_demon.setHasTensionSkill(true);
    tree_space.enemies.push_back(defense_anxiety_demon);
    state_.map.addLocation(tree_space);
}

void Game::createNPCs() {
    // 林清漪 - 引导者NPC（重新设计版）
    NPC lin_qingyi("林清漪", "秘境图书馆的管理员，也是你的引导者。她总是微笑着，似乎对这里的一切都很熟悉。");
    
    // 主菜单对话 - 动态选项显示
    DialogueNode main_menu;
    main_menu.npc_text = "（林清漪缓缓转过身，眼中闪烁着温和的光芒）\n\n欢迎来到文心秘境，年轻的学子。我是林清漪，这里的守护者。\n\n（她轻抚着手中的古籍）\n\n这里，是每个海大学子内心世界的投影。那些无形的学业压力、考试焦虑、选择困难...在这里都化作了可见的心魔。\n\n（她看向你，眼中带着关切）\n\n你看起来有些迷茫，但不要担心。每一个来到这里的人，最终都会找到属于自己的答案。\n\n有什么我可以帮助你的吗？";
    main_menu.options = {
        DialogueOption{"为什么会来到这里？", "why_here", nullptr, 0, "why_here"},
        DialogueOption{"了解文心坛", "wenxin_info", nullptr, 0, "wenxin_info"},
        DialogueOption{"给予装备", "equipment", nullptr, 0, "equipment"},
        DialogueOption{"了解教学区", "teaching_area_info", nullptr, 0, "teaching_area_info"},
        DialogueOption{"支线5叙述", "debate_challenge", nullptr, 0, "debate_challenge"},
        DialogueOption{"再见", "exit", nullptr, 0, ""}
    };
    lin_qingyi.addDialogue("main_menu", main_menu);
    
    // 设置主菜单为默认对话
    lin_qingyi.setDefaultDialogue("main_menu");
    
    // 1. 为什么会来到这里？
    DialogueNode why_here;
    why_here.npc_text = "（林清漪的眼中闪过一丝深邃的光芒）\n\n为什么会来到这里...这是一个很好的问题。\n\n（她轻抚着手中的古籍，声音变得深沉）\n\n每一个来到文心秘境的人，都是因为内心深处的某种渴望或困惑。也许你正在为学业压力而苦恼，也许你正在为未来的选择而迷茫，也许你正在为某种情感而困扰...\n\n（她看向你）\n\n这个秘境，是每个海大学子内心世界的投影。那些无形的压力、焦虑、恐惧...在这里都化作了可见的心魔。\n\n（她的声音变得温柔）\n\n但是，这里也是成长的地方。只有面对这些心魔，战胜它们，你才能真正地成长，找到属于自己的答案。\n\n（她指向不同的方向）\n\n秘境中有三个主要试炼等待着你去挑战，每一个都对应着成长路上必须面对的考验。\n\n（她看向你，眼中带着鼓励）\n\n现在，你还有什么其他想了解的吗？";
    why_here.options = {
        DialogueOption{"我明白了，谢谢您的解释。", "main_menu", nullptr, 0, ""}
    };
    lin_qingyi.addDialogue("why_here", why_here);
    
    // 2. 了解文心坛
    DialogueNode wenxin_info;
    wenxin_info.npc_text = "（林清漪的表情变得严肃，眼中闪过一丝敬畏）\n\n文心潭...那是秘境的最终试炼之地，也是所有心魔的源头。\n\n（她望向东方，声音变得深沉）\n\n那里沉睡着三只最强大的心魔，每一只都代表着学子内心最深层的恐惧：\n\n• 【文献综述怪】：海量资料化作铠甲，阅读即护身，难以击破\n• 【实验失败妖·复苏】：不断召唤失败的回声，以数量压垮意志\n• 【答辩紧张魔·强化】：言辞如刃，情绪波动使其愈战愈狂\n\n（她转向你，声音变得坚定）\n\n只有集齐三把文心秘钥，才能揭开秘境的真相，找到回归现实的方法。但是，这需要你达到等级≥9且至少拥有两件硕士品质装备。\n\n（她的声音变得温柔）\n\n不过，不要着急。先完成三个基础试炼，提升自己的实力，然后再考虑挑战文心潭。\n\n（她看向你）\n\n还有什么其他想了解的吗？";
    wenxin_info.options = {
        DialogueOption{"我明白了，我会先完成基础试炼的。", "main_menu", nullptr, 0, ""}
    };
    lin_qingyi.addDialogue("wenxin_info", wenxin_info);
    
    // 3. 给予装备
    DialogueNode equipment;
    equipment.npc_text = "（林清漪从书架上取下两件物品，眼中闪烁着慈爱的光芒）\n\n这是【普通学子服】和【竹简笔记】，基础装备会保护你。\n\n（她耐心地解释）\n\n【普通学子服】能提供基础的防御力，【竹简笔记】则能提升你的攻击力和速度。\n\n（她的声音变得严肃）\n\n但是，这些只是基础装备。在秘境中，还有更强大的装备等待你去发现：\n• \x1b[32m本科级装备\x1b[0m：适合新手，属性稳定（绿色显示）\n• \x1b[36m硕士级装备\x1b[0m：需要一定实力才能获得（蓝色显示）\n• \x1b[31m博士级装备\x1b[0m：最强大的装备，只有真正的强者才能驾驭（红色显示）\n• \x1b[33m饰品装备\x1b[0m：特殊装备，提供独特效果（黄色显示）\n\n（她将物品递给你）\n\n这些装备会保护你，但记住，真正的力量来自于你的内心。";
    equipment.options = {
        DialogueOption{"谢谢您，我会好好珍惜这些装备的。", "accept_equipment", [this](){
            // 给予装备
            Item uniform = ItemDefinitions::createStudentUniform();
            uniform.price = 0; // 奖励物品免费
            state_.player.inventory().add(uniform, 1);
            
            Item notes = ItemDefinitions::createBambooNotes();
            notes.price = 0; // 奖励物品免费
            state_.player.inventory().add(notes, 1);
            
            std::cout << "【获得装备】普通学子服×1、竹简笔记×1\n";
            
            // 自动接取进入秘境任务
            if (!state_.task_manager.hasActiveTask("m1_enter_secret") && !state_.task_manager.hasCompletedTask("m1_enter_secret")) {
                state_.task_manager.startTask("m1_enter_secret");
                std::cout<<"【任务接取】进入秘境。使用 task 查看详情。\n";
            }
        }, 0, ""},
        DialogueOption{"我怎样才能获得更好的装备？", "equipment_guide", nullptr, 0, ""}
    };
    lin_qingyi.addDialogue("equipment", equipment);
    
    // 4. 了解教学区
    DialogueNode teaching_area_info;
    teaching_area_info.npc_text = "（林清漪指向西边）\n\n教学区详细地图...那是秘境中最复杂也最危险的地方。\n\n（她的声音变得严肃）\n\n要进入教学区详细地图，你需要先到达主地图的教学区，然后使用'enter'命令进入。\n\n（她指向不同的方向）\n\n教学区详细地图包含：\n• 九珠坛：中心广场，可以退出教学区\n• 教学楼二区到七区：不同的试炼区域\n• 树下空间：表达试炼的最终地点\n\n（她的表情变得更加严肃）\n\n那里的怪物比主地图更强大，也更危险：\n• 高数难题精（教学楼五区）：智力试炼的最终Boss，拥有强大的数学攻击\n• 实验失败妖·群（教学楼七区）：抗挫试炼的敌人，会自爆造成真实伤害\n• 答辩紧张魔（树下空间）：表达试炼的敌人，会施加紧张效果\n\n（她的声音变得温柔）\n\n但是，这些怪物也掉落最好的装备。如果你有足够的实力，它们会给你丰厚的回报。\n\n记住，战斗前一定要做好准备，带上足够的生命药水。\n\n（她看向你）\n\n还有什么其他想了解的吗？";
    teaching_area_info.options = {
        DialogueOption{"我明白了，我会小心的。", "main_menu", nullptr, 0, ""},
        DialogueOption{"关于战斗经验制度，能详细说说吗？", "exp_system_info", nullptr, 0, ""}
    };
    lin_qingyi.addDialogue("teaching_area_info", teaching_area_info);
    
    // 经验等级制度说明
    DialogueNode exp_system_info;
    exp_system_info.npc_text = "（林清漪的眼中闪烁着智慧的光芒）\n\n啊，你问到了秘境的战斗经验制度...这是一个很重要的机制。\n\n（她轻抚着手中的古籍，声音变得严肃）\n\n在秘境中，战斗经验的计算遵循以下规则：\n\n【越级挑战奖励】\n• 挑战同等级怪物：获得100%经验值\n• 越1级挑战：获得120%经验值（20%奖励）\n• 越2级挑战：获得150%经验值（50%奖励）\n• 越3级挑战：获得180%经验值（80%奖励）\n• 越4级挑战：获得200%经验值（100%奖励）\n• 越5级及以上：获得250%经验值（150%奖励）\n\n【低等级惩罚】\n• 挑战低1级怪物：获得50%经验值（50%惩罚）\n• 挑战低2级怪物：获得25%经验值（75%惩罚）\n• 挑战低3级怪物：获得10%经验值（90%惩罚）\n• 挑战低4级及以上：获得5%经验值（95%惩罚）\n\n（她的声音变得温柔）\n\n这个制度鼓励你挑战更强的敌人，同时防止你通过刷低级怪物来快速升级。\n\n（她看向你）\n\n所以，勇敢地挑战那些'危险'和'极危'的怪物吧！它们会给你丰厚的经验奖励。\n\n（她指向不同的方向）\n\n还有什么其他想了解的吗？";
    exp_system_info.options = {
        DialogueOption{"我明白了，谢谢您的详细解释。", "main_menu", nullptr, 0, ""},
        DialogueOption{"回到教学区介绍", "teaching_area_info", nullptr, 0, ""}
    };
    lin_qingyi.addDialogue("exp_system_info", exp_system_info);
    
    // 5. 支线5叙述
    DialogueNode debate_challenge;
    debate_challenge.npc_text = "（林清漪的眼中闪过一丝神秘的光芒）\n\n答辩紧张魔...那是一个很特殊的试炼。\n\n（她的声音变得严肃）\n\n在树下空间，你会遇到答辩紧张魔，你战胜他之后，他会问你3个问题。\n\n（她的表情变得认真）\n\n这些问题都是关于学术和人生的，需要你仔细思考。如果你能答对2题以上，就能获得辩峰羽扇——一把非常强大的武器。\n\n（她看向你）\n\n但是要小心，如果答错太多，可能会受到惩罚。你准备好了吗？\n\n（她的声音变得温柔）\n\n这个试炼考验的不是你的战斗能力，而是你的智慧和判断力。保持冷静，仔细思考，相信自己的判断。\n\n（她看向你）\n\n还有什么其他想了解的吗？";
    debate_challenge.options = {
        DialogueOption{"我明白了，谢谢您的指导。", "main_menu", [this](){
            // 自动接取支线任务5：答辩对话挑战
            if (!state_.task_manager.hasActiveTask("side_debate_challenge") && !state_.task_manager.hasCompletedTask("side_debate_challenge")) {
                state_.task_manager.startTask("side_debate_challenge");
                std::cout<<"【任务接取】答辩对话挑战。使用 task 查看详情。\n";
            }
        }, 0, ""}
    };
    lin_qingyi.addDialogue("debate_challenge", debate_challenge);
    
    // 装备指南
    DialogueNode equipment_guide;
    equipment_guide.npc_text = "（林清漪的眼中闪烁着智慧的光芒）\n\n获得更好的装备需要实力和运气。\n\n（她指向不同的方向）\n\n装备来源：\n• 击败怪物掉落：不同等级的怪物掉落不同品质的装备\n• 完成任务奖励：完成试炼和支线任务会获得装备奖励\n• 商店购买：某些NPC会出售装备，但需要好感度\n\n（她的声音变得严肃）\n\n装备品质从低到高：\n• 本科级：基础装备，适合新手\n• 硕士级：中级装备，需要一定实力\n• 博士级：高级装备，只有强者才能驾驭\n\n（她看向你）\n\n记住，装备只是辅助，真正的力量来自于你的内心和努力。\n\n（她看向你）\n\n还有什么其他想了解的吗？";
    equipment_guide.options = {
        DialogueOption{"我明白了，谢谢您的指导。", "main_menu", [this](){
            // 检查是否已经给过装备（通过检查背包中是否已有这些装备）
            bool hasUniform = false;
            bool hasNotes = false;
            
            for (const auto& item : state_.player.inventory().list()) {
                if (item.id == "student_uniform") hasUniform = true;
                if (item.id == "bamboo_notes") hasNotes = true;
            }
            
            if (!hasUniform || !hasNotes) {
                // 给予装备
                Item uniform = ItemDefinitions::createStudentUniform();
                uniform.price = 0; // 奖励物品免费
                state_.player.inventory().add(uniform, 1);
                
                Item notes = ItemDefinitions::createBambooNotes();
                notes.price = 0; // 奖励物品免费
                state_.player.inventory().add(notes, 1);
                
                std::cout << "【获得装备】普通学子服×1、竹简笔记×1\n";
                
                // 自动接取进入秘境任务
                if (!state_.task_manager.hasActiveTask("m1_enter_secret") && !state_.task_manager.hasCompletedTask("m1_enter_secret")) {
                    state_.task_manager.startTask("m1_enter_secret");
                    std::cout<<"【任务接取】进入秘境。使用 task 查看详情。\n";
                }
            } else {
                std::cout<<"【林清漪】\"这些装备我已经给过你了，要好好珍惜哦。\"\n";
            }
        }, 0, ""}
    };
    lin_qingyi.addDialogue("equipment_guide", equipment_guide);
    
    // 接受装备后的对话
    DialogueNode accept_equipment;
    accept_equipment.npc_text = "（林清漪满意地点了点头）\n\n很好，装备已经给你了。记住，完成三个试炼后，来见我。\n\n（她的声音变得严肃）\n\n三个试炼分别在西边的教学楼、南边的体育馆和北边的食堂。完成这些试炼后，你就有资格挑战文心潭了。\n\n（她看向你）\n\n加油，我相信你能够成功的！\n\n（她看向你）\n\n还有什么其他想了解的吗？";
    accept_equipment.options = {
        DialogueOption{"谢谢您，我会努力的！", "main_menu", [this](){
            // 自动接取试炼任务
            if (!state_.task_manager.hasActiveTask("m2_trials") && !state_.task_manager.hasCompletedTask("m2_trials")) {
                state_.task_manager.startTask("m2_trials");
                std::cout<<"【任务接取】完成试炼。使用 task 查看详情。\n";
            }
        }, 0, ""}
    };
    lin_qingyi.addDialogue("accept_equipment", accept_equipment);
    
    // 试炼情况询问对话（当所有选项都被选择完后显示）
    DialogueNode trial_inquiry;
    trial_inquiry.npc_text = "（林清漪温和地看着你）\n\n看来你已经了解了很多关于秘境的信息。\n\n（她的眼中闪过一丝关切）\n\n试炼进行得怎么样？有没有遇到什么困难？\n\n（她轻抚着手中的古籍）\n\n记住，每个试炼都是成长的机会。不要害怕失败，重要的是从中学到东西。\n\n（她看向你）\n\n如果你需要任何帮助，随时可以来找我。";
    trial_inquiry.options = {
        DialogueOption{"谢谢您的关心，我会努力的。", "exit", nullptr, 0, ""},
        DialogueOption{"再见。", "exit", nullptr, 0, ""}
    };
    lin_qingyi.addDialogue("trial_inquiry", trial_inquiry);
    
    
    // 苏小萌 - 食堂支线 S1
    NPC su_xiaomeng("苏小萌", "一个可爱的女同学，正站在食堂窗口前纠结着今天吃什么。她的眉头紧锁，看起来选择困难症发作了。");
    DialogueNode s1_start;
    s1_start.npc_text = "（苏小萌站在食堂窗口前，双手抱头，一脸纠结）\n\n呜啊...今天到底吃什么好呢？\n\n（她看着菜单，眼中满是迷茫）\n\n麻辣烫？香辣可口，但是...会不会太辣了？\n牛肉面？清淡营养，但是...会不会太单调了？\n大盘鸡？分量十足，但是...会不会太油腻了？\n\n（她转向你，眼中带着求助的光芒）\n\n我...我每次都是这样，总是不知道该怎么选择。你能帮帮我吗？";
    s1_start.options = {
        DialogueOption{"当然可以！我来帮你选择。", "s1_choose", [this](){
            if (!state_.task_manager.hasActiveTask("side_canteen_choice") && !state_.task_manager.hasCompletedTask("side_canteen_choice")) {
                state_.task_manager.startTask("side_canteen_choice");
                std::cout<<"【任务接取】食堂选择。使用 task 查看详情。\n";
            }
        }, 0, ""},
        DialogueOption{"选择困难确实很麻烦，让我想想...", "s1_advice", nullptr, 0, ""},
        DialogueOption{"抱歉，我也有选择困难症。", "exit", nullptr, 0, ""}
    };
    su_xiaomeng.addDialogue("welcome", s1_start);
    
    // 设置默认对话
    su_xiaomeng.setDefaultDialogue("welcome");
    
    // 添加选择建议对话
    DialogueNode s1_advice;
    s1_advice.npc_text = "（苏小萌的眼中闪过一丝希望）\n\n真的吗？你也有选择困难症？\n\n（她靠近你，声音变得小声）\n\n其实...我每次都是这样。明明知道这些都是好选择，但就是不知道该怎么选。\n\n（她的声音变得有些沮丧）\n\n有时候我觉得，选择比考试还难。考试至少有个标准答案，但选择...每个选择都好像是对的，又好像都不对。\n\n（她看向你）\n\n你能...你能帮我分析一下吗？";
    s1_advice.options = {
        DialogueOption{"当然可以！让我帮你分析一下。", "s1_choose", [this](){
            if (!state_.task_manager.hasActiveTask("side_canteen_choice") && !state_.task_manager.hasCompletedTask("side_canteen_choice")) {
                state_.task_manager.startTask("side_canteen_choice");
                std::cout<<"【任务接取】食堂选择。使用 task 查看。\n";
            }
        }, 0, ""},
        DialogueOption{"我觉得你可以试试...", "s1_choose", [this](){
            if (!state_.task_manager.hasActiveTask("side_canteen_choice") && !state_.task_manager.hasCompletedTask("side_canteen_choice")) {
                state_.task_manager.startTask("side_canteen_choice");
                std::cout<<"【任务接取】食堂选择。使用 task 查看。\n";
            }
        }, 0, ""},
        DialogueOption{"抱歉，我可能帮不上忙。", "exit", nullptr, 0, ""}
    };
    su_xiaomeng.addDialogue("s1_advice", s1_advice);
    DialogueNode s1_choose;
    s1_choose.npc_text = "（苏小萌的眼中闪烁着期待的光芒）\n\n真的吗？太好了！\n\n（她重新看向菜单，声音变得兴奋）\n\n那你觉得哪个更适合我今天的状态呢？\n\n（她指向不同的选项）\n\n麻辣烫：香辣可口，能让人热血沸腾\n牛肉面：清淡营养，能让人内心平静\n大盘鸡：分量十足，能让人充满活力\n\n（她转向你，眼中带着信任）\n\n我相信你的判断！";
    s1_choose.options = {
        DialogueOption{"选麻辣烫吧！香辣可口，能让人热血沸腾。", "s1_after_pick", [this](){ 
            state_.player.attr().atk += 2; 
            std::cout<<"你感到热血上涌，ATK+2。\n"; 
            if(auto* tk=state_.task_manager.getTask("side_canteen_choice")) {
                tk->setObjective(0, "完成选择 ✓");
                // 标记已经选择过食物，防止重复选择
                state_.player.setNPCFavor("苏小萌", 1); // 使用好感度标记已选择
            }
        }, 0, ""},
        DialogueOption{"选牛肉面吧！清淡营养，能让人内心平静。", "s1_after_pick", [this](){ 
            state_.player.attr().def_ += 3; 
            std::cout<<"一碗下肚，底气更足，DEF+3。\n"; 
            if(auto* tk=state_.task_manager.getTask("side_canteen_choice")) {
                tk->setObjective(0, "完成选择 ✓");
                // 标记已经选择过食物，防止重复选择
                state_.player.setNPCFavor("苏小萌", 1); // 使用好感度标记已选择
            }
        }, 0, ""},
        DialogueOption{"选大盘鸡吧！分量十足，能让人充满活力。", "s1_after_pick", [this](){ 
            state_.player.attr().spd += 3; 
            std::cout<<"辣香刺激，脚步更轻，SPD+3。\n"; 
            if(auto* tk=state_.task_manager.getTask("side_canteen_choice")) {
                tk->setObjective(0, "完成选择 ✓");
                // 标记已经选择过食物，防止重复选择
                state_.player.setNPCFavor("苏小萌", 1); // 使用好感度标记已选择
            }
        }, 0, ""}
    };
    su_xiaomeng.addDialogue("s1_choose", s1_choose);
    DialogueNode s1_after_pick;
    s1_after_pick.npc_text = "（苏小萌的脸上露出了灿烂的笑容）\n\n太好了！谢谢你帮我做出选择！\n\n（她开心地跳了一下）\n\n你知道吗？这是我第一次这么快就做出决定！以前我总是在这里站很久，最后还是随便选一个。\n\n（她的表情变得有些不好意思）\n\n不过...我现在还缺一瓶咖啡因灵液。你知道的，最近学习压力很大，我需要一些提神的东西。\n\n（她看向你，眼中带着期待）\n\n你能给我一瓶吗？我会给你一些好东西作为回报的！";
    s1_after_pick.options = {
        DialogueOption{"当然可以！给你咖啡因灵液。", "s1_give", [this](){
            if (state_.player.inventory().remove("caffeine_elixir",1)) {
                // 奖励：生命药水×1 与 钢勺护符
                Item hp = Item::createConsumable("health_potion","生命药水","恢复生命值的药水。",30,30);
                state_.player.inventory().add(hp,1);
                Item spoon = ItemDefinitions::createSteelSpoon();
                spoon.price = 0; // 奖励物品免费
                spoon.effect_type = "damage_multiplier"; spoon.effect_target = "失败实验体"; spoon.effect_value = 1.3f;
                state_.player.inventory().add(spoon,1);
                state_.player.addNPCFavor("林清漪",20);
                std::cout<<"【S1完成】你交付了咖啡因灵液，获得生命药水×1、钢勺护符×1。林清漪好感+20。\n";
                if(auto* tk=state_.task_manager.getTask("side_canteen_choice")) { tk->setObjective(1, "赠送咖啡因灵液 ✓"); tk->complete(); }
            } else {
                std::cout<<"你没有咖啡因灵液。\n";
                // 设置标志，表示没有物品，需要跳转到不同的对话
                state_.dialogue_memory["苏小萌"].insert("no_caffeine_elixir");
            }
        }, 0, ""},
        DialogueOption{"抱歉，我现在没有咖啡因灵液。", "s1_no_elixir", nullptr, 0, ""},
        DialogueOption{"咖啡因灵液是什么？", "s1_elixir_info", nullptr, 0, ""}
    };
    su_xiaomeng.addDialogue("s1_after_pick", s1_after_pick);
    
    // 添加给予咖啡因灵液后的对话
    DialogueNode s1_give;
    s1_give.npc_text = "（苏小萌的眼中闪烁着感激的光芒）\n\n太谢谢你了！你真是太好了！\n\n（她开心地接过咖啡因灵液）\n\n这个护符是我从家里带来的，据说能对实验失败妖造成额外伤害！\n\n（她将护符递给你）\n\n还有这瓶生命药水，也送给你！\n\n（她的表情变得认真）\n\n谢谢你帮我解决了选择困难的问题，我现在感觉好多了！";
    s1_give.options = {
        DialogueOption{"不客气，能帮到你就好！", "exit", nullptr, 0, ""},
        DialogueOption{"这个护符看起来很特别！", "exit", nullptr, 0, ""}
    };
    su_xiaomeng.addDialogue("s1_give", s1_give);
    
    // 添加没有咖啡因灵液时的对话
    DialogueNode s1_no_elixir_response;
    s1_no_elixir_response.npc_text = "（苏小萌的表情有些失望，但很快又露出了理解的笑容）\n\n哦...没关系，我理解的！\n\n（她轻轻摇了摇头）\n\n其实我也知道，咖啡因灵液确实不容易得到。\n\n（她的表情变得有些不好意思）\n\n不过如果你以后有机会的话，记得来找我哦！我真的很需要一些提神的东西。\n\n（她看向你，眼中带着期待）\n\n到时候我一定会给你很好的回报的！";
    s1_no_elixir_response.options = {
        DialogueOption{"好的，我会记住的！", "exit", nullptr, 0, ""},
        DialogueOption{"抱歉让你失望了。", "exit", nullptr, 0, ""}
    };
    su_xiaomeng.addDialogue("s1_no_elixir_response", s1_no_elixir_response);
    
    // 任务完成后的对话
    DialogueNode s1_completed;
    s1_completed.npc_text = "（苏小萌看到你，脸上露出了灿烂的笑容）\n\n嗨！谢谢你之前帮我选择食物！\n\n（她开心地跳了一下）\n\n你知道吗？自从你帮我做出选择后，我现在已经能够自己快速做决定了！\n\n（她的表情变得有些自豪）\n\n而且那个钢勺护符真的很好用，我现在吃饭都更有底气了！\n\n（她的表情变得认真）\n\n对了，我听说教学楼五区有个高数难题精，很多同学都在那里遇到了困难。如果你想要挑战智力试炼的话，可以去那里看看。\n\n（她看向你）\n\n不过要小心，那个怪物很厉害的！";
    s1_completed.options = {
        DialogueOption{"谢谢你的提醒！我去看看。", "s1_hint_task3", [this](){
            // 自动接取支线任务3：智力试炼
            if (!state_.task_manager.hasActiveTask("side_teach_wisdom") && !state_.task_manager.hasCompletedTask("side_teach_wisdom")) {
                state_.task_manager.startTask("side_teach_wisdom");
                std::cout<<"【任务接取】智力试炼。使用 task 查看详情。\n";
            }
        }, 0, ""},
        DialogueOption{"你现在还会选择困难吗？", "s1_choice_advice", nullptr, 0, ""},
        DialogueOption{"再见！", "exit", nullptr, 0, ""}
    };
    su_xiaomeng.addDialogue("s1_completed", s1_completed);
    
    // 选择建议对话
    DialogueNode s1_choice_advice;
    s1_choice_advice.npc_text = "（苏小萌的眼中闪过一丝思考）\n\n其实...还是会有一点，但是比以前好多了！\n\n（她的声音变得认真）\n\n我现在学会了几个小技巧：\n• 先列出所有选项的优点和缺点\n• 设定一个时间限制，比如5分钟内必须决定\n• 如果实在选不出来，就选第一个想到的\n\n（她看向你）\n\n最重要的是，不要害怕做错选择。每个选择都有它的价值，关键是要勇敢地迈出那一步！";
    s1_choice_advice.options = {
        DialogueOption{"很有用的建议！", "exit", nullptr, 0, ""},
        DialogueOption{"谢谢你的分享！", "exit", nullptr, 0, ""}
    };
    su_xiaomeng.addDialogue("s1_choice_advice", s1_choice_advice);
    
    // 支线任务3提示对话
    DialogueNode s1_hint_task3;
    s1_hint_task3.npc_text = "（苏小萌的眼中闪烁着鼓励的光芒）\n\n太好了！我相信你一定能够战胜那个高数难题精的！\n\n（她的声音变得神秘）\n\n听说那个怪物会出一些很难的数学题，但是如果你能答对的话，会获得很棒的奖励！\n\n（她看向你）\n\n加油！我相信你的智慧！";
    s1_hint_task3.options = {
        DialogueOption{"谢谢你的鼓励！", "exit", nullptr, 0, ""},
        DialogueOption{"我会努力的！", "exit", nullptr, 0, ""}
    };
    su_xiaomeng.addDialogue("s1_hint_task3", s1_hint_task3);
    
    // 添加没有咖啡因灵液的对话
    DialogueNode s1_no_elixir;
    s1_no_elixir.npc_text = "（苏小萌的眼中闪过一丝失望）\n\n啊...没关系，我理解。\n\n（她的声音变得有些沮丧）\n\n其实...其实我也可以去别的地方找找。只是...只是我觉得你人很好，想和你做朋友。\n\n（她看向你）\n\n如果你以后有咖啡因灵液的话，记得来找我哦！我会给你一些好东西的！";
    s1_no_elixir.options = {
        DialogueOption{"好的，我会记住的。", "exit", nullptr, 0, ""},
        DialogueOption{"你能告诉我咖啡因灵液在哪里可以找到吗？", "s1_elixir_info", nullptr, 0, ""}
    };
    su_xiaomeng.addDialogue("s1_no_elixir", s1_no_elixir);
    
    // 添加咖啡因灵液信息对话
    DialogueNode s1_elixir_info;
    s1_elixir_info.npc_text = "（苏小萌的眼中闪过一丝希望）\n\n咖啡因灵液？\n\n（她想了想）\n\n我记得...我记得在荒废北操场有一些怪物会掉落咖啡因灵液。就是那些压力黑雾，它们身上有时候会带着这种灵液。\n\n（她的声音变得兴奋）\n\n如果你能帮我找到一瓶，我会给你一个很特别的护符！那是我从家里带来的，据说能对实验失败妖造成额外伤害！\n\n（她看向你）\n\n怎么样？要不要去试试？";
    s1_elixir_info.options = {
        DialogueOption{"好的，我去试试看。", "exit", nullptr, 0, ""},
        DialogueOption{"压力黑雾在哪里？", "s1_monster_info", nullptr, 0, ""}
    };
    su_xiaomeng.addDialogue("s1_elixir_info", s1_elixir_info);
    
    // 添加怪物信息对话
    DialogueNode s1_monster_info;
    s1_monster_info.npc_text = "（苏小萌指向北边）\n\n压力黑雾在荒废北操场，就是那个废弃的操场。\n\n（她的声音变得有些害怕）\n\n那些怪物...它们很可怕，会让人感到紧张和压力。但是如果你有足够的实力，它们会掉落咖啡因灵液。\n\n（她看向你）\n\n不过要小心，它们比一般的怪物要强一些。建议你先提升一下等级，或者找一些好装备再去挑战。\n\n（她的声音变得温柔）\n\n如果你需要帮助，可以去找林清漪，她总是很乐意帮助新人的。";
    s1_monster_info.options = {
        DialogueOption{"我明白了，谢谢你的提醒。", "exit", nullptr, 0, ""},
        DialogueOption{"我会小心的。", "exit", nullptr, 0, ""}
    };
    su_xiaomeng.addDialogue("s1_monster_info", s1_monster_info);
    // 随机闲聊建议（完成S1后偶发）
    DialogueNode s1_chat;
    s1_chat.npc_text = "（小声）实验楼别硬刚，先穿厚点的护甲更稳。听说陆天宇在体育馆修训练装置，你可以去帮帮他。";
    s1_chat.options = { 
        DialogueOption{"嗯，记住了。", "exit", nullptr, 0, ""},
        DialogueOption{"陆天宇是谁？", "lu_tianyu_info", nullptr, 0, ""}
    };
    su_xiaomeng.addDialogue("chat", s1_chat);
    
    // 陆天宇信息
    DialogueNode lu_tianyu_info;
    lu_tianyu_info.npc_text = "陆天宇是体育馆的管理员，他正在修理训练装置，需要动力碎片。如果你帮他收集，他会给你很好的奖励。";
    lu_tianyu_info.options = { DialogueOption{"谢谢提醒。", "exit", nullptr, 0, ""} };
    su_xiaomeng.addDialogue("lu_tianyu_info", lu_tianyu_info);
    auto* canteen_loc = state_.map.get("canteen");
    if (canteen_loc) canteen_loc->npcs.push_back(su_xiaomeng);

    // 陆天宇 - 体育馆支线 S2
    NPC lu_tianyu("陆天宇", "一个健壮的男同学，正在体育馆里调试一台破旧的训练装置。他的额头上挂着汗珠，看起来已经工作了很久。");
    DialogueNode s2_start;
    s2_start.npc_text = "（陆天宇擦了擦额头上的汗水，看向你）\n\n哦，你是新来的吧？\n\n（他指向身后那台破旧的训练装置）\n\n我正在修理这台训练装置，但是...但是缺少一些关键的动力碎片。\n\n（他的声音变得有些沮丧）\n\n这台装置是专门用来训练毅力和坚持的，但是缺少动力碎片，它就无法正常工作。\n\n（他转向你，眼中带着希望）\n\n你能帮我收集3个动力碎片吗？我知道这很麻烦，但是...但是我真的需要帮助。\n\n💡 提示：动力碎片可以从【迷糊书虫】和【拖延小妖】身上获得，它们就在这个体育馆里。";
    s2_start.options = {
        DialogueOption{"当然可以！我来帮你收集。", "s2_turnin", [this](){
            if (!state_.task_manager.hasActiveTask("side_gym_fragments") && !state_.task_manager.hasCompletedTask("side_gym_fragments")) {
                state_.task_manager.startTask("side_gym_fragments");
                std::cout<<"【任务接取】动力碎片。使用 task 查看详情。\n";
            }
        }, 0, ""},
        DialogueOption{"动力碎片是什么？怎么获得？", "s2_hint", nullptr, 0, ""},
        DialogueOption{"这台训练装置是做什么的？", "s2_machine_info", nullptr, 0, ""},
        DialogueOption{"我再考虑下。", "exit", nullptr, 0, ""}
    };
    lu_tianyu.addDialogue("welcome", s2_start);
    
    // 设置默认对话
    lu_tianyu.setDefaultDialogue("welcome");
    
    // 添加训练装置信息对话
    DialogueNode s2_machine_info;
    s2_machine_info.npc_text = "（陆天宇的眼中闪过一丝自豪）\n\n这台训练装置？\n\n（他轻抚着装置的表面）\n\n这是专门用来训练毅力和坚持的装置。你知道的，学习不是一朝一夕的事情，需要长期的坚持和努力。\n\n（他的声音变得严肃）\n\n这台装置能够模拟各种困难的情况，帮助人们锻炼意志力。但是...但是缺少动力碎片，它就无法正常工作。\n\n（他看向你）\n\n动力碎片是装置的核心组件，只有【迷糊书虫】和【拖延小妖】身上才有。那些小妖...它们代表着我们内心的困惑和拖延。\n\n（他的声音变得坚定）\n\n只有战胜它们，才能获得动力碎片，让装置重新运转起来。";
    s2_machine_info.options = {
        DialogueOption{"我明白了，我来帮你收集动力碎片。", "s2_turnin", [this](){
            if (!state_.task_manager.hasActiveTask("side_gym_fragments") && !state_.task_manager.hasCompletedTask("side_gym_fragments")) {
                state_.task_manager.startTask("side_gym_fragments");
                std::cout<<"【任务接取】动力碎片。使用 task 查看详情。\n";
            }
        }, 0, ""},
        DialogueOption{"拖延小妖在哪里？", "s2_hint", nullptr, 0, ""},
        DialogueOption{"谢谢你的解释。", "welcome", nullptr, 0, ""}
    };
    lu_tianyu.addDialogue("s2_machine_info", s2_machine_info);
    
    // 动力碎片获得方法提示
    DialogueNode s2_hint;
    s2_hint.npc_text = "（陆天宇指向体育馆的各个角落）\n\n动力碎片...那是训练装置的核心组件。\n\n（他的声音变得严肃）\n\n只有【迷糊书虫】和【拖延小妖】身上才有这种碎片。那些小妖...它们就躲在这个体育馆的各个角落里。\n\n（他指向不同的方向）\n\n📍 位置：就在这个体育馆里\n⚔️ 怪物：迷糊书虫、拖延小妖\n📊 掉落率：50%概率\n\n（他的声音变得鼓励）\n\n现在掉落率提高了，应该很快就能收集到！而且...而且这也是对你毅力的考验。\n\n（他看向你）\n\n如果你能帮我收集到3个动力碎片，我会给你一个很特别的奖励！";
    s2_hint.options = {
        DialogueOption{"我明白了，开始收集！", "s2_turnin", [this](){
            if (!state_.task_manager.hasActiveTask("side_gym_fragments") && !state_.task_manager.hasCompletedTask("side_gym_fragments")) {
                state_.task_manager.startTask("side_gym_fragments");
                std::cout<<"【任务接取】动力碎片。使用 task 查看详情。\n";
            }
        }, 0, ""},
        DialogueOption{"拖延小妖长什么样？", "s2_monster_info", nullptr, 0, ""},
        DialogueOption{"我再考虑下。", "exit", nullptr, 0, ""}
    };
    lu_tianyu.addDialogue("s2_hint", s2_hint);
    
    // 添加怪物信息对话
    DialogueNode s2_monster_info;
    s2_monster_info.npc_text = "（陆天宇的表情变得严肃）\n\n迷糊书虫和拖延小妖...它们看起来就像一团黑色的雾气，但是有着人形的轮廓。\n\n（他的声音变得有些害怕）\n\n它们总是躲在角落里，当你靠近的时候，它们会突然出现，试图让你感到困惑和拖延。\n\n（他看向你）\n\n但是不要害怕，它们虽然看起来可怕，但是实力并不强。只要你保持清醒的头脑，就能战胜它们。\n\n（他的声音变得鼓励）\n\n而且...而且这也是对你毅力的考验。只有真正有毅力的人，才能获得动力碎片！";
    s2_monster_info.options = {
        DialogueOption{"我明白了，开始收集！", "s2_turnin", [this](){
            if (!state_.task_manager.hasActiveTask("side_gym_fragments") && !state_.task_manager.hasCompletedTask("side_gym_fragments")) {
                state_.task_manager.startTask("side_gym_fragments");
                std::cout<<"【任务接取】动力碎片。使用 task 查看详情。\n";
            }
        }, 0, ""},
        DialogueOption{"谢谢你的解释。", "welcome", nullptr, 0, ""}
    };
    lu_tianyu.addDialogue("s2_monster_info", s2_monster_info);
    
    DialogueNode s2_turnin;
    s2_turnin.npc_text = "（陆天宇停下手中的工作，转向你）\n\n怎么样？收集到了吗？\n\n（他的眼中闪烁着期待的光芒）\n\n我需要3个动力碎片才能修好这台训练装置。\n\n（他看向你）\n\n如果你已经收集到了，就交给我吧！我会给你一个很特别的奖励！";
    s2_turnin.options = {
        DialogueOption{"是的，我收集到了3个动力碎片！", "s2_check_fragments", [this](){
            // 检查是否有足够的动力碎片，设置记忆标志
            if (state_.player.inventory().quantity("power_fragment")>=3) {
                state_.dialogue_memory["陆天宇"].insert("has_enough_fragments");
            } else {
                state_.dialogue_memory["陆天宇"].insert("not_enough_fragments");
            }
        }, 0, ""},
        DialogueOption{"还没有，我需要继续收集。", "s2_continue", nullptr, 0, ""},
        DialogueOption{"动力碎片很难获得，有什么技巧吗？", "s2_tips", nullptr, 0, ""}
    };
    lu_tianyu.addDialogue("s2_turnin", s2_turnin);
    
    // 检查动力碎片数量的对话
    DialogueNode s2_check_fragments;
    s2_check_fragments.npc_text = "（陆天宇看了看你，发现你还没有收集到足够的动力碎片）\n\n看起来你还需要继续努力。\n\n（他数了数你手中的动力碎片）\n\n目前你有 0 个动力碎片，还需要 3 个。\n\n（他的声音变得鼓励）\n\n没关系，慢慢来。动力碎片确实不容易获得，但是...但是这也是对你毅力的考验。\n\n（他指向体育馆的各个角落）\n\n记住，只有【迷糊书虫】和【拖延小妖】身上才有动力碎片。它们就躲在这个体育馆的各个角落里。\n\n（他的声音变得坚定）\n\n我相信你一定能做到的！";
    s2_check_fragments.options = {
        DialogueOption{"我明白了，继续收集。", "s2_turnin", nullptr, 0, ""},
        DialogueOption{"动力碎片很难获得，有什么技巧吗？", "s2_tips", nullptr, 0, ""}
    };
    lu_tianyu.addDialogue("s2_check_fragments", s2_check_fragments);
    
    // 任务完成后的对话
    DialogueNode s2_done;
    s2_done.npc_text = "（陆天宇的眼中闪烁着兴奋的光芒）\n\n太好了！训练装置终于可以正常工作了！\n\n（他激动地拍了拍你的肩膀）\n\n谢谢你！这个负重护腕是我精心制作的，能增强你的体魄。\n\n（他的表情变得认真）\n\n对了，我听说实验楼有个实验失败妖，很多同学都在那里遇到了困难。如果你想要挑战实验试炼的话，可以去那里看看。\n\n（他看向你）\n\n不过要小心，那个怪物会召唤小怪，很危险的！";
    s2_done.options = {
        DialogueOption{"谢谢你的提醒！我去看看。", "s2_hint_task4", [this](){
            // 自动接取支线任务4：实验失败妖挑战
            if (!state_.task_manager.hasActiveTask("side_lab_challenge") && !state_.task_manager.hasCompletedTask("side_lab_challenge")) {
                state_.task_manager.startTask("side_lab_challenge");
                std::cout<<"【任务接取】挑战实验失败妖。使用 task 查看详情。\n";
            }
        }, 0, ""},
        DialogueOption{"这个训练装置现在能做什么？", "s2_machine_working", nullptr, 0, ""},
        DialogueOption{"再见！", "exit", nullptr, 0, ""}
    };
    lu_tianyu.addDialogue("s2_done", s2_done);
    
    // 支线任务4提示对话
    DialogueNode s2_hint_task4;
    s2_hint_task4.npc_text = "（陆天宇的眼中闪烁着鼓励的光芒）\n\n太好了！我相信你一定能够战胜那个实验失败妖的！\n\n（他的声音变得神秘）\n\n听说那个怪物会召唤很多小怪，但是如果你能击败它的话，会获得很棒的奖励！\n\n（他看向你）\n\n加油！我相信你的实力！";
    s2_hint_task4.options = {
        DialogueOption{"谢谢你的鼓励！", "exit", nullptr, 0, ""},
        DialogueOption{"我会努力的！", "exit", nullptr, 0, ""}
    };
    lu_tianyu.addDialogue("s2_hint_task4", s2_hint_task4);
    
    // 训练装置工作状态对话
    DialogueNode s2_machine_working;
    s2_machine_working.npc_text = "（陆天宇的眼中闪烁着自豪的光芒）\n\n现在这台训练装置可以正常工作了！\n\n（他指向装置）\n\n它可以模拟各种困难的情况，帮助人们锻炼意志力。很多同学都来这里训练，效果很好！\n\n（他的声音变得温柔）\n\n如果你以后需要锻炼毅力，随时可以来这里找我！";
    s2_machine_working.options = {
        DialogueOption{"谢谢！", "exit", nullptr, 0, ""},
        DialogueOption{"我会记住的！", "exit", nullptr, 0, ""}
    };
    lu_tianyu.addDialogue("s2_machine_working", s2_machine_working);
    
    // 添加继续收集对话
    DialogueNode s2_continue;
    s2_continue.npc_text = "（陆天宇的眼中闪过一丝失望，但很快又变得鼓励）\n\n没关系，慢慢来。\n\n（他的声音变得温柔）\n\n我知道动力碎片很难获得，但是...但是这也是对你毅力的考验。\n\n（他看向你）\n\n记住，只有真正有毅力的人，才能获得动力碎片。不要放弃，继续努力！\n\n（他的声音变得坚定）\n\n我相信你一定能做到的！";
    s2_continue.options = {
        DialogueOption{"谢谢你的鼓励，我会继续努力的！", "exit", nullptr, 0, ""},
        DialogueOption{"有什么技巧可以提高获得概率吗？", "s2_tips", nullptr, 0, ""}
    };
    lu_tianyu.addDialogue("s2_continue", s2_continue);
    
    // 添加收集技巧对话
    DialogueNode s2_tips;
    s2_tips.npc_text = "（陆天宇想了想）\n\n技巧...其实没有什么特别的技巧。\n\n（他的声音变得严肃）\n\n但是...但是我知道一些经验。\n\n• 保持耐心：动力碎片的掉落率确实不高，但是多打几次总会有的\n• 保持清醒：拖延小妖会试图让你感到懒惰，但是要保持清醒的头脑\n• 保持毅力：这是最重要的，只有真正有毅力的人才能获得动力碎片\n\n（他看向你）\n\n记住，这不是在玩游戏，这是对你毅力的真正考验！";
    s2_tips.options = {
        DialogueOption{"我明白了，谢谢你的建议。", "exit", nullptr, 0, ""},
        DialogueOption{"我会记住这些的。", "exit", nullptr, 0, ""}
    };
    lu_tianyu.addDialogue("s2_tips", s2_tips);
    // 完成S2后的偶发提示
    DialogueNode s2_chat;
    s2_chat.npc_text = "没拿到学士袍也别怕，先把'被子'披上，扛得住就有输出。听说苏小萌在食堂遇到了选择困难，你可以去帮帮她。";
    s2_chat.options = { 
        DialogueOption{"受教了。", "exit", nullptr, 0, ""},
        DialogueOption{"苏小萌怎么了？", "su_xiaomeng_info", nullptr, 0, ""}
    };
    lu_tianyu.addDialogue("chat", s2_chat);
    
    // 苏小萌信息
    DialogueNode su_xiaomeng_info;
    su_xiaomeng_info.npc_text = "苏小萌在食堂纠结吃什么，如果你帮她做选择，她会给你一个很有用的护符。";
    su_xiaomeng_info.options = { DialogueOption{"谢谢提醒。", "exit", nullptr, 0, ""} };
    lu_tianyu.addDialogue("su_xiaomeng_info", su_xiaomeng_info);
    auto* gym_loc2 = state_.map.get("gymnasium");
    if (gym_loc2) gym_loc2->npcs.push_back(lu_tianyu);
    
    
    
    
    
    // 将林清漪添加到图书馆
    auto* library = state_.map.get("library");
    if (library) {
        library->npcs.push_back(lin_qingyi);
    }
    
    // 钱道然 - 商店NPC（全新实现）
    NPC qian_daoran("钱道然", "一位精明的商人，专门经营修仙用品。他穿着华丽的商服，眼中闪烁着精明的光芒，总是能准确判断顾客的需求。");
    
    // 主菜单对话
    DialogueNode qian_main_menu;
    qian_main_menu.npc_text = "（钱道然的眼神闪过一丝精明）\n\n小友，要买点东西吗？\n\n（他神秘地压低声音）\n\n海大e卡通可是宝贝！有了它，在我这里购物能享受9折优惠！不过想得到它可不容易……只有在教学区地图中击败强大的怪物，才有小概率掉落。";
    qian_main_menu.options = {
        DialogueOption{"打开商店", "shop", nullptr, 0, "shop"},
        DialogueOption{"再见", "exit", nullptr, 0, ""}
    };
    qian_daoran.addDialogue("main_menu", qian_main_menu);
    
    // 设置主菜单为默认对话
    qian_daoran.setDefaultDialogue("main_menu");
    
    
    // 将钱道然添加到图书馆
    if (library) {
        library->npcs.push_back(qian_daoran);
    }
    
    //（删除旧的重复苏小萌/陆天宇定义，以避免重定义）
}

void Game::createItems() {
    // 物品定义已移至ItemDefinitions.hpp中统一管理
    // 这里不再需要重复定义物品，商店物品由ShopSystem管理
}

void Game::createTasks() {
    // ==================== 主线任务 ====================
    
    // 主线任务 M1：进入秘境
    std::vector<TaskReward> m1_rewards = {
        {"student_uniform", "普通学子服", 1, 10, 0, "基础护甲装备"},
        {"bamboo_notes", "竹简笔记", 1, 10, 0, "基础饰品装备"}
    };
    Task m1("m1_enter_secret", "进入秘境", "与林清漪对话，了解秘境情况并获得新手装备", 
            TaskType::MAIN, m1_rewards);
    m1.addObjective("与林清漪对话");
    m1.addObjective("获得新手装备");
    state_.task_manager.addTask(m1);
    
    // 主线任务 M2：完成试炼
    std::vector<TaskReward> m2_rewards = {
        {"", "", 0, 50, 100, "完成试炼的奖励"}
    };
    Task m2("m2_trials", "完成试炼", "完成至少3个区域的试炼（教学楼/食堂/体育馆/实验楼/辩论厅）", 
            TaskType::MAIN, m2_rewards);
    m2.addObjective("完成教学楼试炼");
    m2.addObjective("完成食堂试炼");
    m2.addObjective("完成体育馆试炼");
    m2.addObjective("完成实验楼试炼");
    m2.addObjective("完成辩论厅试炼");
    state_.task_manager.addTask(m2);
    
    // 主线任务 M3：挑战文心潭
    std::vector<TaskReward> m3_rewards = {
        {"wenxin_key", "文心秘钥", 1, 100, 200, "打开真相之门的钥匙"}
    };
    Task m3("m3_wenxin_challenge", "挑战文心潭", "三战心魔，获得文心秘钥", 
            TaskType::MAIN, m3_rewards);
    m3.addObjective("击败文献综述怪");
    m3.addObjective("击败实验失败妖·复苏");
    m3.addObjective("击败答辩压力魔");
    m3.addObjective("获得文心秘钥");
    state_.task_manager.addTask(m3);
    
    // ==================== 支线任务 ====================
    
    // 支线任务 S1：食堂选择
    std::vector<TaskReward> canteen_rewards = {
        {"steel_spoon", "钢勺护符", 1, 20, 50, "对实验楼怪物伤害×1.3"},
        {"", "", 0, 10, 20, "修为+1"}
    };
    Task canteen_task("side_canteen_choice", "食堂选择", "帮助苏小萌解决选择困难，获得钢勺护符", 
                     TaskType::SIDE, canteen_rewards);
    canteen_task.addObjective("与苏小萌对话");
    canteen_task.addObjective("帮助她做出选择");
    canteen_task.addObjective("获得钢勺护符");
    state_.task_manager.addTask(canteen_task);
    
    // 支线任务 S2：动力碎片收集
    std::vector<TaskReward> gym_rewards = {
        {"weight_bracelet", "负重护腕", 1, 30, 60, "ATK+1的护腕"},
        {"", "", 0, 15, 30, "修为+1"}
    };
    Task gym_task("side_gym_fragments", "动力碎片", "帮助陆天宇收集3个动力碎片，完成训练装置。在体育馆击败拖延小妖有20%概率获得。", 
                 TaskType::SIDE, gym_rewards);
    gym_task.addObjective("与陆天宇对话");
    gym_task.addObjective("在体育馆击败拖延小妖");
    gym_task.addObjective("收集3个动力碎片（20%掉落率）");
    gym_task.addObjective("获得负重护腕");
    state_.task_manager.addTask(gym_task);
    
    // 支线任务 S3：智力试炼
    std::vector<TaskReward> teach_rewards = {
        {"wisdom_pen", "启智笔", 1, 40, 80, "智力试炼的奖励"},
        {"", "", 0, 20, 40, "修为+1"}
    };
    Task teach_task("side_teach_wisdom", "智力试炼", "在教学楼击败高数难题精，获得启智笔", 
                   TaskType::SIDE, teach_rewards);
    teach_task.addObjective("在教学楼找到高数难题精");
    teach_task.addObjective("击败高数难题精");
    teach_task.addObjective("获得启智笔");
    state_.task_manager.addTask(teach_task);
    
    // 支线任务 S4：实验失败妖挑战
    std::vector<TaskReward> lab_rewards = {
        {"lab_coat", "实验服", 1, 25, 50, "DEF+2的实验服"},
        {"", "", 0, 15, 25, "修为+1"}
    };
    Task lab_task("side_lab_challenge", "挑战实验失败妖", "在实验楼挑战实验失败妖，获得实验服", 
                 TaskType::SIDE, lab_rewards);
    lab_task.addObjective("进入实验楼");
    lab_task.addObjective("找到实验失败妖");
    lab_task.addObjective("击败实验失败妖");
    lab_task.addObjective("获得实验服");
    state_.task_manager.addTask(lab_task);
    
    // 支线任务 S5：答辩对话挑战
    std::vector<TaskReward> debate_rewards = {
        {"debate_feather_fan", "辩峰羽扇", 1, 40, 80, "SPD+3的羽扇武器"},
        {"", "", 0, 20, 40, "修为+1"}
    };
    Task debate_task("side_debate_challenge", "答辩对话挑战", "在树下空间击败答辩紧张魔，回答3个问题，答对2题以上获得辩峰羽扇", 
                    TaskType::SIDE, debate_rewards);
    debate_task.addObjective("进入树下空间");
    debate_task.addObjective("击败答辩紧张魔");
    debate_task.addObjective("回答3个问题");
    debate_task.addObjective("答对2题以上");
    debate_task.addObjective("获得辩峰羽扇");
    state_.task_manager.addTask(debate_task);
}

void Game::initializeNPCDialogues() {
    // 为所有地图位置的NPC添加对话内容
    // 通过位置ID直接获取可修改的位置引用
    auto locations = state_.map.allLocations();
    for (const auto& location : locations) {
        auto* loc = state_.map.get(location.id);
        if (!loc) continue;
        
        for (auto& npc : loc->npcs) {
            // 检查NPC是否已经有完整的对话数据（从存档加载）
            // 如果对话数据为空或者不完整，则需要重新初始化
            bool needs_initialization = npc.getDialogues().empty();
            
            // 对于林清漪，检查是否有完整的对话系统（应该有多个子对话）
            if (npc.name() == "林清漪" && !needs_initialization) {
                // 检查是否有必要的子对话ID
                bool has_required_dialogues = npc.getDialogues().find("why_here") != npc.getDialogues().end() &&
                                            npc.getDialogues().find("wenxin_info") != npc.getDialogues().end() &&
                                            npc.getDialogues().find("equipment") != npc.getDialogues().end() &&
                                            npc.getDialogues().find("teaching_area_info") != npc.getDialogues().end() &&
                                            npc.getDialogues().find("debate_challenge") != npc.getDialogues().end();
                if (!has_required_dialogues) {
                    needs_initialization = true;
                }
            }
            
            // 对于陆天宇，检查是否有完整的对话系统
            if (npc.name() == "陆天宇" && !needs_initialization) {
                // 检查是否有必要的子对话ID
                bool has_required_dialogues = npc.getDialogues().find("s2_turnin") != npc.getDialogues().end() &&
                                            npc.getDialogues().find("s2_hint") != npc.getDialogues().end() &&
                                            npc.getDialogues().find("s2_machine_info") != npc.getDialogues().end();
                if (!has_required_dialogues) {
                    needs_initialization = true;
                }
            }
            
            // 对于苏小萌，检查是否有完整的对话系统（应该有7个对话）
            if (npc.name() == "苏小萌" && !needs_initialization) {
                // 检查是否有必要的对话ID
                bool has_required_dialogues = npc.getDialogues().find("welcome") != npc.getDialogues().end() &&
                                            npc.getDialogues().find("s1_choose") != npc.getDialogues().end() &&
                                            npc.getDialogues().find("s1_after_pick") != npc.getDialogues().end() &&
                                            npc.getDialogues().find("s1_give") != npc.getDialogues().end();
                if (!has_required_dialogues) {
                    needs_initialization = true;
                }
            }

            if (needs_initialization) {
                if (npc.name() == "林清漪") {
                    initializeLinQingyiDialogues(npc);
                } else if (npc.name() == "钱道然") {
                    initializeQianDaoranDialogues(npc);
                } else if (npc.name() == "陆天宇") {
                    initializeLuTianyuDialogues(npc);
                } else if (npc.name() == "苏小萌") {
                    initializeSuXiaomengDialogues(npc);
                }
            }
        }
    }
}

void Game::initializeLinQingyiDialogues(NPC& npc) {
    // 主菜单对话
    DialogueNode main_menu;
    main_menu.npc_text = "（林清漪缓缓转过身，眼中闪烁着温和的光芒）\n\n欢迎来到文心秘境，年轻的学子。我是林清漪，这里的守护者。\n\n（她轻抚着手中的古籍）\n\n这里，是每个海大学子内心世界的投影。那些无形的学业压力、考试焦虑、选择困难...在这里都化作了可见的心魔。\n\n（她看向你，眼中带着关切）\n\n你看起来有些迷茫，但不要担心。每一个来到这里的人，最终都会找到属于自己的答案。\n\n有什么我可以帮助你的吗？";
    main_menu.options = {
        DialogueOption{"为什么会来到这里？", "why_here", nullptr, 0, "why_here"},
        DialogueOption{"了解文心坛", "wenxin_info", nullptr, 0, "wenxin_info"},
        DialogueOption{"给予装备", "equipment", nullptr, 0, "equipment"},
        DialogueOption{"了解教学区", "teaching_area_info", nullptr, 0, "teaching_area_info"},
        DialogueOption{"支线5叙述", "debate_challenge", nullptr, 0, "debate_challenge"},
        DialogueOption{"再见", "exit", nullptr, 0, ""}
    };
    npc.addDialogue("main_menu", main_menu);
    npc.setDefaultDialogue("main_menu");
    
    // 1. 为什么会来到这里？
    DialogueNode why_here;
    why_here.npc_text = "（林清漪的眼中闪过一丝深邃的光芒）\n\n为什么会来到这里...这是一个很好的问题。\n\n每一个来到文心秘境的人，都是因为内心深处的某种渴望或困惑。\n\n（她轻抚着手中的古籍）\n\n也许你正在为学业而焦虑，也许你正在为未来而迷茫，也许你正在为选择而犹豫...\n\n但请记住，这里不是逃避的地方，而是成长的地方。\n\n（她温柔地看着你）\n\n每一个心魔，都是你内心的一部分。只有面对它们，理解它们，你才能真正地成长。";
    why_here.options = {
        DialogueOption{"我明白了，谢谢您的解释。", "main_menu", nullptr, 0, ""}
    };
    npc.addDialogue("why_here", why_here);
    
    // 2. 了解文心坛
    DialogueNode wenxin_info;
    wenxin_info.npc_text = "（林清漪指向远方）\n\n文心坛，那是这个秘境的核心。\n\n（她的眼中闪过一丝敬畏）\n\n那里汇聚着所有海大学子的心魔，是最危险，也是最珍贵的地方。\n\n只有真正勇敢的人，才能到达那里，面对最终的试炼。\n\n（她转向你）\n\n但现在的你，还需要更多的历练。先去教学区看看吧，那里有你需要的一切。";
    wenxin_info.options = {
        DialogueOption{"我会努力的。", "main_menu", nullptr, 0, ""}
    };
    npc.addDialogue("wenxin_info", wenxin_info);
    
    // 3. 给予装备
    DialogueNode equipment;
    equipment.npc_text = "（林清漪从书架上取下一套装备）\n\n这些装备，是我为每一个来到这里的学子准备的。\n\n（她递给你一套学子服和一本笔记）\n\n虽然它们看起来很普通，但在这个秘境中，它们会是你最可靠的伙伴。\n\n（她温柔地笑着）\n\n记住，真正的力量不在于装备，而在于你的内心。";
    equipment.options = {
        DialogueOption{"谢谢您，我会好好珍惜的。", "equipment_guide", nullptr, 0, ""}
    };
    npc.addDialogue("equipment", equipment);
    
    // 4. 了解教学区
    DialogueNode teaching_area_info;
    teaching_area_info.npc_text = "（林清漪指向教学区的方向）\n\n教学区，那是你成长的地方。\n\n（她的眼中闪烁着智慧的光芒）\n\n在那里，你会遇到各种挑战，也会学到很多知识。\n\n每一个教室，每一间实验室，都蕴含着不同的智慧。\n\n（她转向你）\n\n去吧，去探索，去学习，去成长。当你准备好了，文心坛的大门就会为你打开。";
    teaching_area_info.options = {
        DialogueOption{"我明白了，谢谢您的指导。", "main_menu", nullptr, 0, ""}
    };
    npc.addDialogue("teaching_area_info", teaching_area_info);
    
    // 5. 支线5叙述
    DialogueNode debate_challenge;
    debate_challenge.npc_text = "（林清漪的眼中闪过一丝期待）\n\n支线任务5...那是一个特殊的挑战。\n\n（她轻抚着手中的古籍）\n\n在文心坛的深处，有一个特殊的试炼等待着勇敢的学子。\n\n那是一个关于辩论的挑战，需要你运用智慧和勇气。\n\n（她看向你）\n\n如果你准备好了，就去文心坛寻找那个试炼吧。";
    debate_challenge.options = {
        DialogueOption{"我会去尝试的。", "main_menu", nullptr, 0, ""}
    };
    npc.addDialogue("debate_challenge", debate_challenge);
    
    // 装备指导对话
    DialogueNode equipment_guide;
    equipment_guide.npc_text = "（林清漪满意地点点头）\n\n很好，你接受了这些装备。\n\n（她指向你的装备）\n\n这套学子服会保护你，这本笔记会指引你。\n\n（她温柔地笑着）\n\n记住，在这个秘境中，装备只是辅助，真正的力量来自于你的内心。\n\n现在，去探索吧，去成长吧。当你准备好了，文心坛的大门就会为你打开。";
    equipment_guide.options = {
        DialogueOption{"我明白了，谢谢您的指导。", "main_menu", nullptr, 0, ""}
    };
    npc.addDialogue("equipment_guide", equipment_guide);
    
    // 接受装备后的对话
    DialogueNode accept_equipment;
    accept_equipment.npc_text = "（林清漪满意地点点头）\n\n很好，你接受了这些装备。\n\n（她指向你的装备）\n\n这套学子服会保护你，这本笔记会指引你。\n\n（她温柔地笑着）\n\n记住，在这个秘境中，装备只是辅助，真正的力量来自于你的内心。\n\n现在，去探索吧，去成长吧。当你准备好了，文心坛的大门就会为你打开。";
    accept_equipment.options = {
        DialogueOption{"我明白了，谢谢您的指导。", "main_menu", nullptr, 0, ""}
    };
    npc.addDialogue("accept_equipment", accept_equipment);
}

void Game::initializeQianDaoranDialogues(NPC& npc) {
    // 钱道然的主菜单对话
    DialogueNode main_menu;
    main_menu.npc_text = "（钱道然抬起头，眼中闪烁着商人的精明）\n\n欢迎光临我的商店！我是钱道然，这里的商人。\n\n我这里有很多好东西，从基础装备到高级装备，应有尽有。\n\n有什么我可以帮助你的吗？";
    main_menu.options = {
        DialogueOption{"查看商店", "shop", nullptr, 0, "shop"},
        DialogueOption{"了解商品", "goods_info", nullptr, 0, "goods_info"},
        DialogueOption{"战斗经验制度", "exp_system_info", nullptr, 0, "exp_system_info"},
        DialogueOption{"离开", "main_menu", nullptr, 0, ""}
    };
    npc.addDialogue("main_menu", main_menu);
    npc.setDefaultDialogue("main_menu");
    
    // 了解商品
    DialogueNode goods_info;
    goods_info.npc_text = "（钱道然眼中闪过一丝得意）\n\n我的商品？那可是精心挑选的！\n\n（他指向货架）\n\n从基础的生命药水，到高级的装备，应有尽有。\n\n（他神秘地压低声音）\n\n而且，如果你有海大e卡通，还能享受9折优惠！\n\n（他看向你）\n\n想要什么，尽管说！";
    goods_info.options = {
        DialogueOption{"查看商店", "shop", nullptr, 0, "shop"},
        DialogueOption{"我明白了", "main_menu", nullptr, 0, ""}
    };
    npc.addDialogue("goods_info", goods_info);
    
    // 战斗经验制度说明
    DialogueNode exp_system_info;
    exp_system_info.npc_text = "（钱道然放下手中的账本，眼中闪烁着商人的智慧）\n\n啊，你问战斗经验制度...作为商人，我对这些规则了如指掌。\n\n（他整理了一下衣襟，声音变得专业）\n\n在秘境中，战斗经验的计算遵循以下规则：\n\n【越级挑战奖励】\n• 挑战同等级怪物：获得100%经验值\n• 越1级挑战：获得120%经验值（20%奖励）\n• 越2级挑战：获得150%经验值（50%奖励）\n• 越3级挑战：获得180%经验值（80%奖励）\n• 越4级挑战：获得200%经验值（100%奖励）\n• 越5级及以上：获得250%经验值（150%奖励）\n\n【低等级惩罚】\n• 挑战低1级怪物：获得50%经验值（50%惩罚）\n• 挑战低2级怪物：获得25%经验值（75%惩罚）\n• 挑战低3级怪物：获得10%经验值（90%惩罚）\n• 挑战低4级及以上：获得5%经验值（95%惩罚）\n\n（他的声音变得兴奋）\n\n所以，如果你想要快速升级，就勇敢地挑战那些高等级怪物吧！\n\n（他指向商店）\n\n当然，如果你需要生命药水或复活符来保证安全，我这里都有！\n\n（他看向你）\n\n还有什么其他需要帮助的吗？";
    exp_system_info.options = {
        DialogueOption{"我明白了，谢谢您的解释。", "main_menu", nullptr, 0, ""},
        DialogueOption{"查看商店", "shop", nullptr, 0, "shop"}
    };
    npc.addDialogue("exp_system_info", exp_system_info);
    
    // 商店对话
    DialogueNode shop;
    shop.npc_text = "（钱道然搓着手，眼中闪烁着商人的精明）\n\n欢迎来到我的商店！\n\n（他指向货架）\n\n这里有很多好东西，从基础的生命药水到高级的装备，应有尽有。\n\n（他神秘地压低声音）\n\n而且，如果你有海大e卡通，还能享受9折优惠！\n\n（他看向你）\n\n想要什么，尽管说！";
    shop.options = {
        DialogueOption{"我明白了", "main_menu", nullptr, 0, ""}
    };
    npc.addDialogue("shop", shop);
}

void Game::initializeLuTianyuDialogues(NPC& npc) {
    // 陆天宇 - 体育馆支线 S2 完整对话系统
    
    // 主对话 - welcome
    DialogueNode s2_start;
    s2_start.npc_text = "（陆天宇擦了擦额头上的汗水，看向你）\n\n哦，你是新来的吧？\n\n（他指向身后那台破旧的训练装置）\n\n我正在修理这台训练装置，但是...但是缺少一些关键的动力碎片。\n\n（他的声音变得有些沮丧）\n\n这台装置是专门用来训练毅力和坚持的，但是缺少动力碎片，它就无法正常工作。\n\n（他转向你，眼中带着希望）\n\n你能帮我收集3个动力碎片吗？我知道这很麻烦，但是...但是我真的需要帮助。\n\n💡 提示：动力碎片可以从【迷糊书虫】和【拖延小妖】身上获得，它们就在这个体育馆里。";
    s2_start.options = {
        DialogueOption{"当然可以！我来帮你收集。", "s2_turnin", [this](){
            if (!state_.task_manager.hasActiveTask("side_gym_fragments") && !state_.task_manager.hasCompletedTask("side_gym_fragments")) {
                state_.task_manager.startTask("side_gym_fragments");
                std::cout<<"【任务接取】动力碎片。使用 task 查看详情。\n";
            }
        }, 0, ""},
        DialogueOption{"动力碎片是什么？怎么获得？", "s2_hint", nullptr, 0, ""},
        DialogueOption{"这台训练装置是做什么的？", "s2_machine_info", nullptr, 0, ""},
        DialogueOption{"我再考虑下。", "exit", nullptr, 0, ""}
    };
    npc.addDialogue("welcome", s2_start);
    npc.setDefaultDialogue("welcome");
    
    // 训练装置信息对话
    DialogueNode s2_machine_info;
    s2_machine_info.npc_text = "（陆天宇的眼中闪过一丝自豪）\n\n这台训练装置？\n\n（他轻抚着装置的表面）\n\n这是专门用来训练毅力和坚持的装置。你知道的，学习不是一朝一夕的事情，需要长期的坚持和努力。\n\n（他的声音变得严肃）\n\n这台装置能够模拟各种困难的情况，帮助人们锻炼意志力。但是...但是缺少动力碎片，它就无法正常工作。\n\n（他看向你）\n\n动力碎片是装置的核心组件，只有【迷糊书虫】和【拖延小妖】身上才有。那些小妖...它们代表着我们内心的困惑和拖延。\n\n（他的声音变得坚定）\n\n只有战胜它们，才能获得动力碎片，让装置重新运转起来。";
    s2_machine_info.options = {
        DialogueOption{"我明白了，我来帮你收集动力碎片。", "s2_turnin", [this](){
            if (!state_.task_manager.hasActiveTask("side_gym_fragments") && !state_.task_manager.hasCompletedTask("side_gym_fragments")) {
                state_.task_manager.startTask("side_gym_fragments");
                std::cout<<"【任务接取】动力碎片。使用 task 查看详情。\n";
            }
        }, 0, ""},
        DialogueOption{"拖延小妖在哪里？", "s2_hint", nullptr, 0, ""},
        DialogueOption{"谢谢你的解释。", "welcome", nullptr, 0, ""}
    };
    npc.addDialogue("s2_machine_info", s2_machine_info);
    
    // 动力碎片获得方法提示
    DialogueNode s2_hint;
    s2_hint.npc_text = "（陆天宇指向体育馆的各个角落）\n\n动力碎片...那是训练装置的核心组件。\n\n（他的声音变得严肃）\n\n只有【迷糊书虫】和【拖延小妖】身上才有这种碎片。那些小妖...它们就躲在这个体育馆的各个角落里。\n\n（他指向不同的方向）\n\n📍 位置：就在这个体育馆里\n⚔️ 怪物：迷糊书虫、拖延小妖\n📊 掉落率：50%概率\n\n（他的声音变得鼓励）\n\n现在掉落率提高了，应该很快就能收集到！而且...而且这也是对你毅力的考验。\n\n（他看向你）\n\n如果你能帮我收集到3个动力碎片，我会给你一个很特别的奖励！";
    s2_hint.options = {
        DialogueOption{"我明白了，开始收集！", "s2_turnin", [this](){
            if (!state_.task_manager.hasActiveTask("side_gym_fragments") && !state_.task_manager.hasCompletedTask("side_gym_fragments")) {
                state_.task_manager.startTask("side_gym_fragments");
                std::cout<<"【任务接取】动力碎片。使用 task 查看详情。\n";
            }
        }, 0, ""},
        DialogueOption{"拖延小妖长什么样？", "s2_monster_info", nullptr, 0, ""},
        DialogueOption{"我再考虑下。", "exit", nullptr, 0, ""}
    };
    npc.addDialogue("s2_hint", s2_hint);
    
    // 怪物信息对话
    DialogueNode s2_monster_info;
    s2_monster_info.npc_text = "（陆天宇的表情变得严肃）\n\n迷糊书虫和拖延小妖...它们看起来就像一团黑色的雾气，但是有着人形的轮廓。\n\n（他的声音变得有些害怕）\n\n它们总是躲在角落里，当你靠近的时候，它们会突然出现，试图让你感到困惑和拖延。\n\n（他看向你）\n\n但是不要害怕，它们虽然看起来可怕，但是实力并不强。只要你保持清醒的头脑，就能战胜它们。\n\n（他的声音变得鼓励）\n\n而且...而且这也是对你毅力的考验。只有真正有毅力的人，才能获得动力碎片！";
    s2_monster_info.options = {
        DialogueOption{"我明白了，开始收集！", "s2_turnin", [this](){
            if (!state_.task_manager.hasActiveTask("side_gym_fragments") && !state_.task_manager.hasCompletedTask("side_gym_fragments")) {
                state_.task_manager.startTask("side_gym_fragments");
                std::cout<<"【任务接取】动力碎片。使用 task 查看详情。\n";
            }
        }, 0, ""},
        DialogueOption{"谢谢你的解释。", "welcome", nullptr, 0, ""}
    };
    npc.addDialogue("s2_monster_info", s2_monster_info);
    
    // 任务交付对话
    DialogueNode s2_turnin;
    s2_turnin.npc_text = "（陆天宇停下手中的工作，转向你）\n\n怎么样？收集到了吗？\n\n（他的眼中闪烁着期待的光芒）\n\n我需要3个动力碎片才能修好这台训练装置。\n\n（他看向你）\n\n如果你已经收集到了，就交给我吧！我会给你一个很特别的奖励！";
    s2_turnin.options = {
        DialogueOption{"是的，我收集到了3个动力碎片！", "s2_check_fragments", [this](){
            // 检查是否有足够的动力碎片，设置记忆标志
            if (state_.player.inventory().quantity("power_fragment")>=3) {
                state_.dialogue_memory["陆天宇"].insert("has_enough_fragments");
            } else {
                state_.dialogue_memory["陆天宇"].insert("not_enough_fragments");
            }
        }, 0, ""},
        DialogueOption{"还没有，我需要继续收集。", "s2_continue", nullptr, 0, ""},
        DialogueOption{"动力碎片很难获得，有什么技巧吗？", "s2_tips", nullptr, 0, ""}
    };
    npc.addDialogue("s2_turnin", s2_turnin);
    
    // 检查动力碎片数量的对话
    DialogueNode s2_check_fragments;
    s2_check_fragments.npc_text = "（陆天宇看了看你，发现你还没有收集到足够的动力碎片）\n\n看起来你还需要继续努力。\n\n（他数了数你手中的动力碎片）\n\n目前你有 0 个动力碎片，还需要 3 个。\n\n（他的声音变得鼓励）\n\n没关系，慢慢来。动力碎片确实不容易获得，但是...但是这也是对你毅力的考验。\n\n（他指向体育馆的各个角落）\n\n记住，只有【迷糊书虫】和【拖延小妖】身上才有动力碎片。它们就躲在这个体育馆的各个角落里。\n\n（他的声音变得坚定）\n\n我相信你一定能做到的！";
    s2_check_fragments.options = {
        DialogueOption{"我明白了，继续收集。", "s2_turnin", nullptr, 0, ""},
        DialogueOption{"动力碎片很难获得，有什么技巧吗？", "s2_tips", nullptr, 0, ""}
    };
    npc.addDialogue("s2_check_fragments", s2_check_fragments);
    
    // 任务完成后的对话
    DialogueNode s2_done;
    s2_done.npc_text = "（陆天宇的眼中闪烁着兴奋的光芒）\n\n太好了！训练装置终于可以正常工作了！\n\n（他激动地拍了拍你的肩膀）\n\n谢谢你！这个负重护腕是我精心制作的，能增强你的体魄。\n\n（他的表情变得认真）\n\n对了，我听说实验楼有个实验失败妖，很多同学都在那里遇到了困难。如果你想要挑战实验试炼的话，可以去那里看看。\n\n（他看向你）\n\n不过要小心，那个怪物会召唤小怪，很危险的！";
    s2_done.options = {
        DialogueOption{"谢谢你的提醒！我去看看。", "s2_hint_task4", [this](){
            // 自动接取支线任务4：实验失败妖挑战
            if (!state_.task_manager.hasActiveTask("side_lab_challenge") && !state_.task_manager.hasCompletedTask("side_lab_challenge")) {
                state_.task_manager.startTask("side_lab_challenge");
                std::cout<<"【任务接取】挑战实验失败妖。使用 task 查看详情。\n";
            }
        }, 0, ""},
        DialogueOption{"这个训练装置现在能做什么？", "s2_machine_working", nullptr, 0, ""},
        DialogueOption{"再见！", "exit", nullptr, 0, ""}
    };
    npc.addDialogue("s2_done", s2_done);
    
    // 支线任务4提示对话
    DialogueNode s2_hint_task4;
    s2_hint_task4.npc_text = "（陆天宇的眼中闪烁着鼓励的光芒）\n\n太好了！我相信你一定能够战胜那个实验失败妖的！\n\n（他的声音变得神秘）\n\n听说那个怪物会召唤很多小怪，但是如果你能击败它的话，会获得很棒的奖励！\n\n（他看向你）\n\n加油！我相信你的实力！";
    s2_hint_task4.options = {
        DialogueOption{"谢谢你的鼓励！", "exit", nullptr, 0, ""},
        DialogueOption{"我会努力的！", "exit", nullptr, 0, ""}
    };
    npc.addDialogue("s2_hint_task4", s2_hint_task4);
    
    // 训练装置工作状态对话
    DialogueNode s2_machine_working;
    s2_machine_working.npc_text = "（陆天宇的眼中闪烁着自豪的光芒）\n\n现在这台训练装置可以正常工作了！\n\n（他指向装置）\n\n它可以模拟各种困难的情况，帮助人们锻炼意志力。很多同学都来这里训练，效果很好！\n\n（他的声音变得温柔）\n\n如果你以后需要锻炼毅力，随时可以来这里找我！";
    s2_machine_working.options = {
        DialogueOption{"谢谢！", "exit", nullptr, 0, ""},
        DialogueOption{"我会记住的！", "exit", nullptr, 0, ""}
    };
    npc.addDialogue("s2_machine_working", s2_machine_working);
    
    // 继续收集对话
    DialogueNode s2_continue;
    s2_continue.npc_text = "（陆天宇的眼中闪过一丝失望，但很快又变得鼓励）\n\n没关系，慢慢来。\n\n（他的声音变得温柔）\n\n我知道动力碎片很难获得，但是...但是这也是对你毅力的考验。\n\n（他看向你）\n\n记住，只有真正有毅力的人，才能获得动力碎片。不要放弃，继续努力！\n\n（他的声音变得坚定）\n\n我相信你一定能做到的！";
    s2_continue.options = {
        DialogueOption{"谢谢你的鼓励，我会继续努力的！", "exit", nullptr, 0, ""},
        DialogueOption{"有什么技巧可以提高获得概率吗？", "s2_tips", nullptr, 0, ""}
    };
    npc.addDialogue("s2_continue", s2_continue);
    
    // 收集技巧对话
    DialogueNode s2_tips;
    s2_tips.npc_text = "（陆天宇想了想）\n\n技巧...其实没有什么特别的技巧。\n\n（他的声音变得严肃）\n\n但是...但是我知道一些经验。\n\n• 保持耐心：动力碎片的掉落率确实不高，但是多打几次总会有的\n• 保持清醒：拖延小妖会试图让你感到懒惰，但是要保持清醒的头脑\n• 保持毅力：这是最重要的，只有真正有毅力的人才能获得动力碎片\n\n（他看向你）\n\n记住，这不是在玩游戏，这是对你毅力的真正考验！";
    s2_tips.options = {
        DialogueOption{"我明白了，谢谢你的建议。", "exit", nullptr, 0, ""},
        DialogueOption{"我会记住这些的。", "exit", nullptr, 0, ""}
    };
    npc.addDialogue("s2_tips", s2_tips);
    
    // 完成S2后的偶发提示
    DialogueNode s2_chat;
    s2_chat.npc_text = "没拿到学士袍也别怕，先把'被子'披上，扛得住就有输出。听说苏小萌在食堂遇到了选择困难，你可以去帮帮她。";
    s2_chat.options = { 
        DialogueOption{"受教了。", "exit", nullptr, 0, ""},
        DialogueOption{"苏小萌怎么了？", "su_xiaomeng_info", nullptr, 0, ""}
    };
    npc.addDialogue("chat", s2_chat);
    
    // 苏小萌信息
    DialogueNode su_xiaomeng_info;
    su_xiaomeng_info.npc_text = "苏小萌在食堂纠结吃什么，如果你帮她做选择，她会给你一个很有用的护符。";
    su_xiaomeng_info.options = { DialogueOption{"谢谢提醒。", "exit", nullptr, 0, ""} };
    npc.addDialogue("su_xiaomeng_info", su_xiaomeng_info);
}

void Game::initializeSuXiaomengDialogues(NPC& npc) {
    // 苏小萌 - 食堂支线 S1 完整对话系统
    
    // 主对话 - welcome
    DialogueNode s1_start;
    s1_start.id = "welcome";
    s1_start.npc_text = "（苏小萌站在食堂窗口前，双手抱头，一脸纠结）\n\n呜啊...今天到底吃什么好呢？\n\n（她看着菜单，眼中满是迷茫）\n\n麻辣烫？香辣可口，但是...会不会太辣了？\n牛肉面？清淡营养，但是...会不会太单调了？\n大盘鸡？分量十足，但是...会不会太油腻了？\n\n（她转向你，眼中带着求助的光芒）\n\n我...我每次都是这样，总是不知道该怎么选择。你能帮帮我吗？";
    s1_start.options = {
        DialogueOption{"当然可以！我来帮你选择。", "s1_choose", [this](){
            if (!state_.task_manager.hasActiveTask("side_canteen_choice") && !state_.task_manager.hasCompletedTask("side_canteen_choice")) {
                state_.task_manager.startTask("side_canteen_choice");
                std::cout<<"【任务接取】食堂选择。使用 task 查看详情。\n";
            }
        }, 0, ""},
        DialogueOption{"选择困难确实很麻烦，让我想想...", "s1_advice", nullptr, 0, ""},
        DialogueOption{"抱歉，我也有选择困难症。", "exit", nullptr, 0, ""}
    };
    npc.addDialogue("welcome", s1_start);
    npc.setDefaultDialogue("welcome");
    
    // 添加选择建议对话
    DialogueNode s1_advice;
    s1_advice.id = "s1_advice";
    s1_advice.npc_text = "（苏小萌的眼中闪过一丝希望）\n\n真的吗？你也有选择困难症？\n\n（她靠近你，声音变得小声）\n\n其实...我每次都是这样。明明知道这些都是好选择，但就是不知道该怎么选。\n\n（她的声音变得有些沮丧）\n\n有时候我觉得，选择比考试还难。考试至少有个标准答案，但选择...每个选择都好像是对的，又好像都不对。\n\n（她看向你）\n\n你能...你能帮我分析一下吗？";
    s1_advice.options = {
        DialogueOption{"当然可以！让我帮你分析一下。", "s1_choose", [this](){
            if (!state_.task_manager.hasActiveTask("side_canteen_choice") && !state_.task_manager.hasCompletedTask("side_canteen_choice")) {
                state_.task_manager.startTask("side_canteen_choice");
                std::cout<<"【任务接取】食堂选择。使用 task 查看。\n";
            }
        }, 0, ""},
        DialogueOption{"我觉得你可以试试...", "s1_choose", [this](){
            if (!state_.task_manager.hasActiveTask("side_canteen_choice") && !state_.task_manager.hasCompletedTask("side_canteen_choice")) {
                state_.task_manager.startTask("side_canteen_choice");
                std::cout<<"【任务接取】食堂选择。使用 task 查看。\n";
            }
        }, 0, ""},
        DialogueOption{"抱歉，我可能帮不上忙。", "exit", nullptr, 0, ""}
    };
    npc.addDialogue("s1_advice", s1_advice);
    
    DialogueNode s1_choose;
    s1_choose.id = "s1_choose";
    s1_choose.npc_text = "（苏小萌的眼中闪烁着期待的光芒）\n\n真的吗？太好了！\n\n（她重新看向菜单，声音变得兴奋）\n\n那你觉得哪个更适合我今天的状态呢？\n\n（她指向不同的选项）\n\n麻辣烫：香辣可口，能让人热血沸腾\n牛肉面：清淡营养，能让人内心平静\n大盘鸡：分量十足，能让人充满活力\n\n（她转向你，眼中带着信任）\n\n我相信你的判断！";
    s1_choose.options = {
        DialogueOption{"选麻辣烫吧！香辣可口，能让人热血沸腾。", "s1_after_pick", [this](){ 
            state_.player.attr().atk += 2; 
            std::cout<<"你感到热血上涌，ATK+2。\n"; 
            if(auto* tk=state_.task_manager.getTask("side_canteen_choice")) {
                tk->setObjective(0, "完成选择 ✓");
                // 标记已经选择过食物，防止重复选择
                state_.player.setNPCFavor("苏小萌", 1); // 使用好感度标记已选择
            }
        }, 0, ""},
        DialogueOption{"选牛肉面吧！清淡营养，能让人内心平静。", "s1_after_pick", [this](){ 
            state_.player.attr().def_ += 3; 
            std::cout<<"一碗下肚，底气更足，DEF+3。\n"; 
            if(auto* tk=state_.task_manager.getTask("side_canteen_choice")) {
                tk->setObjective(0, "完成选择 ✓");
                // 标记已经选择过食物，防止重复选择
                state_.player.setNPCFavor("苏小萌", 1); // 使用好感度标记已选择
            }
        }, 0, ""},
        DialogueOption{"选大盘鸡吧！分量十足，能让人充满活力。", "s1_after_pick", [this](){ 
            state_.player.attr().spd += 3; 
            std::cout<<"辣香刺激，脚步更轻，SPD+3。\n"; 
            if(auto* tk=state_.task_manager.getTask("side_canteen_choice")) {
                tk->setObjective(0, "完成选择 ✓");
                // 标记已经选择过食物，防止重复选择
                state_.player.setNPCFavor("苏小萌", 1); // 使用好感度标记已选择
            }
        }, 0, ""}
    };
    npc.addDialogue("s1_choose", s1_choose);
    
    DialogueNode s1_after_pick;
    s1_after_pick.id = "s1_after_pick";
    s1_after_pick.npc_text = "（苏小萌的脸上露出了灿烂的笑容）\n\n太好了！谢谢你帮我做出选择！\n\n（她开心地跳了一下）\n\n你知道吗？这是我第一次这么快就做出决定！以前我总是在这里站很久，最后还是随便选一个。\n\n（她的表情变得有些不好意思）\n\n不过...我现在还缺一瓶咖啡因灵液。你知道的，最近学习压力很大，我需要一些提神的东西。\n\n（她看向你，眼中带着期待）\n\n你能给我一瓶吗？我会给你一些好东西作为回报的！";
    s1_after_pick.options = {
        DialogueOption{"当然可以！给你咖啡因灵液。", "s1_give", [this](){
            if (state_.player.inventory().remove("caffeine_elixir",1)) {
                // 奖励：生命药水×1 与 钢勺护符
                Item hp = Item::createConsumable("health_potion","生命药水","恢复生命值的药水。",30,30);
                state_.player.inventory().add(hp,1);
                Item spoon = ItemDefinitions::createSteelSpoon();
                spoon.price = 0; // 奖励物品免费
                spoon.effect_type = "damage_multiplier"; spoon.effect_target = "失败实验体"; spoon.effect_value = 1.3f;
                state_.player.inventory().add(spoon,1);
                state_.player.addNPCFavor("林清漪",20);
                std::cout<<"【S1完成】你交付了咖啡因灵液，获得生命药水×1、钢勺护符×1。林清漪好感+20。\n";
                if(auto* tk=state_.task_manager.getTask("side_canteen_choice")) { tk->setObjective(1, "赠送咖啡因灵液 ✓"); tk->complete(); }
            } else {
                std::cout<<"你没有咖啡因灵液。\n";
                // 设置标志，表示没有物品，需要跳转到不同的对话
                state_.dialogue_memory["苏小萌"].insert("no_caffeine_elixir");
            }
        }, 0, ""},
        DialogueOption{"抱歉，我现在没有咖啡因灵液。", "s1_no_elixir", nullptr, 0, ""},
        DialogueOption{"咖啡因灵液是什么？", "s1_elixir_info", nullptr, 0, ""}
    };
    npc.addDialogue("s1_after_pick", s1_after_pick);
    
    // 添加给予咖啡因灵液后的对话
    DialogueNode s1_give;
    s1_give.id = "s1_give";
    s1_give.npc_text = "（苏小萌的眼中闪烁着感激的光芒）\n\n太谢谢你了！你真是太好了！\n\n（她开心地接过咖啡因灵液）\n\n这个护符是我从家里带来的，据说能对实验失败妖造成额外伤害！\n\n（她将护符递给你）\n\n还有这瓶生命药水，也送给你！\n\n（她的表情变得认真）\n\n谢谢你帮我解决了选择困难的问题，我现在感觉好多了！";
    s1_give.options = {
        DialogueOption{"不客气，能帮到你就好！", "exit", nullptr, 0, ""},
        DialogueOption{"这个护符看起来很特别！", "exit", nullptr, 0, ""}
    };
    npc.addDialogue("s1_give", s1_give);
    
    // 添加没有咖啡因灵液时的对话
    DialogueNode s1_no_elixir_response;
    s1_no_elixir_response.npc_text = "（苏小萌的表情有些失望，但很快又露出了理解的笑容）\n\n哦...没关系，我理解的！\n\n（她轻轻摇了摇头）\n\n其实我也知道，咖啡因灵液确实不容易得到。\n\n（她的表情变得有些不好意思）\n\n不过如果你以后有机会的话，记得来找我哦！我真的很需要一些提神的东西。\n\n（她看向你，眼中带着期待）\n\n到时候我一定会给你很好的回报的！";
    s1_no_elixir_response.options = {
        DialogueOption{"好的，我会记住的！", "exit", nullptr, 0, ""},
        DialogueOption{"抱歉让你失望了。", "exit", nullptr, 0, ""}
    };
    npc.addDialogue("s1_no_elixir_response", s1_no_elixir_response);
    
    // 任务完成后的对话
    DialogueNode s1_completed;
    s1_completed.npc_text = "（苏小萌看到你，脸上露出了灿烂的笑容）\n\n嗨！谢谢你之前帮我选择食物！\n\n（她开心地跳了一下）\n\n你知道吗？自从你帮我做出选择后，我现在已经能够自己快速做决定了！\n\n（她的表情变得有些自豪）\n\n而且那个钢勺护符真的很好用，我现在吃饭都更有底气了！\n\n（她的表情变得认真）\n\n对了，我听说教学楼五区有个高数难题精，很多同学都在那里遇到了困难。如果你想要挑战智力试炼的话，可以去那里看看。\n\n（她看向你）\n\n不过要小心，那个怪物很厉害的！";
    s1_completed.options = {
        DialogueOption{"谢谢你的提醒！我去看看。", "s1_hint_task3", [this](){
            // 自动接取支线任务3：智力试炼
            if (!state_.task_manager.hasActiveTask("side_teach_wisdom") && !state_.task_manager.hasCompletedTask("side_teach_wisdom")) {
                state_.task_manager.startTask("side_teach_wisdom");
                std::cout<<"【任务接取】智力试炼。使用 task 查看详情。\n";
            }
        }, 0, ""},
        DialogueOption{"你现在还会选择困难吗？", "s1_choice_advice", nullptr, 0, ""},
        DialogueOption{"再见！", "exit", nullptr, 0, ""}
    };
    npc.addDialogue("s1_completed", s1_completed);
    
    // 选择建议对话
    DialogueNode s1_choice_advice;
    s1_choice_advice.npc_text = "（苏小萌的眼中闪过一丝思考）\n\n其实...还是会有一点，但是比以前好多了！\n\n（她的声音变得认真）\n\n我现在学会了几个小技巧：\n• 先列出所有选项的优点和缺点\n• 设定一个时间限制，比如5分钟内必须决定\n• 如果实在选不出来，就选第一个想到的\n\n（她看向你）\n\n最重要的是，不要害怕做错选择。每个选择都有它的价值，关键是要勇敢地迈出那一步！";
    s1_choice_advice.options = {
        DialogueOption{"很有用的建议！", "exit", nullptr, 0, ""},
        DialogueOption{"谢谢你的分享！", "exit", nullptr, 0, ""}
    };
    npc.addDialogue("s1_choice_advice", s1_choice_advice);
    
    // 支线任务3提示对话
    DialogueNode s1_hint_task3;
    s1_hint_task3.npc_text = "（苏小萌的眼中闪烁着鼓励的光芒）\n\n太好了！我相信你一定能够战胜那个高数难题精的！\n\n（她的声音变得神秘）\n\n听说那个怪物会出一些很难的数学题，但是如果你能答对的话，会获得很棒的奖励！\n\n（她看向你）\n\n加油！我相信你的智慧！";
    s1_hint_task3.options = {
        DialogueOption{"谢谢你的鼓励！", "exit", nullptr, 0, ""},
        DialogueOption{"我会努力的！", "exit", nullptr, 0, ""}
    };
    npc.addDialogue("s1_hint_task3", s1_hint_task3);
    
    // 添加没有咖啡因灵液的对话
    DialogueNode s1_no_elixir;
    s1_no_elixir.id = "s1_no_elixir";
    s1_no_elixir.npc_text = "（苏小萌的眼中闪过一丝失望）\n\n啊...没关系，我理解。\n\n（她的声音变得有些沮丧）\n\n其实...其实我也可以去别的地方找找。只是...只是我觉得你人很好，想和你做朋友。\n\n（她看向你）\n\n如果你以后有咖啡因灵液的话，记得来找我哦！我会给你一些好东西的！";
    s1_no_elixir.options = {
        DialogueOption{"好的，我会记住的。", "exit", nullptr, 0, ""},
        DialogueOption{"你能告诉我咖啡因灵液在哪里可以找到吗？", "s1_elixir_info", nullptr, 0, ""}
    };
    npc.addDialogue("s1_no_elixir", s1_no_elixir);
    
    // 添加咖啡因灵液信息对话
    DialogueNode s1_elixir_info;
    s1_elixir_info.id = "s1_elixir_info";
    s1_elixir_info.npc_text = "（苏小萌的眼中闪过一丝希望）\n\n咖啡因灵液？\n\n（她想了想）\n\n我记得...我记得在荒废北操场有一些怪物会掉落咖啡因灵液。就是那些压力黑雾，它们身上有时候会带着这种灵液。\n\n（她的声音变得兴奋）\n\n如果你能帮我找到一瓶，我会给你一个很特别的护符！那是我从家里带来的，据说能对实验失败妖造成额外伤害！\n\n（她看向你）\n\n怎么样？要不要去试试？";
    s1_elixir_info.options = {
        DialogueOption{"好的，我去试试看。", "exit", nullptr, 0, ""},
        DialogueOption{"压力黑雾在哪里？", "s1_monster_info", nullptr, 0, ""}
    };
    npc.addDialogue("s1_elixir_info", s1_elixir_info);
    
    // 添加怪物信息对话
    DialogueNode s1_monster_info;
    s1_monster_info.id = "s1_monster_info";
    s1_monster_info.npc_text = "（苏小萌指向北边）\n\n压力黑雾在荒废北操场，就是那个废弃的操场。\n\n（她的声音变得有些害怕）\n\n那些怪物...它们很可怕，会让人感到紧张和压力。但是如果你有足够的实力，它们会掉落咖啡因灵液。\n\n（她看向你）\n\n不过要小心，它们比一般的怪物要强一些。建议你先提升一下等级，或者找一些好装备再去挑战。\n\n（她的声音变得温柔）\n\n如果你需要帮助，可以去找林清漪，她总是很乐意帮助新人的。";
    s1_monster_info.options = {
        DialogueOption{"我明白了，谢谢你的提醒。", "exit", nullptr, 0, ""},
        DialogueOption{"我会小心的。", "exit", nullptr, 0, ""}
    };
    npc.addDialogue("s1_monster_info", s1_monster_info);
    
    // 随机闲聊建议（完成S1后偶发）
    DialogueNode s1_chat;
    s1_chat.id = "s1_chat";
    s1_chat.npc_text = "（小声）实验楼别硬刚，先穿厚点的护甲更稳。听说陆天宇在体育馆修训练装置，你可以去帮帮他。";
    s1_chat.options = { 
        DialogueOption{"嗯，记住了。", "exit", nullptr, 0, ""},
        DialogueOption{"陆天宇是谁？", "lu_tianyu_info", nullptr, 0, ""}
    };
    npc.addDialogue("chat", s1_chat);
    
    // 陆天宇信息
    DialogueNode lu_tianyu_info;
    lu_tianyu_info.id = "lu_tianyu_info";
    lu_tianyu_info.npc_text = "陆天宇是体育馆的管理员，他正在修理训练装置，需要动力碎片。如果你帮他收集，他会给你很好的奖励。";
    lu_tianyu_info.options = { DialogueOption{"谢谢提醒。", "exit", nullptr, 0, ""} };
    npc.addDialogue("lu_tianyu_info", lu_tianyu_info);
}

} // namespace hx
