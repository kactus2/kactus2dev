//-----------------------------------------------------------------------------
// File: ValueOrIndexedValueEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 20.04.2016
//
// Description:
// Editor for changing values in items that can be given as a single value or array.
//-----------------------------------------------------------------------------

#include "ValueOrIndexedValueEditor.h"


#include <editors/ComponentEditor/common/ExpressionEditor.h>
#include <editors/ComponentEditor/common/ComponentParameterFinder.h>
#include <editors/ComponentEditor/common/ParameterCompleter.h>
#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>
#include <editors/ComponentEditor/parameters/Array/ArrayView.h>
#include <editors/ComponentEditor/parameters/Array/ArrayDelegate.h>
#include <editors/ComponentEditor/parameters/Array/ParameterArrayModel.h>

#include <IPXACTmodels/common/validators/ValueFormatter.h>

#include <QHeaderView>
#include <QEvent>
#include <QLabel>
#include <QScrollArea>
#include <QHBoxLayout>

//-----------------------------------------------------------------------------
// Function: ValueOrIndexedValueEditor::ValueOrIndexedValueEditor()
//-----------------------------------------------------------------------------
ValueOrIndexedValueEditor::ValueOrIndexedValueEditor(QSharedPointer<ParameterFinder> finder,
    QSharedPointer<ExpressionParser> parser, QSharedPointer<ExpressionFormatter> formatter, QWidget* parent):
QTableView(parent),
singleValueEditor_(new ExpressionEditor(finder)),
arrayView_(new ArrayView(this)),
expressionParser_(parser),
parameterFinder_(finder),
expressionFormatter_(formatter),
parameterModel_(new ComponentParameterModel(finder, this))
{
    parameterModel_->setExpressionParser(parser);

    setupSingleValueEditor();

    setupLayout();

    setFocusProxy(singleValueEditor_);
}

//-----------------------------------------------------------------------------
// Function: ValueOrIndexedValueEditor::~ValueOrIndexedValueEditor()
//-----------------------------------------------------------------------------
ValueOrIndexedValueEditor::~ValueOrIndexedValueEditor()
{

}

