/* 
 *
 *  Created on: 20.12.2010
 *      Author: Antti Kamppi
 */

#include "librarytreewidget.h"
#include "librarytreeview.h"
#include "librarytreemodel.h"
#include "libraryhandler.h"

#include <QObject>
#include <QVBoxLayout>
#include <QSizePolicy>

#include <QDebug>

LibraryTreeWidget::LibraryTreeWidget(VLNVDialer* dialer, 
									 LibraryInterface* handler,
									 LibraryTreeModel* dataModel,
									 QWidget* parent):
QWidget(parent),
view_(handler, &filter_, this),
filter_(handler, dialer, dataModel, this), 
dataModel_(dataModel) {

	// set view to use LibraryTreeFilter as source model
	view_.setModel(&filter_);

	// the layout to manage the visible items in the widget
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(&view_);
	layout->setContentsMargins(0, 0, 0, 0);

	setupConnections(dataModel);

	view_.sortByColumn(0, Qt::AscendingOrder);
}

LibraryTreeWidget::~LibraryTreeWidget() {

}

void LibraryTreeWidget::setupConnections(LibraryTreeModel* dataModel) {

	connect(&view_, SIGNAL(errorMessage(const QString&)),
		this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(noticeMessage(const QString&)),
		this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

	// connect the view to the tree model
	connect(&view_, SIGNAL(openComponent(const QModelIndex&)),
		    dataModel, SLOT(onOpenComponent(const QModelIndex&)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(openDesign(const QModelIndex&)), 
		    dataModel, SLOT(onOpenDesign(const QModelIndex&)), Qt::UniqueConnection);

    connect(&view_, SIGNAL(openSWDesign(const QModelIndex&)), 
            dataModel, SLOT(onOpenSWDesign(const QModelIndex&)), Qt::UniqueConnection);

    connect(&view_, SIGNAL(openSystemDesign(const QModelIndex&)), 
        dataModel, SLOT(onOpenSystemDesign(const QModelIndex&)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(createNewComponent(const QModelIndex&)),
		    dataModel, SLOT(onCreateNewComponent(const QModelIndex&)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(createNewDesign(const QModelIndex&)),
		    dataModel, SLOT(onCreateNewDesign(const QModelIndex&)), Qt::UniqueConnection);

    connect(&view_, SIGNAL(createNewSWDesign(const QModelIndex&)),
            dataModel, SLOT(onCreateNewSWDesign(const QModelIndex&)), Qt::UniqueConnection);

    connect(&view_, SIGNAL(createNewSystemDesign(const QModelIndex&)),
            dataModel, SLOT(onCreateNewSystemDesign(const QModelIndex&)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(openBus(const QModelIndex&)),
		    dataModel, SLOT(onOpenBus(const QModelIndex&)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(createBus(const QModelIndex&)),
		    dataModel, SLOT(onCreateBus(const QModelIndex&)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(createAbsDef(const QModelIndex&)),
		    dataModel, SLOT(onCreateAbsDef(const QModelIndex&)), Qt::UniqueConnection);

    connect(&view_, SIGNAL(openComDef(const QModelIndex&)),
            dataModel, SLOT(onOpenComDef(const QModelIndex&)), Qt::UniqueConnection);

    connect(&view_, SIGNAL(createComDef(const QModelIndex&)),
            dataModel, SLOT(onCreateComDef(const QModelIndex&)), Qt::UniqueConnection);

    connect(&view_, SIGNAL(openApiDef(const QModelIndex&)),
            dataModel, SLOT(onOpenApiDef(const QModelIndex&)), Qt::UniqueConnection);

    connect(&view_, SIGNAL(createApiDef(const QModelIndex&)),
            dataModel, SLOT(onCreateApiDef(const QModelIndex&)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(deleteItem(const QModelIndex&)),
		    dataModel, SLOT(onDeleteItem(const QModelIndex&)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(exportItem(const QModelIndex&)),
		dataModel, SLOT(onExportItem(const QModelIndex&)), Qt::UniqueConnection);

    connect(&view_, SIGNAL(showErrors(const QModelIndex&)),
        dataModel, SLOT(onShowErrors(const QModelIndex&)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(itemSelected(const VLNV&)),
		this, SIGNAL(itemSelected(const VLNV&)), Qt::UniqueConnection);

	connect(dataModel, SIGNAL(invalidateFilter()),
		&filter_, SLOT(invalidate()), Qt::UniqueConnection);
}

void LibraryTreeWidget::selectItem( const VLNV& vlnv ) {
	
	// if vlnv is not valid
	if (!vlnv.isValid()) {

		// do not select anything
		view_.clearSelection();
	}

	
	// find the item to be selected
	LibraryItem* item = dataModel_->getRoot()->findItem(vlnv);

	// if item is not found then print an error message telling user that 
	// library is corrupted
	if (!item) {
		emit errorMessage(tr("Selected item was not found, library is corrupted."));
		return;
	}

	// create an index to the item
	QModelIndex itemIndex = dataModel_->index(item);

	QModelIndex filteredIndex = filter_.mapFromSource(itemIndex);
	
	// tell view to select the item
	view_.setCurrentIndex(filteredIndex);
}
