/* 
 *  	Created on: 24.8.2012
 *      Author: Antti Kamppi
 * 		filename: addressblockeditor.cpp
 *		Project: Kactus 2
 */

#include "SingleAddressBlockEditor.h"

#include <common/widgets/usageComboBox/usagecombobox.h>
#include <common/widgets/accessComboBox/accesscombobox.h>
#include <common/widgets/booleanComboBox/booleancombobox.h>

#include <editors/ComponentEditor/common/ExpressionEditor.h>
#include <editors/ComponentEditor/common/ParameterCompleter.h>
#include <editors/ComponentEditor/common/ValueFormatter.h>
#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>

#include <QFormLayout>
#include <QScrollArea>
#include <QAbstractItemView>

//-----------------------------------------------------------------------------
// Function: SingleAddressBlockEditor::SingleAddressBlockEditor()
//-----------------------------------------------------------------------------
SingleAddressBlockEditor::SingleAddressBlockEditor(QSharedPointer<AddressBlock> addressBlock,
    QSharedPointer<Component> component, LibraryInterface* handler,
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ExpressionFormatter> expressionFormatter,
    QWidget* parent /* = 0 */):
ItemEditor(component, handler, parent),
nameEditor_(addressBlock->getNameGroup(), this, tr("Address block name and description")),
registersEditor_(new AddressBlockEditor(addressBlock, component, handler, parameterFinder, expressionFormatter, this)),
usageEditor_(),
baseAddressEditor_(new ExpressionEditor(parameterFinder, this)),
rangeEditor_(new ExpressionEditor(parameterFinder, this)),
widthEditor_(new ExpressionEditor(parameterFinder, this)),
accessEditor_(),
volatileEditor_(),
addressBlock_(addressBlock),
expressionParser_(new IPXactSystemVerilogParser(parameterFinder))
{
    baseAddressEditor_->setFixedHeight(20);
    rangeEditor_->setFixedHeight(20);
    widthEditor_->setFixedHeight(20);

    ComponentParameterModel* componentParametersModel = new ComponentParameterModel(parameterFinder, this);
    componentParametersModel->setExpressionParser(expressionParser_);

    ParameterCompleter* baseAddressEditorCompleter = new ParameterCompleter(this);
    baseAddressEditorCompleter->setModel(componentParametersModel);
    baseAddressEditorCompleter->popup()->hide();

    ParameterCompleter* rangeEditorCompleter = new ParameterCompleter(this);
    rangeEditorCompleter->setModel(componentParametersModel);
    rangeEditorCompleter->popup()->hide();

    ParameterCompleter* widthEditorCompleter = new ParameterCompleter(this);
    widthEditorCompleter->setModel(componentParametersModel);
    widthEditorCompleter->popup()->hide();
    
    baseAddressEditor_->setAppendingCompleter(baseAddressEditorCompleter);
    rangeEditor_->setAppendingCompleter(rangeEditorCompleter);
    widthEditor_->setAppendingCompleter(widthEditorCompleter);

    baseAddressEditor_->setToolTip(formattedValueFor(addressBlock_->getBaseAddress()));
    rangeEditor_->setToolTip(formattedValueFor(addressBlock_->getRange()));
    widthEditor_->setToolTip(formattedValueFor(addressBlock_->getWidthExpression()));

    setupLayout();

    connectSignals();
}

//-----------------------------------------------------------------------------
// Function: SingleAddressBlockEditor::~SingleAddressBlockEditor()
//-----------------------------------------------------------------------------
SingleAddressBlockEditor::~SingleAddressBlockEditor()
{

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

    usageEditor_ = new UsageComboBox(addressBlockDefinitionGroup);
    addressBlockDefinitionLayout->addRow(tr("Usage:"), usageEditor_);
    addressBlockDefinitionLayout->addRow(tr("Base Address [AUB]:"), baseAddressEditor_);
    addressBlockDefinitionLayout->addRow(tr("Range [AUB]:"), rangeEditor_);
    addressBlockDefinitionLayout->addRow(tr("Width [bits]:"), widthEditor_);

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

    QVBoxLayout* registerEditorLayout = new QVBoxLayout();
    registerEditorLayout->addWidget(registersEditor_, 0);

    QWidget* topWidget = new QWidget(scrollArea);
    topWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    scrollArea->setWidget(topWidget);

    QVBoxLayout* topLayout = new QVBoxLayout(topWidget);
    topLayout->addLayout(topOfPageLayout);
    topLayout->addLayout(registerEditorLayout, 1);
    topLayout->setContentsMargins(0, 0, 0, 0);
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

    usageEditor_->setCurrentValue(addressBlock_->getUsage());

    changeExpressionEditorSignalBlockStatus(true);

    baseAddressEditor_->setExpression(addressBlock_->getBaseAddress());
    rangeEditor_->setExpression(addressBlock_->getRange());
    widthEditor_->setExpression(addressBlock_->getWidthExpression());

    changeExpressionEditorSignalBlockStatus(false);

    accessEditor_->setCurrentValue(addressBlock_->getAccess());
    volatileEditor_->setCurrentValue(General::BooleanValue2Bool(addressBlock_->getVolatile(), true));
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

    widthEditor_->setToolTip(formattedValueFor(addressBlock_->getWidthExpression()));
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
    addressBlock_->setUsage(General::str2Usage(newUsage, General::USAGE_COUNT));

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
//void SingleAddressBlockEditor::onVolatileSelected(bool newVolatileValue)
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
