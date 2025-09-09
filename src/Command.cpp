// 这是命令系统的实现文件
// 作者：大一学生
// 功能：实现游戏的命令路由系统，处理玩家的输入命令

#include "Command.hpp"  // 命令系统头文件
#include <sstream>       // 字符串流
#include <cctype>        // 字符类型判断

namespace hx {
// 添加命令处理器
// 参数：name(命令名称), h(处理函数)
// 功能：将命令名称和处理函数绑定
void CommandRouter::add(const std::string& name, Handler h) { 
    handlers_[name] = std::move(h);  // 将处理函数存储到映射中
}

static std::string toLower(std::string s){ for(auto& c:s) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c))); return s; }

bool CommandRouter::route(Game& g, const std::string& line) {
    auto words = splitWords(line);
    if (words.empty()) return false;
    auto it = handlers_.find(toLower(words[0]));
    if (it == handlers_.end()) return false;
    it->second(g, words);
    return true;
}

std::vector<std::string> splitWords(const std::string& line) {
    std::istringstream iss(line); std::vector<std::string> out; std::string w; while(iss>>w) out.push_back(w); return out;
}
} // namespace hx
