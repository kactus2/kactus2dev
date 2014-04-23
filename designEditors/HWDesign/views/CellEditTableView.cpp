//-----------------------------------------------------------------------------
// File: CellEditTableView.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 4.6.2013
//
// Description:
// View for editing tables one cell at a time.
//-----------------------------------------------------------------------------

#include "CellEditTableView.h"

#include <QHeaderView>
#include <QMenu>
#include <QKeySequence>
#include <QClipboard>
#include <QApplication>
#include <QMimeData>
#include <QAbstractTableModel>
#include <QApplication>
#include <QFontMetrics>
#include <QSize>
#include <QHeaderView>

namespace
{
    int const VISIBLE_COLUMNS = 7; 

    int const DEFAULT_COLUMN_WIDTHS[VISIBLE_COLUMNS] =
    {
        120,
        80,
        120,
        35,
        120,
        80,
        120
    };
}

//-----------------------------------------------------------------------------
// Function: CellEditTableView()
//-----------------------------------------------------------------------------
CellEditTableView::CellEditTableView(QWidget *parent):
QTableView(parent),
    copyAction_(tr("Copy"), this),
    pasteAction_(tr("Paste"), this),
    clearAction_(tr("Clear"), this)
{

    // cells are resized to match contents 
    horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);

    //last column is stretched take the available space in the widget
    horizontalHeader()->setStretchLastSection(true);

    // vertical headers are not visible
    verticalHeader()->hide();

    // set the height of a row to be smaller than default
    verticalHeader()->setDefaultSectionSize(fontMetrics().height() + 8);

    // easies to see the different rows from one another
    //setAlternatingRowColors(true);

    // words are wrapped in the cells to minimize space usage
    setWordWrap(true);

    // user can select several items at a time
    setSelectionMode(QAbstractItemView::SingleSelection);

    setEditTriggers(QAbstractItemView::DoubleClicked |
        QAbstractItemView::SelectedClicked |
        QAbstractItemView::EditKeyPressed |
        QAbstractItemView::AnyKeyPressed);

    setupActions();

    setMinimumHeight(MINIMUM_TABLE_HEIGHT);

}

//-----------------------------------------------------------------------------
// Function: ~CellEditTableView()
//-----------------------------------------------------------------------------
CellEditTableView::~CellEditTableView() 
{
}

//-----------------------------------------------------------------------------
// Function: setModel()
//-----------------------------------------------------------------------------
void CellEditTableView::setModel( QAbstractItemModel * model )
{

    QTableView::setModel(model);

    for( int i = 0; i < VISIBLE_COLUMNS; i++)
    {
        setColumnWidth(i,DEFAULT_COLUMN_WIDTHS[i]);
    }


}

//-----------------------------------------------------------------------------
// Function: keyPressEvent()
//-----------------------------------------------------------------------------
void CellEditTableView::keyPressEvent( QKeyEvent* event ) {

    // Call the base class implementation.
    QTableView::keyPressEvent(event);

    if (event->matches(QKeySequence::Copy)) {
        onCopyAction();
    }
    if (event->matches(QKeySequence::Paste)) {
        onPasteAction();
    }
    if (event->matches(QKeySequence::Delete)) {
        onClearAction();
    }
}

//-----------------------------------------------------------------------------
// Function: mousePressEvent()
//-----------------------------------------------------------------------------
void CellEditTableView::mousePressEvent( QMouseEvent* event ) {

    pressedPoint_ = event->pos();

    QModelIndex pressedIndex = indexAt(pressedPoint_);
    if (!pressedIndex.isValid()) {
        setCurrentIndex(pressedIndex);
    }

    QTableView::mousePressEvent(event);	
}

//-----------------------------------------------------------------------------
// Function: contextMenuEvent()
//-----------------------------------------------------------------------------
void CellEditTableView::contextMenuEvent( QContextMenuEvent* event ) {
    pressedPoint_ = event->pos();

    QModelIndex index = indexAt(pressedPoint_);

    if (index.isValid()) {
        QMenu menu(this);		
        menu.addAction(&copyAction_);
        menu.addAction(&pasteAction_);	
        menu.addAction(&clearAction_);


        QMimeData const* mime = QApplication::clipboard()->mimeData();
        pasteAction_.setEnabled(mime != 0 && mime->hasImage() && mime->imageData().isValid() && model()->data(index,Qt::EditRole).isValid());	

        clearAction_.setEnabled(model()->data(index,Qt::EditRole).isValid());
        menu.exec(event->globalPos());    
    }

    event->accept();
}

//-----------------------------------------------------------------------------
// Function: setupActions()
//-----------------------------------------------------------------------------
void CellEditTableView::setupActions() {

    copyAction_.setToolTip(tr("Copy a value from the table"));
    copyAction_.setStatusTip(tr("Copy a value from the table"));
    connect(&copyAction_, SIGNAL(triggered()),
        this, SLOT(onCopyAction()), Qt::UniqueConnection);
    copyAction_.setShortcut(QKeySequence::Copy);

    pasteAction_.setToolTip(tr("Paste a value to the table"));
    pasteAction_.setStatusTip(tr("Paste a value to the table"));
    connect(&pasteAction_, SIGNAL(triggered()),
        this, SLOT(onPasteAction()), Qt::UniqueConnection);
    pasteAction_.setShortcut(QKeySequence::Paste);

    clearAction_.setToolTip(tr("Clear the contents of a cell"));
    clearAction_.setStatusTip(tr("Clear the contents of a cell"));
    connect(&clearAction_, SIGNAL(triggered()),
        this, SLOT(onClearAction()), Qt::UniqueConnection);
    clearAction_.setShortcut(QKeySequence::Delete);
}

//-----------------------------------------------------------------------------
// Function: onCopyAction()
//-----------------------------------------------------------------------------
void CellEditTableView::onCopyAction() {

    // if nothing was selected then don't copy anything.
    if (!indexAt(pressedPoint_).isValid()) {
        return;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);

    QMimeData* mime = new QMimeData();
    mime->setImageData(model()->data(indexAt(pressedPoint_), Qt::DisplayRole));
    QApplication::clipboard()->setMimeData(mime);

    QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------
// Function: onPasteAction()
//-----------------------------------------------------------------------------
void CellEditTableView::onPasteAction() {

    if (!currentIndex().isValid()) {
        return;
    }

    QMimeData const* mime = QApplication::clipboard()->mimeData();
    if ( mime->hasImage() )
    {
        model()->setData(currentIndex(), mime->imageData(), Qt::EditRole);
    }

}

//-----------------------------------------------------------------------------
// Function: onClearAction()
//-----------------------------------------------------------------------------
void CellEditTableView::onClearAction() {

    if (!currentIndex().isValid()) {
        return;
    }

    model()->setData(currentIndex(), QVariant(), Qt::EditRole);
}
