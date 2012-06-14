/* 
 *  	Created on: 14.6.2012
 *      Author: Antti Kamppi
 * 		filename: cpusdelegate.cpp
 *		Project: Kactus 2
 */

#include "cpusdelegate.h"
#include <common/widgets/listManager/listeditor.h>
#include "addressspacerefdelegate.h"

#include <QLineEdit>
#include <QStringList>
#include <QString>

CpusDelegate::CpusDelegate( QSharedPointer<Component> component,
						   QObject *parent ):
QStyledItemDelegate(parent),
component_(component) {
	Q_ASSERT(component);
}

CpusDelegate::~CpusDelegate() {
}

QWidget* CpusDelegate::createEditor( QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index ) const {

	switch (index.column()) {
		case CpusDelegate::NAME_COLUMN:
		case CpusDelegate::DISPLAY_NAME_COLUMN:
		case CpusDelegate::DESCRIPTION_COLUMN: {
			QLineEdit* edit = new QLineEdit(parent);
			connect(edit, SIGNAL(editingFinished()),
				this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
			return edit;
											   }
		case CpusDelegate::ADDRSPACE_COLUMN: {
			// create the editor
			ListEditor* editor = new ListEditor(parent);
			editor->setMinimumHeight(CpusDelegate::LIST_EDITOR_MIN_HEIGHT);

			// set the editing delegate for the editor
			AddressSpaceRefDelegate* addrSpaceDelegate = new AddressSpaceRefDelegate(parent, component_);
			editor->setItemDelegate(addrSpaceDelegate);

			return editor;
											 }
		default: {
			return QStyledItemDelegate::createEditor(parent, option, index);
				 }
	}
}

void CpusDelegate::setEditorData( QWidget* editor, const QModelIndex& index ) const {
	switch (index.column()) {
		case CpusDelegate::NAME_COLUMN:
		case CpusDelegate::DISPLAY_NAME_COLUMN:
		case CpusDelegate::DESCRIPTION_COLUMN: {
			QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
			Q_ASSERT(edit);

			const QString text = index.model()->data(index, Qt::DisplayRole).toString();
			edit->setText(text);
			break;
											   }
		case CpusDelegate::ADDRSPACE_COLUMN: {
			ListEditor* listEditor = qobject_cast<ListEditor*>(editor);
			Q_ASSERT(listEditor);
			QStringList addrSpaces = index.model()->data(index, CpusDelegate::USER_DISPLAY_ROLE).toStringList();
			listEditor->setItems(addrSpaces);

			break;
											 }
		default: {
			QStyledItemDelegate::setEditorData(editor, index);
			break;
				 }
	}
}

void CpusDelegate::setModelData( QWidget* editor, QAbstractItemModel* model, const QModelIndex& index ) const {
	switch (index.column()) {
		case CpusDelegate::NAME_COLUMN:
		case CpusDelegate::DISPLAY_NAME_COLUMN:
		case CpusDelegate::DESCRIPTION_COLUMN: {
			QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
			Q_ASSERT(edit);

			QString text = edit->text();
			model->setData(index, text, Qt::EditRole);
			break;
											   }
		case CpusDelegate::ADDRSPACE_COLUMN: {
			ListEditor* listEditor = qobject_cast<ListEditor*>(editor);
			Q_ASSERT(listEditor);

			QStringList addrSpaces = listEditor->items();
			model->setData(index, addrSpaces, CpusDelegate::USER_EDIT_ROLE);

			break;
											 }
		default: {
			QStyledItemDelegate::setModelData(editor, model, index);
			break;
				 }
	}
}

void CpusDelegate::commitAndCloseEditor() {
	QWidget* edit = qobject_cast<QWidget*>(sender());
	Q_ASSERT(edit);

	emit commitData(edit);
	emit closeEditor(edit);
}
