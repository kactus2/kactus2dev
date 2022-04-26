//-----------------------------------------------------------------------------
// File: SingleSubspaceMapEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 24.02.2022
//
// Description:
// Editor for editing the details of a single subspace map.
//-----------------------------------------------------------------------------

#include "SingleSubspaceMapEditor.h"

#include <common/widgets/busReferenceComboBox/BusReferenceComboBox.h>
#include <common/widgets/segmentComboBox/SegmentComboBox.h>

#include <KactusAPI/include/BusInterfaceInterface.h>
#include <editors/ComponentEditor/common/ExpressionEditor.h>
#include <editors/ComponentEditor/common/ParameterCompleter.h>
#include <editors/ComponentEditor/common/ExpressionFormatter.h>
#include <editors/ComponentEditor/common/ExpressionParser.h>
#include <KactusAPI/include/SubspaceMapInterface.h>
#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>

#include <IPXACTmodels/Component/MemoryMapBase.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/AddressSpace.h>

#include <QFormLayout>
#include <QScrollArea>
#include <QSplitter>

//-----------------------------------------------------------------------------
// Function: SingleSubspaceMapEditor::SingleSubspaceMapEditor()
//-----------------------------------------------------------------------------
SingleSubspaceMapEditor::SingleSubspaceMapEditor(SubspaceMapInterface* subspaceInterface,
    QSharedPointer<SubSpaceMap> subspaceMap, QSharedPointer<MemoryMapBase> containingMap,
    QSharedPointer<Component> component, LibraryInterface* handler,
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ExpressionFormatter> expressionFormatter,
    QSharedPointer<ExpressionParser> expressionParser, QWidget* parent /* = 0 */):
ItemEditor(component, handler, parent),
nameEditor_(subspaceMap, this, tr("Subspace map name and description")),
baseAddressEditor_(new ExpressionEditor(parameterFinder, this)),
isPresentEditor_(new ExpressionEditor(parameterFinder, this)),
masterSelector_(),
segmentSelector_(),
subspaceName_(subspaceMap->name().toStdString()),
subspaceInterface_(subspaceInterface),
containingMap_(containingMap),
expressionParser_(expressionParser)
{
    subspaceInterface_->setMemoryBlocks(containingMap_->getMemoryBlocks());
    subspaceInterface_->setupSubInterfaces(component);

    constructComboBoxes();

    baseAddressEditor_->setFixedHeight(20);
    isPresentEditor_->setFixedHeight(20);

    ComponentParameterModel* componentParametersModel = new ComponentParameterModel(parameterFinder, this);
    componentParametersModel->setExpressionParser(expressionParser_);

    ParameterCompleter* baseAddressEditorCompleter = new ParameterCompleter(this);
    baseAddressEditorCompleter->setModel(componentParametersModel);
    
    ParameterCompleter* isPresentEditorCompleter = new ParameterCompleter(this);
    isPresentEditorCompleter->setModel(componentParametersModel);

    baseAddressEditor_->setAppendingCompleter(baseAddressEditorCompleter);
    isPresentEditor_->setAppendingCompleter(isPresentEditorCompleter);

    setupLayout();

    connectSignals();

    baseAddressEditor_->setProperty("mandatoryField", true);
    masterSelector_->setProperty("mandatoryField", true);
}

//-----------------------------------------------------------------------------
// Function: SingleSubspaceMapEditor::constructComboBoxes()
//-----------------------------------------------------------------------------
void SingleSubspaceMapEditor::constructComboBoxes()
{
    QString masterReference = QString::fromStdString(subspaceInterface_->getMasterReference(subspaceName_));

    masterSelector_ = new BusReferenceComboBox(subspaceInterface_->getBusInterface(), this);
    masterSelector_->setupSelection(masterReference, General::MASTER);

    segmentSelector_ = new SegmentComboBox(this);

    QSharedPointer<AddressSpace> space = getSpace(masterReference);
    if (space)
    {
        QString segmentReference = QString::fromStdString(subspaceInterface_->getSegmentReference(subspaceName_));
        segmentSelector_->setupSelection(space, masterReference);
    }
}

