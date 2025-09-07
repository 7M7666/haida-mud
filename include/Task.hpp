#pragma once
#include <string>
#include <vector>

namespace hx {

// 前向声明
class Player;

// 任务状态
enum class TaskStatus {
    NOT_STARTED,    // 未接取
    IN_PROGRESS,    // 进行中
    COMPLETED,      // 已完成
    FAILED          // 失败
};

// 任务类型
enum class TaskType {
    MAIN,           // 主线任务
    SIDE,           // 支线任务
    DAILY           // 日常任务
};

// 任务奖励
struct TaskReward {
    std::string item_id;
    std::string item_name;
    int quantity;
    int exp_reward;
    int coin_reward;
    std::string description;
};

// 任务类
class Task {
public:
    Task(const std::string& id, const std::string& name, const std::string& description,
         TaskType type, const std::vector<TaskReward>& rewards);
    
    // 基本信息
    const std::string& getId() const { return id_; }
    const std::string& getName() const { return name_; }
    const std::string& getDescription() const { return description_; }
    TaskType getType() const { return type_; }
    TaskStatus getStatus() const { return status_; }
    
    // 状态管理
    void start();
    void complete();
    void fail();
    void setStatus(TaskStatus status) { status_ = status; }
    
    // 奖励
    const std::vector<TaskReward>& getRewards() const { return rewards_; }
    
    // 完成条件
    void addObjective(const std::string& objective);
    const std::vector<std::string>& getObjectives() const { return objectives_; }
    bool isCompleted() const { return status_ == TaskStatus::COMPLETED; }
    // 更新单条目标文本（用于显示实时进度）
    void setObjective(size_t index, const std::string& text);
    
    // 任务信息显示
    std::string getStatusString() const;
    std::string getTypeString() const;
    std::string getFullInfo() const;

private:
    std::string id_;
    std::string name_;
    std::string description_;
    TaskType type_;
    TaskStatus status_;
    std::vector<TaskReward> rewards_;
    std::vector<std::string> objectives_;
};

// 任务管理器
class TaskManager {
public:
    TaskManager();
    
    // 任务管理
    void addTask(const Task& task);
    Task* getTask(const std::string& task_id);
    const Task* getTask(const std::string& task_id) const;
    
    // 任务状态
    void startTask(const std::string& task_id);
    void completeTask(const std::string& task_id);
    void failTask(const std::string& task_id);
    
    // 任务查询
    std::vector<Task*> getActiveTasks();
    std::vector<Task*> getCompletedTasks();
    std::vector<Task*> getTasksByType(TaskType type);
    std::vector<Task*> getAllTasks();
    
    // 任务进度
    bool hasActiveTask(const std::string& task_id) const;
    bool hasCompletedTask(const std::string& task_id) const;
    int getCompletedTaskCount() const;
    
    // 任务信息显示
    void showTaskList() const;
    void showTaskList(const Player& player) const;
    void showTaskDetails(const std::string& task_id) const;
    
    // 序列化方法
    std::vector<Task> getAllTasksData() const;
    void setAllTasksData(const std::vector<Task>& tasks);

private:
    std::vector<Task> tasks_;
};

} // namespace hx
