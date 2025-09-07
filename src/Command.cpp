#include "Command.hpp"
#include <sstream>
#include <cctype>

namespace hx {
void CommandRouter::add(const std::string& name, Handler h) { handlers_[name] = std::move(h); }

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
