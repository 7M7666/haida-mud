cmake_minimum_required(VERSION 3.16)

# 项目基本信息
project(HaidaXiuXianMUD 
    VERSION 1.0.0
    DESCRIPTION "海大修仙MUD游戏"
    LANGUAGES CXX
)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 编译器设置
if(NOT DEFINED CMAKE_CXX_COMPILER)
    if(WIN32)
        # 在Windows上优先使用MSVC，如果没有则使用MinGW
        find_program(CMAKE_CXX_COMPILER 
            NAMES cl.exe g++.exe gcc.exe
            PATHS 
                "C:/Program Files/Microsoft Visual Studio/*/Community/VC/Tools/MSVC/*/bin/Hostx64/x64"
                "C:/Program Files (x86)/Microsoft Visual Studio/*/Community/VC/Tools/MSVC/*/bin/Hostx64/x64"
                "C:/mingw64/bin"
                "C:/msys64/mingw64/bin"
                "W:/Microsoft VS Code/mingw/mingw64/bin"
            NO_DEFAULT_PATH
        )
    else()
        find_program(CMAKE_CXX_COMPILER NAMES g++ clang++ c++)
    endif()
endif()

# 验证编译器是否找到
if(NOT CMAKE_CXX_COMPILER)
    message(FATAL_ERROR "未找到C++编译器，请确保已安装并配置正确的路径")
endif()

# 输出目录配置 - 统一各平台输出路径
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 为不同构建类型设置子目录，避免混在一起
foreach(CONFIG_TYPE ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${CONFIG_TYPE} CONFIG_TYPE_UPPER)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_TYPE_UPPER} ${CMAKE_BINARY_DIR}/${CONFIG_TYPE}/lib)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG_TYPE_UPPER} ${CMAKE_BINARY_DIR}/${CONFIG_TYPE}/lib)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG_TYPE_UPPER} ${CMAKE_BINARY_DIR}/${CONFIG_TYPE}/bin)
endforeach()

# 链接类型配置 - 默认为静态链接，可通过选项修改
option(BUILD_STATIC "使用静态链接" ON)

if(BUILD_STATIC)
    if(MSVC)
        # MSVC静态运行时库
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
        add_compile_options(/MT$<$<CONFIG:Debug>:d>)
    else()
        # GCC/Clang静态链接
        set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
        set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
        set(BUILD_SHARED_LIBS OFF)
    endif()
else()
    if(MSVC)
        # MSVC动态运行时库
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    else()
        # 清除静态链接标志
        string(REPLACE "-static-libgcc -static-libstdc++" "" CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS})
        set(CMAKE_FIND_LIBRARY_SUFFIXES ".so" ".dylib" ".dll")
        set(BUILD_SHARED_LIBS ON)
    endif()
endif()

# 编译选项配置
if(MSVC)
    # MSVC编译器选项
    add_compile_options(/W4 /permissive- /utf-8)
    
    # 调试配置
    add_compile_options($<$<CONFIG:Debug>:/Zi>)       # 生成调试信息
    add_compile_options($<$<CONFIG:Debug>:/Od>)       # 禁用优化
    
    # 发布配置
    add_compile_options($<$<CONFIG:Release>:/O2>)     # 最大化优化
    add_compile_options($<$<CONFIG:Release>:/GL>)     # 全程序优化
    add_link_options($<$<CONFIG:Release>:/LTCG>)      # 链接时代码生成
    
    # 所有配置通用
    add_compile_options(/EHsc)                        # 启用标准C++异常处理
else()
    # GCC/Clang编译器选项
    add_compile_options(-Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion)
    
    # 调试配置
    add_compile_options($<$<CONFIG:Debug>:-g>)        # 生成调试信息
    add_compile_options($<$<CONFIG:Debug>:-O0>)       # 禁用优化
    
    # 发布配置
    add_compile_options($<$<CONFIG:Release>:-O3>)     # 最大化优化
    add_compile_options($<$<CONFIG:Release>:-march=native>)  # 针对本地CPU优化
    
    # 可选：地址消毒器支持
    option(ENABLE_ASAN "启用地址消毒器(仅调试模式)" OFF)
    if(ENABLE_ASAN)
        add_compile_options($<$<CONFIG:Debug>:-fsanitize=address>)
        add_link_options($<$<CONFIG:Debug>:-fsanitize=address>)
    endif()
endif()

# 收集源文件 - 自动查找src目录下的所有.cpp文件
file(GLOB_RECURSE SOURCES 
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
)

# 排除可能不需要的文件
list(FILTER SOURCES EXCLUDE REGEX ".*test.*")

# 创建可执行文件
add_executable(haida_mud ${SOURCES})

# 设置包含目录
target_include_directories(haida_mud PUBLIC 
    ${CMAKE_SOURCE_DIR}/include
)

# 编译定义
target_compile_definitions(haida_mud PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
    PROJECT_VERSION="${PROJECT_VERSION}"
    $<$<PLATFORM_ID:Windows>:PLATFORM_WINDOWS>
    $<$<PLATFORM_ID:Linux>:PLATFORM_LINUX>
    $<$<PLATFORM_ID:Darwin>:PLATFORM_MACOS>
)

# 安装规则
install(TARGETS haida_mud 
    RUNTIME DESTINATION bin
    BUNDLE DESTINATION .
)

# 安装资源文件
install(FILES save.dat DESTINATION bin)

# 创建启动脚本（Windows）
if(WIN32)
    configure_file(
        "${CMAKE_SOURCE_DIR}/build.bat"
        "${CMAKE_BINARY_DIR}/bin/build.bat"
        COPYONLY
    )
endif()

# 添加清理目标
add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}
    COMMENT "Cleaning all build files"
)

# CPack配置用于打包
include(CPack)
set(CPACK_PACKAGE_NAME "HaidaXiuXianMUD")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "海大修仙MUD游戏")
set(CPACK_PACKAGE_VENDOR "HaidaXiuXian Team")
set(CPACK_PACKAGE_CONTACT "your-email@example.com")  # 替换为实际联系方式

# 根据平台设置打包生成器
if(WIN32)
    set(CPACK_GENERATOR "ZIP;NSIS")  # 同时支持ZIP和安装程序
    set(CPACK_PACKAGE_FILE_NAME "HaidaXiuXianMUD-${CPACK_PACKAGE_VERSION}-Windows")
    set(CPACK_NSIS_DISPLAY_NAME "HaidaXiuXianMUD")
    set(CPACK_NSIS_PACKAGE_NAME "HaidaXiuXianMUD ${CPACK_PACKAGE_VERSION}")
elseif(APPLE)
    set(CPACK_GENERATOR "TGZ;DragNDrop")
    set(CPACK_PACKAGE_FILE_NAME "HaidaXiuXianMUD-${CPACK_PACKAGE_VERSION}-macOS")
else()
    set(CPACK_GENERATOR "TGZ;DEB")  # Linux支持TGZ和DEB包
    set(CPACK_PACKAGE_FILE_NAME "HaidaXiuXianMUD-${CPACK_PACKAGE_VERSION}-Linux")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "HaidaXiuXian Team")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.27)")  # 根据实际依赖调整
endif()
