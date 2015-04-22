//-----------------------------------------------------------------------------
// File: SingleMemoryMapEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 15.04.2015
//
// Description:
// Editor for editing the details of a single memory map.
//-----------------------------------------------------------------------------

#include "SingleMemoryMapEditor.h"

#include <QScrollArea>
#include <QLabel>

#include <IPXACTmodels/remapstate.h>
#include <IPXACTmodels/MemoryRemap.h>

#include <QFormLayout>

//-----------------------------------------------------------------------------
// Function: SingleMemoryMapEditor::SingleMemoryMapEditor()
//-----------------------------------------------------------------------------
SingleMemoryMapEditor::SingleMemoryMapEditor(QSharedPointer<Component> component,
    QSharedPointer<AbstractMemoryMap> memoryRemap, QSharedPointer<MemoryMap> parentMemoryMap,
    LibraryInterface* libHandler, QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionFormatter> expressionFormatter, QWidget* parent /* = 0 */):
ItemEditor(component, libHandler, parent),
nameEditor_(memoryRemap->getNameGroup(), this, tr("Memory map name and description")),
memoryMapEditor_(new MemoryMapEditor(component, libHandler, memoryRemap, parameterFinder, expressionFormatter, this)),
addressUnitBitsEditor_(new QLineEdit(parent)),
slaveInterfaceLabel_(new QLabel(this)),
remapStateSelector_(),
memoryRemap_(memoryRemap),
parentMemoryMap_(parentMemoryMap)
{
    addressUnitBitsEditor_->setValidator(
        new QRegularExpressionValidator(QRegularExpression("\\d*"), addressUnitBitsEditor_));

    if (!isMemoryMap())
    {
        addressUnitBitsEditor_->setEnabled(false);
    }

    connect(memoryMapEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(memoryMapEditor_, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(memoryMapEditor_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);
    connect(memoryMapEditor_, SIGNAL(childAdded(int)), this, SIGNAL(childAdded(int)), Qt::UniqueConnection);
    connect(memoryMapEditor_, SIGNAL(childRemoved(int)), this, SIGNAL(childRemoved(int)), Qt::UniqueConnection);

    connect(&nameEditor_, SIGNAL(contentChanged()), this, SLOT(refreshSlaveBinding()), Qt::UniqueConnection);
    connect(&nameEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    connect(addressUnitBitsEditor_, SIGNAL(textEdited(QString const&)),
        this, SLOT(updateAddressUnitBits(QString const&)), Qt::UniqueConnection);

    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: SingleMemoryMapEditor::~SingleMemoryMapEditor()
//-----------------------------------------------------------------------------
SingleMemoryMapEditor::~SingleMemoryMapEditor()
{

}

//-----------------------------------------------------------------------------
// Function: SingleMemoryMapEditor::isValid()
//-----------------------------------------------------------------------------
bool SingleMemoryMapEditor::isValid() const
{
    return nameEditor_.isValid();
}

//-----------------------------------------------------------------------------
// Function: SingleMemoryMapEditor::refresh()
//-----------------------------------------------------------------------------
void SingleMemoryMapEditor::refresh()
{
    nameEditor_.refresh();
    memoryMapEditor_->refresh();
    refreshSlaveBinding();
    addressUnitBitsEditor_->setText(QString::number(parentMemoryMap_->getAddressUnitBits()));

    if (isMemoryMap())
    {
        remapStateSelector_->setEnabled(false);
        QStringList defaultList;
        defaultList.append("Default");
        remapStateSelector_->refresh(defaultList);
        remapStateSelector_->selectItem("Default");
    }
    else
    {
        refreshRemapStateSelector();
    }
}

//-----------------------------------------------------------------------------
// Function: SingleMemoryMapEditor::setupLayout()
//-----------------------------------------------------------------------------
void SingleMemoryMapEditor::setupLayout()
{
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    QHBoxLayout* scrollLayot = new QHBoxLayout(this);
    scrollLayot->addWidget(scrollArea);
    scrollLayot->setContentsMargins(0, 0, 0, 0);

    QGroupBox* additionalDataGroup = new QGroupBox(tr("Memory map definition"));

    QFormLayout* additionalDataLayout = new QFormLayout();
    additionalDataLayout->addRow(tr("Address Unit Bits [AUB]:"), addressUnitBitsEditor_);

    remapStateSelector_ = new ReferenceSelector(additionalDataGroup);
    additionalDataLayout->addRow(tr("Remap state:"), remapStateSelector_);

    additionalDataLayout->addRow(tr("Slave interface binding:"), slaveInterfaceLabel_);

    connect(remapStateSelector_, SIGNAL(itemSelected(QString const&)),
        this, SLOT(onRemapStateSelected(QString const&)), Qt::UniqueConnection);

    additionalDataGroup->setLayout(additionalDataLayout);

    QHBoxLayout* topOfPageLayout = new QHBoxLayout();
    topOfPageLayout->addWidget(&nameEditor_, 0);
    topOfPageLayout->addWidget(additionalDataGroup, 0);

    QVBoxLayout* addressBlockEditorLayout = new QVBoxLayout();
    addressBlockEditorLayout->addWidget(memoryMapEditor_, 0);

    QWidget* topWidget = new QWidget(scrollArea);
    topWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    scrollArea->setWidget(topWidget);

    QVBoxLayout* topLayout = new QVBoxLayout(topWidget);
    topLayout->addLayout(topOfPageLayout);
    topLayout->addLayout(addressBlockEditorLayout);
    topLayout->setContentsMargins(0, 0, 0, 0);
}

//-----------------------------------------------------------------------------
// Function: SingleMemoryMapEditor::showEvent()
//-----------------------------------------------------------------------------
void SingleMemoryMapEditor::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    emit helpUrlRequested("componenteditor/memorymap.html");
}

//-----------------------------------------------------------------------------
// Function: SingleMemoryMapEditor::refreshSlaveBinding()
//-----------------------------------------------------------------------------
void SingleMemoryMapEditor::refreshSlaveBinding()
{
    QString slaveInterfaceText ("No binding");

    QStringList interfaceNames = component()->getSlaveInterfaces(parentMemoryMap_->getName());
    if (!interfaceNames.isEmpty())
    {
        slaveInterfaceText = interfaceNames.join(" ");
    }
    slaveInterfaceLabel_->setText(slaveInterfaceText);
}

//-----------------------------------------------------------------------------
// Function: SingleMemoryMapEditor::updateAddressUnitBits()
//-----------------------------------------------------------------------------
void SingleMemoryMapEditor::updateAddressUnitBits(QString const& newAddressUnitBits)
{
    parentMemoryMap_->setAddressUnitBits(newAddressUnitBits.toUInt());
    emit addressUnitBitsChanged();
}

//-----------------------------------------------------------------------------
// Function: SingleMemoryMapEditor::refreshRemapStateSelector()
//-----------------------------------------------------------------------------
void SingleMemoryMapEditor::refreshRemapStateSelector()
{
    QStringList remapStateNames;

    foreach (QSharedPointer<RemapState> remapState, *component()->getRemapStates())
    {
        remapStateNames.append(remapState->getName());
    }

    remapStateSelector_->refresh(remapStateNames);

    MemoryRemap* changedMemoryRemap = static_cast<MemoryRemap*>(memoryRemap_.data());
    if (changedMemoryRemap)
    {
        if (changedMemoryRemap->getRemapState().isEmpty())
        {
            remapStateSelector_->selectItem(QString("No remap state selected."));
        }
        else
        {
            remapStateSelector_->selectItem(changedMemoryRemap->getRemapState());
        }
    }
}

//-----------------------------------------------------------------------------
// Function: SingleMemoryMapEditor::isMemoryMap()
//-----------------------------------------------------------------------------
bool SingleMemoryMapEditor::isMemoryMap()
{
    if (memoryRemap_->getName() == parentMemoryMap_->getName())
    {
        return true;
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: SingleMemoryMapEditor::onRemapStateSelected()
//-----------------------------------------------------------------------------
void SingleMemoryMapEditor::onRemapStateSelected(QString const& newRemapState)
{
    QSharedPointer<MemoryRemap> changedMemoryRemap = memoryRemap_.dynamicCast<MemoryRemap>();
    if (changedMemoryRemap)
    {
        changedMemoryRemap->setRemapState(newRemapState);
        emit contentChanged();
    }
}
