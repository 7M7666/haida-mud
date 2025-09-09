// 这是实体类的头文件
// 作者：大一学生
// 功能：定义游戏中的基础实体类，所有角色和怪物的基类

#pragma once
#include <string>         // 字符串
#include "Attributes.hpp" // 属性系统

namespace hx {
// 实体基类
// 功能：所有游戏实体的基础类，包括玩家、NPC、敌人等
class Entity {
public:
    // 构造函数
    // 参数：name(实体名称), attr(属性)
    Entity(std::string name, Attributes attr);
    
    // 虚析构函数
    virtual ~Entity() = default;

    // 获取实体名称
    // 返回值：实体名称的常量引用
    const std::string& name() const { return name_; }
    
    // 获取属性（可修改版本）
    // 返回值：属性的引用
    Attributes& attr() { return attr_; }
    
    // 获取属性（只读版本）
    // 返回值：属性的常量引用
    const Attributes& attr() const { return attr_; }

protected:
    std::string name_;    // 实体名称
    Attributes attr_{};   // 实体属性
};
} // namespace hx
