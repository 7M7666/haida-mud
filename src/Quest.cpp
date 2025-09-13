// 这是任务系统的实现文件
// 作者：大一学生
// 功能：实现游戏中的任务系统，包括任务日志管理

#include "Quest.hpp"  // 任务类头文件

namespace hx {
// 添加任务
void QuestLog::add(const Quest& q) { 
    data_[q.id] = q;  // 以任务ID为键存储任务
}

// 完成任务
void QuestLog::complete(const std::string& id) { 
    auto it = data_.find(id);           // 查找任务
    if (it!=data_.end()) it->second.completed = true;  // 如果找到，标记为完成
}

// 检查任务是否完成
bool QuestLog::isCompleted(const std::string& id) const { 
    auto it=data_.find(id);             // 查找任务
    return it!=data_.end() && it->second.completed;  // 返回是否存在且已完成
}
} // namespace hx
