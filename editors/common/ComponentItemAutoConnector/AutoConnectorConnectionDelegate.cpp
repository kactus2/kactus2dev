//-----------------------------------------------------------------------------
// File: AutoConnectorConnectionDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 12.02.2019
//
// Description:
// Delegate that provides widgets for editing connected pairings.
//-----------------------------------------------------------------------------

#include "AutoConnectorConnectionDelegate.h"

//-----------------------------------------------------------------------------
// Function: AutoConnectorConnectionDelegate::AutoConnectorConnectionDelegate()
//-----------------------------------------------------------------------------
AutoConnectorConnectionDelegate::AutoConnectorConnectionDelegate(QListWidget* firstList, QListWidget* secondList,
    QObject* parent):
QStyledItemDelegate(parent),
firstList_(firstList),
secondList_(secondList)
{

}

//-----------------------------------------------------------------------------
// Function: AutoConnectorConnectionDelegate::~AutoConnectorConnectionDelegate()
//-----------------------------------------------------------------------------
AutoConnectorConnectionDelegate::~AutoConnectorConnectionDelegate()
{

}

//-----------------------------------------------------------------------------
// Function: AutoConnectorConnectionDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* AutoConnectorConnectionDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const& option,
    QModelIndex const& index) const
{
    if (index.column() == 0)
    {
        return createItemSelector(firstList_, parent);
    }
    else if (index.column() == 1)
    {
        return createItemSelector(secondList_, parent);
    }
    else
    {
        return QStyledItemDelegate::createEditor(parent, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: AutoConnectorConnectionDelegate::createItemSelector()
//-----------------------------------------------------------------------------
QComboBox* AutoConnectorConnectionDelegate::createItemSelector(QListWidget* itemList, QWidget* parent) const
{
    QComboBox* combo = new QComboBox(parent);
    combo->addItem(QString("<none>"));

    for (int i = 0; i < itemList->count(); ++i)
    {
        QListWidgetItem* itemOnRow = itemList->item(i);
        combo->addItem(itemOnRow->icon(), itemOnRow->text());
    }

    return combo;
}

//-----------------------------------------------------------------------------
// Function: AutoConnectorConnectionDelegate::setEditorData()
//-----------------------------------------------------------------------------
void AutoConnectorConnectionDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const 
{
    if (index.column() == 0 || index.column() == 1)
    {
        QString itemName = index.data(Qt::DisplayRole).toString();
        QComboBox* portCombo = qobject_cast<QComboBox*>(editor);

        int comboIndex = portCombo->findText(itemName);
        portCombo->setCurrentIndex(comboIndex);
    }
    else
    {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

//-----------------------------------------------------------------------------
// Function: AutoConnectorConnectionDelegate::setModelData()
//-----------------------------------------------------------------------------
void AutoConnectorConnectionDelegate::setModelData(QWidget* editor, QAbstractItemModel* model,
    QModelIndex const& index) const 
{
    if (index.column() == 0 || index.column() == 1)
    {
        QComboBox* itemCombo = qobject_cast<QComboBox*>(editor);
        QString itemName = itemCombo->currentText();
        QIcon itemIcon = itemCombo->itemIcon(itemCombo->currentIndex());

        if (itemName == "<none>")
        {
            itemName = "";
        }

        model->setData(index, itemIcon, Qt::DecorationRole);
        model->setData(index, itemName, Qt::DisplayRole);
    }
	else 
    {
        QStyledItemDelegate::setModelData(editor, model, index);
	}
}
