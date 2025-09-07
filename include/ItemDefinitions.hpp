#pragma once
#include "Item.hpp"

namespace hx {

// 统一的物品定义类，避免重复定义
class ItemDefinitions {
public:
    // 基础装备
    static Item createStudentUniform();
    static Item createBambooNotes();
    
    // 试炼奖励装备
    static Item createWisdomPen();
    static Item createSteelSpoon();
    static Item createWeightBracelet();
    static Item createLabNotebook();
    static Item createSafetyGoggles();
    static Item createSpeechWords();
    static Item createDebateFan();
    
    // 教学区装备池
    static Item createWuJingBall();
    static Item createSecondHandGuitar();
    static Item createPhdThesis();
    static Item createBachelorRobe();
    static Item createBedQuilt();
    static Item createLabCoat();
    static Item createPsionicArmor();
    static Item createSteelSpoonAmulet();
    static Item createSpeechWordsAmulet();
    static Item createGogglesAmulet();
    static Item createHnuBadgeAmulet();
    static Item createEcardAmulet();
    static Item createSeatAllLibAmulet();
    
    // 消耗品
    static Item createHealthPotion();
    static Item createRevivalScroll();
    static Item createCaffeineElixir();
    
    // 根据ID创建物品（统一入口）
    static Item createItemById(const std::string& item_id);
};

} // namespace hx
