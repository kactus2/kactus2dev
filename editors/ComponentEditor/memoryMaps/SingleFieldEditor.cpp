//-----------------------------------------------------------------------------
// File: SingleFieldEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 27.04.2015
//
// Description:
// Editor for editing the details of a single field.
//-----------------------------------------------------------------------------

#include "SingleFieldEditor.h"

#include <library/LibraryManager/libraryinterface.h>

#include <editors/ComponentEditor/memoryMaps/WriteValueConstraintComboBox.h>
#include <editors/ComponentEditor/memoryMaps/fieldeditor.h>
#include <editors/ComponentEditor/common/ExpressionEditor.h>
#include <editors/ComponentEditor/common/ParameterCompleter.h>

#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>

#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <common/widgets/accessComboBox/accesscombobox.h>
#include <common/widgets/booleanComboBox/booleancombobox.h>
#include <common/widgets/modWriteComboBox/modwritecombobox.h>
#include <common/widgets/testConstraintComboBox/testconstraintcombobox.h>
#include <common/widgets/readActionComboBox/readactioncombobox.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Field.h>
#include <IPXACTmodels/Component/WriteValueConstraint.h>

#include <IPXACTmodels/Component/validators/EnumeratedValueValidator.h>
#include <IPXACTmodels/Component/validators/FieldValidator.h>
#include <IPXACTmodels/common/validators/ValueFormatter.h>

#include <QComboBox>
#include <QFormLayout>
#include <QScrollArea>
#include <QSplitter>

//-----------------------------------------------------------------------------
// Function: SingleFieldEditor::SingleFieldEditor()
//-----------------------------------------------------------------------------
SingleFieldEditor::SingleFieldEditor(QSharedPointer<Field> field, QSharedPointer<Component> component,
    LibraryInterface* handler, QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<FieldValidator> fieldValidator,
    QWidget* parent /* = 0 */):
ItemEditor(component, handler, parent),
nameEditor_(field, this, tr("Field name and description")),
enumerationsEditor_(new FieldEditor(field->getEnumeratedValues(), fieldValidator->getEnumeratedValueValidator(),
                    component, handler, this)),
offsetEditor_(new ExpressionEditor(parameterFinder, this)),
widthEditor_(new ExpressionEditor(parameterFinder, this)),
volatileEditor_(),
accessEditor_(),
modifiedWriteValueEditor_(),
readActionEditor_(),
testableEditor_(),
testConstrainedEditor_(),
isPresentEditor_(new ExpressionEditor(parameterFinder, this)),
expressionParser_(expressionParser),
writeConstraintEditor_(new QComboBox(this)),
writeConstraintMinLimit_(new ExpressionEditor(parameterFinder, this)),
writeConstraintMaxLimit_(new ExpressionEditor(parameterFinder, this)),
resetValueEditor_(new ExpressionEditor(parameterFinder, this)),
resetMaskEditor_(new ExpressionEditor(parameterFinder, this)),
field_(field),
fieldValidator_(fieldValidator)
{
    offsetEditor_->setFixedHeight(20);
    widthEditor_->setFixedHeight(20);
    isPresentEditor_->setFixedHeight(20);
    resetValueEditor_->setFixedHeight(20);
    resetMaskEditor_->setFixedHeight(20);
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
    writeConstraintMinLimit_->setAppendingCompleter(writeValueMinCompleter);
    writeConstraintMaxLimit_->setAppendingCompleter(writeValueMaxCompleter);
    resetValueEditor_->setAppendingCompleter(resetValueCompleter);
    resetMaskEditor_->setAppendingCompleter(resetMaskCompleter);

    writeConstraintEditor_->setEditable(false);

    writeConstraintEditor_->addItem(tr("Write as read"));
    writeConstraintEditor_->addItem(tr("Use enumerated values"));
    writeConstraintEditor_->addItem(tr("Set minimum and maximum limits"));
    writeConstraintEditor_->addItem(tr("No constraints"));

    if (!field->getWriteConstraint().isNull())
    {
        writeConstraintEditor_->setCurrentIndex(field->getWriteConstraint()->getType());
    }
    else
    {
        writeConstraintEditor_->setCurrentText(tr("No constraints"));
    }
    setWriteMinMaxConstraintEnableStatus(writeConstraintEditor_->currentIndex());

    setupLayout();
    connectSignals();

    offsetEditor_->setProperty("mandatoryField", true);
    widthEditor_->setProperty("mandatoryField", true);
}

//-----------------------------------------------------------------------------
// Function: SingleFieldEditor::~SingleFieldEditor()
//-----------------------------------------------------------------------------
SingleFieldEditor::~SingleFieldEditor()
{

}

