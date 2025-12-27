# KTodo - KDE Task Manager

A feature-rich KDE task manager with hierarchical subtask support built with QML and Kirigami.

## Features

- **Hierarchical Subtasks**: Create unlimited levels of subtasks within tasks
- **Due Dates**: Set due dates for tasks with visual indicators for overdue items
- **Priorities**: Mark tasks with Low, Medium, or High priority
- **Categories**: Organize tasks into categories
- **Tags**: Add multiple tags to tasks for better organization
- **Search**: Quickly find tasks by searching across titles, descriptions, categories, and tags
- **Auto-save**: Tasks are automatically saved to JSON format
- **Modern UI**: Built with Kirigami for a native KDE experience

## Building from Source

### Requirements

- Arch Linux (or Arch-based distro)
- base-devel group installed
- Qt5 and KDE Frameworks 5

### Build and Install

```bash
# Install dependencies
sudo pacman -S qt5-base qt5-quickcontrols2 qt5-declarative kirigami2 kconfig ki18n kcoreaddons cmake extra-cmake-modules

# Build
cd kswitch-arch
makepkg -si -p PKGBUILD.ktodo
```

Or build manually:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr
cmake --build build
sudo cmake --install build
```

## Usage

Launch KTodo from your application menu or run:

```bash
ktodo
```

### Creating Tasks

1. Click the "Add Task" button in the toolbar
2. Fill in the task details:
   - **Title**: Required task name
   - **Description**: Optional detailed description
   - **Due Date**: Optional deadline
   - **Priority**: None, Low, Medium, or High
   - **Category**: Group similar tasks
   - **Tags**: Add multiple comma-separated tags
3. Click "Save"

### Adding Subtasks

1. Hover over a task in the list
2. Click the "Add Subtask" button (+ icon)
3. Fill in the subtask details
4. Subtasks can have their own subtasks, creating a hierarchy

### Organizing Tasks

- **Complete a task**: Check the checkbox next to the task (auto-completes all subtasks)
- **Expand/Collapse**: Click the arrow icon to show/hide subtasks
- **Edit**: Click the edit icon to modify task details
- **Delete**: Click the delete icon to remove a task and all its subtasks
- **Search**: Use the search bar at the top to filter tasks

### Data Storage

Tasks are automatically saved to:
```
~/.local/share/ktodo/tasks.json
```

## Architecture

- **C++ Backend**: Qt-based data model with JSON persistence
- **QML Frontend**: Kirigami-based responsive UI
- **Data Model**: Tree structure supporting recursive subtasks
- **Search**: Real-time filtering with recursive search through subtasks

## License

GPL-3.0

## Contributing

Contributions are welcome! Please feel free to submit issues or pull requests.
