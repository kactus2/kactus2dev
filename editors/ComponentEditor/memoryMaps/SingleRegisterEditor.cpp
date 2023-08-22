//-----------------------------------------------------------------------------
// File: SingleRegisterEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 24.04.2015
//
// Description:
// Editor for editing the details of a single register.
//-----------------------------------------------------------------------------

#include "SingleRegisterEditor.h"

#include <common/widgets/accessComboBox/accesscombobox.h>
#include <common/widgets/booleanComboBox/booleancombobox.h>

#include <KactusAPI/include/LibraryInterface.h>

#include <editors/ComponentEditor/common/ExpressionEditor.h>
#include <KactusAPI/include/ExpressionFormatter.h>
#include <KactusAPI/include/ExpressionParser.h>
#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>
#include <KactusAPI/include/RegisterInterface.h>

#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/AddressBlock.h>
#include <IPXACTmodels/Component/validators/RegisterValidator.h>

#include <QCompleter>
#include <QFormLayout>
#include <QScrollArea>
#include <QSplitter>

//-----------------------------------------------------------------------------
// Function: SingleRegisterEditor::SingleRegisterEditor()
//-----------------------------------------------------------------------------
SingleRegisterEditor::SingleRegisterEditor(QSharedPointer<Register> selectedRegister,
    QSharedPointer<Component> component, LibraryInterface* handler,
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ExpressionParser> expressionParser,
    RegisterInterface* registerInterface,
    QSharedPointer<QList<QSharedPointer<RegisterBase>>> containingRegisterData, QWidget* parent):
ItemEditor(component, handler, parent),
registerName_(selectedRegister->name().toStdString()),
nameEditor_(selectedRegister, component->getRevision(), this, tr("Register name and description")),
fieldsEditor_(new RegisterEditor(selectedRegister->getFields(), component, handler, parameterFinder,
    registerInterface->getSubInterface(), this)),
offsetEditor_(new ExpressionEditor(parameterFinder, this)),
sizeEditor_(new ExpressionEditor(parameterFinder, this)),
dimensionEditor_(new ExpressionEditor(parameterFinder, this)),
isPresentEditor_(new ExpressionEditor(parameterFinder, this)),
volatileEditor_(),
accessEditor_(),
expressionParser_(expressionParser),
containingRegisterData_(containingRegisterData),
registerInterface_(registerInterface)
{
    registerInterface_->setRegisters(containingRegisterData_);

    offsetEditor_->setFixedHeight(20);
    sizeEditor_->setFixedHeight(20);
    dimensionEditor_->setFixedHeight(20);
    isPresentEditor_->setFixedHeight(20);

    ComponentParameterModel* componentParametersModel = new ComponentParameterModel(parameterFinder, this);
    componentParametersModel->setExpressionParser(expressionParser_);

    auto offsetParameterCompleter = new QCompleter(this);
    offsetParameterCompleter->setModel(componentParametersModel);

    auto sizeParameterCompleter = new QCompleter(this);
    sizeParameterCompleter->setModel(componentParametersModel);

    auto dimensionParameterCompleter = new QCompleter(this);
    dimensionParameterCompleter->setModel(componentParametersModel);

    auto isPresentCompleter = new QCompleter(this);
    isPresentCompleter->setModel(componentParametersModel);

    offsetEditor_->setAppendingCompleter(offsetParameterCompleter);
    sizeEditor_->setAppendingCompleter(sizeParameterCompleter);
    dimensionEditor_->setAppendingCompleter(dimensionParameterCompleter);
    isPresentEditor_->setAppendingCompleter(isPresentCompleter);

    setupLayout();

    connectSignals();

    offsetEditor_->setProperty("mandatoryField", true);
    sizeEditor_->setProperty("mandatoryField", true);
}

