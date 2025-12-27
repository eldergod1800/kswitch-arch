import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.20 as Kirigami

RowLayout {
    id: root

    property date selectedDate: new Date()
    property alias enabled: dayField.enabled

    spacing: Kirigami.Units.smallSpacing

    Controls.SpinBox {
        id: dayField
        from: 1
        to: 31
        value: selectedDate.getDate()
        onValueModified: updateDate()
    }

    Controls.ComboBox {
        id: monthCombo
        enabled: dayField.enabled
        model: [
            i18n("Jan"), i18n("Feb"), i18n("Mar"), i18n("Apr"),
            i18n("May"), i18n("Jun"), i18n("Jul"), i18n("Aug"),
            i18n("Sep"), i18n("Oct"), i18n("Nov"), i18n("Dec")
        ]
        currentIndex: selectedDate.getMonth()
        onActivated: updateDate()
    }

    Controls.SpinBox {
        id: yearField
        enabled: dayField.enabled
        from: 2000
        to: 2100
        value: selectedDate.getFullYear()
        onValueModified: updateDate()
    }

    function updateDate() {
        var newDate = new Date(yearField.value, monthCombo.currentIndex, dayField.value)
        selectedDate = newDate
    }

    onSelectedDateChanged: {
        if (selectedDate) {
            dayField.value = selectedDate.getDate()
            monthCombo.currentIndex = selectedDate.getMonth()
            yearField.value = selectedDate.getFullYear()
        }
    }
}
