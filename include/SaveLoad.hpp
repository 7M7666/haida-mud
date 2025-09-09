// 这是存档读档系统的头文件
// 作者：大一学生
// 功能：定义游戏的存档和读档功能

#pragma once
#include "GameState.hpp"  // 游戏状态
#include <string>         // 字符串

namespace hx {
// 存档读档类
// 功能：提供游戏的存档和读档功能
class SaveLoad { 
public: 
    // 保存游戏状态到文件
    // 参数：state(游戏状态), filename(文件名，默认为save.dat)
    // 返回值：true表示保存成功，false表示保存失败
    static bool save(const GameState& state, const std::string& filename="save.dat"); 
    
    // 从文件加载游戏状态
    // 参数：state(要加载到的游戏状态), filename(文件名，默认为save.dat)
    // 返回值：true表示加载成功，false表示加载失败
    static bool load(GameState& state, const std::string& filename="save.dat"); 
};
}
