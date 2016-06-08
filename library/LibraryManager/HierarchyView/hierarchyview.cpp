//-----------------------------------------------------------------------------
// File: hierarchyview.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 01.07.2011
//
// Description:
// HierarchyView displays the filtered contents from hierarchy filter.
//-----------------------------------------------------------------------------

#include "hierarchyview.h"

#include "hierarchyfilter.h"
#include "hierarchyitem.h"

#include <library/LibraryManager/libraryinterface.h>

#include <IPXACTmodels/BusDefinition/BusDefinition.h>
#include <IPXACTmodels/Component/Component.h>

#include <QApplication>
#include <QDesktopServices>
#include <QDrag>
#include <QFileInfo>
#include <QItemSelectionModel>
#include <QMenu>
#include <QMimeData>
#include <QStyledItemDelegate>
#include <QUrl>
#include <QVariant>

//-----------------------------------------------------------------------------
//! Custom item delegate to handle spacing of items.
//-----------------------------------------------------------------------------
class HierarchyItemDelegate : public QStyledItemDelegate
{
public:
    HierarchyItemDelegate(QObject* parent) : QStyledItemDelegate(parent) {}

    virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        return QStyledItemDelegate::sizeHint(option, index) + QSize(0, 10);
    }
};

//-----------------------------------------------------------------------------
// Function: HierarchyView::HierarchyView()
//-----------------------------------------------------------------------------
HierarchyView::HierarchyView(QWidget *parent, LibraryInterface* handler, HierarchyFilter* filter):
QTreeView(parent),
    handler_(handler),
    filter_(filter),
    startPos_(),
    dragIndex_(),
    openDesignAction_(new QAction(tr("Open HW Design"), this)),
    openMemoryDesignAction_(new QAction(tr("Open Memory Design"), this)),
    openSWDesignAction_(new QAction(tr("Open SW Design"), this)),
    openComponentAction_(new QAction(tr("Open Component"), this)),
    createNewComponentAction_(new QAction(tr("New HW Component"), this)),
    createNewDesignAction_(new QAction(tr("New HW Design..."), this)),
    createNewSWDesignAction_(new QAction(tr("New SW Design..."), this)),
    createNewSystemDesignAction_(new QAction(tr("New System Design..."), this)),
    exportAction_(new QAction(tr("Export"), this)),
    showErrorsAction_(new QAction(tr("Show Errors"), this)),
    openBusAction_(new QAction(tr("Open Bus"), this)),
    addSignalsAction_(new QAction(tr("New Abstraction Definition..."), this)),
    openComDefAction_(new QAction(tr("Open COM Definition"), this)),
    openApiDefAction_(new QAction(tr("Open API Definition"), this)),
    openSystemAction_(new QAction(tr("Open System Design"), this)),
    openXmlAction_(new QAction(tr("Open XML File"), this)),
    openContainingFolderAction_(new QAction(tr("Open Containing Folder"), this))
{
    setIconSize(QSize(24, 24));
    setItemDelegate(new HierarchyItemDelegate(this));

    // the view can be sorted
    setSortingEnabled(true);

	// user can select items in the view
	setSelectionBehavior(QAbstractItemView::SelectItems);

	// only one item can be selected at a time
	setSelectionMode(QAbstractItemView::SingleSelection);

	// Items can not be edited
	setEditTriggers(QAbstractItemView::NoEditTriggers);

	// user can't expand/collapse items in a normal way because the 
	// event handler for mouse press events has been reimplemented
	setItemsExpandable(false);

	setupActions();
}

//-----------------------------------------------------------------------------
// Function: HierarchyView::~HierarchyView()
//-----------------------------------------------------------------------------
HierarchyView::~HierarchyView()
{
}

