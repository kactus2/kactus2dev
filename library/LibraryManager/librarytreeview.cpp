//-----------------------------------------------------------------------------
// File: librarytreeview.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 20.12.2010
//
// Description:
// The widget to display the library in a tree.
//-----------------------------------------------------------------------------

#include "librarytreeview.h"
#include "libraryitem.h"
#include "libraryinterface.h"

#include <IPXACTmodels/common/VLNV.h>
#include <IPXACTmodels/common/Document.h>

#include <IPXACTmodels/Component/Component.h>

#include <IPXACTmodels/kactusExtensions/KactusAttribute.h>

#include <QApplication>
#include <QDesktopServices>
#include <QDrag>
#include <QFileInfo>
#include <QHeaderView>
#include <QMenu>
#include <QMessageBox>
#include <QMimeData>
#include <QUrl>

//-----------------------------------------------------------------------------
// Function: LibraryTreeView::LibraryTreeView()
//-----------------------------------------------------------------------------
LibraryTreeView::LibraryTreeView(LibraryInterface* handler, LibraryTreeFilter* filter, QWidget* parent)
    : QTreeView(parent), 
      handler_(handler),
      filter_(filter),
      startPos_(),
      dragIndex_(),
      openDesignAction_(new QAction(tr("Open HW Design"), this)),
      openSWDesignAction_(new QAction(tr("Open SW Design"), this)),
      openComponentAction_(new QAction(tr("Open Component"), this)),
      createNewDesignAction_(new QAction(tr("New HW Design..."), this)),
      createNewSWDesignAction_(new QAction(tr("New SW Design..."), this)),
      createNewSystemDesignAction_(new QAction(tr("New System Design..."), this)),
      deleteAction_(new QAction(tr("Delete Item"), this)), 
      exportAction_(new QAction(tr("Export"), this)),  
      showErrorsAction_(new QAction(tr("Show Errors"), this)),
      openBusAction_(new QAction(tr("Open"), this)),
      addSignalsAction_(new QAction(tr("New Abstraction Definition..."), this)),
      openComDefAction_(new QAction(tr("Open"), this)),
      openApiDefAction_(new QAction(tr("Open"), this)),
      openSystemAction_(new QAction(tr("Open System Design"), this)),
      openXmlAction_(new QAction(tr("Open XML File"), this)),
      openContainingFolderAction_(new QAction(tr("Open Containing Folder"), this))
{

	Q_ASSERT_X(filter, "LibraryTreeView constructor", "Null filter pointer given");

    setIconSize(QSize(20, 20));

	setSortingEnabled(true);

	// user can select items in the view
	setSelectionBehavior(QAbstractItemView::SelectItems);
	setSelectionMode(QAbstractItemView::SingleSelection);

	// Items can not be edited.
	setEditTriggers(QAbstractItemView::NoEditTriggers);

	setupActions();

	// User can't expand/collapse items in a normal way because the event handler for mouse press events 
    // has been reimplemented.
	setItemsExpandable(false);
}

//-----------------------------------------------------------------------------
// Function: LibraryTreeView::~LibraryTreeView()
//-----------------------------------------------------------------------------
LibraryTreeView::~LibraryTreeView()
{

}

