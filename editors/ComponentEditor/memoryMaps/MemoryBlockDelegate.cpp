//-----------------------------------------------------------------------------
// File: MemoryBlockDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 14.02.2022
//
// Description:
// The delegate that provides editors to add/remove/edit details of memory blocks.
//-----------------------------------------------------------------------------

#include "MemoryBlockDelegate.h"

#include <common/widgets/booleanComboBox/booleancombobox.h>

#include <IPXACTmodels/common/validators/namevalidator.h>

#include <QLineEdit>

//-----------------------------------------------------------------------------
// Function: MemoryBlockDelegate::MemoryBlockDelegate()
//-----------------------------------------------------------------------------
MemoryBlockDelegate::MemoryBlockDelegate(QAbstractItemModel* completionModel,
                                     QSharedPointer<ParameterFinder> parameterFinder, QObject *parent):
ExpressionDelegate(completionModel, parameterFinder, parent)
{

}

//-----------------------------------------------------------------------------
// Function: MemoryBlockDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* MemoryBlockDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const& option,
    QModelIndex const& index) const 
{
    if (index.column() == nameColumn())
    {
        return createNameEditor(parent, option, index);
    }
    else
    {
        return ExpressionDelegate::createEditor(parent, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockDelegate::columnAcceptsExpression()
//-----------------------------------------------------------------------------
bool MemoryBlockDelegate::columnAcceptsExpression(int column) const
{
    return column == baseAddressColumn() || column == isPresentColumn();
}


//-----------------------------------------------------------------------------
// Function: MemoryBlockDelegate::commitAndCloseEditor()
//-----------------------------------------------------------------------------
void MemoryBlockDelegate::commitAndCloseEditor()
{
	QWidget* editor = qobject_cast<QWidget*>(sender());
	Q_ASSERT(editor);

	emit commitData(editor);
	emit closeEditor(editor);
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockDelegate::createNameEditor()
//-----------------------------------------------------------------------------
QWidget* MemoryBlockDelegate::createNameEditor(QWidget* parent, QStyleOptionViewItem const& option, 
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
