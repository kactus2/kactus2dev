/* 
 *
 *  Created on: 13.4.2011
 *      Author: Antti Kamppi
 * 		filename: portmapsdelegate.cpp
 */

#include "portmapsdelegate.h"

#include <models/component.h>
#include <models/port.h>

#include <QLineEdit>
#include <QRegExpValidator>
#include <QRegExp>

static const int LOGICAL_COLUMN = 2;
static const int PHYSICAL_COLUMN = 3;

PortMapsDelegate::PortMapsDelegate(QObject *parent): 
QStyledItemDelegate(parent) {
}

PortMapsDelegate::~PortMapsDelegate() {
}

QWidget* PortMapsDelegate::createEditor( QWidget* parent, 
										const QStyleOptionViewItem&,
										const QModelIndex& index ) const {
	// if editing the port name
	if (index.column() == LOGICAL_COLUMN || index.column() == PHYSICAL_COLUMN) {
		QLineEdit* editor = new QLineEdit(parent);
		connect(editor, SIGNAL(editingFinished()),
			this, SLOT(commitAndCloseEditor()));
		return editor;
	}
	// if editing the boundary of a port
	else {
		QLineEdit* editor = new QLineEdit(parent);

		QRegExp regExp(QString("[0-9]*"), Qt::CaseInsensitive, QRegExp::W3CXmlSchema11);
		QRegExpValidator* validator = new QRegExpValidator(regExp, editor);
		editor->setValidator(validator);

		connect(editor, SIGNAL(editingFinished()),
			this, SLOT(commitAndCloseEditor()));
		return editor;
	}
	
}

void PortMapsDelegate::setEditorData( QWidget* editor, 
									 const QModelIndex& index ) const {
										
	// if editing the port name
	if (index.column() == LOGICAL_COLUMN || index.column() == PHYSICAL_COLUMN) {
		QLineEdit* lineEditor = qobject_cast<QLineEdit*>(editor);
		QString text = index.model()->data(index, Qt::DisplayRole).toString();
		lineEditor->setText(text);
	}
	// if editing the port boundary
	else {
		QLineEdit* intEditor = qobject_cast<QLineEdit*>(editor);
				
		QString value = index.model()->data(index, Qt::DisplayRole).toString();
		intEditor->setText(value);
	}

}

void PortMapsDelegate::setModelData( QWidget* editor, 
									QAbstractItemModel* model,
									const QModelIndex& index ) const {

	// if editing the port name
	if (index.column() == LOGICAL_COLUMN || index.column() == PHYSICAL_COLUMN) {
		QLineEdit* lineEditor = qobject_cast<QLineEdit*>(editor);
		QString text = lineEditor->text();
		model->setData(index, text, Qt::EditRole);
	}
	// if editing the physical port
	else {
		QLineEdit* intEditor = qobject_cast<QLineEdit*>(editor);
		QString value = intEditor->text();
		model->setData(index, value, Qt::EditRole);
	}
}

void PortMapsDelegate::commitAndCloseEditor() {

	QLineEdit* lineEditor = qobject_cast<QLineEdit*>(sender());

	if (lineEditor) {
		emit commitData(lineEditor);
		emit closeEditor(lineEditor);
	}
}
