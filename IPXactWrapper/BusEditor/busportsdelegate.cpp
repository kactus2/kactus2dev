/* 
 *  Created on: 27.6.2011
 *      Author: Antti Kamppi
 * 		filename: busportsdelegate.cpp
 */

#include "busportsdelegate.h"

#include <QComboBox>
#include <QStringList>
#include <QLineEdit>
#include <QIntValidator>
#include <QSpinBox>

#include <QDebug>

BusPortsDelegate::BusPortsDelegate(QObject *parent): 
QStyledItemDelegate(parent) {
}

BusPortsDelegate::~BusPortsDelegate() {
}

QWidget* BusPortsDelegate::createEditor( QWidget* parent, 
										const QStyleOptionViewItem& option,
										const QModelIndex& index ) const {

	switch (index.column()) {
		// name and comment need only normal QLineEdit
		case 0:
		case 8: {
			QLineEdit* line = new QLineEdit(parent);
			connect(line, SIGNAL(editingFinished()),
				this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
			return line;
			  }
		// qualifier
		case 1: {
			QComboBox* box = new QComboBox(parent);
			QStringList list;
			list.append(QString("address"));
			list.append(QString("data"));
			list.append(QString("clock"));
			list.append(QString("reset"));
			list.append(QString("any"));
			box->addItems(list);
			return box;
				}
		// width
		case 2: {
			QSpinBox* box = new QSpinBox(parent);
			box->setRange(1, 99999);
			connect(box, SIGNAL(editingFinished()),
				this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
			return box;
				}
		// default value
		case 3: {
			// the editor
			QLineEdit* line = new QLineEdit(parent);

			// the validator for editor input
			QIntValidator* validator = new QIntValidator(line);
			validator->setRange(0, 99999);

			line->setValidator(validator);

			connect(line, SIGNAL(editingFinished()),
				this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
			return line;
				}
		// mode
		case 4: {
			QComboBox* box = new QComboBox(parent);
			QStringList list;
			list.append(QString("master"));
			list.append(QString("slave"));
			list.append(QString("system"));
			list.append(QString("any"));
			box->addItems(list);
			connect(box, SIGNAL(destroyed()),
				this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
			return box;
				}
		// direction of the port
		case 5: {
			QComboBox* box = new QComboBox(parent);
			QStringList list;
			list.append(QString("in"));
			list.append(QString("out"));
			list.append(QString("inout"));
			box->addItems(list);
			connect(box, SIGNAL(destroyed()),
				this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
			return box;
				}
		// presence
		case 6: {
			QComboBox* box = new QComboBox(parent);
			QStringList list;
			list.append(QString("required"));
			list.append(QString("optional"));
			list.append(QString("illegal"));
			box->addItems(list);
			connect(box, SIGNAL(destroyed()),
				this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
			return box;
				}
		// driver
		case 7: {
			QComboBox* box = new QComboBox(parent);
			QStringList list;
			list.append(QString("none"));
			list.append(QString("any"));
			list.append(QString("clock"));
			list.append(QString("singleshot"));
			box->addItems(list);
			connect(box, SIGNAL(destroyed()),
				this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
			return box;
				}
		
		default: {
			return QStyledItemDelegate::createEditor(parent, option, index);
				 }
	}
}

void BusPortsDelegate::setEditorData( QWidget* editor, 
									 const QModelIndex& index ) const {

	switch (index.column()) {
		// name and comment or default value
		case 0:
		case 8:
		case 3: {
			QLineEdit* line = qobject_cast<QLineEdit*>(editor);
			Q_ASSERT_X(line, "BusPortsDelegate::setEditorData",
				"Type conversion failed for QLineEdit");

			QString text = index.model()->data(index, Qt::DisplayRole).toString();
			line->setText(text);
			break;
			  }
		// qualifier, mode, direction, presence, driver
		case 1: 
		case 4:
		case 5: 
		case 6: 
		case 7: {
			QComboBox* box = qobject_cast<QComboBox*>(editor);
			Q_ASSERT_X(box, "BusPortsDelegate::setEditorData",
				"Type conversion failed for combo box");

			QString text = index.model()->data(index, Qt::DisplayRole).toString();
			int index = box->findText(text);

			box->setCurrentIndex(index);
			break;
				}
		// width
		case 2: {
			QSpinBox* box = qobject_cast<QSpinBox*>(editor);
			Q_ASSERT_X(box, "BusPortsDelegate::setEditorData",
				"Type conversion failed for width spin box");

			int width = index.model()->data(index, Qt::DisplayRole).toInt();
			box->setValue(width);
			break;
				}
		default: {
			QStyledItemDelegate::setEditorData(editor, index);
			break;
				 }
	}
}

void BusPortsDelegate::setModelData( QWidget* editor,
									QAbstractItemModel* model, 
									const QModelIndex& index ) const {

	switch (index.column()) {
		// name and comment or default value
		case 0:
		case 8: 
		case 3: {
			QLineEdit* line = qobject_cast<QLineEdit*>(editor);
			Q_ASSERT_X(line, "BusPortsDelegate::setModelData",
				"Type conversion failed for normal QLineEdit");

			QString text = line->text();
			model->setData(index, text, Qt::EditRole);
			break;
			  }
		// qualifier, mode, direction, presence, driver
		case 1: 
		case 4:
		case 5: 
		case 6: 
		case 7: {
			QComboBox* box = qobject_cast<QComboBox*>(editor);
			Q_ASSERT_X(box, "BusPortsDelegate::setModelData",
				"Type conversion failed for combo box");

			QString text = box->currentText();
			model->setData(index, text, Qt::EditRole);
			break;
				}
		// width
		case 2: {
			QSpinBox* box = qobject_cast<QSpinBox*>(editor);
			Q_ASSERT_X(box, "BusPortsDelegate::setModelData",
				"Type conversion failed for spin box");

			int value = box->value();
			model->setData(index, value, Qt::EditRole);
			break;
				}
		default: {
			QStyledItemDelegate::setModelData(editor, model, index);
			break;
				 }
	}
}

void BusPortsDelegate::commitAndCloseEditor() {
	// try to get pointer to editor in both cases
	QComboBox* combo = qobject_cast<QComboBox*>(sender());
	QLineEdit* lineEdit = qobject_cast<QLineEdit*>(sender());
	QSpinBox* spinBox = qobject_cast<QSpinBox*>(sender());

	// if the editor was combo box
	if (combo) {
		emit commitData(combo);
		emit closeEditor(combo);
	}

	// if the editor was spinBox
	else if (spinBox) {
		emit commitData(spinBox);
		emit closeEditor(spinBox);
	}

	// if editor was line edit
	else if (lineEdit) {
		emit commitData(lineEdit);
		emit closeEditor(lineEdit);
	}
}