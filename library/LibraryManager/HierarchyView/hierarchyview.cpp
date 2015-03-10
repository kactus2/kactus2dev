/* 
 *  	Created on: 1.7.2011
 *      Author: Antti Kamppi
 * 		filename: hierarchyview.cpp
 *		Project: Kactus 2
 */

#include "hierarchyview.h"

#include "hierarchyfilter.h"
#include "hierarchyitem.h"

#include <library/LibraryManager/libraryinterface.h>
#include <IPXACTmodels/component.h>
#include <IPXACTmodels/busdefinition.h>

#include <QMenu>
#include <QApplication>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <QItemSelectionModel>
#include <QDrag>
#include <QMimeData>
#include <QVariant>
#include <QStyledItemDelegate>

#include <QDebug>

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

HierarchyView::HierarchyView(QWidget *parent, 
							 LibraryInterface* handler,
							 HierarchyFilter* filter)
    : QTreeView(parent),
      handler_(handler),
      filter_(filter),
      startPos_(),
      dragIndex_(),
      openDesignAction_(NULL),
      openMemoryDesignAction_(NULL),
      openSWDesignAction_(NULL),
      openCompAction_(NULL),
      createNewComponentAction_(NULL),
      createNewDesignAction_(NULL),
      createNewSWDesignAction_(NULL),
      createNewSystemDesignAction_(NULL),
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

HierarchyView::~HierarchyView() {
}

