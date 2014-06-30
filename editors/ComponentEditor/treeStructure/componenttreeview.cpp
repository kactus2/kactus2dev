/* 
 *
 *  Created on: 21.1.2011
 *      Author: Antti Kamppi
 */

#include "componenttreeview.h"
#include "componenteditoritem.h"
#include <library/LibraryManager/libraryinterface.h>
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
QTreeView(parent),
pressedPoint_(),
locked_(true),
fileEditAction_(tr("Edit"), this),
handler_(handler),
componentVLNV_(compVLNV) 
{
	// can be used in debugging to identify the object
	setObjectName(tr("ComponentTreeView"));

	// header is not shown 
	setHeaderHidden(true);

	setIndentation(20);

	// only one item can be selected at a time
	setSelectionMode(QAbstractItemView::SingleSelection);

	setSelectionBehavior(QAbstractItemView::SelectItems);

	connect(&fileEditAction_, SIGNAL(triggered()),
		this, SLOT(onFileOpen()), Qt::UniqueConnection);

    fileEditAction_.setToolTip(tr("Open the file for editing."));
    fileEditAction_.setStatusTip(tr("Open the file for editing."));
}

//-----------------------------------------------------------------------------
// Function: ComponentTreeView::~ComponentTreeView()
//-----------------------------------------------------------------------------
ComponentTreeView::~ComponentTreeView() 
{

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
	QModelIndex index = indexAt(event->pos());

	if (!index.isValid()) {
		return;
	}
	
	// save the position where click occurred
	pressedPoint_ = event->pos();

    // To get correct internal pointer, the index from source model must be used. Proxy performs the mapping.
    ComponentEditorTreeProxyModel* proxy = dynamic_cast<ComponentEditorTreeProxyModel*>(model());
    Q_ASSERT(proxy != 0);
	ComponentEditorItem* item = static_cast<ComponentEditorItem*>(proxy->mapToSource(index).internalPointer());
	
	// if item can be opened then show the context menu
	if (item->canBeOpened()) 
    {
		QMenu menu(this);
		menu.addAction(&fileEditAction_);
		menu.exec(event->globalPos());
	}

	event->accept();
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
	
	onFileOpen();
}

//-----------------------------------------------------------------------------
// Function: ComponentTreeView::onFileOpen()
//-----------------------------------------------------------------------------
void ComponentTreeView::onFileOpen()
{
	const QString xmlPath = handler_->getPath(componentVLNV_);
	
	QModelIndex index = indexAt(pressedPoint_);
	
    // To get correct internal pointer, the index from source model must be used. Proxy performs the mapping.
    ComponentEditorTreeProxyModel* proxy = dynamic_cast<ComponentEditorTreeProxyModel*>(model());
    Q_ASSERT(proxy != 0);
	ComponentEditorItem* item = static_cast<ComponentEditorItem*>(proxy->mapToSource(index).internalPointer());
    
	item->openItem(item->hasBuiltinEditor());
}
