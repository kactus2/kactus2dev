#include "InterfaceEnumEditor.h"

#include <KactusAPI/include/ParameterFinder.h>
#include <editors/ComponentEditor/common/ExpressionEditor.h>
#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>

//-----------------------------------------------------------------------------
// Function: InterfaceEnumEditor::InterfaceEnumEditor()
//-----------------------------------------------------------------------------
InterfaceEnumEditor::InterfaceEnumEditor(ExpressionSet expressionSet, ComponentParameterModel* parameterCompleterModel, Document::Revision docRevision, QWidget* parent)
    : QFrame(parent),
    mainLayout_(new QVBoxLayout(this)),
    itemLayout_(new QGridLayout()),
    optionalStretchLayout_(new QHBoxLayout()),
    parameterFinder_(expressionSet.finder),
    parameterCompleterModel_(parameterCompleterModel),
    expressionFormatter_(expressionSet.formatter)
{
    setFrameStyle(QFrame::StyledPanel);
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_NoMousePropagation);
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

    // Container widget for all interfaces
    QWidget* itemContainer = new QWidget();
    //itemContainer->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    itemContainer->setLayout(itemLayout_);

    optionalStretchLayout_->addWidget(itemContainer);
    mainLayout_->addLayout(optionalStretchLayout_);

    setLayout(mainLayout_);
    isStd22_ = docRevision == Document::Revision::Std22;
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
        checkBox->setToolTip(item.interfaceName);
        checkBox->adjustSize();

        // Cut off interface name (elide the text) if too long and readjust checkbox
        // TODO do this after every interface has been added
        auto maxCheckboxSize = static_cast<int>(0.5 * parentWidget()->width());
        auto possiblyElidedText = checkBox->fontMetrics().elidedText(item.interfaceName, Qt::TextElideMode::ElideRight, maxCheckboxSize);
        checkBox->setText(possiblyElidedText);
        checkBox->adjustSize();

        itemLayout_->addWidget(checkBox, rowCounter, 0);

        QLabel* startLabel = nullptr;
        QLabel* rangeLabel = nullptr;
        ExpressionEditor* startEdit = nullptr;
        ExpressionEditor* rangeEdit = nullptr;

        // Add remapping editors if item is target-adjacent.
        // Only show start and range editors if channel is selected (configuring start for bridge would require 
        // creating address spaces etc.).
        if (item.isTargetAdjacent && isChannel)
        {
            needInterfacesStretch = false;

            startLabel = new QLabel("Start:");
            startEdit = new ExpressionEditor(parameterFinder_);
            connect(startEdit, SIGNAL(editingFinished()), this, SLOT(onExpressionValueEdited()), Qt::UniqueConnection);

            auto startCompleter = new QCompleter(parent());
            startCompleter->setModel(parameterCompleterModel_);
            startEdit->setAppendingCompleter(startCompleter);

            itemLayout_->addWidget(startLabel, rowCounter, 1);
            itemLayout_->addWidget(startEdit, rowCounter, 2);

            rangeLabel = new QLabel("Range:");
            rangeEdit = new ExpressionEditor(parameterFinder_);
            connect(rangeEdit, SIGNAL(editingFinished()), this, SLOT(onExpressionValueEdited()), Qt::UniqueConnection);

            auto rangeCompleter = new QCompleter(parent());
            rangeCompleter->setModel(parameterCompleterModel_);
            rangeEdit->setAppendingCompleter(rangeCompleter);

            // Set minimum dimensions of start and range editors so that there is no unnecessary scrolling needed
            startLabel->setFixedWidth(startLabel->sizeHint().width());
            startLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);

            startEdit->setFixedHeight(ExpressionEditor::DEFAULT_HEIGHT);
            startEdit->setMinimumWidth(0);
            startEdit->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);

            rangeLabel->setFixedWidth(rangeLabel->sizeHint().width());
            rangeLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);

            rangeEdit->setFixedHeight(ExpressionEditor::DEFAULT_HEIGHT);
            rangeEdit->setMinimumWidth(0);
            rangeEdit->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);

            itemLayout_->addWidget(rangeLabel, rowCounter, 3);
            itemLayout_->addWidget(rangeEdit, rowCounter, 4);   
        }

        singleInterfaces_.emplace(item.interfaceName, InterfaceItem{ checkBox, startLabel, startEdit, rangeLabel, rangeEdit });
        rowCounter++;
    }
}

//-----------------------------------------------------------------------------
// Function: InterfaceEnumEditor::addStretchIfNeeded()
//-----------------------------------------------------------------------------
void InterfaceEnumEditor::addStretchIfNeeded()
{
    if (needInterfacesStretch)
    {
        optionalStretchLayout_->addStretch(1);
    }
}

//-----------------------------------------------------------------------------
// Function: InterfaceEnumEditor::clearAll()
//-----------------------------------------------------------------------------
void InterfaceEnumEditor::clearAll() {

    QLayoutItem* currentChild;
    while ((currentChild = itemLayout_->takeAt(0)) != nullptr)
    {
        delete currentChild->widget();
        delete currentChild;
    }

    singleInterfaces_.clear();
    needInterfacesStretch = false;
}

//-----------------------------------------------------------------------------
// Function: InterfaceEnumEditor::onExpressionValueEdited()
//-----------------------------------------------------------------------------
void InterfaceEnumEditor::onExpressionValueEdited()
{
    if (auto senderEditor = qobject_cast<ExpressionEditor*>(sender()))
    {
        senderEditor->finishEditingCurrentWord();
    }
}

//-----------------------------------------------------------------------------
// Function: InterfaceEnumEditor::getSelectedInterfaces()
//-----------------------------------------------------------------------------
QList<InterfaceEnumEditor::InterfaceData> InterfaceEnumEditor::getSelectedInterfaces() const
{
    QList<InterfaceEnumEditor::InterfaceData> selectedInterfaces;

    for (auto const& singleInterfaceName : singleInterfaces_.keys())
    {
        auto interfaceEditor = singleInterfaces_.value(singleInterfaceName);
        if (interfaceEditor.checkBox->isChecked())
        {
            auto selectedInterfaceData = InterfaceData{
                singleInterfaceName
            };

            if (interfaceEditor.startEdit)
            {
                selectedInterfaceData.startValue = interfaceEditor.startEdit->getExpression();
            }

            if (interfaceEditor.rangeEdit)
            {
                selectedInterfaceData.range = interfaceEditor.rangeEdit->getExpression();
            }

            selectedInterfaces.push_back(selectedInterfaceData);
        }
    }

    return selectedInterfaces;
}
