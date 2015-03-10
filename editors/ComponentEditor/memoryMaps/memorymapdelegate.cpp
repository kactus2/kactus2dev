/* 
 *  	Created on: 22.8.2012
 *      Author: Antti Kamppi
 * 		filename: memorymapdelegate.cpp
 *		Project: Kactus 2
 */

#include "memorymapdelegate.h"

#include "MemoryMapColumns.h"

#include <common/widgets/booleanComboBox/booleancombobox.h>
#include <common/widgets/usageComboBox/usagecombobox.h>
#include <common/widgets/accessComboBox/accesscombobox.h>

#include <IPXACTmodels/validators/namevalidator.h>

#include <QLineEdit>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QSpinBox>

//-----------------------------------------------------------------------------
// Function: MemoryMapDelegate::MemoryMapDelegate()
//-----------------------------------------------------------------------------
MemoryMapDelegate::MemoryMapDelegate(QCompleter* parameterNameCompleter,
                                     QSharedPointer<ParameterFinder> parameterFinder, QObject *parent):
ExpressionDelegate(parameterNameCompleter, parameterFinder, parent)
{

}

//-----------------------------------------------------------------------------
// Function: MemoryMapDelegate::~MemoryMapDelegate()
//-----------------------------------------------------------------------------
MemoryMapDelegate::~MemoryMapDelegate()
{

}

//-----------------------------------------------------------------------------
// Function: MemoryMapDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* MemoryMapDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const& option,
    QModelIndex const& index) const 
{
    if (index.column() == MemoryMapColumns::NAME_COLUMN)
    {
        return createNameEditor(parent, option, index);
    }
    else if (index.column() == MemoryMapColumns::USAGE_COLUMN)
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
        return ExpressionDelegate::createEditor(parent, option, index);
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

        General::Access access = General::str2Access(index.model()->data(index).toString(), General::ACCESS_COUNT);
        accessBox->setCurrentValue(access);
    }
    else if (index.column() == MemoryMapColumns::VOLATILE_COLUMN)
    {
        BooleanComboBox* boolBox = qobject_cast<BooleanComboBox*>(editor);
        Q_ASSERT(boolBox);

        bool value = index.model()->data(index).toBool();
        boolBox->setCurrentValue(value);
    }
    else
    {
        ExpressionDelegate::setEditorData(editor, index);
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

        General::Access access = accessBox->getCurrentValue();
        model->setData(index, General::access2Str(access), Qt::EditRole);
    }
    else if (index.column() ==  MemoryMapColumns::VOLATILE_COLUMN)
    {
        BooleanComboBox* boolBox = qobject_cast<BooleanComboBox*>(editor);
        Q_ASSERT(boolBox);

        bool value = boolBox->getCurrentValue();
        model->setData(index, value, Qt::EditRole);
    }
    else 
    {
        ExpressionDelegate::setModelData(editor, model, index);
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryMapDelegate::commitAndCloseEditor()
//-----------------------------------------------------------------------------
void MemoryMapDelegate::commitAndCloseEditor()
{
	QWidget* editor = qobject_cast<QWidget*>(sender());
	Q_ASSERT(editor);

	emit commitData(editor);
	emit closeEditor(editor);
}

//-----------------------------------------------------------------------------
// Function: MemoryMapDelegate::createNameEditor()
//-----------------------------------------------------------------------------
QWidget* MemoryMapDelegate::createNameEditor(QWidget* parent, QStyleOptionViewItem const& option, 
    QModelIndex const& index) const
{
    QWidget* nameEditor = QStyledItemDelegate::createEditor(parent, option, index);

    QLineEdit* lineEditor = qobject_cast<QLineEdit*>(nameEditor);
    if (lineEditor)
    {
        lineEditor->setValidator(new NameValidator(lineEditor));
    }

    connect(nameEditor, SIGNAL(editingFinished()), this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
    return nameEditor;
}

//-----------------------------------------------------------------------------
// Function: memorymapdelegate::columnAcceptsExpression()
//-----------------------------------------------------------------------------
bool MemoryMapDelegate::columnAcceptsExpression(int column) const
{
    return column == MemoryMapColumns::BASE_COLUMN || column == MemoryMapColumns::RANGE_COLUMN ||
        column == MemoryMapColumns::WIDTH_COLUMN;
}
