/* 
 *  	Created on: 28.6.2012
 *      Author: Antti Kamppi
 * 		filename: cominterfacesdelegate.cpp
 *		Project: Kactus 2
 */

#include "cominterfacesdelegate.h"

#include <QLineEdit>
#include <QComboBox>
#include <QStringList>

ComInterfacesDelegate::ComInterfacesDelegate(QObject *parent):
QStyledItemDelegate(parent) {
}

ComInterfacesDelegate::~ComInterfacesDelegate() {
}

QWidget* ComInterfacesDelegate::createEditor( QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index ) const {
	switch (index.column()) {
		case NAME_COLUMN: 
		case DESCRIPTION_COLUMN: {
			QLineEdit* edit = new QLineEdit(parent);
			connect(edit, SIGNAL(editingFinished()),
				this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
			return edit;
								 }
		case COM_DEF_COLUMN: {
			Q_ASSERT(false);
			return NULL;
							 }
		case TRANSFER_TYPE_COLUMN: {
			QComboBox* combo = new QComboBox(parent);
			return combo;
								   }
		case DIRECTION_COLUMN: {
			QComboBox* combo = new QComboBox(parent);
			combo->addItem(tr("in"));
			combo->addItem(tr("out"));
			combo->addItem(tr("inout"));
			combo->setCurrentIndex(0);
			return combo;
							   }
		default: {
			return QStyledItemDelegate::createEditor(parent, option, index);
				 }
	}
}

void ComInterfacesDelegate::setEditorData( QWidget* editor, const QModelIndex& index ) const {
	switch (index.column()) {
		case NAME_COLUMN: 
		case DESCRIPTION_COLUMN: {
			QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
			Q_ASSERT(edit);

			const QString text = index.model()->data(index, Qt::DisplayRole).toString();
			edit->setText(text);
			break;
								 }
		case COM_DEF_COLUMN: {
			Q_ASSERT(false);
			break;
							 }
		case TRANSFER_TYPE_COLUMN: {
			QComboBox* combo = qobject_cast<QComboBox*>(editor);
			Q_ASSERT(combo);

			// remove the previous items
			combo->clear();

			// add the possible options for transfer types.
			QStringList types = index.model()->data(index, TRANSFER_TYPE_OPTIONS).toStringList();
			combo->addItem("");
			combo->addItems(types);

			// select the right option
			QString selected = index.model()->data(index, Qt::DisplayRole).toString();
			int comboIndex = combo->findText(selected);
			combo->setCurrentIndex(comboIndex);
			break;
								   }
		case DIRECTION_COLUMN: {
			QComboBox* combo = qobject_cast<QComboBox*>(editor);
			Q_ASSERT(combo);

			QString dir = index.model()->data(index, Qt::DisplayRole).toString();
			int comboIndex = combo->findText(dir);
			combo->setCurrentIndex(comboIndex);
			break;
							   }
		default: {
			QStyledItemDelegate::setEditorData(editor, index);
			break;
				 }
	}
}

void ComInterfacesDelegate::setModelData( QWidget* editor, QAbstractItemModel* model, const QModelIndex& index ) const {
	switch (index.column()) {
		case NAME_COLUMN: 
		case DESCRIPTION_COLUMN: {
			QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
			Q_ASSERT(edit);

			QString text = edit->text();
			model->setData(index, text, Qt::EditRole);
			break;
								 }
		case COM_DEF_COLUMN: {
			Q_ASSERT(false);
			break;
							 }
		case TRANSFER_TYPE_COLUMN: {
			QComboBox* combo = qobject_cast<QComboBox*>(editor);
			Q_ASSERT(combo);

			QString type = combo->currentText();
			model->setData(index, type, Qt::EditRole);
			break;
								   }
		case DIRECTION_COLUMN: {
			QComboBox* combo = qobject_cast<QComboBox*>(editor);
			Q_ASSERT(combo);

			QString dir = combo->currentText();
			model->setData(index, dir, Qt::EditRole);
			break;
							   }
		default: {
			QStyledItemDelegate::setModelData(editor, model, index);
			break;
				 }
	}
}

void ComInterfacesDelegate::commitAndCloseEditor() {
	QWidget* edit = qobject_cast<QWidget*>(sender());
	Q_ASSERT(edit);

	emit commitData(edit);
	emit closeEditor(edit);
}
