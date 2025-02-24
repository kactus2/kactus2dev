//-----------------------------------------------------------------------------
// File: ParameterDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 10.11.2014
//
// Description:
// Delegate that provides widgets for editing parameters.
//-----------------------------------------------------------------------------

#include "ParameterDelegate.h"

#include "ParameterColumns.h"

#include <IPXACTmodels/common/validators/namevalidator.h>
#include <IPXACTmodels/common/Choice.h>

#include <KactusAPI/include/ParameterFinder.h>
#include <KactusAPI/include/IPXactSystemVerilogParser.h>

#include <editors/ComponentEditor/common/ExpressionLineEditor.h>
#include <editors/ComponentEditor/common/ExpressionEditor.h>

#include <editors/ComponentEditor/parameters/Array/ParameterArrayModel.h>
#include <editors/ComponentEditor/parameters/Array/ArrayDelegate.h>
#include <editors/ComponentEditor/parameters/Array/ArrayView.h>

#include <common/KactusColors.h>

#include <QComboBox>
#include <QCompleter>
#include <QLineEdit>
#include <QPainter>
#include <QScrollArea>
#include <QSortFilterProxyModel>
#include <QComboBox>

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::ParameterDelegate()
//-----------------------------------------------------------------------------
ParameterDelegate::ParameterDelegate(QSharedPointer<QList<QSharedPointer<Choice> > > choices, 
    QAbstractItemModel* completionModel, QSharedPointer<ParameterFinder> finder,
    QSharedPointer<ExpressionFormatter> expressionFormatter, QObject* parent /*= 0*/) :
