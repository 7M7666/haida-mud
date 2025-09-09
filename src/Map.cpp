// 这是地图系统的实现文件
// 作者：大一学生
// 功能：实现游戏中的地图渲染和导航系统

#include "Map.hpp"        // 地图类头文件
#include <iostream>        // 输入输出流
#include <algorithm>       // 算法库
#include <sstream>         // 字符串流
#include <map>             // 映射容器
#include <windows.h>       // Windows控制台颜色支持

namespace hx {

// Windows控制台颜色设置函数
// 参数：color(颜色代码)
// 功能：设置控制台文字的颜色，用于美化地图显示
void setConsoleColor(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);  // 获取控制台句柄
    SetConsoleTextAttribute(hConsole, color);           // 设置文字属性
}

void Map::addLocation(const Location& loc){ data_[loc.id]=loc; }
const Location* Map::get(const std::string& id) const{ auto it=data_.find(id); return it==data_.end()?nullptr:&it->second; }
Location* Map::get(const std::string& id){ auto it=data_.find(id); return it==data_.end()?nullptr:&it->second; }
std::vector<Location> Map::allLocations() const{ std::vector<Location> out; for(auto &kv:data_) out.push_back(kv.second); return out; }

// 检查是否为教学区地点
bool Map::isTeachingAreaLocation(const std::string& locationId) const {
    return std::find(teachingAreaLocations_.begin(), teachingAreaLocations_.end(), locationId) != teachingAreaLocations_.end();
}

// 主地图渲染 - 单条走廊+分支结构
std::string Map::renderMainMap(const std::string& currentId) const {
    std::stringstream ss;
    ss << "=== 主地图 ===\n";
    
    // 地图模板 - 使用占位符表示地名
    std::string mapTemplate = 
        "{north_playground} —— {canteen}\n"
        "      |\n"
        "{activity_center} —— {gymnasium} —— {plaza_36} —— {teaching_area} —— {info_building} —— {library} —— {wenxintan}\n";
    
    // 地名映射
    std::map<std::string, std::string> locationNames = {
        {"north_playground", "荒废北操场"},
        {"canteen", "食堂"},
        {"activity_center", "大学生活动中心"},
        {"gymnasium", "体育馆"},
        {"plaza_36", "三六广场"},
        {"teaching_area", "教学区"},
        {"info_building", "信息楼"},
        {"library", "秘境图书馆"},
        {"wenxintan", "文心潭"}
    };
    
    // 渲染地图，替换占位符
    std::string result = mapTemplate;
    for (const auto& [id, name] : locationNames) {
        std::string placeholder = "{" + id + "}";
        std::string displayName;
        
        if (id == currentId) {
            // 当前位置用黄色高亮
            displayName = "\033[33m" + name + "\033[0m";
        } else {
            displayName = name;
        }
        
        // 替换占位符
        size_t pos = result.find(placeholder);
        if (pos != std::string::npos) {
            result.replace(pos, placeholder.length(), displayName);
        }
    }
    
    ss << result;
    return ss.str();
}

// 教学区子地图渲染 - 树形分支结构
std::string Map::renderTeachingDetailMap(const std::string& currentId) const {
    std::stringstream ss;
    ss << "=== 教学区地图 ===\n";
    
    // 地图模板 - 使用占位符表示地名
    std::string mapTemplate = 
        "{teach_2} —— {teach_3} —— {jiuzhutan} —— {teach_4}\n"
        "                    |\n"
        "                  {teach_5} —— {teach_6} —— {teach_7}\n"
        "                                        |\n"
        "                                      {tree_space}\n";
    
    // 地名映射
    std::map<std::string, std::string> locationNames = {
        {"teach_2", "教学楼二区"},
        {"teach_3", "教学楼三区"},
        {"teach_4", "教学楼四区"},
        {"teach_5", "教学楼五区"},
        {"jiuzhutan", "九珠坛"},
        {"teach_6", "教学楼六区"},
        {"teach_7", "教学楼七区"},
        {"tree_space", "树下空间"}
    };
    
    // 渲染地图，替换占位符
    std::string result = mapTemplate;
    for (const auto& [id, name] : locationNames) {
        std::string placeholder = "{" + id + "}";
        std::string displayName;
        
        if (id == currentId) {
            // 当前位置用黄色高亮
            displayName = "\033[33m" + name + "\033[0m";
        } else {
            displayName = name;
        }
        
        // 替换占位符
        size_t pos = result.find(placeholder);
        if (pos != std::string::npos) {
            result.replace(pos, placeholder.length(), displayName);
        }
    }
    
    ss << result;
    return ss.str();
}

