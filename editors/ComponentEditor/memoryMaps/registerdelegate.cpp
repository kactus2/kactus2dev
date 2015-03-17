/* 
 *  	Created on: 25.8.2012
 *      Author: Antti Kamppi
 * 		filename: registerdelegate.cpp
 *		Project: Kactus 2
 */

#include "registerdelegate.h"
#include "RegisterColumns.h"

#include <common/widgets/booleanComboBox/booleancombobox.h>
#include <common/widgets/accessComboBox/accesscombobox.h>
#include <common/widgets/modWriteComboBox/modwritecombobox.h>
#include <common/widgets/readActionComboBox/readactioncombobox.h>
#include <common/widgets/testConstraintComboBox/testconstraintcombobox.h>

#include <QLineEdit>
#include <QSpinBox>

//-----------------------------------------------------------------------------
// Function: registerdelegate::RegisterDelegate()
//-----------------------------------------------------------------------------
RegisterDelegate::RegisterDelegate(QCompleter* parameterNameCompleter,
    QSharedPointer<ParameterFinder> parameterFinder, QObject *parent):
ExpressionDelegate(parameterNameCompleter, parameterFinder, parent)
{

}

//-----------------------------------------------------------------------------
// Function: registerdelegate::~RegisterDelegate()
//-----------------------------------------------------------------------------
RegisterDelegate::~RegisterDelegate() 
{

}