//-----------------------------------------------------------------------------
// Function: SingleFieldEditor::setupLayout()
//-----------------------------------------------------------------------------
void SingleFieldEditor::setupLayout()
{
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    QHBoxLayout* scrollLayout = new QHBoxLayout(this);
    scrollLayout->addWidget(scrollArea);
    scrollLayout->setContentsMargins(0, 0, 0, 0);

    QGroupBox* fieldDefinitionGroup = new QGroupBox(tr("Field definition"));

    offsetEditor_->setFrameShadow(QFrame::Sunken);
    widthEditor_->setFrameShadow(QFrame::Sunken);
    isPresentEditor_->setFrameShadow(QFrame::Sunken);
    resetValueEditor_->setFrameShadow(QFrame::Sunken);
    resetMaskEditor_->setFrameShadow(QFrame::Sunken);

    QFormLayout* fieldDefinitionLayout = new QFormLayout(fieldDefinitionGroup);
    fieldDefinitionLayout->addRow(tr("Offset [bits], f(x):"), offsetEditor_);
    fieldDefinitionLayout->addRow(tr("Width [bits], f(x):"), widthEditor_);
    fieldDefinitionLayout->addRow(tr("Is present, f(x):"), isPresentEditor_);
    fieldDefinitionLayout->addRow(tr("Reset value, f(x):"), resetValueEditor_);
    fieldDefinitionLayout->addRow(tr("Reset mask, f(x):"), resetMaskEditor_);

    QGroupBox* fieldConstraintGroup = new QGroupBox(tr("Field constraints"));

    writeConstraintMinLimit_->setFrameShadow(QFrame::Sunken);
    writeConstraintMaxLimit_->setFrameShadow(QFrame::Sunken);

    QFormLayout* fieldConstraintLayout = new QFormLayout(fieldConstraintGroup);
    volatileEditor_ = new BooleanComboBox(fieldDefinitionGroup);
    fieldConstraintLayout->addRow(tr("Volatile:"), volatileEditor_);
    connect(volatileEditor_, SIGNAL(activated(QString const&)),
        this, SLOT(onVolatileSelected(QString const&)), Qt::UniqueConnection);

    accessEditor_ = new AccessComboBox(fieldDefinitionGroup);
    fieldConstraintLayout->addRow(tr("Access:"), accessEditor_);
    connect(accessEditor_, SIGNAL(activated(QString const&)),
        this, SLOT(onAccessSelected(QString const&)), Qt::UniqueConnection);

    modifiedWriteValueEditor_ = new ModWriteComboBox(fieldDefinitionGroup);
    fieldConstraintLayout->addRow(tr("Modified write value:"), modifiedWriteValueEditor_);
    connect(modifiedWriteValueEditor_, SIGNAL(activated(QString const&)),
        this, SLOT(onModifiedWriteSelected(QString const&)), Qt::UniqueConnection);

    readActionEditor_ = new ReadActionComboBox(fieldDefinitionGroup);
    fieldConstraintLayout->addRow(tr("Read action:"), readActionEditor_);
    connect(readActionEditor_, SIGNAL(activated(QString const&)),
        this, SLOT(onReadActionSelected(QString const&)), Qt::UniqueConnection);

    testableEditor_ = new BooleanComboBox(fieldDefinitionGroup);
    fieldConstraintLayout->addRow(tr("Testable:"), testableEditor_);
    connect(testableEditor_, SIGNAL(activated(QString const&)),
        this, SLOT(onTestableSelected(QString const&)), Qt::UniqueConnection);

    testConstrainedEditor_ = new TestConstraintComboBox(fieldDefinitionGroup);
    fieldConstraintLayout->addRow(tr("Test constraint:"), testConstrainedEditor_);
    connect(testConstrainedEditor_, SIGNAL(activated(QString const&)),
        this, SLOT(onTestConstrainedSelected(QString const&)), Qt::UniqueConnection);

    fieldConstraintLayout->addRow(tr("Write value constraint:"), writeConstraintEditor_);
    fieldConstraintLayout->addRow(tr("Write constraint minimum, f(x):"), writeConstraintMinLimit_);
    fieldConstraintLayout->addRow(tr("Write constraint maximum, f(x):"), writeConstraintMaxLimit_);

    QVBoxLayout* topLeftOfPageLayout = new QVBoxLayout();
    topLeftOfPageLayout->addWidget(&nameEditor_, 0,Qt::AlignTop);
    topLeftOfPageLayout->addWidget(fieldDefinitionGroup);

    QHBoxLayout* topOfPageLayout = new QHBoxLayout();
    topOfPageLayout->addLayout(topLeftOfPageLayout, 0);
    topOfPageLayout->addWidget(fieldConstraintGroup, 0);

    QWidget* topOfPageWidget = new QWidget();
    topOfPageWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    topOfPageWidget->setLayout(topOfPageLayout);
    topOfPageWidget->setContentsMargins(0, 0, 0, 0);

    QSplitter* verticalSplitter = new QSplitter(Qt::Vertical, scrollArea);
    verticalSplitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    verticalSplitter->addWidget(topOfPageWidget);
    verticalSplitter->addWidget(enumerationsEditor_);
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
    connect(writeConstraintMinLimit_, SIGNAL(increaseReference(QString const&)),
        this, SIGNAL(increaseReferences(QString const&)), Qt::UniqueConnection);
    connect(writeConstraintMinLimit_, SIGNAL(decreaseReference(QString const&)),
        this, SIGNAL(decreaseReferences(QString const&)), Qt::UniqueConnection);
    connect(writeConstraintMaxLimit_, SIGNAL(increaseReference(QString const&)),
        this, SIGNAL(increaseReferences(QString const&)), Qt::UniqueConnection);
    connect(writeConstraintMaxLimit_, SIGNAL(decreaseReference(QString const&)),
        this, SIGNAL(decreaseReferences(QString const&)), Qt::UniqueConnection);
    connect(resetValueEditor_, SIGNAL(increaseReference(QString const&)),
        this, SIGNAL(increaseReferences(QString const&)), Qt::UniqueConnection);
    connect(resetValueEditor_, SIGNAL(decreaseReference(QString const&)),
        this, SIGNAL(decreaseReferences(QString const&)), Qt::UniqueConnection);
    connect(resetMaskEditor_, SIGNAL(increaseReference(QString const&)),
        this, SIGNAL(increaseReferences(QString const&)), Qt::UniqueConnection);
    connect(resetMaskEditor_, SIGNAL(decreaseReference(QString const&)),
        this, SIGNAL(decreaseReferences(QString const&)), Qt::UniqueConnection);

    connect(offsetEditor_, SIGNAL(editingFinished()), this, SLOT(onOffsetEdited()), Qt::UniqueConnection);
    connect(widthEditor_, SIGNAL(editingFinished()), this, SLOT(onWidthEdited()), Qt::UniqueConnection);
    connect(isPresentEditor_, SIGNAL(editingFinished()), this, SLOT(onIsPresentEdited()), Qt::UniqueConnection);
    connect(writeConstraintMinLimit_, SIGNAL(editingFinished()),
        this, SLOT(onWriteConstraintMinimumEdited()), Qt::UniqueConnection);
    connect(writeConstraintMaxLimit_, SIGNAL(editingFinished()),
        this, SLOT(onWriteConstraintMaximumEdited()), Qt::UniqueConnection);
    connect(resetValueEditor_, SIGNAL(editingFinished()), this, SLOT(onResetValueEdited()), Qt::UniqueConnection);
    connect(resetMaskEditor_, SIGNAL(editingFinished()), this, SLOT(onResetMaskEdited()), Qt::UniqueConnection);

    connect(&nameEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(enumerationsEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(offsetEditor_, SIGNAL(editingFinished()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(widthEditor_, SIGNAL(editingFinished()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(isPresentEditor_, SIGNAL(editingFinished()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(writeConstraintMinLimit_, SIGNAL(editingFinished()),
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(writeConstraintMaxLimit_, SIGNAL(editingFinished()),
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(resetValueEditor_, SIGNAL(editingFinished()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(resetMaskEditor_, SIGNAL(editingFinished()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    connect(&nameEditor_, SIGNAL(nameChanged()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);
    connect(offsetEditor_, SIGNAL(editingFinished()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);
    connect(widthEditor_, SIGNAL(editingFinished()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);
    connect(isPresentEditor_, SIGNAL(editingFinished()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);

    connect(writeConstraintEditor_, SIGNAL(activated(int)),
        this, SLOT(onWriteConstraintSelected(int)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: SingleFieldEditor::refresh()
//-----------------------------------------------------------------------------
void SingleFieldEditor::refresh()
{
    nameEditor_.refresh();
    enumerationsEditor_->refresh();

    changeExpressionEditorSignalBlockStatus(true);

    offsetEditor_->setExpression(field_->getBitOffset());
    offsetEditor_->setToolTip(formattedValueFor(field_->getBitOffset()));

    widthEditor_->setExpression(field_->getBitWidth());
    widthEditor_->setToolTip(formattedValueFor(field_->getBitWidth()));

    isPresentEditor_->setExpression(field_->getIsPresent());
    isPresentEditor_->setToolTip(formattedValueFor(field_->getIsPresent()));

    if (field_->getWriteConstraint())
    {
        writeConstraintMinLimit_->setExpression(field_->getWriteConstraint()->getMinimum());
        writeConstraintMinLimit_->setToolTip(formattedValueFor(field_->getWriteConstraint()->getMinimum()));

        writeConstraintMaxLimit_->setExpression(field_->getWriteConstraint()->getMaximum());
        writeConstraintMaxLimit_->setToolTip(formattedValueFor(field_->getWriteConstraint()->getMaximum()));
    }

    resetValueEditor_->setExpression(field_->getResetValue());
    resetValueEditor_->setToolTip(formattedValueFor(field_->getResetValue()));

    resetMaskEditor_->setExpression(field_->getResetMask());
    resetMaskEditor_->setToolTip(formattedValueFor(field_->getResetMask()));

    changeExpressionEditorSignalBlockStatus(false);

    volatileEditor_->setCurrentValue(field_->getVolatile().toString());
    accessEditor_->setCurrentValue(field_->getAccess());
    modifiedWriteValueEditor_->setCurrentValue(field_->getModifiedWrite());
    readActionEditor_->setCurrentValue(field_->getReadAction());
    testableEditor_->setCurrentValue(field_->getTestable().toString());
    testConstrainedEditor_->setCurrentValue(field_->getTestConstraint());
    
    if (field_->getWriteConstraint())
    {
        setWriteMinMaxConstraintEnableStatus(field_->getWriteConstraint()->getType());
    }
}

//-----------------------------------------------------------------------------
// Function: SingleFieldEditor::changeExpressionEditorSignalBlockStatus()
//-----------------------------------------------------------------------------
void SingleFieldEditor::changeExpressionEditorSignalBlockStatus(bool blockStatus) const
{
    offsetEditor_->blockSignals(blockStatus);
    widthEditor_->blockSignals(blockStatus);
    isPresentEditor_->blockSignals(blockStatus);
    writeConstraintMinLimit_->blockSignals(blockStatus);
    writeConstraintMaxLimit_->blockSignals(blockStatus);
    resetValueEditor_->blockSignals(blockStatus);
    resetMaskEditor_->blockSignals(blockStatus);
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
// Function: SingleFieldEditor::formattedValueFor()
//-----------------------------------------------------------------------------
QString SingleFieldEditor::formattedValueFor(QString const& expression) const
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
// Function: SingleFieldEditor::onOffsetEdited()
//-----------------------------------------------------------------------------
void SingleFieldEditor::onOffsetEdited()
{
    offsetEditor_->finishEditingCurrentWord();
    QString newBitOffset = offsetEditor_->getExpression();

    field_->setBitOffset(newBitOffset);
    offsetEditor_->setToolTip(formattedValueFor(newBitOffset));
}

//-----------------------------------------------------------------------------
// Function: SingleFieldEditor::onWidthEdited()
//-----------------------------------------------------------------------------
void SingleFieldEditor::onWidthEdited()
{
    widthEditor_->finishEditingCurrentWord();
    QString newBitWidth = widthEditor_->getExpression();

    field_->setBitWidth(newBitWidth);
    widthEditor_->setToolTip(formattedValueFor(newBitWidth));
}

//-----------------------------------------------------------------------------
// Function: SingleFieldEditor::onVolatileSelected()
//-----------------------------------------------------------------------------
void SingleFieldEditor::onVolatileSelected(QString const& newVolatileValue)
{
    if (newVolatileValue == QLatin1String("true"))
    {
        field_->setVolatile(true);
    }
    else if (newVolatileValue == QLatin1String("false"))
    {
        field_->setVolatile(false);
    }
    else
    {
        field_->clearVolatile();
    }

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: SingleFieldEditor::onAccessSelected()
//-----------------------------------------------------------------------------
void SingleFieldEditor::onAccessSelected(QString const& newAccessValue)
{
    field_->setAccess(AccessTypes::str2Access(newAccessValue, AccessTypes::ACCESS_COUNT));

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: SingleFieldEditor::onModifiedWriteSelected()
//-----------------------------------------------------------------------------
void SingleFieldEditor::onModifiedWriteSelected(QString const& newModWriteValue)
{
    field_->setModifiedWrite(General::str2ModifiedWrite(newModWriteValue));

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: SingleFieldEditor::onReadActionSelected()
//-----------------------------------------------------------------------------
void SingleFieldEditor::onReadActionSelected(QString const& newReadActionValue)
{
    field_->setReadAction(General::str2ReadAction(newReadActionValue));

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: SingleFieldEditor::onTestableSelected()
//-----------------------------------------------------------------------------
void SingleFieldEditor::onTestableSelected(QString const& newTestableValue)
{
    if (newTestableValue == QLatin1String("true"))
    {
        field_->setTestable(true);
    }
    else if (newTestableValue == QLatin1String("false"))
    {
        field_->setTestable(false);
    }
    else
    {
        field_->clearTestable();
    }

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: SingleFieldEditor::onTestConstrainedSelected()
//-----------------------------------------------------------------------------
void SingleFieldEditor::onTestConstrainedSelected(QString const& newTestConstrainedValue)
{
    field_->setTestConstraint(General::str2TestConstraint(newTestConstrainedValue));

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: SingleFieldEditor::onIsPresentEdited()
//-----------------------------------------------------------------------------
void SingleFieldEditor::onIsPresentEdited()
{
    isPresentEditor_->finishEditingCurrentWord();
    QString newIsPresent = isPresentEditor_->getExpression();

    field_->setIsPresent(newIsPresent);
    isPresentEditor_->setToolTip(formattedValueFor(newIsPresent));
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
    if (newIndex == WriteValueConstraint::TYPE_COUNT)
    {
        field_->setWriteConstraint(QSharedPointer<WriteValueConstraint>(0));
    }

    else
    {
        if (!field_->getWriteConstraint())
        {
            field_->setWriteConstraint(QSharedPointer<WriteValueConstraint>(new WriteValueConstraint()));
        }        

        field_->getWriteConstraint()->setType(static_cast<WriteValueConstraint::Type>(newIndex));
    }
    
    setWriteMinMaxConstraintEnableStatus(newIndex);
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: SingleFieldEditor::onWriteConstraintMinimumEdited()
//-----------------------------------------------------------------------------
void SingleFieldEditor::onWriteConstraintMinimumEdited()
{
    Q_ASSERT(field_->getWriteConstraint());

    writeConstraintMinLimit_->finishEditingCurrentWord();
    QString newMinimumLimit = writeConstraintMinLimit_->getExpression();

    if (!field_->getWriteConstraint())
    {
        QSharedPointer<WriteValueConstraint> newWriteValueConstraint (new WriteValueConstraint());
        newWriteValueConstraint->setType(WriteValueConstraint::MIN_MAX);
        field_->setWriteConstraint(newWriteValueConstraint);
    }

    field_->getWriteConstraint()->setMinimum(newMinimumLimit);
    writeConstraintMinLimit_->setToolTip(formattedValueFor(newMinimumLimit));
}

//-----------------------------------------------------------------------------
// Function: SingleFieldEditor::onWriteConstraintMaximumEdited()
//-----------------------------------------------------------------------------
void SingleFieldEditor::onWriteConstraintMaximumEdited()
{
    Q_ASSERT(field_->getWriteConstraint());

    writeConstraintMaxLimit_->finishEditingCurrentWord();
    QString newMaximumLimit = writeConstraintMaxLimit_->getExpression();

    if (!field_->getWriteConstraint())
    {
        QSharedPointer<WriteValueConstraint> newWriteValueConstraint (new WriteValueConstraint());
        newWriteValueConstraint->setType(WriteValueConstraint::MIN_MAX);
        field_->setWriteConstraint(newWriteValueConstraint);
    }

    field_->getWriteConstraint()->setMaximum(newMaximumLimit);
    writeConstraintMaxLimit_->setToolTip(formattedValueFor(newMaximumLimit));
}

//-----------------------------------------------------------------------------
// Function: SingleFieldEditor::onResetValueEdited()
//-----------------------------------------------------------------------------
void SingleFieldEditor::onResetValueEdited()
{
    resetValueEditor_->finishEditingCurrentWord();
    QString newResetValue = resetValueEditor_->getExpression();

    if (newResetValue.isEmpty())
    {
        resetMaskEditor_->clear();
        field_->setResetMask(QStringLiteral(""));
    }

    field_->setResetValue(newResetValue);
    resetValueEditor_->setToolTip(formattedValueFor(newResetValue));
}

//-----------------------------------------------------------------------------
// Function: SingleFieldEditor::onResetMaskEdited()
//-----------------------------------------------------------------------------
void SingleFieldEditor::onResetMaskEdited()
{
    resetMaskEditor_->finishEditingCurrentWord();
    QString newResetMask = resetMaskEditor_->getExpression();

    field_->setResetMask(newResetMask);
    resetMaskEditor_->setToolTip(formattedValueFor(newResetMask));
}