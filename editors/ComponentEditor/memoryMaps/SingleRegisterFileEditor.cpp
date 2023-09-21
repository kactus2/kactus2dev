//-----------------------------------------------------------------------------
// File: SingleRegisterFileEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Dan Chianucci
// Date: 19.06.2018
//
// Description:
// Editor for a single register file item in component editor tree.
//-----------------------------------------------------------------------------

#include "SingleRegisterFileEditor.h"

#include <editors/ComponentEditor/common/ExpressionEditor.h>
#include <KactusAPI/include/ExpressionFormatter.h>
#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>
#include <editors/ComponentEditor/memoryMaps/addressblockeditor.h>
#include <editors/ComponentEditor/memoryMaps/AccessPoliciesEditor.h>

#include <KactusAPI/include/RegisterInterface.h>

#include <QCompleter>
#include <QFormLayout>
#include <QScrollArea>
#include <QSplitter>

//-----------------------------------------------------------------------------
// Function: SingleRegisterFileEditor::SingleRegisterFileEditor()
//-----------------------------------------------------------------------------
SingleRegisterFileEditor::SingleRegisterFileEditor(RegisterInterface* registerInterface,
    QSharedPointer<RegisterFile> registerFile, QSharedPointer<Component> component, LibraryInterface* handler,
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ExpressionFormatter> expressionFormatter,
    QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<RegisterFileValidator> registerFileValidator,
    QWidget* parent) :
    ItemEditor(component, handler, parent),
    registerFile_(registerFile),
    nameEditor_(registerFile, component->getRevision(), this, tr("Register file name and description")),
registersEditor_(new AddressBlockEditor(
    registerFile->getRegisterData(), registerInterface, component, handler, parameterFinder, this)),
registerFileEditor_(new RegisterFileEditor(registerFile_->getRegisterData(), component, handler,
    parameterFinder, expressionFormatter, registerFileValidator, this)),
accessPoliciesEditor_(new AccessPoliciesEditor(registerFile->getAccessPolicies(), 
    registerInterface->getAccessPolicyInterface(), this)),
offsetEditor_(new ExpressionEditor(parameterFinder, this)),
rangeEditor_(new ExpressionEditor(parameterFinder, this)),
dimensionEditor_(new ExpressionEditor(parameterFinder, this)),
isPresentEditor_(new ExpressionEditor(parameterFinder, this)),
expressionParser_(expressionParser),
registerFileValidator_(registerFileValidator)
{
    offsetEditor_->setFixedHeight(20);
    rangeEditor_->setFixedHeight(20);
    dimensionEditor_->setFixedHeight(20);
    isPresentEditor_->setFixedHeight(20);

    ComponentParameterModel* componentParametersModel = new ComponentParameterModel(parameterFinder, this);
    componentParametersModel->setExpressionParser(expressionParser_);

    auto offsetParameterCompleter = new QCompleter(this);
    offsetParameterCompleter->setModel(componentParametersModel);

    auto rangeParameterCompleter = new QCompleter(this);
    rangeParameterCompleter->setModel(componentParametersModel);

    auto dimensionParameterCompleter = new QCompleter(this);
    dimensionParameterCompleter->setModel(componentParametersModel);

    auto isPresentCompleter = new QCompleter(this);
    isPresentCompleter->setModel(componentParametersModel);

    offsetEditor_->setAppendingCompleter(offsetParameterCompleter);
    rangeEditor_->setAppendingCompleter(rangeParameterCompleter);
    dimensionEditor_->setAppendingCompleter(dimensionParameterCompleter);
    isPresentEditor_->setAppendingCompleter(isPresentCompleter);

    setupLayout();

    connectSignals();

    offsetEditor_->setProperty("mandatoryField", true);
    rangeEditor_->setProperty("mandatoryField", true);
}

//-----------------------------------------------------------------------------
// Function: SingleRegisterFileEditor::showEvent()
//-----------------------------------------------------------------------------
void SingleRegisterFileEditor::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    emit helpUrlRequested("componenteditor/addressblock.html");
}