//-----------------------------------------------------------------------------
// Function: HierarchyView::setupActions()
//-----------------------------------------------------------------------------
void HierarchyView::setupActions()
{
	openDesignAction_->setStatusTip(tr("Open a HW design"));
	openDesignAction_->setToolTip(tr("Open a HW design"));
	connect(openDesignAction_, SIGNAL(triggered()), this, SLOT(onOpenDesign()), Qt::UniqueConnection);

    openMemoryDesignAction_->setStatusTip(tr("Open a memory design"));
    openMemoryDesignAction_->setToolTip(tr("Open a memory design"));
    connect(openMemoryDesignAction_, SIGNAL(triggered()), this, SLOT(onOpenMemoryDesign()), Qt::UniqueConnection);

    openSWDesignAction_->setStatusTip(tr("Open a SW design"));
    openSWDesignAction_->setToolTip(tr("Open a SW design"));
    connect(openSWDesignAction_, SIGNAL(triggered()), this, SLOT(onOpenSWDesign()), Qt::UniqueConnection);

	openComponentAction_->setStatusTip(tr("Open component editor"));
	openComponentAction_->setToolTip(tr("Open component editor"));
	connect(openComponentAction_, SIGNAL(triggered()), this, SLOT(onOpenComponent()), Qt::UniqueConnection);

	createNewComponentAction_->setStatusTip(tr("Create new HW component"));
	createNewComponentAction_->setToolTip(tr("Create new HW component"));
	connect(createNewComponentAction_, SIGNAL(triggered()), this, SLOT(onCreateComponent()), Qt::UniqueConnection);

	createNewDesignAction_->setStatusTip(tr("Create new HW design"));
	createNewDesignAction_->setToolTip(tr("Create new HW design"));
	connect(createNewDesignAction_, SIGNAL(triggered()), this, SLOT(onCreateDesign()), Qt::UniqueConnection);

    createNewSWDesignAction_->setStatusTip(tr("Create new SW design"));
    createNewSWDesignAction_->setToolTip(tr("Create new SW design"));
    connect(createNewSWDesignAction_, SIGNAL(triggered()), this, SLOT(onCreateSWDesign()), Qt::UniqueConnection);

    createNewSystemDesignAction_->setStatusTip(tr("Create new System design"));
    createNewSystemDesignAction_->setToolTip(tr("Create new System design"));
    connect(createNewSystemDesignAction_, SIGNAL(triggered()),
        this, SLOT(onCreateSystemDesign()), Qt::UniqueConnection);

	exportAction_->setStatusTip(tr("Export item and it's sub-items to another location"));
	exportAction_->setToolTip(tr("Export item and it's sub-items to another location"));
	connect(exportAction_, SIGNAL(triggered()), this, SLOT(onExportAction()), Qt::UniqueConnection);

    showErrorsAction_->setStatusTip(tr("Show all errors of the item"));
    showErrorsAction_->setToolTip(tr("Show all errors of the item"));
    connect(showErrorsAction_, SIGNAL(triggered()), this, SLOT(onShowErrors()), Qt::UniqueConnection);

	openBusAction_->setStatusTip(tr("Open the bus in bus editor"));
	openBusAction_->setToolTip(tr("Open the bus in bus editor"));
	connect(openBusAction_, SIGNAL(triggered()), this, SLOT(onOpenBus()), Qt::UniqueConnection);

	addSignalsAction_->setStatusTip(tr("Create new abstraction definition for the bus"));
	addSignalsAction_->setToolTip(tr("Create new abstraction definition for the bus"));
	connect(addSignalsAction_, SIGNAL(triggered()), this, SLOT(onAddSignals()), Qt::UniqueConnection);

    openComDefAction_->setStatusTip(tr("Open the COM definition in an editor"));
    openComDefAction_->setToolTip(tr("Open the COM definition in an editor"));
    connect(openComDefAction_, SIGNAL(triggered()), this, SLOT(onOpenComDef()), Qt::UniqueConnection);

    openApiDefAction_->setStatusTip(tr("Open the API definition in an editor"));
    openApiDefAction_->setToolTip(tr("Open the API definition in an editor"));
    connect(openApiDefAction_, SIGNAL(triggered()), this, SLOT(onOpenApiDef()), Qt::UniqueConnection);

	openSystemAction_->setStatusTip(tr("Open system design for editing"));
	openSystemAction_->setToolTip(tr("Open system design for editing"));
	connect(openSystemAction_, SIGNAL(triggered()), this, SLOT(onOpenSystemDesign()), Qt::UniqueConnection);

	connect(openXmlAction_, SIGNAL(triggered()), this, SLOT(onOpenXml()), Qt::UniqueConnection);

    connect(openContainingFolderAction_, SIGNAL(triggered()),
        this, SLOT(onOpenContainingFolder()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: HierarchyView::onOpenComponent()
//-----------------------------------------------------------------------------
void HierarchyView::onOpenComponent()
{
	foreach (QModelIndex const& index, selectedIndexes())
    {
		emit openComponent(filter_->mapToSource(index));
	}
}

//-----------------------------------------------------------------------------
// Function: HierarchyView::onOpenBus()
//-----------------------------------------------------------------------------
void HierarchyView::onOpenBus()
{
	foreach (QModelIndex const& index, selectedIndexes())
    {
		emit openComponent(filter_->mapToSource(index));
	}
}

//-----------------------------------------------------------------------------
// Function: HierarchyView::onOpenComDef()
//-----------------------------------------------------------------------------
void HierarchyView::onOpenComDef()
{
    foreach (QModelIndex const& index, selectedIndexes())
    {
        emit openComponent(filter_->mapToSource(index));
    }
}

//-----------------------------------------------------------------------------
// Function: HierarchyView::onOpenApiDef()
//-----------------------------------------------------------------------------
void HierarchyView::onOpenApiDef()
{
    foreach (QModelIndex const& index, selectedIndexes())
    {
        emit openComponent(filter_->mapToSource(index));
    }
}

//-----------------------------------------------------------------------------
// Function: HierarchyView::onOpenDesign()
//-----------------------------------------------------------------------------
void HierarchyView::onOpenDesign()
{
	foreach (QModelIndex const& index, selectedIndexes())
    {
		emit openDesign(filter_->mapToSource(index));
	}
}

//-----------------------------------------------------------------------------
// Function: HierarchyView::onOpenMemoryDesign()
//-----------------------------------------------------------------------------
void HierarchyView::onOpenMemoryDesign()
{
    foreach (QModelIndex const& index, selectedIndexes())
    {
        emit openMemoryDesign(filter_->mapToSource(index));
    }
}

//-----------------------------------------------------------------------------
// Function: HierarchyView::onOpenSWDesign()
//-----------------------------------------------------------------------------
void HierarchyView::onOpenSWDesign()
{
    foreach (QModelIndex const& index, selectedIndexes())
    {
        emit openSWDesign(filter_->mapToSource(index));
    }
}

//-----------------------------------------------------------------------------
// Function: HierarchyView::onOpenSystemDesign()
//-----------------------------------------------------------------------------
void HierarchyView::onOpenSystemDesign()
{
    foreach (QModelIndex const& index, selectedIndexes())
    {
        emit openSystemDesign(filter_->mapToSource(index));
    }
}

//-----------------------------------------------------------------------------
// Function: HierarchyView::onCreateComponent()
//-----------------------------------------------------------------------------
void HierarchyView::onCreateComponent()
{
	emit createNewComponent(filter_->mapToSource(currentIndex()));
}

//-----------------------------------------------------------------------------
// Function: HierarchyView::onCreateDesign()
//-----------------------------------------------------------------------------
void HierarchyView::onCreateDesign()
{
	emit createNewDesign(filter_->mapToSource(currentIndex()));
}

//-----------------------------------------------------------------------------
// Function: HierarchyView::onCreateSWDesign()
//-----------------------------------------------------------------------------
void HierarchyView::onCreateSWDesign()
{
    emit createNewSWDesign(filter_->mapToSource(currentIndex()));
}

//-----------------------------------------------------------------------------
// Function: HierarchyView::onCreateSystemDesign()
//-----------------------------------------------------------------------------
void HierarchyView::onCreateSystemDesign()
{
    emit createNewSystemDesign(filter_->mapToSource(currentIndex()));
}

//-----------------------------------------------------------------------------
// Function: HierarchyView::onAddSignals()
//-----------------------------------------------------------------------------
void HierarchyView::onAddSignals()
{
	emit createNewAbsDef(filter_->mapToSource(currentIndex()));
}

//-----------------------------------------------------------------------------
// Function: HierarchyView::onCreateBus()
//-----------------------------------------------------------------------------
void HierarchyView::onCreateBus()
{
	emit createNewBus(filter_->mapToSource(currentIndex()));
}

//-----------------------------------------------------------------------------
// Function: HierarchyView::onCreateComDef()
//-----------------------------------------------------------------------------
void HierarchyView::onCreateComDef()
{
    emit createNewComDef(filter_->mapToSource(currentIndex()));
}

//-----------------------------------------------------------------------------
// Function: HierarchyView::onCreateApiDef()
//-----------------------------------------------------------------------------
void HierarchyView::onCreateApiDef()
{
    emit createNewApiDef(filter_->mapToSource(currentIndex()));
}

//-----------------------------------------------------------------------------
// Function: HierarchyView::onExportAction()
//-----------------------------------------------------------------------------
void HierarchyView::onExportAction()
{
	foreach (QModelIndex const& index, selectedIndexes())
    {
		emit exportItem(filter_->mapToSource(index));
	}
}

//-----------------------------------------------------------------------------
// Function: HierarchyView::onShowErrors()
//-----------------------------------------------------------------------------
void HierarchyView::onShowErrors()
{
    emit showErrors(filter_->mapToSource(currentIndex()));
}

//-----------------------------------------------------------------------------
// Function: HierarchyView::onOpenXml()
//-----------------------------------------------------------------------------
void HierarchyView::onOpenXml()
{
	QModelIndex const& index = filter_->mapToSource(currentIndex());
	HierarchyItem* item = static_cast<HierarchyItem*>(index.internalPointer());
	VLNV vlnv = item->getVLNV();

	QString xmlPath = handler_->getPath(vlnv);
	// open the file in operating system's default editor
	QDesktopServices::openUrl(QUrl::fromLocalFile(xmlPath));
}

//-----------------------------------------------------------------------------
// Function: HierarchyView::onOpenContainingFolder()
//-----------------------------------------------------------------------------
void HierarchyView::onOpenContainingFolder()
{
    QModelIndex index = filter_->mapToSource(currentIndex());
    HierarchyItem* item = static_cast<HierarchyItem*>(index.internalPointer());
    VLNV vlnv = item->getVLNV();

    QString path = QFileInfo(handler_->getPath(vlnv)).absolutePath();
    
    // Open the folder in the operating system's default file browser.
    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}

//-----------------------------------------------------------------------------
// Function: HierarchyView::contextMenuEvent()
//-----------------------------------------------------------------------------
void HierarchyView::contextMenuEvent(QContextMenuEvent* event)
{
	// accept the event so it is not passed forwards
	event->accept();

	QModelIndex current = currentIndex();
	QModelIndexList indexes = selectedIndexes();

	// if nothing was chosen
	if (!current.isValid())
    {
		return;
	}

	// get original model index so internalPointer can be used
	QModelIndex sourceIndex = filter_->mapToSource(current);

	HierarchyItem* item = static_cast<HierarchyItem*>(sourceIndex.internalPointer());
	VLNV vlnv = item->getVLNV();

	// create a menu to show contextMenu actions
	QMenu menu(this);

	// if item can be identified as single item
	if (vlnv.isValid())
    {
		// parse the model
		QSharedPointer<Document const> libComp = handler_->getModelReadOnly(vlnv);
        Q_ASSERT_X(libComp, "HierarchyView::contextMenuEvent()", "Object was not found in library.");

        QMenu* menuNew = 0;

        // if component
        if (item->type() == HierarchyItem::COMPONENT)
        {
            QSharedPointer<Component const> component = libComp.staticCast<Component const>();

            // depending on the type of the component
            if (component->getImplementation() == KactusAttribute::SYSTEM)
            {
                menu.addAction(openComponentAction_);
            }

            else if (component->getImplementation() == KactusAttribute::SW)
            {
                menu.addAction(openComponentAction_);

                menu.addSeparator();

                if (indexes.size() == 1)
                {
                    menuNew = menu.addMenu(tr("Add"));
                    menuNew->addAction(createNewSWDesignAction_);
                }
            }

            else
            {
                menu.addAction(openComponentAction_);
                menu.addSeparator();

                // Add create actions if only one object is selected.
                if (indexes.size() == 1)
                {
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
		}
		else if (item->type() == HierarchyItem::BUSDEFINITION)
        {
            menu.addAction(openBusAction_);
            menuNew = menu.addMenu(tr("Add"));
            menuNew->addAction(addSignalsAction_);
		}
		else if (item->type() == HierarchyItem::ABSDEFINITION)
        {
			menu.addAction(openBusAction_);
		}
        else if (item->type() == HierarchyItem::COMDEFINITION)
        {
            menu.addAction(openComDefAction_);
        }
        else if (item->type() == HierarchyItem::APIDEFINITION)
        {
            menu.addAction(openApiDefAction_);
        }
		else if (item->type() == HierarchyItem::HW_DESIGN)
        {
			menu.addAction(openDesignAction_);
		}
		else if (item->type() == HierarchyItem::SW_DESIGN)
        {
			menu.addAction(openSWDesignAction_);
		}
		else if (item->type() == HierarchyItem::SYS_DESIGN)
        {
			menu.addAction(openSystemAction_);
		}

        // Insert the New menu to the popup menu if it was created.
        if (menuNew != 0)
        {
            menu.addSeparator();
            menu.addMenu(menuNew);
        }

        menu.addSeparator();

        if (!item->isValid())
        {
            menu.addAction(showErrorsAction_);
        }

        menu.addAction(openContainingFolderAction_);
        menu.addAction(openXmlAction_);
    }

    menu.addAction(exportAction_);

	menu.exec(event->globalPos());
}

//-----------------------------------------------------------------------------
// Function: HierarchyView::mouseDoubleClickEvent()
//-----------------------------------------------------------------------------
void HierarchyView::mouseDoubleClickEvent(QMouseEvent* event)
{
	// accept the event so it is not passed forwards
	event->accept();

	QModelIndex origIndex = indexAt(event->pos());

	// get original model index so internalPointer can be used
	QModelIndex sourceIndex = filter_->mapToSource(origIndex);

	if (sourceIndex.isValid())
    {
        HierarchyItem* item = static_cast<HierarchyItem*>(sourceIndex.internalPointer());
        if (item->type() == HierarchyItem::HW_DESIGN)
        {
            emit openDesign(sourceIndex);
        }
        else if (item->type() == HierarchyItem::SW_DESIGN)
        {
            emit openSWDesign(sourceIndex);
        }
        else if (item->type() == HierarchyItem::SYS_DESIGN)
        {
            emit openSystemDesign(sourceIndex);
        }
        else
        {
            emit openComponent(sourceIndex);		
        }

		// select the object even if the instance column was clicked
		QModelIndex indexToSelect = model()->index(origIndex.row(), 0, origIndex.parent());
		setCurrentIndex(indexToSelect);
	}

	// let the default handler process the event
	QTreeView::mouseDoubleClickEvent(event);
}

//-----------------------------------------------------------------------------
// Function: HierarchyView::mousePressEvent()
//-----------------------------------------------------------------------------
void HierarchyView::mousePressEvent(QMouseEvent *event)
{
	startPos_ = event->pos();
	QModelIndex index = indexAt(startPos_);

	// check if the object is already expanded or not
	bool expanded = false;
	if (index.isValid())
    {
		expanded = isExpanded(index);
	}

	if (event->button() == Qt::LeftButton)
    {
		dragIndex_ = filter_->mapToSource(index);

		// left click expands/collapses items, right click shows context menu
		if (expanded)
        {
			collapse(index);
		}
		else
        {
			expand(index);
		}
	}

	QTreeView::mousePressEvent(event);
}

//-----------------------------------------------------------------------------
// Function: HierarchyView::mouseReleaseEvent()
//-----------------------------------------------------------------------------
void HierarchyView::mouseReleaseEvent(QMouseEvent* event)
{
	QModelIndex index = indexAt(event->pos());
	
	if (index.isValid() && index.column() == 0)
    {
		QModelIndex sourceIndex = filter_->mapToSource(index);
		HierarchyItem* item = static_cast<HierarchyItem*>(sourceIndex.internalPointer());
		emit componentSelected(item->getVLNV());
	}

	QTreeView::mouseReleaseEvent(event);
}

//-----------------------------------------------------------------------------
// Function: HierarchyView::mouseMoveEvent()
//-----------------------------------------------------------------------------
void HierarchyView::mouseMoveEvent(QMouseEvent* event)
{
	// if left mouse button is pressed down
	if (event->buttons() & Qt::LeftButton)
    {
		// calculate the distance of the drag
		int distance = (event->pos() - startPos_).manhattanLength();

		// make sure the drag distance is large enough to start the drag
		if (distance >= QApplication::startDragDistance())
        {
			// find the item that is being dragged
			HierarchyItem* item = static_cast<HierarchyItem*>(dragIndex_.internalPointer());
            if (item != 0)
            {
			    VLNV vlnv = item->getVLNV();

			    // if vlnv is valid
			    if (vlnv.isValid())
                {
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

//-----------------------------------------------------------------------------
// Function: HierarchyView::setCurrentIndex()
//-----------------------------------------------------------------------------
void HierarchyView::setCurrentIndex(QModelIndex const& index)
{
	QModelIndex temp = index;
	
	// expand the whole tree up to the index
	while (temp.parent().isValid())
    {
		temp = temp.parent();
		expand(temp);
	}

	// use the default implementation
	QTreeView::setCurrentIndex(index);
}

//-----------------------------------------------------------------------------
// Function: HierarchyView::setSelectedIndexes()
//-----------------------------------------------------------------------------
void HierarchyView::setSelectedIndexes(QModelIndexList const& indexes)
{
	// clear any previously selected items
	clearSelection();

	foreach (QModelIndex const& index, indexes)
    {		
        if (index.isValid())
        {
            selectionModel()->select(index, QItemSelectionModel::Select);

            QModelIndex temp = index;
            expand(temp);

            // expand the whole tree up to the index
            while (temp.parent().isValid())
            {
                temp = temp.parent();
                expand(temp);
            }
        }
	}
}
