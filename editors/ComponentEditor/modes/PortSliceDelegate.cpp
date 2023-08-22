//-----------------------------------------------------------------------------
// File: PortSliceDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 14.08.2023
//
// Description:
// Delegate that provides widgets for editing port slices in mode.
//-----------------------------------------------------------------------------

#include "PortSliceDelegate.h"

#include "PortSliceColumns.h"

#include <KactusAPI/include/ParameterFinder.h>
#include <editors/ComponentEditor/common/ParameterCompleter.h>
#include <KactusAPI/include/IPXactSystemVerilogParser.h>

#include <editors/ComponentEditor/common/ReferenceSelector/ReferenceSelector.h>

#include <editors/ComponentEditor/remapStates/ValueOrIndexedValueEditor.h>

#include <IPXACTmodels/Component/Choice.h>

#include <QScrollArea>


//-----------------------------------------------------------------------------
// Function: PortSliceDelegate::PortSliceDelegate()
//-----------------------------------------------------------------------------
PortSliceDelegate::PortSliceDelegate(QStringList const& portNameList, QAbstractItemModel* completionModel,
                        ExpressionSet expressions, QObject* parent):
ExpressionDelegate(completionModel, expressions.finder, parent),
expressionFormatter_(expressions.formatter),
expressionParser_(expressions.parser),
availablePortNames_(portNameList)
{

}

//-----------------------------------------------------------------------------
// Function: PortSliceDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* PortSliceDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const& option,
    QModelIndex const& index) const
{
    if (index.column() == PortSliceColumns::PORT_REF)
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
    else
    {
        return ExpressionDelegate::createEditor(parent, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: PortSliceDelegate::setEditorData()
//-----------------------------------------------------------------------------
void PortSliceDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const
{
    if (index.column() == PortSliceColumns::PORT_REF)
    {
        QString text = index.model()->data(index, Qt::DisplayRole).toString();
        ReferenceSelector* referenceSelector = qobject_cast<ReferenceSelector*>(editor);

        referenceSelector->selectItem(text);
    }
    else
    {
        ExpressionDelegate::setEditorData(editor, index);
    }
}

//-----------------------------------------------------------------------------
// Function: PortSliceDelegate::setModelData()
//-----------------------------------------------------------------------------
void PortSliceDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index)
    const
{
    if (index.column() == PortSliceColumns::PORT_REF)
    {
        auto referenceSelector = qobject_cast<ReferenceSelector*>(editor);
        QString text = referenceSelector->currentText();
        model->setData(index, text, Qt::EditRole);
    }
    else
    {
        ExpressionDelegate::setModelData(editor, model, index);
    }
}

//-----------------------------------------------------------------------------
// Function: PortSliceDelegate::columnAcceptsExpression()
//-----------------------------------------------------------------------------
bool PortSliceDelegate::columnAcceptsExpression(int column) const
{
    return column == PortSliceColumns::LEFT_BOUND || column == PortSliceColumns::RIGHT_BOUND;
}

//-----------------------------------------------------------------------------
// Function: PortSliceDelegate::descriptionColumn()
//-----------------------------------------------------------------------------
int PortSliceDelegate::descriptionColumn() const
{
    return PortSliceColumns::DESCRIPTION;
}

//-----------------------------------------------------------------------------
// Function: PortSliceDelegate::setNewPortNames()
//-----------------------------------------------------------------------------
void PortSliceDelegate::setNewPortNames(QStringList const& newPorts)
{
    availablePortNames_ = newPorts;
}

//-----------------------------------------------------------------------------
// Function: PortSliceDelegate::setComponentPorts()
//-----------------------------------------------------------------------------
void PortSliceDelegate::setComponentPorts(QStringList const& componentPorts)
{
    componentPortNames_ = componentPorts;
}
