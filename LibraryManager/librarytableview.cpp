/* 
 *
 *  Created on: 20.12.2010
 *      Author: Antti Kamppi
 */

#include "librarytableview.h"
#include "librarydata.h"
#include "libraryhandler.h"

#include <models/component.h>
#include <models/librarycomponent.h>
#include <models/busdefinition.h>

#include <QHeaderView>
#include <QMenu>
#include <QMessageBox>

#include <QDebug>

LibraryTableView::LibraryTableView(LibraryHandler* handler, 
								   LibrarySearchFilter* filter, 
								   QWidget *parent):
QTableView(parent),
filter_(filter),
handler_(handler),
openDesignAction_(NULL),
openCompAction_(NULL),
createNewComponentAction_(NULL),
createNewDesignAction_(NULL),
deleteAction_(NULL),
exportAction_(NULL),
hideAction_(NULL),
showAction_(NULL),
refreshAction_(NULL),
openBusDefAction_(NULL),
createBusDefAction_(NULL) {

	// the view can be sorted
	setSortingEnabled(true);

	// cells are resized to match contents and last column is stretched take the
	// available space in the widget
	horizontalHeader()->setResizeMode(
			QHeaderView::ResizeToContents);
	horizontalHeader()->setStretchLastSection(true);

	horizontalHeader()->setMinimumSectionSize(20);
	horizontalHeader()->setSortIndicatorShown(false);
	horizontalHeader()->setDefaultSectionSize(20);

	setColumnWidth(4, 20);
	setAlternatingRowColors(true);

	// vertical headers are not visible
	verticalHeader()->hide();

	// words are wrapped in the cells to minimize space usage
	setWordWrap(true);

	// user can only select one item at the time and the whole row is selected
	setSelectionMode(QAbstractItemView::SingleSelection);
	setSelectionBehavior(QAbstractItemView::SelectRows);

	// set up the context menu actions
	setupActions();
}

LibraryTableView::~LibraryTableView() {
}

void LibraryTableView::contextMenuEvent(QContextMenuEvent* event) {

	Q_ASSERT_X(event, "LibraryTableView::contextMenuEvent",
			"Invalid event pointer given as parameter");

	// accept the event so it is not passed forwards
	event->accept();

	QModelIndex current = currentIndex();

	// if nothing was chosen
	if (!current.isValid()) {
		return;
	}

	// get original model index so internalPointer can be used
	QModelIndex sourceIndex = filter_->mapToSource(current);

	VLNV* vlnv = static_cast<VLNV*>(sourceIndex.internalPointer());

	Q_ASSERT_X(vlnv, "LibraryTableView::contextMenuEvent",
		"Null item pointer");

	// create a menu to show contextMenu actions
	QMenu menu(this);

	bool hierarchical = false;

	// parse the model
	QSharedPointer<LibraryComponent> libComp = handler_->getModel(*vlnv);
	if (!libComp) {
		emit errorMessage(tr("Item not found in library"));
		return;
	}

	// if component
	if (libComp->getVlnv()->getType() == VLNV::COMPONENT) {
		QSharedPointer<Component> component = libComp.staticCast<Component>();

		if (component->isHierarchical()) {
			menu.addAction(openDesignAction_);
			hierarchical = true;
		}

		menu.addAction(openCompAction_);

	}
	else if (libComp->getVlnv()->getType() == VLNV::BUSDEFINITION) {

		menu.addAction(openBusDefAction_);
	}

	// if item is a component
	if (vlnv->getType() == VLNV::COMPONENT) {
		menu.addAction(createNewComponentAction_);

		// if component was not hierarchical then design can be created for it.
		if (!hierarchical)
			menu.addAction(createNewDesignAction_);
	}
	// if item is for bus definitions
	else if (vlnv->getType() == VLNV::BUSDEFINITION) {
		menu.addAction(createBusDefAction_);
	}

	menu.addAction(deleteAction_);
	menu.addAction(exportAction_);

	// if item is visible
	if (!vlnv->isHidden())
		menu.addAction(hideAction_);
	
	// if item is hidden
	else 
		menu.addAction(showAction_);

	menu.addAction(refreshAction_);

	menu.exec(event->globalPos());
}

