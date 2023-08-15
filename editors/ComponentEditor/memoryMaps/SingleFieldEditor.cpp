//-----------------------------------------------------------------------------
// File: SingleFieldEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 27.04.2015
//
// Description:
// Editor for editing the details of a single field.
//-----------------------------------------------------------------------------

#include "SingleFieldEditor.h"
#include "FieldAccessPoliciesEditor.h"

#include <KactusAPI/include/LibraryInterface.h>

#include <editors/ComponentEditor/memoryMaps/WriteValueConstraintComboBox.h>
#include <editors/ComponentEditor/memoryMaps/fieldeditor.h>
#include <editors/ComponentEditor/memoryMaps/ResetsEditor.h>
#include <KactusAPI/include/FieldInterface.h>

#include <editors/ComponentEditor/common/ExpressionEditor.h>
#include <KactusAPI/include/ExpressionFormatter.h>
#include <editors/ComponentEditor/common/ParameterCompleter.h>
#include <KactusAPI/include/ExpressionParser.h>

#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>

#include <common/widgets/accessComboBox/accesscombobox.h>
#include <common/widgets/booleanComboBox/booleancombobox.h>
#include <common/widgets/modWriteComboBox/modwritecombobox.h>
#include <common/widgets/testConstraintComboBox/testconstraintcombobox.h>
#include <common/widgets/readActionComboBox/readactioncombobox.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Field.h>
#include <IPXACTmodels/Component/WriteValueConstraint.h>
#include <IPXACTmodels/Component/ResetType.h>
#include <IPXACTmodels/Component/Register.h>

#include <IPXACTmodels/Component/validators/EnumeratedValueValidator.h>
#include <IPXACTmodels/Component/validators/FieldValidator.h>

#include <QComboBox>
#include <QFormLayout>
#include <QScrollArea>
#include <QSplitter>
#include <QGridLayout>

//-----------------------------------------------------------------------------
// Function: SingleFieldEditor::SingleFieldEditor()
//-----------------------------------------------------------------------------
SingleFieldEditor::SingleFieldEditor(QSharedPointer<Field> fieldItem, QSharedPointer<Component> component,
    LibraryInterface* handler, QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<FieldValidator> fieldValidator,
    FieldInterface* fieldInterface, QSharedPointer<Register> containingRegister, QWidget* parent):
ItemEditor(component, handler, parent),
nameEditor_(fieldItem, component->getRevision(), this, tr("Field name and description")),
resetsEditor_(new ResetsEditor(fieldInterface->getSubInterface(), component->getResetTypes(), expressionParser,
    parameterFinder, fieldItem, this)),
enumerationsEditor_(new FieldEditor(fieldItem->getEnumeratedValues(),
    fieldValidator->getEnumeratedValueValidator(), component, handler, this)),
