#include "InterfaceEnumEditor.h"

//-----------------------------------------------------------------------------
// Function: InterfaceEnumEditor::InterfaceEnumEditor()
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
// Function: InterfaceEnumEditor::addItems()
//-----------------------------------------------------------------------------
void InterfaceEnumEditor::addItems(const QStringList& items, bool isEndpoint, const QString& instanceName) {
    QList<QCheckBox*> checkBoxes;
    int maxCheckBoxWidth = 0;

    for (const QString& item : items) {
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
        QLineEdit* rangeEdit = nullptr;
        QLabel* startLabel = nullptr;
        QLabel* rangeLabel = nullptr;

        if (isEndpoint)
        {
            startLabel = new QLabel("Start:");
            rangeLabel = new QLabel("Range:");
            startEdit = new QLineEdit();
            rangeEdit = new QLineEdit();

            itemLayout->addWidget(startLabel);
            itemLayout->addWidget(startEdit);
            itemLayout->addWidget(rangeLabel);
            itemLayout->addWidget(rangeEdit);

            connect(checkBox, &QCheckBox::toggled, this, [=](bool checked) {
                if (checked) {
                    emit endpointInterfaceChecked(checkBox->text(), instanceName);
                }
                else {
                    emit endpointInterfaceUnchecked(checkBox->text(), instanceName);
                }
                });
        }
        else 
        {
            connect(checkBox, &QCheckBox::toggled, this, [=](bool checked) {
                if (checked) {
                    emit startingPointInterfaceChecked(checkBox->text(), instanceName);
                }
                else {
                    emit startingPointInterfaceUnchecked(checkBox->text(), instanceName);
                }
                });
        }

        interfaceItems_.append({ checkBox, startLabel, startEdit, rangeLabel, rangeEdit });
        scrollLayout_->addLayout(itemLayout);
    }
}

//-----------------------------------------------------------------------------
// Function: InterfaceEnumEditor::clearAll()
//-----------------------------------------------------------------------------
void InterfaceEnumEditor::clearAll() {
    for (const InterfaceItem& item : interfaceItems_) {
        delete item.checkBox;
        delete item.startEdit;
        delete item.rangeEdit;
        delete item.startLabel;
        delete item.rangeLabel;
    }
    interfaceItems_.clear();
}

//-----------------------------------------------------------------------------
// Function: InterfaceEnumEditor::getSelectedStartingPointInterfaces()
//-----------------------------------------------------------------------------
QStringList InterfaceEnumEditor::getSelectedStartingPointInterfaces() const {
    QStringList selectedInitiators;

    for (const InterfaceItem& item : interfaceItems_) {
        if (item.checkBox->isChecked() && !item.startEdit && !item.rangeEdit) {
            selectedInitiators.append(item.checkBox->text());
        }
    }
    return selectedInitiators;
}

//-----------------------------------------------------------------------------
// Function: InterfaceEnumEditor::getSelectedEndpointInterfaces()
//-----------------------------------------------------------------------------
QList<EndpointInterfaceData> InterfaceEnumEditor::getSelectedEndpointInterfaces() const {
    QList<EndpointInterfaceData> selectedTargets;

    for (const InterfaceItem& item : interfaceItems_) {
        if (item.checkBox->isChecked() && item.startEdit && item.rangeEdit) {
            EndpointInterfaceData data;
            data.name = item.checkBox->text();
            data.startValue = item.startEdit->text();
            data.range = item.rangeEdit->text();
            selectedTargets.append(data);
        }
    }
    return selectedTargets;
}

//-----------------------------------------------------------------------------
// Function: InterfaceEnumEditor::setEndpointInterfaceValues()
//-----------------------------------------------------------------------------
void InterfaceEnumEditor::setEndpointInterfaceValues(const QString& interfaceName, quint64 start, quint64 range) {
    auto formatToIpxactHex = [](quint64 value, int minDigits = 4) -> QString {
        return "'h" + QString::number(value, 16).rightJustified(minDigits, '0').toUpper();
        };

    for (InterfaceItem& item : interfaceItems_) {
        if (item.checkBox->text() == interfaceName && item.startEdit && item.rangeEdit) {
            item.startEdit->setText(formatToIpxactHex(start));
            item.rangeEdit->setText(formatToIpxactHex(range));
            break;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: InterfaceEnumEditor::clearEndpointInterfaceValues()
//-----------------------------------------------------------------------------
void InterfaceEnumEditor::clearEndpointInterfaceValues(const QString& interfaceName) {
    for (InterfaceItem& item : interfaceItems_) {
        if (item.checkBox->text() == interfaceName && item.startEdit && item.rangeEdit) {
            item.startEdit->clear();
            item.rangeEdit->clear();
            break;
        }
    }
}
