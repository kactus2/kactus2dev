//-----------------------------------------------------------------------------
// File: filesdelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 6.6.2012
//
// Description:
// Delegate for creating editors for files.
//-----------------------------------------------------------------------------

#include "filesdelegate.h"
#include "filetypeeditordelegate.h"

#include <common/widgets/listManager/listeditor.h>

#include <QComboBox>
#include <QLineEdit>
#include <QTextEdit>

namespace
{
    enum FilesColumns
    {
        NAME_COLUMN = 0,
        PATH_COLUMN,
        TYPES_COLUMN,
        DESCRIPTION
    };
}

//-----------------------------------------------------------------------------
// Function: FilesDelegate::FilesDelegate()
//-----------------------------------------------------------------------------
FilesDelegate::FilesDelegate(QObject *parent):
MultilineDescriptionDelegate(parent)
{
}


//-----------------------------------------------------------------------------
// Function: FilesDelegate::~FilesDelegate()
//-----------------------------------------------------------------------------
FilesDelegate::~FilesDelegate()
{
}

//-----------------------------------------------------------------------------
// Function: FilesDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* FilesDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, QModelIndex const& index) const
{
	if (index.column() == NAME_COLUMN || index.column() == PATH_COLUMN)
	{
        return 0;
	}
    else if (index.column() == TYPES_COLUMN)
    {
        // create the editor
        ListEditor* listEditor = new ListEditor(parent);
        listEditor->setMinimumHeight(FilesDelegate::LIST_EDITOR_MIN_HEIGHT);

        // create the delegate for the editor
        FileTypeEditorDelegate* delegate = new FileTypeEditorDelegate(parent);
        listEditor->setItemDelegate(delegate);

        return listEditor;
    }
    else 
    {
        return MultilineDescriptionDelegate::createEditor(parent, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: FilesDelegate::setEditorData()
//-----------------------------------------------------------------------------
void FilesDelegate::setEditorData(QWidget* editor, QModelIndex const& index ) const
{
    if (index.column() == NAME_COLUMN || index.column() == PATH_COLUMN)
    {
        return;
    }

    if (index.column() == TYPES_COLUMN)
    {
        ListEditor* listEditor = qobject_cast<ListEditor*>(editor);
        Q_ASSERT(listEditor);
        QStringList groups = index.data(Qt::DisplayRole).toString().split(' ', QString::SkipEmptyParts);
        listEditor->setItems(groups);
    }
    else
    {
        MultilineDescriptionDelegate::setEditorData(editor, index);
    }
}

//-----------------------------------------------------------------------------
// Function: FilesDelegate::setModelData()
//-----------------------------------------------------------------------------
void FilesDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const
{
	if (index.column() == NAME_COLUMN || index.column() == PATH_COLUMN)
    {
        return;
    }
    else if (index.column() == TYPES_COLUMN)
    {
        ListEditor* listEditor = qobject_cast<ListEditor*>(editor);
        Q_ASSERT(listEditor);

        QStringList fileTypes = listEditor->items();
        model->setData(index, fileTypes.join(' '), Qt::EditRole);
    }
    else
    {
        MultilineDescriptionDelegate::setModelData(editor, model, index);
    }
}

//-----------------------------------------------------------------------------
// Function: FilesDelegate::descriptionColumn()
//-----------------------------------------------------------------------------
int FilesDelegate::descriptionColumn() const
{
    return DESCRIPTION;
}

//-----------------------------------------------------------------------------
// Function: FilesDelegate::commitAndCloseEditor()
//-----------------------------------------------------------------------------
void FilesDelegate::commitAndCloseEditor()
{
	QWidget* edit = qobject_cast<QWidget*>(sender());
	Q_ASSERT(edit);

	emit commitData(edit);
	emit closeEditor(edit);
}
