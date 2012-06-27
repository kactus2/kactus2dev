/* 
 *
 *  Created on: 4.4.2011
 *      Author: Antti Kamppi
 * 		filename: otherclockdriverseditor.cpp
 */

#include "otherclockdriverseditor.h"
#include <common/widgets/summaryLabel/summarylabel.h>
#include "clockdriversdelegate.h"

#include <QVBoxLayout>

OtherClockDriversEditor::OtherClockDriversEditor(QSharedPointer<Component> component,
												 QWidget *parent): 
ItemEditor(component, parent),
view_(this), 
model_(component, this) {

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

	view_.setItemDelegate(new ClockDriversDelegate(this));

	// set proxy to do the sorting automatically
	proxy_ = new QSortFilterProxyModel(this);

	// set source model for proxy
	proxy_->setSourceModel(&model_);
	// set proxy to be the source for the view
	view_.setModel(proxy_);

	// sort the view
	view_.sortByColumn(0, Qt::AscendingOrder);

	// display a label on top the table
	SummaryLabel* summaryLabel = new SummaryLabel(tr("Other clock drivers"), this);

	// create the layout, add widgets to it
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(summaryLabel, 0, Qt::AlignCenter);
	layout->addWidget(&view_);
	layout->setContentsMargins(0, 0, 0, 0);

	refresh();
}

OtherClockDriversEditor::~OtherClockDriversEditor() {
}

bool OtherClockDriversEditor::isValid() const {
	return model_.isValid();
}

void OtherClockDriversEditor::makeChanges() {
	model_.apply();
}

void OtherClockDriversEditor::refresh() {
	model_.restore();
}
