/* 
 *  	Created on: 27.6.2012
 *      Author: Antti Kamppi
 * 		filename: apiinterfacesdelegate.cpp
 *		Project: Kactus 2
 */

#include "apiinterfacesdelegate.h"

#include <QLineEdit>
#include <QComboBox>

ApiInterfacesDelegate::ApiInterfacesDelegate(QObject *parent):
QStyledItemDelegate(parent) {
}

ApiInterfacesDelegate::~ApiInterfacesDelegate() {
}

QWidget* ApiInterfacesDelegate::createEditor( QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index ) const {
	switch (index.column()) {
		case NAME_COLUMN:
		case DISPLAY_NAME_COLUMN:
		case DESCRIPTION_COLUMN: {
			QLineEdit* edit = new QLineEdit(parent);
			connect(edit, SIGNAL(editingFinished()),
				this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
			return edit;
								 }
		case API_DEF_COLUMN: {
			Q_ASSERT(false);
			return NULL;
							 }
		case DEPENDENCY_COLUMN: {
			QComboBox* combo = new QComboBox(parent);
			combo->addItem(tr("requester"));
			combo->addItem(tr("provider"));
			combo->setCurrentIndex(0);
			combo->setEditable(false);
			return combo;
								}
		default: {
			return QStyledItemDelegate::createEditor(parent, option, index);
				 }
	}
}

void ApiInterfacesDelegate::setEditorData( QWidget* editor, const QModelIndex& index ) const {
	switch (index.column()) {
		case NAME_COLUMN:
		case DISPLAY_NAME_COLUMN:
		case DESCRIPTION_COLUMN: {
			QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
			Q_ASSERT(edit);

			const QString text = index.model()->data(index, Qt::DisplayRole).toString();
			edit->setText(text);
			break;
								 }
		case API_DEF_COLUMN: {
			Q_ASSERT(false);
			break;
							 }
		case DEPENDENCY_COLUMN: {
			QComboBox* combo = qobject_cast<QComboBox*>(editor);
			Q_ASSERT(combo);

			QString selectedText = index.model()->data(index, Qt::DisplayRole).toString();
			int indexToSelect = combo->findText(selectedText);
			combo->setCurrentIndex(indexToSelect);
			break;
								}
		default: {
			QStyledItemDelegate::setEditorData(editor, index);
			break;
				 }
	}
}

void ApiInterfacesDelegate::setModelData( QWidget* editor, QAbstractItemModel* model, const QModelIndex& index ) const {
	switch (index.column()) {
		case NAME_COLUMN:
		case DISPLAY_NAME_COLUMN:
		case DESCRIPTION_COLUMN: {
			QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
			Q_ASSERT(edit);

			QString text = edit->text();
			model->setData(index, text, Qt::EditRole);
			break;
								 }
		case API_DEF_COLUMN: {
			Q_ASSERT(false);
			break;
							 }
		case DEPENDENCY_COLUMN: {
			QComboBox* combo = qobject_cast<QComboBox*>(editor);
			Q_ASSERT(combo);

			QString text = combo->currentText();
			model->setData(index, text, Qt::EditRole);
			break;
								}
		default: {
			QStyledItemDelegate::setModelData(editor, model, index);
			break;
				 }
	}
}

void ApiInterfacesDelegate::commitAndCloseEditor() {
	QWidget* edit = qobject_cast<QWidget*>(sender());
	Q_ASSERT(edit);

	emit commitData(edit);
	emit closeEditor(edit);
}
