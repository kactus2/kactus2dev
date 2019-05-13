//-----------------------------------------------------------------------------
// File: WirePortsDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 09.05.2019
//
// Description:
// The delegate that provides editors to edit wire ports of a component.
//-----------------------------------------------------------------------------

#include "WirePortsDelegate.h"

#include <editors/ComponentEditor/ports/WirePortColumns.h>
#include <editors/ComponentEditor/ports/PortWireTypeEditor.h>

#include <QComboBox>

//-----------------------------------------------------------------------------
// Function: WirePortsDelegate::WirePortsDelegate()
//-----------------------------------------------------------------------------
WirePortsDelegate::WirePortsDelegate(QSharedPointer<Component> component, QCompleter* parameterCompleter,
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<PortTypeValidator> typeValidator,
    QObject* parent):
PortsDelegate(component, parameterCompleter, parameterFinder, typeValidator, parent)
{

}

//-----------------------------------------------------------------------------
// Function: WirePortsDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* WirePortsDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const& option, 
    QModelIndex const& index) const
{
    if (index.column() == WirePortColumns::DIRECTION)
    {
        return createSelectorForDirection(parent);
    }
    else
    {
        return PortsDelegate::createEditor(parent, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: WirePortsDelegate::setEditorData()
//-----------------------------------------------------------------------------
void WirePortsDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const
{
    if (index.column() == WirePortColumns::DIRECTION)
    {
        QString text = index.data(Qt::DisplayRole).toString();
        QComboBox* combo = qobject_cast<QComboBox*>(editor);

        int comboIndex = combo->findText(text);
        combo->setCurrentIndex(comboIndex);
    }
    else
    {
        PortsDelegate::setEditorData(editor, index);
    }
}

//-----------------------------------------------------------------------------
// Function: WirePortsDelegate::setModelData()
//-----------------------------------------------------------------------------
void WirePortsDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index ) const
{
    if (index.column() == WirePortColumns::DIRECTION)
    {
        QComboBox* combo = qobject_cast<QComboBox*>(editor);
        QString text = combo->currentText();
        model->setData(index, text, Qt::EditRole);
    }
    else
    {
        PortsDelegate::setModelData(editor, model, index);
    }
}

//-----------------------------------------------------------------------------
// Function: WirePortsDelegate::columnAcceptsExpression()
//-----------------------------------------------------------------------------
bool WirePortsDelegate::columnAcceptsExpression(int column) const
{
    return PortsDelegate::columnAcceptsExpression(column) || column == WirePortColumns::DEFAULT_VALUE ||
        column == WirePortColumns::LEFT_BOUND || column == WirePortColumns::RIGHT_BOUND;
}

//-----------------------------------------------------------------------------
// Function: WirePortsDelegate::descriptionColumn()
//-----------------------------------------------------------------------------
int WirePortsDelegate::descriptionColumn() const
{
    return WirePortColumns::DESCRIPTION;
}

//-----------------------------------------------------------------------------
// Function: WirePortsDelegate::nameColumn()
//-----------------------------------------------------------------------------
int WirePortsDelegate::nameColumn() const
{
    return WirePortColumns::NAME;
}

//-----------------------------------------------------------------------------
// Function: WirePortsDelegate::typeColumn()
//-----------------------------------------------------------------------------
int WirePortsDelegate::typeColumn() const
{
    return WirePortColumns::TYPE_NAME;
}

//-----------------------------------------------------------------------------
// Function: WirePortsDelegate::tagColumn()
//-----------------------------------------------------------------------------
int WirePortsDelegate::tagColumn() const
{
    return WirePortColumns::TAG_GROUP;
}

//-----------------------------------------------------------------------------
// Function: WirePortsDelegate::adHocColumn()
//-----------------------------------------------------------------------------
int WirePortsDelegate::adHocColumn() const
{
    return WirePortColumns::ADHOC_VISIBILITY;
}

//-----------------------------------------------------------------------------
// Function: WirePortsDelegate::arrayLeftColumn()
//-----------------------------------------------------------------------------
int WirePortsDelegate::arrayLeftColumn() const
{
    return WirePortColumns::ARRAY_LEFT;
}

//-----------------------------------------------------------------------------
// Function: WirePortsDelegate::arrayRightColumn()
//-----------------------------------------------------------------------------
int WirePortsDelegate::arrayRightColumn() const
{
    return WirePortColumns::ARRAY_RIGHT;
}

//-----------------------------------------------------------------------------
// Function: WirePortsDelegate::createSelectorForDirection()
//-----------------------------------------------------------------------------
QWidget* WirePortsDelegate::createSelectorForDirection(QWidget* parent) const
{
    QComboBox* directionSelector = new QComboBox(parent);
    directionSelector->addItem("in");
    directionSelector->addItem("out");
    directionSelector->addItem("inout");
    directionSelector->addItem("phantom");

    return directionSelector;
}
