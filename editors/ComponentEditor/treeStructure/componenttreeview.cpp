/* 
 *
 *  Created on: 21.1.2011
 *      Author: Antti Kamppi
 */

#include "componenttreeview.h"
#include "componenteditoritem.h"
#include <library/LibraryManager/libraryinterface.h>
#include <IPXACTmodels/generaldeclarations.h>
#include <editors/ComponentEditor/treeStructure/ComponentEditorTreeSortProxyModel.h>

#include <QEvent>
#include <QCursor>
#include <QApplication>
#include <QMenu>
#include <QDesktopServices>
#include <QUrl>

ComponentTreeView::ComponentTreeView(LibraryInterface* handler,
									 const VLNV& compVLNV,
									 QWidget *parent):
QTreeView(parent),
pressedPoint_(),
locked_(true),
fileOpenAction_(tr("Open"), this),
fileOpenDefaultAction_(tr("Open in Default Editor"), this),
handler_(handler),
componentVLNV_(compVLNV) {

	// can be used in debugging to identify the object
	setObjectName(tr("ComponentTreeView"));

	// header is not shown 
	setHeaderHidden(true);

	setIndentation(20);

	// only one item can be selected at a time
	setSelectionMode(QAbstractItemView::SingleSelection);

	setSelectionBehavior(QAbstractItemView::SelectItems);

	connect(&fileOpenAction_, SIGNAL(triggered()),
		this, SLOT(onFileOpen()), Qt::UniqueConnection);
    connect(&fileOpenDefaultAction_, SIGNAL(triggered()),
        this, SLOT(onFileOpenDefault()), Qt::UniqueConnection);

    fileOpenAction_.setToolTip(tr("Open the file for editing."));
    fileOpenAction_.setStatusTip(tr("Open the file for editing."));

	fileOpenDefaultAction_.setToolTip(tr("Open the file in operating system's default editor."));
	fileOpenDefaultAction_.setStatusTip(tr("Open the file in operating system's default editor."));
}

ComponentTreeView::~ComponentTreeView() {
}

void ComponentTreeView::currentChanged(const QModelIndex & current, 
									   const QModelIndex & previous ) 
{
	// call the base class implementation
	QTreeView::currentChanged(current, previous);

	// inform that the current index has changed
	emit activated(current);
}

void ComponentTreeView::keyPressEvent( QKeyEvent* event ) {
	QTreeView::keyPressEvent(event);
}

void ComponentTreeView::mousePressEvent( QMouseEvent* event ) {
	pressedPoint_ = event->pos();
	QTreeView::mousePressEvent(event);
}

void ComponentTreeView::mouseReleaseEvent( QMouseEvent* event ) {

	// calculate the distance of the drag
	//int distance = (event->pos() - pressedPoint_).manhattanLength();

	// make sure widget is not locked
	if (!locked_) {

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

void ComponentTreeView::mouseMoveEvent( QMouseEvent* event ) {

	// if not dragging an item or item is locked
	if (event->buttons() == Qt::NoButton || locked_)
		QTreeView::mouseMoveEvent(event);
	
	// if dragging item
	else {
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
		if (parent1 == parent2) {
			setCursor(QCursor(Qt::ClosedHandCursor));
		}
		else {
			setCursor(QCursor(Qt::ForbiddenCursor));
		}
	}
}

void ComponentTreeView::setLocked( bool locked ) {
	locked_ = locked;
}

void ComponentTreeView::selectItem( const QModelIndex& index ) {
	selectionModel()->select(index, QItemSelectionModel::ClearAndSelect);
}

void ComponentTreeView::contextMenuEvent( QContextMenuEvent* event ) {
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
	if (item->canBeOpened()) {
		QMenu menu(this);
		menu.addAction(&fileOpenAction_);
        
        if (item->hasBuiltinEditor())
        {
            menu.addAction(&fileOpenDefaultAction_);
        }

		menu.exec(event->globalPos());
	}

	event->accept();
}

void ComponentTreeView::mouseDoubleClickEvent( QMouseEvent* event ) {
	event->accept();

	QModelIndex index = indexAt(event->pos());

	if (!index.isValid()) {
		return;
	}

	// save the position where click occurred
	pressedPoint_ = event->pos();

	//ComponentEditorItem* item = static_cast<ComponentEditorItem*>(index.internalPointer());
	
	onFileOpen();
}


void ComponentTreeView::onFileOpen() {
	const QString xmlPath = handler_->getPath(componentVLNV_);
	
	QModelIndex index = indexAt(pressedPoint_);
	
    // To get correct internal pointer, the index from source model must be used. Proxy performs the mapping.
    ComponentEditorTreeProxyModel* proxy = dynamic_cast<ComponentEditorTreeProxyModel*>(model());
    Q_ASSERT(proxy != 0);
	ComponentEditorItem* item = static_cast<ComponentEditorItem*>(proxy->mapToSource(index).internalPointer());
    
	item->openItem(true);
}

void ComponentTreeView::onFileOpenDefault() {
    const QString xmlPath = handler_->getPath(componentVLNV_);

    QModelIndex index = indexAt(pressedPoint_);

    // To get correct internal pointer, the index from source model must be used. Proxy performs the mapping.
    ComponentEditorTreeProxyModel* proxy = dynamic_cast<ComponentEditorTreeProxyModel*>(model());
    Q_ASSERT(proxy != 0);
    ComponentEditorItem* item = static_cast<ComponentEditorItem*>(proxy->mapToSource(index).internalPointer());
    Q_ASSERT(item->canBeOpened());

    item->openItem(false);
}