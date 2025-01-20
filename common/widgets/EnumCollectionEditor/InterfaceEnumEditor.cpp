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
    QList<QCheckBox*> checkBoxes;
    int maxCheckBoxWidth = 0;

    foreach(const QString & item, items) {
        QCheckBox* checkBox = new QCheckBox(item);
        checkBox->adjustSize();
        maxCheckBoxWidth = std::max(maxCheckBoxWidth, checkBox->sizeHint().width());
        checkBoxes.append(checkBox);
    }

    for (int i = 0; i < items.size(); ++i) {
        QHBoxLayout* itemLayout = new QHBoxLayout();
        QCheckBox* checkBox = checkBoxes[i];
        checkBox->setMinimumWidth(maxCheckBoxWidth);
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
        interfaceItems_.append({ checkBox, startLabel, startEdit, endLabel, endEdit });
        scrollLayout_->addLayout(itemLayout);
    }
}

void InterfaceEnumEditor::clearAll() {
    for (const InterfaceItem& item : interfaceItems_) {
        delete item.checkBox;
        delete item.startEdit;
        delete item.endEdit;
        delete item.startLabel;
        delete item.endLabel;
    }
    interfaceItems_.clear();
}

QStringList InterfaceEnumEditor::getSelectedInitiatorInterfaces() const {
    QStringList selectedInitiators;

    for (const InterfaceItem& item : interfaceItems_) {
        if (item.checkBox->isChecked() && !item.startEdit && !item.endEdit) {
            selectedInitiators.append(item.checkBox->text());
        }
    }
    return selectedInitiators;
}

QList<TargetInterfaceData> InterfaceEnumEditor::getSelectedTargetInterfaces() const {
    QList<TargetInterfaceData> selectedTargets;

    for (const InterfaceItem& item : interfaceItems_) {
        if (item.checkBox->isChecked() && item.startEdit && item.endEdit) {
            TargetInterfaceData data;
            data.name = item.checkBox->text();
            data.startValue = item.startEdit->text();
            data.endValue = item.endEdit->text();
            selectedTargets.append(data);
        }
    }
    return selectedTargets;
}
