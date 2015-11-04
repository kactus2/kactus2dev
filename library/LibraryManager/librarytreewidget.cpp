//-----------------------------------------------------------------------------
// File: librarytreewidget.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 20.12.2010
//
// Description:
// LibraryTreeWidget is the widget that displays the library content in a tree.
//-----------------------------------------------------------------------------

#include "librarytreewidget.h"
#include "librarytreeview.h"
#include "librarytreemodel.h"

#include <QObject>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: LibraryTreeWidget::LibraryTreeWidget()
//-----------------------------------------------------------------------------
LibraryTreeWidget::LibraryTreeWidget(LibraryInterface* handler, LibraryTreeModel* dataModel, QWidget* parent):
QWidget(parent),
    filter_(new LibraryTreeFilter(handler, this)),
    view_(handler, filter_, this),
    dataModel_(dataModel)
{
    filter_->setSourceModel(dataModel);

	// set view to use LibraryTreeFilter as source model
	view_.setModel(filter_);

	// the layout to manage the visible items in the widget
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(&view_);
	layout->setContentsMargins(0, 0, 0, 0);

	setupConnections(dataModel);

	view_.sortByColumn(0, Qt::AscendingOrder);
}

//-----------------------------------------------------------------------------
// Function: LibraryTreeWidget::~LibraryTreeWidget()
//-----------------------------------------------------------------------------
LibraryTreeWidget::~LibraryTreeWidget()
{

}

//-----------------------------------------------------------------------------
// Function: LibraryTreeWidget::setupConnections()
//-----------------------------------------------------------------------------
void LibraryTreeWidget::setupConnections(LibraryTreeModel* dataModel)
{
	connect(&view_, SIGNAL(errorMessage(QString const&)),
        this, SIGNAL(errorMessage(QString const&)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(noticeMessage(QString const&)),
		this, SIGNAL(noticeMessage(QString const&)), Qt::UniqueConnection);

	// connect the view to the tree model
	connect(&view_, SIGNAL(openComponent(QModelIndex const&)),
		    dataModel, SLOT(onOpenDocument(QModelIndex const&)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(openDesign(QModelIndex const&)), 
		    dataModel, SLOT(onOpenDesign(QModelIndex const&)), Qt::UniqueConnection);

    connect(&view_, SIGNAL(openSWDesign(QModelIndex const&)), 
            dataModel, SLOT(onOpenSWDesign(QModelIndex const&)), Qt::UniqueConnection);

    connect(&view_, SIGNAL(openSystemDesign(QModelIndex const&)), 
        dataModel, SLOT(onOpenSystemDesign(QModelIndex const&)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(createNewComponent(QModelIndex const&)),
		    dataModel, SLOT(onCreateNewComponent(QModelIndex const&)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(createNewDesign(QModelIndex const&)),
		    dataModel, SLOT(onCreateNewDesign(QModelIndex const&)), Qt::UniqueConnection);

    connect(&view_, SIGNAL(createNewSWDesign(QModelIndex const&)),
            dataModel, SLOT(onCreateNewSWDesign(QModelIndex const&)), Qt::UniqueConnection);

    connect(&view_, SIGNAL(createNewSystemDesign(QModelIndex const&)),
            dataModel, SLOT(onCreateNewSystemDesign(QModelIndex const&)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(openBus(QModelIndex const&)),
		    dataModel, SLOT(onOpenDocument(QModelIndex const&)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(createBus(QModelIndex const&)),
		    dataModel, SLOT(onCreateBus(QModelIndex const&)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(createAbsDef(QModelIndex const&)),
		    dataModel, SLOT(onCreateAbsDef(QModelIndex const&)), Qt::UniqueConnection);

    connect(&view_, SIGNAL(openComDef(QModelIndex const&)),
            dataModel, SLOT(onOpenDocument(QModelIndex const&)), Qt::UniqueConnection);

    connect(&view_, SIGNAL(createComDef(QModelIndex const&)),
            dataModel, SLOT(onCreateComDef(QModelIndex const&)), Qt::UniqueConnection);

    connect(&view_, SIGNAL(openApiDef(QModelIndex const&)),
            dataModel, SLOT(onOpenDocument(QModelIndex const&)), Qt::UniqueConnection);

    connect(&view_, SIGNAL(createApiDef(QModelIndex const&)),
            dataModel, SLOT(onCreateApiDef(QModelIndex const&)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(deleteItem(QModelIndex const&)),
		    dataModel, SLOT(onDeleteItem(QModelIndex const&)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(exportItem(QModelIndex const&)),
		dataModel, SLOT(onExportItem(QModelIndex const&)), Qt::UniqueConnection);

    connect(&view_, SIGNAL(showErrors(QModelIndex const&)),
        dataModel, SLOT(onShowErrors(QModelIndex const&)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(itemSelected(const VLNV&)),
		this, SIGNAL(itemSelected(const VLNV&)), Qt::UniqueConnection);

	connect(dataModel, SIGNAL(invalidateFilter()), filter_, SLOT(invalidate()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: LibraryTreeWidget::selectItem()
//-----------------------------------------------------------------------------
void LibraryTreeWidget::selectItem(VLNV const& vlnv)
{	
	// if vlnv is not valid
	if (!vlnv.isValid())
    {
		view_.clearSelection();
	}
	
	// find the item to be selected
	LibraryItem* item = dataModel_->getRoot()->findItem(vlnv);

	// If item is not found then print an error message telling user that library is corrupted.
	if (!item)
    {
		emit errorMessage(tr("Selected item was not found, library is corrupted."));
		return;
	}

	// create an index to the item
	QModelIndex filteredIndex = filter_->mapFromSource(dataModel_->index(item));
	
	// tell view to select the item
	view_.setCurrentIndex(filteredIndex);
}

//-----------------------------------------------------------------------------
// Function: LibraryTreeWidget::getFilter()
//-----------------------------------------------------------------------------
LibraryFilter* LibraryTreeWidget::getFilter() const
{
    return filter_;
}
