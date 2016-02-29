//-----------------------------------------------------------------------------
// File: SystemViewsDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 13.7.2012
//
// Description:
// The delegate class that provides editors for editing system views.
//-----------------------------------------------------------------------------

#include "SystemViewsDelegate.h"

#include <QLineEdit>

//-----------------------------------------------------------------------------
// Function: SystemViewsDelegate::SystemViewsDelegate()
//-----------------------------------------------------------------------------
SystemViewsDelegate::SystemViewsDelegate(QObject *parent):
QStyledItemDelegate(parent) 
{
}

//-----------------------------------------------------------------------------
// Function: SystemViewsDelegate::~SystemViewsDelegate()
//-----------------------------------------------------------------------------
SystemViewsDelegate::~SystemViewsDelegate()
{
}

//-----------------------------------------------------------------------------
// Function: SystemViewsDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* SystemViewsDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const& option, 
    QModelIndex const& index) const
{
    if (index.column() == NAME_COLUMN || index.column() == DISPLAY_NAME_COLUMN ||
        index.column() == DESCRIPTION_COLUMN)
    {
        QLineEdit* edit = new QLineEdit(parent);
        connect(edit, SIGNAL(editingFinished()), this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
        return edit;
    }
    else if (index.column() == HIER_REF_COLUMN)
    {
        Q_ASSERT(false);
        return 0;
    }
    else
    {
        return QStyledItemDelegate::createEditor(parent, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: SystemViewsDelegate::setEditorData()
//-----------------------------------------------------------------------------
void SystemViewsDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const
{
    if (index.column() == NAME_COLUMN || index.column() == DISPLAY_NAME_COLUMN ||
        index.column() == DESCRIPTION_COLUMN)
    {
        QLineEdit* lineEditor = qobject_cast<QLineEdit*>(editor);
        Q_ASSERT(lineEditor);

        lineEditor->setText(index.data(Qt::DisplayRole).toString());
    }
    else if (index.column() == HIER_REF_COLUMN)
    {
        Q_ASSERT(false);
    }
    else
    {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

//-----------------------------------------------------------------------------
// Function: SystemViewsDelegate::setModelData()
//-----------------------------------------------------------------------------
void SystemViewsDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const
{
    if (index.column() == NAME_COLUMN || index.column() == DISPLAY_NAME_COLUMN ||
        index.column() == DESCRIPTION_COLUMN)
    {
        QLineEdit* lineEditor = qobject_cast<QLineEdit*>(editor);
        Q_ASSERT(lineEditor);

        model->setData(index, lineEditor->text(), Qt::EditRole);
    }
    else if (index.column() == HIER_REF_COLUMN)
    {
        Q_ASSERT(false);
    }
    else
    {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}

//-----------------------------------------------------------------------------
// Function: SystemViewsDelegate::commitAndCloseEditor()
//-----------------------------------------------------------------------------
void SystemViewsDelegate::commitAndCloseEditor()
{
	QWidget* edit = qobject_cast<QWidget*>(sender());
	Q_ASSERT(edit);

	emit commitData(edit);
	emit closeEditor(edit);
}
