//-----------------------------------------------------------------------------
// File: SingleAddressBlockEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 23.04.2015
//
// Description:
// Editor for editing the details of a single address block.
//-----------------------------------------------------------------------------

#include "SingleAddressBlockEditor.h"

#include <common/widgets/usageComboBox/usagecombobox.h>
#include <common/widgets/accessComboBox/accesscombobox.h>
#include <common/widgets/booleanComboBox/booleancombobox.h>

#include <editors/ComponentEditor/common/ExpressionEditor.h>
#include <editors/ComponentEditor/common/ParameterCompleter.h>
#include <editors/ComponentEditor/common/ValueFormatter.h>

#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>

#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <QFormLayout>
#include <QScrollArea>
#include <QSplitter>

//-----------------------------------------------------------------------------
// Function: SingleAddressBlockEditor::SingleAddressBlockEditor()
//-----------------------------------------------------------------------------
SingleAddressBlockEditor::SingleAddressBlockEditor(QSharedPointer<AddressBlock> addressBlock,
    QSharedPointer<Component> component, LibraryInterface* handler,
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ExpressionFormatter> expressionFormatter,
    QSharedPointer<ExpressionParser> expressionParser,
    QWidget* parent):
ItemEditor(component, handler, parent),
nameEditor_(addressBlock->getNameGroup(), this, tr("Address block name and description")),
registersEditor_(new AddressBlockEditor(addressBlock, component, handler, parameterFinder, expressionFormatter,
    this)),
usageEditor_(),
baseAddressEditor_(new ExpressionEditor(parameterFinder, this)),
rangeEditor_(new ExpressionEditor(parameterFinder, this)),
widthEditor_(new ExpressionEditor(parameterFinder, this)),
accessEditor_(),
volatileEditor_(),
addressBlock_(addressBlock),
expressionParser_(expressionParser)
{
    baseAddressEditor_->setFixedHeight(20);
    rangeEditor_->setFixedHeight(20);
    widthEditor_->setFixedHeight(20);

    ComponentParameterModel* componentParametersModel = new ComponentParameterModel(parameterFinder, this);
    componentParametersModel->setExpressionParser(expressionParser_);

    ParameterCompleter* baseAddressEditorCompleter = new ParameterCompleter(this);
    baseAddressEditorCompleter->setModel(componentParametersModel);

    ParameterCompleter* rangeEditorCompleter = new ParameterCompleter(this);
    rangeEditorCompleter->setModel(componentParametersModel);

    ParameterCompleter* widthEditorCompleter = new ParameterCompleter(this);
    widthEditorCompleter->setModel(componentParametersModel);
    
    baseAddressEditor_->setAppendingCompleter(baseAddressEditorCompleter);
    rangeEditor_->setAppendingCompleter(rangeEditorCompleter);
    widthEditor_->setAppendingCompleter(widthEditorCompleter);

    setupLayout();

    connectSignals();

    baseAddressEditor_->setProperty("mandatoryField", true);
    rangeEditor_->setProperty("mandatoryField", true);
    widthEditor_->setProperty("mandatoryField", true);
}

//-----------------------------------------------------------------------------
// Function: SingleAddressBlockEditor::~SingleAddressBlockEditor()
//-----------------------------------------------------------------------------
SingleAddressBlockEditor::~SingleAddressBlockEditor()
{

}

//-----------------------------------------------------------------------------
// Function: SingleAddressBlockEditor::isValid()
//-----------------------------------------------------------------------------
bool SingleAddressBlockEditor::isValid() const
{
    bool nameGroupIsValid = nameEditor_.isValid();
    bool addressBlockIsValid = addressBlock_->isValid(component()->getChoices());

    return nameGroupIsValid && addressBlockIsValid;
}

//-----------------------------------------------------------------------------
// Function: SingleAddressBlockEditor::showEvent()
//-----------------------------------------------------------------------------
void SingleAddressBlockEditor::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    emit helpUrlRequested("componenteditor/addressblock.html");
}

