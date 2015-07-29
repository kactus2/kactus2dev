/* 
 *  	Created on: 29.5.2012
 *      Author: Antti Kamppi
 * 		filename: filesetsdelegate.cpp
 *		Project: Kactus 2
 */

#include "filesetsdelegate.h"

#include <common/widgets/listManager/listeditor.h>
#include <editors/ComponentEditor/fileSet/groupmanagerdelegate.h>

#include <QStringList>
#include <QLineEdit>
#include <QTextEdit>

//-----------------------------------------------------------------------------
// Function: FileSetsDelegate::FileSetsDelegate()
//-----------------------------------------------------------------------------
FileSetsDelegate::FileSetsDelegate(QObject *parent):
MultilineDescriptionDelegate(parent)
{
}

//-----------------------------------------------------------------------------
// Function: FileSetsDelegate::~FileSetsDelegate()
//-----------------------------------------------------------------------------
FileSetsDelegate::~FileSetsDelegate()
{
}

//-----------------------------------------------------------------------------
// Function: FileSetsDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* FileSetsDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option,
    QModelIndex const& index ) const
{
	if (index.column() == NAME_COLUMN)
    {
        QLineEdit* edit = new QLineEdit(parent);
        connect(edit, SIGNAL(editingFinished()), this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
        return edit;
    }
    else if (index.column() == GROUP_COLUMN)
    {
        ListEditor* editor = new ListEditor(parent);
        editor->setMinimumHeight(FileSetsDelegate::LIST_EDITOR_MIN_HEIGHT);

        // set the editing delegate for the editor
        GroupManagerDelegate* groupDelegate = new GroupManagerDelegate(parent);
        editor->setItemDelegate(groupDelegate);

        return editor;
    }
    else
    {
        return MultilineDescriptionDelegate::createEditor(parent, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: FileSetsDelegate::setEditorData()
//-----------------------------------------------------------------------------
void FileSetsDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const
{
    if (index.column() == NAME_COLUMN)
    {
        QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
        const QString text = index.data(Qt::DisplayRole).toString();
        edit->setText(text);
    }
    else if (index.column() == GROUP_COLUMN)
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
// Function: FileSetsDelegate::setModelData()
//-----------------------------------------------------------------------------
void FileSetsDelegate::setModelData( QWidget* editor, QAbstractItemModel* model, QModelIndex const& index ) const
{	
    if (index.column() == NAME_COLUMN)
    {
        QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
        Q_ASSERT(edit);

        QString text = edit->text();
        model->setData(index, text, Qt::EditRole);
    }
    else if (index.column() == GROUP_COLUMN)
    {
        ListEditor* listEditor = qobject_cast<ListEditor*>(editor);
        Q_ASSERT(listEditor);

        QStringList groups = listEditor->items();
        model->setData(index, groups.join(' '), Qt::EditRole);
    }
    else
    {
        MultilineDescriptionDelegate::setModelData(editor, model, index);
    }
}

//-----------------------------------------------------------------------------
// Function: FileSetsDelegate::descriptionColumn()
//-----------------------------------------------------------------------------
int FileSetsDelegate::descriptionColumn() const
{
    return DESC_COLUMN;
}

//-----------------------------------------------------------------------------
// Function: FileSetsDelegate::commitAndCloseEditor()
//-----------------------------------------------------------------------------
void FileSetsDelegate::commitAndCloseEditor()
{
	QWidget* edit = qobject_cast<QWidget*>(sender());
	Q_ASSERT(edit);

	emit commitData(edit);
	emit closeEditor(edit);
}
