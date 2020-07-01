//-----------------------------------------------------------------------------
// File: SingleMemoryMapEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 15.04.2015
//
// Description:
// Editor for editing the details of a single memory map.
//-----------------------------------------------------------------------------

#include "SingleMemoryMapEditor.h"

#include <QScrollArea>
#include <QLabel>

#include <editors/ComponentEditor/common/ExpressionEditor.h>
#include <editors/ComponentEditor/memoryMaps/interfaces/MemoryMapInterface.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/MemoryMapBase.h>
#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/Component/MemoryRemap.h>
#include <IPXACTmodels/Component/RemapState.h>

#include <QFormLayout>
#include <QSplitter>

//-----------------------------------------------------------------------------
// Function: SingleMemoryMapEditor::SingleMemoryMapEditor()
//-----------------------------------------------------------------------------
SingleMemoryMapEditor::SingleMemoryMapEditor(QSharedPointer<Component> component,
    QSharedPointer<MemoryMapBase> memoryRemap, QString const& parentMapName, LibraryInterface* libHandler,
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ExpressionParser> expressionParser,
    MemoryMapInterface* mapInterface, bool isMemoryRemap, QWidget* parent):
ItemEditor(component, libHandler, parent),
nameEditor_(memoryRemap, this, tr("Memory remap name and description")),
memoryMapEditor_(new MemoryMapEditor(component, libHandler, parameterFinder, expressionParser,
    mapInterface->getSubInterface(), memoryRemap->getMemoryBlocks(), this)),
