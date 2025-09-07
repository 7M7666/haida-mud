#include "Player.hpp"
#include <iostream>
#include <string>

namespace hx {

Player::Ending Player::calculateEnding() const {
    // 检查文心潭失败次数
    if (wenxin_failures_ >= 3) {
        return Ending::FAILURE;
    }
    
    // 检查等级和好感度
    int lin_qingyi_favor = getNPCFavor("林清漪");
    
    // 检查是否拥有特定装备（使用正确的物品ID）
    bool has_wisdom_pen = inventory_->quantity("wisdom_pen") > 0;
    bool has_safety_goggles = inventory_->quantity("goggles") > 0;
    bool has_debate_fan = inventory_->quantity("debate_fan") > 0;
    int special_equipment_count = (has_wisdom_pen ? 1 : 0) + (has_safety_goggles ? 1 : 0) + (has_debate_fan ? 1 : 0);
    
    // 结局判断逻辑（统一等级要求为12级）
    if (level_ >= 12) {
        return Ending::ACADEMIC_SUCCESS;  // 学业有成
    } else if (lin_qingyi_favor >= 60) {
        return Ending::GUARDIAN;  // 秘境守护者
    } else if (lin_qingyi_favor >= 50 && special_equipment_count >= 2) {
        return Ending::BALANCED_WALKER;  // 平衡行者
    } else if (level_ < 12 || lin_qingyi_favor < 40) {
        return Ending::NORMAL_RETURN;  // 普通回归
    } else {
        return Ending::NORMAL_RETURN;  // 默认普通回归
    }
}

std::string Player::getEndingDescription(Ending ending) const {
    switch (ending) {
        case Ending::ACADEMIC_SUCCESS:
            return R"(
=== 结局：学业有成 ===

经过不懈的努力，你终于达到了等级10，成为了海大计算机学院的优秀学生。

在秘境中的历练让你明白了学习的真谛：不仅仅是知识的积累，更是意志的磨练和心灵的成长。
每一个试炼都让你变得更加坚强，每一次战斗都让你更加成熟。

林清漪看着你的成长，欣慰地笑了："你终于明白了，学习不是目的，而是通往更好自己的道路。"

你带着满满的收获离开了秘境，回到了现实世界。从此以后，你不再是那个无所事事的学生，
而是一个有目标、有毅力、有智慧的年轻人。

【结局：学业有成】
- 等级达到12级
- 在秘境中获得了全面的成长
- 成为了优秀的学生代表
            )";
            
        case Ending::GUARDIAN:
            return R"(
=== 结局：秘境守护者 ===

你与林清漪建立了深厚的友谊，她对你的好感度达到了60以上。
在长期的交流中，你逐渐了解了秘境的秘密，也理解了林清漪选择留下的原因。

"其实，我也曾是这本古籍的读者。"林清漪轻声说道，"但我选择了留下，成为秘境的守护者。
因为我相信，每一个迷茫的学生都需要一个引导者，帮助他们找到自己的道路。"

你被她的精神所感动，决定继承她的意志，成为新的秘境守护者。
从此以后，你将帮助更多像你一样迷茫的学生，引导他们走向正确的道路。

【结局：秘境守护者】
- 与林清漪建立了深厚友谊
- 理解了秘境的真正意义
- 选择成为新的守护者
            )";
            
        case Ending::BALANCED_WALKER:
            return R"(
=== 结局：平衡行者 ===

你虽然没有达到最高等级，但你在秘境中找到了属于自己的平衡。
你拥有多件珍贵的装备，与林清漪也建立了良好的关系。

你明白了，成长不是一蹴而就的，而是需要在各个方面都保持平衡。
智力、毅力、表达力，每一个方面都很重要，但更重要的是找到适合自己的节奏。

林清漪对你说："你找到了属于自己的道路，这比什么都重要。"
你带着这份感悟离开了秘境，在现实世界中继续寻找属于自己的平衡。

【结局：平衡行者】
- 与林清漪关系良好
- 拥有多件珍贵装备
- 找到了属于自己的平衡
            )";
            
        case Ending::NORMAL_RETURN:
            return R"(
=== 结局：普通回归 ===

你离开了秘境，回到了现实世界。虽然你没有达到很高的等级，
也没有与林清漪建立深厚的友谊，但这次经历仍然让你有所收获。

你明白了学习的重要性，也体验了不同的试炼。
虽然结果可能不是最理想的，但这次经历将成为你人生中宝贵的回忆。

也许有一天，你会再次翻开那本《文心潭秘录》，重新开始你的秘境之旅。

【结局：普通回归】
- 等级未达到12级
- 与林清漪关系一般
- 但获得了宝贵的经历
            )";
            
        case Ending::FAILURE:
            return R"(
=== 结局：失败 ===

在文心潭的最终试炼中，你连续失败了三次。
心魔的力量太过强大，你无法战胜内心的恐惧和焦虑。

林清漪看着你，眼中充满了遗憾："也许现在的你还没有准备好面对这一切。
但不要灰心，失败是成功之母。当你真正准备好时，可以再次尝试。"

你被传送出了秘境，回到了现实世界。
这次失败让你明白了自己的不足，也让你更加珍惜学习的机会。

也许有一天，当你变得更加坚强时，你会再次挑战文心潭的试炼。

【结局：失败】
- 在文心潭连续失败三次
- 无法战胜内心的心魔
- 需要更多的时间来成长
            )";
            
        default:
            return "未知结局。";
    }
}



} // namespace hx
