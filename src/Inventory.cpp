// 背包系统的实现文件
// 玩家的背包系统，管理物品的存储和数量

#include "Inventory.hpp"  // 背包类头文件

namespace hx {
// 添加物品到背包
// 输入要添加的物品和数量
void Inventory::add(const Item& item,int qty){ 
    auto &entry = data_[item.id];           // 获取或创建物品条目
    if(entry.second==0) entry.first=item;   // 如果是新物品，设置物品信息
    entry.second+=qty;                      // 增加数量
    entry.first.count=entry.second;         // 更新物品的计数
}

// 从背包中移除物品
// 输入物品ID和要移除的数量
// 如果移除成功返回true，物品不足返回false
bool Inventory::remove(const std::string& id,int qty){ 
    auto it=data_.find(id);                 // 查找物品
    if(it==data_.end()) return false;       // 物品不存在，返回失败
    if(it->second.second<qty) return false; // 数量不足，返回失败
    it->second.second-=qty;                 // 减少数量
    it->second.first.count=it->second.second; // 更新物品计数
    if(it->second.second==0) data_.erase(it); // 如果数量为0，删除物品
    return true;                            // 返回成功
}

// 获取物品数量
// 参数：id(物品ID)
// 返回值：物品的数量，如果不存在返回0
// 功能：查询背包中指定物品的数量
int Inventory::quantity(const std::string& id) const{ 
    auto it=data_.find(id);                 // 查找物品
    return it==data_.end()?0:it->second.second; // 返回数量或0
}

// 获取背包中所有物品的列表
// 返回值：包含所有物品的向量
// 功能：返回背包中所有物品的副本
std::vector<Item> Inventory::list() const{ 
    std::vector<Item> out;                  // 创建输出向量
    for(auto &kv:data_) out.push_back(kv.second.first); // 遍历所有物品并添加到向量
    return out;                             // 返回物品列表
}

// 获取简单物品列表（与list()功能相同）
// 返回值：物品列表
// 功能：为了兼容性而保留的方法
std::vector<Item> Inventory::asSimpleItems() const{ 
    return list(); 
}

// 从简单物品列表设置背包内容
// 参数：items(物品列表)
// 功能：清空背包并用新的物品列表填充
void Inventory::setFromSimple(const std::vector<Item>& items){ 
    data_.clear();                          // 清空背包
    for(auto &it:items) data_[it.id] = {it, it.count}; // 添加所有物品
}
} // namespace hx
