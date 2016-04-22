//-----------------------------------------------------------------------------
// File: RemapConditionDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 09.04.2015
//
// Description:
// Delegate that provides widgets for editing remap condition.
//-----------------------------------------------------------------------------

#include "RemapConditionDelegate.h"

#include "RemapConditionColumns.h"

#include <editors/ComponentEditor/common/ParameterFinder.h>
#include <editors/ComponentEditor/common/ParameterCompleter.h>
#include <editors/ComponentEditor/common/IPXactSystemVerilogParser.h>

#include <editors/ComponentEditor/common/ReferenceSelector/ReferenceSelector.h>

#include <editors/ComponentEditor/remapStates/ValueOrIndexedValueEditor.h>

#include <IPXACTmodels/Component/Choice.h>

#include <QScrollArea>
#include <QColor>

//-----------------------------------------------------------------------------
// Function: RemapConditionDelegate::RemapConditionDelegate()
//-----------------------------------------------------------------------------
RemapConditionDelegate::RemapConditionDelegate(QStringList const& portNameList, QCompleter* parameterCompleter,
                        QSharedPointer<ParameterFinder> finder, QSharedPointer<ExpressionParser> expressionParser,
                        QSharedPointer<ExpressionFormatter> expressionFormatter, QObject* parent):
ExpressionDelegate(parameterCompleter, finder, parent),
expressionFormatter_(expressionFormatter),
expressionParser_(expressionParser),
availablePortNames_(portNameList)
{

}

//-----------------------------------------------------------------------------
// Function: RemapConditionDelegate::~RemapConditionDelegate()
//-----------------------------------------------------------------------------
RemapConditionDelegate::~RemapConditionDelegate()
{

}

