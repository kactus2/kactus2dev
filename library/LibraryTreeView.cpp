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

#include "LibraryTreeView.h"
#include "LibraryItem.h"
#include "LibraryInterface.h"

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
LibraryTreeView::LibraryTreeView(LibraryInterface* handler, LibraryTreeFilter* filter, QWidget* parent):
    ExpandingTreeView(parent),
    handler_(handler),
    filter_(filter),
    startPos_(),
    dragIndex_(),
    openHWDesignAction_(new QAction(tr("Open HW Design"), this)),
    openHWDesignMenu_(new QMenu(tr("Open HW Design"), this)),
    openSWDesignAction_(new QAction(tr("Open SW Design"), this)),
    openMemoryDesignAction_(new QAction(tr("Open Memory Design"), this)),
    openMemoryDesignMenu_(new QMenu(tr("Open Memory Design"), this)),
    openComponentAction_(new QAction(tr("Open Component"), this)),
    createNewDesignAction_(new QAction(tr("New HW Design..."), this)),
    createNewSWDesignAction_(new QAction(tr("New SW Design..."), this)),
    createNewSystemDesignAction_(new QAction(tr("New System Design..."), this)),
    deleteAction_(new QAction(tr("Delete Item..."), this)), 
    exportAction_(new QAction(tr("Export..."), this)),  
    showErrorsAction_(new QAction(tr("Show Errors..."), this)),
    openBusAction_(new QAction(tr("Open"), this)),
    openCatalogAction_(new QAction(tr("Open Catalog"), this)),
    addSignalsAction_(new QAction(tr("New Abstraction Definition..."), this)),
    openComDefAction_(new QAction(tr("Open"), this)),
    openApiDefAction_(new QAction(tr("Open"), this)),
    openSystemAction_(new QAction(tr("Open System Design"), this)),
    openXmlAction_(new QAction(tr("Open XML File"), this)),
    openContainingFolderAction_(new QAction(tr("Open Containing Folder"), this)),
    expandChilds_(new QAction(tr("Expand Branches"), this)),
    collapseChilds_(new QAction(tr("Collapse Branches"), this)),
    expandAllAction_(new QAction(tr("Expand All"), this)),
    collapseAllAction_(new QAction(tr("Collapse All"), this))
{
	Q_ASSERT_X(filter, "LibraryTreeView constructor", "Null filter pointer given");

    setIconSize(QSize(20, 20));

	setSortingEnabled(true);

	setSelectionBehavior(QAbstractItemView::SelectItems);
	setSelectionMode(QAbstractItemView::SingleSelection);

	// Items can not be edited.
	setEditTriggers(QAbstractItemView::NoEditTriggers);

	setupActions();
}

