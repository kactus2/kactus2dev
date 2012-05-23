/* 
 *  	Created on: 1.7.2011
 *      Author: Antti Kamppi
 * 		filename: hierarchyview.cpp
 *		Project: Kactus 2
 */

#include "hierarchyview.h"

#include "hierarchyfilter.h"
#include "hierarchyitem.h"

#include <LibraryManager/libraryinterface.h>
#include <models/component.h>
#include <models/busdefinition.h>

#include <QMenu>
#include <QApplication>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <QItemSelectionModel>

#include <QDebug>

HierarchyView::HierarchyView(QWidget *parent, 
							 LibraryInterface* handler,
							 HierarchyFilter* filter)
    : QTreeView(parent),
      handler_(handler),
      filter_(filter),
      startPos_(),
      dragIndex_(),
      openDesignAction_(NULL),
      openSWDesignAction_(NULL),
      openCompAction_(NULL),
      createNewComponentAction_(NULL),
      createNewDesignAction_(NULL),
      createNewSWDesignAction_(NULL),
      exportAction_(NULL),
      openBusAction_(NULL),
      addSignalsAction_(NULL),
      createBusAction_(NULL),
      openComDefAction_(NULL),
      createComDefAction_(NULL),
      openApiDefAction_(NULL),
      createApiDefAction_(NULL),
      openSystemAction_(NULL),
      openXmlAction_(NULL)
{

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
	openDesignAction_ = new QAction(tr("Open Design"), this);
	openDesignAction_->setStatusTip(tr("Open a hierarchical design"));
	openDesignAction_->setToolTip(tr("Open a hierarchical design"));
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

	createNewComponentAction_ = new QAction(tr("Create New Component"), this);
	createNewComponentAction_->setStatusTip(tr("Create new component"));
	createNewComponentAction_->setToolTip(tr("Create new component"));
	connect(createNewComponentAction_, SIGNAL(triggered()),
		this, SLOT(onCreateComponent()), Qt::UniqueConnection);

	createNewDesignAction_ = new QAction(tr("Create New Design"), this);
	createNewDesignAction_->setStatusTip(tr("Create new design"));
	createNewDesignAction_->setToolTip(tr("Create new design"));
	connect(createNewDesignAction_, SIGNAL(triggered()),
		this, SLOT(onCreateDesign()), Qt::UniqueConnection);

    createNewSWDesignAction_ = new QAction(tr("Create New SW Design"), this);
    createNewSWDesignAction_->setStatusTip(tr("Create new SW design"));
    createNewSWDesignAction_->setToolTip(tr("Create new SW design"));
    connect(createNewSWDesignAction_, SIGNAL(triggered()),
        this, SLOT(onCreateSWDesign()), Qt::UniqueConnection);

	exportAction_ = new QAction(tr("Export"), this);
	exportAction_->setStatusTip(tr("Export item and it's sub-items to another location"));
	exportAction_->setToolTip(tr("Export item and it's sub-items to another location"));
	connect(exportAction_, SIGNAL(triggered()),
		this, SLOT(onExportAction()), Qt::UniqueConnection);

	openBusAction_ = new QAction(tr("Open Bus"), this);
	openBusAction_->setStatusTip(tr("Open the bus in bus editor"));
	openBusAction_->setToolTip(tr("Open the bus in bus editor"));
	connect(openBusAction_, SIGNAL(triggered()),
		this, SLOT(onOpenBus()), Qt::UniqueConnection);

	addSignalsAction_ = new QAction(tr("Add Signals to Bus"), this);
	addSignalsAction_->setStatusTip(tr("Add new signal definitions to selected bus"));
	addSignalsAction_->setToolTip(tr("Add new signal definitions to selected bus"));
	connect(addSignalsAction_, SIGNAL(triggered()),
		this, SLOT(onAddSignals()), Qt::UniqueConnection);

	createBusAction_ = new QAction(tr("Create New Bus"), this);
	createBusAction_->setStatusTip(tr("Create new bus"));
	createBusAction_->setToolTip(tr("Create new bus"));
	connect(createBusAction_, SIGNAL(triggered()),
		this, SLOT(onCreateBus()), Qt::UniqueConnection);

    openComDefAction_ = new QAction(tr("Open COM Definition"), this);
    openComDefAction_->setStatusTip(tr("Open the COM definition in an editor"));
    openComDefAction_->setToolTip(tr("Open the COM definition in an editor"));
    connect(openComDefAction_, SIGNAL(triggered()),
        this, SLOT(onOpenComDef()), Qt::UniqueConnection);

    createComDefAction_ = new QAction(tr("Create New COM Definition"), this);
    createComDefAction_->setStatusTip(tr("Create new COM definition"));
    createComDefAction_->setToolTip(tr("Create new COM definition"));
    connect(createComDefAction_, SIGNAL(triggered()),
        this, SLOT(onCreateComDef()), Qt::UniqueConnection);

    openApiDefAction_ = new QAction(tr("Open API Definition"), this);
    openApiDefAction_->setStatusTip(tr("Open the API definition in an editor"));
    openApiDefAction_->setToolTip(tr("Open the API definition in an editor"));
    connect(openApiDefAction_, SIGNAL(triggered()),
        this, SLOT(onOpenApiDef()), Qt::UniqueConnection);

    createApiDefAction_ = new QAction(tr("Create New API Definition"), this);
    createApiDefAction_->setStatusTip(tr("Create new API definition"));
    createApiDefAction_->setToolTip(tr("Create new API definition"));
    connect(createApiDefAction_, SIGNAL(triggered()),
        this, SLOT(onCreateApiDef()), Qt::UniqueConnection);

	openSystemAction_ = new QAction(tr("Open System"), this);
	openSystemAction_->setStatusTip(tr("Open system for editing"));
	openSystemAction_->setToolTip(tr("Open system for editing"));
	connect(openSystemAction_, SIGNAL(triggered()),
		this, SLOT(onOpenComponent()), Qt::UniqueConnection);

	openXmlAction_ = new QAction(tr("Open the xml-file"), this);
	connect(openXmlAction_, SIGNAL(triggered()),
		this, SLOT(onOpenXml()), Qt::UniqueConnection);
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

void HierarchyView::onOpenSWDesign() {
    QModelIndexList indexes = selectedIndexes();
    foreach (QModelIndex index, indexes) {
        emit openSWDesign(filter_->mapToSource(index));
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

void HierarchyView::onOpenXml() {
	QModelIndex index = filter_->mapToSource(currentIndex());
	HierarchyItem* item = static_cast<HierarchyItem*>(index.internalPointer());
	VLNV vlnv = item->getVLNV();

	QString xmlPath = handler_->getPath(vlnv);
	// open the file in operating system's default editor
	QDesktopServices::openUrl(QUrl::fromLocalFile(xmlPath));
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

	bool hierarchical = false;

	// if item can be identified as single item
	if (vlnv.isValid()) {

		// parse the model
		QSharedPointer<LibraryComponent> libComp = handler_->getModel(vlnv);
		Q_ASSERT_X(libComp, "HierarchyView::contextMenuEvent()",
			"Object was not found in library.");

		// if component
		if (item->type() == HierarchyItem::COMPONENT)
        {
			QSharedPointer<Component> component = libComp.staticCast<Component>();

			// depending on the type of the component
			switch (component->getComponentImplementation())
            {
            case KactusAttribute::KTS_SYS:
                {
                    menu.addAction(openSystemAction_);
                    break;
                }

            case KactusAttribute::KTS_SW:
                {
                    if (component->hasSWViews())
                    {
                        menu.addAction(openSWDesignAction_);
                    }

                    menu.addAction(openCompAction_);

                    if (!component->hasSWViews() && indexes.size() == 1)
                    {
                        menu.addAction(createNewSWDesignAction_);
                    }
                    break;
                }

            default:
                {
                    if (component->isHierarchical())
                    {
                        menu.addAction(openDesignAction_);
                        hierarchical = true;
                    }

                    if (component->hasSWViews())
                    {
                        menu.addAction(openSWDesignAction_);
                    }

                    menu.addAction(openCompAction_);

                    // only if just one object is selected
                    if (indexes.size() == 1)
                    {
                        menu.addAction(createNewComponentAction_);
                    }

                    // if component was not hierarchical then design can be created for it.
                    if (!hierarchical && indexes.size() == 1)
                    {
                        menu.addAction(createNewDesignAction_);
                    }

                    if (!component->hasSWViews() && indexes.size() == 1)
                    {
                        menu.addAction(createNewSWDesignAction_);
                    }
                    break;
                }
            }
		}
		else if (item->type() == HierarchyItem::BUSDEFINITION) {

			QSharedPointer<BusDefinition> busDef = libComp.staticCast<BusDefinition>();

			// make sure the bus is for hardware
			if (busDef->getType() == KactusAttribute::KTS_BUSDEF_HW) {
				menu.addAction(openBusAction_);
				menu.addAction(addSignalsAction_);
				menu.addAction(createBusAction_);
			}
		}
		else if (item->type() == HierarchyItem::ABSDEFINITION) {
			menu.addAction(openBusAction_);
			menu.addAction(createBusAction_);
		}
        else if (item->type() == HierarchyItem::COMDEFINITION) {
            menu.addAction(openComDefAction_);
            menu.addAction(createComDefAction_);
        }
        else if (item->type() == HierarchyItem::APIDEFINITION) {
            menu.addAction(openApiDefAction_);
            menu.addAction(createApiDefAction_);
        }
	}
	
	menu.addAction(exportAction_);

	menu.addAction(openXmlAction_);

	menu.exec(event->globalPos());
}

void HierarchyView::mouseDoubleClickEvent( QMouseEvent * event ) {
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

void HierarchyView::mousePressEvent( QMouseEvent *event ) {
	if (event->button() == Qt::LeftButton) {
		startPos_ = event->pos();
		QModelIndex index = indexAt(startPos_);
		dragIndex_ = filter_->mapToSource(index);
	}
	QTreeView::mousePressEvent(event);
}

void HierarchyView::mouseReleaseEvent( QMouseEvent * event ) {
	QModelIndex index = indexAt(event->pos());
	// if shift or control was not pressed and left mouse button was
	if ((event->modifiers() & Qt::ShiftModifier) == 0 &&
		(event->modifiers() & Qt::ControlModifier) == 0 &&
		event->button() == Qt::LeftButton) {

			// if the item is not yet expanded
			if (!isExpanded(index))
				expand(index);

			// if item was expanded then close it
			else
				collapse(index);
	}

	if (index.isValid()) {
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
			emit dragInitiated(dragIndex_);
		}
	}
	QTreeView::mouseMoveEvent(event);
}

void HierarchyView::setCurrentIndex( const QModelIndex& index ) {

	QModelIndex temp = index;
	expand(temp);
	
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
