//-----------------------------------------------------------------------------
// File: BridgesDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 21.6.2012
//
// Description:
// 
//-----------------------------------------------------------------------------

#include "bridgesdelegate.h"

#include "BridgeColumns.h"

#include <common/widgets/interfaceSelector/interfaceselector.h>

#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/Component/Component.h>

#include <QApplication>
#include <QMouseEvent>
#include <QPainter>

//-----------------------------------------------------------------------------
// Function: BridgesDelegate::BridgesDelegate()
//-----------------------------------------------------------------------------
BridgesDelegate::BridgesDelegate(BusInterfaceInterface* busInterface, Document::Revision revision, QObject* parent):
QStyledItemDelegate(parent),
busInterface_(busInterface),
revision_(revision)
{
}

//-----------------------------------------------------------------------------
// Function: BridgesDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* BridgesDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option,
    QModelIndex const& index) const
{
    if (index.column() == BridgeColumns::INITIATOR_COLUMN)
    {
        auto busSelector(new InterfaceSelector(busInterface_, parent, General::INITIATOR));
        if (revision_ == Document::Revision::Std14)
        {
            busSelector->setInterfaceMode(General::MASTER);
        }

        return busSelector;
    }
    else
    {
        return QStyledItemDelegate::createEditor(parent, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: BridgesDelegate::setEditorData()
//-----------------------------------------------------------------------------
void BridgesDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const
{
    if (index.column() == BridgeColumns::INITIATOR_COLUMN)
    {
        InterfaceSelector* selector = qobject_cast<InterfaceSelector*>(editor);
        Q_ASSERT(selector);

        selector->refresh();
        selector->selectInterface(index.model()->data(index, Qt::DisplayRole).toString());
    }
    else 
    {
        QStyledItemDelegate::setEditorData(editor, index);
    }

}

//-----------------------------------------------------------------------------
// Function: BridgesDelegate::setModelData()
//-----------------------------------------------------------------------------
void BridgesDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const
{
    if (index.column() == BridgeColumns::INITIATOR_COLUMN)
    {
        InterfaceSelector* selector = qobject_cast<InterfaceSelector*>(editor);
        Q_ASSERT(selector);

        QString selected = selector->currentText();
        model->setData(index, selected, Qt::EditRole);
    }
    else
    {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}