//-----------------------------------------------------------------------------
// Function: SingleRegisterFileEditor::refresh()
//-----------------------------------------------------------------------------
void SingleRegisterFileEditor::refresh()
{
    nameEditor_.refresh();
    registerFileEditor_->refresh();

    registersEditor_->refresh();
    registerFileEditor_->refresh();

    accessPoliciesEditor_->refresh();

    changeExpressionEditorsSignalBlockStatus(true);

    offsetEditor_->setExpression(registerFile_->getAddressOffset());
    offsetEditor_->setToolTip(formattedValueFor(registerFile_->getAddressOffset()));

    rangeEditor_->setExpression(registerFile_->getRange());
    rangeEditor_->setToolTip(formattedValueFor(registerFile_->getRange()));

    dimensionEditor_->setExpression(registerFile_->getDimension());
    dimensionEditor_->setToolTip(formattedValueFor(registerFile_->getDimension()));

    isPresentEditor_->setExpression(registerFile_->getIsPresent());
    isPresentEditor_->setToolTip(formattedValueFor(registerFile_->getIsPresent()));

    changeExpressionEditorsSignalBlockStatus(false);
}

//-----------------------------------------------------------------------------
// Function: SingleRegisterFileEditor::onRangeChanged()
//-----------------------------------------------------------------------------
void SingleRegisterFileEditor::onRangeEdited()
{
    rangeEditor_->finishEditingCurrentWord();
    registerFile_->setRange(rangeEditor_->getExpression());

    rangeEditor_->setToolTip(formattedValueFor(registerFile_->getRange()));
}

//-----------------------------------------------------------------------------
// Function: SingleRegisterFileEditor::onDimensionEdited()
//-----------------------------------------------------------------------------
void SingleRegisterFileEditor::onDimensionEdited()
{
    dimensionEditor_->finishEditingCurrentWord();

    QString newDimension = dimensionEditor_->getExpression();
    registerFile_->setDimension(newDimension);

    dimensionEditor_->setToolTip(formattedValueFor(newDimension));
}

//-----------------------------------------------------------------------------
// Function: SingleRegisterFileEditor::onOffsetEdited()
//-----------------------------------------------------------------------------
void SingleRegisterFileEditor::onOffsetEdited()
{
    offsetEditor_->finishEditingCurrentWord();
    registerFile_->setAddressOffset(offsetEditor_->getExpression());

    QString formattedAddressOffset = formattedValueFor(registerFile_->getAddressOffset());
    offsetEditor_->setToolTip(formattedAddressOffset);
}

//-----------------------------------------------------------------------------
// Function: SingleRegisterEditor::onIsPresentEdited()
//-----------------------------------------------------------------------------
void SingleRegisterFileEditor::onIsPresentEdited()
{
    isPresentEditor_->finishEditingCurrentWord();

    QString newIsPresent = isPresentEditor_->getExpression();
    registerFile_->setIsPresent(newIsPresent);

    isPresentEditor_->setToolTip(formattedValueFor(newIsPresent));
}

//-----------------------------------------------------------------------------
// Function: SingleRegisterFileEditor::formattedValueFor()
//-----------------------------------------------------------------------------
QString SingleRegisterFileEditor::formattedValueFor(QString const& expression) const
{
    return ExpressionFormatter::format(expression, expressionParser_);
}

