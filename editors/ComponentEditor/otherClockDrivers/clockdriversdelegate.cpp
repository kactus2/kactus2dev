/* 
 *
 *  Created on: 4.4.2011
 *      Author: Antti Kamppi
 * 		filename: clockdriversdelegate.cpp
 */

#include "clockdriversdelegate.h"

#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>

ClockDriversDelegate::ClockDriversDelegate(QObject *parent): 
QStyledItemDelegate(parent) {
}

ClockDriversDelegate::~ClockDriversDelegate() {
}

QWidget* ClockDriversDelegate::createEditor(QWidget* parent, 
											const QStyleOptionViewItem& option, 
											const QModelIndex& index ) const {
	// if clock name or clock source
	if (index.column() == 0 || index.column() == 1) {

		QLineEdit* editor = new QLineEdit(parent);
		connect(editor, SIGNAL(editingFinished()),
			this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
		return editor;
	}

	// if clock period, pulse offset or pulse duration
	else if (index.column() == 2 ||
		index.column() == 4 || index.column() == 7) {

			QDoubleSpinBox* editor = new QDoubleSpinBox(parent);
			editor->setMinimum(0);
			editor->setSingleStep(0.1);
			connect(editor, SIGNAL(editingFinished()),
				this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
			return editor;
	}

	// if pulse value
	else if (index.column() == 6) {

		QSpinBox* editor = new QSpinBox(parent);
		editor->setMinimum(0);
		editor->setMaximum(1);
		editor->setSingleStep(1);

		connect(editor, SIGNAL(editingFinished()),
			this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
		return editor;
	}

	// if time unit
	else if (index.column() == 3 || index.column() == 5 || index.column() == 8) {

		QComboBox* editor = new QComboBox(parent);
		editor->addItem(QString("ns"));
		editor->addItem(QString("ps"));
		connect(editor, SIGNAL(currentIndexChanged(int)),
			this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
		return editor;
	}
	else {
		return QStyledItemDelegate::createEditor(parent, option, index);
	}
}

void ClockDriversDelegate::setEditorData( QWidget* editor, 
										 const QModelIndex& index ) const {

	// if clock name or clock source
	if (index.column() == 0 || index.column() == 1) {

		QLineEdit* lineEditor = qobject_cast<QLineEdit*>(editor);
		QString text = index.model()->data(index, Qt::DisplayRole).toString();
		lineEditor->setText(text);
	}

	// if clock period, pulse offset or pulse duration
	else if (index.column() == 2 ||
		index.column() == 4 || index.column() == 7) {

			QDoubleSpinBox* spinEditor = qobject_cast<QDoubleSpinBox*>(editor);
			double value = index.model()->data(index, Qt::DisplayRole).toDouble();
			spinEditor->setValue(value);
	}

	// if pulse value
	else if (index.column() == 6) {

		QSpinBox* spinEditor = qobject_cast<QSpinBox*>(editor);
		int value = index.model()->data(index, Qt::DisplayRole).toInt();
		spinEditor->setValue(value);
	}

	// if time unit
	else if (index.column() == 3 || index.column() == 5 || index.column() == 8) {

		QComboBox* comboEditor = qobject_cast<QComboBox*>(editor);
		QString value = index.model()->data(index, Qt::DisplayRole).toString();
		int index = comboEditor->findText(value);
		comboEditor->setCurrentIndex(index);
	}
	else {
		QStyledItemDelegate::setEditorData(editor, index);
	}
}

void ClockDriversDelegate::setModelData( QWidget* editor, 
										QAbstractItemModel* model, 
										const QModelIndex& index ) const {

	// if clock name or clock source
	if (index.column() == 0 || index.column() == 1) {

		QLineEdit* lineEditor = qobject_cast<QLineEdit*>(editor);
		QString text = lineEditor->text();
		model->setData(index, text, Qt::EditRole);
	}

	// if clock period, pulse offset or pulse duration
	else if (index.column() == 2 ||
		index.column() == 4 || index.column() == 7) {

			QDoubleSpinBox* spinEditor = qobject_cast<QDoubleSpinBox*>(editor);
			double value = spinEditor->value();
			model->setData(index, value, Qt::EditRole);
	}

	// if pulse value
	else if (index.column() == 6) {

		QSpinBox* spinEditor = qobject_cast<QSpinBox*>(editor);
		int value = spinEditor->value();
		model->setData(index, value, Qt::EditRole);
	}

	// if time unit
	else if (index.column() == 3 || index.column() == 5 || index.column() == 8) {

		QComboBox* comboEditor = qobject_cast<QComboBox*>(editor);
		QString value = comboEditor->currentText();
		model->setData(index, value, Qt::EditRole);
	}
	else {
		QStyledItemDelegate::setModelData(editor, model, index);
	}
}

void ClockDriversDelegate::commitAndCloseEditor() {

	QSpinBox* spinBox = qobject_cast<QSpinBox*>(sender());
	QDoubleSpinBox* doubleSpin = qobject_cast<QDoubleSpinBox*>(sender());
	QComboBox* comboBox = qobject_cast<QComboBox*>(sender());
	QLineEdit* lineEdit = qobject_cast<QLineEdit*>(sender());
	QWidget* widget = qobject_cast<QWidget*>(sender());

	if (comboBox) {
		emit commitData(comboBox);
		emit closeEditor(comboBox);
	}
	else if (doubleSpin) {
		emit commitData(doubleSpin);
		emit closeEditor(doubleSpin);
	}
	else if (spinBox) {
		emit commitData(spinBox);
		emit closeEditor(spinBox);
	}
	else if (lineEdit) {
		emit commitData(lineEdit);
		emit closeEditor(lineEdit);
	}
	else if (widget) {
		emit commitData(widget);
		emit closeEditor(widget);
	}
}
