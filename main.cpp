#include <iostream>
#include <iomanip>
#include <fstream>
#include <limits>
#include <sstream>
#include <string>
#include <cctype>
using namespace std;

// Daily Planner / To-Do List System
const int MAX_TASKS = 100;
const string DATA_FILE = "tasks.txt";

// Task structure to hold task details
struct Task {
    int id;
    string title;
    string date;
    string time;
    int priority;
    bool completed;
};

// Function prototypes
void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// Validate date format (YYYY-MM-DD) and basic date values
bool isValidDate(const string &date) {
    if (date.length() != 10 || date[4] != '-' || date[7] != '-') {
        return false;
    }

    for (int i = 0; i < 10; i++) {
        if ((i != 4 && i != 7) && !isdigit(date[i])) {
            return false;
        }
    }

    int month = stoi(date.substr(5, 2));
    int day = stoi(date.substr(8, 2));
    return month >= 1 && month <= 12 && day >= 1 && day <= 31;
}

// Validate time format (HH:MM) and basic time values
bool isValidTime(const string &time) {
    if (time.length() != 5 || time[2] != ':') {
        return false;
    }

    for (int i = 0; i < 5; i++) {
        if (i != 2 && !isdigit(time[i])) {
            return false;
        }
    }

    int hour = stoi(time.substr(0, 2));
    int minute = stoi(time.substr(3, 2));
    return hour >= 0 && hour <= 23 && minute >= 0 && minute <= 59;
}

// Get integer input with validation for a specific range
int getIntInput(const string &prompt, int minValue, int maxValue) {
    int value;

    while (true) {
        cout << prompt;
        cin >> value;

        if (!cin.fail() && value >= minValue && value <= maxValue) {
            clearInput();
            return value;
        }

        cout << "Invalid input. Please enter a number from " << minValue
             << " to " << maxValue << ".\n";
        clearInput();
    }
}

// Get non-empty string input
string getRequiredText(const string &prompt) {
    string text;

    do {
        cout << prompt;
        getline(cin, text);

        if (text.empty()) {
            cout << "Input cannot be empty.\n";
        }
    } while (text.empty());

    return text;
}

// Get and validate date input
string getDateInput() {
    string date;

    do {
        cout << "Enter date (YYYY-MM-DD): ";
        getline(cin, date);

        if (!isValidDate(date)) {
            cout << "Invalid date format. Example: 2026-06-19\n";
        }
    } while (!isValidDate(date));

    return date;
}

// Get and validate time input
string getTimeInput() {
    string time;

    do {
        cout << "Enter time (HH:MM, 24-hour): ";
        getline(cin, time);

        if (!isValidTime(time)) {
            cout << "Invalid time format. Example: 08:30\n";
        }
    } while (!isValidTime(time));

    return time;
}

// Convert priority integer to string representation
string priorityName(int priority) {
    if (priority == 1) {
        return "High";
    }
    if (priority == 2) {
        return "Medium";
    }
    return "Low";
}

// Print table header for task display
void printHeader() {
    cout << left
        << setw(5) << "ID"
        << setw(26) << "Title"
        << setw(13) << "Date"
        << setw(8) << "Time"
        << setw(10) << "Priority"
        << "Status\n";
    cout << string(70, '-') << "\n";
}

// Print task details in a formatted manner
void printTask(const Task& task) {
    string shortTitle = task.title;

    if (shortTitle.length() > 23) {
        shortTitle = shortTitle.substr(0, 20) + "...";
    }

    cout << left
        << setw(5) << task.id
        << setw(26) << shortTitle
        << setw(13) << task.date
        << setw(8) << task.time
        << setw(10) << priorityName(task.priority)
        << (task.completed ? "Completed" : "Pending") << "\n";
}

// Find the index of a task by its ID, return -1 if not found
int findTaskIndexById(Task tasks[], int taskCount, int id) {
    for (int i = 0; i < taskCount; i++) {
        if (tasks[i].id == id) {
            return i;
        }
    }

    return -1;
}

