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
#include <editors/ComponentEditor/memoryMaps/RemapModeReferenceEditor.h>
#include <editors/ComponentEditor/memoryMaps/SubspaceMapsEditor.h>
#include <KactusAPI/include/MemoryMapInterface.h>
#include <KactusAPI/include/ModeReferenceInterface.h>

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
nameEditor_(memoryRemap, component->getRevision(), this, tr("Memory remap name and description")),
addressBlockEditor_(new MemoryMapEditor(component, libHandler, parameterFinder, expressionParser,
    mapInterface->getAddressBlockInterface(), memoryRemap->getMemoryBlocks(), this)),
subspaceMapEditor_(new SubspaceMapsEditor(component, parameterFinder, expressionParser,
    mapInterface->getSubspaceMapInterface() , memoryRemap->getMemoryBlocks(), this)),
addressUnitBitsEditor_(new QLineEdit(parent)),
isPresentEditor_(new ExpressionEditor(parameterFinder, this)),
targetInterfaceLabel_(new QLabel(this)),
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

    isPresentEditor_->setFixedHeight(ExpressionEditor::DEFAULT_HEIGHT);

    remapStateSelector_->setProperty("mandatoryField", true);

    modeRefInterface_ = mapInterface_->getModeReferenceInterface();
    modeRefInterface_->setContainingElementIsRemap(isMemoryRemap);

    if (isMemoryRemap_)
    {
        addressUnitBitsEditor_->setEnabled(false);

        currentRemap_ = memoryRemap.dynamicCast<MemoryRemap>();
        modeRefInterface_->setModeReferences(mapInterface_->getRemapModeReferences(parentMapName_, remapName_));

        auto containingModeRefs = mapInterface_->getRemapModeReferencesExcludingRemap(parentMapName_, remapName_);
        modeRefInterface_->setContainingElementModeReferences(containingModeRefs);
        modeRefInterface_->setContainingElementIsRemap(true);
    }
    else
    {
        // Clear the current mode refs, if user was first editing remap, then moves to default remap editor. If not done, 
        // ghost cells will appear in the mode ref table.
        if (auto currentModeRefs = modeRefInterface_->getModeReferences(); !currentModeRefs.empty())
        {
            modeRefInterface_->setModeReferences(std::vector<std::pair<unsigned int, std::string> >());
        }
    }

    modeReferenceEditor_ = new RemapModeReferenceEditor(modeRefInterface_, this);

    connectSignals();
    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: SingleMemoryMapEditor::connectSignals()