void LibraryTableView::setupActions() {

	openDesignAction_ = new QAction(tr("Open Design"), this);
	openDesignAction_->setStatusTip(tr("Open a hierarchical design"));
	openDesignAction_->setToolTip(tr("Open a hierarchical design"));
	connect(openDesignAction_, SIGNAL(triggered()),
		this, SLOT(onOpenDesign()), Qt::UniqueConnection);

	openCompAction_ = new QAction(tr("Open Component"), this);
	openCompAction_->setStatusTip(tr("Open component editor"));
	openCompAction_->setToolTip(tr("Open component editor"));
	connect(openCompAction_, SIGNAL(triggered()),
		this, SLOT(onOpenComponent()), Qt::UniqueConnection);

	createNewComponentAction_ = new QAction(tr("Create New Component Here"), this);
	createNewComponentAction_->setStatusTip(tr("Create new component"));
	createNewComponentAction_->setToolTip(tr("Create new component"));
	connect(createNewComponentAction_, SIGNAL(triggered()),
		this, SLOT(onCreateComponent()), Qt::UniqueConnection);

	createNewDesignAction_ = new QAction(tr("Create New Design Here"), this);
	createNewDesignAction_->setStatusTip(tr("Create new design"));
	createNewDesignAction_->setToolTip(tr("Create new design"));
	connect(createNewDesignAction_, SIGNAL(triggered()),
		this, SLOT(onCreateDesign()), Qt::UniqueConnection);

	deleteAction_ = new QAction(tr("Delete Item"), this);
	deleteAction_->setStatusTip(tr("Delete item from the library"));
	deleteAction_->setToolTip(tr("Delete the item from the library"));
	connect(deleteAction_, SIGNAL(triggered()),
		this, SLOT(onDeleteAction()), Qt::UniqueConnection);

	exportAction_ = new QAction(tr("Export"), this);
	exportAction_->setStatusTip(tr("Export item and it's sub-items to another location"));
	exportAction_->setToolTip(tr("Export item and it's sub-items to another location"));
	connect(exportAction_, SIGNAL(triggered()),
		this, SLOT(onExportAction()), Qt::UniqueConnection);

	hideAction_ = new QAction(tr("Hide Item"), this);
	hideAction_->setStatusTip(tr("Set item as hidden"));
	hideAction_->setToolTip(tr("Set item as hidden"));
	connect(hideAction_, SIGNAL(triggered()),
		this, SLOT(onHideAction()), Qt::UniqueConnection);

	showAction_ = new QAction(tr("Show Item"), this);
	showAction_->setStatusTip(tr("Set item as unhidden"));
	showAction_->setToolTip(tr("Set item as unhidden"));
	connect(showAction_, SIGNAL(triggered()),
		this, SLOT(onShowAction()), Qt::UniqueConnection);

	refreshAction_ = new QAction(tr("Check Library Integrity"), this);
	refreshAction_->setStatusTip(tr("Refresh the library and check it's integrity"));
	refreshAction_->setToolTip(tr("Refresh the library and check it's integrity"));
	connect(refreshAction_, SIGNAL(triggered()),
		this, SIGNAL(refreshLibrary()), Qt::UniqueConnection);

	openBusDefAction_ = new QAction(tr("Open"), this);
	openBusDefAction_->setStatusTip(tr("Open"));
	openBusDefAction_->setToolTip(tr("Open"));
	connect(openBusDefAction_, SIGNAL(triggered()),
		this, SLOT(onOpenBusDef()), Qt::UniqueConnection);

	createBusDefAction_ = new QAction(tr("Create New"), this);
	createBusDefAction_->setStatusTip(tr("Create new"));
	createBusDefAction_->setToolTip(tr("Create new"));
	connect(createBusDefAction_, SIGNAL(triggered()),
		this, SLOT(onCreateBusDef()), Qt::UniqueConnection);
}

void LibraryTableView::onExportAction() {
	emit exportItem(filter_->mapToSource(currentIndex()));
}

void LibraryTableView::onDeleteAction() {

	// ask for confirmation for delete
	QMessageBox::StandardButton button = QMessageBox::question(this, 
		tr("Delete the selected item?"), 
		tr("Are you sure you want to delete the selected item?"),
		QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Cancel);

	// if user clicked "yes" to delete
	if (button == QMessageBox::Yes)
		emit deleteItem(filter_->mapToSource(currentIndex()));
}

void LibraryTableView::onHideAction() {
	emit hideItem(filter_->mapToSource(currentIndex()));
}

void LibraryTableView::onShowAction() {
	emit showItem(filter_->mapToSource(currentIndex()));
}

void LibraryTableView::onOpenDesign() {
	emit openDesign(filter_->mapToSource(currentIndex()));
}

void LibraryTableView::onOpenComponent() {
	emit openComponent(filter_->mapToSource(currentIndex()));
}

void LibraryTableView::onCreateComponent() {
	emit createNewComponent(filter_->mapToSource(currentIndex()));
}

void LibraryTableView::onCreateDesign() {
	emit createNewDesign(filter_->mapToSource(currentIndex()));
}

void LibraryTableView::onOpenBusDef() {
	emit openBusDef(filter_->mapToSource(currentIndex()));
}

void LibraryTableView::onCreateBusDef() {
	emit createBusDef(filter_->mapToSource(currentIndex()));
}
