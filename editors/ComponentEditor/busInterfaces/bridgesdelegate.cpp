/* 
 *  	Created on: 21.6.2012
 *      Author: Antti Kamppi
 * 		filename: bridgesdelegate.cpp
 *		Project: Kactus 2
 */

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
BridgesDelegate::BridgesDelegate(QSharedPointer<Component> component, QObject *parent):
QStyledItemDelegate(parent),
    component_(component),
    opaqueGroupModify_(false)
{
}

//-----------------------------------------------------------------------------
// Function: BridgesDelegate::~BridgesDelegate()
//-----------------------------------------------------------------------------
BridgesDelegate::~BridgesDelegate()
{
}

//-----------------------------------------------------------------------------
// Function: BridgesDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* BridgesDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option,
    QModelIndex const& index) const
{
    if (index.column() == BridgeColumns::MASTER_COLUMN)
    {
        return new InterfaceSelector(component_, parent, General::MASTER);			
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
    if (index.column() == BridgeColumns::MASTER_COLUMN)
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
    if (index.column() == BridgeColumns::MASTER_COLUMN)
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
