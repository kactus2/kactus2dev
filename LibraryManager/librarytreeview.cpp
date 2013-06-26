/* 
 *
 *  Created on: 20.12.2010
 *      Author: Antti Kamppi
 */

#include "librarytreeview.h"
#include "libraryitem.h"
#include "libraryinterface.h"
#include "vlnv.h"

#include <models/librarycomponent.h>
#include <models/component.h>
#include <models/busdefinition.h>

#include <common/KactusAttribute.h>

#include <QMenu>
#include <QApplication>
#include <QHeaderView>
#include <QMessageBox>
#include <QUrl>
#include <QDesktopServices>
#include <QMimeData>
#include <QDrag>

#include <QDebug>

LibraryTreeView::LibraryTreeView(LibraryInterface* handler, 
								 LibraryTreeFilter* filter, 
								 QWidget *parent)
    : QTreeView(parent), 
      handler_(handler),
      filter_(filter),
      startPos_(),
      dragIndex_(),
      openDesignAction_(NULL),
      openSWDesignAction_(NULL),
      openCompAction_(NULL),
      createNewDesignAction_(NULL),
      createNewSWDesignAction_(NULL),
      createNewSystemDesignAction_(NULL),
      deleteAction_(NULL), 
      exportAction_(NULL),  
      showErrorsAction_(NULL),
      openBusAction_(NULL),
      addSignalsAction_(NULL),
      openComDefAction_(NULL),
      openApiDefAction_(NULL),
      openSystemAction_(NULL),
      openXmlAction_(NULL),
      openContainingFolderAction_(NULL)
{

	Q_ASSERT_X(filter, "LibraryTreeView constructor",
		"Null filter pointer given");

    setIconSize(QSize(20, 20));

	// the view can be sorted
	setSortingEnabled(true);

	// user can select items in the view
	setSelectionBehavior(QAbstractItemView::SelectItems);

	// only one item can be selected at a time
	setSelectionMode(QAbstractItemView::SingleSelection);

	// Items can not be edited
	setEditTriggers(QAbstractItemView::NoEditTriggers);

	setupActions();

	// user can't expand/collapse items in a normal way because the 
	// event handler for mouse press events has been reimplemented
	setItemsExpandable(false);
}

LibraryTreeView::~LibraryTreeView() {

}

