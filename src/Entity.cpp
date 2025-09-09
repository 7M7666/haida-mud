// 实体类的实现文件
// 游戏中的基础实体类，所有角色和怪物的基类

#include "Entity.hpp"  // 实体类头文件

namespace hx {
// 创建实体的时候会调用这个函数
// 输入实体名称和属性
Entity::Entity(std::string name, Attributes attr) : name_(std::move(name)), attr_(attr) {}
} // namespace hx
