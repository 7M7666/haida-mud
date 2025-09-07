#include "Game.hpp"
#include <cstdlib>
#include <ctime>

int main() {
    // 初始化随机数种子
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    
    hx::Game game; 
    game.run();
    return 0;
}
