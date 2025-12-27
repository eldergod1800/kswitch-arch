import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.20 as Kirigami

Kirigami.Dialog {
    id: dialog

    property var editingIndex: null
    property var parentIndex: null
    property var taskData: ({})

    title: editingIndex ? i18n("Edit Task") : i18n("New Task")
    standardButtons: Controls.Dialog.Save | Controls.Dialog.Cancel

    onAccepted: {
        var data = {
            "title": titleField.text,
            "description": descriptionField.text,
            "dueDate": dueDatePicker.selectedDate,
            "priority": priorityCombo.currentIndex,
            "category": categoryCombo.editText,
            "tags": tagsField.text.split(',').map(function(t) { return t.trim() }).filter(function(t) { return t.length > 0 })
        }

        if (editingIndex) {
            // Update existing task
            taskManager.taskModel.updateTask(editingIndex, data)
        } else {
            // Add new task
            taskManager.taskModel.addTask(data.title, parentIndex || undefined)
            var newIndex = taskManager.taskModel.index(
                taskManager.taskModel.rowCount(parentIndex || undefined) - 1,
                0,
                parentIndex || undefined
            )
            // Update with full data
            delete data.title // Don't set title twice
            taskManager.taskModel.updateTask(newIndex, data)
        }

        // Add new category/tags if they don't exist
        if (data.category) {
            taskManager.addCategory(data.category)
        }
        for (var i = 0; i < data.tags.length; i++) {
            taskManager.addTag(data.tags[i])
        }

        clear()
    }

    onRejected: {
        clear()
    }

    onOpened: {
        if (editingIndex) {
            // Populate fields with existing data
            titleField.text = taskData.title || ""
            descriptionField.text = taskData.description || ""
            dueDatePicker.selectedDate = taskData.dueDate || new Date()
            dueDatePicker.enabled = taskData.dueDate ? true : false
            priorityCombo.currentIndex = taskData.priority || 0
            categoryCombo.editText = taskData.category || ""
            tagsField.text = taskData.tags ? taskData.tags.join(', ') : ""
        }
        titleField.forceActiveFocus()
    }

    function clear() {
        titleField.text = ""
        descriptionField.text = ""
        dueDatePicker.selectedDate = new Date()
        dueDatePicker.enabled = false
        priorityCombo.currentIndex = 0
        categoryCombo.editText = ""
        tagsField.text = ""
    }

    ColumnLayout {
        spacing: Kirigami.Units.largeSpacing

        Kirigami.FormLayout {
            Layout.fillWidth: true

            Controls.TextField {
                id: titleField
                Kirigami.FormData.label: i18n("Title:")
                placeholderText: i18n("Task title")
            }

            Controls.TextArea {
                id: descriptionField
                Kirigami.FormData.label: i18n("Description:")
                placeholderText: i18n("Task description (optional)")
                Layout.fillWidth: true
                Layout.preferredHeight: 100
                wrapMode: TextEdit.Wrap
            }

            RowLayout {
                Kirigami.FormData.label: i18n("Due Date:")

                Controls.CheckBox {
                    id: dueDateCheckbox
                    checked: dueDatePicker.enabled
                    onToggled: dueDatePicker.enabled = checked
                }

                DatePicker {
                    id: dueDatePicker
                    Layout.fillWidth: true
                    enabled: false
                }
            }

            Controls.ComboBox {
                id: priorityCombo
                Kirigami.FormData.label: i18n("Priority:")
                model: [
                    i18n("None"),
                    i18n("Low"),
                    i18n("Medium"),
                    i18n("High")
                ]
            }

            Controls.ComboBox {
                id: categoryCombo
                Kirigami.FormData.label: i18n("Category:")
                editable: true
                model: taskManager.allCategories
                editText: ""
            }

            Controls.TextField {
                id: tagsField
                Kirigami.FormData.label: i18n("Tags:")
                placeholderText: i18n("Comma-separated tags")
                Layout.fillWidth: true

                Controls.ToolTip {
                    visible: tagsField.hovered
                    text: i18n("Separate multiple tags with commas")
                }
            }

            RowLayout {
                Kirigami.FormData.label: i18n("Suggestions:")
                Layout.fillWidth: true
                visible: tagRepeater.count > 0

                Repeater {
                    id: tagRepeater
                    model: taskManager.allTags.slice(0, 5)

                    Kirigami.Chip {
                        text: modelData
                        closable: false
                        checkable: true
                        onToggled: {
                            if (checked) {
                                var tags = tagsField.text.split(',').map(function(t) { return t.trim() }).filter(function(t) { return t.length > 0 })
                                if (tags.indexOf(modelData) === -1) {
                                    tags.push(modelData)
                                    tagsField.text = tags.join(', ')
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