void LibraryTreeView::contextMenuEvent(QContextMenuEvent* event) {

	Q_ASSERT_X(event, "LibraryTreeView::contextMenuEvent",
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

	LibraryItem* item = static_cast<LibraryItem*>(sourceIndex.internalPointer());
	VLNV vlnv = item->getVLNV();

	// create a menu to show contextMenu actions
	QMenu menu(this);

	bool hierarchical = false;

	// if item can be identified as single item
	if (vlnv.isValid()) {

		// parse the model
		QSharedPointer<LibraryComponent const> libComp = handler_->getModelReadOnly(vlnv);
		if (!libComp) {
			emit errorMessage(tr("Item not found in the library"));
			return;
		}

        QMenu* menuNew = 0;

		// if component
		if (libComp->getVlnv()->getType() == VLNV::COMPONENT) {
			QSharedPointer<Component const> component = libComp.staticCast<Component const>();

			// depending on the type of the component
			switch (component->getComponentImplementation()) {
				case KactusAttribute::KTS_SYS: {
                    if (component->hasSystemViews())
                    {
					    menu.addAction(openSystemAction_);
                    }

                    menu.addAction(openCompAction_);
					break;
											   }
				case KactusAttribute::KTS_SW: {
					if (component->hasSWViews())
                        menu.addAction(openSWDesignAction_);

                    menu.addAction(openCompAction_);

                    menuNew = menu.addMenu(tr("Add"));
                    menuNew->addAction(createNewSWDesignAction_);
					break;
											  }
				default: {

					if (!component->getHierViews().isEmpty()) {
						menu.addAction(openDesignAction_);
						hierarchical = true;
					}

                    if (component->hasSWViews())
                    {
                        menu.addAction(openSWDesignAction_);
                    }

                    if (component->hasSystemViews())
                    {
                        menu.addAction(openSystemAction_);
                    }

					menu.addAction(openCompAction_);
                    menu.addSeparator();

                    menuNew = menu.addMenu(tr("Add"));
					menuNew->addAction(createNewDesignAction_);
                    menuNew->addAction(createNewSWDesignAction_);

                    // Add New System Design action only if the component contains hierarchical HW views.
                    if (!component->getHierViews().isEmpty())
                    {
                        menuNew->addAction(createNewSystemDesignAction_);
                    }

					break;
                         }
			}		
		}
		else if (libComp->getVlnv()->getType() == VLNV::BUSDEFINITION) {

			QSharedPointer<BusDefinition const> busDef = libComp.staticCast<BusDefinition const>();
			
            menu.addAction(openBusAction_);
            menuNew = menu.addMenu(tr("Add"));
            menuNew->addAction(addSignalsAction_);
		}
		else if (libComp->getVlnv()->getType() == VLNV::ABSTRACTIONDEFINITION) {
			menu.addAction(openBusAction_);
		}
        else if (libComp->getVlnv()->getType() == VLNV::COMDEFINITION) {
            menu.addAction(openComDefAction_);
        }
        else if (libComp->getVlnv()->getType() == VLNV::APIDEFINITION) {
            menu.addAction(openApiDefAction_);
        }

        // Insert the New menu to the popup menu if it was created.
        if (menuNew != 0)
        {
            menu.addSeparator();
            menu.addMenu(menuNew);
        }

        menu.addSeparator();
        
        if (!libComp->isValid())
        {
            menu.addAction(showErrorsAction_);
        }

        menu.addAction(openContainingFolderAction_);
		menu.addAction(openXmlAction_);
	}

	menu.addAction(exportAction_);
	menu.addAction(deleteAction_);

	menu.exec(event->globalPos());
}

void LibraryTreeView::setupActions() {

	openDesignAction_ = new QAction(tr("Open HW Design"), this);
	openDesignAction_->setStatusTip(tr("Open a HW design"));
	openDesignAction_->setToolTip(tr("Open a HW design"));
	connect(openDesignAction_, SIGNAL(triggered()),
		this, SLOT(onOpenDesign()), Qt::UniqueConnection);

    openSWDesignAction_ = new QAction(tr("Open SW Design"), this);
    openSWDesignAction_->setStatusTip(tr("Open a SW design"));
    openSWDesignAction_->setToolTip(tr("Open a SW design"));
    connect(openSWDesignAction_, SIGNAL(triggered()),
        this, SLOT(onOpenSWDesign()), Qt::UniqueConnection);

	openCompAction_ = new QAction(tr("Open Component"), this);
	openCompAction_->setStatusTip(tr("Open component editor"));
	openCompAction_->setToolTip(tr("Open component editor"));
	connect(openCompAction_, SIGNAL(triggered()),
		this, SLOT(onOpenComponent()), Qt::UniqueConnection);

	createNewDesignAction_ = new QAction(tr("New HW Design..."), this);
	createNewDesignAction_->setStatusTip(tr("Create new HW design"));
	createNewDesignAction_->setToolTip(tr("Create new HW design"));
	connect(createNewDesignAction_, SIGNAL(triggered()),
		this, SLOT(onCreateDesign()), Qt::UniqueConnection);

    createNewSWDesignAction_ = new QAction(tr("New SW Design..."), this);
    createNewSWDesignAction_->setStatusTip(tr("Create new SW design"));
    createNewSWDesignAction_->setToolTip(tr("Create new SW design"));
    connect(createNewSWDesignAction_, SIGNAL(triggered()),
            this, SLOT(onCreateSWDesign()), Qt::UniqueConnection);

    createNewSystemDesignAction_ = new QAction(tr("New System Design..."), this);
    createNewSystemDesignAction_->setStatusTip(tr("Create new system design"));
    createNewSystemDesignAction_->setToolTip(tr("Create new system design"));
    connect(createNewSystemDesignAction_, SIGNAL(triggered()),
        this, SLOT(onCreateSystemDesign()), Qt::UniqueConnection);

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

    showErrorsAction_ = new QAction(tr("Show Errors"), this);
    showErrorsAction_->setStatusTip(tr("Show all errors of the item"));
    showErrorsAction_->setToolTip(tr("Show all errors of the item"));
    connect(showErrorsAction_, SIGNAL(triggered()),
            this, SLOT(onShowErrors()), Qt::UniqueConnection);

	openBusAction_ = new QAction(tr("Open"), this);
	openBusAction_->setStatusTip(tr("Open"));
	openBusAction_->setToolTip(tr("Open"));
	connect(openBusAction_, SIGNAL(triggered()),
		this, SLOT(onOpenBus()), Qt::UniqueConnection);

	addSignalsAction_ = new QAction(tr("New Abstraction Definition..."), this);
    addSignalsAction_->setStatusTip(tr("Create new abstraction definition for the bus"));
    addSignalsAction_->setToolTip(tr("Create new abstraction definition for the bus"));
	connect(addSignalsAction_, SIGNAL(triggered()),
		this, SLOT(onAddSignals()), Qt::UniqueConnection);

    openComDefAction_ = new QAction(tr("Open"), this);
    openComDefAction_->setStatusTip(tr("Open"));
    openComDefAction_->setToolTip(tr("Open"));
    connect(openComDefAction_, SIGNAL(triggered()),
        this, SLOT(onOpenComDef()), Qt::UniqueConnection);

    openApiDefAction_ = new QAction(tr("Open"), this);
    openApiDefAction_->setStatusTip(tr("Open"));
    openApiDefAction_->setToolTip(tr("Open"));
    connect(openApiDefAction_, SIGNAL(triggered()),
        this, SLOT(onOpenApiDef()), Qt::UniqueConnection);

    openSystemAction_ = new QAction(tr("Open System Design"), this);
	openSystemAction_->setStatusTip(tr("Open system design for editing"));
	openSystemAction_->setToolTip(tr("Open system design for editing"));
	connect(openSystemAction_, SIGNAL(triggered()),
		this, SLOT(onOpenSystemDesign()), Qt::UniqueConnection);

	openXmlAction_ = new QAction(tr("Open XML File"), this);
	connect(openXmlAction_, SIGNAL(triggered()),
		this, SLOT(onOpenXml()), Qt::UniqueConnection);

    openContainingFolderAction_ = new QAction(tr("Open Containing Folder"), this);
    connect(openContainingFolderAction_, SIGNAL(triggered()),
            this, SLOT(onOpenContainingFolder()), Qt::UniqueConnection);
}

void LibraryTreeView::onDeleteAction() {	
	emit deleteItem(filter_->mapToSource(currentIndex()));
}

void LibraryTreeView::onExportAction() {
	emit exportItem(filter_->mapToSource(currentIndex()));
}

void LibraryTreeView::mouseDoubleClickEvent( QMouseEvent * event ) {
	
	Q_ASSERT_X(event, "LibraryTreeView::contextMenuEvent",
		"Invalid event pointer given as parameter");

	// accept the event so it is not passed forwards
	event->accept();

	QModelIndex current = currentIndex();

	// if nothing was chosen
	if (!current.isValid()) {
		return;
	}

	emit openComponent(filter_->mapToSource(current));

	// let the default handler process the event
	QTreeView::mouseDoubleClickEvent(event);
}

void LibraryTreeView::mousePressEvent( QMouseEvent *event ) {
	
	startPos_ = event->pos();
	QModelIndex index = indexAt(startPos_);

	bool expanded = false;
	if (index.isValid()) {
		expanded = isExpanded(index);
	}

	setCurrentIndex(index);

	if (event->button() == Qt::LeftButton) {
		dragIndex_ = filter_->mapToSource(index);

		// if the item is not yet expanded
		if (!expanded) {
			expand(index);
		}
		// if item was expanded then close it
		else {
			collapse(index);
		}
	}

	QTreeView::mousePressEvent(event);
}

void LibraryTreeView::mouseReleaseEvent( QMouseEvent* event ) {

	// find the clicked index
	QModelIndex index = indexAt(event->pos());
	if (index.isValid()) {

		// find the source index
		QModelIndex sourceIndex = filter_->mapToSource(index);
		LibraryItem* item = static_cast<LibraryItem*>(sourceIndex.internalPointer());

		// if item contains a single vlnv
		VLNV vlnv = item->getVLNV();
		if (vlnv.isValid()) {
			emit itemSelected(vlnv);
		}
	}

	QTreeView::mouseReleaseEvent(event);
}

void LibraryTreeView::mouseMoveEvent( QMouseEvent *event ) {
	// if left mouse button is pressed down
	if (event->buttons() & Qt::LeftButton) {

		// calculate the distance of the drag
		int distance = (event->pos() - startPos_).manhattanLength();

		// make sure the drag distance is large enough to start the drag
		if (distance >= QApplication::startDragDistance()) {

			LibraryItem* item = static_cast<LibraryItem*>(dragIndex_.internalPointer());

            if (item != 0)
            {
			    VLNV vlnv = item->getVLNV();
    			
			    // if vlnv is valid
			    if (vlnv.isValid()) {
				    QDrag *drag = new QDrag(this);
				    QMimeData *mimeData = new QMimeData;

				    QVariant data = QVariant::fromValue(vlnv);

				    mimeData->setImageData(data);
				    drag->setMimeData(mimeData);
				    drag->exec(Qt::MoveAction | Qt::CopyAction | Qt::LinkAction);
			    }
            }
		}
	}
	QTreeView::mouseMoveEvent(event);
}

void LibraryTreeView::setCurrentIndex( const QModelIndex& index ) {

	// expand the tree to full length all the way from the root
	QModelIndex temp = index;

	// expand the whole tree up to the index
	while (temp.parent().isValid()) {
		temp = temp.parent();
		expand(temp);
	}

	// use the default implementation
	QTreeView::setCurrentIndex(index);
}

void LibraryTreeView::onOpenDesign() {
	emit openDesign(filter_->mapToSource(currentIndex()));
}

void LibraryTreeView::onOpenSWDesign() {
    emit openSWDesign(filter_->mapToSource(currentIndex()));
}

void LibraryTreeView::onOpenSystemDesign() {
    emit openSystemDesign(filter_->mapToSource(currentIndex()));
}

void LibraryTreeView::onOpenComponent() {
	emit openComponent(filter_->mapToSource(currentIndex()));
}

void LibraryTreeView::onCreateComponent() {
	emit createNewComponent(filter_->mapToSource(currentIndex()));
}

void LibraryTreeView::onCreateDesign() {
	emit createNewDesign(filter_->mapToSource(currentIndex()));
}

void LibraryTreeView::onCreateSWDesign() {
    emit createNewSWDesign(filter_->mapToSource(currentIndex()));
}

void LibraryTreeView::onCreateSystemDesign() {
    emit createNewSystemDesign(filter_->mapToSource(currentIndex()));
}

void LibraryTreeView::onOpenBus() {
	emit openBus(filter_->mapToSource(currentIndex()));
}

void LibraryTreeView::onCreateBus() {
	emit createBus(filter_->mapToSource(currentIndex()));
}

void LibraryTreeView::onAddSignals() {
	emit createAbsDef(filter_->mapToSource(currentIndex()));
}

void LibraryTreeView::onOpenComDef() {
    emit openComDef(filter_->mapToSource(currentIndex()));
}

void LibraryTreeView::onCreateComDef() {
    emit createComDef(filter_->mapToSource(currentIndex()));
}

void LibraryTreeView::onOpenApiDef() {
    emit openApiDef(filter_->mapToSource(currentIndex()));
}

void LibraryTreeView::onCreateApiDef() {
    emit createApiDef(filter_->mapToSource(currentIndex()));
}

//-----------------------------------------------------------------------------
// Function: LibraryTreeView::onShowErrors()
//-----------------------------------------------------------------------------
void LibraryTreeView::onShowErrors()
{
    emit showErrors(filter_->mapToSource(currentIndex()));
}

void LibraryTreeView::onOpenXml() {
	QModelIndex index = filter_->mapToSource(currentIndex());
	LibraryItem* item = static_cast<LibraryItem*>(index.internalPointer());
	VLNV vlnv = item->getVLNV();

	if (vlnv.isValid()) {
		QString xmlPath = handler_->getPath(vlnv);
		// open the file in operating system's default editor
		QDesktopServices::openUrl(QUrl::fromLocalFile(xmlPath));
	}
}

//-----------------------------------------------------------------------------
// Function: LibraryTreeView::onOpenContainingFolder()
//-----------------------------------------------------------------------------
void LibraryTreeView::onOpenContainingFolder()
{
    QModelIndex index = filter_->mapToSource(currentIndex());
    LibraryItem* item = static_cast<LibraryItem*>(index.internalPointer());
    VLNV vlnv = item->getVLNV();

    if (vlnv.isValid()) {
        QString path = QFileInfo(handler_->getPath(vlnv)).absolutePath();

        // Open the folder in the operating system's default file browser.
        QDesktopServices::openUrl(QUrl::fromLocalFile(path));
    }    
}