//-----------------------------------------------------------------------------
// Function: registerdelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* RegisterDelegate::createEditor( QWidget* parent, const QStyleOptionViewItem& option, 
    const QModelIndex& index ) const 
{
    if (index.column() == RegisterColumns::VOLATILE_COLUMN || index.column() == RegisterColumns::TESTABLE_COLUMN)
    {
        BooleanComboBox* boolBox = new BooleanComboBox(parent);
        return boolBox;
    }
    else if (index.column() == RegisterColumns::ACCESS_COLUMN)
    {
        AccessComboBox* accessBox = new AccessComboBox(parent);
        return accessBox;
    }
    else if (index.column() == RegisterColumns::MOD_WRITE_COLUMN)
    {
        ModWriteComboBox* modBox = new ModWriteComboBox(parent);
        return modBox;
    }
    else if (index.column() == RegisterColumns::READ_ACTION_COLUMN)
    {
        ReadActionComboBox* readBox = new ReadActionComboBox(parent);
        return readBox;
    }
    else if (index.column() == RegisterColumns::TEST_CONSTR_COLUMN)
    {
        TestConstraintComboBox* testBox = new TestConstraintComboBox(parent);
        return testBox;
    }
    else
    {
        return ExpressionDelegate::createEditor(parent, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: registerdelegate::setEditorData()
//-----------------------------------------------------------------------------
void RegisterDelegate::setEditorData( QWidget* editor, const QModelIndex& index ) const
{
    if (index.column() == RegisterColumns::VOLATILE_COLUMN || index.column() == RegisterColumns::TESTABLE_COLUMN)
    {
        BooleanComboBox* boolBox = qobject_cast<BooleanComboBox*>(editor);
        Q_ASSERT(boolBox);

        bool value = index.model()->data(index, Qt::DisplayRole).toBool();
        boolBox->setCurrentValue(value);
    }
    else if (index.column() == RegisterColumns::ACCESS_COLUMN)
    {
        AccessComboBox* accessBox = qobject_cast<AccessComboBox*>(editor);
        Q_ASSERT(accessBox);

        General::Access access = General::str2Access(index.model()->data(
            index, Qt::DisplayRole).toString(), General::ACCESS_COUNT);
        accessBox->setCurrentValue(access);
    }
    else if (index.column() == RegisterColumns::MOD_WRITE_COLUMN)
    {
        ModWriteComboBox* modBox = qobject_cast<ModWriteComboBox*>(editor);
        Q_ASSERT(modBox);

        General::ModifiedWrite modWrite = General::str2ModifiedWrite(
            index.model()->data(index, Qt::DisplayRole).toString());
        modBox->setCurrentValue(modWrite);
    }
    else if (index.column() == RegisterColumns::READ_ACTION_COLUMN)
    {
        ReadActionComboBox* readBox = qobject_cast<ReadActionComboBox*>(editor);
        Q_ASSERT(readBox);

        General::ReadAction readAction = General::str2ReadAction(
            index.model()->data(index, Qt::DisplayRole).toString());
        readBox->setCurrentValue(readAction);
    }
    else if (index.column() == RegisterColumns::TEST_CONSTR_COLUMN)
    {
        TestConstraintComboBox* testBox = qobject_cast<TestConstraintComboBox*>(editor);
        Q_ASSERT(testBox);

        General::TestConstraint testConstr = General::str2TestConstraint(
            index.model()->data(index, Qt::DisplayRole).toString());
        testBox->setCurrentValue(testConstr);
    }
    else
    {
        ExpressionDelegate::setEditorData(editor, index);
    }
}

//-----------------------------------------------------------------------------
// Function: registerdelegate::setModelData()
//-----------------------------------------------------------------------------
void RegisterDelegate::setModelData( QWidget* editor, QAbstractItemModel* model, const QModelIndex& index ) const 
{
    if (index.column() == RegisterColumns::VOLATILE_COLUMN || index.column() == RegisterColumns::TESTABLE_COLUMN)
    {
        BooleanComboBox* boolBox = qobject_cast<BooleanComboBox*>(editor);
        Q_ASSERT(boolBox);

        bool value = boolBox->getCurrentValue();
        model->setData(index, value, Qt::EditRole);
    }
    else if (index.column() == RegisterColumns::ACCESS_COLUMN)
    {
        AccessComboBox* accessBox = qobject_cast<AccessComboBox*>(editor);
        Q_ASSERT(accessBox);

        General::Access access = accessBox->getCurrentValue();
        model->setData(index, General::access2Str(access), Qt::EditRole);
    }
    else if (index.column() == RegisterColumns::MOD_WRITE_COLUMN)
    {
        ModWriteComboBox* modBox = qobject_cast<ModWriteComboBox*>(editor);
        Q_ASSERT(modBox);

        General::ModifiedWrite modWrite = modBox->getCurrentValue();
        model->setData(index, General::modifiedWrite2Str(modWrite), Qt::EditRole);
    }
    else if (index.column() == RegisterColumns::READ_ACTION_COLUMN)
    {
        ReadActionComboBox* readBox = qobject_cast<ReadActionComboBox*>(editor);
        Q_ASSERT(readBox);

        General::ReadAction readAction = readBox->getCurrentValue();
        model->setData(index, General::readAction2Str(readAction), Qt::EditRole);
    }
    else if (index.column() == RegisterColumns::TEST_CONSTR_COLUMN)
    {
        TestConstraintComboBox* testBox = qobject_cast<TestConstraintComboBox*>(editor);
        Q_ASSERT(testBox);

        General::TestConstraint testConstr = testBox->getCurrentValue();
        model->setData(index, General::testConstraint2Str(testConstr), Qt::EditRole);
    }
    else
    {
        ExpressionDelegate::setModelData(editor, model, index);
    }
}

//-----------------------------------------------------------------------------
// Function: registerdelegate::commitAndCloseEditor()
//-----------------------------------------------------------------------------
void RegisterDelegate::commitAndCloseEditor() {
	QWidget* edit = qobject_cast<QWidget*>(sender());
	Q_ASSERT(edit);

	emit commitData(edit);
	emit closeEditor(edit);
}

//-----------------------------------------------------------------------------
// Function: registerdelegate::columnAcceptsExpression()
//-----------------------------------------------------------------------------
bool RegisterDelegate::columnAcceptsExpression(int column) const
{
    return column == RegisterColumns::WIDTH_COLUMN || column == RegisterColumns::OFFSET_COLUMN ||
        column == RegisterColumns::IS_PRESENT_COLUMN;
}