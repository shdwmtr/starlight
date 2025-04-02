#ifndef TASK_SCHEDULER_H
#define TASK_SCHEDULER_H

#include <vector>
#include <functional>
#include <memory>

class TaskScheduler {
public:
    // Define task type: function that takes a unique pointer to a double for progress tracking
    using Task = std::function<void(std::unique_ptr<double>&)>;

    // Constructor
    TaskScheduler();
    
    // Add a single task to the scheduler
    void addTask(Task task);

    // Add multiple tasks at once
    void addTasks(const std::vector<Task>& newTasks);

    // Get the current overall progress (0.0 to 1.0)
    double getProgress() const;

    // Execute all tasks in sequence
    void run();

    // Get number of tasks
    size_t getTaskCount() const;

    // Get current task index (0-based)
    size_t getCurrentTaskIndex() const;

private:
    std::vector<Task> tasks;
    double overallProgress;
    size_t currentTaskIndex;
    std::unique_ptr<double> currentTaskProgress;
};

#endif // TASK_SCHEDULER_H