//-----------------------------------------------------------------------------
void SingleMemoryMapEditor::connectSignals()
{
    connect(addressBlockEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(addressBlockEditor_, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(addressBlockEditor_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);
    connect(addressBlockEditor_, SIGNAL(childAdded(int)), this, SIGNAL(childAdded(int)), Qt::UniqueConnection);
    connect(addressBlockEditor_, SIGNAL(childRemoved(int)), this, SIGNAL(childRemoved(int)), Qt::UniqueConnection);
    connect(addressBlockEditor_, SIGNAL(childAddressingChanged(int)),
        this, SIGNAL(childAddressingChanged(int)), Qt::UniqueConnection);

    connect(addressBlockEditor_, SIGNAL(graphicsChanged(int)), this, SIGNAL(childGraphicsChanged(int)), Qt::UniqueConnection);

    connect(this, SIGNAL(assignNewAddressUnitBits(QString const&)),
        addressBlockEditor_, SIGNAL(assignNewAddressUnitBits(QString const&)), Qt::UniqueConnection);

    connect(addressBlockEditor_, SIGNAL(addressBlockNameChanged(QString const&, QString const&)),
        this, SIGNAL(addressBlockNameChanged(QString const&, QString const&)), Qt::UniqueConnection);

    connect(subspaceMapEditor_, SIGNAL(childAdded(int)), this, SIGNAL(childAdded(int)), Qt::UniqueConnection);
    connect(subspaceMapEditor_, SIGNAL(childRemoved(int)), this, SIGNAL(childRemoved(int)), Qt::UniqueConnection);

    connect(subspaceMapEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(subspaceMapEditor_, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(subspaceMapEditor_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    connect(subspaceMapEditor_, SIGNAL(childAddressingChanged(int)),
        this, SIGNAL(childAddressingChanged(int)), Qt::UniqueConnection);
    connect(subspaceMapEditor_, SIGNAL(graphicsChanged(int)),
        this, SIGNAL(childGraphicsChanged(int)), Qt::UniqueConnection);

    connect(this, SIGNAL(assignNewAddressUnitBits(QString const&)),
        subspaceMapEditor_, SIGNAL(assignNewAddressUnitBits(QString const&)), Qt::UniqueConnection);

    connect(subspaceMapEditor_, SIGNAL(subspaceMapNameChanged(QString const&, QString const&)),
        this, SIGNAL(subspaceMapNameChanged(QString const&, QString const&)), Qt::UniqueConnection);

    connect(subspaceMapEditor_, SIGNAL(invalidateOtherFilter()),
        addressBlockEditor_, SIGNAL(invalidateThisFilter()), Qt::UniqueConnection);
    connect(addressBlockEditor_, SIGNAL(invalidateOtherFilter()),
        subspaceMapEditor_, SIGNAL(invalidateThisFilter()), Qt::UniqueConnection);

    connect(&nameEditor_, SIGNAL(contentChanged()), this, SLOT(refreshTargetBinding()), Qt::UniqueConnection);
    connect(&nameEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&nameEditor_, SIGNAL(nameChanged()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);

    connect(addressUnitBitsEditor_, SIGNAL(editingFinished()),
        this, SLOT(updateAddressUnitBits()), Qt::UniqueConnection);

    connect(isPresentEditor_, SIGNAL(increaseReference(QString const&)),
        this, SIGNAL(increaseReferences(QString const&)), Qt::UniqueConnection);
    connect(isPresentEditor_, SIGNAL(decreaseReference(QString const&)),
        this, SIGNAL(decreaseReferences(QString const&)), Qt::UniqueConnection);

    connect(isPresentEditor_, SIGNAL(editingFinished()), this, SLOT(onIsPresentEdited()), Qt::UniqueConnection);
    connect(isPresentEditor_, SIGNAL(editingFinished()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(isPresentEditor_, SIGNAL(editingFinished()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);

    connect(&nameEditor_, SIGNAL(nameChanged()), this, SLOT(onNameChange()), Qt::UniqueConnection);

    connect(modeReferenceEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(modeReferenceEditor_, SIGNAL(contentChanged()), this, SLOT(onRemapModeReferencesEdited()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: SingleMemoryMapEditor::refresh()
//-----------------------------------------------------------------------------
void SingleMemoryMapEditor::refresh()
{
    mapInterface_->setMemoryMaps(component());

    nameEditor_.refresh();
    addressBlockEditor_->refresh();
    subspaceMapEditor_->refresh();

    refreshTargetBinding();
    addressUnitBitsEditor_->setText(QString::fromStdString(mapInterface_->getAddressUnitBits(parentMapName_)));

    if (component()->getRevision() != Document::Revision::Std22)
    {
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
    }
    else
    {
        // Refresh mode references for use in current editor.
        if (isMemoryRemap_)
        {
            modeRefInterface_->setModeReferences(mapInterface_->getRemapModeReferences(parentMapName_, remapName_));
            modeRefInterface_->setContainingElementIsRemap(true);

            auto containingModeRefs = mapInterface_->getRemapModeReferencesExcludingRemap(parentMapName_, remapName_);
            modeRefInterface_->setContainingElementModeReferences(containingModeRefs);
        }
        // Clear mode references from interface, 
        else
        {
            if (auto currentModeRefs = modeRefInterface_->getModeReferences(); !currentModeRefs.empty())
            {
                modeRefInterface_->setModeReferences(std::vector<std::pair<unsigned int, std::string> >());
            }
        }
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

    QLayout* topOfPageLayout;
    if (Document::Revision revision = component()->getRevision(); 
        revision == Document::Revision::Std14)
    {
        memoryMapDefinitionGroupLayout->addRow(tr("Address Unit Bits [AUB]:"), addressUnitBitsEditor_);
        memoryMapDefinitionGroupLayout->addRow(tr("Is present, f(x):"), isPresentEditor_);
        memoryMapDefinitionGroupLayout->addRow(tr("Remap state:"), remapStateSelector_);
        memoryMapDefinitionGroupLayout->addRow(tr("Slave interface binding:"), targetInterfaceLabel_);
        connect(remapStateSelector_, SIGNAL(itemSelected(QString const&)),
            this, SLOT(onRemapStateSelected(QString const&)), Qt::UniqueConnection);

        QHBoxLayout* layout = new QHBoxLayout();
        layout->addWidget(&nameEditor_, 0);
        layout->addWidget(memoryMapDefinitionGroup, 0);
        topOfPageLayout = layout;
        modeReferenceEditor_->setVisible(false);
    }
    else if (revision == Document::Revision::Std22)
    {
        memoryMapDefinitionGroupLayout->addRow(tr("Address Unit Bits [AUB]:"), addressUnitBitsEditor_);
        memoryMapDefinitionGroupLayout->addRow(tr("Target interface binding:"), targetInterfaceLabel_);

        QGridLayout* layout = new QGridLayout();
        layout->addWidget(&nameEditor_, 0, 0, 2, 1);
        layout->addWidget(memoryMapDefinitionGroup, 0, 1, 1, 1);
        layout->addWidget(modeReferenceEditor_, 1, 1, 1, 1);

        if (!isMemoryRemap_)
        {
            modeReferenceEditor_->setEnabled(false);
        }

        topOfPageLayout = layout;
    }

    //QHBoxLayout* topOfPageLayout = new QHBoxLayout();
    //topOfPageLayout->addWidget(&nameEditor_, 0);
    //topOfPageLayout->addWidget(memoryMapDefinitionGroup, 0);

    QWidget* topOfPageWidget = new QWidget();
    topOfPageWidget->setSizeIncrement(QSizePolicy::Expanding, QSizePolicy::Maximum);
    topOfPageWidget->setLayout(topOfPageLayout);
    topOfPageWidget->setContentsMargins(0, 0, 0, 0);

    QSplitter* verticalSplitter = new QSplitter(Qt::Vertical, scrollArea);
    verticalSplitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    verticalSplitter->addWidget(topOfPageWidget);
    verticalSplitter->addWidget(addressBlockEditor_);
    verticalSplitter->addWidget(subspaceMapEditor_);

    verticalSplitter->setStretchFactor(1,1);

    for (int i = 1; i <= verticalSplitter->count(); ++i)
    {
        QSplitterHandle* handle = verticalSplitter->handle(i);

        QVBoxLayout* handleLayout = new QVBoxLayout(handle);
        handleLayout->setSpacing(0);
        handleLayout->setContentsMargins(0, 0, 0, 0);

        QFrame* line = new QFrame(handle);
        line->setLineWidth(2);
        line->setMidLineWidth(2);
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        handleLayout->addWidget(line);
    }

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
void SingleMemoryMapEditor::refreshTargetBinding()
{
    QString targetInterfaceText ("No binding");

    QStringList interfaceNames = component()->getTargetInterfaces(QString::fromStdString(parentMapName_));
    if (!interfaceNames.isEmpty())
    {
        targetInterfaceText = interfaceNames.join(", ");
    }
    targetInterfaceLabel_->setText(targetInterfaceText);
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

    for (QSharedPointer<RemapState> remapState : *component()->getRemapStates())
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
// Function: SingleMemoryMapEditor::onRemapModeReferencesEdited()
//-----------------------------------------------------------------------------
void SingleMemoryMapEditor::onRemapModeReferencesEdited()
{
    auto newRemapModes = modeRefInterface_->getModeReferences();

    mapInterface_->setRemapModeReferences(parentMapName_, remapName_, newRemapModes);
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
