// 这是任务系统的头文件
// 作者：大一学生
// 功能：定义游戏中的任务系统，包括任务和任务日志

#pragma once
#include <string>         // 字符串
#include <unordered_map>  // 哈希映射

namespace hx {
// 任务结构体
// 功能：定义游戏中的一个任务
struct Quest { 
    std::string id;        // 任务ID
    std::string name;      // 任务名称
    std::string desc;      // 任务描述
    bool completed{false}; // 是否已完成
};

// 任务日志类
// 功能：管理玩家的所有任务
class QuestLog { 
public: 
    // 添加任务到日志
    void add(const Quest& q); 
    
    // 完成任务
    void complete(const std::string& id); 
    
    // 检查任务是否已完成
    bool isCompleted(const std::string& id) const; 
    
private: 
    std::unordered_map<std::string, Quest> data_;  // 任务数据存储
};
} // namespace hx