//-----------------------------------------------------------------------------
// Function: SingleSubspaceMapEditor::getSpace()
//-----------------------------------------------------------------------------
QSharedPointer<AddressSpace> SingleSubspaceMapEditor::getSpace(QString const& busReference)
{
    BusInterfaceInterface* busInterface = subspaceInterface_->getBusInterface();

    QString spaceReference =
        QString::fromStdString(busInterface->getAddressSpaceReference(busReference.toStdString()));
    if (!spaceReference.isEmpty())
    {
        for (auto space : *component()->getAddressSpaces())
        {
            if (space->name() == spaceReference)
            {
                return space;
            }
        }
    }

    return QSharedPointer<AddressSpace>();
}

//-----------------------------------------------------------------------------
// Function: SingleSubspaceMapEditor::showEvent()
//-----------------------------------------------------------------------------
void SingleSubspaceMapEditor::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    emit helpUrlRequested("componenteditor/subspacemap.html");
}

//-----------------------------------------------------------------------------
// Function: SingleSubspaceMapEditor::refresh()
//-----------------------------------------------------------------------------
void SingleSubspaceMapEditor::refresh()
{
    subspaceInterface_->setMemoryBlocks(containingMap_->getMemoryBlocks());
    subspaceInterface_->setupSubInterfaces(component());

    nameEditor_.refresh();

    changeExpressionEditorSignalBlockStatus(true);

    baseAddressEditor_->setExpression(
        QString::fromStdString(subspaceInterface_->getBaseAddressExpression(subspaceName_)));
    baseAddressEditor_->setToolTip(
        QString::fromStdString(subspaceInterface_->getBaseAddressValue(subspaceName_)));

    isPresentEditor_->setExpression(
        QString::fromStdString(subspaceInterface_->getIsPresentExpression(subspaceName_)));
    isPresentEditor_->setToolTip(QString::fromStdString(subspaceInterface_->getIsPresentValue(subspaceName_)));

    changeExpressionEditorSignalBlockStatus(false);

    QString busReference = QString::fromStdString(subspaceInterface_->getMasterReference(subspaceName_));
    QString segmentReference = QString::fromStdString(subspaceInterface_->getSegmentReference(subspaceName_));
    QSharedPointer<AddressSpace> space = getSpace(busReference);

    masterSelector_->setupSelection(busReference, General::MASTER);
    segmentSelector_->setupSelection(space, segmentReference);
}

//-----------------------------------------------------------------------------
// Function: SingleSubspaceMapEditor::onBaseAddressChanged()
//-----------------------------------------------------------------------------
void SingleSubspaceMapEditor::onBaseAddressChanged()
{
    baseAddressEditor_->finishEditingCurrentWord();

    subspaceInterface_->setBaseAddress(subspaceName_, baseAddressEditor_->getExpression().toStdString());
    baseAddressEditor_->setToolTip(QString::fromStdString(subspaceInterface_->getBaseAddressValue(subspaceName_)));
}

//-----------------------------------------------------------------------------
// Function: SingleSubspaceMapEditor::onIsPresentEdited()
//-----------------------------------------------------------------------------
void SingleSubspaceMapEditor::onIsPresentEdited()
{
    isPresentEditor_->finishEditingCurrentWord();

    subspaceInterface_->setIsPresent(subspaceName_, isPresentEditor_->getExpression().toStdString());
    isPresentEditor_->setToolTip(QString::fromStdString(subspaceInterface_->getIsPresentValue(subspaceName_)));

    emit contentChanged();
    emit graphicsChanged();
    emit addressingChanged();
}

//-----------------------------------------------------------------------------
// Function: SingleSubspaceMapEditor::setupLayout()
//-----------------------------------------------------------------------------
void SingleSubspaceMapEditor::setupLayout()
{
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    QHBoxLayout* scrollLayout = new QHBoxLayout(this);
    scrollLayout->addWidget(scrollArea);
    scrollLayout->setContentsMargins(0, 0, 0, 0);

    QGroupBox* subspaceDefinitionGroup = new QGroupBox(tr("Subspace map definition"));
    QFormLayout* subspaceDefinitionLayout = new QFormLayout(subspaceDefinitionGroup);

    subspaceDefinitionLayout->addRow(tr("Base Address [AUB], f(x):"), baseAddressEditor_);
    
    subspaceDefinitionLayout->addRow(tr("Master bus interface reference:"), masterSelector_);
    subspaceDefinitionLayout->addRow(tr("Segment reference:"), segmentSelector_);
    subspaceDefinitionLayout->addRow(tr("Is present, f(x):"), isPresentEditor_);

    QHBoxLayout* topOfPageLayout = new QHBoxLayout();
    topOfPageLayout->addWidget(&nameEditor_, 0);
    topOfPageLayout->addWidget(subspaceDefinitionGroup, 0);

    QWidget* topOfPageWidget = new QWidget(scrollArea);
    topOfPageWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    topOfPageWidget->setLayout(topOfPageLayout);
    topOfPageWidget->setContentsMargins(0, 0, 0, 0);
}

