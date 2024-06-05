#include "InterfaceEnumEditor.h"

InterfaceEnumEditor::InterfaceEnumEditor(QWidget* parent) : QFrame(parent) {
    setFrameStyle(QFrame::StyledPanel);
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_NoMousePropagation);

    mainLayout_ = new QVBoxLayout(this);
    setLayout(mainLayout_);

    scrollArea_ = new QScrollArea(this);
    scrollArea_->setWidgetResizable(true);
    scrollArea_->setFrameShape(QFrame::NoFrame);
    scrollContainer_ = new QWidget();
    scrollArea_->setWidget(scrollContainer_);
    scrollLayout_ = new QVBoxLayout(scrollContainer_);
    scrollContainer_->setLayout(scrollLayout_);

    mainLayout_->addWidget(scrollArea_);
}

void InterfaceEnumEditor::addItems(const QStringList& items, bool isTarget) {
    foreach(const QString & item, items) {
        QHBoxLayout* itemLayout = new QHBoxLayout();
        QCheckBox* checkBox = new QCheckBox(item);
        itemLayout->addWidget(checkBox);

        QLineEdit* startEdit = nullptr;
        QLineEdit* endEdit = nullptr;

        QLabel* startLabel = nullptr;
        QLabel* endLabel = nullptr;

        if (isTarget) {
            startEdit = new QLineEdit();
            endEdit = new QLineEdit();

            startLabel = new QLabel("Start:");
            endLabel = new QLabel("End:");

            itemLayout->addWidget(startLabel);
            itemLayout->addWidget(startEdit);
            itemLayout->addWidget(endLabel);
            itemLayout->addWidget(endEdit);
        }

        // Store the item (initiator or target) in the list
        interfaceItems_.append({ checkBox, startLabel, startEdit, endLabel, endEdit });

        // Add layout to the scroll layout
        scrollLayout_->addLayout(itemLayout);
    }
}

// Method to clear all items
void InterfaceEnumEditor::clearAll() {
    for (const InterfaceItem& item : interfaceItems_) {
        delete item.checkBox;
        delete item.startEdit;
        delete item.endEdit;
        delete item.startLabel;
        delete item.endLabel;
    }
    interfaceItems_.clear(); // Clear the list now that all items are deleted
}

// Method to get selected initiator interfaces (no start/end values)
QStringList InterfaceEnumEditor::getSelectedInitiatorInterfaces() const {
    QStringList selectedInitiators;

    for (const InterfaceItem& item : interfaceItems_) {
        if (item.checkBox->isChecked() && !item.startEdit && !item.endEdit) { // No line edits imply initiator
            selectedInitiators.append(item.checkBox->text());
        }
    }
    return selectedInitiators;
}

// Method to get selected target interfaces with start/end values
QList<TargetInterfaceData> InterfaceEnumEditor::getSelectedTargetInterfaces() const {
    QList<TargetInterfaceData> selectedTargets;

    for (const InterfaceItem& item : interfaceItems_) {
        if (item.checkBox->isChecked() && item.startEdit && item.endEdit) { // Line edits imply target
            TargetInterfaceData data;
            data.name = item.checkBox->text();
            data.startValue = item.startEdit->text();
            data.endValue = item.endEdit->text();
            selectedTargets.append(data);
        }
    }
    return selectedTargets;
}
