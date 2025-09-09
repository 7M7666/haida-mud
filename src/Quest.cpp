// 这是任务系统的实现文件
// 作者：大一学生
// 功能：实现游戏中的任务系统，包括任务日志管理

#include "Quest.hpp"  // 任务类头文件

namespace hx {
// 添加任务到任务日志
// 参数：q(任务对象)
// 功能：将任务添加到任务日志中
void QuestLog::add(const Quest& q) { 
    data_[q.id] = q;  // 以任务ID为键存储任务
}

// 完成任务
// 参数：id(任务ID)
// 功能：将指定任务标记为已完成
void QuestLog::complete(const std::string& id) { 
    auto it = data_.find(id);           // 查找任务
    if (it!=data_.end()) it->second.completed = true;  // 如果找到，标记为完成
}

// 检查任务是否已完成
// 参数：id(任务ID)
// 返回值：true表示已完成，false表示未完成或不存在
// 功能：检查指定任务是否已经完成
bool QuestLog::isCompleted(const std::string& id) const { 
    auto it=data_.find(id);             // 查找任务
    return it!=data_.end() && it->second.completed;  // 返回是否存在且已完成
}
} // namespace hx
