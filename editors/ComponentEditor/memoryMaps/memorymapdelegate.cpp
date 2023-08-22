//-----------------------------------------------------------------------------
// File: memorymapdelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 22.08.2012
//
// Description:
// The delegate that provides editors to add/remove/edit details of a single memory map.
//-----------------------------------------------------------------------------

#include "memorymapdelegate.h"

#include "MemoryMapColumns.h"

#include <common/widgets/booleanComboBox/booleancombobox.h>
#include <common/widgets/usageComboBox/usagecombobox.h>
#include <common/widgets/accessComboBox/accesscombobox.h>

#include <IPXACTmodels/common/validators/namevalidator.h>

#include <QLineEdit>

//-----------------------------------------------------------------------------
// Function: MemoryMapDelegate::MemoryMapDelegate()
//-----------------------------------------------------------------------------
MemoryMapDelegate::MemoryMapDelegate(QAbstractItemModel* completionModel,
                                     QSharedPointer<ParameterFinder> parameterFinder, QObject *parent):
MemoryBlockDelegate(completionModel, parameterFinder, parent)
{

}

//-----------------------------------------------------------------------------
// Function: MemoryMapDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* MemoryMapDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const& option,
    QModelIndex const& index) const 
{
    if (index.column() == MemoryMapColumns::USAGE_COLUMN)
    {
        UsageComboBox* usageBox = new UsageComboBox(parent);
        return usageBox;
    }
    else if (index.column() == MemoryMapColumns::ACCESS_COLUMN)
    {
        AccessComboBox* accessBox = new AccessComboBox(parent);
        return accessBox;
    }
    else if (index.column() == MemoryMapColumns::VOLATILE_COLUMN)
    {
        BooleanComboBox* boolBox = new BooleanComboBox(parent);
        return boolBox;
    }
    else
    {
        return MemoryBlockDelegate::createEditor(parent, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryMapDelegate::setEditorData()
//-----------------------------------------------------------------------------
void MemoryMapDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const
{
    if (index.column() == MemoryMapColumns::USAGE_COLUMN)
    {
        UsageComboBox* usageBox = qobject_cast<UsageComboBox*>(editor);
        Q_ASSERT(usageBox);

        General::Usage usage = General::str2Usage(index.model()->data(index).toString(), General::USAGE_COUNT);
        usageBox->setCurrentValue(usage);
    }
    else if (index.column() == MemoryMapColumns::ACCESS_COLUMN)
    {
        AccessComboBox* accessBox = qobject_cast<AccessComboBox*>(editor);
        Q_ASSERT(accessBox);

        AccessTypes::Access access =
            AccessTypes::str2Access(index.model()->data(index).toString(), AccessTypes::ACCESS_COUNT);
        accessBox->setCurrentValue(access);
    }
    else if (index.column() == MemoryMapColumns::VOLATILE_COLUMN)
    {
        BooleanComboBox* boolBox = qobject_cast<BooleanComboBox*>(editor);
        Q_ASSERT(boolBox);

        QString value = index.model()->data(index).toString();
        boolBox->setCurrentValue(value);
    }
    else
    {
        MemoryBlockDelegate::setEditorData(editor, index);
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryMapDelegate::setModelData()
//-----------------------------------------------------------------------------
void MemoryMapDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const
{
    if (index.column() ==  MemoryMapColumns::USAGE_COLUMN)
    {
        UsageComboBox* usageBox = qobject_cast<UsageComboBox*>(editor);
        Q_ASSERT(usageBox);

        General::Usage usage = usageBox->getCurrentValue();
        model->setData(index, General::usage2Str(usage), Qt::EditRole);
    }
    else if (index.column() ==  MemoryMapColumns::ACCESS_COLUMN)
    {
        AccessComboBox* accessBox = qobject_cast<AccessComboBox*>(editor);
        Q_ASSERT(accessBox);

        AccessTypes::Access access = accessBox->getCurrentValue();
        model->setData(index, AccessTypes::access2Str(access), Qt::EditRole);
    }
    else if (index.column() ==  MemoryMapColumns::VOLATILE_COLUMN)
    {
        BooleanComboBox* boolBox = qobject_cast<BooleanComboBox*>(editor);
        Q_ASSERT(boolBox);

        QString value = boolBox->getCurrentValue();
        model->setData(index, value, Qt::EditRole);
    }
    else 
    {
        MemoryBlockDelegate::setModelData(editor, model, index);
    }
}

//-----------------------------------------------------------------------------
// Function: memorymapdelegate::columnAcceptsExpression()
//-----------------------------------------------------------------------------
bool MemoryMapDelegate::columnAcceptsExpression(int column) const
{
    return MemoryBlockDelegate::columnAcceptsExpression(column) ||
        column == MemoryMapColumns::RANGE_COLUMN || column == MemoryMapColumns::WIDTH_COLUMN;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapDelegate::descriptionColumn()
//-----------------------------------------------------------------------------
int MemoryMapDelegate::descriptionColumn() const
{
    return MemoryMapColumns::DESCRIPTION_COLUMN;
}

//-----------------------------------------------------------------------------
// Function: memorymapdelegate::nameColumn()
//-----------------------------------------------------------------------------
int MemoryMapDelegate::nameColumn() const
{
    return MemoryMapColumns::NAME_COLUMN;
}

//-----------------------------------------------------------------------------
// Function: memorymapdelegate::baseAddressColumn()
//-----------------------------------------------------------------------------
int MemoryMapDelegate::baseAddressColumn() const
{
    return MemoryMapColumns::BASE_COLUMN;
}

//-----------------------------------------------------------------------------
// Function: memorymapdelegate::isPresentColumn()
//-----------------------------------------------------------------------------
int MemoryMapDelegate::isPresentColumn() const
{
    return MemoryMapColumns::IS_PRESENT;
}
