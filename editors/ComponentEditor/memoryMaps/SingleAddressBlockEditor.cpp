//-----------------------------------------------------------------------------
// File: SingleAddressBlockEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 23.04.2015
//
// Description:
// Editor for editing the details of a single address block.
//-----------------------------------------------------------------------------

#include "SingleAddressBlockEditor.h"

#include "registerfileeditor.h"

#include <common/widgets/usageComboBox/usagecombobox.h>
#include <common/widgets/accessComboBox/accesscombobox.h>
#include <common/widgets/booleanComboBox/booleancombobox.h>

#include <editors/ComponentEditor/common/ExpressionEditor.h>
#include <KactusAPI/include/ExpressionFormatter.h>
#include <KactusAPI/include/ExpressionParser.h>
#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>
#include <editors/ComponentEditor/memoryMaps/AccessPoliciesEditor.h>
#include <KactusAPI/include/AddressBlockInterface.h>
#include <KactusAPI/include/RegisterInterface.h>

#include <IPXACTmodels/Component/MemoryMapBase.h>
#include <IPXACTmodels/Component/validators/AddressBlockValidator.h>
#include <IPXACTmodels/Component/validators/RegisterFileValidator.h>

#include <QFormLayout>
#include <QScrollArea>
#include <QSplitter>

//-----------------------------------------------------------------------------
// Function: SingleAddressBlockEditor::SingleAddressBlockEditor()
//-----------------------------------------------------------------------------
SingleAddressBlockEditor::SingleAddressBlockEditor(AddressBlockInterface* blockInterface,
    QSharedPointer<AddressBlock> addressBlock,
    QSharedPointer<MemoryMapBase> containingMap,
    QSharedPointer<Component> component, LibraryInterface* handler,
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ExpressionFormatter> expressionFormatter,
    QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<AddressBlockValidator> addressBlockValidator,
    QWidget* parent):
ItemEditor(component, handler, parent),
nameEditor_(addressBlock, component->getRevision(), this,blockInterface, tr("Address block name and description")),
usageEditor_(),
baseAddressEditor_(new ExpressionEditor(parameterFinder, this)),
rangeEditor_(new ExpressionEditor(parameterFinder, this)),
widthEditor_(new ExpressionEditor(parameterFinder, this)),
isPresentEditor_(new ExpressionEditor(parameterFinder, this)),
accessEditor_(),
volatileEditor_(),
registersEditor_(new AddressBlockEditor(addressBlock->getRegisterData(), blockInterface->getSubInterface(),
    component, handler, parameterFinder, this)),
registerFilesEditor_(new RegisterFileEditor(addressBlock->getRegisterData(), component, handler,
    parameterFinder, expressionFormatter, addressBlockValidator->getRegisterFileValidator(), this)),
blockName_(addressBlock->name().toStdString()),
blockInterface_(blockInterface),
containingMap_(containingMap),
expressionParser_(expressionParser),
accessPoliciesEditor_(new AccessPoliciesEditor(addressBlock->getAccessPolicies(), 
    blockInterface->getSubInterface()->getAccessPolicyInterface(), this))
{
    blockInterface_->setMemoryBlocks(containingMap_->getMemoryBlocks());

    baseAddressEditor_->setFixedHeight(ExpressionEditor::DEFAULT_HEIGHT);
    rangeEditor_->setFixedHeight(ExpressionEditor::DEFAULT_HEIGHT);
    widthEditor_->setFixedHeight(ExpressionEditor::DEFAULT_HEIGHT);
    isPresentEditor_->setFixedHeight(ExpressionEditor::DEFAULT_HEIGHT);

    ComponentParameterModel* componentParametersModel = new ComponentParameterModel(parameterFinder, this);
    componentParametersModel->setExpressionParser(expressionParser_);

    auto baseAddressEditorCompleter = new QCompleter(this);
    baseAddressEditorCompleter->setModel(componentParametersModel);

    auto rangeEditorCompleter = new QCompleter(this);
    rangeEditorCompleter->setModel(componentParametersModel);

    auto widthEditorCompleter = new QCompleter(this);
    widthEditorCompleter->setModel(componentParametersModel);
    
    auto isPresentEditorCompleter = new QCompleter(this);
    isPresentEditorCompleter->setModel(componentParametersModel);

    baseAddressEditor_->setAppendingCompleter(baseAddressEditorCompleter);
    rangeEditor_->setAppendingCompleter(rangeEditorCompleter);
    widthEditor_->setAppendingCompleter(widthEditorCompleter);
    isPresentEditor_->setAppendingCompleter(isPresentEditorCompleter);

    setupLayout();

    connectSignals();

    baseAddressEditor_->setProperty("mandatoryField", true);
    rangeEditor_->setProperty("mandatoryField", true);
    widthEditor_->setProperty("mandatoryField", true);
}