addressUnitBitsEditor_(new QLineEdit(parent)),
isPresentEditor_(new ExpressionEditor(parameterFinder, this)),
slaveInterfaceLabel_(new QLabel(this)),
remapStateSelector_(new ReferenceSelector(this)),
remapName_(memoryRemap->name().toStdString()),
parentMapName_(parentMapName.toStdString()),
mapInterface_(mapInterface),
isMemoryRemap_(isMemoryRemap)
{
    if (!isMemoryRemap_)
    {
        remapName_ = "";
    }

    mapInterface_->setMemoryMaps(component);

    addressUnitBitsEditor_->setValidator
        (new QRegularExpressionValidator(QRegularExpression("\\d*"), addressUnitBitsEditor_));

    isPresentEditor_->setFixedHeight(20);

    if (isMemoryRemap_)
    {
        addressUnitBitsEditor_->setEnabled(false);
    }

    remapStateSelector_->setProperty("mandatoryField", true);

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
    
    connect(isPresentEditor_, SIGNAL(increaseReference(QString const&)),
        this, SIGNAL(increaseReferences(QString const&)), Qt::UniqueConnection);
    connect(isPresentEditor_, SIGNAL(decreaseReference(QString const&)),
        this, SIGNAL(decreaseReferences(QString const&)), Qt::UniqueConnection);

    connect(isPresentEditor_, SIGNAL(editingFinished()), this, SLOT(onIsPresentEdited()), Qt::UniqueConnection);
    connect(isPresentEditor_, SIGNAL(editingFinished()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(isPresentEditor_, SIGNAL(editingFinished()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);

    connect(this, SIGNAL(assignNewAddressUnitBits(QString const&)),
        memoryMapEditor_, SIGNAL(assignNewAddressUnitBits(QString const&)), Qt::UniqueConnection);

    connect(memoryMapEditor_, SIGNAL(addressBlockNameChanged(QString const&, QString const&)),
        this, SIGNAL(addressBlockNameChanged(QString const&, QString const&)), Qt::UniqueConnection);

    connect(&nameEditor_, SIGNAL(nameChanged()), this, SLOT(onNameChange()), Qt::UniqueConnection);

    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: SingleMemoryMapEditor::~SingleMemoryMapEditor()
//-----------------------------------------------------------------------------
SingleMemoryMapEditor::~SingleMemoryMapEditor()
{

}

//-----------------------------------------------------------------------------
// Function: SingleMemoryMapEditor::refresh()
//-----------------------------------------------------------------------------
void SingleMemoryMapEditor::refresh()
{
    mapInterface_->setMemoryMaps(component());

    nameEditor_.refresh();
    memoryMapEditor_->refresh();
    refreshSlaveBinding();
    addressUnitBitsEditor_->setText(QString::fromStdString(mapInterface_->getAddressUnitBits(parentMapName_)));

    if (!isMemoryRemap_)
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

    isPresentEditor_->setExpression(
        QString::fromStdString(mapInterface_->getIsPresentExpression(parentMapName_, remapName_)));
    isPresentEditor_->setToolTip(
        QString::fromStdString(mapInterface_->getIsPresentValue(parentMapName_, remapName_)));
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
    memoryMapDefinitionGroupLayout->addRow(tr("Is present, f(x):"), isPresentEditor_);
    memoryMapDefinitionGroupLayout->addRow(tr("Remap state:"), remapStateSelector_);
    memoryMapDefinitionGroupLayout->addRow(tr("Slave interface binding:"), slaveInterfaceLabel_);

    connect(remapStateSelector_, SIGNAL(itemSelected(QString const&)),
        this, SLOT(onRemapStateSelected(QString const&)), Qt::UniqueConnection);

    QHBoxLayout* topOfPageLayout = new QHBoxLayout();
    topOfPageLayout->addWidget(&nameEditor_, 0);
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

    QStringList interfaceNames = component()->getSlaveInterfaces(QString::fromStdString(parentMapName_));
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
    mapInterface_->setAddressUnitBits(parentMapName_, addressUnitBitsEditor_->text().toStdString());

    emit addressUnitBitsChanged();
    emit contentChanged();

    emit assignNewAddressUnitBits(addressUnitBitsEditor_->text());
}

//-----------------------------------------------------------------------------
// Function: SingleMemoryMapEditor::onIsPresentEdited()
//-----------------------------------------------------------------------------
void SingleMemoryMapEditor::onIsPresentEdited()
{
    isPresentEditor_->finishEditingCurrentWord();

    QString newIsPresent = isPresentEditor_->getExpression();
    //isPresentEditor_->setToolTip(formattedValueFor(newIsPresent));

    mapInterface_->setIsPresent(parentMapName_, newIsPresent.toStdString(), remapName_);
    isPresentEditor_->setToolTip(
        QString::fromStdString(mapInterface_->getIsPresentValue(parentMapName_, remapName_)));
}

//-----------------------------------------------------------------------------
// Function: SingleMemoryMapEditor::refreshRemapStateSelector()
//-----------------------------------------------------------------------------
void SingleMemoryMapEditor::refreshRemapStateSelector()
{
    QStringList remapStateNames;

    foreach (QSharedPointer<RemapState> remapState, *component()->getRemapStates())
    {
        remapStateNames.append(remapState->name());
    }

    remapStateSelector_->refresh(remapStateNames);

    if (isMemoryRemap_)
    {
        QString remapState = QString::fromStdString(mapInterface_->getRemapState(parentMapName_, remapName_));
        if (remapState.isEmpty())
        {
            remapStateSelector_->selectItem(QString("No remap state selected."));
        }
        else
        {
            remapStateSelector_->selectItem(remapState);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: SingleMemoryMapEditor::onRemapStateSelected()
//-----------------------------------------------------------------------------
void SingleMemoryMapEditor::onRemapStateSelected(QString const& newRemapState)
{
    if (isMemoryRemap_)
    {
        mapInterface_->setRemapState(parentMapName_, remapName_, newRemapState.toStdString());
        emit contentChanged();
    }
}

//-----------------------------------------------------------------------------
// Function: SingleMemoryMapEditor::onMemoryMapNameChanged()
//-----------------------------------------------------------------------------
void SingleMemoryMapEditor::onMemoryMapNameChanged(QString const& oldName, QString const& newName)
{
    if (remapName_.empty() && oldName == QString::fromStdString(parentMapName_))
    {
        parentMapName_ = newName.toStdString();
    }
}

//-----------------------------------------------------------------------------
// Function: SingleMemoryMapEditor::onMemoryRemapNameChanged()
//-----------------------------------------------------------------------------
void SingleMemoryMapEditor::onMemoryRemapNameChanged(QString const& parentName, QString const& oldName,
    QString const& newName)
{
    if (parentName == QString::fromStdString(parentMapName_) &&  oldName == QString::fromStdString(remapName_))
    {
        remapName_ = newName.toStdString();
    }
}

//-----------------------------------------------------------------------------
// Function: SingleMemoryMapEditor::onNameChange()
//-----------------------------------------------------------------------------
void SingleMemoryMapEditor::onNameChange()
{
    if (remapName_.empty())
    {
        parentMapName_ = nameEditor_.name().toStdString();
    }
    else
    {
        remapName_ = nameEditor_.name().toStdString();
    }
}
