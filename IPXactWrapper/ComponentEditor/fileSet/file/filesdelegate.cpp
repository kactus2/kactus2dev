/* 
 *  	Created on: 6.6.2012
 *      Author: Antti Kamppi
 * 		filename: filesdelegate.cpp
 *		Project: Kactus 2
 */

#include "filesdelegate.h"
#include "filetypeeditordelegate.h"
#include <common/widgets/listManager/listeditor.h>

#include <QLineEdit>

#include <QComboBox>

FilesDelegate::FilesDelegate(QObject *parent):
QStyledItemDelegate(parent) {
}

FilesDelegate::~FilesDelegate() {
}

QWidget* FilesDelegate::createEditor( QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index ) const {

	switch (index.column()) {
		// file name or path should never be edited directly
		case 0:
		case 1: {
			Q_ASSERT(false);
			return NULL;
				}
		case 2: {
			// create the editor
			ListEditor* listEditor = new ListEditor(parent);
			listEditor->setMinimumHeight(FilesDelegate::LIST_EDITOR_MIN_HEIGHT);

			// create the delegate for the editor
			FileTypeEditorDelegate* delegate = new FileTypeEditorDelegate(parent);
			listEditor->setItemDelegate(delegate);

			return listEditor;
				}
		case 3: {
			QLineEdit* edit = new QLineEdit(parent);
			connect(edit, SIGNAL(editingFinished()),
				this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
			return edit;
				}
		default: {
			return QStyledItemDelegate::createEditor(parent, option, index);
				 }
	}
}

void FilesDelegate::setEditorData( QWidget* editor, const QModelIndex& index ) const {
	switch (index.column()) {
		// file name or path can not be edited.
		case 0: 
		case 1: {
			Q_ASSERT(false);
			return;
				}
		case 2: {
			ListEditor* listEditor = qobject_cast<ListEditor*>(editor);
			Q_ASSERT(listEditor);
			QStringList groups = index.model()->data(index, FilesDelegate::USER_DISPLAY_ROLE).toStringList();
			listEditor->setItems(groups);
			break;
				}
		case 3: {
			QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
			Q_ASSERT(edit);

			const QString text = index.model()->data(index, Qt::DisplayRole).toString();
			edit->setText(text);
			break;
				}
		default: {
			QStyledItemDelegate::setEditorData(editor, index);
				 }
	}
}

void FilesDelegate::setModelData( QWidget* editor, QAbstractItemModel* model, const QModelIndex& index ) const {
	switch (index.column()) {
		// file name or path can not be edited directly
		case 0:
		case 1: {
			Q_ASSERT(false);
			return;
				}
		case 2: {
			ListEditor* listEditor = qobject_cast<ListEditor*>(editor);
			Q_ASSERT(listEditor);

			QStringList fileTypes = listEditor->items();
			model->setData(index, fileTypes, FilesDelegate::USER_EDIT_ROLE);
			break;
				}
		case 3: {
			QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
			Q_ASSERT(edit);

			QString text = edit->text();
			model->setData(index, text, Qt::EditRole);
			break;
				}
		default: {
			QStyledItemDelegate::setModelData(editor, model, index);
			break;
				 }
	}
}

void FilesDelegate::commitAndCloseEditor() {
	QWidget* edit = qobject_cast<QWidget*>(sender());
	Q_ASSERT(edit);

	emit commitData(edit);
	emit closeEditor(edit);
}