//-----------------------------------------------------------------------------
// Function: LibraryTreeView::contextMenuEvent()
//-----------------------------------------------------------------------------
void LibraryTreeView::contextMenuEvent(QContextMenuEvent* event)
{
	// accept the event so it is not passed forwards
	event->accept();

	QModelIndex current = currentIndex();

	// if nothing was chosen
	if (!current.isValid())
    {
		return;
	}

	// get original model index so internalPointer can be used
	QModelIndex sourceIndex = filter_->mapToSource(current);

	LibraryItem* item = static_cast<LibraryItem*>(sourceIndex.internalPointer());
	VLNV vlnv = item->getVLNV();

	// create a menu to show contextMenu actions
	QMenu menu(this);

	// if item can be identified as single item
	if (vlnv.isValid())
    {
		// parse the model
		QSharedPointer<Document const> document = handler_->getModelReadOnly(vlnv);
		if (!document)
        {
			emit errorMessage(tr("Item not found in the library"));
			return;
		}

        VLNV::IPXactType documentType = document->getVlnv().getType();

        QMenu* menuNew = 0;

        if (documentType == VLNV::COMPONENT)
        {
            QSharedPointer<Component const> component = document.staticCast<Component const>();

            // depending on the type of the component
            if (component->getImplementation() == KactusAttribute::SYSTEM)
            {
                if (component->hasSystemViews())
                {
                    menu.addAction(openSystemAction_);
                }

                menu.addAction(openComponentAction_);
            }
            else if (component->getImplementation() == KactusAttribute::SW)
            {
                if (component->hasSWViews())
                {
                    menu.addAction(openSWDesignAction_);
                }
                menu.addAction(openComponentAction_);

                menuNew = menu.addMenu(tr("Add"));
                menuNew->addAction(createNewSWDesignAction_);
            }
            else 
            {

                if (!component->getHierViews().isEmpty())
                {
                    menu.addAction(openDesignAction_);
                }

                if (component->hasSWViews())
                {
                    menu.addAction(openSWDesignAction_);
                }

                if (component->hasSystemViews())
                {
                    menu.addAction(openSystemAction_);
                }

                menu.addAction(openComponentAction_);
                menu.addSeparator();

                menuNew = menu.addMenu(tr("Add"));
                menuNew->addAction(createNewDesignAction_);
                menuNew->addAction(createNewSWDesignAction_);

                // Add New System Design action only if the component contains hierarchical HW views.
                if (!component->getHierViews().isEmpty())
                {
                    menuNew->addAction(createNewSystemDesignAction_);
                }
            }
        }

		else if (documentType == VLNV::BUSDEFINITION)
        {
            menu.addAction(openBusAction_);
            menuNew = menu.addMenu(tr("Add"));
            menuNew->addAction(addSignalsAction_);
		}

		else if (documentType == VLNV::ABSTRACTIONDEFINITION)
        {
			menu.addAction(openBusAction_);
		}

        else if (documentType == VLNV::COMDEFINITION)
        {
            menu.addAction(openComDefAction_);
        }

        else if (documentType == VLNV::APIDEFINITION)
        {
            menu.addAction(openApiDefAction_);
        }

        // Insert the New menu to the popup menu if it was created.
        if (menuNew != 0)
        {
            menu.addSeparator();
            menu.addMenu(menuNew);
        }

        menu.addSeparator();
        
         if (!handler_->isValid(vlnv))
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

//-----------------------------------------------------------------------------
// Function: LibraryTreeView::setupActions()
//-----------------------------------------------------------------------------
void LibraryTreeView::setupActions()
{
	openDesignAction_->setStatusTip(tr("Open a HW design"));
	openDesignAction_->setToolTip(tr("Open a HW design"));
	connect(openDesignAction_, SIGNAL(triggered()),	this, SLOT(onOpenDesign()), Qt::UniqueConnection);

    openSWDesignAction_->setStatusTip(tr("Open a SW design"));
    openSWDesignAction_->setToolTip(tr("Open a SW design"));
    connect(openSWDesignAction_, SIGNAL(triggered()), this, SLOT(onOpenSWDesign()), Qt::UniqueConnection);

	openComponentAction_->setStatusTip(tr("Open component editor"));
	openComponentAction_->setToolTip(tr("Open component editor"));
	connect(openComponentAction_, SIGNAL(triggered()), this, SLOT(onOpenComponent()), Qt::UniqueConnection);

	createNewDesignAction_->setStatusTip(tr("Create new HW design"));
	createNewDesignAction_->setToolTip(tr("Create new HW design"));
	connect(createNewDesignAction_, SIGNAL(triggered()), this, SLOT(onCreateDesign()), Qt::UniqueConnection);

    createNewSWDesignAction_->setStatusTip(tr("Create new SW design"));
    createNewSWDesignAction_->setToolTip(tr("Create new SW design"));
    connect(createNewSWDesignAction_, SIGNAL(triggered()), this, SLOT(onCreateSWDesign()), Qt::UniqueConnection);

    createNewSystemDesignAction_->setStatusTip(tr("Create new system design"));
    createNewSystemDesignAction_->setToolTip(tr("Create new system design"));
    connect(createNewSystemDesignAction_, SIGNAL(triggered()),
        this, SLOT(onCreateSystemDesign()), Qt::UniqueConnection);

	deleteAction_->setStatusTip(tr("Delete item from the library"));
	deleteAction_->setToolTip(tr("Delete the item from the library"));
	connect(deleteAction_, SIGNAL(triggered()),	this, SLOT(onDeleteAction()), Qt::UniqueConnection);

	exportAction_->setStatusTip(tr("Export item and it's sub-items to another location"));
	exportAction_->setToolTip(tr("Export item and it's sub-items to another location"));
	connect(exportAction_, SIGNAL(triggered()), this, SLOT(onExportAction()), Qt::UniqueConnection);

    showErrorsAction_->setStatusTip(tr("Show all errors of the item"));
    showErrorsAction_->setToolTip(tr("Show all errors of the item"));
    connect(showErrorsAction_, SIGNAL(triggered()), this, SLOT(onShowErrors()), Qt::UniqueConnection);

	openBusAction_->setStatusTip(tr("Open"));
	openBusAction_->setToolTip(tr("Open"));
	connect(openBusAction_, SIGNAL(triggered()), this, SLOT(onOpenBus()), Qt::UniqueConnection);

    addSignalsAction_->setStatusTip(tr("Create new abstraction definition for the bus"));
    addSignalsAction_->setToolTip(tr("Create new abstraction definition for the bus"));
	connect(addSignalsAction_, SIGNAL(triggered()),	this, SLOT(onAddSignals()), Qt::UniqueConnection);

    openComDefAction_->setStatusTip(tr("Open"));
    openComDefAction_->setToolTip(tr("Open"));
    connect(openComDefAction_, SIGNAL(triggered()), this, SLOT(onOpenComDef()), Qt::UniqueConnection);

    openApiDefAction_->setStatusTip(tr("Open"));
    openApiDefAction_->setToolTip(tr("Open"));
    connect(openApiDefAction_, SIGNAL(triggered()), this, SLOT(onOpenApiDef()), Qt::UniqueConnection);

	openSystemAction_->setStatusTip(tr("Open system design for editing"));
	openSystemAction_->setToolTip(tr("Open system design for editing"));
	connect(openSystemAction_, SIGNAL(triggered()),	this, SLOT(onOpenSystemDesign()), Qt::UniqueConnection);

	connect(openXmlAction_, SIGNAL(triggered()), this, SLOT(onOpenXml()), Qt::UniqueConnection);

    connect(openContainingFolderAction_, SIGNAL(triggered()),
        this, SLOT(onOpenContainingFolder()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: LibraryTreeView::onDeleteAction()
//-----------------------------------------------------------------------------
void LibraryTreeView::onDeleteAction()
{	
	emit deleteItem(filter_->mapToSource(currentIndex()));
}

//-----------------------------------------------------------------------------
// Function: LibraryTreeView::onExportAction()
//-----------------------------------------------------------------------------
void LibraryTreeView::onExportAction()
{
	emit exportItem(filter_->mapToSource(currentIndex()));
}

//-----------------------------------------------------------------------------
// Function: LibraryTreeView::mouseDoubleClickEvent()
//-----------------------------------------------------------------------------
void LibraryTreeView::mouseDoubleClickEvent(QMouseEvent* event)
{
	// accept the event so it is not passed forwards
	event->accept();

	QModelIndex current = currentIndex();
	if (!current.isValid())
    {
		return;
	}

	emit openComponent(filter_->mapToSource(current));

	// let the default handler process the event
	QTreeView::mouseDoubleClickEvent(event);
}

//-----------------------------------------------------------------------------
// Function: LibraryTreeView::mousePressEvent()
//-----------------------------------------------------------------------------
void LibraryTreeView::mousePressEvent(QMouseEvent* event)
{
	startPos_ = event->pos();
	QModelIndex index = indexAt(startPos_);

	setCurrentIndex(index);

	if (event->button() == Qt::LeftButton)
    {
		dragIndex_ = filter_->mapToSource(index);

		// if the item is not yet expanded
		if (index.isValid() && !isExpanded(index))
        {
			expand(index);
		}
		// if item was expanded then close it
		else
        {
			collapse(index);
		}
	}

	QTreeView::mousePressEvent(event);
}

//-----------------------------------------------------------------------------
// Function: LibraryTreeView::mouseReleaseEvent()
//-----------------------------------------------------------------------------
void LibraryTreeView::mouseReleaseEvent(QMouseEvent* event)
{
	// find the clicked index
	QModelIndex index = indexAt(event->pos());
	if (index.isValid())
    {
		QModelIndex sourceIndex = filter_->mapToSource(index);
		LibraryItem* item = static_cast<LibraryItem*>(sourceIndex.internalPointer());

		// if item contains a single vlnv
        VLNV vlnv = item->getVLNV();
		if (vlnv.isValid())
        {
			emit itemSelected(vlnv);
		}
	}

	QTreeView::mouseReleaseEvent(event);
}

//-----------------------------------------------------------------------------
// Function: LibraryTreeView::mouseMoveEvent()
//-----------------------------------------------------------------------------
void LibraryTreeView::mouseMoveEvent(QMouseEvent* event)
{
	// if left mouse button is pressed down
	if (event->buttons() & Qt::LeftButton)
    {
		int dragDistance = (event->pos() - startPos_).manhattanLength();

		// make sure the drag distance is large enough to start the drag.
		if (dragDistance >= QApplication::startDragDistance())
        {
			LibraryItem* item = static_cast<LibraryItem*>(dragIndex_.internalPointer());

            if (item != 0)
            {
			    VLNV vlnv = item->getVLNV();
    			
			    // if vlnv is valid
			    if (vlnv.isValid())
                {
				    QMimeData* mimeData = new QMimeData;
				    mimeData->setImageData(QVariant::fromValue(vlnv));

                    QDrag* drag = new QDrag(this);
				    drag->setMimeData(mimeData);
				    drag->exec(Qt::MoveAction | Qt::CopyAction | Qt::LinkAction);
			    }
            }
		}
	}
	QTreeView::mouseMoveEvent(event);
}

//-----------------------------------------------------------------------------
// Function: LibraryTreeView::setCurrentIndex()
//-----------------------------------------------------------------------------
void LibraryTreeView::setCurrentIndex(QModelIndex const& index)
{
	// Expand the tree to full length all the way from the root.
	QModelIndex temp = index;

	while (temp.parent().isValid())
    {
		temp = temp.parent();
		expand(temp);
	}

	QTreeView::setCurrentIndex(index);
}

//-----------------------------------------------------------------------------
// Function: LibraryTreeView::onOpenDesign()
//-----------------------------------------------------------------------------
void LibraryTreeView::onOpenDesign()
{
	emit openDesign(filter_->mapToSource(currentIndex()));
}

//-----------------------------------------------------------------------------
// Function: LibraryTreeView::onOpenSWDesign()
//-----------------------------------------------------------------------------
void LibraryTreeView::onOpenSWDesign()
{
    emit openSWDesign(filter_->mapToSource(currentIndex()));
}

//-----------------------------------------------------------------------------
// Function: LibraryTreeView::onOpenSystemDesign()
//-----------------------------------------------------------------------------
void LibraryTreeView::onOpenSystemDesign()
{
    emit openSystemDesign(filter_->mapToSource(currentIndex()));
}

//-----------------------------------------------------------------------------
// Function: LibraryTreeView::onOpenComponent()
//-----------------------------------------------------------------------------
void LibraryTreeView::onOpenComponent()
{
	emit openComponent(filter_->mapToSource(currentIndex()));
}

//-----------------------------------------------------------------------------
// Function: LibraryTreeView::onCreateComponent()
//-----------------------------------------------------------------------------
void LibraryTreeView::onCreateComponent()
{
	emit createNewComponent(filter_->mapToSource(currentIndex()));
}

//-----------------------------------------------------------------------------
// Function: LibraryTreeView::onCreateDesign()
//-----------------------------------------------------------------------------
void LibraryTreeView::onCreateDesign()
{
	emit createNewDesign(filter_->mapToSource(currentIndex()));
}

//-----------------------------------------------------------------------------
// Function: LibraryTreeView::onCreateSWDesign()
//-----------------------------------------------------------------------------
void LibraryTreeView::onCreateSWDesign()
{
    emit createNewSWDesign(filter_->mapToSource(currentIndex()));
}

//-----------------------------------------------------------------------------
// Function: LibraryTreeView::onCreateSystemDesign()
//-----------------------------------------------------------------------------
void LibraryTreeView::onCreateSystemDesign()
{
    emit createNewSystemDesign(filter_->mapToSource(currentIndex()));
}

//-----------------------------------------------------------------------------
// Function: LibraryTreeView::onCreateDesign()
//-----------------------------------------------------------------------------
void LibraryTreeView::onOpenBus()
{
	emit openBus(filter_->mapToSource(currentIndex()));
}

//-----------------------------------------------------------------------------
// Function: LibraryTreeView::onCreateBus()
//-----------------------------------------------------------------------------
void LibraryTreeView::onCreateBus()
{
	emit createBus(filter_->mapToSource(currentIndex()));
}

//-----------------------------------------------------------------------------
// Function: LibraryTreeView::onAddSignals()
//-----------------------------------------------------------------------------
void LibraryTreeView::onAddSignals()
{
	emit createAbsDef(filter_->mapToSource(currentIndex()));
}

//-----------------------------------------------------------------------------
// Function: LibraryTreeView::onOpenComDef()
//-----------------------------------------------------------------------------
void LibraryTreeView::onOpenComDef()
{
    emit openComDef(filter_->mapToSource(currentIndex()));
}

//-----------------------------------------------------------------------------
// Function: LibraryTreeView::onCreateComDef()
//-----------------------------------------------------------------------------
void LibraryTreeView::onCreateComDef()
{
    emit createComDef(filter_->mapToSource(currentIndex()));
}

//-----------------------------------------------------------------------------
// Function: LibraryTreeView::onOpenApiDef()
//-----------------------------------------------------------------------------
void LibraryTreeView::onOpenApiDef()
{
    emit openApiDef(filter_->mapToSource(currentIndex()));
}

//-----------------------------------------------------------------------------
// Function: LibraryTreeView::onCreateApiDef()
//-----------------------------------------------------------------------------
void LibraryTreeView::onCreateApiDef()
{
    emit createApiDef(filter_->mapToSource(currentIndex()));
}

//-----------------------------------------------------------------------------
// Function: LibraryTreeView::onShowErrors()
//-----------------------------------------------------------------------------
void LibraryTreeView::onShowErrors()
{
    emit showErrors(filter_->mapToSource(currentIndex()));
}

//-----------------------------------------------------------------------------
// Function: LibraryTreeView::onOpenXml()
//-----------------------------------------------------------------------------
void LibraryTreeView::onOpenXml()
{
	QModelIndex index = filter_->mapToSource(currentIndex());
	LibraryItem* item = static_cast<LibraryItem*>(index.internalPointer());
	
    VLNV vlnv = item->getVLNV();
	if (vlnv.isValid())
    {
		// open the file in operating system's default editor
		QDesktopServices::openUrl(QUrl::fromLocalFile(handler_->getPath(vlnv)));
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
    if (vlnv.isValid())
    {
        QString path = QFileInfo(handler_->getPath(vlnv)).absolutePath();

        // Open the folder in the operating system's default file browser.
        QDesktopServices::openUrl(QUrl::fromLocalFile(path));
    }    
}
