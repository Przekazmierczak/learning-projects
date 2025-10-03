/*
There is a task management system that allows users to manage their tasks, each associated with a priority. The system should efficiently handle adding, modifying, executing, and removing tasks.
Implement the TaskManager class:
TaskManager(vector<vector<int>>& tasks) initializes the task manager with a list of user-task-priority triples. Each element in the input list is of the form [userId, taskId, priority], which adds a task to the specified user with the given priority.
void add(int userId, int taskId, int priority) adds a task with the specified taskId and priority to the user with userId. It is guaranteed that taskId does not exist in the system.
void edit(int taskId, int newPriority) updates the priority of the existing taskId to newPriority. It is guaranteed that taskId exists in the system.
void rmv(int taskId) removes the task identified by taskId from the system. It is guaranteed that taskId exists in the system.
int execTop() executes the task with the highest priority across all users. If there are multiple tasks with the same highest priority, execute the one with the highest taskId. After executing, the taskId is removed from the system. Return the userId associated with the executed task. If no tasks are available, return -1.
Note that a user may be assigned multiple tasks.

Example 1:
Input:
["TaskManager", "add", "edit", "execTop", "rmv", "add", "execTop"]
[[[[1, 101, 10], [2, 102, 20], [3, 103, 15]]], [4, 104, 5], [102, 8], [], [101], [5, 105, 15], []]

Output:
[null, null, null, 3, null, null, 5]

Explanation
TaskManager taskManager = new TaskManager([[1, 101, 10], [2, 102, 20], [3, 103, 15]]); // Initializes with three tasks for Users 1, 2, and 3.
taskManager.add(4, 104, 5); // Adds task 104 with priority 5 for User 4.
taskManager.edit(102, 8); // Updates priority of task 102 to 8.
taskManager.execTop(); // return 3. Executes task 103 for User 3.
taskManager.rmv(101); // Removes task 101 from the system.
taskManager.add(5, 105, 15); // Adds task 105 with priority 15 for User 5.
taskManager.execTop(); // return 5. Executes task 105 for User 5.

Constraints:
1 <= tasks.length <= 105
0 <= userId <= 105
0 <= taskId <= 105
0 <= priority <= 109
0 <= newPriority <= 109
At most 2 * 105 calls will be made in total to add, edit, rmv, and execTop methods.
The input is generated such that taskId will be valid.
*/

public class TaskManager
{
    PriorityQueue<int, (int priority, int userId)> maxHeap = new PriorityQueue<int, (int, int)>();

    Dictionary<int, (int priority, int userId)> map = new Dictionary<int, (int, int)>();

    public TaskManager(IList<IList<int>> tasks)
    {
        foreach (IList<int> task in tasks)
        {
            Add(task[0], task[1], task[2]);
        }
    }

    public void Add(int userId, int taskId, int priority)
    {
        maxHeap.Enqueue(taskId, (-priority, -taskId));
        map[taskId] = (priority, userId);
    }

    public void Edit(int taskId, int newPriority)
    {
        Add(map[taskId].userId, taskId, newPriority);
        map[taskId] = (newPriority, map[taskId].userId);
    }

    public void Rmv(int taskId)
    {
        map[taskId] = (-1, map[taskId].userId);
    }

    public int ExecTop()
    {
        while (true)
        {
            if (maxHeap.TryDequeue(out int taskId, out var ptuple))
            {
                if (map[taskId].priority == -ptuple.priority)
                {
                    Rmv(taskId);
                    return map[taskId].userId;
                }
            }
            else
            {
                return -1;
            }
        }
    }
}

/**
 * Your TaskManager object will be instantiated and called as such:
 * TaskManager obj = new TaskManager(tasks);
 * obj.Add(userId,taskId,priority);
 * obj.Edit(taskId,newPriority);
 * obj.Rmv(taskId);
 * int param_4 = obj.ExecTop();
 */