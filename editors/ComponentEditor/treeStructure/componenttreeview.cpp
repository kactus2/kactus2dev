/* 
 *
 *  Created on: 21.1.2011
 *      Author: Antti Kamppi
 */

#include "componenttreeview.h"

#include "componenteditoritem.h"

#include <KactusAPI/include/LibraryInterface.h>
#include <editors/ComponentEditor/treeStructure/ComponentEditorTreeSortProxyModel.h>

#include <QEvent>
#include <QCursor>
#include <QMenu>
#include <QUrl>

//-----------------------------------------------------------------------------
// Function: ComponentTreeView::ComponentTreeView()
//-----------------------------------------------------------------------------
ComponentTreeView::ComponentTreeView(LibraryInterface* handler,
									 const VLNV& compVLNV,
									 QWidget *parent):
ExpandingTreeView(parent),
pressedPoint_(),
locked_(true),
handler_(handler),
componentVLNV_(compVLNV),
expandAllItemsAction_(new QAction(tr("Expand all"), this)),
collapseAllItemsAction_(new QAction(tr("Collapse all"), this)),
itemExpandAction_(new QAction("Expand", this)),
itemCollapseAction_(new QAction("Collapse", this))
{
	// can be used in debugging to identify the object
	setObjectName(tr("ComponentTreeView"));

	// header is not shown 
	setHeaderHidden(true);

	setIndentation(20);

	setFrameShape(QFrame::NoFrame);

	// only one item can be selected at a time
	setSelectionMode(QAbstractItemView::SingleSelection);

	setSelectionBehavior(QAbstractItemView::SelectItems);

    connect(expandAllItemsAction_, SIGNAL(triggered()), this, SLOT(expandAll()), Qt::UniqueConnection);
    connect(collapseAllItemsAction_, SIGNAL(triggered()), this, SLOT(collapseAll()), Qt::UniqueConnection);

    connect(itemExpandAction_, SIGNAL(triggered()), this, SLOT(onExpandItem()), Qt::UniqueConnection);
    connect(itemCollapseAction_, SIGNAL(triggered()), this, SLOT(onCollapseItem()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ComponentTreeView::currentChanged()
//-----------------------------------------------------------------------------
void ComponentTreeView::currentChanged(const QModelIndex & current, 
									   const QModelIndex & previous ) 
{
	// call the base class implementation
	QTreeView::currentChanged(current, previous);

	// inform that the current index has changed
	emit activated(current);
}

//-----------------------------------------------------------------------------
// Function: ComponentTreeView::mousePressEvent()
//-----------------------------------------------------------------------------
void ComponentTreeView::mousePressEvent( QMouseEvent* event )
{
	pressedPoint_ = event->pos();
	QTreeView::mousePressEvent(event);

    QModelIndex index = indexAt(pressedPoint_);
}

//-----------------------------------------------------------------------------
// Function: ComponentTreeView::mouseReleaseEvent()
//-----------------------------------------------------------------------------
void ComponentTreeView::mouseReleaseEvent( QMouseEvent* event ) 
{
	// make sure widget is not locked
	if (!locked_) 
    {
		QModelIndex pressedIndex = indexAt(pressedPoint_);

		QModelIndex releaseIndex = indexAt(event->pos());

		// if either index is invalid or indexes are the same
		if (!releaseIndex.isValid() || !pressedIndex.isValid() || 
			(releaseIndex == pressedIndex)) {
			
			// do the normal release event functionality
			QTreeView::mouseReleaseEvent(event);

			// restore the normal cursor
			setCursor(QCursor(Qt::ArrowCursor));
			return;
		}

		QModelIndex parent1 = pressedIndex.model()->parent(pressedIndex);
		QModelIndex parent2 = releaseIndex.model()->parent(releaseIndex);

		if (parent1.isValid() && parent1 == parent2) {
			emit moveItem(pressedIndex, releaseIndex);

			QModelIndex newIndex = indexAt(event->pos());
			setCurrentIndex(newIndex);
			emit activated(newIndex);
		}
	}

	// restore the normal cursor
	setCursor(QCursor(Qt::ArrowCursor));

	QTreeView::mouseReleaseEvent(event);
}

//-----------------------------------------------------------------------------
// Function: ComponentTreeView::mouseMoveEvent()
//-----------------------------------------------------------------------------
void ComponentTreeView::mouseMoveEvent( QMouseEvent* event )
{
	// if not dragging an item or item is locked
	if (event->buttons() == Qt::NoButton || locked_)
    {
		QTreeView::mouseMoveEvent(event);
	}
	// if dragging item
	else 
    {
		QModelIndex startIndex = indexAt(pressedPoint_);
		QModelIndex thisIndex = indexAt(event->pos());

		// if either index is invalid then dragging is not possible
		if (!startIndex.isValid() || !thisIndex.isValid()) {
			setCursor(QCursor(Qt::ForbiddenCursor));
			return;
		}

		QModelIndex parent1 = startIndex.model()->parent(startIndex);
		QModelIndex parent2 = thisIndex.model()->parent(thisIndex);

		// if both have same parent
		if (parent1 == parent2) 
        {
			setCursor(QCursor(Qt::ClosedHandCursor));
		}
		else
        {
			setCursor(QCursor(Qt::ForbiddenCursor));
		}
	}
}

//-----------------------------------------------------------------------------
// Function: ComponentTreeView::setLocked()
//-----------------------------------------------------------------------------
void ComponentTreeView::setLocked( bool locked )
{
	locked_ = locked;
}

//-----------------------------------------------------------------------------
// Function: ComponentTreeView::selectItem()
//-----------------------------------------------------------------------------
void ComponentTreeView::selectItem( const QModelIndex& index )
{
	selectionModel()->select(index, QItemSelectionModel::ClearAndSelect);
}

//-----------------------------------------------------------------------------
// Function: ComponentTreeView::contextMenuEvent()
//-----------------------------------------------------------------------------
void ComponentTreeView::contextMenuEvent( QContextMenuEvent* event )
{
    menuIndex_ = indexAt(event->pos());

	// save the position where click occurred
	pressedPoint_ = event->pos();
	
	// if item can be opened then show the context menu
    QMenu menu(this);

    if (ComponentEditorItem* item = getPressedItem(); item)
    {
         menu.addActions(item->actions());

        if (item->hasChildren())
        {
            menu.addAction(itemExpandAction_);
            menu.addAction(itemCollapseAction_);

            menu.addSeparator();
        }
    }

    menu.addAction(expandAllItemsAction_);
    menu.addAction(collapseAllItemsAction_);

    menu.exec(event->globalPos());

	event->accept();
}

//-----------------------------------------------------------------------------
// Function: ComponentTreeView::onExpandItem()
//-----------------------------------------------------------------------------
void ComponentTreeView::onExpandItem()
{
    expand(menuIndex_);
}

//-----------------------------------------------------------------------------
// Function: ComponentTreeView::onCollapseItem()
//-----------------------------------------------------------------------------
void ComponentTreeView::onCollapseItem()
{
    collapse(menuIndex_);
}

//-----------------------------------------------------------------------------
// Function: ComponentTreeView::mouseDoubleClickEvent()
//-----------------------------------------------------------------------------
void ComponentTreeView::mouseDoubleClickEvent( QMouseEvent* event )
{
	event->accept();

	QModelIndex index = indexAt(event->pos());

	if (!index.isValid()) {
		return;
	}

	// save the position where click occurred
	pressedPoint_ = event->pos();
	
	getPressedItem()->openItem();
}

//-----------------------------------------------------------------------------
// Function: ComponentTreeView::getPressedItem()
//-----------------------------------------------------------------------------
ComponentEditorItem* ComponentTreeView::getPressedItem()
{
    // To get correct internal pointer, the index from source model must be used. Proxy performs the mapping.
    ComponentEditorTreeProxyModel* proxy = dynamic_cast<ComponentEditorTreeProxyModel*>(model());
    Q_ASSERT(proxy != 0);

    QModelIndex index = indexAt(pressedPoint_);

    return static_cast<ComponentEditorItem*>(proxy->mapToSource(index).internalPointer());    
}
