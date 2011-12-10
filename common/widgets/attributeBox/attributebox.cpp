/* 
 *
 *  Created on: 15.1.2011
 *      Author: Antti Kamppi
 */

#include "attributebox.h"

#include <common/dialogs/attributeDialog/attributedialog.h>

#include <QVBoxLayout>
#include <QModelIndex>

AttributeBox::AttributeBox(QWidget* parent, const QString title):
QGroupBox(title, parent),
model_(this),
view_(this) {

	// the top layout manager
	QVBoxLayout* topLayout = new QVBoxLayout(this);

	view_.setModel(&model_);

	topLayout->addWidget(&view_);

	// connect the signal that tells view if it should resize it's columns when
	// data in model changes
	connect(&model_, SIGNAL(attributeChanged(int)),
			&view_, SLOT(resizeColumnToContents(int)), Qt::UniqueConnection);
	connect(&model_, SIGNAL(attributeChanged(int)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);

	// the signals from the view
	connect(&view_, SIGNAL(removeItem(const QModelIndex&)),
		&model_, SLOT(remove(const QModelIndex&)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(addItem(const QModelIndex&)),
		&model_, SLOT(addItem(const QModelIndex&)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(moveItem(const QModelIndex&, const QModelIndex&)),
		&model_, SLOT(moveItem(const QModelIndex&, const QModelIndex&)), Qt::UniqueConnection);

	// the signals from the model
	connect(&model_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
}

AttributeBox::~AttributeBox() {
}

const QMap<QString, QString> AttributeBox::getAttributes() const {
	return model_.getAttributes();
}

bool AttributeBox::isValid() const {
	return model_.validAttributes();
}

void AttributeBox::onAdd() {

	// create the dialog to input a new name-value-pair
	AttributeDialog dialog(this);

	// execute the dialog
	AttributeDialog::Attribute* attribute = dialog.exec();

	// if user did not input anything
	if (!attribute) {
		return;
	}

	// add the attribute to the model
	model_.addAttribute(attribute->name_, attribute->value_);

	// inform that contents has changed
	emit contentChanged();

	// delete the temporary attribute created by the dialog
	delete attribute;
}

void AttributeBox::onRemove() {
	model_.removeAttribute(view_.currentIndex());

	// inform that contents has changed
	emit contentChanged();
}

void AttributeBox::setAttributes(const QMap<QString, QString>& attributes) {
	model_.setAttributes(attributes);
}

void AttributeBox::clear() {
	model_.clear();
}

bool AttributeBox::isEmpty() const {
	return (model_.rowCount() == 0);
}
