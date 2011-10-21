/* 
 *
 *  Created on: 1.4.2011
 *      Author: Antti Kamppi
 * 		filename: portsdelegate.cpp
 */

#include "portsdelegate.h"

#include <common/widgets/booleanComboBox/booleancombobox.h>

#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>

// the constants defining which column represents what kind of information
static const int nameColumn = 0;
static const int directionColumn = 1;
static const int widthColumn = 2;
static const int leftColumn = 3;
static const int rightColumn = 4;
static const int typeNameColumn = 5;
static const int typeDefColumn = 6;
static const int defaultColumn = 7;
static const int descriptionColumn = 8;

PortsDelegate::PortsDelegate(QObject *parent): QStyledItemDelegate(parent) {
}

PortsDelegate::~PortsDelegate() {
}

QWidget* PortsDelegate::createEditor( QWidget* parent, 
									 const QStyleOptionViewItem& option, 
									 const QModelIndex& index ) const {
	// if the column is the one specified for direction items 
	if (index.column() == directionColumn) {

		QComboBox* combo = new QComboBox(parent);
		combo->addItem(QString("in"));
		combo->addItem(QString("out"));
		combo->addItem(QString("inout"));
		combo->addItem(QString("phantom"));
		connect(combo, SIGNAL(currentIndexChanged(int)),
			this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
		return combo;
	}

	// if item is for left or right bounds or width
	else if (index.column() == leftColumn || index.column() == rightColumn ||
		index.column() == widthColumn) {
		QSpinBox* spinBox = new QSpinBox(parent);
		spinBox->setRange(0, 2048);
		spinBox->setSingleStep(1);
		connect(spinBox, SIGNAL(editingFinished()),
			this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
		return spinBox;
	}

	// if item is for default value
	else if (index.column() == defaultColumn) {
		
		QLineEdit* defaultEdit = new QLineEdit(parent);
		connect(defaultEdit, SIGNAL(editingFinished()),
			this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
		return defaultEdit;
	}

	// if column was for the name, type or description column
	else {

		// use line edit to edit all other columns
		QLineEdit* editor = new QLineEdit(parent);
		connect(editor, SIGNAL(editingFinished()),
			this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
		return editor;
	}
}

void PortsDelegate::setEditorData( QWidget* editor, 
								  const QModelIndex& index ) const {
	// if the column is the one specified for direction
	if (index.column() == directionColumn) {

		QString text = index.model()->data(index, Qt::DisplayRole).toString();
		QComboBox* combo = qobject_cast<QComboBox*>(editor);

		int comboIndex = combo->findText(text);
		combo->setCurrentIndex(comboIndex);
	}

	// if item is for left or right bounds or width
	else if (index.column() == leftColumn || index.column() == rightColumn ||
		index.column() == widthColumn) {

		QSpinBox* spinBox = qobject_cast<QSpinBox*>(editor);
		int value = index.model()->data(index, Qt::DisplayRole).toInt();
		spinBox->setValue(value);
	}

	// if item is for default value
	else if (index.column() == defaultColumn) {

		QLineEdit* defaultEdit = qobject_cast<QLineEdit*>(editor);
		QString value = index.model()->data(index, Qt::DisplayRole).toString();
		defaultEdit->setText(value);
	}

	// if column was for name, type or description column
	else {
		// use the line edit for other columns
		QString text = index.model()->data(index, Qt::DisplayRole).toString();
		QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editor);
		lineEdit->setText(text);
	}
}

void PortsDelegate::setModelData( QWidget* editor, 
								 QAbstractItemModel* model, 
								 const QModelIndex& index ) const {
	// if the column is the one specified for direction
	if (index.column() == directionColumn) {

		QComboBox* combo = qobject_cast<QComboBox*>(editor);
		QString text = combo->currentText();
		model->setData(index, text, Qt::EditRole);
	}

	// if item is for left or right bounds or width
	else if (index.column() == leftColumn || index.column() == rightColumn ||
		index.column() == widthColumn) {
		
		QSpinBox* spinBox = qobject_cast<QSpinBox*>(editor);
		int value = spinBox->value();
		model->setData(index, value, Qt::EditRole);
	}

	// if item is for default value
	else if (index.column() == defaultColumn) {

		QLineEdit* defaultEdit = qobject_cast<QLineEdit*>(editor);
		QString value = defaultEdit->text();
		model->setData(index, value, Qt::EditRole);
	}

	// if column was for name, type or description
	else {
		// get the text from line edit for other columns
		QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editor);
		QString text = lineEdit->text();
		model->setData(index, text, Qt::EditRole);
	}
}

void PortsDelegate::commitAndCloseEditor() {

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
	else {
		emit commitData(lineEdit);
		emit closeEditor(lineEdit);
	}
}
