/* 
 *  	Created on: 12.8.2011
 *      Author: Antti Kamppi
 * 		filename: configurableelementdelegate.cpp
 *		Project: Kactus 2
 */

#include "configurableelementdelegate.h"

#include <models/modelparameter.h>

#include <QLineEdit>
#include <QComboBox>
#include <QMap>
#include <QStringList>

ConfigurableElementDelegate::ConfigurableElementDelegate(QSharedPointer<Component> component,
														 QObject *parent):
QStyledItemDelegate(parent),
component_(component) {
}

ConfigurableElementDelegate::~ConfigurableElementDelegate() {
}

QWidget* ConfigurableElementDelegate::createEditor(QWidget* parent, 
												   const QStyleOptionViewItem& option,
												   const QModelIndex& index ) const {

	switch (index.column()) {

		// if editing name of configurable element
		case 0: {
			QComboBox* box = new QComboBox(parent);
			// set box to be editable
			box->setEditable(true);

			if (component_) {
				QMap<QString, QSharedPointer<ModelParameter> > params = component_->getModelParameters();
				QStringList paramNames = params.keys();
				box->addItems(paramNames);
			}
			return box;
				}

		// if editing value of configurable element
		case 1: {
			QLineEdit* line = new QLineEdit(parent);
			connect(line, SIGNAL(editingFinished()),
				this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
			return line;
				}
		default:
			return QStyledItemDelegate::createEditor(parent, option, index);
	}
}

void ConfigurableElementDelegate::setEditorData( QWidget* editor, 
												const QModelIndex& index ) const {
	
	switch (index.column()) {
		
		// if name of configurable element
		case 0: {

			QComboBox* box = qobject_cast<QComboBox*>(editor);
			Q_ASSERT_X(box, "ConfigurableElementDelegate::setEditorData",
				"Type conversion failed for QComboBox");

			QString text = index.model()->data(index, Qt::DisplayRole).toString();
			int index = box->findText(text);

			box->setCurrentIndex(index);
			return;
				}
		// if value of configurable element
		case 1: {
			QLineEdit* line = qobject_cast<QLineEdit*>(editor);
			Q_ASSERT_X(line, "ConfigurableElementDelegate::setEditorData",
				"Type conversion failed for QLineEdit");

			QString text = index.model()->data(index, Qt::DisplayRole).toString();
			line->setText(text);
			return;
				}
		default: {
			QStyledItemDelegate::setEditorData(editor, index);
			return;
				 }
	}
}

void ConfigurableElementDelegate::setModelData( QWidget* editor,
											   QAbstractItemModel* model,
											   const QModelIndex& index ) const {

	switch (index.column()) {
	
		// if name of the configurable element
		case 0: {
			QComboBox* box = qobject_cast<QComboBox*>(editor);
			Q_ASSERT_X(box, "ConfigurableElementDelegate::setEditorData",
				"Type conversion failed for QComboBox");

			QString text = box->currentText();
			model->setData(index, text, Qt::EditRole);
			return;
				}
		// if value of configurable element
		case 1: {
			QLineEdit* line = qobject_cast<QLineEdit*>(editor);
			Q_ASSERT_X(line, "ConfigurableElementDelegate::setEditorData",
				"Type conversion failed for QLineEdit");

			QString text = line->text();
			model->setData(index, text, Qt::EditRole);
			return;
				}
		default: {
			QStyledItemDelegate::setModelData(editor, model, index);
			return;
				 }
	}
}

void ConfigurableElementDelegate::setComponent( QSharedPointer<Component> component ) {
	component_ = component;
}

void ConfigurableElementDelegate::commitAndCloseEditor() {

	QLineEdit* lineEdit = qobject_cast<QLineEdit*>(sender());

	// if editor was line edit
	if (lineEdit) {
		emit commitData(lineEdit);
		emit closeEditor(lineEdit);
	}
}
