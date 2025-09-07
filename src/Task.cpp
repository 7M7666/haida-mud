#include "Task.hpp"
#include "Player.hpp"
#include <sstream>
#include <algorithm>
#include <iostream>

namespace hx {

// Task类实现
Task::Task(const std::string& id, const std::string& name, const std::string& description,
           TaskType type, const std::vector<TaskReward>& rewards)
    : id_(id), name_(name), description_(description), type_(type), status_(TaskStatus::NOT_STARTED), rewards_(rewards) {}

void Task::start() {
    if (status_ == TaskStatus::NOT_STARTED) {
        status_ = TaskStatus::IN_PROGRESS;
    }
}

void Task::complete() {
    if (status_ == TaskStatus::IN_PROGRESS) {
        status_ = TaskStatus::COMPLETED;
    }
}

void Task::fail() {
    if (status_ == TaskStatus::IN_PROGRESS) {
        status_ = TaskStatus::FAILED;
    }
}

void Task::addObjective(const std::string& objective) {
    objectives_.push_back(objective);
}

void Task::setObjective(size_t index, const std::string& text) {
    if (index < objectives_.size()) objectives_[index] = text;
}

std::string Task::getStatusString() const {
    switch (status_) {
        case TaskStatus::NOT_STARTED: return "未接取";
        case TaskStatus::IN_PROGRESS: return "进行中";
        case TaskStatus::COMPLETED: return "已完成";
        case TaskStatus::FAILED: return "失败";
        default: return "未知";
    }
}

std::string Task::getTypeString() const {
    switch (type_) {
        case TaskType::MAIN: return "主线";
        case TaskType::SIDE: return "支线";
        case TaskType::DAILY: return "日常";
        default: return "未知";
    }
}

std::string Task::getFullInfo() const {
    std::ostringstream oss;
    
    // 任务标题
    std::string status_icon = "";
    switch (status_) {
        case TaskStatus::NOT_STARTED: status_icon = "○"; break;
        case TaskStatus::IN_PROGRESS: status_icon = "●"; break;
        case TaskStatus::COMPLETED: status_icon = "✓"; break;
        case TaskStatus::FAILED: status_icon = "✗"; break;
    }
    
    oss << "【" << getTypeString() << "任务】" << status_icon << " " << name_ << "\n";
    oss << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    oss << "状态: " << getStatusString() << "\n";
    oss << "描述: " << description_ << "\n\n";
    
    if (!objectives_.empty()) {
        oss << "📋 任务目标:\n";
        for (size_t i = 0; i < objectives_.size(); ++i) {
            oss << "   " << (i + 1) << ". " << objectives_[i] << "\n";
        }
        oss << "\n";
    }
    
    if (!rewards_.empty()) {
        oss << "🎁 任务奖励:\n";
        for (const auto& reward : rewards_) {
            if (reward.quantity > 0) {
                oss << "   • " << reward.item_name << " x" << reward.quantity;
                if (!reward.description.empty()) {
                    oss << " (" << reward.description << ")";
                }
                oss << "\n";
            }
            if (reward.exp_reward > 0) {
                oss << "   • 经验值 +" << reward.exp_reward << "\n";
            }
            if (reward.coin_reward > 0) {
                oss << "   • 金币 +" << reward.coin_reward << "\n";
            }
        }
    }
    
    return oss.str();
}

// TaskManager类实现
TaskManager::TaskManager() = default;

void TaskManager::addTask(const Task& task) {
    tasks_.push_back(task);
}

Task* TaskManager::getTask(const std::string& task_id) {
    auto it = std::find_if(tasks_.begin(), tasks_.end(),
        [&task_id](const Task& task) { return task.getId() == task_id; });
    return it != tasks_.end() ? &(*it) : nullptr;
}

const Task* TaskManager::getTask(const std::string& task_id) const {
    auto it = std::find_if(tasks_.begin(), tasks_.end(),
        [&task_id](const Task& task) { return task.getId() == task_id; });
    return it != tasks_.end() ? &(*it) : nullptr;
}

void TaskManager::startTask(const std::string& task_id) {
    Task* task = getTask(task_id);
    if (task) {
        task->start();
    }
}

void TaskManager::completeTask(const std::string& task_id) {
    Task* task = getTask(task_id);
    if (task) {
        task->complete();
        std::cout << "\033[31m【任务完成】" << task->getName() << " 已完成！\033[0m\n";
    }
}

void TaskManager::failTask(const std::string& task_id) {
    Task* task = getTask(task_id);
    if (task) {
        task->fail();
    }
}

std::vector<Task*> TaskManager::getActiveTasks() {
    std::vector<Task*> active;
    for (auto& task : tasks_) {
        if (task.getStatus() == TaskStatus::IN_PROGRESS) {
            active.push_back(&task);
        }
    }
    return active;
}

std::vector<Task*> TaskManager::getCompletedTasks() {
    std::vector<Task*> completed;
    for (auto& task : tasks_) {
        if (task.getStatus() == TaskStatus::COMPLETED) {
            completed.push_back(&task);
        }
    }
    return completed;
}

std::vector<Task*> TaskManager::getTasksByType(TaskType type) {
    std::vector<Task*> filtered;
    for (auto& task : tasks_) {
        if (task.getType() == type) {
            filtered.push_back(&task);
        }
    }
    return filtered;
}

std::vector<Task*> TaskManager::getAllTasks() {
    std::vector<Task*> all;
    for (auto& task : tasks_) {
        all.push_back(&task);
    }
    return all;
}

bool TaskManager::hasActiveTask(const std::string& task_id) const {
    const Task* task = getTask(task_id);
    return task && task->getStatus() == TaskStatus::IN_PROGRESS;
}

bool TaskManager::hasCompletedTask(const std::string& task_id) const {
    const Task* task = getTask(task_id);
    return task && task->getStatus() == TaskStatus::COMPLETED;
}

int TaskManager::getCompletedTaskCount() const {
    int count = 0;
    for (const auto& task : tasks_) {
        if (task.getStatus() == TaskStatus::COMPLETED) {
            count++;
        }
    }
    return count;
}

void TaskManager::showTaskList() const {
    std::cout << "\n=== 任务列表 ===\n";
    
    // 主线任务
    std::cout << "\n【主线任务】\n";
    bool has_main = false;
    for (const auto& task : tasks_) {
        if (task.getType() == TaskType::MAIN) {
            std::string status_icon = "";
            switch (task.getStatus()) {
                case TaskStatus::NOT_STARTED: status_icon = "○"; break;
                case TaskStatus::IN_PROGRESS: status_icon = "●"; break;
                case TaskStatus::COMPLETED: status_icon = "✓"; break;
                case TaskStatus::FAILED: status_icon = "✗"; break;
            }
            std::cout << "  " << status_icon << " " << task.getName() << " [" << task.getStatusString() << "]\n";
            has_main = true;
        }
    }
    if (!has_main) {
        std::cout << "  暂无主线任务\n";
    }
    
    // 支线任务
    std::cout << "\n【支线任务】\n";
    bool has_side = false;
    for (const auto& task : tasks_) {
        if (task.getType() == TaskType::SIDE) {
            std::string status_icon = "";
            switch (task.getStatus()) {
                case TaskStatus::NOT_STARTED: status_icon = "○"; break;
                case TaskStatus::IN_PROGRESS: status_icon = "●"; break;
                case TaskStatus::COMPLETED: status_icon = "✓"; break;
                case TaskStatus::FAILED: status_icon = "✗"; break;
            }
            std::cout << "  " << status_icon << " " << task.getName() << " [" << task.getStatusString() << "]\n";
            has_side = true;
        }
    }
    if (!has_side) {
        std::cout << "  暂无支线任务\n";
    }
    
    std::cout << "\n使用 'task <任务名>' 查看详细信息\n";
    std::cout << "状态说明: ○未接取 ●进行中 ✓已完成 ✗失败\n";
}

void TaskManager::showTaskList(const Player& player) const {
    std::cout << "\n=== 任务列表 ===\n";
    
    // 显示林清漪好感度
    int favor = player.getNPCFavor("林清漪");
    std::cout << "❤️  林清漪好感度: " << favor << "\n";
    std::cout << std::string(20, '-') << "\n";
    
    // 主线任务
    std::cout << "\n【主线任务】\n";
    bool has_main = false;
    for (const auto& task : tasks_) {
        if (task.getType() == TaskType::MAIN) {
            std::string status_icon = "";
            switch (task.getStatus()) {
                case TaskStatus::NOT_STARTED: status_icon = "○"; break;
                case TaskStatus::IN_PROGRESS: status_icon = "●"; break;
                case TaskStatus::COMPLETED: status_icon = "✓"; break;
                case TaskStatus::FAILED: status_icon = "✗"; break;
            }
            std::cout << "  " << status_icon << " " << task.getName() << " [" << task.getStatusString() << "]\n";
            has_main = true;
        }
    }
    if (!has_main) {
        std::cout << "  暂无主线任务\n";
    }
    
    // 支线任务
    std::cout << "\n【支线任务】\n";
    bool has_side = false;
    for (const auto& task : tasks_) {
        if (task.getType() == TaskType::SIDE) {
            std::string status_icon = "";
            switch (task.getStatus()) {
                case TaskStatus::NOT_STARTED: status_icon = "○"; break;
                case TaskStatus::IN_PROGRESS: status_icon = "●"; break;
                case TaskStatus::COMPLETED: status_icon = "✓"; break;
                case TaskStatus::FAILED: status_icon = "✗"; break;
            }
            std::cout << "  " << status_icon << " " << task.getName() << " [" << task.getStatusString() << "]\n";
            has_side = true;
        }
    }
    if (!has_side) {
        std::cout << "  暂无支线任务\n";
    }
    
    std::cout << "\n使用 'task <任务名>' 查看详细信息\n";
    std::cout << "状态说明: ○未接取 ●进行中 ✓已完成 ✗失败\n";
}

void TaskManager::showTaskDetails(const std::string& task_identifier) const {
    // 首先尝试按ID查找
    const Task* task = getTask(task_identifier);
    
    // 如果按ID没找到，尝试按名称查找
    if (!task) {
        auto it = std::find_if(tasks_.begin(), tasks_.end(),
            [&task_identifier](const Task& t) { return t.getName() == task_identifier; });
        task = it != tasks_.end() ? &(*it) : nullptr;
    }
    
    if (task) {
        std::cout << "\n" << task->getFullInfo() << "\n";
    } else {
        std::cout << "未找到任务: " << task_identifier << "\n";
    }
}

std::vector<Task> TaskManager::getAllTasksData() const {
    return tasks_;
}

void TaskManager::setAllTasksData(const std::vector<Task>& tasks) {
    tasks_ = tasks;
}

} // namespace hx
