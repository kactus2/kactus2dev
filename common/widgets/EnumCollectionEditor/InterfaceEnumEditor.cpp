#include "InterfaceEnumEditor.h"

//-----------------------------------------------------------------------------
// Function: InterfaceEnumEditor::InterfaceEnumEditor()
//-----------------------------------------------------------------------------
InterfaceEnumEditor::InterfaceEnumEditor(Document::Revision docRevision, QWidget* parent)
    : QFrame(parent),
    mainLayout_(new QVBoxLayout(this)),
    scrollArea_(new QScrollArea(this)),
    scrollContainer_(new QWidget()),
    scrollLayout_(new QVBoxLayout(scrollContainer_)),
    itemLayout_(new QGridLayout())
{
    setFrameStyle(QFrame::StyledPanel);
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_NoMousePropagation);
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

    setLayout(mainLayout_);
    scrollLayout_->addLayout(itemLayout_);

    scrollArea_->setWidgetResizable(true);
    scrollArea_->setFrameShape(QFrame::NoFrame);

    scrollContainer_->setLayout(scrollLayout_);
    scrollArea_->setWidget(scrollContainer_);

    mainLayout_->addWidget(scrollArea_);

    isStd22_ = docRevision == Document::Revision::Std22;
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
// Function: InterfaceEnumEditor::addItems()
//-----------------------------------------------------------------------------
void InterfaceEnumEditor::addItems(const QList<InterfaceInput>& items, General::InterfaceMode interfaceMode, bool isChannel)
{
    if (items.isEmpty())
    {
        return;
    }

    QLabel* typeLabel = new QLabel();

    bool isTarget = interfaceMode == General::InterfaceMode::TARGET || interfaceMode == General::InterfaceMode::SLAVE;
    bool isMirroredTarget = interfaceMode == General::InterfaceMode::MIRRORED_TARGET || interfaceMode == General::InterfaceMode::MIRRORED_SLAVE;
    bool isInitiator = interfaceMode == General::InterfaceMode::INITIATOR || interfaceMode == General::InterfaceMode::MASTER;
    bool isMirroredInitiator = interfaceMode == General::InterfaceMode::MIRRORED_INITIATOR || interfaceMode == General::InterfaceMode::MIRRORED_MASTER;

    if (isInitiator)
    {
        typeLabel->setText(isStd22_ ? "Initiators:" : "Masters:");
    }
    else if (isMirroredInitiator)
    {
        typeLabel->setText(isStd22_ ? "Mirrored initiators:" : "Mirrored masters:");
    }
    else if (isTarget)
    {
        typeLabel->setText(isStd22_ ? "Targets:" : "Slaves:");
    }
    else if (isMirroredTarget)
    {
        typeLabel->setText(isStd22_ ? "Mirrored targets:" : "Mirrored slaves:");
    }

    itemLayout_->addWidget(typeLabel, itemLayout_->rowCount(), 0);
    typeLabel->setMaximumSize(typeLabel->sizeHint());

    int rowCounter = itemLayout_->rowCount();

    for (auto const& item : items)
    {
        auto checkBox = new QCheckBox(item.interfaceName);
        checkBox->adjustSize();
        
        itemLayout_->addWidget(checkBox, rowCounter, 0);

        QLabel* startLabel = nullptr;
        QLabel* rangeLabel = nullptr;
        QLineEdit* startEdit = nullptr;
        QLineEdit* rangeEdit = nullptr;

        // Add remapping editors if item is target-adjacent.
        // Only show start and range editors if channel is selected (configuring start for bridge would require 
        // creating address spaces etc.).
        if (item.isTargetAdjacent && isChannel)
        {
            startLabel = new QLabel("Start:");
            startEdit = new QLineEdit();
            itemLayout_->addWidget(startLabel, rowCounter, 1);
            itemLayout_->addWidget(startEdit, rowCounter, 2);

            rangeLabel = new QLabel("Range:");
            rangeEdit = new QLineEdit();
            itemLayout_->addWidget(rangeLabel, rowCounter, 3);
            itemLayout_->addWidget(rangeEdit, rowCounter, 4);   
        }

        singleInterfaces_.emplace(item.interfaceName, InterfaceItem{ checkBox, startLabel, startEdit, rangeLabel, rangeEdit });
        rowCounter++;
    }
}

//-----------------------------------------------------------------------------
// Function: InterfaceEnumEditor::clearAll()
//-----------------------------------------------------------------------------
void InterfaceEnumEditor::clearAll() {
    // TODO remove this old stuff
    for (auto const& item : interfaceItems_) {
        delete item.checkBox;
        delete item.startEdit;
        delete item.rangeEdit;
        delete item.startLabel;
        delete item.rangeLabel;
    }

    interfaceItems_.clear();

    QLayoutItem* currentChild;
    while ((currentChild = itemLayout_->takeAt(0)) != nullptr)
    {
        delete currentChild->widget();
        delete currentChild;
    }

    itemLayoutCondenser_ = nullptr;
    singleInterfaces_.clear();
}

//-----------------------------------------------------------------------------
// Function: InterfaceEnumEditor::createLayoutCondenser()
//-----------------------------------------------------------------------------
void InterfaceEnumEditor::createLayoutCondenser()
{
    itemLayoutCondenser_ = new QWidget();
    itemLayoutCondenser_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    itemLayout_->addWidget(itemLayoutCondenser_, itemLayout_->rowCount(), 0);
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
// Function: InterfaceEnumEditor::getSelectedInterfaces()
//-----------------------------------------------------------------------------
QList<InterfaceEnumEditor::InterfaceData> InterfaceEnumEditor::getSelectedInterfaces() const
{
    QList<InterfaceEnumEditor::InterfaceData> selectedInterfaces;

    for (auto const& interfaceEditor : singleInterfaces_)
    {
        if (interfaceEditor.checkBox->isChecked())
        {
            auto selectedInterfaceData = InterfaceData{
                interfaceEditor.checkBox->text()
            };

            if (interfaceEditor.startEdit)
            {
                selectedInterfaceData.startValue = interfaceEditor.startEdit->text();
            }

            if (interfaceEditor.rangeEdit)
            {
                selectedInterfaceData.range = interfaceEditor.rangeEdit->text();
            }

            selectedInterfaces.push_back(selectedInterfaceData);
        }
    }

    return selectedInterfaces;
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