//-----------------------------------------------------------------------------
// Function: LibraryTreeView::contextMenuEvent()
//-----------------------------------------------------------------------------
void LibraryTreeView::contextMenuEvent(QContextMenuEvent* event)
{
	event->accept();

	QModelIndex current = currentIndex();
	if (!current.isValid())
    {
		return;
	}

    QMenu menu(this);
  
    // Get original model index so internalPointer can be used.
    QModelIndex sourceIndex = filter_->mapToSource(current);
	VLNV vlnv = static_cast<LibraryItem*>(sourceIndex.internalPointer())->getVLNV();

	if (vlnv.isValid())
    {
		QSharedPointer<Document const> document = handler_->getModelReadOnly(vlnv);

        if (document)
        {
            VLNV::IPXactType documentType = handler_->getDocumentType(vlnv);
            if (documentType == VLNV::COMPONENT)
            {
                QSharedPointer<Component const> component = document.staticCast<Component const>();

                menu.addAction(openComponentAction_);

                // depending on the type of the component
                if (component->getImplementation() == KactusAttribute::SYSTEM)
                {
                    if (component->hasSystemViews())
                    {
                        menu.addAction(openSystemAction_);
                    }
                }
                else
                {
                    QMenu* menuNew = menu.addMenu(tr("Add"));

                    if (component->getImplementation() == KactusAttribute::HW)
                    {
                        QStringList hierarchicalViewNames = component->getHierViews();
                        if (!hierarchicalViewNames.isEmpty())
                        {
                            openHWDesignMenu_->clear();
                            openMemoryDesignMenu_->clear();

                            for (QString const& viewName : hierarchicalViewNames)
                            {
                                openHWDesignMenu_->addAction(new QAction(viewName, openHWDesignMenu_));
                                openMemoryDesignMenu_->addAction(new QAction(viewName, openMemoryDesignMenu_));
                            }

                            if (hierarchicalViewNames.count() == 1)
                            {
                                menu.addAction(openHWDesignAction_);
                                connect(openHWDesignAction_, SIGNAL(triggered()),
                                    openHWDesignMenu_->actions().first(), SLOT(trigger()));

                                menu.addAction(openMemoryDesignAction_);
                                connect(openMemoryDesignAction_, SIGNAL(triggered()),
                                    openMemoryDesignMenu_->actions().first(), SLOT(trigger()));
                            }
                            else
                            {
                                menu.addMenu(openHWDesignMenu_);
                                menu.addMenu(openMemoryDesignMenu_);
                            }
                        }

                        if (component->hasSystemViews())
                        {
                            menu.addAction(openSystemAction_);
                        }

                        menuNew->addAction(createNewDesignAction_);

                        // Add New System Design action only if the component contains hierarchical HW views.
                        if (!component->getHierViews().isEmpty())
                        {
                            menuNew->addAction(createNewSystemDesignAction_);
                        }
                    }

                    for (QSharedPointer<View> view : *component->getViews())
                    {
                        VLNV reference = component->getModel()->getHierRef(view->name());

                        QSharedPointer<const Document> document = handler_->getModelReadOnly(reference);

                        if (document && document->getImplementation() == KactusAttribute::SW)
                        {
                            menu.addAction(openSWDesignAction_);
                            break;
                        }
                    }

                    menu.addSeparator();

                    menuNew->addAction(createNewSWDesignAction_);
                    menu.addMenu(menuNew);
                }
            }

            else if (documentType == VLNV::ABSTRACTIONDEFINITION)
            {
                menu.addAction(openBusAction_);
            }

            else if (documentType == VLNV::BUSDEFINITION)
            {
                menu.addAction(openBusAction_);
                menu.addSeparator();

                QMenu* menuNew = menu.addMenu(tr("Add"));
                menuNew->addAction(addSignalsAction_);
                menu.addMenu(menuNew);
            }

            else if (documentType == VLNV::CATALOG)
            {
                menu.addAction(openCatalogAction_);
            }

            else if (documentType == VLNV::COMDEFINITION)
            {
                menu.addAction(openComDefAction_);
            }

            else if (documentType == VLNV::APIDEFINITION)
            {
                menu.addAction(openApiDefAction_);
            }
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

    menu.addSeparator();
    if (model()->rowCount(current) > 0)
    {
        menu.addAction(expandChilds_);
        menu.addAction(collapseChilds_);
    }
    menu.addAction(expandAllAction_);
    menu.addAction(collapseAllAction_);

	menu.exec(event->globalPos());
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

    if (event->button() == Qt::LeftButton)
    {
        dragIndex_ = filter_->mapToSource(index);
    }

	setCurrentIndex(index);

    bool previouslyExpanded = isExpanded(index);
    QTreeView::mousePressEvent(event);

    // Change expand state if the click did not change it otherwise.
    if (event->button() == Qt::LeftButton && index.isValid() && (isExpanded(index) == previouslyExpanded))
    {
        setExpanded(index, !previouslyExpanded);
    }
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
void LibraryTreeView::onOpenDesign(QAction* viewAction)
{
	emit openDesign(filter_->mapToSource(currentIndex()), viewAction->text());
}

//-----------------------------------------------------------------------------
// Function: LibraryTreeView::onOpenSWDesign()
//-----------------------------------------------------------------------------
void LibraryTreeView::onOpenSWDesign()
{
    emit openSWDesign(filter_->mapToSource(currentIndex()));
}

//-----------------------------------------------------------------------------
// Function: librarytreeview::onOpenMemoryDesign()
//-----------------------------------------------------------------------------
void LibraryTreeView::onOpenMemoryDesign(QAction* viewAction)
{
    emit openMemoryDesign(filter_->mapToSource(currentIndex()), viewAction->text());
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
// Function: LibraryTreeView::onOpenBus()
//-----------------------------------------------------------------------------
void LibraryTreeView::onOpenBus()
{
	emit openBus(filter_->mapToSource(currentIndex()));
}

//-----------------------------------------------------------------------------
// Function: LibraryTreeView::onOpenCatalog()
//-----------------------------------------------------------------------------
void LibraryTreeView::onOpenCatalog()
{
    emit openCatalog(filter_->mapToSource(currentIndex()));
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
// Function: LibraryTreeView::onOpenApiDef()
//-----------------------------------------------------------------------------
void LibraryTreeView::onOpenApiDef()
{
    emit openApiDef(filter_->mapToSource(currentIndex()));
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

//-----------------------------------------------------------------------------
// Function: LibraryTreeView::onExpandBranches()
//-----------------------------------------------------------------------------
void LibraryTreeView::onExpandBranches()
{
    QModelIndex index = currentIndex();

    setExpanded(index, true);
    setChildrenExpandStates(index, true);
}

//-----------------------------------------------------------------------------
// Function: LibraryTreeView::onCollapseBranches()
//-----------------------------------------------------------------------------
void LibraryTreeView::onCollapseBranches()
{
    QModelIndex index = currentIndex();

    setChildrenExpandStates(index, false);
}

//-----------------------------------------------------------------------------
// Function: LibraryTreeView::setChildrenExpandStates()
//-----------------------------------------------------------------------------
void LibraryTreeView::setChildrenExpandStates(QModelIndex index, bool expanded)
{
    if (index.isValid())
    {
        int childCount = model()->rowCount(index);
        for (int i = 0; i < childCount; i++)
        {
            QModelIndex childIndex = model()->index(i, 0, index);
            setExpanded(childIndex, expanded);
            setChildrenExpandStates(childIndex, expanded);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryTreeView::setupActions()
//-----------------------------------------------------------------------------
void LibraryTreeView::setupActions()
{
    openHWDesignMenu_->setStatusTip(tr("Open a HW design"));
    openHWDesignMenu_->setToolTip(tr("Open a HW design"));
    connect(openHWDesignMenu_, SIGNAL(triggered(QAction*)),	this, SLOT(onOpenDesign(QAction*)), Qt::UniqueConnection);

    openSWDesignAction_->setStatusTip(tr("Open a SW design"));
    openSWDesignAction_->setToolTip(tr("Open a SW design"));
    connect(openSWDesignAction_, SIGNAL(triggered()), this, SLOT(onOpenSWDesign()), Qt::UniqueConnection);

    openMemoryDesignMenu_->setStatusTip(tr("Open a Memory design"));
    openMemoryDesignMenu_->setToolTip(tr("Open a Memory design"));
    connect(openMemoryDesignMenu_, SIGNAL(triggered(QAction*)),	this, SLOT(onOpenMemoryDesign(QAction*)),
        Qt::UniqueConnection);

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

    openCatalogAction_->setStatusTip(tr("Open a catalog"));
    openCatalogAction_->setToolTip(tr("Open a catalog"));
    connect(openCatalogAction_, SIGNAL(triggered()), this, SLOT(onOpenCatalog()), Qt::UniqueConnection);

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

    connect(expandChilds_, SIGNAL(triggered()), this, SLOT(onExpandBranches()), Qt::UniqueConnection);

    connect(collapseChilds_, SIGNAL(triggered()), this, SLOT(onCollapseBranches()), Qt::UniqueConnection);

    connect(expandAllAction_, SIGNAL(triggered()), this, SLOT(expandAll()), Qt::UniqueConnection);

    connect(collapseAllAction_, SIGNAL(triggered()), this, SLOT(collapseAll()), Qt::UniqueConnection);

}