// 增强版主地图渲染 - 使用树状结构和更好的排版
std::string Map::renderEnhancedMainMap(const std::string& currentId) const {
    std::stringstream ss;
    
    // 地名映射
    std::map<std::string, std::string> locationNames = {
        {"north_playground", "荒废北操场"},
        {"canteen", "食堂"},
        {"activity_center", "大学生活动中心"},
        {"gymnasium", "体育馆"},
        {"plaza_36", "三六广场"},
        {"teaching_area", "教学区"},
        {"info_building", "信息楼"},
        {"library", "秘境图书馆"},
        {"wenxintan", "文心潭"}
    };
    
    // 构建树状地图
    ss << "                    🏫 海大校园秘境\n";
    ss << "                         │\n";
    ss << "                    【" << (currentId == "library" ? "秘境图书馆" : "图书馆") << "】\n";
    ss << "                         │\n";
    ss << "    【" << (currentId == "north_playground" ? "荒废北操场" : "北操场") << "】──【" << (currentId == "canteen" ? "食堂" : "食堂") << "】\n";
    ss << "                         │\n";
    ss << "【" << (currentId == "activity_center" ? "活动中心" : "活动中心") << "】──【" << (currentId == "gymnasium" ? "体育馆" : "体育馆") << "】──【" << (currentId == "plaza_36" ? "三六广场" : "三六广场") << "】──【" << (currentId == "teaching_area" ? "教学区" : "教学区") << "】──【" << (currentId == "info_building" ? "信息楼" : "信息楼") << "】──【" << (currentId == "library" ? "秘境图书馆" : "图书馆") << "】──【" << (currentId == "wenxintan" ? "文心潭" : "文心潭") << "】\n";
    
    // 添加当前位置高亮
    ss << "\n📍 当前位置：【" << locationNames[currentId] << "】\n";
    
    return ss.str();
}

// 增强版教学区地图渲染 - 使用树状结构
std::string Map::renderEnhancedTeachingDetailMap(const std::string& currentId) const {
    std::stringstream ss;
    
    // 地名映射
    std::map<std::string, std::string> locationNames = {
        {"teach_2", "教学楼二区"},
        {"teach_3", "教学楼三区"},
        {"teach_4", "教学楼四区"},
        {"teach_5", "教学楼五区"},
        {"jiuzhutan", "九珠坛"},
        {"teach_6", "教学楼六区"},
        {"teach_7", "教学楼七区"},
        {"tree_space", "树下空间"}
    };
    
    // 构建树状地图
    ss << "                    🏛️ 教学区详细地图\n";
    ss << "                         │\n";
    ss << "【" << (currentId == "teach_2" ? "教学楼二区" : "二区") << "】──【" << (currentId == "teach_3" ? "教学楼三区" : "三区") << "】──【" << (currentId == "jiuzhutan" ? "九珠坛" : "九珠坛") << "】──【" << (currentId == "teach_4" ? "教学楼四区" : "四区") << "】\n";
    ss << "                         │\n";
    ss << "                    【" << (currentId == "teach_5" ? "教学楼五区" : "五区") << "】──【" << (currentId == "teach_6" ? "教学楼六区" : "六区") << "】──【" << (currentId == "teach_7" ? "教学楼七区" : "七区") << "】\n";
    ss << "                                              │\n";
    ss << "                                          【" << (currentId == "tree_space" ? "树下空间" : "树下空间") << "】\n";
    
    // 添加当前位置高亮
    ss << "\n📍 当前位置：【" << locationNames[currentId] << "】\n";
    
    return ss.str();
}

} // namespace hx