//-----------------------------------------------------------------------------
// Function: SingleAddressBlockEditor::refresh()
//-----------------------------------------------------------------------------
void SingleAddressBlockEditor::refresh()
{
    nameEditor_.refresh();
    registersEditor_->refresh();

    General::Usage usage = addressBlock_->getUsage();
    registersEditor_->setEnabled(!addressBlock_->getRegisterData().isEmpty() ||
        (usage != General::RESERVED && usage != General::MEMORY));
    usageEditor_->setCurrentValue(usage);

    changeExpressionEditorSignalBlockStatus(true);

    baseAddressEditor_->setExpression(addressBlock_->getBaseAddress());
    baseAddressEditor_->setToolTip(formattedValueFor(addressBlock_->getBaseAddress()));
    rangeEditor_->setExpression(addressBlock_->getRange());
    rangeEditor_->setToolTip(formattedValueFor(addressBlock_->getRange()));
    widthEditor_->setExpression(addressBlock_->getWidthExpression());
    widthEditor_->setToolTip(formattedValueFor(addressBlock_->getWidthExpression()));

    changeExpressionEditorSignalBlockStatus(false);

    accessEditor_->setCurrentValue(addressBlock_->getAccess());
    volatileEditor_->setCurrentValue(General::BooleanValue2Bool(addressBlock_->getVolatile(), true));
}

//-----------------------------------------------------------------------------
// Function: SingleAddressBlockEditor::onBaseAddressChanged()
//-----------------------------------------------------------------------------
void SingleAddressBlockEditor::onBaseAddressChanged()
{
    baseAddressEditor_->finishEditingCurrentWord();
    addressBlock_->setBaseAddress(baseAddressEditor_->getExpression());

    baseAddressEditor_->setToolTip(formattedValueFor(addressBlock_->getBaseAddress()));
}

//-----------------------------------------------------------------------------
// Function: SingleAddressBlockEditor::onRangeChanged()
//-----------------------------------------------------------------------------
void SingleAddressBlockEditor::onRangeChanged()
{
    rangeEditor_->finishEditingCurrentWord();
    addressBlock_->setRange(rangeEditor_->getExpression());

    rangeEditor_->setToolTip(formattedValueFor(addressBlock_->getRange()));
}

//-----------------------------------------------------------------------------
// Function: SingleAddressBlockEditor::onWidthChanged()
//-----------------------------------------------------------------------------
void SingleAddressBlockEditor::onWidthChanged()
{
    widthEditor_->finishEditingCurrentWord();
    addressBlock_->setWidthExpression(widthEditor_->getExpression());

    QString formattedWidth = formattedValueFor(addressBlock_->getWidthExpression());

    addressBlock_->setWidth(formattedWidth.toInt());

    widthEditor_->setToolTip(formattedWidth);
}

