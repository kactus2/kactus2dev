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

#include <editors/ComponentEditor/parameters/Array/ParameterArrayModel.h>
#include <editors/ComponentEditor/parameters/Array/ArrayDelegate.h>
#include <editors/ComponentEditor/parameters/Array/ArrayView.h>

#include <IPXACTmodels/choice.h>

#include <QScrollArea>
#include <QColor>

//-----------------------------------------------------------------------------
// Function: RemapConditionDelegate::RemapConditionDelegate()
//-----------------------------------------------------------------------------
RemapConditionDelegate::RemapConditionDelegate(QStringList const& portNameList, QCompleter* parameterCompleter,
    QSharedPointer<ParameterFinder> finder, QSharedPointer<ExpressionFormatter> expressionFormatter,
    QObject* parent /* = 0 */):
ExpressionDelegate(parameterCompleter, finder, parent),
expressionFormatter_(expressionFormatter),
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
        ArrayView* editor = new ArrayView(parent);

        QScrollArea* scrollingWidget = new QScrollArea(parent);
        scrollingWidget->setWidgetResizable(true);
        scrollingWidget->setWidget(editor);

        scrollingWidget->parent()->installEventFilter(editor);

        return scrollingWidget;
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
        setArrayEditor(editor, index);
    }
    else
    {
        ExpressionDelegate::setEditorData(editor, index);
    }
}

//-----------------------------------------------------------------------------
// Function: RemapConditionDelegate::setArrayEditor()
//-----------------------------------------------------------------------------
void RemapConditionDelegate::setArrayEditor(QWidget* editor, QModelIndex const& index) const
{
    ArrayView* view = dynamic_cast<ArrayView*>(dynamic_cast<QScrollArea*>(editor)->widget());

    int arraySize = getPortWidth(index);

    QSharedPointer<IPXactSystemVerilogParser> expressionParser(new IPXactSystemVerilogParser(
        getParameterFinder()));

    QModelIndex portLeftIndex = index.sibling(index.row(), RemapConditionColumns::LEFT_COLUMN);
    int portLeft = portLeftIndex.data(Qt::ToolTipRole).toInt();

    QModelIndex portRightIndex = index.sibling(index.row(), RemapConditionColumns::RIGHT_COLUMN);
    int portRight = portRightIndex.data(Qt::ToolTipRole).toInt();

    int arrayStart = portLeft;
    if (portRight < portLeft)
    {
        arrayStart = portRight;
    }

    ParameterArrayModel* model = new ParameterArrayModel(arraySize, expressionParser, getParameterFinder(),
        expressionFormatter_, QSharedPointer<Choice>(new Choice()), QColor("white"), arrayStart, view);

    QModelIndex valueIndex = index.sibling(index.row(), RemapConditionColumns::VALUE_COLUMN);
    QString portValue = valueIndex.data(Qt::EditRole).toString();
    model->setArrayData(portValue);

    view->setItemDelegate(new ArrayDelegate(getNameCompleter(), getParameterFinder(),
        QSharedPointer<Choice>(new Choice()), this->parent()));

    view->setModel(model);

    view->setSortingEnabled(false);
    view->resizeColumnsToContents();

    connect(model, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    connect(view->itemDelegate(), SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(view->itemDelegate(), SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);
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
        QScrollArea* scrollWidget = qobject_cast<QScrollArea*>(editor);
        ArrayView* arrayTable = qobject_cast<ArrayView*>(scrollWidget->widget());
        ParameterArrayModel* arrayModel = qobject_cast<ParameterArrayModel*>(arrayTable->model());

        QString arrayValue = arrayModel->getArrayData();
        model->setData(index, arrayValue, Qt::EditRole);
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

    int editorMinimumSize = 24 * (portWidth + 1);

    const int PARENT_HEIGHT = editor->parentWidget()->height();
    const int AVAILABLE_HEIGHT_BELOW = PARENT_HEIGHT - option.rect.top();

    if (AVAILABLE_HEIGHT_BELOW > editorMinimumSize)
    {
        editor->move(option.rect.topLeft());
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