/* 
 *
 *  Created on: 5.4.2011
 *      Author: Antti Kamppi
 * 		filename: busifparameterstab.cpp
 */

#include "busifparameterstab.h"

#include <models/businterface.h>
#include <IPXactWrapper/ComponentEditor/parameters/parametersdelegate.h>

#include <QHeaderView>
#include <QHBoxLayout>
#include <QVBoxLayout>

BusIfParametersTab::BusIfParametersTab(void* dataPointer, QWidget *parent): 
QWidget(parent), busif_(static_cast<BusInterface*>(dataPointer)),
addRowButton_(QIcon(":/icons/graphics/add.png"), QString(), this),
removeRowButton_(QIcon(":/icons/graphics/remove.png"), QString(), this),
view_(this), 
model_(&static_cast<BusInterface*>(dataPointer)->getParameters(), this){

	Q_ASSERT_X(dataPointer, "BusIfParametersTab constructor",
		"Null dataPointer given as parameter");

	connect(&addRowButton_, SIGNAL(clicked(bool)),
		&model_, SLOT(onAddRow()), Qt::UniqueConnection);
	connect(&removeRowButton_, SIGNAL(clicked(bool)),
		this, SLOT(onRemove()), Qt::UniqueConnection);

	connect(&model_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&model_, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&model_, SIGNAL(errorMessage(const QString&)),
		this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
	connect(&model_, SIGNAL(noticeMessage(const QString&)),
		this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

	// set view to be sortable
	view_.setSortingEnabled(true);
	view_.horizontalHeader()->setStretchLastSection(true);
	view_.horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
	view_.setSelectionMode(QAbstractItemView::SingleSelection);
	view_.setAlternatingRowColors(true);
	//view_.setSelectionBehavior(QAbstractItemView::SelectRows);
	view_.verticalHeader()->hide();
	view_.setEditTriggers(QAbstractItemView::AllEditTriggers);
	view_.setWordWrap(true);

	view_.setItemDelegate(new ParametersDelegate(this));

	// set proxy to do the sorting automatically
	proxy_ = new QSortFilterProxyModel(this);

	// set source model for proxy
	proxy_->setSourceModel(&model_);
	// set proxy to be the source for the view
	view_.setModel(proxy_);

	// sort the view
	view_.sortByColumn(0, Qt::AscendingOrder);

	QHBoxLayout* buttonLayout = new QHBoxLayout();
	buttonLayout->addWidget(&addRowButton_, 0, Qt::AlignLeft);
	buttonLayout->addWidget(&removeRowButton_, 0, Qt::AlignLeft);
	buttonLayout->addStretch();

	// create the layout, add widgets to it
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(&view_);
	layout->addLayout(buttonLayout);
}

BusIfParametersTab::~BusIfParametersTab() {
}

bool BusIfParametersTab::isValid() const {
	return model_.isValid();
}

void BusIfParametersTab::restoreChanges() {
	model_.restore();
}

void BusIfParametersTab::applyChanges() {
	model_.apply();
}

void BusIfParametersTab::onRemove() {
	QModelIndex index = proxy_->mapToSource(view_.currentIndex());

	// if index is valid then remove the row
	if (index.isValid())
		model_.onRemoveRow(index.row());
}
