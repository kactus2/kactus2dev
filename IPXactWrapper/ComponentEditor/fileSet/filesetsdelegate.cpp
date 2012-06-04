/* 
 *  	Created on: 29.5.2012
 *      Author: Antti Kamppi
 * 		filename: filesetsdelegate.cpp
 *		Project: Kactus 2
 */

#include "filesetsdelegate.h"
#include <common/widgets/listManager/listeditor.h>
#include <IPXactWrapper/ComponentEditor/fileSet/groupmanagerdelegate.h>

#include <QStringList>
#include <QLineEdit>

FileSetsDelegate::FileSetsDelegate(QObject *parent):
QStyledItemDelegate(parent) {
}

FileSetsDelegate::~FileSetsDelegate() {
}

QWidget* FileSetsDelegate::createEditor( QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index ) const {
	
	switch (index.column()) {
		// name and description uses the regular line edit
		case FileSetsDelegate::NAME_COLUMN:
		case FileSetsDelegate::DESC_COLUMN: {
			
			QLineEdit* edit = new QLineEdit(parent);
			connect(edit, SIGNAL(editingFinished()),
				this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
			return edit;
											}
		// group is a list and needs a specific editor
		case FileSetsDelegate::GROUP_COLUMN: {
			// create the editor
			ListEditor* editor = new ListEditor(parent);
			editor->setMinimumHeight(FileSetsDelegate::LIST_EDITOR_MIN_HEIGHT);

			// set the editing delegate for the editor
			GroupManagerDelegate* groupDelegate = new GroupManagerDelegate(parent);
			editor->setItemDelegate(groupDelegate);

			return editor;
											 }
		default: {
			return QStyledItemDelegate::createEditor(parent, option, index);
				 }
	}
}

void FileSetsDelegate::setEditorData( QWidget* editor, const QModelIndex& index ) const {
	
	switch (index.column()) {
		case FileSetsDelegate::NAME_COLUMN:
		case FileSetsDelegate::DESC_COLUMN: {
			QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
			Q_ASSERT(edit);

			const QString text = index.model()->data(index, Qt::DisplayRole).toString();
			edit->setText(text);
			break;
											}
		case FileSetsDelegate::GROUP_COLUMN: {
			ListEditor* listEditor = qobject_cast<ListEditor*>(editor);
			Q_ASSERT(listEditor);
			QStringList groups = index.model()->data(index, FileSetsDelegate::USER_DISPLAY_ROLE).toStringList();
			listEditor->setItems(groups);

			break;
											 }
		default: {
			QStyledItemDelegate::setEditorData(editor, index);
			break;
				 }
	}	
}

void FileSetsDelegate::setModelData( QWidget* editor, QAbstractItemModel* model, const QModelIndex& index ) const {
	
	switch (index.column()) {
		case FileSetsDelegate::NAME_COLUMN:
		case FileSetsDelegate::DESC_COLUMN: {
			QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
			Q_ASSERT(edit);

			QString text = edit->text();
			model->setData(index, text, Qt::EditRole);
			break;
											}
		case FileSetsDelegate::GROUP_COLUMN: {
			ListEditor* listEditor = qobject_cast<ListEditor*>(editor);
			Q_ASSERT(listEditor);

			QStringList groups = listEditor->items();
			model->setData(index, groups, FileSetsDelegate::USER_EDIT_ROLE);

			break;
											 }
		default: {
			QStyledItemDelegate::setModelData(editor, model, index);
			break;
				 }
	}
}

void FileSetsDelegate::commitAndCloseEditor() {
	QWidget* edit = qobject_cast<QWidget*>(sender());
	Q_ASSERT(edit);

	emit commitData(edit);
	emit closeEditor(edit);
}
