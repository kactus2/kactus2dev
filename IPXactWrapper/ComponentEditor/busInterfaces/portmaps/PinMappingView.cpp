//-----------------------------------------------------------------------------
// File: PinMappingView.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 3.10.2013
//
// Description:
// View for bit-field mapping.
//-----------------------------------------------------------------------------

#include "PinMappingView.h"

#include "physlistview.h"
#include <LibraryManager/vlnv.h>

#include <QHeaderView>
#include <QMenu>
#include <QKeySequence>
#include <QClipboard>
#include <QApplication>
#include <QModelIndexList>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QSortFilterProxyModel>
#include <QAbstractTableModel>
#include <QApplication>
#include <QFontMetrics>
#include <QSize>
#include <QHeaderView>

//-----------------------------------------------------------------------------
// Function: PinMappingView::PinMappingView()
//-----------------------------------------------------------------------------
PinMappingView::PinMappingView(QWidget *parent)
    : QTableView(parent),
      pressedPoint_(),
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
	setAlternatingRowColors(true);

	// words are wrapped in the cells to minimize space usage
	setWordWrap(true);

	// user can select several items at a time
	setSelectionMode(QAbstractItemView::ContiguousSelection);

	setupActions();

	setMinimumHeight(MINIMUM_TABLE_HEIGHT);
}

//-----------------------------------------------------------------------------
// Function: PinMappingView::~PinMappingView()
//-----------------------------------------------------------------------------
PinMappingView::~PinMappingView() {
}

//-----------------------------------------------------------------------------
// Function: PinMappingView::keyPressEvent()
//-----------------------------------------------------------------------------
void PinMappingView::keyPressEvent( QKeyEvent* event ) {

	// call the base class implementation
	QTableView::keyPressEvent(event);

	if (event->matches(QKeySequence::Delete)) {
		onClearAction();
	}
}

//-----------------------------------------------------------------------------
// Function: PinMappingView::mousePressEvent()
//-----------------------------------------------------------------------------
void PinMappingView::mousePressEvent( QMouseEvent* event ) {
	pressedPoint_ = event->pos();

	// if user clicked area that has no item
	QModelIndex pressedIndex = indexAt(pressedPoint_);
	if (!pressedIndex.isValid()) {
		setCurrentIndex(pressedIndex);
	}

	QTableView::mousePressEvent(event);
}

//-----------------------------------------------------------------------------
// Function: PinMappingView::mouseReleaseEvent()
//-----------------------------------------------------------------------------
void PinMappingView::mouseReleaseEvent( QMouseEvent* event ) {
	setCursor(QCursor(Qt::ArrowCursor));
	QTableView::mouseReleaseEvent(event);
}

//-----------------------------------------------------------------------------
// Function: PinMappingView::contextMenuEvent()
//-----------------------------------------------------------------------------
void PinMappingView::contextMenuEvent( QContextMenuEvent* event ) {
	pressedPoint_ = event->pos();

	QModelIndex index = indexAt(pressedPoint_);

	QMenu menu(this);

	// if at least one valid item is selected
	if (index.isValid()) {
		menu.addAction(&clearAction_);		
	}	

	menu.exec(event->globalPos());

	event->accept();
}


//-----------------------------------------------------------------------------
// Function: PinMappingView::dragEnterEvent()
//-----------------------------------------------------------------------------
void PinMappingView::dragEnterEvent(QDragEnterEvent *event)
{
    // Accept drops only from physical port list.
    PhysListView* physView = dynamic_cast<PhysListView*>(event->source());
    if (physView != 0)
    {
       event->accept();
    }
}


//-----------------------------------------------------------------------------
// Function: PinMappingView::setupActions()
//-----------------------------------------------------------------------------
void PinMappingView::setupActions() {

	clearAction_.setToolTip(tr("Clear the contents of a cell"));
	clearAction_.setStatusTip(tr("Clear the contents of a cell"));
	connect(&clearAction_, SIGNAL(triggered()),
		this, SLOT(onClearAction()), Qt::UniqueConnection);
	clearAction_.setShortcut(QKeySequence::Delete);

}

//-----------------------------------------------------------------------------
// Function: PinMappingView::onClearAction()
//-----------------------------------------------------------------------------
void PinMappingView::onClearAction() {
	
	QModelIndexList indexes = selectedIndexes();

	// clear the contents of each cell
	foreach (QModelIndex index, indexes) {
		model()->setData(index, QVariant(), Qt::EditRole);
	}
}



//-----------------------------------------------------------------------------
// Function: PinMappingView::setModel()
//-----------------------------------------------------------------------------
void PinMappingView::setModel( QAbstractItemModel* model ) {
	
	// the base class implementation does most of the work
	QTableView::setModel(model);

	// contains info on the used font
	QFontMetrics fMetrics = fontMetrics();

	// set the widths for the columns
	int columnCount = model->columnCount(QModelIndex());
	for (int i = 0; i < columnCount; ++i) {

		// the width required by the contents of the model
		int contentSize = sizeHintForColumn(i);

		// get the header for the section
		QString headerText = model->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString();
		// if the header contains several lines
		QStringList headerLines = headerText.split("\n", QString::SkipEmptyParts);
		int headerSize = 0;

		// find the line that needs most space
		foreach (QString headerLine, headerLines) {
			headerSize = qMax(headerSize, fMetrics.width(headerLine));
		}
        headerSize += 45;

        // set the width for the column
        setColumnWidth(i, qMax(contentSize, headerSize));
    }
}