//-----------------------------------------------------------------------------
// Function: RemapConditionDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* RemapConditionDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const& option,
    QModelIndex const& index) const
{
    if (index.column() == RemapConditionColumns::NAME_COLUMN)
    {
        ReferenceSelector* referenceSelector = new ReferenceSelector(parent);

        QStringList visiblePortNames = availablePortNames_;
        QString selectedName = index.model()->data(index, Qt::DisplayRole).toString();
        if (componentPortNames_.contains(selectedName))
        {
            visiblePortNames.append(selectedName);
        }

        referenceSelector->refresh(visiblePortNames);

        return referenceSelector;
    }

    else if (index.column() == RemapConditionColumns::VALUE_COLUMN && valueIsArray(index))
    {
        ValueOrIndexedValueEditor* editor =
            new ValueOrIndexedValueEditor(getParameterFinder(), expressionParser_, expressionFormatter_, parent);

        parent->installEventFilter(editor);

        connect(editor, SIGNAL(increaseReferences(QString)),
            this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
        connect(editor, SIGNAL(decreaseReferences(QString)),
            this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

        return editor;
    }

    else
    {
        return ExpressionDelegate::createEditor(parent, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: RemapConditionDelegate::setEditorData()
//-----------------------------------------------------------------------------
void RemapConditionDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const
{
    if (index.column() == RemapConditionColumns::NAME_COLUMN)
    {
        QString text = index.model()->data(index, Qt::DisplayRole).toString();
        ReferenceSelector* referenceSelector = qobject_cast<ReferenceSelector*>(editor);

        referenceSelector->selectItem(text);
    }
    else if (index.column() == RemapConditionColumns::VALUE_COLUMN && valueIsArray(index))
    {
        setValueAndArrayEditor(editor, index);
    }
    else
    {
        ExpressionDelegate::setEditorData(editor, index);
    }
}

//-----------------------------------------------------------------------------
// Function: RemapConditionDelegate::setValueAndArrayEditor()
//-----------------------------------------------------------------------------
void RemapConditionDelegate::setValueAndArrayEditor(QWidget* editor, QModelIndex const& index) const
{
    QModelIndex portLeftIndex = index.sibling(index.row(), RemapConditionColumns::LEFT_COLUMN);
    int portLeft = portLeftIndex.data(Qt::ToolTipRole).toInt();

    QModelIndex portRightIndex = index.sibling(index.row(), RemapConditionColumns::RIGHT_COLUMN);
    int portRight = portRightIndex.data(Qt::ToolTipRole).toInt();

    QModelIndex valueIndex = index.sibling(index.row(), RemapConditionColumns::VALUE_COLUMN);
    QString portValue = valueIndex.data(Qt::EditRole).toString();

    ValueOrIndexedValueEditor* valueEditor = qobject_cast<ValueOrIndexedValueEditor*>(editor);
    if (valueEditor)
    {
        valueEditor->setupArrayEditor(portValue, portLeft, portRight);
    }
}

//-----------------------------------------------------------------------------
// Function: RemapConditionDelegate::setModelData()
//-----------------------------------------------------------------------------
void RemapConditionDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index)
    const
{
    if (index.column() == RemapConditionColumns::NAME_COLUMN)
    {
        ReferenceSelector* referenceSelector = qobject_cast<ReferenceSelector*>(editor);
        QString text = referenceSelector->currentText();
        model->setData(index, text, Qt::EditRole);
    }
    else if (index.column() == RemapConditionColumns::VALUE_COLUMN && valueIsArray(index))
    {
        ValueOrIndexedValueEditor* valueEditor = qobject_cast<ValueOrIndexedValueEditor*>(editor);
        if (valueEditor)
        {
            QString arrayValue = valueEditor->getValueData();

            model->setData(index, arrayValue, Qt::EditRole);
        }
    }
    else
    {
        ExpressionDelegate::setModelData(editor, model, index);
    }
}

//-----------------------------------------------------------------------------
// Function: RemapConditionDelegate::columnAcceptsExpression()
//-----------------------------------------------------------------------------
bool RemapConditionDelegate::columnAcceptsExpression(int column) const
{
    return column == RemapConditionColumns::VALUE_COLUMN;
}

//-----------------------------------------------------------------------------
// Function: RemapConditionDelegate::descriptionColumn()
//-----------------------------------------------------------------------------
int RemapConditionDelegate::descriptionColumn() const
{
    return -1;
}

//-----------------------------------------------------------------------------
// Function: RemapConditionDelegate::valueIsArray()
//-----------------------------------------------------------------------------
bool RemapConditionDelegate::valueIsArray(QModelIndex const& index) const
{
    QModelIndex portLeftIndex = index.sibling(index.row(), RemapConditionColumns::LEFT_COLUMN);
    bool portLeftIsOk = true;
    portLeftIndex.data(Qt::ToolTipRole).toInt(&portLeftIsOk);

    QModelIndex portRightIndex = index.sibling(index.row(), RemapConditionColumns::RIGHT_COLUMN);
    bool portRightIsOk = true;
    portRightIndex.data(Qt::ToolTipRole).toInt(&portRightIsOk);

    int portWidth = getPortWidth(index);

    return portLeftIsOk && portRightIsOk && portWidth > 1;
}

//-----------------------------------------------------------------------------
// Function: RemapConditionDelegate::getPortWidth()
//-----------------------------------------------------------------------------
int RemapConditionDelegate::getPortWidth(QModelIndex const& index) const
{
    QModelIndex portLeftIndex = index.sibling(index.row(), RemapConditionColumns::LEFT_COLUMN);
    int portLeft = portLeftIndex.data(Qt::ToolTipRole).toInt();

    QModelIndex portRightIndex = index.sibling(index.row(), RemapConditionColumns::RIGHT_COLUMN);
    int portRight = portRightIndex.data(Qt::ToolTipRole).toInt();

    return abs(portLeft - portRight) + 1;
}

//-----------------------------------------------------------------------------
// Function: RemapConditionDelegate::updateEditorGeometry()
//-----------------------------------------------------------------------------
void RemapConditionDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
    const QModelIndex &index) const
{
    ExpressionDelegate::updateEditorGeometry(editor, option, index);

    if (index.column() == RemapConditionColumns::VALUE_COLUMN && valueIsArray(index))
    {
        repositionAndResizeEditor(editor, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: RemapConditionDelegate::repositionAndResizeEditor()
//-----------------------------------------------------------------------------
void RemapConditionDelegate::repositionAndResizeEditor(QWidget* editor, QStyleOptionViewItem const& option,
    QModelIndex const& index) const
{
    int portWidth = getPortWidth(index);

    const int ROWHEIGHT = 23;

    int editorMinimumSize = ROWHEIGHT * (portWidth + 4);

    const int PARENT_HEIGHT = editor->parentWidget()->height();
    const int AVAILABLE_HEIGHT_BELOW = PARENT_HEIGHT - option.rect.top();

    if (AVAILABLE_HEIGHT_BELOW > editorMinimumSize)
    {
        int movementY = option.rect.topLeft().y() - 20;

        if (movementY <= 0)
        {
            movementY = 0;
        }

        QPoint targetPoint (option.rect.topLeft().x(), movementY);

        editor->move(targetPoint);
    }
    else
    {
        int editorNewY = PARENT_HEIGHT - editorMinimumSize;

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
// Function: RemapConditionDelegate::setNewPortNames()
//-----------------------------------------------------------------------------
void RemapConditionDelegate::setNewPortNames(QStringList const& newPorts)
{
    availablePortNames_ = newPorts;
}

//-----------------------------------------------------------------------------
// Function: RemapConditionDelegate::setComponentPorts()
//-----------------------------------------------------------------------------
void RemapConditionDelegate::setComponentPorts(QStringList const& componentPorts)
{
    componentPortNames_ = componentPorts;
}
