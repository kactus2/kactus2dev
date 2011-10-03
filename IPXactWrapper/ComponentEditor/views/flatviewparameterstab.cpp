/* 
 *
 *  Created on: 18.4.2011
 *      Author: Antti Kamppi
 * 		filename: flatviewparameterstab.cpp
 */

#include "flatviewparameterstab.h"

#include <IPXactWrapper/ComponentEditor/parameters/parametersdelegate.h>
#include <models/view.h>

#include <QHeaderView>
#include <QIcon>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QString>

FlatViewParametersTab::FlatViewParametersTab(View* view, QWidget *parent): 
QWidget(parent),
view_(view),
addRowButton_(QIcon(":/icons/graphics/add.png"), QString(), this),
removeRowButton_(QIcon(":/icons/graphics/remove.png"), QString(), this),
tableView_(this), model_(&(view->getParameters()), this),
proxy_(this) {

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
	tableView_.setSortingEnabled(true);
	tableView_.horizontalHeader()->setStretchLastSection(true);
	tableView_.horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
	tableView_.setSelectionMode(QAbstractItemView::SingleSelection);
	tableView_.setAlternatingRowColors(true);
	//view_.setSelectionBehavior(QAbstractItemView::SelectRows);
	tableView_.verticalHeader()->hide();
	tableView_.setEditTriggers(QAbstractItemView::AllEditTriggers);
	tableView_.setWordWrap(true);

	tableView_.setItemDelegate(new ParametersDelegate(this));

	// set source model for proxy
	proxy_.setSourceModel(&model_);
	// set proxy to be the source for the view
	tableView_.setModel(&proxy_);

	// sort the view
	tableView_.sortByColumn(0, Qt::AscendingOrder);

	QHBoxLayout* buttonLayout = new QHBoxLayout();
	buttonLayout->addWidget(&addRowButton_, 0, Qt::AlignLeft);
	buttonLayout->addWidget(&removeRowButton_, 0, Qt::AlignLeft);
	buttonLayout->addStretch();

	// create the layout, add widgets to it
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(&tableView_);
	layout->addLayout(buttonLayout);
}

FlatViewParametersTab::~FlatViewParametersTab() {
}

bool FlatViewParametersTab::isValid() const {
	return model_.isValid();
}

void FlatViewParametersTab::restoreChanges() {
	model_.restore();
}

void FlatViewParametersTab::applyChanges() {
	model_.apply();
}

void FlatViewParametersTab::onRemove() {
	QModelIndex index = proxy_.mapToSource(tableView_.currentIndex());

	// if index is valid then remove the row
	if (index.isValid())
		model_.onRemoveRow(index.row());
}