//-----------------------------------------------------------------------------
// Function: ValueOrIndexedValueEditor::setupSingleValueEditor()
//-----------------------------------------------------------------------------
void ValueOrIndexedValueEditor::setupSingleValueEditor()
{
    singleValueEditor_->setFixedHeight(20);

    QCompleter* singleCompleter = createParameterNameCompleter(singleValueEditor_);
    singleValueEditor_->setAppendingCompleter(singleCompleter);

    connect(singleValueEditor_, SIGNAL(increaseReference(QString const&)),
        this, SIGNAL(increaseReferences(QString const&)), Qt::UniqueConnection);
    connect(singleValueEditor_, SIGNAL(decreaseReference(QString const&)),
        this, SIGNAL(decreaseReferences(QString const&)), Qt::UniqueConnection);
    
    connect(singleValueEditor_, SIGNAL(editingFinished()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(singleValueEditor_, SIGNAL(editingFinished()), this, SLOT(onSingleValueEdited()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ValueOrIndexedValueEditor::onSingleValueEdited()
//-----------------------------------------------------------------------------
void ValueOrIndexedValueEditor::onSingleValueEdited()
{
    singleValueEditor_->finishEditingCurrentWord();

    QString newSingleValue = singleValueEditor_->getExpression();

    singleValueEditor_->setToolTip(formattedValueFor(newSingleValue));

    if (newSingleValue.isEmpty())
    {
        arrayView_->setDisabled(false);
    }
    else
    {
        arrayView_->setDisabled(true);
    }
}

//-----------------------------------------------------------------------------
// Function: ValueOrIndexedValueEditor::createParameterNameCompleter()
//-----------------------------------------------------------------------------
QCompleter* ValueOrIndexedValueEditor::createParameterNameCompleter(QObject* completerParent) const
{
    ParameterCompleter* expressionCompleter = new ParameterCompleter(completerParent);
    expressionCompleter->setModel(parameterModel_);

    return expressionCompleter;
}

//-----------------------------------------------------------------------------
// Function: ValueOrIndexedValueEditor::setupLayout()
//-----------------------------------------------------------------------------
void ValueOrIndexedValueEditor::setupLayout()
{
    QVBoxLayout* editorLayout = new QVBoxLayout(this);

    editorLayout->setMargin(0);
    editorLayout->setContentsMargins(0,0,0,0);

    QString singleExplanation = tr("Set the value for the whole port");
    QLabel* singleEditorLabel = new QLabel(singleExplanation, this);

    editorLayout->addWidget(singleEditorLabel);
    editorLayout->addWidget(singleValueEditor_);

    QScrollArea* arrayScroller = new QScrollArea(this);
    arrayScroller->setWidgetResizable(true);
    arrayScroller->parent()->installEventFilter(arrayView_);
    arrayScroller->setWidget(arrayView_);

    QString arrayExplanation = tr("Or set the values for one or more bits of the port");
    QLabel* arrayLabel = new QLabel(arrayExplanation, this);

    editorLayout->addWidget(arrayLabel);
    editorLayout->addWidget(arrayScroller);

    singleValueEditor_->installEventFilter(this);
    arrayView_->installEventFilter(this);
    singleEditorLabel->installEventFilter(this);
    arrayLabel->installEventFilter(this);
}

//-----------------------------------------------------------------------------
// Function: ValueOrIndexedValueEditor::setupArrayEditor()
//-----------------------------------------------------------------------------
void ValueOrIndexedValueEditor::setupArrayEditor(QString const& value, int const& leftBound, int const& rightBound)
{
    int arraySize = abs(leftBound - rightBound) + 1;

    int arrayStart = qMin(leftBound, rightBound);

    QSharedPointer<Choice> newChoice(new Choice());

    ParameterArrayModel* arrayModel = new ParameterArrayModel(arraySize, expressionParser_, parameterFinder_,
        expressionFormatter_, newChoice, QColor("white"), arrayStart, arrayView_);

    if (value.contains('{') && value.contains('}'))
    {
        arrayModel->setArrayData(value);

        singleValueEditor_->setDisabled(true);
    }
    else if (!value.isEmpty())
    {
        setValueToSingleEditor(value);

        arrayView_->setDisabled(true);
    }

    QCompleter* arrayCompleter = createParameterNameCompleter(arrayView_);

    arrayView_->setItemDelegate(new ArrayDelegate(arrayCompleter, parameterFinder_, newChoice, this->parent()));
    arrayView_->setModel(arrayModel);
    arrayView_->resizeColumnsToContents();

    connect(arrayModel, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
        this, SLOT(onDataChangeInArrayView()), Qt::UniqueConnection);
    connect(arrayModel, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    connect(arrayView_->itemDelegate(), SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(arrayView_->itemDelegate(), SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ValueOrIndexedValueEditor::onDataChangeInArrayView()
//-----------------------------------------------------------------------------
void ValueOrIndexedValueEditor::onDataChangeInArrayView()
{
    ParameterArrayModel* arrayModel = dynamic_cast<ParameterArrayModel*>(arrayView_->model());

    QStringList arrayData = arrayModel->getArrayDataAsList();

    foreach (QString singleBit, arrayData)
    {
        if (!singleBit.isEmpty())
        {
            singleValueEditor_->setDisabled(true);
            return;
        }
    }

    singleValueEditor_->setDisabled(false);
}

//-----------------------------------------------------------------------------
// Function: ValueOrIndexedValueEditor::setValueToSingleEditor()
//-----------------------------------------------------------------------------
void ValueOrIndexedValueEditor::setValueToSingleEditor(QString const& value)
{
    singleValueEditor_->blockSignals(true);

    singleValueEditor_->setExpression(value);
    singleValueEditor_->setToolTip(formattedValueFor(value));

    singleValueEditor_->blockSignals(false);
}

//-----------------------------------------------------------------------------
// Function: ValueOrIndexedValueEditor::formattedValueFor()
//-----------------------------------------------------------------------------
QString ValueOrIndexedValueEditor::formattedValueFor(QString const& expression) const
{
    if (expressionParser_->isPlainValue(expression))
    {
        return expression;
    }
    else if (expressionParser_->isValidExpression(expression))
    {
        ValueFormatter formatter;
        return formatter.format(expressionParser_->parseExpression(expression),
            expressionParser_->baseForExpression(expression));
    }
    else
    {
        return "n/a";
    }
}

//-----------------------------------------------------------------------------
// Function: ValueOrIndexedValueEditor::getValueData()
//-----------------------------------------------------------------------------
QString ValueOrIndexedValueEditor::getValueData() const
{
    QString singleValue = singleValueEditor_->getExpression();

    QString arrayData;

    ParameterArrayModel* arrayModel = dynamic_cast<ParameterArrayModel*>(arrayView_->model());
    if (arrayModel)
    {
        arrayData = arrayModel->getArrayData();
    }

    QString returnValue ("");

    if (arrayContainsValues(arrayData))
    {
        returnValue = arrayData;
    }
    else if (!singleValue.isEmpty())
    {
        returnValue = singleValue;
    }

    return returnValue;
}

//-----------------------------------------------------------------------------
// Function: ValueOrIndexedValueEditor::arrayContainsValues()
//-----------------------------------------------------------------------------
bool ValueOrIndexedValueEditor::arrayContainsValues(QString const& arrayContents) const
{
    QStringList arrayData = arrayContents.split(',');
    arrayData.first().remove('{');
    arrayData.last().remove('}');

    foreach (QString singleBit, arrayData)
    {
        if (!singleBit.isEmpty())
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ValueOrIndexedValueEditor::eventFilter()
//-----------------------------------------------------------------------------
bool ValueOrIndexedValueEditor::eventFilter(QObject* filterObject, QEvent* filterEvent)
{
    QLabel* explanationLabel = qobject_cast<QLabel*>(filterObject);
    if ((explanationLabel || filterObject == singleValueEditor_ || filterObject == arrayView_ ) &&
        filterEvent->type() == QEvent::MouseButtonPress)
    {
        return true;
    }

    return QTableView::eventFilter(filterObject, filterEvent);
}