offsetEditor_(new ExpressionEditor(parameterFinder, this)),
widthEditor_(new ExpressionEditor(parameterFinder, this)),
volatileEditor_(),
accessEditor_(),
modifiedWriteValueEditor_(),
readActionEditor_(),
testableEditor_(),
testConstrainedEditor_(),
isPresentEditor_(new ExpressionEditor(parameterFinder, this)),
reservedEditor_(new ExpressionEditor(parameterFinder, this)),
fieldIdEditor_(new QLineEdit(this)),
expressionParser_(expressionParser),
writeConstraintEditor_(new QComboBox(this)),
writeConstraintMinLimit_(new ExpressionEditor(parameterFinder, this)),
writeConstraintMaxLimit_(new ExpressionEditor(parameterFinder, this)),
fieldName_(fieldItem->name().toStdString()),
fieldValidator_(fieldValidator),
fieldInterface_(fieldInterface),
containingRegister_(containingRegister)
{
    fieldInterface_->setFields(containingRegister_->getFields());

    offsetEditor_->setFixedHeight(20);
    widthEditor_->setFixedHeight(20);
    isPresentEditor_->setFixedHeight(20);
    reservedEditor_->setFixedHeight(20);
    writeConstraintMinLimit_->setFixedHeight(20);
    writeConstraintMaxLimit_->setFixedHeight(20);

    ComponentParameterModel* componentParametersModel = new ComponentParameterModel(parameterFinder, this);
    componentParametersModel->setExpressionParser(expressionParser_);

    ParameterCompleter* offsetCompleter = new ParameterCompleter(this);
    offsetCompleter->setModel(componentParametersModel);

    ParameterCompleter* widthCompleter = new ParameterCompleter(this);
    widthCompleter->setModel(componentParametersModel);

    ParameterCompleter* isPresentCompleter = new ParameterCompleter(this);
    isPresentCompleter->setModel(componentParametersModel);

    ParameterCompleter* reservedCompleter = new ParameterCompleter(this);
    reservedCompleter->setModel(componentParametersModel);


    ParameterCompleter* writeValueMinCompleter = new ParameterCompleter(this);
    writeValueMinCompleter->setModel(componentParametersModel);

    ParameterCompleter* writeValueMaxCompleter = new ParameterCompleter(this);
    writeValueMaxCompleter->setModel(componentParametersModel);

    ParameterCompleter* resetValueCompleter = new ParameterCompleter(this);
    resetValueCompleter->setModel(componentParametersModel);

    ParameterCompleter* resetMaskCompleter = new ParameterCompleter(this);
    resetMaskCompleter->setModel(componentParametersModel);

    offsetEditor_->setAppendingCompleter(offsetCompleter);
    widthEditor_->setAppendingCompleter(widthCompleter);
    isPresentEditor_->setAppendingCompleter(isPresentCompleter);
    reservedEditor_->setAppendingCompleter(reservedCompleter);
    writeConstraintMinLimit_->setAppendingCompleter(writeValueMinCompleter);
    writeConstraintMaxLimit_->setAppendingCompleter(writeValueMaxCompleter);

    writeConstraintEditor_->setEditable(false);
    writeConstraintEditor_->addItems(WriteValueConversions::getConstraintTypes());

    writeConstraintEditor_->setCurrentText(
        QString::fromStdString(fieldInterface_->getWriteConstraint(fieldName_)));

    setWriteMinMaxConstraintEnableStatus(writeConstraintEditor_->currentIndex());

    accessPoliciesEditor_ = new FieldAccessPoliciesEditor(fieldItem->name(), fieldInterface_, 
        parameterFinder, expressionParser, this);

    setupLayout();
    connectSignals();

    offsetEditor_->setProperty("mandatoryField", true);
    widthEditor_->setProperty("mandatoryField", true);
}

//-----------------------------------------------------------------------------
// Function: SingleFieldEditor::onFieldNameChanged()
//-----------------------------------------------------------------------------
void SingleFieldEditor::onFieldNameChanged(QString const& oldName, QString const& newName)
{
    if (oldName == QString::fromStdString(fieldName_))
    {
        fieldName_ = newName.toStdString();
    }
}

//-----------------------------------------------------------------------------
// Function: SingleFieldEditor::onFieldNameChanged()
//-----------------------------------------------------------------------------
void SingleFieldEditor::onFieldNameChanged()
{
    fieldName_ = nameEditor_.name().toStdString();
}

