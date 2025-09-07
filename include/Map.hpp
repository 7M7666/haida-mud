#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include "Location.hpp"

namespace hx {
class Map {
public:
    void addLocation(const Location& loc);
    const Location* get(const std::string& id) const;
    Location* get(const std::string& id);
    std::vector<Location> allLocations() const;
    // build simple ASCII map bounded by min/max coords
    std::string renderAsciiWithHighlight(const std::string& currentId) const;
    
    // 新增：主地图渲染
    std::string renderMainMap(const std::string& currentId) const;
    
    // 新增：教学区子地图渲染
    std::string renderTeachingDetailMap(const std::string& currentId) const;
    
    // 新增：增强版地图渲染
    std::string renderEnhancedMainMap(const std::string& currentId) const;
    std::string renderEnhancedTeachingDetailMap(const std::string& currentId) const;
    
    // 新增：检查是否为教学区地点
    bool isTeachingAreaLocation(const std::string& locationId) const;
    
private:
    std::unordered_map<std::string, Location> data_;
    
    // 主地图地点ID列表
    std::vector<std::string> mainMapLocations_ = {
        "north_playground", "canteen", "activity_center", "gymnasium", 
        "plaza_36", "teaching_area", "info_building", "library", "wenxintan"
    };
    
    // 教学区子地图地点ID列表
    std::vector<std::string> teachingAreaLocations_ = {
        "teach_2", "teach_3", "teach_4", "jiuzhutan", 
        "teach_5", "teach_6", "teach_7", "tree_space"
    };
};
} // namespace hx
