/* 
 *  	Created on: 26.8.2011
 *      Author: Antti Kamppi
 * 		filename: activeviewdelegate.cpp
 *		Project: Kactus 2
 */

#include "activeviewdelegate.h"

#include <QComboBox>
#include <QStringList>
#include <QString>

ActiveViewDelegate::ActiveViewDelegate(QObject *parent):
QStyledItemDelegate(parent) {

}

ActiveViewDelegate::~ActiveViewDelegate() {
}

QWidget* ActiveViewDelegate::createEditor( QWidget* parent, 
										  const QStyleOptionViewItem& option, 
										  const QModelIndex& index ) const {

	if (index.column() == 1) {
		QComboBox* comboBox = new QComboBox(parent);
		return comboBox;
	}
	else
		return QStyledItemDelegate::createEditor(parent, option, index);
}

void ActiveViewDelegate::setEditorData( QWidget* editor, const QModelIndex& index ) const {

	if (index.column() == 1) {
		QComboBox* comboBox = qobject_cast<QComboBox*>(editor);
		Q_ASSERT(comboBox);

		// add the possible values for the combo box
		QStringList viewNames = index.model()->data(index, Qt::UserRole).toStringList();
		comboBox->clear();
		comboBox->addItems(viewNames);
		
		// find the index of the currently selected item and set it as current.
		QString current = index.model()->data(index, Qt::DisplayRole).toString();
		int selectedIndex = comboBox->findText(current);
		comboBox->setCurrentIndex(selectedIndex);
	}
	else 
		QStyledItemDelegate::setEditorData(editor, index);
}

void ActiveViewDelegate::setModelData( QWidget* editor, QAbstractItemModel* model, 
									  const QModelIndex& index ) const {
										  
	if (index.column() == 1) {
		QComboBox* comboBox = qobject_cast<QComboBox*>(editor);
		Q_ASSERT(comboBox);

		QString text = comboBox->currentText();
		model->setData(index, text, Qt::EditRole);
	}
	else 
		QStyledItemDelegate::setModelData(editor, model, index);
}

