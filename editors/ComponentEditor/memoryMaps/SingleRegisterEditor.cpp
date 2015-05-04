//-----------------------------------------------------------------------------
// File: SingleRegisterEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 24.04.2015
//
// Description:
// Editor for editing the details of a single register.
//-----------------------------------------------------------------------------

#include "SingleRegisterEditor.h"

#include <common/widgets/accessComboBox/accesscombobox.h>
#include <common/widgets/booleanComboBox/booleancombobox.h>

#include <library/LibraryManager/libraryinterface.h>

#include <editors/ComponentEditor/common/ExpressionEditor.h>
#include <editors/ComponentEditor/common/ParameterCompleter.h>
#include <editors/ComponentEditor/common/ValueFormatter.h>
#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>

#include <IPXACTmodels/validators/BinaryValidator.h>

#include <QFormLayout>
#include <QScrollArea>
#include <QSplitter>

//-----------------------------------------------------------------------------
// Function: SingleRegisterEditor::SingleRegisterEditor()
//-----------------------------------------------------------------------------
SingleRegisterEditor::SingleRegisterEditor(QSharedPointer<Register> selectedRegister,
    QSharedPointer<Component> component, LibraryInterface* handler,
    QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionFormatter> expressionFormatter, QWidget* parent /* = 0 */):
ItemEditor(component, handler, parent),
selectedRegister_(selectedRegister),
nameEditor_(selectedRegister->getNameGroup(), this, tr("Register name and description")),
fieldsEditor_(new RegisterEditor(selectedRegister, component, handler, parameterFinder, expressionFormatter, this)),
offsetEditor_(new ExpressionEditor(parameterFinder, this)),
sizeEditor_(new ExpressionEditor(parameterFinder, this)),
dimensionEditor_(new ExpressionEditor(parameterFinder, this)),
volatileEditor_(),
accessEditor_(),
resetValueEditor_(new QLineEdit(this)),
resetMaskEditor_(new QLineEdit(this)),
expressionParser_(new IPXactSystemVerilogParser(parameterFinder)),
resetValueValidator_(),
resetMaskValidator_()
{
    offsetEditor_->setFixedHeight(20);
    sizeEditor_->setFixedHeight(20);
    dimensionEditor_->setFixedHeight(20);

    ComponentParameterModel* componentParametersModel = new ComponentParameterModel(parameterFinder, this);
    componentParametersModel->setExpressionParser(expressionParser_);

    ParameterCompleter* offsetParameterCompleter = new ParameterCompleter(this);
    offsetParameterCompleter->setModel(componentParametersModel);

    ParameterCompleter* sizeParameterCompleter = new ParameterCompleter(this);
    sizeParameterCompleter->setModel(componentParametersModel);

    ParameterCompleter* dimensionParameterCompleter = new ParameterCompleter(this);
    dimensionParameterCompleter->setModel(componentParametersModel);

    offsetEditor_->setAppendingCompleter(offsetParameterCompleter);
    sizeEditor_->setAppendingCompleter(sizeParameterCompleter);
    dimensionEditor_->setAppendingCompleter(dimensionParameterCompleter);

    resetValueValidator_ = new BinaryValidator(formattedValueFor(selectedRegister_->getSizeExpression()), resetValueEditor_);
    resetMaskValidator_= new BinaryValidator(formattedValueFor(selectedRegister_->getSizeExpression()), resetMaskEditor_);

    resetValueEditor_->setValidator(resetValueValidator_);
    resetMaskEditor_->setValidator(resetMaskValidator_);

    setupLayout();

    connectSignals();

    offsetEditor_->setProperty("mandatoryField", true);
    sizeEditor_->setProperty("mandatoryField", true);
}

//-----------------------------------------------------------------------------
// Function: SingleRegisterEditor::~SingleRegisterEditor()
//-----------------------------------------------------------------------------
SingleRegisterEditor::~SingleRegisterEditor()
{

}

