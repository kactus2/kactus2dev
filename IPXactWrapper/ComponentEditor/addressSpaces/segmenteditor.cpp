/* 
 *  	Created on: 22.2.2012
 *      Author: Antti Kamppi
 * 		filename: segmenteditor.cpp
 *		Project: Kactus 2
 */

#include "segmenteditor.h"

#include <QVBoxLayout>

SegmentEditor::SegmentEditor( AddressSpace* addrSpace, QWidget *parent ):
QGroupBox(tr("Segments"), parent),
view_(this),
proxy_(this),
model_(addrSpace, this) {

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

	// set source model for proxy
	proxy_.setSourceModel(&model_);
	// set proxy to be the source for the view
	view_.setModel(&proxy_);

	// sort the view
	view_.sortByColumn(0, Qt::AscendingOrder);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(&view_);

	model_.restore();
}

SegmentEditor::~SegmentEditor() {
}

bool SegmentEditor::isValid() const {
	return model_.isValid();
}

void SegmentEditor::restore() {
	model_.restore();
}

void SegmentEditor::makeChanges() {
	model_.apply();
}
