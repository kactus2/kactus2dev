/* 
 *
 *  Created on: 18.4.2011
 *      Author: Antti Kamppi
 * 		filename: envidentifiersdelegate.cpp
 */

#include "envidentifiersdelegate.h"

#include <QLineEdit>

EnvIdentifiersDelegate::EnvIdentifiersDelegate(QObject *parent): 
QStyledItemDelegate(parent) {
}

EnvIdentifiersDelegate::~EnvIdentifiersDelegate() {
}

QWidget* EnvIdentifiersDelegate::createEditor( QWidget* parent, 
											  const QStyleOptionViewItem&,
											  const QModelIndex& ) const {

	QLineEdit* editor = new QLineEdit(parent);
	connect(editor, SIGNAL(editingFinished()),
	  this, SLOT(commitAndCloseEditor()));
	return editor;
}

void EnvIdentifiersDelegate::setEditorData( QWidget* editor, 
										   const QModelIndex& index ) const {

	QLineEdit* lineEditor = qobject_cast<QLineEdit*>(editor);
	QString text = index.model()->data(index, Qt::DisplayRole).toString();
	lineEditor->setText(text);
}

void EnvIdentifiersDelegate::setModelData( QWidget* editor,
										  QAbstractItemModel* model, 
										  const QModelIndex& index ) const {

	QLineEdit* lineEditor = qobject_cast<QLineEdit*>(editor);
	QString text = lineEditor->text();
	model->setData(index, text, Qt::EditRole);
}

void EnvIdentifiersDelegate::commitAndCloseEditor() {
	QLineEdit* editor = qobject_cast<QLineEdit*>(sender());
	emit commitData(editor);
	emit closeEditor(editor);
}
