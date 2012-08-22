/* 
 *  	Created on: 22.8.2012
 *      Author: Antti Kamppi
 * 		filename: memorymapsdelegate.cpp
 *		Project: Kactus 2
 */

#include "memorymapsdelegate.h"

#include <QLineEdit>
#include <QSpinBox>

MemoryMapsDelegate::MemoryMapsDelegate(QObject *parent):
QStyledItemDelegate(parent) {
}

MemoryMapsDelegate::~MemoryMapsDelegate() {
}

QWidget* MemoryMapsDelegate::createEditor( QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index ) const {
	switch (index.column()) {
		case MemoryMapsDelegate::NAME_COLUMN:
		case MemoryMapsDelegate::DESC_COLUMN: {
			QLineEdit* edit = new QLineEdit(parent);
			connect(edit, SIGNAL(editingFinished()),
				this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
			return edit;
											  }
		case MemoryMapsDelegate::AUB_COLUMN: {
			QSpinBox* spinBox = new QSpinBox(parent);
			connect(spinBox, SIGNAL(editingFinished()),
				this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
			spinBox->setMinimum(1);
			return spinBox;
											 }
		default: {
			return QStyledItemDelegate::createEditor(parent, option, index);
				 }
	}
}

void MemoryMapsDelegate::setEditorData( QWidget* editor, const QModelIndex& index ) const {
	switch (index.column()) {
		case MemoryMapsDelegate::NAME_COLUMN:
		case MemoryMapsDelegate::DESC_COLUMN: {
			QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
			Q_ASSERT(edit);

			const QString text = index.model()->data(index, Qt::DisplayRole).toString();
			edit->setText(text);
			break;
											  }
		case MemoryMapsDelegate::AUB_COLUMN: {
			QSpinBox* spinBox = qobject_cast<QSpinBox*>(editor);
			Q_ASSERT(spinBox);

			unsigned int value = index.model()->data(index, Qt::DisplayRole).toUInt();
			spinBox->setValue(value);
			break;
											 }
		default: {
			QStyledItemDelegate::setEditorData(editor, index);
			break;
				 }
	}
}

void MemoryMapsDelegate::setModelData( QWidget* editor, QAbstractItemModel* model, const QModelIndex& index ) const {
	switch (index.column()) {
		case MemoryMapsDelegate::NAME_COLUMN:
		case MemoryMapsDelegate::DESC_COLUMN: {
			QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
			Q_ASSERT(edit);

			QString text = edit->text();
			model->setData(index, text, Qt::EditRole);
			break;
											  }
		case MemoryMapsDelegate::AUB_COLUMN: {
			QSpinBox* spinBox = qobject_cast<QSpinBox*>(editor);
			Q_ASSERT(spinBox);

			unsigned int value = spinBox->value();
			model->setData(index, value, Qt::EditRole);
			break;
											 }
		default: {
			QStyledItemDelegate::setModelData(editor, model, index);
			break;
				 }
	}
}

void MemoryMapsDelegate::commitAndCloseEditor() {
	QWidget* edit = qobject_cast<QWidget*>(sender());
	Q_ASSERT(edit);

	emit commitData(edit);
	emit closeEditor(edit);
}
