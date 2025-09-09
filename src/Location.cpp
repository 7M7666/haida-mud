// 这是地点类的实现文件
// 作者：大一学生
// 功能：实现游戏中的地点系统，包括出口和NPC管理

#include "Location.hpp"  // 地点类头文件

namespace hx {
// 根据标签查找出口
// 参数：label(出口标签)
// 返回值：找到的出口指针，如果没找到返回nullptr
// 功能：在地点的出口列表中查找指定标签的出口
const Exit* Location::findExitByLabel(const std::string& label) const{ 
    // 遍历所有出口
    for(auto &e:exits) 
        if(e.label==label) return &e;  // 找到匹配的出口，返回指针
    return nullptr;                    // 没找到，返回空指针
} 

// 根据名称查找NPC
// 参数：name(NPC名称)
// 返回值：找到的NPC指针，如果没找到返回nullptr
// 功能：在地点的NPC列表中查找指定名称的NPC
const NPC* Location::findNPC(const std::string& name) const {
    // 遍历所有NPC
    for(const auto& npc : npcs) {
        if(npc.name() == name) return &npc;  // 找到匹配的NPC，返回指针
    }
    return nullptr;                          // 没找到，返回空指针
}
} // namespace hx
