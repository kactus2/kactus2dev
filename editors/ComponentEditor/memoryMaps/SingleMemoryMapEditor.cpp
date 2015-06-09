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
#include <QSplitter>

//-----------------------------------------------------------------------------
// Function: SingleMemoryMapEditor::SingleMemoryMapEditor()
//-----------------------------------------------------------------------------
SingleMemoryMapEditor::SingleMemoryMapEditor(QSharedPointer<Component> component,
    QSharedPointer<AbstractMemoryMap> memoryRemap, QSharedPointer<MemoryMap> parentMemoryMap,
    LibraryInterface* libHandler, QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionFormatter> expressionFormatter, 
    QSharedPointer<ExpressionParser> expressionParser,
    QWidget* parent):
ItemEditor(component, libHandler, parent),
nameEditor_(memoryRemap->getNameGroup(), this, tr("Memory remap name and description")),
memoryMapEditor_(new MemoryMapEditor(component, libHandler, memoryRemap, parameterFinder, expressionFormatter,
    expressionParser, this)),
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

    connect(&nameEditor_, SIGNAL(nameChanged()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);
    connect(memoryMapEditor_, SIGNAL(graphicsChanged()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);

    connect(addressUnitBitsEditor_, SIGNAL(editingFinished()),
        this, SLOT(updateAddressUnitBits()), Qt::UniqueConnection);

    setupLayout();

    remapStateSelector_->setProperty("mandatoryField", true);
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
    bool nameIsValid = nameEditor_.isValid();
    bool memoryMapEditorIsValid = memoryMapEditor_->isValid();

    bool memoryRemapIsValid = false;

    if (isMemoryMap())
    {
        memoryRemapIsValid =
            parentMemoryMap_->isValid(component()->getChoices(), component()->getRemapStateNames());
    }
    else
    {
        QSharedPointer<MemoryRemap> transformedMemoryRemap = memoryRemap_.dynamicCast<MemoryRemap>();
        if (transformedMemoryRemap)
        {
            memoryRemapIsValid =
                transformedMemoryRemap->isValid(component()->getChoices(), component()->getRemapStateNames());
        }
    }

    return nameIsValid && memoryMapEditorIsValid && memoryRemapIsValid;
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

    QGroupBox* memoryMapDefinitionGroup = new QGroupBox(tr("Memory map definition"));

    QFormLayout* memoryMapDefinitionGroupLayout = new QFormLayout(memoryMapDefinitionGroup);
    memoryMapDefinitionGroupLayout->addRow(tr("Address Unit Bits [AUB]:"), addressUnitBitsEditor_);

    remapStateSelector_ = new ReferenceSelector(memoryMapDefinitionGroup);
    memoryMapDefinitionGroupLayout->addRow(tr("Remap state:"), remapStateSelector_);

    memoryMapDefinitionGroupLayout->addRow(tr("Slave interface binding:"), slaveInterfaceLabel_);

    connect(remapStateSelector_, SIGNAL(itemSelected(QString const&)),
        this, SLOT(onRemapStateSelected(QString const&)), Qt::UniqueConnection);

    QHBoxLayout* topOfPageLayout = new QHBoxLayout();
    topOfPageLayout->addWidget(&nameEditor_, 0, Qt::AlignTop);
    topOfPageLayout->addWidget(memoryMapDefinitionGroup, 0);

    QWidget* topOfPageWidget = new QWidget();
    topOfPageWidget->setSizeIncrement(QSizePolicy::Expanding, QSizePolicy::Maximum);
    topOfPageWidget->setLayout(topOfPageLayout);
    topOfPageWidget->setContentsMargins(0, 0, 0, 0);

    QSplitter* verticalSplitter = new QSplitter(Qt::Vertical, scrollArea);
    verticalSplitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    verticalSplitter->addWidget(topOfPageWidget);
    verticalSplitter->addWidget(memoryMapEditor_);

    verticalSplitter->setStretchFactor(1,1);

    QSplitterHandle* handle = verticalSplitter->handle(1);

    QVBoxLayout* handleLayout = new QVBoxLayout(handle);
    handleLayout->setSpacing(0);
    handleLayout->setMargin(0);

    QFrame* line = new QFrame(handle);
    line->setLineWidth(2);
    line->setMidLineWidth(2);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    handleLayout->addWidget(line);

    verticalSplitter->setHandleWidth(10);

    scrollArea->setWidget(verticalSplitter);
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
        slaveInterfaceText = interfaceNames.join(", ");
    }
    slaveInterfaceLabel_->setText(slaveInterfaceText);
}

//-----------------------------------------------------------------------------
// Function: SingleMemoryMapEditor::updateAddressUnitBits()
//-----------------------------------------------------------------------------
void SingleMemoryMapEditor::updateAddressUnitBits()
{
    parentMemoryMap_->setAddressUnitBits(addressUnitBitsEditor_->text().toUInt());
    emit addressUnitBitsChanged();
    emit contentChanged();
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
bool SingleMemoryMapEditor::isMemoryMap() const
{
    QSharedPointer<MemoryMap> transFormedMemoryRemap = memoryRemap_.dynamicCast<MemoryMap>();

    if (transFormedMemoryRemap && transFormedMemoryRemap == parentMemoryMap_)
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
