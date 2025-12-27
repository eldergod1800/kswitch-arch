import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.20 as Kirigami

Controls.ScrollView {
    id: root

    Controls.ScrollBar.horizontal.policy: Controls.ScrollBar.AlwaysOff

    ListView {
        id: listView
        model: taskManager.filteredModel

        delegate: TaskDelegate {
            width: listView.width
        }

        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            width: parent.width - (Kirigami.Units.largeSpacing * 4)
            visible: listView.count === 0
            text: searchField.text.length > 0 ? i18n("No tasks found") : i18n("No tasks")
            explanation: searchField.text.length > 0 ?
                i18n("Try a different search term") :
                i18n("Add a task to get started")
            icon.name: "task"
        }
    }
}