//-----------------------------------------------------------------------------
// Function: SingleSubspaceMapEditor::connectSignals()
//-----------------------------------------------------------------------------
void SingleSubspaceMapEditor::connectSignals() const
{
    connect(masterSelector_, SIGNAL(activated(QString const&)),
        this, SLOT(onMasterSelected(QString const&)), Qt::UniqueConnection);
    connect(segmentSelector_, SIGNAL(activated(QString const&)),
        this, SLOT(onSegmentSelected(QString const&)), Qt::UniqueConnection);

    connect(baseAddressEditor_, SIGNAL(increaseReference(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(baseAddressEditor_, SIGNAL(decreaseReference(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);
    connect(isPresentEditor_, SIGNAL(increaseReference(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(isPresentEditor_, SIGNAL(decreaseReference(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    connect(baseAddressEditor_, SIGNAL(editingFinished()), this, SLOT(onBaseAddressChanged()), Qt::UniqueConnection);    

    connect(&nameEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(baseAddressEditor_, SIGNAL(editingFinished()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    connect(&nameEditor_, SIGNAL(nameChanged()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);

    connect(baseAddressEditor_, SIGNAL(editingFinished()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);
    connect(baseAddressEditor_, SIGNAL(editingFinished()), this, SIGNAL(addressingChanged()), Qt::UniqueConnection);

    connect(isPresentEditor_, SIGNAL(editingFinished()), this, SLOT(onIsPresentEdited()), Qt::UniqueConnection);

    connect(&nameEditor_, SIGNAL(nameChanged()), this, SLOT(onSubspaceNameChanged()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: SingleSubspaceMapEditor::changeExpressionEditorSignalBlocks()
//-----------------------------------------------------------------------------
void SingleSubspaceMapEditor::changeExpressionEditorSignalBlockStatus(bool blockStatus)
{
    baseAddressEditor_->blockSignals(blockStatus);
    isPresentEditor_->blockSignals(blockStatus);
}

//-----------------------------------------------------------------------------
// Function: SingleSubspaceMapEditor::onSubspaceNameChanged()
//-----------------------------------------------------------------------------
void SingleSubspaceMapEditor::onSubspaceNameChanged(QString const& oldName, QString const& newName)
{
    if (oldName == QString::fromStdString(subspaceName_))
    {
        subspaceName_ = newName.toStdString();
    }
}

//-----------------------------------------------------------------------------
// Function: SingleSubspaceMapEditor::onSubspaceNameChanged()
//-----------------------------------------------------------------------------
void SingleSubspaceMapEditor::onSubspaceNameChanged()
{
    QString oldName = QString::fromStdString(subspaceName_);
    subspaceName_ = nameEditor_.name().toStdString();

    emit thisEditorNameChange(oldName, QString::fromStdString(subspaceName_));
}

//-----------------------------------------------------------------------------
// Function: SingleSubspaceMapEditor::onMasterSelected()
//-----------------------------------------------------------------------------
void SingleSubspaceMapEditor::onMasterSelected(QString const& newMasterReference)
{
    subspaceInterface_->setMasterReference(subspaceName_, newMasterReference.toStdString());

    QSharedPointer<AddressSpace> space = getSpace(newMasterReference);
    if (space)
    {
        QString segmentReference = QString::fromStdString(subspaceInterface_->getSegmentReference(subspaceName_));
        segmentSelector_->setupSelection(space, segmentReference);
    }

    emit contentChanged();
    emit graphicsChanged();
    emit addressingChanged();
}

//-----------------------------------------------------------------------------
// Function: SingleSubspaceMapEditor::onSegmentSelected()
//-----------------------------------------------------------------------------
void SingleSubspaceMapEditor::onSegmentSelected(QString const& newSegment)
{
    subspaceInterface_->setSegmentReference(subspaceName_, newSegment.toStdString());

    emit contentChanged();
    emit graphicsChanged();
    emit addressingChanged();
}
