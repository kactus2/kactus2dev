//-----------------------------------------------------------------------------
// File: registerfiledelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 24.08.2012
//
// Description:
// The delegate that provides editors to add/remove/edit the details of address block.
//-----------------------------------------------------------------------------

#include "registerfiledelegate.h"

#include <common/widgets/booleanComboBox/booleancombobox.h>
#include <common/widgets/accessComboBox/accesscombobox.h>

//-----------------------------------------------------------------------------
// Function: RegisterFileDelegate::RegisterFileDelegate()
//-----------------------------------------------------------------------------
RegisterFileDelegate::RegisterFileDelegate(QCompleter* parameterNameCompleter,
                                           QSharedPointer<ParameterFinder> parameterFinder,
                                           QObject *parent):
  ExpressionDelegate(parameterNameCompleter, parameterFinder, parent)
{

}

//-----------------------------------------------------------------------------
// Function: RegisterFileDelegate::~RegisterFileDelegate()
//-----------------------------------------------------------------------------
RegisterFileDelegate::~RegisterFileDelegate()
{

}

//-----------------------------------------------------------------------------
// Function: RegisterFileDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* RegisterFileDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const& option,
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
// Function: RegisterFileDelegate::setEditorData()
//-----------------------------------------------------------------------------
void RegisterFileDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const
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
// Function: RegisterFileDelegate::setModelData()
//-----------------------------------------------------------------------------
void RegisterFileDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const
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
// Function: RegisterFileDelegate::descriptionColumn()
//-----------------------------------------------------------------------------
int RegisterFileDelegate::descriptionColumn() const
{
    return AddressBlockColumns::DESCRIPTION;
}

//-----------------------------------------------------------------------------
// Function: RegisterFileDelegate::columnAcceptsExpression()
//-----------------------------------------------------------------------------
bool RegisterFileDelegate::columnAcceptsExpression(int column) const
{
    return column == AddressBlockColumns::REGISTER_DIMENSION ||
        column == AddressBlockColumns::REGISTER_SIZE ||
        column == AddressBlockColumns::REGISTER_OFFSET ||
        column == AddressBlockColumns::IS_PRESENT;
}
