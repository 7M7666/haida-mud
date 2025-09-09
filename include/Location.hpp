// 这是地点系统的头文件
// 作者：大一学生
// 功能：定义游戏中的地点系统，包括地点信息和导航

#pragma once
#include <string>     // 字符串
#include <vector>     // 向量容器
#include "Enemy.hpp"  // 敌人类
#include "Item.hpp"   // 物品类
#include "NPC.hpp"    // NPC类

namespace hx {
// 出口结构体
// 功能：定义地点之间的连接
struct Exit { 
    std::string label;  // 出口标签
    std::string to;     // 目标地点ID
};

// 坐标结构体
// 功能：定义地点的坐标位置
struct Coord { 
    int x;  // X坐标
    int y;  // Y坐标
};

// 地点类
// 功能：表示游戏中的一个地点，包含所有相关信息
class Location {
public:
    std::string id;
    std::string name;
    std::string desc;
    Coord coord{0,0};
    std::vector<Exit> exits;
    std::vector<Enemy> enemies;
    std::vector<Item> shop;
    std::vector<NPC> npcs;
    const Exit* findExitByLabel(const std::string& label) const;
    const NPC* findNPC(const std::string& name) const;
};
} // namespace hx