//-----------------------------------------------------------------------------
// Function: SingleRegisterEditor::isValid()
//-----------------------------------------------------------------------------
bool SingleRegisterEditor::isValid() const
{
    bool isNameEditorValid = nameEditor_.isValid();
    bool isRegisterValid = selectedRegister_->isValid(component()->getChoices());

    return isNameEditorValid && isRegisterValid;
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

    offsetEditor_->setFrameShadow(QFrame::Sunken);
    sizeEditor_->setFrameShadow(QFrame::Sunken);
    dimensionEditor_->setFrameShadow(QFrame::Sunken);

    QFormLayout* registerDefinitionLayout = new QFormLayout(registerDefinitionGroup);
    registerDefinitionLayout->addRow(tr("Offset [AUB], f(x):"), offsetEditor_);
    registerDefinitionLayout->addRow(tr("Size [bits], f(x):"), sizeEditor_);
    registerDefinitionLayout->addRow(tr("Dimension, f(x):"), dimensionEditor_);

    volatileEditor_ = new BooleanComboBox(registerDefinitionGroup);
    registerDefinitionLayout->addRow(tr("Volatile:"), volatileEditor_);

    connect(volatileEditor_, SIGNAL(activated(QString const&)),
        this, SLOT(onVolatileSelected(QString const&)), Qt::UniqueConnection);

    accessEditor_ = new AccessComboBox(registerDefinitionGroup);
    registerDefinitionLayout->addRow(tr("Access:"), accessEditor_);

    connect(accessEditor_, SIGNAL(activated(QString const&)),
        this, SLOT(onAccessSelected(QString const&)), Qt::UniqueConnection);

    registerDefinitionLayout->addRow(tr("Reset value:"), resetValueEditor_);
    registerDefinitionLayout->addRow(tr("Reset mask:"), resetMaskEditor_);

    QHBoxLayout* topOfPageLayout = new QHBoxLayout();
    topOfPageLayout->addWidget(&nameEditor_, 0, Qt::AlignTop);
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
// Function: SingleRegisterEditor::refresh()
//-----------------------------------------------------------------------------
void SingleRegisterEditor::refresh()
{
    nameEditor_.refresh();
    fieldsEditor_->refresh();

    changeExpressionEditorsSignalBlockStatus(true);
    
    offsetEditor_->setExpression(selectedRegister_->getAddressOffset());
    offsetEditor_->setToolTip(formattedValueFor(selectedRegister_->getAddressOffset()));

    sizeEditor_->setExpression(selectedRegister_->getSizeExpression());
    sizeEditor_->setToolTip(formattedValueFor(selectedRegister_->getSizeExpression()));

    dimensionEditor_->setExpression(selectedRegister_->getDimensionExpression());
    dimensionEditor_->setToolTip(formattedValueFor(selectedRegister_->getDimensionExpression()));

    changeExpressionEditorsSignalBlockStatus(false);

    accessEditor_->setCurrentValue(selectedRegister_->getAccess());
    volatileEditor_->setCurrentValue(General::BooleanValue2Bool(selectedRegister_->getVolatile(), true));

    resetValueEditor_->setText(selectedRegister_->getRegisterValue());
    resetMaskEditor_->setText(selectedRegister_->getRegisterMask());

    resetMaskValidator_->setNewExpressionvalidator(formattedValueFor(selectedRegister_->getSizeExpression()));
    resetValueValidator_->setNewExpressionvalidator(formattedValueFor(selectedRegister_->getSizeExpression()));

    setResetValueColours();
    setResetMaskColours();
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

    connect(offsetEditor_, SIGNAL(editingFinished()), this, SLOT(onOffsetEdited()), Qt::UniqueConnection);
    connect(sizeEditor_, SIGNAL(editingFinished()), this, SLOT(onSizeEdited()), Qt::UniqueConnection);
    connect(dimensionEditor_, SIGNAL(editingFinished()), this, SLOT(onDimensionEdited()), Qt::UniqueConnection);

    connect(resetValueEditor_, SIGNAL(textEdited(QString const&)),
        this, SLOT(onResetValueChanged(QString const&)), Qt::UniqueConnection);
    connect(resetMaskEditor_, SIGNAL(textEdited(QString const&)),
        this, SLOT(onResetMaskChanged(QString const&)), Qt::UniqueConnection);

    connect(&nameEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(fieldsEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(offsetEditor_, SIGNAL(editingFinished()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(sizeEditor_, SIGNAL(editingFinished()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(dimensionEditor_, SIGNAL(editingFinished()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    connect(offsetEditor_, SIGNAL(editingFinished()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);
    connect(sizeEditor_, SIGNAL(editingFinished()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);
    connect(dimensionEditor_, SIGNAL(editingFinished()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);
    connect(fieldsEditor_, SIGNAL(graphicsChanged()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);
    connect(&nameEditor_, SIGNAL(nameChanged()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: SingleRegisterEditor::formattedValueFor()
//-----------------------------------------------------------------------------
QString SingleRegisterEditor::formattedValueFor(QString const& expression) const
{
    if (expressionParser_->isValidExpression(expression))
    {
        ValueFormatter formatter;
        return formatter.format(expressionParser_->parseExpression(expression),
            expressionParser_->baseForExpression(expression));
    }
    else if (expressionParser_->isPlainValue(expression))
    {
        return expression;
    }
    else
    {
        return "n/a";
    }
}

//-----------------------------------------------------------------------------
// Function: SingleRegisterEditor::changeExpressionEditorsSignalBlockStatus()
//-----------------------------------------------------------------------------
void SingleRegisterEditor::changeExpressionEditorsSignalBlockStatus(bool blockStatus) const
{
    offsetEditor_->blockSignals(blockStatus);
    sizeEditor_->blockSignals(blockStatus);
    dimensionEditor_->blockSignals(blockStatus);
}

//-----------------------------------------------------------------------------
// Function: SingleRegisterEditor::onOffsetEdited()
//-----------------------------------------------------------------------------
void SingleRegisterEditor::onOffsetEdited()
{
    offsetEditor_->finishEditingCurrentWord();
    selectedRegister_->setAddressOffset(offsetEditor_->getExpression());

    QString formattedAddressOffset = formattedValueFor(selectedRegister_->getAddressOffset());
    offsetEditor_->setToolTip(formattedAddressOffset);
}

//-----------------------------------------------------------------------------
// Function: SingleRegisterEditor::onSizeEdited()
//-----------------------------------------------------------------------------
void SingleRegisterEditor::onSizeEdited()
{
    sizeEditor_->finishEditingCurrentWord();
    selectedRegister_->setSizeExpression(sizeEditor_->getExpression());

    QString formattedSize = formattedValueFor(selectedRegister_->getSizeExpression());
    selectedRegister_->setSize(formattedSize.toInt());

    resetMaskValidator_->setNewExpressionvalidator(formattedValueFor(selectedRegister_->getSizeExpression()));
    resetValueValidator_->setNewExpressionvalidator(formattedValueFor(selectedRegister_->getSizeExpression()));

    setResetValueColours();
    setResetMaskColours();

    sizeEditor_->setToolTip(formattedSize);
}

//-----------------------------------------------------------------------------
// Function: SingleRegisterEditor::onDimensionEdited()
//-----------------------------------------------------------------------------
void SingleRegisterEditor::onDimensionEdited()
{
    dimensionEditor_->finishEditingCurrentWord();
    selectedRegister_->setDimensionExpression(dimensionEditor_->getExpression());

    QString formattedDimension = formattedValueFor(selectedRegister_->getDimensionExpression());
    selectedRegister_->setDim(formattedDimension.toInt());

    dimensionEditor_->setToolTip(formattedDimension);
}

//-----------------------------------------------------------------------------
// Function: SingleRegisterEditor::onVolatileSelected()
//-----------------------------------------------------------------------------
void SingleRegisterEditor::onVolatileSelected(QString const& newVolatileValue)
{
    bool registerIsVolatile = false;
    if (newVolatileValue == "true")
    {
        registerIsVolatile = true;
    }

    selectedRegister_->setVolatile(General::bool2BooleanValue(registerIsVolatile));

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: SingleRegisterEditor::onAccessSelected()
//-----------------------------------------------------------------------------
void SingleRegisterEditor::onAccessSelected(QString const& newAccessValue)
{
    selectedRegister_->setAccess(General::str2Access(newAccessValue, General::ACCESS_COUNT));

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: SingleRegisterEditor::onResetValueChanged()
//-----------------------------------------------------------------------------
void SingleRegisterEditor::onResetValueChanged(QString const& newResetValue)
{
    selectedRegister_->setRegisterValue(newResetValue);

    setResetValueColours();

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: SingleRegisterEditor::onResetMaskChanged()
//-----------------------------------------------------------------------------
void SingleRegisterEditor::onResetMaskChanged(QString const& newResetMask)
{
    selectedRegister_->setRegisterMask(newResetMask);

    setResetMaskColours();

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: SingleRegisterEditor::isResetValid()
//-----------------------------------------------------------------------------
bool SingleRegisterEditor::isResetValid(QString const& resetValue) const
{
    QRegularExpression resetExpression("(?<size>[1-9]?[0-9]*)'(?<bitChar>[bB])(?<bits>[01_]+)");
    QRegularExpressionMatch match = resetExpression.match(resetValue);

    if (match.hasMatch())
    {
        QString sizeOfBits = match.captured("size");
        QString bitChar = match.captured("bitChar");
        QString bits = match.captured("bits");
        bits.remove('_');

        if (bits.size() == selectedRegister_->getSize() && (sizeOfBits.isEmpty() ||
            sizeOfBits.toInt() == selectedRegister_->getSize()))
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: SingleRegisterEditor::setResetFontColours()
//-----------------------------------------------------------------------------
void SingleRegisterEditor::setResetValueColours() const
{
    QPalette* palette = new QPalette();

    if (isResetValid(selectedRegister_->getRegisterValue()))
    {
        palette->setColor(QPalette::Text, Qt::black);
        resetValueEditor_->setPalette(*palette);
    }
    else
    {
        palette->setColor(QPalette::Text, Qt::red);
        resetValueEditor_->setPalette(*palette);
    }
}

//-----------------------------------------------------------------------------
// Function: SingleRegisterEditor::setResetMaskColours()
//-----------------------------------------------------------------------------
void SingleRegisterEditor::setResetMaskColours() const
{
    QPalette* palette = new QPalette();

    if (isResetValid(selectedRegister_->getRegisterMask()))
    {
        palette->setColor(QPalette::Text, Qt::black);
        resetMaskEditor_->setPalette(*palette);
    }
    else
    {
        palette->setColor(QPalette::Text, Qt::red);
        resetMaskEditor_->setPalette(*palette);
    }
}

//-----------------------------------------------------------------------------
// Function: SingleRegisterEditor::showEvent()
//-----------------------------------------------------------------------------
void SingleRegisterEditor::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    emit helpUrlRequested("componenteditor/register.html");
}
