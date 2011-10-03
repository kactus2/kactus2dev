/* 
 *
 *  Created on: 4.4.2011
 *      Author: Antti Kamppi
 * 		filename: parametersdelegate.cpp
 */

#include "parametersdelegate.h"

#include <QLineEdit>

ParametersDelegate::ParametersDelegate(QObject *parent): 
QStyledItemDelegate(parent) {
}

ParametersDelegate::~ParametersDelegate() {
}

QWidget* ParametersDelegate::createEditor( QWidget* parent, 
										  const QStyleOptionViewItem&, 
										  const QModelIndex&) const {
	QLineEdit* editor = new QLineEdit(parent);
	connect(editor, SIGNAL(editingFinished()),
		this, SLOT(commitAndCloseEditor()));
	return editor;
}

void ParametersDelegate::setEditorData( QWidget* editor, 
									   const QModelIndex& index ) const {
	QLineEdit* lineEditor = qobject_cast<QLineEdit*>(editor);
	QString text = index.model()->data(index, Qt::DisplayRole).toString();
	lineEditor->setText(text);
}

void ParametersDelegate::setModelData( QWidget* editor, 
									  QAbstractItemModel* model, 
									  const QModelIndex& index ) const {
	QLineEdit* lineEditor = qobject_cast<QLineEdit*>(editor);
	QString text = lineEditor->text();
	model->setData(index, text, Qt::EditRole);
}

void ParametersDelegate::commitAndCloseEditor() {

	QLineEdit* editor = qobject_cast<QLineEdit*>(sender());
	emit commitData(editor);
	emit closeEditor(editor);
}
