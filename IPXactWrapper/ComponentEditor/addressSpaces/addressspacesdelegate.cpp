/* 
 *  	Created on: 11.6.2012
 *      Author: Antti Kamppi
 * 		filename: addressspacesdelegate.cpp
 *		Project: Kactus 2
 */

#include "addressspacesdelegate.h"
#include <common/widgets/NumberLineEdit/numberlineedit.h>

#include <QLineEdit>
#include <QSpinBox>

AddressSpacesDelegate::AddressSpacesDelegate(QObject *parent):
QStyledItemDelegate(parent) {
}

AddressSpacesDelegate::~AddressSpacesDelegate() {
}

QWidget* AddressSpacesDelegate::createEditor( QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index ) const {
	switch (index.column()) {
		case NAME_COLUMN:
		case DESCRIPTION_COLUMN: {
			QLineEdit* lineEdit = new QLineEdit(parent);
			connect(lineEdit, SIGNAL(editingFinished()),
				this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
			return lineEdit;
								 }
		case RANGE_COLUMN: {
			NumberLineEdit* numberEdit = new NumberLineEdit(parent);
			connect(numberEdit, SIGNAL(editingFinished()),
				this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
			return numberEdit;
						   }
		case ADDR_UNIT_COLUMN: {
			QSpinBox* spinBox = new QSpinBox(parent);
			spinBox->setRange(1, 4096);
			connect(spinBox, SIGNAL(editingFinished()),
				this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
			return spinBox;
							   }
		case WIDTH_COLUMN: {
			QSpinBox* spinBox = new QSpinBox(parent);
			spinBox->setRange(0, 4096);
			connect(spinBox, SIGNAL(editingFinished()),
				this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
			return spinBox;
							   }
		default: {
			return QStyledItemDelegate::createEditor(parent, option, index);
				 }
	}
}

void AddressSpacesDelegate::setEditorData( QWidget* editor, const QModelIndex& index ) const {
	switch (index.column()) {
		case NAME_COLUMN:
		case RANGE_COLUMN:
		case DESCRIPTION_COLUMN: {
			
			QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editor);
			Q_ASSERT(lineEdit);

			// set the text from model to the editor
			QString text = index.model()->data(index, Qt::DisplayRole).toString();
			lineEdit->setText(text);
			break;
								 }
		case ADDR_UNIT_COLUMN: 
		case WIDTH_COLUMN: {
			QSpinBox* spinBox = qobject_cast<QSpinBox*>(editor);
			Q_ASSERT(spinBox);

			unsigned int number = index.model()->data(index, Qt::DisplayRole).toUInt();
			spinBox->setValue(number);
			break;
						   }
		default: {
			QStyledItemDelegate::setEditorData(editor, index);
				 }
	}
}

void AddressSpacesDelegate::setModelData( QWidget* editor, QAbstractItemModel* model, const QModelIndex& index ) const {
	switch (index.column()) {
		case NAME_COLUMN:
		case RANGE_COLUMN:
		case DESCRIPTION_COLUMN: {
			QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editor);
			Q_ASSERT(lineEdit);

			QString text = lineEdit->text();
			model->setData(index, text, Qt::EditRole);
			break;
								 }
		case ADDR_UNIT_COLUMN: 
		case WIDTH_COLUMN: {
			QSpinBox* spinBox = qobject_cast<QSpinBox*>(editor);
			Q_ASSERT(spinBox);

			int number = spinBox->value();
			model->setData(index, number, Qt::EditRole);
			break;
						   }
		default: {
			QStyledItemDelegate::setModelData(editor, model, index);
			break;
				 }
	}
}

void AddressSpacesDelegate::commitAndCloseEditor() {
	QWidget* edit = qobject_cast<QWidget*>(sender());
	Q_ASSERT(edit);

	emit commitData(edit);
	emit closeEditor(edit);
}
