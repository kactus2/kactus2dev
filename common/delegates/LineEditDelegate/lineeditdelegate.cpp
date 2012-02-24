/* 
 *  	Created on: 9.12.2011
 *      Author: Antti Kamppi
 * 		filename: lineeditdelegate.cpp
 *		Project: Kactus 2
 */

#include "lineeditdelegate.h"

#include <QLineEdit>

LineEditDelegate::LineEditDelegate(QObject *parent):
QStyledItemDelegate(parent) {
}

LineEditDelegate::~LineEditDelegate() {
}

QWidget* LineEditDelegate::createEditor( QWidget* parent, 
										const QStyleOptionViewItem&, 
										const QModelIndex&) const {

	QLineEdit* edit = new QLineEdit(parent);
	connect(edit, SIGNAL(editingFinished()),
		this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
	edit->setMinimumHeight(LineEditDelegate::MINIMUM_EDITOR_HEIGHT);
	return edit;
}

void LineEditDelegate::setEditorData( QWidget* editor,
									 const QModelIndex& index ) const {

	QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
	Q_ASSERT(edit);

	const QString text = index.model()->data(index, Qt::DisplayRole).toString();
	edit->setText(text);
}

void LineEditDelegate::setModelData( QWidget* editor, 
									QAbstractItemModel* model,
									const QModelIndex& index ) const {

	QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
	Q_ASSERT(edit);

	QString text = edit->text();
	model->setData(index, text, Qt::EditRole);
}

void LineEditDelegate::commitAndCloseEditor() {
	QLineEdit* edit = qobject_cast<QLineEdit*>(sender());
	Q_ASSERT(edit);

	emit commitData(edit);
	emit closeEditor(edit);
}
