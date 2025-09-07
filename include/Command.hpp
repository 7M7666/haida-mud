#pragma once
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>

namespace hx {
class Game; // fwd

class CommandRouter {
public:
    using Handler = std::function<void(Game&, const std::vector<std::string>&)>;
    void add(const std::string& name, Handler h);
    bool route(Game& g, const std::string& line);

private:
    std::unordered_map<std::string, Handler> handlers_;
};

std::vector<std::string> splitWords(const std::string& line);
} // namespace hx