//-----------------------------------------------------------------------------
// Function: SingleRegisterEditor::setupLayout()
//-----------------------------------------------------------------------------
void SingleRegisterEditor::setupLayout()
{
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    QHBoxLayout* scrollLayout = new QHBoxLayout(this);
    scrollLayout->addWidget(scrollArea);
    scrollLayout->setContentsMargins(0, 0, 0, 0);

    QGroupBox* registerDefinitionGroup = new QGroupBox(tr("Register definition"));

    QFormLayout* registerDefinitionLayout = new QFormLayout(registerDefinitionGroup);
    registerDefinitionLayout->addRow(tr("Offset [AUB], f(x):"), offsetEditor_);
    registerDefinitionLayout->addRow(tr("Size [bits], f(x):"), sizeEditor_);
    registerDefinitionLayout->addRow(tr("Dimension, f(x):"), dimensionEditor_);
    registerDefinitionLayout->addRow(tr("Is present, f(x):"), isPresentEditor_);

    volatileEditor_ = new BooleanComboBox(registerDefinitionGroup);
    registerDefinitionLayout->addRow(tr("Volatile:"), volatileEditor_);

    connect(volatileEditor_, SIGNAL(currentTextChanged(QString const&)),
        this, SLOT(onVolatileSelected(QString const&)), Qt::UniqueConnection);

    accessEditor_ = new AccessComboBox(registerDefinitionGroup);
    registerDefinitionLayout->addRow(tr("Access:"), accessEditor_);

    connect(accessEditor_, SIGNAL(currentTextChanged(QString const&)),
        this, SLOT(onAccessSelected(QString const&)), Qt::UniqueConnection);

    QHBoxLayout* topOfPageLayout = new QHBoxLayout();
    topOfPageLayout->addWidget(&nameEditor_, 0);
    topOfPageLayout->addWidget(registerDefinitionGroup, 0);

    QWidget* topOfPageWidget = new QWidget();
    topOfPageWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    topOfPageWidget->setLayout(topOfPageLayout);
    topOfPageWidget->setContentsMargins(0, 0, 0, 0);

    QSplitter* verticalSplitter = new QSplitter(Qt::Vertical, scrollArea);
    verticalSplitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    verticalSplitter->addWidget(topOfPageWidget);
    verticalSplitter->addWidget(fieldsEditor_);
    verticalSplitter->setStretchFactor(1, 1);

    QSplitterHandle* handle = verticalSplitter->handle(1);
    QVBoxLayout* handleLayout = new QVBoxLayout(handle);
    handleLayout->setSpacing(0);
    handleLayout->setContentsMargins(0, 0, 0, 0);

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
// Function: SingleRegisterEditor::refresh()
//-----------------------------------------------------------------------------
void SingleRegisterEditor::refresh()
{
    registerInterface_->setRegisters(containingRegisterData_);

    nameEditor_.refresh();
    fieldsEditor_->refresh();

    changeExpressionEditorsSignalBlockStatus(true);
    
    offsetEditor_->setExpression(QString::fromStdString(registerInterface_->getOffsetExpression(registerName_)));
    offsetEditor_->setToolTip(
        QString::fromStdString(registerInterface_->getOffsetValue(registerName_)));

    sizeEditor_->setExpression(QString::fromStdString(registerInterface_->getSizeExpression(registerName_)));
    sizeEditor_->setToolTip(QString::fromStdString(registerInterface_->getSizeValue(registerName_)));

    dimensionEditor_->setExpression(
        QString::fromStdString(registerInterface_->getDimensionExpression(registerName_)));
    dimensionEditor_->setToolTip(
        QString::fromStdString(registerInterface_->getDimensionValue(registerName_)));

    isPresentEditor_->setExpression(
        QString::fromStdString(registerInterface_->getIsPresentExpression(registerName_)));
    isPresentEditor_->setToolTip(
        QString::fromStdString(registerInterface_->getIsPresentValue(registerName_)));

    changeExpressionEditorsSignalBlockStatus(false);

    accessEditor_->setCurrentValue(registerInterface_->getAccess(registerName_));
    volatileEditor_->setCurrentValue(QString::fromStdString(registerInterface_->getVolatile(registerName_)));
}

//-----------------------------------------------------------------------------
// Function: SingleRegisterEditor::connectSignals()
//-----------------------------------------------------------------------------
void SingleRegisterEditor::connectSignals()
{
    connect(fieldsEditor_, SIGNAL(childAdded(int)), this, SIGNAL(childAdded(int)), Qt::UniqueConnection);
    connect(fieldsEditor_, SIGNAL(childRemoved(int)), this, SIGNAL(childRemoved(int)), Qt::UniqueConnection);
    connect(fieldsEditor_, SIGNAL(increaseReferences(QString const&)),
        this, SIGNAL(increaseReferences(QString const&)), Qt::UniqueConnection);
    connect(fieldsEditor_, SIGNAL(decreaseReferences(QString const&)),
        this, SIGNAL(decreaseReferences(QString const&)), Qt::UniqueConnection);

    connect(offsetEditor_, SIGNAL(increaseReference(QString const&)),
        this, SIGNAL(increaseReferences(QString const&)), Qt::UniqueConnection);
    connect(offsetEditor_, SIGNAL(decreaseReference(QString const&)),
        this, SIGNAL(decreaseReferences(QString const&)), Qt::UniqueConnection);
    connect(sizeEditor_, SIGNAL(increaseReference(QString const&)),
        this, SIGNAL(increaseReferences(QString const&)), Qt::UniqueConnection);
    connect(sizeEditor_, SIGNAL(decreaseReference(QString const&)),
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
    connect(sizeEditor_, SIGNAL(editingFinished()), this, SLOT(onSizeEdited()), Qt::UniqueConnection);
    connect(dimensionEditor_, SIGNAL(editingFinished()), this, SLOT(onDimensionEdited()), Qt::UniqueConnection);
    connect(isPresentEditor_, SIGNAL(editingFinished()), this, SLOT(onIsPresentEdited()), Qt::UniqueConnection);

    connect(&nameEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(offsetEditor_, SIGNAL(editingFinished()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(sizeEditor_, SIGNAL(editingFinished()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(dimensionEditor_, SIGNAL(editingFinished()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(isPresentEditor_, SIGNAL(editingFinished()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(fieldsEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    connect(&nameEditor_, SIGNAL(nameChanged()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);
    connect(offsetEditor_, SIGNAL(editingFinished()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);
    connect(offsetEditor_, SIGNAL(editingFinished()), this, SIGNAL(addressingChanged()), Qt::UniqueConnection);
    
    connect(sizeEditor_, SIGNAL(editingFinished()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);
    connect(sizeEditor_, SIGNAL(editingFinished()), this, SIGNAL(addressingChanged()), Qt::UniqueConnection);

    connect(dimensionEditor_, SIGNAL(editingFinished()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);
    connect(dimensionEditor_, SIGNAL(editingFinished()), this, SIGNAL(addressingChanged()), Qt::UniqueConnection);

    //connect(isPresentEditor_, SIGNAL(editingFinished()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);
    connect(isPresentEditor_, SIGNAL(editingFinished()), this, SIGNAL(addressingChanged()), Qt::UniqueConnection);

    connect(fieldsEditor_, SIGNAL(graphicsChanged(int)), 
        this, SIGNAL(childGraphicsChanged(int)), Qt::UniqueConnection);
    connect(fieldsEditor_, SIGNAL(addressingChanged(int)), 
        this, SIGNAL(childAddressingChanged(int)), Qt::UniqueConnection);

    connect(&nameEditor_, SIGNAL(nameChanged()), this, SLOT(onRegisterNameChanged()), Qt::UniqueConnection);

    connect(fieldsEditor_, SIGNAL(fieldNameChanged(QString const&, QString const&)),
        this, SIGNAL(fieldNameChanged(QString const&, QString const&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: SingleRegisterEditor::formattedValueFor()
//-----------------------------------------------------------------------------
QString SingleRegisterEditor::formattedValueFor(QString const& expression) const
{
    return ExpressionFormatter::format(expression, expressionParser_);
}

//-----------------------------------------------------------------------------
// Function: SingleRegisterEditor::changeExpressionEditorsSignalBlockStatus()
//-----------------------------------------------------------------------------
void SingleRegisterEditor::changeExpressionEditorsSignalBlockStatus(bool blockStatus) const
{
    offsetEditor_->blockSignals(blockStatus);
    sizeEditor_->blockSignals(blockStatus);
    dimensionEditor_->blockSignals(blockStatus);
    isPresentEditor_->blockSignals(blockStatus);
}

//-----------------------------------------------------------------------------
// Function: SingleRegisterEditor::onOffsetEdited()
//-----------------------------------------------------------------------------
void SingleRegisterEditor::onOffsetEdited()
{
    offsetEditor_->finishEditingCurrentWord();
    QString newAddressOffset = offsetEditor_->getExpression();

    registerInterface_->setOffset(registerName_, newAddressOffset.toStdString());
    offsetEditor_->setToolTip(formattedValueFor(newAddressOffset));
}

//-----------------------------------------------------------------------------
// Function: SingleRegisterEditor::onSizeEdited()
//-----------------------------------------------------------------------------
void SingleRegisterEditor::onSizeEdited()
{
    sizeEditor_->finishEditingCurrentWord();
    QString newSize = sizeEditor_->getExpression();

    registerInterface_->setSize(registerName_, newSize.toStdString());
    sizeEditor_->setToolTip(formattedValueFor(newSize));
}

//-----------------------------------------------------------------------------
// Function: SingleRegisterEditor::onDimensionEdited()
//-----------------------------------------------------------------------------
void SingleRegisterEditor::onDimensionEdited()
{
    dimensionEditor_->finishEditingCurrentWord();
    QString newDimension = dimensionEditor_->getExpression();

    registerInterface_->setDimension(registerName_, newDimension.toStdString());
    dimensionEditor_->setToolTip(formattedValueFor(newDimension));
}

//-----------------------------------------------------------------------------
// Function: SingleRegisterEditor::onIsPresentEdited()
//-----------------------------------------------------------------------------
void SingleRegisterEditor::onIsPresentEdited()
{
    isPresentEditor_->finishEditingCurrentWord();
    QString newIsPresent = isPresentEditor_->getExpression();

    registerInterface_->setIsPresent(registerName_, newIsPresent.toStdString());
    isPresentEditor_->setToolTip(formattedValueFor(newIsPresent));
}

//-----------------------------------------------------------------------------
// Function: SingleRegisterEditor::onVolatileSelected()
//-----------------------------------------------------------------------------
void SingleRegisterEditor::onVolatileSelected(QString const& newVolatileValue)
{
    registerInterface_->setVolatile(registerName_, newVolatileValue.toStdString());

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: SingleRegisterEditor::onAccessSelected()
//-----------------------------------------------------------------------------
void SingleRegisterEditor::onAccessSelected(QString const& newAccessValue)
{
    registerInterface_->setAccess(registerName_, newAccessValue.toStdString());

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: SingleRegisterEditor::showEvent()
//-----------------------------------------------------------------------------
void SingleRegisterEditor::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    emit helpUrlRequested("componenteditor/register.html");
}

//-----------------------------------------------------------------------------
// Function: SingleRegisterEditor::onRegisterNameChanged()
//-----------------------------------------------------------------------------
void SingleRegisterEditor::onRegisterNameChanged(QString const& oldName, QString const& newName)
{
    if (oldName == QString::fromStdString(registerName_))
    {
        registerName_ = newName.toStdString();
    }
}

//-----------------------------------------------------------------------------
// Function: SingleRegisterEditor::onRegisterNameChanged()
//-----------------------------------------------------------------------------
void SingleRegisterEditor::onRegisterNameChanged()
{
    registerName_ = nameEditor_.name().toStdString();
}