void HierarchyView::setupActions() {
	openDesignAction_ = new QAction(tr("Open HW Design"), this);
	openDesignAction_->setStatusTip(tr("Open a HW design"));
	openDesignAction_->setToolTip(tr("Open a HW design"));
	connect(openDesignAction_, SIGNAL(triggered()),
		    this, SLOT(onOpenDesign()), Qt::UniqueConnection);

    openMemoryDesignAction_ = new QAction(tr("Open Memory Design"), this);
    openMemoryDesignAction_->setStatusTip(tr("Open a memory design"));
    openMemoryDesignAction_->setToolTip(tr("Open a memory design"));
    connect(openMemoryDesignAction_, SIGNAL(triggered()),
        this, SLOT(onOpenMemoryDesign()), Qt::UniqueConnection);

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

	createNewComponentAction_ = new QAction(tr("New HW Component"), this);
	createNewComponentAction_->setStatusTip(tr("Create new HW component"));
	createNewComponentAction_->setToolTip(tr("Create new HW component"));
	connect(createNewComponentAction_, SIGNAL(triggered()),
		this, SLOT(onCreateComponent()), Qt::UniqueConnection);

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
    createNewSystemDesignAction_->setStatusTip(tr("Create new System design"));
    createNewSystemDesignAction_->setToolTip(tr("Create new System design"));
    connect(createNewSystemDesignAction_, SIGNAL(triggered()),
        this, SLOT(onCreateSystemDesign()), Qt::UniqueConnection);

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

	openBusAction_ = new QAction(tr("Open Bus"), this);
	openBusAction_->setStatusTip(tr("Open the bus in bus editor"));
	openBusAction_->setToolTip(tr("Open the bus in bus editor"));
	connect(openBusAction_, SIGNAL(triggered()),
		this, SLOT(onOpenBus()), Qt::UniqueConnection);

	addSignalsAction_ = new QAction(tr("New Abstraction Definition..."), this);
	addSignalsAction_->setStatusTip(tr("Create new abstraction definition for the bus"));
	addSignalsAction_->setToolTip(tr("Create new abstraction definition for the bus"));
	connect(addSignalsAction_, SIGNAL(triggered()),
		this, SLOT(onAddSignals()), Qt::UniqueConnection);

	openComDefAction_ = new QAction(tr("Open COM Definition"), this);
    openComDefAction_->setStatusTip(tr("Open the COM definition in an editor"));
    openComDefAction_->setToolTip(tr("Open the COM definition in an editor"));
    connect(openComDefAction_, SIGNAL(triggered()),
        this, SLOT(onOpenComDef()), Qt::UniqueConnection);

    openApiDefAction_ = new QAction(tr("Open API Definition"), this);
    openApiDefAction_->setStatusTip(tr("Open the API definition in an editor"));
    openApiDefAction_->setToolTip(tr("Open the API definition in an editor"));
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

void HierarchyView::onOpenComponent() {
	QModelIndexList indexes = selectedIndexes();
	foreach (QModelIndex index, indexes) {
		emit openComponent(filter_->mapToSource(index));
	}
}

void HierarchyView::onOpenBus() {
	QModelIndexList indexes = selectedIndexes();
	foreach (QModelIndex index, indexes) {
		emit openComponent(filter_->mapToSource(index));
	}
}

void HierarchyView::onOpenComDef() {
    QModelIndexList indexes = selectedIndexes();
    foreach (QModelIndex index, indexes) {
        emit openComponent(filter_->mapToSource(index));
    }
}

void HierarchyView::onOpenApiDef() {
    QModelIndexList indexes = selectedIndexes();
    foreach (QModelIndex index, indexes) {
        emit openComponent(filter_->mapToSource(index));
    }
}

void HierarchyView::onOpenDesign() {
	QModelIndexList indexes = selectedIndexes();
	foreach (QModelIndex index, indexes) {
		emit openDesign(filter_->mapToSource(index));
	}
}

void HierarchyView::onOpenMemoryDesign() {
    QModelIndexList indexes = selectedIndexes();
    foreach (QModelIndex index, indexes) {
        emit openMemoryDesign(filter_->mapToSource(index));
    }
}

void HierarchyView::onOpenSWDesign() {
    QModelIndexList indexes = selectedIndexes();
    foreach (QModelIndex index, indexes) {
        emit openSWDesign(filter_->mapToSource(index));
    }
}

void HierarchyView::onOpenSystemDesign() {
    QModelIndexList indexes = selectedIndexes();
    foreach (QModelIndex index, indexes) {
        emit openSystemDesign(filter_->mapToSource(index));
    }
}

void HierarchyView::onCreateComponent() {
	emit createNewComponent(filter_->mapToSource(currentIndex()));
}

void HierarchyView::onCreateDesign() {
	emit createNewDesign(filter_->mapToSource(currentIndex()));
}

void HierarchyView::onCreateSWDesign() {
    emit createNewSWDesign(filter_->mapToSource(currentIndex()));
}

void HierarchyView::onCreateSystemDesign() {
    emit createNewSystemDesign(filter_->mapToSource(currentIndex()));
}

void HierarchyView::onAddSignals() {
	emit createNewAbsDef(filter_->mapToSource(currentIndex()));
}

void HierarchyView::onCreateBus() {
	emit createNewBus(filter_->mapToSource(currentIndex()));
}

void HierarchyView::onCreateComDef() {
    emit createNewComDef(filter_->mapToSource(currentIndex()));
}

void HierarchyView::onCreateApiDef() {
    emit createNewApiDef(filter_->mapToSource(currentIndex()));
}

void HierarchyView::onExportAction() {
	QModelIndexList indexes = selectedIndexes();
	foreach (QModelIndex index, indexes) {
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

void HierarchyView::onOpenXml() {
	QModelIndex index = filter_->mapToSource(currentIndex());
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

void HierarchyView::contextMenuEvent( QContextMenuEvent* event ) {
	Q_ASSERT_X(event, "LibraryTreeView::contextMenuEvent",
		"Invalid event pointer given as parameter");

	// accept the event so it is not passed forwards
	event->accept();

	QModelIndex current = currentIndex();
	QModelIndexList indexes = selectedIndexes();

	// if nothing was chosen
	if (!current.isValid()) {
		return;
	}

	// get original model index so internalPointer can be used
	QModelIndex sourceIndex = filter_->mapToSource(current);

	HierarchyItem* item = static_cast<HierarchyItem*>(sourceIndex.internalPointer());
	VLNV vlnv = item->getVLNV();

	// create a menu to show contextMenu actions
	QMenu menu(this);

	// if item can be identified as single item
	if (vlnv.isValid()) {

		// parse the model
		QSharedPointer<LibraryComponent const> libComp = handler_->getModelReadOnly(vlnv);
		Q_ASSERT_X(libComp, "HierarchyView::contextMenuEvent()",
			"Object was not found in library.");

        QMenu* menuNew = 0;

		// if component
		if (item->type() == HierarchyItem::COMPONENT) {
			QSharedPointer<Component const> component = libComp.staticCast<Component const>();

			// depending on the type of the component
			switch (component->getComponentImplementation()) {
            case KactusAttribute::SYSTEM: {
                    menu.addAction(openCompAction_);
                    break;
                }

            case KactusAttribute::SW: {

                    menu.addAction(openCompAction_);

                    menu.addSeparator();

                    if (indexes.size() == 1) {
                        menuNew = menu.addMenu(tr("Add"));
                        menuNew->addAction(createNewSWDesignAction_);
                    }

                    break;
                }

            default: {

                    menu.addAction(openCompAction_);
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

                    break;
                }
            }
		}
		else if (item->type() == HierarchyItem::BUSDEFINITION) {

			QSharedPointer<BusDefinition const> busDef = libComp.staticCast<BusDefinition const>();

            menu.addAction(openBusAction_);
            
            menuNew = menu.addMenu(tr("Add"));
            menuNew->addAction(addSignalsAction_);
		}
		else if (item->type() == HierarchyItem::ABSDEFINITION) {
			menu.addAction(openBusAction_);
		}
        else if (item->type() == HierarchyItem::COMDEFINITION) {
            menu.addAction(openComDefAction_);
        }
        else if (item->type() == HierarchyItem::APIDEFINITION) {
            menu.addAction(openApiDefAction_);
        }
		else if (item->type() == HierarchyItem::HW_DESIGN) {
			menu.addAction(openDesignAction_);
		}
		else if (item->type() == HierarchyItem::SW_DESIGN) {
			menu.addAction(openSWDesignAction_);
		}
		else if (item->type() == HierarchyItem::SYS_DESIGN) {
			menu.addAction(openSystemAction_);
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

	menu.exec(event->globalPos());
}

void HierarchyView::mouseDoubleClickEvent( QMouseEvent * event ) {
	Q_ASSERT_X(event, "LibraryTreeView::contextMenuEvent",
		"Invalid event pointer given as parameter");

	// accept the event so it is not passed forwards
	event->accept();

	QModelIndex origIndex = indexAt(event->pos());

	// get original model index so internalPointer can be used
	QModelIndex sourceIndex = filter_->mapToSource(origIndex);

	if (sourceIndex.isValid()) {
		HierarchyItem* item = static_cast<HierarchyItem*>(sourceIndex.internalPointer());
		switch (item->type()) {
			case HierarchyItem::HW_DESIGN:
				emit openDesign(sourceIndex);
				break;
			case HierarchyItem::SW_DESIGN:
				emit openSWDesign(sourceIndex);
				break;
			case HierarchyItem::SYS_DESIGN:
				emit openSystemDesign(sourceIndex);
				break;
			default:
				emit openComponent(sourceIndex);
				break;
		}

		// select the object even if the instance column was clicked
		QModelIndex indexToSelect = model()->index(origIndex.row(), 0, origIndex.parent());
		setCurrentIndex(indexToSelect);
	}

	// let the default handler process the event
	QTreeView::mouseDoubleClickEvent(event);
}

void HierarchyView::mousePressEvent( QMouseEvent *event ) {
	
	startPos_ = event->pos();
	QModelIndex index = indexAt(startPos_);

	// check if the object is already expanded or not
	bool expanded = false;
	if (index.isValid()) {
		expanded = isExpanded(index);
	}
	
	// select the object even if the instance column was clicked
// 	QModelIndex indexToSelect = model()->index(index.row(), 0, index.parent());
// 	setCurrentIndex(indexToSelect);

	if (event->button() == Qt::LeftButton) {
		dragIndex_ = filter_->mapToSource(index);

		// left click expands/collapses items, right click shows context menu
		if (expanded) {
			collapse(index);
		}
		else {
			expand(index);
		}
	}

	//setCurrentIndex(index);
	QTreeView::mousePressEvent(event);
}

void HierarchyView::mouseReleaseEvent( QMouseEvent * event ) {
	QModelIndex index = indexAt(event->pos());
	
	if (index.isValid() && index.column() == 0) {
		QModelIndex sourceIndex = filter_->mapToSource(index);
		HierarchyItem* item = static_cast<HierarchyItem*>(sourceIndex.internalPointer());
		emit componentSelected(item->getVLNV());
	}

	QTreeView::mouseReleaseEvent(event);
}

void HierarchyView::mouseMoveEvent( QMouseEvent *event ) {
	// if left mouse button is pressed down
	if (event->buttons() & Qt::LeftButton) {

		// calculate the distance of the drag
		int distance = (event->pos() - startPos_).manhattanLength();

		// make sure the drag distance is large enough to start the drag
		if (distance >= QApplication::startDragDistance()) {
			
			// find the item that is being dragged
			HierarchyItem* item = static_cast<HierarchyItem*>(dragIndex_.internalPointer());

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

void HierarchyView::setCurrentIndex( const QModelIndex& index ) {

	QModelIndex temp = index;
	
	// expand the whole tree up to the index
	while (temp.parent().isValid()) {
		temp = temp.parent();
		expand(temp);
	}

	// use the default implementation
	QTreeView::setCurrentIndex(index);
}

void HierarchyView::setSelectedIndexes( const QModelIndexList& indexes ) {
	// clear any previously selected items
	clearSelection();

	foreach (QModelIndex index, indexes) {
		
		if (!index.isValid())
			continue;

		selectionModel()->select(index, QItemSelectionModel::Select);

		QModelIndex temp = index;
		expand(temp);

		// expand the whole tree up to the index
		while (temp.parent().isValid()) {
			temp = temp.parent();
			expand(temp);
		}
	}
}
