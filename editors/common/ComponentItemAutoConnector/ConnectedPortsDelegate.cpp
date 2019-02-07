//-----------------------------------------------------------------------------
// File: ConnectedPortsDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 07.02.2019
//
// Description:
// Delegate that provides widgets for editing connected port pairings.
//-----------------------------------------------------------------------------

#include "ConnectedPortsDelegate.h"

#include <IPXACTmodels/Component/Component.h>

//-----------------------------------------------------------------------------
// Function: ConnectedPortsDelegate::ConnectedPortsDelegate()
//-----------------------------------------------------------------------------
ConnectedPortsDelegate::ConnectedPortsDelegate(QSharedPointer<Component> firstComponent,
    QSharedPointer<Component> secondComponent, QObject* parent) :
QStyledItemDelegate(parent),
firstComponent_(firstComponent),
secondComponent_(secondComponent)
{

}

//-----------------------------------------------------------------------------
// Function: ConnectedPortsDelegate::~ConnectedPortsDelegate()
//-----------------------------------------------------------------------------
ConnectedPortsDelegate::~ConnectedPortsDelegate()
{

}

//-----------------------------------------------------------------------------
// Function: ConnectedPortsDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* ConnectedPortsDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const& option,
    QModelIndex const& index) const
{
    if (index.column() == 0)
    {
        return createPortSelector(firstComponent_, parent);
    }
    else if (index.column() == 1)
    {
        return createPortSelector(secondComponent_, parent);
    }
    else
    {
        return QStyledItemDelegate::createEditor(parent, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: ConnectedPortsDelegate::createPortSelector()
//-----------------------------------------------------------------------------
QComboBox* ConnectedPortsDelegate::createPortSelector(QSharedPointer<Component> component, QWidget* parent) const
{
    QComboBox* combo = new QComboBox(parent);
    combo->addItem(QString("<none>"));

    for (auto componentPort : *component->getPorts())
    {
        combo->addItem(QIcon(getIconPath(componentPort->getDirection())), componentPort->name());
    }

    return combo;
}

//-----------------------------------------------------------------------------
// Function: ConnectedPortsDelegate::getIconPath()
//-----------------------------------------------------------------------------
QString ConnectedPortsDelegate::getIconPath(DirectionTypes::Direction portDirection) const
{
    QString iconPath = ":icons/common/graphics/cross.png";
    if (portDirection == DirectionTypes::IN)
    {
        iconPath = ":icons/common/graphics/input.png";
    }
    else if (portDirection == DirectionTypes::OUT)
    {
        iconPath = ":icons/common/graphics/output.png";
    }
    else if (portDirection == DirectionTypes::INOUT)
    {
        iconPath = ":icons/common/graphics/inout.png";
    }
    else if (portDirection == DirectionTypes::DIRECTION_PHANTOM)
    {
        iconPath = ":icons/common/graphics/phantom.png";
    }

    return iconPath;
}

//-----------------------------------------------------------------------------
// Function: ConnectedPortsDelegate::setEditorData()
//-----------------------------------------------------------------------------
void ConnectedPortsDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const 
{
    if (index.column() == 0 || index.column() == 1)
    {
        QString portName = index.data(Qt::DisplayRole).toString();
        QComboBox* portCombo = qobject_cast<QComboBox*>(editor);

        int comboIndex = portCombo->findText(portName);
        portCombo->setCurrentIndex(comboIndex);
    }
    else
    {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

//-----------------------------------------------------------------------------
// Function: ConnectedPortsDelegate::setModelData()
//-----------------------------------------------------------------------------
void ConnectedPortsDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index)
    const 
{
    if (index.column() == 0 || index.column() == 1)
    {
        QComboBox* portCombo = qobject_cast<QComboBox*>(editor);
        QString portName = portCombo->currentText();

        if (portName == "<none>")
        {
            portName = "";
        }

        bool testi = model->setData(index, portName, Qt::DisplayRole);
    }
	else 
    {
        QStyledItemDelegate::setModelData(editor, model, index);
	}
}