// Get the next available task ID by finding the highest existing ID and adding 1
int getNextId(Task tasks[], int taskCount) {
    int highestId = 0;

    for (int i = 0; i < taskCount; i++) {
        if (tasks[i].id > highestId) {
            highestId = tasks[i].id;
        }
    }

    return highestId + 1;
}

// Add a new task to the task list with user input and validation
void addTask(Task tasks[], int& taskCount) {
    if (taskCount >= MAX_TASKS) {
        cout << "Task list is full. Please delete a task first.\n";
        return;
    }

    cout << "\n--- Add New Task ---\n";
    tasks[taskCount].id = getNextId(tasks, taskCount);
    tasks[taskCount].title = getRequiredText("Enter task title: ");
    tasks[taskCount].date = getDateInput();
    tasks[taskCount].time = getTimeInput();
    tasks[taskCount].priority = getIntInput("Priority (1=High, 2=Medium, 3=Low): ", 1, 3);
    tasks[taskCount].completed = false;
    taskCount++;

    cout << "Task added successfully.\n";
}

// Display all tasks in a formatted table, or show a message if no tasks are available
void viewAllTasks(Task tasks[], int taskCount) {
    cout << "\n--- All Tasks ---\n";

    if (taskCount == 0) {
        cout << "No tasks available.\n";
        return;
    }

    printHeader();
    for (int i = 0; i < taskCount; i++) {
        printTask(tasks[i]);
    }
}

// Display tasks that match a specific date entered by the user, or show a message if no tasks are found for that date
void viewTasksByDate(Task tasks[], int taskCount) {
    string date = getDateInput();
    bool found = false;

    cout << "\n--- Tasks on " << date << " ---\n";
    printHeader();

    for (int i = 0; i < taskCount; i++) {
        if (tasks[i].date == date) {
            printTask(tasks[i]);
            found = true;
        }
    }

    if (!found) {
        cout << "No tasks found for this date.\n";
    }
}

// Search for tasks that contain a keyword in their title and display the matching tasks, or show a message if no matches are found
void searchTask(Task tasks[], int taskCount) {
    string keyword = getRequiredText("Enter keyword to search: ");
    bool found = false;

    cout << "\n--- Search Results ---\n";
    printHeader();

    for (int i = 0; i < taskCount; i++) {
        if (tasks[i].title.find(keyword) != string::npos) {
            printTask(tasks[i]);
            found = true;
        }
    }

    if (!found) {
        cout << "No matching tasks found.\n";
    }
}

// Update an existing task by its ID with new details entered by the user, or show a message if the task ID is not found
void updateTask(Task tasks[], int taskCount) {
    int id = getIntInput("Enter task ID to update: ", 1, 9999);
    int index = findTaskIndexById(tasks, taskCount, id);

    if (index == -1) {
        cout << "Task ID not found.\n";
        return;
    }

    cout << "\n--- Update Task ---\n";
    tasks[index].title = getRequiredText("Enter new task title: ");
    tasks[index].date = getDateInput();
    tasks[index].time = getTimeInput();
    tasks[index].priority = getIntInput("Priority (1=High, 2=Medium, 3=Low): ", 1, 3);

    cout << "Task updated successfully.\n";
}

// Mark a task as completed by its ID, or show a message if the task ID is not found
void markTaskComplete(Task tasks[], int taskCount) {
    int id = getIntInput("Enter task ID to mark complete: ", 1, 9999);
    int index = findTaskIndexById(tasks, taskCount, id);

    if (index == -1) {
        cout << "Task ID not found.\n";
        return;
    }

    tasks[index].completed = true;
    cout << "Task marked as completed.\n";
}

// Delete a task by its ID and shift remaining tasks up to fill the gap, or show a message if the task ID is not found
void deleteTask(Task tasks[], int& taskCount) {
    int id = getIntInput("Enter task ID to delete: ", 1, 9999);
    int index = findTaskIndexById(tasks, taskCount, id);

    if (index == -1) {
        cout << "Task ID not found.\n";
        return;
    }
     
    for (int i = index; i < taskCount - 1; i++) {
        tasks[i] = tasks[i + 1];
    }

    taskCount--;
    cout << "Task deleted successfully.\n";
}

