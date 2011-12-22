/* 
 *
 *  Created on: 29.3.2011
 *      Author: Antti Kamppi
 * 		filename: modelparametereditor.cpp
 */

#include "modelparametereditor.h"

#include "usagedelegate.h"
#include <models/component.h>

#include <QVBoxLayout>

ModelParameterEditor::ModelParameterEditor(QSharedPointer<Component> component,
										   void* dataPointer,
										   QWidget *parent): 
ItemEditor(component, parent), 
view_(this),
model_(dataPointer, this) {

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

	// set the delegate to edit the usage column
	view_.setItemDelegate(new UsageDelegate(2, this));

	// items can not be dragged
	view_.setItemsDraggable(false);

	// set proxy to do the sorting automatically
	proxy_ = new QSortFilterProxyModel(this);

	// set source model for proxy
	proxy_->setSourceModel(&model_);
	// set proxy to be the source for the view
	view_.setModel(proxy_);

	// sort the view
	view_.sortByColumn(0, Qt::AscendingOrder);;

	// create the layout, add widgets to it
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(&view_);

	model_.restore();
}

ModelParameterEditor::~ModelParameterEditor() {
}

bool ModelParameterEditor::isValid() const {
	return model_.isValid();
}

void ModelParameterEditor::makeChanges() {
	model_.apply();
}
