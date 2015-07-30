/* 
 *  	Created on: 24.8.2012
 *      Author: Antti Kamppi
 * 		filename: addressblockdelegate.cpp
 *		Project: Kactus 2
 */

#include "addressblockdelegate.h"

#include <common/widgets/booleanComboBox/booleancombobox.h>
#include <common/widgets/accessComboBox/accesscombobox.h>

#include <IPXACTmodels/validators/BinaryValidator.h>

#include <QIntValidator>
#include <QKeyEvent>
#include <QLineEdit>
#include <QTextEdit>

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

        bool value = index.model()->data(index, Qt::DisplayRole).toBool();
        boolBox->setCurrentValue(value);
    }
    else if (index.column() == AddressBlockColumns::REGISTER_ACCESS)
    {
        AccessComboBox* accessBox = qobject_cast<AccessComboBox*>(editor);
        Q_ASSERT(accessBox);

        General::Access access = General::str2Access(index.model()->data(
            index, Qt::DisplayRole).toString(), General::ACCESS_COUNT);
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

        bool value = boolBox->getCurrentValue();
        model->setData(index, value, Qt::EditRole);
    }
    else if (index.column() == AddressBlockColumns::REGISTER_ACCESS)
    {
        AccessComboBox* accessBox = qobject_cast<AccessComboBox*>(editor);
        Q_ASSERT(accessBox);

        General::Access access = accessBox->getCurrentValue();
        model->setData(index, General::access2Str(access), Qt::EditRole);
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