//-----------------------------------------------------------------------------
// Function: SingleAddressBlockEditor::showEvent()
//-----------------------------------------------------------------------------
void SingleAddressBlockEditor::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);

    if (component()->getRevision() == Document::Revision::Std22)
    {
        emit helpUrlRequested("componenteditor/addressblock2022.html");
    }
    else
    {
        emit helpUrlRequested("componenteditor/addressblock.html");
    }
}

//-----------------------------------------------------------------------------
// Function: SingleAddressBlockEditor::refresh()
//-----------------------------------------------------------------------------
void SingleAddressBlockEditor::refresh()
{
    blockInterface_->setMemoryBlocks(containingMap_->getMemoryBlocks());

    nameEditor_.refresh();
    registersEditor_->refresh();

    accessPoliciesEditor_->refresh();

    General::Usage usage = blockInterface_->getUsage(blockName_);
    registersEditor_->setEnabled(blockInterface_->hasRegisters(blockName_) || (usage != General::RESERVED));
    registerFilesEditor_->setEnabled(blockInterface_->hasRegisters(blockName_) || (usage != General::RESERVED));

    // Block signals from here for the duration of refreshing editors.
    blockSignals(true);

    usageEditor_->setCurrentValue(usage);

    changeExpressionEditorSignalBlockStatus(true);

    baseAddressEditor_->setExpression(
        QString::fromStdString(blockInterface_->getBaseAddressExpression(blockName_)));
    baseAddressEditor_->setToolTip(
        QString::fromStdString(blockInterface_->getBaseAddressValue(blockName_)));

    rangeEditor_->setExpression(QString::fromStdString(blockInterface_->getRangeExpression(blockName_)));
    rangeEditor_->setToolTip(QString::fromStdString(blockInterface_->getRangeValue(blockName_)));

    widthEditor_->setExpression(QString::fromStdString(blockInterface_->getWidthExpression(blockName_)));
    widthEditor_->setToolTip(QString::fromStdString(blockInterface_->getWidthValue(blockName_)));

    isPresentEditor_->setExpression(QString::fromStdString(blockInterface_->getIsPresentExpression(blockName_)));
    isPresentEditor_->setToolTip(QString::fromStdString(blockInterface_->getIsPresentValue(blockName_)));

    changeExpressionEditorSignalBlockStatus(false);

    accessEditor_->setCurrentValue(blockInterface_->getAccess(blockName_));
    volatileEditor_->setCurrentValue(QString::fromStdString(blockInterface_->getVolatile(blockName_)));

    blockSignals(false);
}

//-----------------------------------------------------------------------------
// Function: SingleAddressBlockEditor::onBaseAddressChanged()
//-----------------------------------------------------------------------------
void SingleAddressBlockEditor::onBaseAddressChanged()
{
    baseAddressEditor_->finishEditingCurrentWord();

    blockInterface_->setBaseAddress(blockName_, baseAddressEditor_->getExpression().toStdString());
    baseAddressEditor_->setToolTip(QString::fromStdString(blockInterface_->getBaseAddressValue(blockName_)));
}

//-----------------------------------------------------------------------------
// Function: SingleAddressBlockEditor::onRangeChanged()
//-----------------------------------------------------------------------------
void SingleAddressBlockEditor::onRangeChanged()
{
    rangeEditor_->finishEditingCurrentWord();

    blockInterface_->setRange(blockName_, rangeEditor_->getExpression().toStdString());
    rangeEditor_->setToolTip(QString::fromStdString(blockInterface_->getRangeValue(blockName_)));
}