//-----------------------------------------------------------------------------
// Function: SingleFieldEditor::refresh()
//-----------------------------------------------------------------------------
void SingleFieldEditor::refresh()
{
    fieldInterface_->setFields(containingRegister_->getFields());

    nameEditor_.refresh();
    resetsEditor_->refresh();
    enumerationsEditor_->refresh();

    changeExpressionEditorSignalBlockStatus(true);

    offsetEditor_->setExpression(QString::fromStdString(fieldInterface_->getOffsetExpression(fieldName_)));
    offsetEditor_->setToolTip(QString::fromStdString(fieldInterface_->getOffsetValue(fieldName_)));

    widthEditor_->setExpression(QString::fromStdString(fieldInterface_->getWidthExpression(fieldName_)));
    widthEditor_->setToolTip(QString::fromStdString(fieldInterface_->getWidthValue(fieldName_)));

    isPresentEditor_->setExpression(QString::fromStdString(fieldInterface_->getIsPresentExpression(fieldName_)));
    isPresentEditor_->setToolTip(QString::fromStdString(fieldInterface_->getIsPresentValue(fieldName_)));

    reservedEditor_->setExpression(QString::fromStdString(fieldInterface_->getReservedExpression(fieldName_)));
    reservedEditor_->setToolTip(QString::fromStdString(fieldInterface_->getReservedValue(fieldName_)));

    fieldIdEditor_->setText(QString::fromStdString(fieldInterface_->getID(fieldName_)));

    if (fieldInterface_->hasWriteConstraint(fieldName_))
    {
        writeConstraintMinLimit_->setExpression(
            QString::fromStdString(fieldInterface_->getWriteConstraintMinimumExpression(fieldName_)));
        writeConstraintMinLimit_->setToolTip(
            QString::fromStdString(fieldInterface_->getWriteConstraintMinimumValue(fieldName_)));

        writeConstraintMaxLimit_->setExpression(
            QString::fromStdString(fieldInterface_->getWriteConstraintMaximumExpression(fieldName_)));
        writeConstraintMaxLimit_->setToolTip(
            QString::fromStdString(fieldInterface_->getWriteConstraintMaximumValue(fieldName_)));
    }

    fieldValidator_->componentChange(component());

    changeExpressionEditorSignalBlockStatus(false);

    volatileEditor_->setCurrentValue(QString::fromStdString(fieldInterface_->getVolatile(fieldName_)));
    accessEditor_->setCurrentValue(fieldInterface_->getAccessType(fieldName_));
    modifiedWriteValueEditor_->setCurrentValue(fieldInterface_->getModifiedWriteValue(fieldName_));
    readActionEditor_->setCurrentValue(fieldInterface_->getReadAction(fieldName_));
    testableEditor_->setCurrentValue(QString::fromStdString(fieldInterface_->getTestableValue(fieldName_)));
    testConstrainedEditor_->setCurrentValue(fieldInterface_->getTestConstraint(fieldName_));

    if (fieldInterface_->hasWriteConstraint(fieldName_))
    {
        setWriteMinMaxConstraintEnableStatus(writeConstraintEditor_->currentIndex());
    }
}

//-----------------------------------------------------------------------------
// Function: SingleFieldEditor::showEvent()
//-----------------------------------------------------------------------------
void SingleFieldEditor::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    emit helpUrlRequested("componenteditor/field.html");
}

//-----------------------------------------------------------------------------
// Function: SingleFieldEditor::changeExpressionEditorSignalBlockStatus()
//-----------------------------------------------------------------------------
void SingleFieldEditor::changeExpressionEditorSignalBlockStatus(bool blockStatus) const
{
    offsetEditor_->blockSignals(blockStatus);
    widthEditor_->blockSignals(blockStatus);
    isPresentEditor_->blockSignals(blockStatus);
    reservedEditor_->blockSignals(blockStatus);
    writeConstraintMinLimit_->blockSignals(blockStatus);
    writeConstraintMaxLimit_->blockSignals(blockStatus);
}

//-----------------------------------------------------------------------------
// Function: SingleFieldEditor::formattedValueFor()
//-----------------------------------------------------------------------------
QString SingleFieldEditor::formattedValueFor(QString const& expression) const
{
    return ExpressionFormatter::format(expression, expressionParser_);
}

//-----------------------------------------------------------------------------
// Function: SingleFieldEditor::onOffsetEdited()
//-----------------------------------------------------------------------------
void SingleFieldEditor::onOffsetEdited()
{
    offsetEditor_->finishEditingCurrentWord();
    QString newBitOffset = offsetEditor_->getExpression();

    fieldInterface_->setOffset(fieldName_, newBitOffset.toStdString());
    offsetEditor_->setToolTip(formattedValueFor(newBitOffset));
}

//-----------------------------------------------------------------------------
// Function: SingleFieldEditor::onWidthEdited()
//-----------------------------------------------------------------------------
void SingleFieldEditor::onWidthEdited()
{
    widthEditor_->finishEditingCurrentWord();
    QString newBitWidth = widthEditor_->getExpression();

    fieldInterface_->setWidth(fieldName_, newBitWidth.toStdString());
    widthEditor_->setToolTip(formattedValueFor(newBitWidth));
}

