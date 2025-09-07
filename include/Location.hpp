#pragma once
#include <string>
#include <vector>
#include "Enemy.hpp"
#include "Item.hpp"
#include "NPC.hpp"

namespace hx {
struct Exit { std::string label; std::string to; };
struct Coord { int x; int y; };
class Location {
public:
    std::string id;
    std::string name;
    std::string desc;
    Coord coord{0,0};
    std::vector<Exit> exits;
    std::vector<Enemy> enemies;
    std::vector<Item> shop;
    std::vector<NPC> npcs;
    const Exit* findExitByLabel(const std::string& label) const;
    const NPC* findNPC(const std::string& name) const;
};
} // namespace hx
