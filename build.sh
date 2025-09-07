#!/bin/bash

echo "正在编译海大修仙秘：文心潭秘录 MUD..."

# 检查是否有g++编译器
if command -v g++ &> /dev/null; then
    echo "使用 g++ 编译器..."
    g++ -std=c++17 -Iinclude src/*.cpp -o haida_mud
    if [ $? -eq 0 ]; then
        echo "编译成功！"
        echo "运行游戏：./haida_mud"
        chmod +x haida_mud
    else
        echo "编译失败！"
    fi
    exit 0
fi

# 检查是否有clang++编译器
if command -v clang++ &> /dev/null; then
    echo "使用 clang++ 编译器..."
    clang++ -std=c++17 -Iinclude src/*.cpp -o haida_mud
    if [ $? -eq 0 ]; then
        echo "编译成功！"
        echo "运行游戏：./haida_mud"
        chmod +x haida_mud
    else
        echo "编译失败！"
    fi
    exit 0
fi

# 检查是否有CMake
if command -v cmake &> /dev/null; then
    echo "使用 CMake 构建..."
    mkdir -p build
    cd build
    cmake ..
    cmake --build .
    if [ $? -eq 0 ]; then
        echo "编译成功！"
        echo "运行游戏：./haida_mud"
    else
        echo "编译失败！"
    fi
    cd ..
    exit 0
fi

echo "错误：未找到可用的C++编译器！"
echo "请安装以下任一编译器："
echo "1. g++ (GNU C++ Compiler)"
echo "2. clang++ (LLVM C++ Compiler)"
echo "3. CMake + 编译器"
exit 1
