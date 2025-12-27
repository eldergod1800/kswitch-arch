import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.20 as Kirigami

Kirigami.ApplicationWindow {
    id: root

    title: i18n("KTodo")
    width: 800
    height: 600
    minimumWidth: 400
    minimumHeight: 300

    pageStack.initialPage: Kirigami.Page {
        id: mainPage
        title: i18n("Tasks")

        actions {
            main: Kirigami.Action {
                text: i18n("Add Task")
                icon.name: "list-add"
                onTriggered: {
                    taskEditor.editingIndex = null
                    taskEditor.parentIndex = null
                    taskEditor.taskData = {}
                    taskEditor.open()
                }
            }
        }

        header: ColumnLayout {
            width: parent.width
            spacing: 0

            Controls.ToolBar {
                Layout.fillWidth: true

                RowLayout {
                    anchors.fill: parent

                    Kirigami.SearchField {
                        id: searchField
                        Layout.fillWidth: true
                        placeholderText: i18n("Search tasks...")
                        onTextChanged: taskManager.filteredModel.searchText = text
                    }
                }
            }

            Kirigami.Separator {
                Layout.fillWidth: true
            }
        }

        TaskListView {
            id: taskListView
            anchors.fill: parent
        }
    }

    TaskEditor {
        id: taskEditor
    }

    Component.onCompleted: {
        taskManager.load()
    }
}
