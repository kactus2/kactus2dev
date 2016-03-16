//-----------------------------------------------------------------------------
// File: swviewsdelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 27.06.2012
//
// Description:
// The delegate class that provides editors for editing software views.
//-----------------------------------------------------------------------------

#include "swviewsdelegate.h"

#include <QLineEdit>

//-----------------------------------------------------------------------------
// Function: SWViewsDelegate::SWViewsDelegate()
//-----------------------------------------------------------------------------
SWViewsDelegate::SWViewsDelegate(QObject *parent):
QStyledItemDelegate(parent)
{
}

//-----------------------------------------------------------------------------
// Function: SWViewsDelegate::~SWViewsDelegate()
//-----------------------------------------------------------------------------
SWViewsDelegate::~SWViewsDelegate()
{
}

//-----------------------------------------------------------------------------
// Function: SWViewsDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* SWViewsDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const& option, 
    QModelIndex const& index) const
{
    if (index.column() == NAME_COLUMN ||
        index.column() == DISPLAY_NAME_COLUMN ||
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
// Function: SWViewsDelegate::setEditorData()
//-----------------------------------------------------------------------------
void SWViewsDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const
{
	if (index.column() == NAME_COLUMN ||
		index.column() == DISPLAY_NAME_COLUMN ||
		index.column() == DESCRIPTION_COLUMN)
    {
        QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
        Q_ASSERT(edit);

        edit->setText(index.data(Qt::DisplayRole).toString());
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
// Function: SWViewsDelegate::setModelData()
//-----------------------------------------------------------------------------
void SWViewsDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const
{
    if (index.column() == NAME_COLUMN || index.column() == DISPLAY_NAME_COLUMN ||
        index.column() == DESCRIPTION_COLUMN)
    {
        QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
        Q_ASSERT(edit);
        model->setData(index, edit->text(), Qt::EditRole);
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
// Function: SWViewsDelegate::commitAndCloseEditor()
//-----------------------------------------------------------------------------
void SWViewsDelegate::commitAndCloseEditor()
{
	QWidget* edit = qobject_cast<QWidget*>(sender());
	Q_ASSERT(edit);

	emit commitData(edit);
	emit closeEditor(edit);
}