//-----------------------------------------------------------------------------
// Function: SingleAddressBlockEditor::onWidthChanged()
//-----------------------------------------------------------------------------
void SingleAddressBlockEditor::onWidthChanged()
{
    widthEditor_->finishEditingCurrentWord();

    blockInterface_->setWidth(blockName_, widthEditor_->getExpression().toStdString());
    widthEditor_->setToolTip(QString::fromStdString(blockInterface_->getWidthValue(blockName_)));
}

//-----------------------------------------------------------------------------
// Function: SingleRegisterEditor::onIsPresentEdited()
//-----------------------------------------------------------------------------
void SingleAddressBlockEditor::onIsPresentEdited()
{
    isPresentEditor_->finishEditingCurrentWord();

    blockInterface_->setIsPresent(blockName_, isPresentEditor_->getExpression().toStdString());
    isPresentEditor_->setToolTip(
        QString::fromStdString(blockInterface_->getIsPresentValue(blockName_)));
}

//-----------------------------------------------------------------------------
// Function: SingleAddressBlockEditor::formattedValueFor()
//-----------------------------------------------------------------------------
QString SingleAddressBlockEditor::formattedValueFor(QString const& expression) const
{
    return ExpressionFormatter::format(expression, expressionParser_);
}

//-----------------------------------------------------------------------------
// Function: SingleAddressBlockEditor::onUsageSelected()
//-----------------------------------------------------------------------------
void SingleAddressBlockEditor::onUsageSelected(QString const& newUsage)
{
    General::Usage usage = General::str2Usage(newUsage, General::USAGE_COUNT);

    blockInterface_->setUsage(blockName_, newUsage.toStdString());
    registersEditor_->setEnabled(blockInterface_->hasRegisters(blockName_) || (usage != General::RESERVED));
    registerFilesEditor_->setEnabled(blockInterface_->hasRegisters(blockName_) || (usage != General::RESERVED));

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: SingleAddressBlockEditor::onAccessSelected()
//-----------------------------------------------------------------------------
void SingleAddressBlockEditor::onAccessSelected(QString const& newAccess)
{
    blockInterface_->setAccess(blockName_, newAccess.toStdString());

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: SingleAddressBlockEditor::onVolatileSelected()
//-----------------------------------------------------------------------------
void SingleAddressBlockEditor::onVolatileSelected(QString const& newVolatileValue)
{
    blockInterface_->setVolatile(blockName_, newVolatileValue.toStdString());

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: SingleAddressBlockEditor::setupLayout()
//-----------------------------------------------------------------------------
void SingleAddressBlockEditor::setupLayout()
{
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    QHBoxLayout* scrollLayout = new QHBoxLayout(this);
    scrollLayout->addWidget(scrollArea);
    scrollLayout->setContentsMargins(0, 0, 0, 0);

    QGroupBox* addressBlockDefinitionGroup = new QGroupBox(tr("Address block definition"));

    QFormLayout* addressBlockDefinitionLayout = new QFormLayout(addressBlockDefinitionGroup);

    addressBlockDefinitionLayout->addRow(tr("Base Address [AUB], f(x):"), baseAddressEditor_);
    addressBlockDefinitionLayout->addRow(tr("Range [AUB], f(x):"), rangeEditor_);
    addressBlockDefinitionLayout->addRow(tr("Width [bits], f(x):"), widthEditor_);
    
    volatileEditor_ = new BooleanComboBox(addressBlockDefinitionGroup);
    usageEditor_ = new UsageComboBox(addressBlockDefinitionGroup);
    accessEditor_ = new AccessComboBox(addressBlockDefinitionGroup);

    if (component()->getRevision() == Document::Revision::Std14)
    {
        addressBlockDefinitionLayout->addRow(tr("Is present, f(x):"), isPresentEditor_);
        addressBlockDefinitionLayout->addRow(tr("Volatile:"), volatileEditor_);
        addressBlockDefinitionLayout->addRow(tr("Usage:"), usageEditor_);
        addressBlockDefinitionLayout->addRow(tr("Access:"), accessEditor_);
    }
    else
    {
        addressBlockDefinitionLayout->addRow(tr("Usage:"), usageEditor_);
        addressBlockDefinitionLayout->addRow(tr("Volatile:"), volatileEditor_);

        auto spacer = new QWidget();
        spacer->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        addressBlockDefinitionLayout->addRow(spacer);

        isPresentEditor_->setVisible(false);
        accessEditor_->setVisible(false);
    }
    
    QLayout* topOfPageLayout;

    if (component()->getRevision() == Document::Revision::Std14)
    {
        QHBoxLayout* oldStdLayout = new QHBoxLayout();
        oldStdLayout->addWidget(&nameEditor_, 0);
        oldStdLayout->addWidget(addressBlockDefinitionGroup, 0);
        topOfPageLayout = oldStdLayout;
    }
    else //if (component()->getRevision() == Document::Revision::Std22)
    {
        QGridLayout* newStdLayout = new QGridLayout();

        newStdLayout->addWidget(&nameEditor_, 0, 0, 2, 1);
        newStdLayout->addWidget(addressBlockDefinitionGroup, 0, 1);
        newStdLayout->addWidget(accessPoliciesEditor_, 1, 1);

        newStdLayout->setRowStretch(0, 1);
        newStdLayout->setRowStretch(1, 2);

        topOfPageLayout = newStdLayout;
    }

    QWidget* topOfPageWidget = new QWidget();
    topOfPageWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    topOfPageWidget->setLayout(topOfPageLayout);
    topOfPageWidget->setContentsMargins(0, 0, 0, 0);

    QSplitter* verticalSplitter = new QSplitter(Qt::Vertical, scrollArea);
    verticalSplitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    verticalSplitter->addWidget(topOfPageWidget);
    verticalSplitter->addWidget(registersEditor_);
    verticalSplitter->addWidget(registerFilesEditor_);
    verticalSplitter->setStretchFactor(0, 4);
    verticalSplitter->setStretchFactor(1, 5);
    verticalSplitter->setStretchFactor(2, 5);

    for (int i = 1; i <= 2; ++i)
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
// Function: SingleAddressBlockEditor::connectSignals()
//-----------------------------------------------------------------------------
void SingleAddressBlockEditor::connectSignals() const
{
    connect(registersEditor_, SIGNAL(childAdded(int)), this, SIGNAL(childAdded(int)), Qt::UniqueConnection);
    connect(registersEditor_, SIGNAL(childRemoved(int)), this, SIGNAL(childRemoved(int)), Qt::UniqueConnection);
    connect(registersEditor_, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(registersEditor_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    connect(registersEditor_, SIGNAL(registerNameChanged(QString const&, QString const&)),
        this, SIGNAL(registerNameChanged(QString const&, QString const&)), Qt::UniqueConnection);

    connect(registerFilesEditor_, SIGNAL(childAdded(int)), this, SIGNAL(childAdded(int)), Qt::UniqueConnection);
    connect(registerFilesEditor_, SIGNAL(childRemoved(int)), this, SIGNAL(childRemoved(int)), Qt::UniqueConnection);
    connect(registerFilesEditor_, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(registerFilesEditor_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    connect(baseAddressEditor_, SIGNAL(increaseReference(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(baseAddressEditor_, SIGNAL(decreaseReference(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);
    connect(rangeEditor_, SIGNAL(increaseReference(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(rangeEditor_, SIGNAL(decreaseReference(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);
    connect(widthEditor_, SIGNAL(increaseReference(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(widthEditor_, SIGNAL(decreaseReference(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);
    connect(isPresentEditor_, SIGNAL(increaseReference(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(isPresentEditor_, SIGNAL(decreaseReference(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    connect(baseAddressEditor_, SIGNAL(editingFinished()), this, SLOT(onBaseAddressChanged()), Qt::UniqueConnection);    
    connect(rangeEditor_, SIGNAL(editingFinished()), this, SLOT(onRangeChanged()), Qt::UniqueConnection);
    connect(widthEditor_, SIGNAL(editingFinished()), this, SLOT(onWidthChanged()), Qt::UniqueConnection);

    connect(&nameEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(registersEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(registerFilesEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(baseAddressEditor_, SIGNAL(editingFinished()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(rangeEditor_, SIGNAL(editingFinished()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(widthEditor_, SIGNAL(editingFinished()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    connect(&nameEditor_, SIGNAL(nameChanged()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);
    connect(registersEditor_, SIGNAL(graphicsChanged(int)), this, SIGNAL(childGraphicsChanged(int)), Qt::UniqueConnection);
    connect(registersEditor_, SIGNAL(childAddressingChanged(int)), this, SIGNAL(childAddressingChanged(int)), Qt::UniqueConnection);    

    connect(registerFilesEditor_, SIGNAL(graphicsChanged(int)), this, SIGNAL(childGraphicsChanged(int)), Qt::UniqueConnection);
    connect(registerFilesEditor_, SIGNAL(childAddressingChanged(int)), this, SIGNAL(childAddressingChanged(int)), Qt::UniqueConnection);

    connect(baseAddressEditor_, SIGNAL(editingFinished()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);
    connect(baseAddressEditor_, SIGNAL(editingFinished()), this, SIGNAL(addressingChanged()), Qt::UniqueConnection);
    
    connect(rangeEditor_, SIGNAL(editingFinished()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);
    connect(rangeEditor_, SIGNAL(editingFinished()), this, SIGNAL(addressingChanged()), Qt::UniqueConnection);

    connect(widthEditor_, SIGNAL(editingFinished()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);
    connect(widthEditor_, SIGNAL(editingFinished()), this, SIGNAL(addressingChanged()), Qt::UniqueConnection);

    connect(isPresentEditor_, SIGNAL(editingFinished()), this, SLOT(onIsPresentEdited()), Qt::UniqueConnection);
    connect(isPresentEditor_, SIGNAL(editingFinished()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(isPresentEditor_, SIGNAL(editingFinished()), this, SIGNAL(addressingChanged()), Qt::UniqueConnection);

    connect(&nameEditor_, SIGNAL(nameChanged()), this, SLOT(onAddressBlockNameChanged()), Qt::UniqueConnection);

    connect(this, SIGNAL(addressUnitBitsChanged(int)),
        registersEditor_, SIGNAL(addressUnitBitsChanged(int)), Qt::UniqueConnection);
    connect(this, SIGNAL(addressUnitBitsChanged(int)),
        registerFilesEditor_, SIGNAL(addressUnitBitsChanged(int)), Qt::UniqueConnection);
    connect(registerFilesEditor_, SIGNAL(childAddressingChanged(int)), this, SIGNAL(childAddressingChanged(int)), Qt::UniqueConnection);

    connect(usageEditor_, SIGNAL(currentTextChanged(QString const&)),
        this, SLOT(onUsageSelected(QString const&)), Qt::UniqueConnection);

    connect(accessEditor_, SIGNAL(currentTextChanged(QString const&)),
        this, SLOT(onAccessSelected(QString const&)), Qt::UniqueConnection);

    connect(volatileEditor_, SIGNAL(currentTextChanged(QString const&)),
        this, SLOT(onVolatileSelected(QString const&)), Qt::UniqueConnection);

    connect(accessPoliciesEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: SingleAddressBlockEditor::changeExpressionEditorSignalBlocks()
//-----------------------------------------------------------------------------
void SingleAddressBlockEditor::changeExpressionEditorSignalBlockStatus(bool blockStatus)
{
    baseAddressEditor_->blockSignals(blockStatus);
    rangeEditor_->blockSignals(blockStatus);
    widthEditor_->blockSignals(blockStatus);
    isPresentEditor_->blockSignals(blockStatus);
}

//-----------------------------------------------------------------------------
// Function: SingleAddressBlockEditor::onAddressBlockNameChanged()
//-----------------------------------------------------------------------------
void SingleAddressBlockEditor::onAddressBlockNameChanged(QString const& oldName, QString const& newName)
{
    if (oldName == QString::fromStdString(blockName_))
    {
        blockName_ = newName.toStdString();
    }
}

//-----------------------------------------------------------------------------
// Function: SingleAddressBlockEditor::onAddressBlockNameChanged()
//-----------------------------------------------------------------------------
void SingleAddressBlockEditor::onAddressBlockNameChanged()
{
    blockName_ = nameEditor_.name().toStdString();
}