//-----------------------------------------------------------------------------
// Function: SingleRegisterFileEditor::setupLayout()
//-----------------------------------------------------------------------------
void SingleRegisterFileEditor::setupLayout()
{
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    QHBoxLayout* scrollLayout = new QHBoxLayout(this);
    scrollLayout->addWidget(scrollArea);
    scrollLayout->setContentsMargins(0, 0, 0, 0);

    QGroupBox* registerDefinitionGroup = new QGroupBox(tr("Register file definition"));

    QFormLayout* registerDefinitionLayout = new QFormLayout(registerDefinitionGroup);
    
    QLayout* topOfPageLayout;
    if (component()->getRevision() == Document::Revision::Std14)
    {
        registerDefinitionLayout->addRow(tr("Offset [AUB], f(x):"), offsetEditor_);
        registerDefinitionLayout->addRow(tr("Range [AUB], f(x):"), rangeEditor_);
        registerDefinitionLayout->addRow(tr("Dimension, f(x):"), dimensionEditor_);
        registerDefinitionLayout->addRow(tr("Is present, f(x):"), isPresentEditor_);

        QHBoxLayout* newTopOfPageLayout = new QHBoxLayout();
        newTopOfPageLayout->addWidget(&nameEditor_, 0);
        newTopOfPageLayout->addWidget(registerDefinitionGroup, 0);
        topOfPageLayout = newTopOfPageLayout;
    }
    else if (component()->getRevision() == Document::Revision::Std22)
    {
        registerDefinitionLayout->addRow(tr("Offset [AUB], f(x):"), offsetEditor_);
        registerDefinitionLayout->addRow(tr("Range [AUB], f(x):"), rangeEditor_);

        auto spacer = new QWidget();
        spacer->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        registerDefinitionLayout->addRow(spacer);

        QGridLayout* newTopOfPageLayout = new QGridLayout();
        newTopOfPageLayout->addWidget(&nameEditor_, 0, 0, 2, 1);
        newTopOfPageLayout->addWidget(registerDefinitionGroup, 0, 1);
        newTopOfPageLayout->addWidget(accessPoliciesEditor_, 1, 1);

        newTopOfPageLayout->setRowStretch(0, 1);
        newTopOfPageLayout->setRowStretch(1, 3);

        topOfPageLayout = newTopOfPageLayout;
    }

    QWidget* topOfPageWidget = new QWidget();
    topOfPageWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    topOfPageWidget->setLayout(topOfPageLayout);
    topOfPageWidget->setContentsMargins(0, 0, 0, 0);

    QSplitter* verticalSplitter = new QSplitter(Qt::Vertical, scrollArea);
    verticalSplitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    verticalSplitter->addWidget(topOfPageWidget);
    verticalSplitter->addWidget(registersEditor_);
    verticalSplitter->addWidget(registerFileEditor_);

    if (component()->getRevision() == Document::Revision::Std14)
    {
        verticalSplitter->setStretchFactor(0, 1);
        verticalSplitter->setStretchFactor(1, 2);
        verticalSplitter->setStretchFactor(2, 2);
    }
    else if (component()->getRevision() == Document::Revision::Std22)
    {
        verticalSplitter->setStretchFactor(0, 2);
        verticalSplitter->setStretchFactor(1, 3);
        verticalSplitter->setStretchFactor(2, 3);
    }

    for (int i = 1; i < 3; ++i)
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
// Function: SingleRegisterFileEditor::connectSignals()
//-----------------------------------------------------------------------------
void SingleRegisterFileEditor::connectSignals()
{
    connect(registersEditor_, SIGNAL(childAdded(int)), this, SIGNAL(childAdded(int)), Qt::UniqueConnection);
    connect(registersEditor_, SIGNAL(childRemoved(int)), this, SIGNAL(childRemoved(int)), Qt::UniqueConnection);
    connect(registersEditor_, SIGNAL(increaseReferences(QString const&)),
        this, SIGNAL(increaseReferences(QString const&)), Qt::UniqueConnection);
    connect(registersEditor_, SIGNAL(decreaseReferences(QString const&)),
        this, SIGNAL(decreaseReferences(QString const&)), Qt::UniqueConnection);

    connect(registerFileEditor_, SIGNAL(childAdded(int)), this, SIGNAL(childAdded(int)), Qt::UniqueConnection);
    connect(registerFileEditor_, SIGNAL(childRemoved(int)), this, SIGNAL(childRemoved(int)), Qt::UniqueConnection);
    connect(registerFileEditor_, SIGNAL(increaseReferences(QString const&)),
        this, SIGNAL(increaseReferences(QString const&)), Qt::UniqueConnection);
    connect(registerFileEditor_, SIGNAL(decreaseReferences(QString const&)),
        this, SIGNAL(decreaseReferences(QString const&)), Qt::UniqueConnection);

    connect(offsetEditor_, SIGNAL(increaseReference(QString const&)),
        this, SIGNAL(increaseReferences(QString const&)), Qt::UniqueConnection);
    connect(offsetEditor_, SIGNAL(decreaseReference(QString const&)),
        this, SIGNAL(decreaseReferences(QString const&)), Qt::UniqueConnection);
    connect(rangeEditor_, SIGNAL(increaseReference(QString const&)),
        this, SIGNAL(increaseReferences(QString const&)), Qt::UniqueConnection);
    connect(rangeEditor_, SIGNAL(decreaseReference(QString const&)),
        this, SIGNAL(decreaseReferences(QString const&)), Qt::UniqueConnection);
    connect(dimensionEditor_, SIGNAL(increaseReference(QString const&)),
        this, SIGNAL(increaseReferences(QString const&)), Qt::UniqueConnection);
    connect(dimensionEditor_, SIGNAL(decreaseReference(QString const&)),
        this, SIGNAL(decreaseReferences(QString const&)), Qt::UniqueConnection);
    connect(isPresentEditor_, SIGNAL(increaseReference(QString const&)),
        this, SIGNAL(increaseReferences(QString const&)), Qt::UniqueConnection);
    connect(isPresentEditor_, SIGNAL(decreaseReference(QString const&)),
        this, SIGNAL(decreaseReferences(QString const&)), Qt::UniqueConnection);

    connect(offsetEditor_, SIGNAL(editingFinished()), this, SLOT(onOffsetEdited()), Qt::UniqueConnection);
    connect(rangeEditor_, SIGNAL(editingFinished()), this, SLOT(onRangeEdited()), Qt::UniqueConnection);
    connect(dimensionEditor_, SIGNAL(editingFinished()), this, SLOT(onDimensionEdited()), Qt::UniqueConnection);
    connect(isPresentEditor_, SIGNAL(editingFinished()), this, SLOT(onIsPresentEdited()), Qt::UniqueConnection);

    connect(&nameEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(offsetEditor_, SIGNAL(editingFinished()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(rangeEditor_, SIGNAL(editingFinished()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(dimensionEditor_, SIGNAL(editingFinished()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(isPresentEditor_, SIGNAL(editingFinished()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(registersEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(registerFileEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    connect(&nameEditor_, SIGNAL(nameChanged()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);
    connect(offsetEditor_, SIGNAL(editingFinished()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);
    connect(rangeEditor_, SIGNAL(editingFinished()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);
    connect(dimensionEditor_, SIGNAL(editingFinished()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);
    connect(isPresentEditor_, SIGNAL(editingFinished()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);
    connect(registersEditor_, SIGNAL(graphicsChanged(int)), this, SIGNAL(childGraphicsChanged(int)), Qt::UniqueConnection);
    connect(registerFileEditor_, SIGNAL(graphicsChanged(int)), this, SIGNAL(childGraphicsChanged(int)), Qt::UniqueConnection);

    connect(offsetEditor_, SIGNAL(editingFinished()), this, SIGNAL(addressingChanged()), Qt::UniqueConnection);
    connect(rangeEditor_, SIGNAL(editingFinished()), this, SIGNAL(addressingChanged()), Qt::UniqueConnection);
    connect(dimensionEditor_, SIGNAL(editingFinished()), this, SIGNAL(addressingChanged()), Qt::UniqueConnection);
    connect(isPresentEditor_, SIGNAL(editingFinished()), this, SIGNAL(addressingChanged()), Qt::UniqueConnection);
    
    connect(registersEditor_, SIGNAL(childAddressingChanged(int)),
        this, SIGNAL(childAddressingChanged(int)), Qt::UniqueConnection);
    connect(registerFileEditor_, SIGNAL(childAddressingChanged(int)),
        this, SIGNAL(childAddressingChanged(int)), Qt::UniqueConnection);

    connect(registersEditor_, SIGNAL(registerNameChanged(QString const&, QString const&)),
        this, SIGNAL(registerNameChanged(QString const&, QString const&)), Qt::UniqueConnection);

    connect(accessPoliciesEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: SingleRegisterFileEditor::changeExpressionEditorSignalBlocks()
//-----------------------------------------------------------------------------
void SingleRegisterFileEditor::changeExpressionEditorsSignalBlockStatus(bool blockStatus) const
{
    offsetEditor_->blockSignals(blockStatus);
    rangeEditor_->blockSignals(blockStatus);
    dimensionEditor_->blockSignals(blockStatus);
    isPresentEditor_->blockSignals(blockStatus);
}