ChoiceCreatorDelegate(completionModel, finder, parent),
expressionFormatter_(expressionFormatter)
{
    setChoices(choices);
}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::~ParameterDelegate()
//-----------------------------------------------------------------------------
ParameterDelegate::~ParameterDelegate()
{

}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* ParameterDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const& option, 
    QModelIndex const& index ) const
{
    if (index.column() == nameColumn())
    {
        QWidget* editor = QStyledItemDelegate::createEditor(parent, option, index);
        
        QLineEdit* lineEditor = qobject_cast<QLineEdit*>(editor);

        if (lineEditor)
        {
            lineEditor->setValidator(new NameValidator(lineEditor));
        }

        return editor;
    }

    if (index.column() == choiceColumn()) 
    {
        return createChoiceSelector(parent);
    }
    else if (index.column() == formatColumn()) 
    {
        return createFormatSelector(parent);
    }
    else if (index.column() == resolveColumn())
    {
        return createResolveSelector(parent);
    }
    else if (index.column() == usageCountColumn())
    {
        QModelIndex valueIdIndex = index.sibling(index.row(), idColumn());
        QModelIndex nameIndex = index.sibling(index.row(), nameColumn());
        QString targetId = valueIdIndex.data(Qt::DisplayRole).toString();
        QString parameterName = nameIndex.data(Qt::DisplayRole).toString();
        emit(openReferenceTree(targetId, parameterName));

        return 0;
    }
    else if (index.column() == valueColumn() && !choiceNameOnRow(index).isEmpty())
    {
        QComboBox* newCombo(new QComboBox(parent));
        newCombo->setEditable(true);
     
        newCombo->addItem("<none>");
        newCombo->addItems(getChoiceComboItems(index));

        ExpressionLineEditor* expressionEditor = createExpressionLineEditor(parent);
        newCombo->setLineEdit(expressionEditor);

        return newCombo;
    }
    else if (index.column() == valueColumn() && valueIsArray(index))
    {
        ArrayView* editor = new ArrayView(parent);

        QScrollArea* scrollingWidget = new QScrollArea(parent);
        scrollingWidget->setWidgetResizable(true);
        scrollingWidget->setWidget(editor);

        scrollingWidget->parent()->installEventFilter(editor);

        return scrollingWidget;
    }

    else if (index.column() == descriptionColumn())
    {
        QTextEdit* editor = new QTextEdit(parent);
        editor->setMinimumHeight(120);
        return editor;
    }
    else
    {
        return ChoiceCreatorDelegate::createEditor(parent, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::setEditorData()
//-----------------------------------------------------------------------------
void ParameterDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const 
{
    if (index.column() == valueColumn() && valueIsArray(index))
    {
        ArrayView* view = dynamic_cast<ArrayView*>(dynamic_cast<QScrollArea*>(editor)->widget());

        QModelIndex arrayLeftIndex = index.sibling(index.row(), arrayLeftColumn());
        int arrayLeftValue = arrayLeftIndex.data(Qt::ToolTipRole).toInt();

        QModelIndex arrayRightIndex = index.sibling(index.row(), arrayRightColumn());
        int arrayRightValue = arrayRightIndex.data(Qt::ToolTipRole).toInt();

        int arraySize = getArraySize(arrayLeftValue, arrayRightValue);

        int arrayStartIndex = arrayLeftValue;
        if (arrayRightValue < arrayLeftValue)
        {
            arrayStartIndex = arrayRightValue;
        }

        QSharedPointer<IPXactSystemVerilogParser> expressionParser(new IPXactSystemVerilogParser(
            getParameterFinder()));

        QSharedPointer<Choice> selectedChoice = findChoice(index);

        ParameterArrayModel* model = new ParameterArrayModel(arraySize, expressionParser, getParameterFinder(),
            expressionFormatter_, selectedChoice, KactusColors::MANDATORY_FIELD, arrayStartIndex, docRevision_, view);

        QModelIndex valueIndex = index.sibling(index.row(), valueColumn());
        QString parameterValue = valueIndex.data(Qt::EditRole).toString();
        model->setArrayData(parameterValue);

        QModelIndex typeIndex = index.sibling(index.row(), formatColumn());
        QString parameterType = typeIndex.data(Qt::EditRole).toString();
        model->setParameterType(parameterType);

        view->setItemDelegate(new ArrayDelegate(completionModel_, getParameterFinder(), selectedChoice,
            this->parent()));

        view->setModel(model);
        view->setSortingEnabled(false);
        view->resizeColumnsToContents();

        connect(model, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
        connect(model, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
            this, SIGNAL(contentChanged()), Qt::UniqueConnection);

        connect(view->itemDelegate(), SIGNAL(increaseReferences(QString)),
            this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
        connect(view->itemDelegate(), SIGNAL(decreaseReferences(QString)),
            this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);
    }
    else if (qobject_cast<QComboBox*>(editor))
    {
        QString text = index.data(Qt::DisplayRole).toString();
        QComboBox* combo = qobject_cast<QComboBox*>(editor);

        if (index.column() == valueColumn())
        {
            if (text.isEmpty())
            {
                text = "<none>";
            }
            else
            {
                QString enumerationValue = findEnumerationValue(index, text);
                if (!enumerationValue.isEmpty())
                {
                    text = text.prepend(enumerationValue + ":");
                }
            }
        }

        if (combo->isEditable())
        {
            ExpressionLineEditor* expressionEditor = qobject_cast<ExpressionLineEditor*>(combo->lineEdit());
            if (expressionEditor)
            {
                QString dataText = index.data(Qt::EditRole).toString();
                expressionEditor->setExpression(index.data(Qt::EditRole).toString());
            }
        }

        int comboIndex = combo->findText(text);
        if (comboIndex < 0)
        {
            combo->addItem(text);
            comboIndex = combo->findText(text);
        }

        combo->setCurrentIndex(comboIndex);

    }
	else if (index.column() == usageCountColumn())
    {
        // Do nothing.
	}
    else if (index.column() == descriptionColumn())
    {
        QString text = index.data(Qt::EditRole).toString();
        QTextEdit* textEdit = qobject_cast<QTextEdit*>(editor);

        textEdit->setText(text);
    }
    else
    {
        ChoiceCreatorDelegate::setEditorData(editor, index);
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::setModelData()
//-----------------------------------------------------------------------------
void ParameterDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, 
    QModelIndex const& index) const 
{
    if (index.column() == valueColumn() && valueIsArray(index))
    {
        QScrollArea* scrollWidget = qobject_cast<QScrollArea*>(editor);
        ArrayView* arrayTable = qobject_cast<ArrayView*>(scrollWidget->widget());
        ParameterArrayModel* arrayModel = qobject_cast<ParameterArrayModel*>(arrayTable->model());

        QString arrayValue = arrayModel->getArrayData();
        model->setData(index, arrayValue, Qt::EditRole);
    }
    else if (qobject_cast<QComboBox*>(editor))
    {
        QComboBox* combo = qobject_cast<QComboBox*>(editor);
        QString text = combo->currentText();
        
        if (text == "<none>")
        {
            text = "";
        }
        else if (index.column() == valueColumn() && text.contains(":"))
        {
            text = text.left(text.indexOf(":"));
        }
        else if (combo->isEditable())
        {
            ExpressionLineEditor* expressionEditor = qobject_cast<ExpressionLineEditor*>(combo->lineEdit());
            if (expressionEditor)
            {
                expressionEditor->finishEditingCurrentWord();

                text = expressionEditor->getExpression();
            }
        }

        model->setData(index, text, Qt::EditRole);
    }
    else if (index.column() == descriptionColumn())
    {
        QTextEdit* textEdit = qobject_cast<QTextEdit*>(editor);

        model->setData(index, textEdit->toPlainText(), Qt::EditRole);
    }
	else 
    {
        ChoiceCreatorDelegate::setModelData(editor, model, index);
	}
}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::paint()
//-----------------------------------------------------------------------------
void ParameterDelegate::paint(QPainter *painter, QStyleOptionViewItem const& option, 
    QModelIndex const& index) const
{
    ChoiceCreatorDelegate::paint(painter, option, index);

    if (index.column() == maximumColumn() || index.column() == valueColumn() || 
        index.column() == arrayRightColumn())
    {
        QPen oldPen = painter->pen();
        QPen newPen(Qt::lightGray);
        newPen.setWidth(2);
        painter->setPen(newPen);
        painter->drawLine(option.rect.topRight() + QPoint(1,1), option.rect.bottomRight() + QPoint(1,1));
        painter->setPen(oldPen);
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::nameColumn()
//-----------------------------------------------------------------------------
int ParameterDelegate::nameColumn() const
{
    return ParameterColumns::NAME;
}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::choiceColumn()
//-----------------------------------------------------------------------------
int ParameterDelegate::choiceColumn() const
{
    return ParameterColumns::CHOICE;
}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::formatColumn()
//-----------------------------------------------------------------------------
int ParameterDelegate::formatColumn() const
{
    return ParameterColumns::TYPE;
}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::bitWidthLeftColumn()
//-----------------------------------------------------------------------------
int ParameterDelegate::bitWidthLeftColumn() const
{
    return ParameterColumns::BITWIDTH_LEFT;
}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::bitWidthRightColumn()
//-----------------------------------------------------------------------------
int ParameterDelegate::bitWidthRightColumn() const
{
    return ParameterColumns::BITWIDTH_RIGHT;
}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::minimumColumn()
//-----------------------------------------------------------------------------
int ParameterDelegate::minimumColumn() const
{
    return ParameterColumns::MINIMUM;
}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::maximumColumn()
//-----------------------------------------------------------------------------
int ParameterDelegate::maximumColumn() const
{
    return ParameterColumns::MAXIMUM;
}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::valueColumn()
//-----------------------------------------------------------------------------
int ParameterDelegate::valueColumn() const
{
    return ParameterColumns::VALUE;
}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::resolveColumn()
//-----------------------------------------------------------------------------
int ParameterDelegate::resolveColumn() const
{
    return ParameterColumns::RESOLVE;
}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::descriptionColumn()
//-----------------------------------------------------------------------------
int ParameterDelegate::descriptionColumn() const
{
    return ParameterColumns::DESCRIPTION;
}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::arrayLeftColumn()
//-----------------------------------------------------------------------------
int ParameterDelegate::arrayLeftColumn() const
{
    return ParameterColumns::ARRAY_LEFT;
}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::arrayRightColumn()
//-----------------------------------------------------------------------------
int ParameterDelegate::arrayRightColumn() const
{
    return ParameterColumns::ARRAY_RIGHT;
}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::usageCountColumn()
//-----------------------------------------------------------------------------
int ParameterDelegate::usageCountColumn() const
{
    return ParameterColumns::USAGE_COUNT;
}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::valueIdColumn()
//-----------------------------------------------------------------------------
int ParameterDelegate::idColumn() const
{
    return ParameterColumns::ID;
}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::columnAcceptsExpression()
//-----------------------------------------------------------------------------
bool ParameterDelegate::columnAcceptsExpression(int column) const
{
    return column == valueColumn() || column == bitWidthLeftColumn() || column == bitWidthRightColumn() ||
        column == arrayLeftColumn() || column == arrayRightColumn();
}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::choiceNameOnRow()
//-----------------------------------------------------------------------------
QString ParameterDelegate::choiceNameOnRow(QModelIndex const& index) const
{
    return index.sibling(index.row(), choiceColumn()).data().toString();
}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::createFormatEditor()
//-----------------------------------------------------------------------------
QWidget* ParameterDelegate::createFormatSelector(QWidget* parent) const
{
    QComboBox* combo = new QComboBox(parent);
    combo->addItem(QString(""));
    combo->addItem(QString("bit"));
    combo->addItem(QString("byte"));
    combo->addItem(QString("shortint"));
    combo->addItem(QString("int"));
    combo->addItem(QString("longint"));
    combo->addItem(QString("shortreal"));
    combo->addItem(QString("real"));
    combo->addItem(QString("string"));
    return combo;
}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::createChoiceSelector()
//-----------------------------------------------------------------------------
QWidget* ParameterDelegate::createChoiceSelector(QWidget* parent) const
{
    QComboBox* combo = new QComboBox(parent);
    combo->addItem(QString("<none>"));

    QSharedPointer<QList<QSharedPointer<Choice> > > choices = getChoices();
    if (choices)
    {
        foreach (QSharedPointer<Choice> choice, *getChoices())
        {
            combo->addItem(choice->name());
        }
    }

    return combo;
}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::createResolveSelector()
//-----------------------------------------------------------------------------
QWidget* ParameterDelegate::createResolveSelector(QWidget* parent) const
{
    QComboBox* combo = new QComboBox(parent);
    combo->addItem(QString("immediate"));
    combo->addItem(QString("user"));
    combo->addItem(QString("generated"));
    combo->addItem(QString());
    return combo;
}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::setStdRevision()
//-----------------------------------------------------------------------------
void ParameterDelegate::setStdRevision(Document::Revision docRevision)
{
    docRevision_ = docRevision;
}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::valueIsArray()
//-----------------------------------------------------------------------------
bool ParameterDelegate::valueIsArray(QModelIndex const& index) const
{
    QModelIndex arrayLeftIndex = index.sibling(index.row(), arrayLeftColumn());
    bool arrayLeftIsOk = true;
    int arrayLeftValue = arrayLeftIndex.data(Qt::ToolTipRole).toInt(&arrayLeftIsOk);
    
    QModelIndex arrayRightIndex = index.sibling(index.row(), arrayRightColumn());
    bool arrayRightIsOk = true;
    int arrayRightValue = arrayRightIndex.data(Qt::ToolTipRole).toInt(&arrayRightIsOk);

    int arraySize = getArraySize(arrayLeftValue, arrayRightValue);

    return arrayLeftIsOk && arrayRightIsOk && arraySize > 0;
}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::updateEditorGeometry()
//-----------------------------------------------------------------------------
void ParameterDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
    const QModelIndex &index) const
{
    ExpressionDelegate::updateEditorGeometry(editor, option, index);

    if (index.column() == valueColumn() && valueIsArray(index))
    {
        repositionAndResizeEditor(editor, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::repositionAndResizeEditor()
//-----------------------------------------------------------------------------
void ParameterDelegate::repositionAndResizeEditor(QWidget* editor, QStyleOptionViewItem const& option,
    QModelIndex const& index) const
{   
    QModelIndex arrayLeftIndex = index.sibling(index.row(), arrayLeftColumn());
    int arrayLeftValue = arrayLeftIndex.data(Qt::ToolTipRole).toInt();

    QModelIndex arrayRightIndex = index.sibling(index.row(), arrayRightColumn());
    int arrayRightValue = arrayRightIndex.data(Qt::ToolTipRole).toInt();

    int arraySize = getArraySize(arrayLeftValue, arrayRightValue);

    int editorMinimumSize = 24 * (arraySize + 1);

    editor->setFixedWidth(300);

    const int PARENT_HEIGHT = editor->parentWidget()->height();
    const int AVAILABLE_HEIGHT_BELOW = PARENT_HEIGHT - option.rect.top();

    if (AVAILABLE_HEIGHT_BELOW > editorMinimumSize)
    {
        editor->move(option.rect.topLeft());
    }
    else
    {
        int editorNewY = PARENT_HEIGHT-editorMinimumSize;
        if (editorNewY <= 0)
        {
            editorNewY = 0;
        }

        editor->move(option.rect.left(), editorNewY);
    }

    if (editorMinimumSize > PARENT_HEIGHT)
    {
        editor->setFixedHeight(PARENT_HEIGHT);
    }
    else
    {
        editor->setFixedHeight(editorMinimumSize);
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::getArraySize()
//-----------------------------------------------------------------------------
int ParameterDelegate::getArraySize(int arrayLeftValue, int arrayRightValue) const
{
    return abs(arrayLeftValue - arrayRightValue) + 1;
}