//-----------------------------------------------------------------------------
// Function: SingleFieldEditor::onVolatileSelected()
//-----------------------------------------------------------------------------
void SingleFieldEditor::onVolatileSelected(QString const& newVolatileValue)
{
    fieldInterface_->setVolatile(fieldName_, newVolatileValue.toStdString());

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: SingleFieldEditor::onAccessSelected()
//-----------------------------------------------------------------------------
void SingleFieldEditor::onAccessSelected(QString const& newAccessValue)
{
    fieldInterface_->setAccess(fieldName_, newAccessValue.toStdString());

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: SingleFieldEditor::onModifiedWriteSelected()
//-----------------------------------------------------------------------------
void SingleFieldEditor::onModifiedWriteSelected(QString const& newModWriteValue)
{
    fieldInterface_->setModifiedWrite(fieldName_, newModWriteValue.toStdString());

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: SingleFieldEditor::onReadActionSelected()
//-----------------------------------------------------------------------------
void SingleFieldEditor::onReadActionSelected(QString const& newReadActionValue)
{
    fieldInterface_->setReadAction(fieldName_, newReadActionValue.toStdString());

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: SingleFieldEditor::onTestableSelected()
//-----------------------------------------------------------------------------
void SingleFieldEditor::onTestableSelected(QString const& newTestableValue)
{
    fieldInterface_->setTestable(fieldName_, newTestableValue.toStdString());

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: SingleFieldEditor::onTestConstrainedSelected()
//-----------------------------------------------------------------------------
void SingleFieldEditor::onTestConstrainedSelected(QString const& newTestConstrainedValue)
{
    fieldInterface_->setTestConstraint(fieldName_, newTestConstrainedValue.toStdString());

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: SingleFieldEditor::onIsPresentEdited()
//-----------------------------------------------------------------------------
void SingleFieldEditor::onIsPresentEdited()
{
    isPresentEditor_->finishEditingCurrentWord();
    QString newIsPresent = isPresentEditor_->getExpression();

    fieldInterface_->setIsPresent(fieldName_, newIsPresent.toStdString());
    isPresentEditor_->setToolTip(formattedValueFor(newIsPresent));
}

//-----------------------------------------------------------------------------
// Function: SingleFieldEditor::onReservedEdited()
//-----------------------------------------------------------------------------
void SingleFieldEditor::onReservedEdited()
{
    reservedEditor_->finishEditingCurrentWord();
    QString newReserved = reservedEditor_->getExpression();

    fieldInterface_->setReserved(fieldName_, newReserved.toStdString());
    reservedEditor_->setToolTip(formattedValueFor(newReserved));
}

//-----------------------------------------------------------------------------
// Function: SingleFieldEditor::onFieldIdChanged()
//-----------------------------------------------------------------------------
void SingleFieldEditor::onFieldIdChanged()
{
    fieldInterface_->setID(fieldName_, fieldIdEditor_->text().toStdString());
}

//-----------------------------------------------------------------------------
// Function: SingleFieldEditor::setWriteMinMaxConstraintEnableStatus()
//-----------------------------------------------------------------------------
void SingleFieldEditor::setWriteMinMaxConstraintEnableStatus(int writeConstraintIndex) const
{
    if (writeConstraintIndex == WriteValueConstraint::MIN_MAX)
    {
        writeConstraintMinLimit_->setEnabled(true);
        writeConstraintMaxLimit_->setEnabled(true);
    }
    else
    {
        writeConstraintMinLimit_->setEnabled(false);
        writeConstraintMaxLimit_->setEnabled(false);
    }
}

//-----------------------------------------------------------------------------
// Function: SingleFieldEditor::onWriteConstraintSelected()
//-----------------------------------------------------------------------------
void SingleFieldEditor::onWriteConstraintSelected(int newIndex)
{
    QString writeConstaintText = writeConstraintEditor_->currentText();
    fieldInterface_->setWriteConstraint(fieldName_, writeConstaintText.toStdString());

    setWriteMinMaxConstraintEnableStatus(newIndex);
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: SingleFieldEditor::onWriteConstraintMinimumEdited()
//-----------------------------------------------------------------------------
void SingleFieldEditor::onWriteConstraintMinimumEdited()
{
    writeConstraintMinLimit_->finishEditingCurrentWord();
    QString newMinimumLimit = writeConstraintMinLimit_->getExpression();

    fieldInterface_->setWriteConstraintMinimum(fieldName_, newMinimumLimit.toStdString());
    writeConstraintMinLimit_->setToolTip(formattedValueFor(newMinimumLimit));
}

//-----------------------------------------------------------------------------
// Function: SingleFieldEditor::onWriteConstraintMaximumEdited()
//-----------------------------------------------------------------------------
void SingleFieldEditor::onWriteConstraintMaximumEdited()
{

    writeConstraintMaxLimit_->finishEditingCurrentWord();
    QString newMaximumLimit = writeConstraintMaxLimit_->getExpression();

    fieldInterface_->setWriteConstraintMaximum(fieldName_, newMaximumLimit.toStdString());
    writeConstraintMaxLimit_->setToolTip(formattedValueFor(newMaximumLimit));
}

//-----------------------------------------------------------------------------
// Function: SingleFieldEditor::connectSignals()
//-----------------------------------------------------------------------------
void SingleFieldEditor::connectSignals()
{
    connect(offsetEditor_, SIGNAL(increaseReference(QString const&)),
        this, SIGNAL(increaseReferences(QString const&)), Qt::UniqueConnection);
    connect(offsetEditor_, SIGNAL(decreaseReference(QString const&)),
        this, SIGNAL(decreaseReferences(QString const&)), Qt::UniqueConnection);
    connect(widthEditor_, SIGNAL(increaseReference(QString const&)),
        this, SIGNAL(increaseReferences(QString const&)), Qt::UniqueConnection);
    connect(widthEditor_, SIGNAL(decreaseReference(QString const&)),
        this, SIGNAL(decreaseReferences(QString const&)), Qt::UniqueConnection);
    connect(isPresentEditor_, SIGNAL(increaseReference(QString const&)),
        this, SIGNAL(increaseReferences(QString const&)), Qt::UniqueConnection);
    connect(isPresentEditor_, SIGNAL(decreaseReference(QString const&)),
        this, SIGNAL(decreaseReferences(QString const&)), Qt::UniqueConnection);
    connect(reservedEditor_, SIGNAL(increaseReference(QString const&)),
        this, SIGNAL(increaseReferences(QString const&)), Qt::UniqueConnection);
    connect(reservedEditor_, SIGNAL(decreaseReference(QString const&)),
        this, SIGNAL(decreaseReferences(QString const&)), Qt::UniqueConnection);
    connect(writeConstraintMinLimit_, SIGNAL(increaseReference(QString const&)),
        this, SIGNAL(increaseReferences(QString const&)), Qt::UniqueConnection);
    connect(writeConstraintMinLimit_, SIGNAL(decreaseReference(QString const&)),
        this, SIGNAL(decreaseReferences(QString const&)), Qt::UniqueConnection);
    connect(writeConstraintMaxLimit_, SIGNAL(increaseReference(QString const&)),
        this, SIGNAL(increaseReferences(QString const&)), Qt::UniqueConnection);
    connect(writeConstraintMaxLimit_, SIGNAL(decreaseReference(QString const&)),
        this, SIGNAL(decreaseReferences(QString const&)), Qt::UniqueConnection);
    connect(resetsEditor_, SIGNAL(increaseReferences(QString const&)),
        this, SIGNAL(increaseReferences(QString const&)), Qt::UniqueConnection);
    connect(resetsEditor_, SIGNAL(decreaseReferences(QString const&)),
        this, SIGNAL(decreaseReferences(QString const&)), Qt::UniqueConnection);

    connect(offsetEditor_, SIGNAL(editingFinished()), this, SLOT(onOffsetEdited()), Qt::UniqueConnection);
    connect(widthEditor_, SIGNAL(editingFinished()), this, SLOT(onWidthEdited()), Qt::UniqueConnection);
    connect(isPresentEditor_, SIGNAL(editingFinished()), this, SLOT(onIsPresentEdited()), Qt::UniqueConnection);
    connect(reservedEditor_, SIGNAL(editingFinished()), this, SLOT(onReservedEdited()), Qt::UniqueConnection);
    connect(fieldIdEditor_, SIGNAL(editingFinished()), this, SLOT(onFieldIdChanged()), Qt::UniqueConnection);
    connect(writeConstraintMinLimit_, SIGNAL(editingFinished()), this, SLOT(onWriteConstraintMinimumEdited()), Qt::UniqueConnection);
    connect(writeConstraintMaxLimit_, SIGNAL(editingFinished()), this, SLOT(onWriteConstraintMaximumEdited()), Qt::UniqueConnection);

    connect(&nameEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(resetsEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(enumerationsEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(offsetEditor_, SIGNAL(editingFinished()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(widthEditor_, SIGNAL(editingFinished()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(isPresentEditor_, SIGNAL(editingFinished()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(reservedEditor_, SIGNAL(editingFinished()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(fieldIdEditor_, SIGNAL(editingFinished()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    connect(writeConstraintMinLimit_, SIGNAL(editingFinished()),this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(writeConstraintMaxLimit_, SIGNAL(editingFinished()),this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    connect(&nameEditor_, SIGNAL(nameChanged()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);
    connect(&nameEditor_, SIGNAL(nameChanged()), this, SLOT(onFieldNameChanged()), Qt::UniqueConnection);
    connect(offsetEditor_, SIGNAL(editingFinished()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);
    connect(offsetEditor_, SIGNAL(editingFinished()), this, SIGNAL(addressingChanged()), Qt::UniqueConnection);
    
    connect(widthEditor_, SIGNAL(editingFinished()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);
    connect(widthEditor_, SIGNAL(editingFinished()), this, SIGNAL(addressingChanged()), Qt::UniqueConnection);
    
    connect(isPresentEditor_, SIGNAL(editingFinished()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);
    connect(isPresentEditor_, SIGNAL(editingFinished()), this, SIGNAL(addressingChanged()), Qt::UniqueConnection);


    connect(writeConstraintEditor_, SIGNAL(activated(int)),this, SLOT(onWriteConstraintSelected(int)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: SingleFieldEditor::setupLayout()
//-----------------------------------------------------------------------------
void SingleFieldEditor::setupLayout()
{
    bool showStd14 = component()->getRevision() == Document::Revision::Std14;

    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    QHBoxLayout* scrollLayout = new QHBoxLayout(this);
    scrollLayout->addWidget(scrollArea);
    scrollLayout->setContentsMargins(0, 0, 0, 0);

    QGroupBox* fieldDefinitionGroup = new QGroupBox(tr("Field definition"));
    QFormLayout* fieldDefinitionLayout = new QFormLayout(fieldDefinitionGroup);
    fieldDefinitionLayout->setAlignment(Qt::AlignTop);
    fieldDefinitionLayout->setFormAlignment(Qt::AlignTop);
    fieldDefinitionLayout->setLabelAlignment(Qt::AlignTop);
    
    fieldDefinitionLayout->addRow(tr("Offset [bits], f(x):"), offsetEditor_);
    fieldDefinitionLayout->addRow(tr("Width [bits], f(x):"), widthEditor_);

    if (showStd14)
    {
        fieldDefinitionLayout->addRow(tr("Is present, f(x):"), isPresentEditor_);
        fieldDefinitionLayout->addRow(tr("Reserved, f(x):"), reservedEditor_);
        fieldDefinitionLayout->addRow(tr("Field ID:"), fieldIdEditor_);
    }
    else
    {
        auto spacer = new QWidget();
        spacer->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        fieldDefinitionLayout->addRow(spacer);
    }

    QGroupBox* fieldConstraintGroup = new QGroupBox(tr("Field constraints"));
    QFormLayout* fieldConstraintLayout = new QFormLayout(fieldConstraintGroup);
    volatileEditor_ = new BooleanComboBox(fieldConstraintGroup);
    fieldConstraintLayout->addRow(tr("Volatile:"), volatileEditor_);
    connect(volatileEditor_, SIGNAL(currentTextChanged(QString const&)),
        this, SLOT(onVolatileSelected(QString const&)), Qt::UniqueConnection);

    accessEditor_ = new AccessComboBox(fieldConstraintGroup);
    fieldConstraintLayout->addRow(tr("Access:"), accessEditor_);
    connect(accessEditor_, SIGNAL(currentTextChanged(QString const&)),
        this, SLOT(onAccessSelected(QString const&)), Qt::UniqueConnection);

    modifiedWriteValueEditor_ = new ModWriteComboBox(fieldConstraintGroup);
    fieldConstraintLayout->addRow(tr("Modified write value:"), modifiedWriteValueEditor_);
    connect(modifiedWriteValueEditor_, SIGNAL(currentTextChanged(QString const&)),
        this, SLOT(onModifiedWriteSelected(QString const&)), Qt::UniqueConnection);

    readActionEditor_ = new ReadActionComboBox(fieldConstraintGroup);
    fieldConstraintLayout->addRow(tr("Read action:"), readActionEditor_);
    connect(readActionEditor_, SIGNAL(currentTextChanged(QString const&)),
        this, SLOT(onReadActionSelected(QString const&)), Qt::UniqueConnection);

    testableEditor_ = new BooleanComboBox(fieldConstraintGroup);
    fieldConstraintLayout->addRow(tr("Testable:"), testableEditor_);
    connect(testableEditor_, SIGNAL(currentTextChanged(QString const&)),
        this, SLOT(onTestableSelected(QString const&)), Qt::UniqueConnection);

    testConstrainedEditor_ = new TestConstraintComboBox(fieldConstraintGroup);
    fieldConstraintLayout->addRow(tr("Test constraint:"), testConstrainedEditor_);
    connect(testConstrainedEditor_, SIGNAL(currentTextChanged(QString const&)),
        this, SLOT(onTestConstrainedSelected(QString const&)), Qt::UniqueConnection);


    fieldConstraintLayout->addRow(tr("Write value constraint:"), writeConstraintEditor_);
    fieldConstraintLayout->addRow(tr("Write constraint minimum, f(x):"), writeConstraintMinLimit_);
    fieldConstraintLayout->addRow(tr("Write constraint maximum, f(x):"), writeConstraintMaxLimit_);

    fieldConstraintGroup->setVisible(showStd14);

    QGridLayout* topOfPageLayout = new QGridLayout();
    topOfPageLayout->addWidget(&nameEditor_, 0, 0);
    topOfPageLayout->setRowStretch(0, 1);
    topOfPageLayout->setRowStretch(1, 2);

    if (showStd14)
    {
        topOfPageLayout->addWidget(fieldConstraintGroup, 0, 1);
        topOfPageLayout->addWidget(resetsEditor_, 1, 1);
        topOfPageLayout->addWidget(fieldDefinitionGroup, 1, 0);
    }
    else
    {
        topOfPageLayout->addWidget(enumerationsEditor_, 1, 1);
        topOfPageLayout->addWidget(fieldDefinitionGroup, 0, 1);
        topOfPageLayout->addWidget(resetsEditor_, 1, 0);
    }
    
    QWidget* topOfPageWidget = new QWidget();
    topOfPageWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    topOfPageWidget->setLayout(topOfPageLayout);
    topOfPageWidget->setContentsMargins(0, 0, 0, 0);

    QSplitter* verticalSplitter = new QSplitter(Qt::Vertical, scrollArea);
    verticalSplitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    verticalSplitter->addWidget(topOfPageWidget);

    if (showStd14)
    {
        verticalSplitter->addWidget(enumerationsEditor_);
    }
    else
    {
        verticalSplitter->addWidget(accessPoliciesEditor_);
    }

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
