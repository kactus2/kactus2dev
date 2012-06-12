/* 
 *
 *  Created on: 5.4.2011
 *      Author: Antti Kamppi
 * 		filename: busifparameterstab.cpp
 */

#include "busifparameterstab.h"

#include <models/businterface.h>
#include <common/delegates/LineEditDelegate/lineeditdelegate.h>

#include <QVBoxLayout>

BusIfParametersTab::BusIfParametersTab(void* dataPointer, QWidget *parent): 
QWidget(parent), 
busif_(static_cast<BusInterface*>(dataPointer)),
view_(this), 
model_(static_cast<BusInterface*>(dataPointer)->getParameters(), this),
proxy_(NULL) {

	Q_ASSERT_X(dataPointer, "BusIfParametersTab constructor",
		"Null dataPointer given as parameter");

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

	view_.setItemDelegate(new LineEditDelegate(this));

	// set proxy to do the sorting automatically
	proxy_ = new QSortFilterProxyModel(this);

	// set source model for proxy
	proxy_->setSourceModel(&model_);
	// set proxy to be the source for the view
	view_.setModel(proxy_);

	// sort the view
	view_.sortByColumn(0, Qt::AscendingOrder);

	// create the layout, add widgets to it
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(&view_);
}

BusIfParametersTab::~BusIfParametersTab() {
}

bool BusIfParametersTab::isValid() const {
	return model_.isValid();
}

void BusIfParametersTab::refresh() {
	view_.update();
}

