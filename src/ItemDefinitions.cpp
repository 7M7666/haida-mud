// 这是物品定义的实现文件
// 作者：大一学生
// 功能：定义游戏中的所有物品，包括装备、消耗品、任务物品等

#include "ItemDefinitions.hpp"  // 物品定义头文件

namespace hx {

// 基础装备 - 游戏中的基础装备物品
Item ItemDefinitions::createStudentUniform() {
    Item item = Item::createEquipment("student_uniform", "普通学子服", 
        "海大计算机学院的标准校服，虽然普通但很实用。", 
        EquipmentType::ARMOR, EquipmentSlot::ARMOR, 0, 2, 0, 10, 50);
    item.description = "海大计算机学院的标准校服，虽然普通但很实用。穿上它，你感觉自己更像一个真正的学生了。";
    item.quality = EquipmentQuality::UNDERGRAD;
    return item;
}

Item ItemDefinitions::createBambooNotes() {
    Item item = Item::createEquipment("bamboo_notes", "竹简笔记", 
        "古老的竹简，记载着学习心得。", 
        EquipmentType::WEAPON, EquipmentSlot::WEAPON, 5, 0, 2, 0, 30);
    item.description = "古老的竹简，记载着学习心得。虽然古老，但知识永远不会过时。";
    item.quality = EquipmentQuality::UNDERGRAD;
    return item;
}

// 试炼奖励装备
Item ItemDefinitions::createWisdomPen() {
    Item item = Item::createEquipment("wisdom_pen", "启智笔", 
        "智力试炼的奖励，能提升思维敏捷度。", 
        EquipmentType::WEAPON, EquipmentSlot::WEAPON, 6, 0, 0, 0, 120);
    item.description = "智力试炼的奖励，能提升思维敏捷度。笔尖闪烁着智慧的光芒。";
    item.quality = EquipmentQuality::MASTER;
    item.effect_type = "damage_multiplier";
    item.effect_target = "难题类";
    item.effect_value = 1.15f;
    item.effect_description = "对难题类敌人额外造成15%伤害（乘法计算）";
    return item;
}

Item ItemDefinitions::createSteelSpoon() {
    Item item = Item::createEquipment("steel_spoon", "钢勺护符", 
        "食堂选择的奖励，能提供额外保护。", 
        EquipmentType::ACCESSORY, EquipmentSlot::ACCESSORY1, 1, 0, 0, 0, 80);
    item.description = "食堂选择的奖励，能提供额外保护。虽然是勺子，但经过特殊处理变得很坚固。";
    item.effect_description = "对实验失败妖最终伤害×1.3";
    item.effect_type = "damage_multiplier";
    item.effect_target = "失败实验体";
    item.effect_value = 1.3f;
    return item;
}

Item ItemDefinitions::createWeightBracelet() {
    Item item = Item::createEquipment("weight_bracelet", "负重护腕", 
        "毅力试炼的奖励，能增强体魄。", 
        EquipmentType::ARMOR, EquipmentSlot::ARMOR, 3, 9, 0, 30, 90);
    item.description = "毅力试炼的奖励，能增强体魄。戴上它，你感觉自己的力量增加了。";
    item.quality = EquipmentQuality::DOCTOR;
    return item;
}

Item ItemDefinitions::createLabNotebook() {
    Item item = Item::createEquipment("lab_notebook", "实验记录本", 
        "抗挫试炼的奖励，记录着实验心得。", 
        EquipmentType::ACCESSORY, EquipmentSlot::ACCESSORY1, 5, 5, 10, 0, 70);
    item.description = "抗挫试炼的奖励，记录着实验心得。每一页都写满了失败和成功的经验。";
    return item;
}

Item ItemDefinitions::createSafetyGoggles() {
    Item item = Item::createEquipment("safety_goggles", "护目镜", 
        "实验楼的隐藏奖励，能保护眼睛。", 
        EquipmentType::ACCESSORY, EquipmentSlot::ACCESSORY2, 0, 0, 6, 0, 160);
    item.description = "实验楼的隐藏奖励，能保护眼睛。戴上它，你感觉视野更清晰了。";
    item.effect_type = "extra_evasion"; 
    item.effect_value = 0.05f; 
    item.effect_description = "闪避率+5%";
    return item;
}

Item ItemDefinitions::createSpeechWords() {
    Item item = Item::createEquipment("speech_words", "演讲之词", 
        "表达试炼的奖励，能提升表达能力。", 
        EquipmentType::ACCESSORY, EquipmentSlot::ACCESSORY1, 1, 0, 0, 0, 150);
    item.description = "表达试炼的奖励，能提升表达能力。每一个字都蕴含着说服力。";
    item.effect_description = "战斗开始获得鼓舞(3回合)";
    item.effect_type = "auto_inspiration";
    item.effect_target = "all";
    item.effect_value = 1.0f;
    return item;
}

Item ItemDefinitions::createDebateFan() {
    Item item = Item::createEquipment("debate_fan", "辩锋羽扇", 
        "树下空间的奖励，能增强辩论技巧。", 
        EquipmentType::WEAPON, EquipmentSlot::WEAPON, 15, 0, 6, 0, 350);
    item.description = "树下空间的奖励，能增强辩论技巧。扇动时仿佛能听到辩论的声音。";
    item.quality = EquipmentQuality::DOCTOR;
    item.effect_type = "damage_multiplier";
    item.effect_target = "答辩紧张魔·强化";
    item.effect_value = 1.3f;
    item.effect_description = "对答辩紧张魔·强化造成1.3倍伤害";
    return item;
}

// 教学区装备池
Item ItemDefinitions::createWuJingBall() {
    Item item = Item::createEquipment("wu_jing_ball", "吴京篮球", "形意合一，攻守兼备。", 
        EquipmentType::WEAPON, EquipmentSlot::WEAPON, 4, 0, 2, 0, 110);
    item.quality = EquipmentQuality::UNDERGRAD;
    return item;
}

Item ItemDefinitions::createSecondHandGuitar() {
    Item item = Item::createEquipment("second_hand_guitar", "二手吉他", "一曲肝肠断，输出即情绪。", 
        EquipmentType::WEAPON, EquipmentSlot::WEAPON, 7, 0, 0, 0, 180);
    item.effect_type = "on_attack_inspiration";
    item.effect_value = 0.3f;
    item.effect_description = "攻击时30%概率触发鼓舞效果（自身ATK +15%，持续2回合）";
    item.quality = EquipmentQuality::MASTER;
    return item;
}

Item ItemDefinitions::createPhdThesis() {
    Item item = Item::createEquipment("phd_thesis", "博士大论文", "厚重如砖，砸谁谁懵。", 
        EquipmentType::WEAPON, EquipmentSlot::WEAPON, 15, 0, 2, 0, 320);
    item.quality = EquipmentQuality::DOCTOR;
    return item;
}

Item ItemDefinitions::createBachelorRobe() {
    Item item = Item::createEquipment("bachelor_robe", "学士袍", "雍容华贵，学养自显。", 
        EquipmentType::ARMOR, EquipmentSlot::ARMOR, 0, 0, 0, 0, 220);
    item.effect_type = "all_stats_multiplier"; 
    item.effect_value = 1.1f; 
    item.effect_description = "全属性 +10%";
    item.quality = EquipmentQuality::MASTER;
    return item;
}

Item ItemDefinitions::createBedQuilt() {
    Item item = Item::createEquipment("bed_quilt", "床上的被子", "最是温柔被窝中。", 
        EquipmentType::ARMOR, EquipmentSlot::ARMOR, 0, 7, 0, 0, 140);
    item.effect_type = "per_turn_heal_percent"; 
    item.effect_value = 0.03f; 
    item.effect_description = "每回合恢复3%最大HP";
    item.quality = EquipmentQuality::UNDERGRAD;
    return item;
}

Item ItemDefinitions::createLabCoat() {
    Item item = Item::createEquipment("lab_coat", "实验服", "严谨至上，安全第一。", 
        EquipmentType::ARMOR, EquipmentSlot::ARMOR, 0, 10, 0, 35, 260);
    item.quality = EquipmentQuality::DOCTOR;
    return item;
}

Item ItemDefinitions::createPsionicArmor() {
    Item item = Item::createEquipment("psionic_armor", "灵能护甲", "灵能护体，固若金汤。", 
        EquipmentType::ARMOR, EquipmentSlot::ARMOR, 0, 6, 0, 20, 230);
    item.effect_type = "periodic_shield"; 
    item.effect_value = 0.3f; 
    item.effect_description = "每3回合自动套1次30%减伤盾";
    item.quality = EquipmentQuality::MASTER;
    return item;
}

Item ItemDefinitions::createSteelSpoonAmulet() {
    Item item = Item::createEquipment("steel_spoon_amulet", "钢勺护符", "食堂选择的奖励，能提供额外保护。", 
        EquipmentType::ACCESSORY, EquipmentSlot::ACCESSORY1, 1, 0, 0, 0, 80);
    item.effect_type = "damage_multiplier";
    item.effect_target = "实验失败妖";
    item.effect_value = 1.3f;
    item.effect_description = "对实验失败妖系敌人最终伤害×1.3";
    return item;
}

Item ItemDefinitions::createSpeechWordsAmulet() {
    Item item = Item::createEquipment("speech_words_amulet", "演讲之词", "表达试炼的奖励，能提升表达能力。", 
        EquipmentType::ACCESSORY, EquipmentSlot::ACCESSORY1, 1, 0, 0, 0, 150);
    item.effect_type = "auto_inspiration";
    item.effect_target = "all";
    item.effect_value = 1.0f;
    item.effect_description = "战斗开始自动获得鼓舞效果（ATK +15%，持续3回合）";
    return item;
}

Item ItemDefinitions::createGogglesAmulet() {
    Item item = Item::createEquipment("goggles_amulet", "护目镜", "实验楼的隐藏奖励，能保护眼睛。", 
        EquipmentType::ACCESSORY, EquipmentSlot::ACCESSORY2, 0, 0, 6, 0, 160);
    item.effect_type = "extra_evasion"; 
    item.effect_value = 0.05f; 
    item.effect_description = "SPD +6，闪避率额外 +5%";
    return item;
}

Item ItemDefinitions::createHnuBadgeAmulet() {
    Item item = Item::createEquipment("hnu_badge_amulet", "海纳百川校徽", "海纳百川，有容乃大。", 
        EquipmentType::ACCESSORY, EquipmentSlot::ACCESSORY1, 3, 3, 3, 3, 200);
    item.effect_type = "auto_focus"; 
    item.effect_value = 1.0f; 
    item.effect_description = "全属性 +3，战斗开始获得专注效果（持续1回合）";
    return item;
}

Item ItemDefinitions::createEcardAmulet() {
    Item item = Item::createEquipment("ecard_amulet", "海大e卡通", "一卡在手，畅行校园。", 
        EquipmentType::ACCESSORY, EquipmentSlot::ACCESSORY2, 0, 0, 0, 0, 600);
    item.effect_type = "shop_discount"; 
    item.effect_value = 0.9f; 
    item.effect_description = "商店购物享受10%折扣";
    item.quality = EquipmentQuality::DOCTOR;
    return item;
}

Item ItemDefinitions::createSeatAllLibAmulet() {
    Item item = Item::createEquipment("seat_all_lib_amulet", "全图书馆占座物品", "今日座位，舍我其谁。", 
        EquipmentType::ACCESSORY, EquipmentSlot::ACCESSORY2, 0, 0, 5, 0, 600);
    item.effect_type = "first_turn_priority"; 
    item.effect_value = 1.0f; 
    item.effect_description = "SPD +5，首回合必定先攻";
    item.quality = EquipmentQuality::DOCTOR;
    return item;
}

// 消耗品
Item ItemDefinitions::createHealthPotion() {
    return Item::createConsumable("health_potion", "生命药水", "恢复30点生命值", 30, 30);
}

Item ItemDefinitions::createRevivalScroll() {
    Item item = Item::createConsumable("revival_scroll", "复活符", "死亡时无惩罚复活", 0, 300);
    item.is_quest_item = true;
    return item;
}

Item ItemDefinitions::createCaffeineElixir() {
    return Item::createConsumable("caffeine_elixir", "咖啡因灵液", "获得专注：下一次攻击必中", 150, 150);
}

// 根据ID创建物品（统一入口）
Item ItemDefinitions::createItemById(const std::string& item_id) {
    if (item_id == "student_uniform") return createStudentUniform();
    if (item_id == "bamboo_notes") return createBambooNotes();
    if (item_id == "wisdom_pen") return createWisdomPen();
    if (item_id == "steel_spoon") return createSteelSpoon();
    if (item_id == "weight_bracelet") return createWeightBracelet();
    if (item_id == "lab_notebook") return createLabNotebook();
    if (item_id == "safety_goggles") return createSafetyGoggles();
    if (item_id == "speech_words") return createSpeechWords();
    if (item_id == "debate_fan") return createDebateFan();
    if (item_id == "wu_jing_ball") return createWuJingBall();
    if (item_id == "second_hand_guitar") return createSecondHandGuitar();
    if (item_id == "phd_thesis") return createPhdThesis();
    if (item_id == "bachelor_robe") return createBachelorRobe();
    if (item_id == "bed_quilt") return createBedQuilt();
    if (item_id == "lab_coat") return createLabCoat();
    if (item_id == "psionic_armor") return createPsionicArmor();
    if (item_id == "steel_spoon_amulet") return createSteelSpoonAmulet();
    if (item_id == "speech_words_amulet") return createSpeechWordsAmulet();
    if (item_id == "goggles_amulet") return createGogglesAmulet();
    if (item_id == "hnu_badge_amulet") return createHnuBadgeAmulet();
    if (item_id == "ecard_amulet") return createEcardAmulet();
    if (item_id == "seat_all_lib_amulet") return createSeatAllLibAmulet();
    if (item_id == "health_potion") return createHealthPotion();
    if (item_id == "revival_scroll") return createRevivalScroll();
    if (item_id == "caffeine_elixir") return createCaffeineElixir();
    
    // 默认返回普通学子服
    return createStudentUniform();
}

} // namespace hx
