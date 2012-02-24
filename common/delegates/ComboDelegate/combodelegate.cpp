/* 
 *  	Created on: 9.12.2011
 *      Author: Antti Kamppi
 * 		filename: combodelegate.cpp
 *		Project: Kactus 2
 */

#include "combodelegate.h"

#include <QComboBox>

ComboDelegate::ComboDelegate(QObject *parent):
QStyledItemDelegate(parent) {
}

ComboDelegate::~ComboDelegate() {
}

QWidget* ComboDelegate::createEditor( QWidget* parent,
									 const QStyleOptionViewItem&,
									 const QModelIndex&) const {

	QComboBox* combo = new QComboBox(parent);
	combo->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	combo->setMinimumHeight(ComboDelegate::MINIMUM_EDITOR_HEIGHT);
	return combo;
}

void ComboDelegate::setEditorData( QWidget* editor, 
								  const QModelIndex& index ) const {

	QString text = index.model()->data(index, Qt::DisplayRole).toString();

	QComboBox* combo = qobject_cast<QComboBox*>(editor);
	Q_ASSERT(combo);

	int rowNumber = combo->findText(text);
	
	// if the current text was not found
	if (rowNumber < 0) {
		combo->setEditText(text);
	}
	// if text was found then select it
	else {
		combo->setCurrentIndex(rowNumber);
	}
}

void ComboDelegate::setModelData( QWidget* editor, 
								 QAbstractItemModel* model, 
								 const QModelIndex& index ) const {

	QComboBox* combo = qobject_cast<QComboBox*>(editor);
	Q_ASSERT(combo);
	QString text = combo->currentText();
	model->setData(index, text, Qt::EditRole);
}
