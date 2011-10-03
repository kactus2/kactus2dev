/* 
 *
 *  Created on: 15.1.2011
 *      Author: Antti Kamppi
 */

#include "attributebox.h"

#include <common/dialogs/attributeDialog/attributedialog.h>

#include <QGridLayout>
#include <QPushButton>
#include <QHeaderView>
#include <QModelIndex>

AttributeBox::AttributeBox(QWidget* parent, const QString title):
 QGroupBox(title, parent), model_(this), view_(this) {

	// the top layout manager
	QGridLayout* topLayout = new QGridLayout(this);

	view_.setModel(&model_);

	// cells are resized to match contents and last column is stretched take the
	// available space in the widget
	view_.horizontalHeader()->setResizeMode(
			QHeaderView::ResizeToContents);
	view_.horizontalHeader()->setStretchLastSection(true);

	// vertical headers are not visible
	view_.verticalHeader()->hide();

	// words are wrapped in the cells to minimize space usage
	view_.setWordWrap(true);

	// user can only select one item at the time and the whole row is selected
	view_.setSelectionMode(QAbstractItemView::SingleSelection);
	view_.setSelectionBehavior(QAbstractItemView::SelectRows);
	topLayout->addWidget(&view_, 0, 0, 1, 2);

	// connect the signal that tells view if it should resize it's columns when
	// data in model changes
	connect(&model_, SIGNAL(attributeChanged(int)),
			&view_, SLOT(resizeColumnToContents(int)), Qt::UniqueConnection);
	connect(&model_, SIGNAL(attributeChanged(int)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);

	// button to add new attributes
	QPushButton* addButton = new QPushButton(tr("Add"), this);
	addButton->setToolTip(tr("Add a new attribute to the list"));
	connect(addButton, SIGNAL(clicked(bool)),
			this, SLOT(onAdd()), Qt::UniqueConnection);
	topLayout->addWidget(addButton, 1, 0, 1, 1);

	// button to remove attributes
	QPushButton* removeButton = new QPushButton(tr("Remove"), this);
	removeButton->setToolTip(tr("Remove selected attribute from the list"));
	connect(removeButton, SIGNAL(clicked(bool)),
			this, SLOT(onRemove()), Qt::UniqueConnection);
	topLayout->addWidget(removeButton, 1, 1, 1, 1);
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
