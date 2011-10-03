/* 
 *
 *  Created on: 31.3.2011
 *      Author: Antti Kamppi
 * 		filename: usagedelegate.cpp
 */

#include "usagedelegate.h"

#include <QComboBox>
#include <QLineEdit>

UsageDelegate::UsageDelegate( int usageColumn, QObject *parent /*= 0*/ ):
QStyledItemDelegate(parent), usageColumn_(usageColumn) {
}

UsageDelegate::~UsageDelegate() {
}

QWidget* UsageDelegate::createEditor(QWidget* parent, 
									 const QStyleOptionViewItem&, 
									 const QModelIndex& index ) const {

	// if the column is the one specified for usage items 
	if (index.column() == usageColumn_) {
		
		QComboBox* combo = new QComboBox(parent);
		combo->addItem(QString("typed"));
		combo->addItem(QString("nontyped"));
		connect(combo, SIGNAL(currentIndexChanged(int)),
			this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
		return combo;
	}
	// if column was not the one for the usage
	else {
		
		// use line edit to edit all other columns
		QLineEdit* editor = new QLineEdit(parent);
		connect(editor, SIGNAL(editingFinished()),
			this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
		return editor;
	}
}

void UsageDelegate::setEditorData( QWidget* editor, 
								  const QModelIndex& index ) const {

	// if the column is the one specified for usage
	if (index.column() == usageColumn_) {

		QString text = index.model()->data(index, Qt::DisplayRole).toString();
		QComboBox* combo = qobject_cast<QComboBox*>(editor);
		
		int comboIndex = combo->findText(text);
		combo->setCurrentIndex(comboIndex);
	}

	// if column was some other than usage
	else {
		// use the line edit for other columns
		QString text = index.model()->data(index, Qt::DisplayRole).toString();
		QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editor);
		lineEdit->setText(text);
	}
}

void UsageDelegate::setModelData( QWidget* editor, QAbstractItemModel* model,
								 const QModelIndex& index ) const {

	// if the column is the one specified for usage
	if (index.column() == usageColumn_) {

		QComboBox* combo = qobject_cast<QComboBox*>(editor);
		QString text = combo->currentText();
		model->setData(index, text, Qt::EditRole);
	}

	// if column was some other than usage
	else {
		// get the text from line edit for other columns
		QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editor);
		QString text = lineEdit->text();
		model->setData(index, text, Qt::EditRole);
	}
}

void UsageDelegate::commitAndCloseEditor() {
	
	// try to get pointer to editor in both cases
	QComboBox* combo = qobject_cast<QComboBox*>(sender());
	QLineEdit* lineEdit = qobject_cast<QLineEdit*>(sender());

	// if the editor was combo box
	if (combo) {
		emit commitData(combo);
		emit closeEditor(combo);
	}

	// if editor was line edit
	else {
		emit commitData(lineEdit);
		emit closeEditor(lineEdit);
	}
}
