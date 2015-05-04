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
#include <editors/ComponentEditor/common/ValueFormatter.h>
#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>

#include <common\widgets\accessComboBox\accesscombobox.h>
#include <common\widgets\booleanComboBox\booleancombobox.h>
#include <common\widgets\modWriteComboBox\modwritecombobox.h>
#include <common\widgets\testConstraintComboBox\testconstraintcombobox.h>
#include <common\widgets\readActionComboBox\readactioncombobox.h>

#include <QScrollArea>
#include <QFormLayout>
#include <QSplitter>

//-----------------------------------------------------------------------------
// Function: SingleFieldEditor::SingleFieldEditor()
//-----------------------------------------------------------------------------
SingleFieldEditor::SingleFieldEditor(QSharedPointer<Field> field, QSharedPointer<Component> component,
                                     LibraryInterface* handler, QSharedPointer<ParameterFinder> parameterFinder,
                                     QWidget* parent /* = 0 */):
ItemEditor(component, handler, parent),
nameEditor_(field->getNameGroup(), this, tr("Field name and description")),
enumerationsEditor_(new FieldEditor(field, component, handler, this)),
offsetEditor_(new ExpressionEditor(parameterFinder, this)),
widthEditor_(new ExpressionEditor(parameterFinder, this)),
volatileEditor_(),
accessEditor_(),
modifiedWriteValueEditor_(),
readActionEditor_(),
testableEditor_(),
testConstrainedEditor_(),
isPresentEditor_(new ExpressionEditor(parameterFinder, this)),
expressionParser_(new IPXactSystemVerilogParser(parameterFinder)),
writeConstraintEditor_(new WriteValueConstraintComboBox(field->getWriteConstraint(), this)),
writeConstraintMinLimit_(new QLineEdit(this)),
writeConstraintMaxLimit_(new QLineEdit(this)),
field_(field)
{
    offsetEditor_->setFixedHeight(20);
    widthEditor_->setFixedHeight(20);
    isPresentEditor_->setFixedHeight(20);

    writeConstraintMinLimit_->setValidator(
        new QRegularExpressionValidator(QRegularExpression("\\d*"), writeConstraintMinLimit_));

    writeConstraintMaxLimit_->setValidator(
        new QRegularExpressionValidator(QRegularExpression("\\d*"), writeConstraintMaxLimit_));

    ComponentParameterModel* componentParametersModel = new ComponentParameterModel(parameterFinder, this);
    componentParametersModel->setExpressionParser(expressionParser_);

    ParameterCompleter* offsetCompleter = new ParameterCompleter(this);
    offsetCompleter->setModel(componentParametersModel);

    ParameterCompleter* widthCompleter = new ParameterCompleter(this);
    widthCompleter->setModel(componentParametersModel);

    ParameterCompleter* isPresetCompleter = new ParameterCompleter(this);
    isPresetCompleter->setModel(componentParametersModel);

    offsetEditor_->setAppendingCompleter(offsetCompleter);
    widthEditor_->setAppendingCompleter(widthCompleter);
    isPresentEditor_->setAppendingCompleter(isPresetCompleter);

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
// Function: SingleFieldEditor::isValid()
//-----------------------------------------------------------------------------
bool SingleFieldEditor::isValid() const
{
    bool nameIsValid = nameEditor_.isValid();
    bool fieldIsValid = enumerationsEditor_->isValid();

    return nameIsValid && fieldIsValid;
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

    QFormLayout* fieldDefinitionLayout = new QFormLayout(fieldDefinitionGroup);
    fieldDefinitionLayout->addRow(tr("Offset [bits], f(x):"), offsetEditor_);
    fieldDefinitionLayout->addRow(tr("Width [bits], f(x):"), widthEditor_);
    fieldDefinitionLayout->addRow(tr("Is present, f(x);"), isPresentEditor_);

    QGroupBox* fieldConstraintGroup = new QGroupBox(tr("Field constraints"));

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
    fieldConstraintLayout->addRow(tr("Test constrained:"), testConstrainedEditor_);
    connect(testConstrainedEditor_, SIGNAL(activated(QString const&)),
        this, SLOT(onTestConstrainedSelected(QString const&)), Qt::UniqueConnection);

    fieldConstraintLayout->addRow(tr("Write value constraint:"), writeConstraintEditor_);
    fieldConstraintLayout->addRow(tr("Write constraint minimum:"), writeConstraintMinLimit_);
    fieldConstraintLayout->addRow(tr("Write constraint maximum:"), writeConstraintMaxLimit_);

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

    connect(offsetEditor_, SIGNAL(editingFinished()), this, SLOT(onOffsetEdited()), Qt::UniqueConnection);
    connect(widthEditor_, SIGNAL(editingFinished()), this, SLOT(onWdithEdited()), Qt::UniqueConnection);
    connect(isPresentEditor_, SIGNAL(editingFinished()), this, SLOT(onIsPresentEdited()), Qt::UniqueConnection);

    connect(&nameEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(enumerationsEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(offsetEditor_, SIGNAL(editingFinished()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(widthEditor_, SIGNAL(editingFinished()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(isPresentEditor_, SIGNAL(editingFinished()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    connect(&nameEditor_, SIGNAL(nameChanged()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);
    connect(offsetEditor_, SIGNAL(editingFinished()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);
    connect(widthEditor_, SIGNAL(editingFinished()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);
    connect(isPresentEditor_, SIGNAL(editingFinished()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);

    connect(writeConstraintEditor_, SIGNAL(activated(int)),
        this, SLOT(onWriteConstraintSelected(int)), Qt::UniqueConnection);
    connect(writeConstraintMinLimit_, SIGNAL(textEdited(QString const&)),
        this, SLOT(onWriteConstraintMinimumEdited(QString const&)), Qt::UniqueConnection);
    connect(writeConstraintMaxLimit_, SIGNAL(textEdited(QString const&)),
        this, SLOT(onWriteConstraintMaximumEdited(QString const&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: SingleFieldEditor::refresh()
//-----------------------------------------------------------------------------
void SingleFieldEditor::refresh()
{
    nameEditor_.refresh();
    enumerationsEditor_->refresh();

    changeExpressionEditorSignalBlockStatus(true);

    offsetEditor_->setExpression(field_->getBitOffsetExpression());
    offsetEditor_->setToolTip(formattedValueFor(field_->getBitOffsetExpression()));

    widthEditor_->setExpression(field_->getBitWidthExpression());
    widthEditor_->setToolTip(formattedValueFor(field_->getBitWidthExpression()));

    isPresentEditor_->setExpression(field_->getIsPresentExpression());
    isPresentEditor_->setToolTip(formattedValueFor(field_->getIsPresentExpression()));

    changeExpressionEditorSignalBlockStatus(false);

    volatileEditor_->setCurrentValue(field_->getVolatile());
    accessEditor_->setCurrentValue(field_->getAccess());
    modifiedWriteValueEditor_->setCurrentValue(field_->getModifiedWrite());
    readActionEditor_->setCurrentValue(field_->getReadAction());
    testableEditor_->setCurrentValue(field_->getTestable());
    testConstrainedEditor_->setCurrentValue(field_->getTestConstraint());

    writeConstraintMinLimit_->setText(QString::number(field_->getWriteConstraint()->getMinimum()));
    writeConstraintMaxLimit_->setText(QString::number(field_->getWriteConstraint()->getMaximum()));

    setWriteMinMaxConstraintEnableStatus(field_->getWriteConstraint()->getType());
}

//-----------------------------------------------------------------------------
// Function: SingleFieldEditor::changeExpressionEditorSignalBlockStatus()
//-----------------------------------------------------------------------------
void SingleFieldEditor::changeExpressionEditorSignalBlockStatus(bool blockStatus) const
{
    offsetEditor_->blockSignals(blockStatus);
    widthEditor_->blockSignals(blockStatus);
    isPresentEditor_->blockSignals(blockStatus);
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
    field_->setBitOffsetExpression(offsetEditor_->getExpression());

    QString formattedOffset = formattedValueFor(field_->getBitOffsetExpression());
    field_->setBitOffset(formattedOffset.toInt());

    offsetEditor_->setToolTip(formattedOffset);
}

//-----------------------------------------------------------------------------
// Function: SingleFieldEditor::onWdithEdited()
//-----------------------------------------------------------------------------
void SingleFieldEditor::onWdithEdited()
{
    widthEditor_->finishEditingCurrentWord();
    field_->setBitWidthExpression(widthEditor_->getExpression());

    QString formattedWidth = formattedValueFor(field_->getBitWidthExpression());
    field_->setBitWidth(formattedWidth.toInt());

    widthEditor_->setToolTip(formattedWidth);
}

//-----------------------------------------------------------------------------
// Function: SingleFieldEditor::onVolatileSelected()
//-----------------------------------------------------------------------------
void SingleFieldEditor::onVolatileSelected(QString const& newVolatileValue)
{
    bool fieldIsVolatile = false;
    if (newVolatileValue == "true")
    {
        fieldIsVolatile = true;
    }

    field_->setVolatile(fieldIsVolatile);

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: SingleFieldEditor::onAccessSelected()
//-----------------------------------------------------------------------------
void SingleFieldEditor::onAccessSelected(QString const& newAccessValue)
{
    field_->setAccess(General::str2Access(newAccessValue, General::ACCESS_COUNT));

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
    bool fieldIsTestable = false;
    if (newTestableValue == "true")
    {
        fieldIsTestable = true;
    }

    field_->setTestable(fieldIsTestable);

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
    field_->setIsPresentExpression(isPresentEditor_->getExpression());

    isPresentEditor_->setToolTip(formattedValueFor(field_->getIsPresentExpression()));
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
    setWriteMinMaxConstraintEnableStatus(newIndex);
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: SingleFieldEditor::onWriteConstraintMinimumEdited()
//-----------------------------------------------------------------------------
void SingleFieldEditor::onWriteConstraintMinimumEdited(QString const& newWriteConstraintMin)
{
    field_->getWriteConstraint()->setMinimum(newWriteConstraintMin.toInt());
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: SingleFieldEditor::onWriteConstraintMaximumEdited()
//-----------------------------------------------------------------------------
void SingleFieldEditor::onWriteConstraintMaximumEdited(QString const& newWriteConstraintMax)
{
    field_->getWriteConstraint()->setMaximum(newWriteConstraintMax.toInt());
    emit contentChanged();
}