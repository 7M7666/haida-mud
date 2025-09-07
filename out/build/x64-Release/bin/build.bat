@echo off
echo 正在编译海大修仙秘：文心潭秘录 MUD...

REM 检查是否有g++编译器
where g++ >nul 2>nul
if %errorlevel% equ 0 (
    echo 使用 g++ 编译器...
    g++ -std=c++17 -Iinclude src/*.cpp -o haida_mud.exe
    if %errorlevel% equ 0 (
        echo 编译成功！
        echo 运行游戏：haida_mud.exe
    ) else (
        echo 编译失败！
    )
    goto :end
)

REM 检查是否有cl编译器（Visual Studio）
where cl >nul 2>nul
if %errorlevel% equ 0 (
    echo 使用 Visual Studio 编译器...
    cl /std:c++17 /Iinclude src\*.cpp /Fe:haida_mud.exe
    if %errorlevel% equ 0 (
        echo 编译成功！
        echo 运行游戏：haida_mud.exe
    ) else (
        echo 编译失败！
    )
    goto :end
)

REM 检查是否有CMake
where cmake >nul 2>nul
if %errorlevel% equ 0 (
    echo 使用 CMake 构建...
    if not exist build mkdir build
    cd build
    cmake ..
    cmake --build .
    if %errorlevel% equ 0 (
        echo 编译成功！
        echo 运行游戏：build\haida_mud.exe
    ) else (
        echo 编译失败！
    )
    cd ..
    goto :end
)

echo 错误：未找到可用的C++编译器！
echo 请安装以下任一编译器：
echo 1. MinGW-w64 (g++)
echo 2. Visual Studio (cl)
echo 3. CMake + 编译器

:end
pause
