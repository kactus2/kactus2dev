/* 
 *  	Created on: 21.2.2012
 *      Author: Antti Kamppi
 * 		filename: addressspaceparameterseditor.cpp
 *		Project: Kactus 2
 */

#include "addressspaceparameterseditor.h"

#include <IPXactWrapper/ComponentEditor/parameters/parametersdelegate.h>

#include <QVBoxLayout>

AddressSpaceParametersEditor::AddressSpaceParametersEditor(AddressSpace* addrSpace,
														   QWidget *parent):
QGroupBox(tr("Parameters"), parent),
addrSpace_(addrSpace),
view_(this),
model_(&(addrSpace->getParameters()), this),
proxy_(this) {

	connect(&model_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&model_, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&model_, SIGNAL(errorMessage(const QString&)),
		this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
	connect(&model_, SIGNAL(noticeMessage(const QString&)),
		this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(addItem(const QModelIndex&)),
		&model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(&view_, SIGNAL(removeItem(const QModelIndex&)),
		&model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);

	// set view to be sortable
	view_.setSortingEnabled(true);

	// items can not be dragged
	view_.setItemsDraggable(false);

	view_.setItemDelegate(new ParametersDelegate(this));

	// set source model for proxy
	proxy_.setSourceModel(&model_);
	// set proxy to be the source for the view
	view_.setModel(&proxy_);

	// sort the view
	view_.sortByColumn(0, Qt::AscendingOrder);

	// create the layout, add widgets to it
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(&view_);
}

AddressSpaceParametersEditor::~AddressSpaceParametersEditor() {
}

bool AddressSpaceParametersEditor::isValid() const {
	return model_.isValid();
}

void AddressSpaceParametersEditor::restoreChanges() {
	model_.restore();
}

void AddressSpaceParametersEditor::applyChanges() {
	model_.apply();
}
