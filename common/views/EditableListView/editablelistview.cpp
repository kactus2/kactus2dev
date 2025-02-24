//-----------------------------------------------------------------------------
// File: editablelistview.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 12.12.2011
//
// Description:
// View-class with add, remove and move operations for lists of items.
//-----------------------------------------------------------------------------

#include "editablelistview.h"

#include <QMenu>
#include <QPainter>

#include <common/KactusColors.h>

//-----------------------------------------------------------------------------
// Function: EditableListView::EditableListView()
//-----------------------------------------------------------------------------
EditableListView::EditableListView(QWidget *parent):
QListView(parent),
    pressedPoint_(),
    addAction_(tr("Add new"), this),
    removeAction_(tr("Remove"), this)
{
    setMovement(QListView::Free);

    setToolTip(tr("Double click to add new item."));

	setupActions();
}

//-----------------------------------------------------------------------------
// Function: EditableListView::~EditableListView()
//-----------------------------------------------------------------------------
EditableListView::~EditableListView()
{
}

//-----------------------------------------------------------------------------
// Function: EditableListView::paintEvent()
//-----------------------------------------------------------------------------
void EditableListView::paintEvent(QPaintEvent *event)
{
    QListView::paintEvent(event);

    if (model() && model()->rowCount() == 0) 
    {     
        const int MARGIN = 2;
        QRect placeholderTextRec = rect();
        placeholderTextRec.setX(placeholderTextRec.x() + MARGIN);
        placeholderTextRec.setY(placeholderTextRec.y() + MARGIN);
        placeholderTextRec.setWidth(placeholderTextRec.width() - MARGIN);
        placeholderTextRec.setHeight(placeholderTextRec.height() - MARGIN);

        QPainter p(this->viewport());
        p.setPen(KactusColors::DISABLED_TEXT);
        p.drawText(placeholderTextRec, Qt::AlignLeft | Qt::TextWordWrap, tr("Double click to add new item."));
    }
}

//-----------------------------------------------------------------------------
// Function: EditableListView::mouseMoveEvent()
//-----------------------------------------------------------------------------
void EditableListView::mouseMoveEvent(QMouseEvent* e)
{
	// if left mouse button was pressed 
	if (e->buttons() & Qt::LeftButton)
    {
		QModelIndex startIndex = indexAt(pressedPoint_);
		QModelIndex thisIndex = indexAt(e->pos());

		// if the item was dragged to new location
		if (startIndex.isValid() && startIndex != thisIndex)
        {
			setCursor(QCursor(Qt::ClosedHandCursor));

			emit moveItem(startIndex, thisIndex);

			// update the pressed point so the dragging works also when moving further to another index
			pressedPoint_ = e->pos();
		}
	}

	QListView::mouseMoveEvent(e);
}

//-----------------------------------------------------------------------------
// Function: EditableListView::mouseDoubleClickEvent()
//-----------------------------------------------------------------------------
void EditableListView::mouseDoubleClickEvent(QMouseEvent* event)
{
	// if there is no item on the clicked position then a new item should be added
	QModelIndex index = indexAt(event->pos());
	if (!index.isValid())
    {
        onAddAction();
		event->accept();                
		return;
	}

	QListView::mouseDoubleClickEvent(event);
}

//-----------------------------------------------------------------------------
// Function: EditableListView::mousePressEvent()
//-----------------------------------------------------------------------------
void EditableListView::mousePressEvent(QMouseEvent* event)
{
	pressedPoint_ = event->pos();
	QListView::mousePressEvent(event);
}

//-----------------------------------------------------------------------------
// Function: EditableListView::mouseReleaseEvent()
//-----------------------------------------------------------------------------
void EditableListView::mouseReleaseEvent(QMouseEvent* event)
{
	setCursor(QCursor(Qt::ArrowCursor));
	QListView::mouseReleaseEvent(event);
}

//-----------------------------------------------------------------------------
// Function: EditableListView::contextMenuEvent()
//-----------------------------------------------------------------------------
void EditableListView::contextMenuEvent(QContextMenuEvent* event)
{
	pressedPoint_ = event->pos();

	QMenu menu(this);
	menu.addAction(&addAction_);
	menu.addAction(&removeAction_);
    
    removeAction_.setEnabled(selectionModel()->hasSelection());

	menu.exec(event->globalPos());

    removeAction_.setEnabled(true);
	event->accept();
}

//-----------------------------------------------------------------------------
// Function: EditableListView::rowsInserted()
//-----------------------------------------------------------------------------
void EditableListView::rowsInserted(QModelIndex const& parent, int start, int end)
{
    QListView::rowsInserted(parent, start, end);

    QModelIndex lastIndex = model()->index(end, 0, parent);
    setCurrentIndex(lastIndex);
}

//-----------------------------------------------------------------------------
// Function: EditableListView::onAddAction()
//-----------------------------------------------------------------------------
void EditableListView::onAddAction() 
{
	QModelIndex index = indexAt(pressedPoint_);
	emit addItem(index);
}

//-----------------------------------------------------------------------------
// Function: EditableListView::onRemoveAction()
//-----------------------------------------------------------------------------
void EditableListView::onRemoveAction()
{
	QModelIndex index = currentIndex();
	emit removeItem(index);
}

//-----------------------------------------------------------------------------
// Function: EditableListView::setupActions()
//-----------------------------------------------------------------------------
void EditableListView::setupActions()
{
    addAction(&addAction_);
    addAction_.setEnabled(true);
    addAction_.setShortcut(QKeySequence::InsertLineSeparator);
    addAction_.setShortcutContext(Qt::WidgetShortcut);
    QString tooltipAdd = tr("Add a new item to list (%1)").arg(addAction_.shortcut().toString(QKeySequence::NativeText));
    addAction_.setToolTip(tooltipAdd);
    addAction_.setStatusTip(tooltipAdd);
    connect(&addAction_, SIGNAL(triggered()), this, SLOT(onAddAction()), Qt::UniqueConnection);

    addAction(&removeAction_);
    removeAction_.setEnabled(true);
    removeAction_.setShortcut(QKeySequence::Delete);
    removeAction_.setShortcutContext(Qt::WidgetShortcut);
    QString tooltipRemove = tr("Remove an item from the list (%1)").arg(removeAction_.shortcut().toString(QKeySequence::NativeText));
    removeAction_.setToolTip(tooltipRemove);
    removeAction_.setStatusTip(tooltipRemove);
    connect(&removeAction_, SIGNAL(triggered()), this, SLOT(onRemoveAction()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: EditableListView::currentChanged()
//-----------------------------------------------------------------------------
void EditableListView::currentChanged(QModelIndex const& current, QModelIndex const& previous)
{
    QListView::currentChanged(current, previous);
    emit selectionChanged(current);
}

//-----------------------------------------------------------------------------
// Function: editablelistview::keyPressEvent()
//-----------------------------------------------------------------------------
void EditableListView::keyPressEvent(QKeyEvent *event)
{
    int rowCount = model()->rowCount();

    if (currentIndex().row() == 0 && event->key() == Qt::Key_Up)
    {
        setCurrentIndex(model()->index(rowCount - 1, 0));
    }
    else if (currentIndex().row() == rowCount - 1 && event->key() == Qt::Key_Down)
    {
        setCurrentIndex(model()->index(0, 0));
    }
    else
    {
        QListView::keyPressEvent(event);
    }
}
