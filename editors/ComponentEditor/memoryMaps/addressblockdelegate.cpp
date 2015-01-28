/* 
 *  	Created on: 24.8.2012
 *      Author: Antti Kamppi
 * 		filename: addressblockdelegate.cpp
 *		Project: Kactus 2
 */

#include "addressblockdelegate.h"

#include <common/widgets/booleanComboBox/booleancombobox.h>
#include <common/widgets/accessComboBox/accesscombobox.h>

#include <QLineEdit>
#include <QIntValidator>

//-----------------------------------------------------------------------------
// Function: AddressBlockDelegate::AddressBlockDelegate()
//-----------------------------------------------------------------------------
AddressBlockDelegate::AddressBlockDelegate(QObject *parent):
QStyledItemDelegate(parent) 
{

}

//-----------------------------------------------------------------------------
// Function: AddressBlockDelegate::~AddressBlockDelegate()
//-----------------------------------------------------------------------------
AddressBlockDelegate::~AddressBlockDelegate()
{

}

//-----------------------------------------------------------------------------
// Function: AddressBlockDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* AddressBlockDelegate::createEditor(QWidget* parent,  
    QStyleOptionViewItem const& option, 
    QModelIndex const& index ) const
{
    if (index.column() == AddressBlockColumns::SIZE_COLUMN)
    {
        QLineEdit* sizeEditor = new QLineEdit(parent);
        sizeEditor->setValidator(new QIntValidator(0, 16383, sizeEditor));

        return sizeEditor;
    }
    else if (index.column() == AddressBlockColumns::VOLATILE_COLUMN)
    {
        return new BooleanComboBox(parent);
    }
    else if (index.column() == AddressBlockColumns::ACCESS_COLUMN)
    {
        return new AccessComboBox(parent);
    }
    else
    {
        return QStyledItemDelegate::createEditor(parent, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: AddressBlockDelegate::setEditorData()
//-----------------------------------------------------------------------------
void AddressBlockDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const
{
    if (index.column() == AddressBlockColumns::VOLATILE_COLUMN)
    {
        BooleanComboBox* boolBox = qobject_cast<BooleanComboBox*>(editor);
        Q_ASSERT(boolBox);

        bool value = index.model()->data(index, Qt::DisplayRole).toBool();
        boolBox->setCurrentValue(value);
    }
    else if (index.column() == AddressBlockColumns::ACCESS_COLUMN)
    {
        AccessComboBox* accessBox = qobject_cast<AccessComboBox*>(editor);
        Q_ASSERT(accessBox);

        General::Access access = General::str2Access(index.model()->data(
            index, Qt::DisplayRole).toString(), General::ACCESS_COUNT);
        accessBox->setCurrentValue(access);
    }
    else
    {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

//-----------------------------------------------------------------------------
// Function: AddressBlockDelegate::setModelData()
//-----------------------------------------------------------------------------
void AddressBlockDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const
{
    if (index.column() == AddressBlockColumns::VOLATILE_COLUMN)
    {
        BooleanComboBox* boolBox = qobject_cast<BooleanComboBox*>(editor);
        Q_ASSERT(boolBox);

        bool value = boolBox->getCurrentValue();
        model->setData(index, value, Qt::EditRole);
    }
    else if (index.column() == AddressBlockColumns::ACCESS_COLUMN)
    {
        AccessComboBox* accessBox = qobject_cast<AccessComboBox*>(editor);
        Q_ASSERT(accessBox);

        General::Access access = accessBox->getCurrentValue();
        model->setData(index, General::access2Str(access), Qt::EditRole);
    }
    else 
    {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}