// Show a summary of the planner including total tasks, completed tasks, pending tasks, and high-priority pending tasks
void showSummary(Task tasks[], int taskCount) {
    int completed = 0;
    int highPriorityPending = 0;

    for (int i = 0; i < taskCount; i++) {
        if (tasks[i].completed) {
            completed++;
        }
        else if (tasks[i].priority == 1) {
            highPriorityPending++;
        }
    }

    cout << "\n--- Planner Summary ---\n";
    cout << "Total tasks: " << taskCount << "\n";
    cout << "Completed tasks: " << completed << "\n";
    cout << "Pending tasks: " << taskCount - completed << "\n";
    cout << "High-priority pending tasks: " << highPriorityPending << "\n";
}

// Save tasks to a file in a simple delimited format, or show a message if the file cannot be opened
void saveTasks(Task tasks[], int taskCount) {
    ofstream file(DATA_FILE);

    if (!file) {
        cout << "Unable to save tasks.\n";
        return;
    }

    for (int i = 0; i < taskCount; i++) {
        file << tasks[i].id << "|"
            << tasks[i].title << "|"
            << tasks[i].date << "|"
            << tasks[i].time << "|"
            << tasks[i].priority << "|"
            << tasks[i].completed << "\n";
    }
}

// Load tasks from a file, parsing the delimited format and populating the task array, or show a message if the file cannot be opened
void loadTasks(Task tasks[], int& taskCount) {
    ifstream file(DATA_FILE);
    string line;

    taskCount = 0;

    while (getline(file, line) && taskCount < MAX_TASKS) {
        stringstream ss(line);
        string idText, priorityText, completedText;

        getline(ss, idText, '|');
        getline(ss, tasks[taskCount].title, '|');
        getline(ss, tasks[taskCount].date, '|');
        getline(ss, tasks[taskCount].time, '|');
        getline(ss, priorityText, '|');
        getline(ss, completedText, '|');

        if (!idText.empty() && !priorityText.empty() && !completedText.empty()) {
            tasks[taskCount].id = stoi(idText);
            tasks[taskCount].priority = stoi(priorityText);
            tasks[taskCount].completed = stoi(completedText);
            taskCount++;
        }
    }
}

// Display the main menu with options for the user to choose from
void showMenu() {
    cout << "\n====================================\n";
    cout << " Daily Planner / To-Do List System\n";
    cout << "====================================\n";
    cout << "\n1. Add new task\n";
    cout << "2. View all tasks\n";
    cout << "3. View tasks by date\n";
    cout << "4. Search task\n";
    cout << "5. Update task\n";
    cout << "6. Mark task as completed\n";
    cout << "7. Delete task\n";
    cout << "8. Show planner summary\n";
    cout << "9. Save and exit\n";
}

// Main function to run the daily planner system, handling user input and calling appropriate functions based on the user's choice
int main() {
    Task tasks[MAX_TASKS];
    int taskCount = 0;
    int choice;

    loadTasks(tasks, taskCount);

    do {
        showMenu();
        choice = getIntInput("\nEnter your choice: ", 1, 9);

        switch (choice) {
        case 1:
            addTask(tasks, taskCount);
            break;
        case 2:
            viewAllTasks(tasks, taskCount);
            break;
        case 3:
            viewTasksByDate(tasks, taskCount);
            break;
        case 4:
            searchTask(tasks, taskCount);
            break;
        case 5:
            updateTask(tasks, taskCount);
            break;
        case 6:
            markTaskComplete(tasks, taskCount);
            break;
        case 7:
            deleteTask(tasks, taskCount);
            break;
        case 8:
            showSummary(tasks, taskCount);
            break;
        case 9:
            saveTasks(tasks, taskCount);
            cout << "Tasks saved. Thank you for using the planner.\n";
            break;
        }
    } while (choice != 9);

    return 0;
}
