#include "task_scheduler.h"
#include <algorithm>

TaskScheduler::TaskScheduler() 
    : overallProgress(0.0), 
      currentTaskIndex(0), 
      currentTaskProgress(std::make_unique<double>(0.0)) {
}

void TaskScheduler::addTask(Task task) {
    tasks.push_back(task);
}

void TaskScheduler::addTasks(const std::vector<Task>& newTasks) {
    tasks.insert(tasks.end(), newTasks.begin(), newTasks.end());
}

double TaskScheduler::getProgress() const {
    if (tasks.empty()) return 1.0; // No tasks means 100% complete
    
    double completedTasksProgress = static_cast<double>(currentTaskIndex) / tasks.size();
    double currentTaskContribution = 0.0;
    
    if (currentTaskIndex < tasks.size() && currentTaskProgress) {
        currentTaskContribution = (*currentTaskProgress) / tasks.size();
    }
    
    return completedTasksProgress + currentTaskContribution;
}

void TaskScheduler::run() {
    if (tasks.empty()) return;
    
    for (currentTaskIndex = 0; currentTaskIndex < tasks.size(); currentTaskIndex++) {
        // Reset progress for new task
        currentTaskProgress = std::make_unique<double>(0.0);
        
        // Execute current task
        tasks[currentTaskIndex](currentTaskProgress);
        
        // Ensure progress is capped at 100% per task
        *currentTaskProgress = std::min(1.0, *currentTaskProgress);
        
        // Update overall progress
        overallProgress = getProgress();
    }
    
    // Ensure we're at 100% when complete
    overallProgress = 1.0;
}

size_t TaskScheduler::getTaskCount() const {
    return tasks.size();
}

size_t TaskScheduler::getCurrentTaskIndex() const {
    return currentTaskIndex;
}