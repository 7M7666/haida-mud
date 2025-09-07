#pragma once
#include <string>
#include <unordered_map>

namespace hx {
struct Quest { std::string id; std::string name; std::string desc; bool completed{false}; };
class QuestLog { 
public: 
    void add(const Quest& q); 
    void complete(const std::string& id); 
    bool isCompleted(const std::string& id) const; 
private: 
    std::unordered_map<std::string, Quest> data_; 
};
} // namespace hx
