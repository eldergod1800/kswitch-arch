import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.20 as Kirigami

Kirigami.AbstractListItem {
    id: delegate

    property bool isExpanded: false
    property int level: 0

    contentItem: ColumnLayout {
        spacing: 0

        // Main task item
        RowLayout {
            Layout.fillWidth: true
            Layout.leftMargin: level * Kirigami.Units.largeSpacing * 2
            spacing: Kirigami.Units.smallSpacing

            // Expand/collapse button
            Controls.ToolButton {
                visible: model.hasChildren
                icon.name: isExpanded ? "arrow-down" : "arrow-right"
                onClicked: isExpanded = !isExpanded
                Layout.preferredWidth: Kirigami.Units.iconSizes.small
                Layout.preferredHeight: Kirigami.Units.iconSizes.small
            }

            // Spacer for items without children
            Item {
                visible: !model.hasChildren
                Layout.preferredWidth: Kirigami.Units.iconSizes.small
            }

            // Checkbox
            Controls.CheckBox {
                id: checkbox
                checked: model.completed
                onToggled: {
                    taskManager.taskModel.setData(
                        taskManager.filteredModel.mapToSource(
                            taskManager.filteredModel.index(index, 0)
                        ),
                        checked,
                        257 // CompletedRole
                    )
                }
            }

            // Task content
            ColumnLayout {
                Layout.fillWidth: true
                spacing: Kirigami.Units.smallSpacing

                RowLayout {
                    Layout.fillWidth: true

                    // Title
                    Controls.Label {
                        text: model.title
                        font.strikeout: model.completed
                        Layout.fillWidth: true
                        wrapMode: Text.Wrap
                    }

                    // Priority indicator
                    Rectangle {
                        visible: model.priority > 0
                        width: Kirigami.Units.iconSizes.small
                        height: Kirigami.Units.iconSizes.small
                        radius: width / 2
                        color: {
                            switch(model.priority) {
                                case 1: return Kirigami.Theme.positiveTextColor
                                case 2: return Kirigami.Theme.neutralTextColor
                                case 3: return Kirigami.Theme.negativeTextColor
                                default: return "transparent"
                            }
                        }
                    }
                }

                // Metadata row
                RowLayout {
                    Layout.fillWidth: true
                    spacing: Kirigami.Units.largeSpacing
                    visible: model.dueDate.toString() !== "" || model.category !== "" || model.tags.length > 0

                    // Due date
                    RowLayout {
                        visible: model.dueDate.toString() !== ""
                        spacing: Kirigami.Units.smallSpacing

                        Kirigami.Icon {
                            source: "view-calendar"
                            width: Kirigami.Units.iconSizes.small
                            height: width
                        }

                        Controls.Label {
                            text: Qt.formatDateTime(model.dueDate, "MMM dd, yyyy")
                            font.pointSize: Kirigami.Theme.smallFont.pointSize
                            color: {
                                if (model.completed) return Kirigami.Theme.disabledTextColor
                                var now = new Date()
                                var due = new Date(model.dueDate)
                                if (due < now) return Kirigami.Theme.negativeTextColor
                                return Kirigami.Theme.textColor
                            }
                        }
                    }

                    // Category
                    Kirigami.Chip {
                        visible: model.category !== ""
                        text: model.category
                        closable: false
                    }

                    // Tags
                    Repeater {
                        model: tags

                        Kirigami.Chip {
                            text: modelData
                            closable: false
                        }
                    }

                    // Subtask count
                    Controls.Label {
                        visible: model.hasChildren
                        text: i18n("%1 subtasks", model.childCount)
                        font.pointSize: Kirigami.Theme.smallFont.pointSize
                        color: Kirigami.Theme.disabledTextColor
                    }
                }
            }

            // Action buttons
            Controls.ToolButton {
                icon.name: "list-add"
                text: i18n("Add Subtask")
                display: Controls.AbstractButton.IconOnly
                onClicked: {
                    taskEditor.editingIndex = null
                    taskEditor.parentIndex = taskManager.filteredModel.mapToSource(
                        taskManager.filteredModel.index(index, 0)
                    )
                    taskEditor.taskData = {}
                    taskEditor.open()
                }

                Controls.ToolTip.visible: hovered
                Controls.ToolTip.text: text
                Controls.ToolTip.delay: Kirigami.Units.toolTipDelay
            }

            Controls.ToolButton {
                icon.name: "document-edit"
                text: i18n("Edit")
                display: Controls.AbstractButton.IconOnly
                onClicked: {
                    var sourceIndex = taskManager.filteredModel.mapToSource(
                        taskManager.filteredModel.index(index, 0)
                    )
                    taskEditor.editingIndex = sourceIndex
                    taskEditor.parentIndex = null
                    taskEditor.taskData = taskManager.taskModel.getTaskData(sourceIndex)
                    taskEditor.open()
                }

                Controls.ToolTip.visible: hovered
                Controls.ToolTip.text: text
                Controls.ToolTip.delay: Kirigami.Units.toolTipDelay
            }

            Controls.ToolButton {
                icon.name: "edit-delete"
                text: i18n("Delete")
                display: Controls.AbstractButton.IconOnly
                onClicked: {
                    deleteDialog.taskIndex = taskManager.filteredModel.mapToSource(
                        taskManager.filteredModel.index(index, 0)
                    )
                    deleteDialog.open()
                }

                Controls.ToolTip.visible: hovered
                Controls.ToolTip.text: text
                Controls.ToolTip.delay: Kirigami.Units.toolTipDelay
            }
        }

        // Subtasks (recursive)
        Repeater {
            model: isExpanded && hasChildren ? taskManager.filteredModel.rowCount(
                taskManager.filteredModel.index(index, 0)
            ) : 0

            Loader {
                Layout.fillWidth: true
                sourceComponent: TaskDelegate {
                    level: delegate.level + 1
                    width: parent.width
                }

                onLoaded: {
                    item.model = taskManager.filteredModel
                    item.index = model.index
                }
            }
        }
    }

    // Delete confirmation dialog
    Kirigami.PromptDialog {
        id: deleteDialog
        property var taskIndex

        title: i18n("Delete Task")
        subtitle: i18n("Are you sure you want to delete this task and all its subtasks?")

        standardButtons: Controls.Dialog.Ok | Controls.Dialog.Cancel

        onAccepted: {
            taskManager.taskModel.removeTask(taskIndex)
        }
    }
}
