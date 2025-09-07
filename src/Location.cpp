#include "Location.hpp"

namespace hx {
const Exit* Location::findExitByLabel(const std::string& label) const{ 
    for(auto &e:exits) if(e.label==label) return &e; 
    return nullptr; 
} 

const NPC* Location::findNPC(const std::string& name) const {
    for(const auto& npc : npcs) {
        if(npc.name() == name) return &npc;
    }
    return nullptr;
}
} // namespace hx
