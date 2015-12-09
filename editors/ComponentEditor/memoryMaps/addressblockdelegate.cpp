//-----------------------------------------------------------------------------
// File: addressblockdelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 24.08.2012
//
// Description:
// The delegate that provides editors to add/remove/edit the details of address block.
//-----------------------------------------------------------------------------

#include "addressblockdelegate.h"

#include <common/widgets/booleanComboBox/booleancombobox.h>
#include <common/widgets/accessComboBox/accesscombobox.h>

//-----------------------------------------------------------------------------
// Function: AddressBlockDelegate::AddressBlockDelegate()
//-----------------------------------------------------------------------------
AddressBlockDelegate::AddressBlockDelegate(QCompleter* parameterNameCompleter,
    QSharedPointer<ParameterFinder> parameterFinder, QObject *parent):
ExpressionDelegate(parameterNameCompleter, parameterFinder, parent) 
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
QWidget* AddressBlockDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const& option, 
    QModelIndex const& index ) const
{
    if (index.column() == AddressBlockColumns::VOLATILE)
    {
        return new BooleanComboBox(parent);
    }
    else if (index.column() == AddressBlockColumns::REGISTER_ACCESS)
    {
        return new AccessComboBox(parent);
    }
    else
    {
        return ExpressionDelegate::createEditor(parent, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: AddressBlockDelegate::setEditorData()
//-----------------------------------------------------------------------------
void AddressBlockDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const
{
    if (index.column() == AddressBlockColumns::VOLATILE)
    {
        BooleanComboBox* boolBox = qobject_cast<BooleanComboBox*>(editor);
        Q_ASSERT(boolBox);

        QString value = index.model()->data(index, Qt::DisplayRole).toString();
        boolBox->setCurrentValue(value);
    }
    else if (index.column() == AddressBlockColumns::REGISTER_ACCESS)
    {
        AccessComboBox* accessBox = qobject_cast<AccessComboBox*>(editor);
        Q_ASSERT(accessBox);

        AccessTypes::Access access = AccessTypes::str2Access(
            index.model()->data(index, Qt::DisplayRole).toString(), AccessTypes::ACCESS_COUNT);
        accessBox->setCurrentValue(access);
    }
    else
    {
        ExpressionDelegate::setEditorData(editor, index);
    }
}

//-----------------------------------------------------------------------------
// Function: AddressBlockDelegate::setModelData()
//-----------------------------------------------------------------------------
void AddressBlockDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const
{
    if (index.column() == AddressBlockColumns::VOLATILE)
    {
        BooleanComboBox* boolBox = qobject_cast<BooleanComboBox*>(editor);
        Q_ASSERT(boolBox);

        QString value = boolBox->getCurrentValue();
        model->setData(index, value, Qt::EditRole);
    }
    else if (index.column() == AddressBlockColumns::REGISTER_ACCESS)
    {
        AccessComboBox* accessBox = qobject_cast<AccessComboBox*>(editor);
        Q_ASSERT(accessBox);

        AccessTypes::Access access = accessBox->getCurrentValue();
        model->setData(index, AccessTypes::access2Str(access), Qt::EditRole);
    }
    else 
    {
        ExpressionDelegate::setModelData(editor, model, index);
    }
}

//-----------------------------------------------------------------------------
// Function: AddressBlockDelegate::descriptionColumn()
//-----------------------------------------------------------------------------
int AddressBlockDelegate::descriptionColumn() const
{
    return AddressBlockColumns::DESCRIPTION;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockDelegate::columnAcceptsExpression()
//-----------------------------------------------------------------------------
bool AddressBlockDelegate::columnAcceptsExpression(int column) const
{
    return column == AddressBlockColumns::REGISTER_DIMENSION ||
        column == AddressBlockColumns::REGISTER_SIZE ||
        column == AddressBlockColumns::REGISTER_OFFSET ||
        column == AddressBlockColumns::IS_PRESENT;
}
