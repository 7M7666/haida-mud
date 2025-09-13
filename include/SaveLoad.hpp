// 存档读档系统
#pragma once
#include "GameState.hpp"
#include <string>

namespace hx {

class SaveLoad { 
public: 
    static bool save(const GameState& state, const std::string& filename="save.dat"); 
    static bool load(GameState& state, const std::string& filename="save.dat"); 
};
}