//-----------------------------------------------------------------------------
// Function: SingleAddressBlockEditor::formattedValueFor()
//-----------------------------------------------------------------------------
QString SingleAddressBlockEditor::formattedValueFor(QString const& expression) const
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
// Function: SingleAddressBlockEditor::onUsageSelected()
//-----------------------------------------------------------------------------
void SingleAddressBlockEditor::onUsageSelected(QString const& newUsage)
{
    General::Usage usage = General::str2Usage(newUsage, General::USAGE_COUNT);

    addressBlock_->setUsage(usage);
    registersEditor_->setEnabled(!addressBlock_->getRegisterData().isEmpty() || 
        (usage != General::RESERVED && usage != General::MEMORY));

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: SingleAddressBlockEditor::onAccessSelected()
//-----------------------------------------------------------------------------
void SingleAddressBlockEditor::onAccessSelected(QString const& newAccess)
{
    addressBlock_->setAccess(General::str2Access(newAccess, General::ACCESS_COUNT));

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: SingleAddressBlockEditor::onVolatileSelected()
//-----------------------------------------------------------------------------
void SingleAddressBlockEditor::onVolatileSelected(QString const& newVolatileValue)
{
    bool addressBlockIsVolatile = false;

    if (newVolatileValue == "true")
    {
        addressBlockIsVolatile = true;
    }

    addressBlock_->setVolatile(General::bool2BooleanValue(addressBlockIsVolatile));

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

    baseAddressEditor_->setFrameShadow(QFrame::Sunken);
    rangeEditor_->setFrameShadow(QFrame::Sunken);
    widthEditor_->setFrameShadow(QFrame::Sunken);

    QGroupBox* addressBlockDefinitionGroup = new QGroupBox(tr("Address block definition"));

    QFormLayout* addressBlockDefinitionLayout = new QFormLayout(addressBlockDefinitionGroup);

    addressBlockDefinitionLayout->addRow(tr("Base Address [AUB], f(x):"), baseAddressEditor_);
    addressBlockDefinitionLayout->addRow(tr("Range [AUB], f(x):"), rangeEditor_);
    addressBlockDefinitionLayout->addRow(tr("Width [bits], f(x):"), widthEditor_);
    
    usageEditor_ = new UsageComboBox(addressBlockDefinitionGroup);
    addressBlockDefinitionLayout->addRow(tr("Usage:"), usageEditor_);

    accessEditor_ = new AccessComboBox(addressBlockDefinitionGroup);
    addressBlockDefinitionLayout->addRow(tr("Access:"), accessEditor_);
    volatileEditor_ = new BooleanComboBox(addressBlockDefinitionGroup);
    addressBlockDefinitionLayout->addRow(tr("Volatile:"), volatileEditor_);

    connect(usageEditor_, SIGNAL(activated(QString const&)),
        this, SLOT(onUsageSelected(QString const&)), Qt::UniqueConnection);

    connect(accessEditor_, SIGNAL(activated(QString const&)),
        this, SLOT(onAccessSelected(QString const&)), Qt::UniqueConnection);

    connect(volatileEditor_, SIGNAL(activated(QString const&)),
        this, SLOT(onVolatileSelected(QString const&)), Qt::UniqueConnection);

    QHBoxLayout* topOfPageLayout = new QHBoxLayout();
    topOfPageLayout->addWidget(&nameEditor_, 0, Qt::AlignTop);
    topOfPageLayout->addWidget(addressBlockDefinitionGroup, 0);

    QWidget* topOfPageWidget = new QWidget();
    topOfPageWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    topOfPageWidget->setLayout(topOfPageLayout);
    topOfPageWidget->setContentsMargins(0, 0, 0, 0);

    QSplitter* verticalSplitter = new QSplitter(Qt::Vertical, scrollArea);
    verticalSplitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    verticalSplitter->addWidget(topOfPageWidget);
    verticalSplitter->addWidget(registersEditor_);
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

    connect(baseAddressEditor_, SIGNAL(editingFinished()), this, SLOT(onBaseAddressChanged()), Qt::UniqueConnection);
    connect(rangeEditor_, SIGNAL(editingFinished()), this, SLOT(onRangeChanged()), Qt::UniqueConnection);
    connect(widthEditor_, SIGNAL(editingFinished()), this, SLOT(onWidthChanged()), Qt::UniqueConnection);

    connect(&nameEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(registersEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(baseAddressEditor_, SIGNAL(editingFinished()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(rangeEditor_, SIGNAL(editingFinished()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(widthEditor_, SIGNAL(editingFinished()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    connect(&nameEditor_, SIGNAL(nameChanged()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);
    connect(registersEditor_, SIGNAL(graphicsChanged()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);
    connect(baseAddressEditor_, SIGNAL(editingFinished()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);
    connect(rangeEditor_, SIGNAL(editingFinished()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);
    connect(widthEditor_, SIGNAL(editingFinished()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);

    connect(this, SIGNAL(addressUnitBitsChanged(int)),
        registersEditor_, SIGNAL(addressUnitBitsChanged(int)), Qt::UniqueConnection);
}
//-----------------------------------------------------------------------------
// Function: SingleAddressBlockEditor::changeExpressionEditorSignalBlocks()
//-----------------------------------------------------------------------------
void SingleAddressBlockEditor::changeExpressionEditorSignalBlockStatus(bool blockStatus)
{
    baseAddressEditor_->blockSignals(blockStatus);
    rangeEditor_->blockSignals(blockStatus);
    widthEditor_->blockSignals(blockStatus);
}
