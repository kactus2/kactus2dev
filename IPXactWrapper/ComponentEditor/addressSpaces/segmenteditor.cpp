/* 
 *  	Created on: 22.2.2012
 *      Author: Antti Kamppi
 * 		filename: segmenteditor.cpp
 *		Project: Kactus 2
 */

#include "segmenteditor.h"
#include <LibraryManager/libraryinterface.h>
#include <common/delegates/LineEditDelegate/lineeditdelegate.h>

#include <QVBoxLayout>

SegmentEditor::SegmentEditor(QSharedPointer<AddressSpace> addrSpace, 
	QSharedPointer<Component> component,
	LibraryInterface* handler,
							 QWidget *parent ):
QGroupBox(tr("Segments"), parent),
view_(this),
proxy_(this),
model_(addrSpace, this),
component_(component),
handler_(handler) {

	connect(&model_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&model_, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&model_, SIGNAL(errorMessage(const QString&)),
		this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
	connect(&model_, SIGNAL(noticeMessage(const QString&)),
		this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

	connect(&model_, SIGNAL(segmentAdded(QSharedPointer<Segment>)),
		this, SIGNAL(segmentAdded(QSharedPointer<Segment>)), Qt::UniqueConnection);
	connect(&model_, SIGNAL(segmentRemoved(const QString&)),
		this, SIGNAL(segmentRemoved(const QString&)), Qt::UniqueConnection);
	connect(&model_, SIGNAL(segmentRenamed(const QString&, const QString&)),
		this, SIGNAL(segmentRenamed(const QString&, const QString&)), Qt::UniqueConnection);
	connect(&model_, SIGNAL(segmentChanged(QSharedPointer<Segment>)),
		this, SIGNAL(segmentChanged(QSharedPointer<Segment>)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(addItem(const QModelIndex&)),
		&model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(&view_, SIGNAL(removeItem(const QModelIndex&)),
		&model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);

	const QString compPath = handler_->getDirectoryPath(*component_->getVlnv());
	QString defPath = QString("%1/segmentList.csv").arg(compPath);
	view_.setDefaultImportExportPath(defPath);
	view_.setAllowImportExport(true);

	// set view to be sortable
	view_.setSortingEnabled(true);

	// items can not be dragged
	view_.setItemsDraggable(false);

	view_.setItemDelegate(new LineEditDelegate(this));

	// set source model for proxy
	proxy_.setSourceModel(&model_);
	// set proxy to be the source for the view
	view_.setModel(&proxy_);

	// sort the view
	view_.sortByColumn(0, Qt::AscendingOrder);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(&view_);
}

SegmentEditor::~SegmentEditor() {
}

bool SegmentEditor::isValid() const {
	return model_.isValid();
}

void SegmentEditor::refresh() {
	view_.update();
	view_.sortByColumn(1, Qt::AscendingOrder);
}
