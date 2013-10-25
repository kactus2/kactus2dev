//-----------------------------------------------------------------------------
// File: BitMappingView.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 3.10.2013
//
// Description:
// View for bit-field mapping.
//-----------------------------------------------------------------------------

#include "BitMappingView.h"

#include "BitMappingModel.h"
#include "physlistview.h"
#include <LibraryManager/vlnv.h>
#include <models/generaldeclarations.h>
#include <IPXactWrapper/ComponentEditor/busInterfaces/portmaps/BitSelectionDialog.h>

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

Q_DECLARE_METATYPE(General::PortBounds)

//-----------------------------------------------------------------------------
// Function: BitMappingView::BitMappingView()
//-----------------------------------------------------------------------------
BitMappingView::BitMappingView(QSharedPointer<Component> component, QWidget *parent)
    : component_(component),
      QTableView(parent),
      pressedPoint_(),
      dropPoint_(),
      droppedData_(0),
      clearAction_(tr("Clear"), this),
      selectBitsAction_(tr("Select bits to map"), this),
    logicalPort_()
{
    Q_ASSERT(component_);
    
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
// Function: BitMappingView::~BitMappingView()
//-----------------------------------------------------------------------------
BitMappingView::~BitMappingView() {
}

//-----------------------------------------------------------------------------
// Function: BitMappingView::keyPressEvent()
//-----------------------------------------------------------------------------
void BitMappingView::keyPressEvent( QKeyEvent* event ) {

	// call the base class implementation
	QTableView::keyPressEvent(event);

	if (event->matches(QKeySequence::Delete)) {
		onClearAction();
	}
}

//-----------------------------------------------------------------------------
// Function: BitMappingView::mousePressEvent()
//-----------------------------------------------------------------------------
void BitMappingView::mousePressEvent( QMouseEvent* event ) {
	pressedPoint_ = event->pos();

	// if user clicked area that has no item
	QModelIndex pressedIndex = indexAt(pressedPoint_);
	if (!pressedIndex.isValid()) {
		setCurrentIndex(pressedIndex);
	}

	QTableView::mousePressEvent(event);
}

//-----------------------------------------------------------------------------
// Function: BitMappingView::mouseReleaseEvent()
//-----------------------------------------------------------------------------
void BitMappingView::mouseReleaseEvent( QMouseEvent* event ) {
	setCursor(QCursor(Qt::ArrowCursor));
	QTableView::mouseReleaseEvent(event);
}

//-----------------------------------------------------------------------------
// Function: BitMappingView::contextMenuEvent()
//-----------------------------------------------------------------------------
void BitMappingView::contextMenuEvent( QContextMenuEvent* event ) {
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
// Function: BitMappingView::dragEnterEvent()
//-----------------------------------------------------------------------------
void BitMappingView::dragEnterEvent(QDragEnterEvent *event)
{
    // Accept drops only from physical port list.
    PhysListView* physView = dynamic_cast<PhysListView*>(event->source());
    if (physView != 0)
    {
        event->accept();
    }
}

//-----------------------------------------------------------------------------
// Function: BitMappingView::setupActions()
//-----------------------------------------------------------------------------
void BitMappingView::setupActions() {

	clearAction_.setToolTip(tr("Clear the contents of a cell"));
	clearAction_.setStatusTip(tr("Clear the contents of a cell"));
	connect(&clearAction_, SIGNAL(triggered()),
		this, SLOT(onClearAction()), Qt::UniqueConnection);
	clearAction_.setShortcut(QKeySequence::Delete);

    selectBitsAction_.setToolTip(tr("Select physical bits to map"));
    selectBitsAction_.setStatusTip(tr("Select physical bits to map"));
    connect(&selectBitsAction_, SIGNAL(triggered()),
        this, SLOT(onSelectBitsAction()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: BitMappingView::onClearAction()
//-----------------------------------------------------------------------------
void BitMappingView::onClearAction() {
	
	QModelIndexList indexes = selectedIndexes();

	// clear the contents of each cell
	foreach (QModelIndex index, indexes) {
		model()->setData(index, QVariant(), Qt::EditRole);
	}
}


//-----------------------------------------------------------------------------
// Function: BitMappingView::onSelectBitsAction()
//-----------------------------------------------------------------------------
void BitMappingView::onSelectBitsAction()
{
    if (droppedData_ != 0 && droppedData_->hasText())
    {
        QModelIndex bitIndex = model()->index(indexAt(dropPoint_).row(), BitMappingModel::INDEX);
        int targetBit = model()->data(bitIndex).toInt();

        foreach (QString portName, droppedData_->text().split(';', QString::SkipEmptyParts))
        {
            if (component_->hasPort(portName))
            {
                int physLeft = component_->getPortLeftBound(portName);
                int physRight = component_->getPortRightBound(portName);
                int physSize = abs(physLeft - physRight) + 1;
                int logicalBitsLeft = model()->rowCount() - targetBit;
                BitSelectionDialog dialog(logicalPort_, targetBit, portName, physSize, logicalBitsLeft, this);
                if (dialog.exec() == QDialog::Accepted)
                {
                    physLeft = dialog.getHigherBound();
                    physRight = dialog.getLowerBound();
                    physSize = abs(physLeft - physRight) + 1;

                    General::PortBounds bound(portName, physLeft, physRight);
                    model()->setData(model()->index(targetBit, BitMappingModel::BIT), QVariant::fromValue(bound));
                    targetBit += physSize;
                }
            }
        }    
    }
}

//-----------------------------------------------------------------------------
// Function: BitMappingView::setModel()
//-----------------------------------------------------------------------------
void BitMappingView::setModel( QAbstractItemModel* model ) {
	
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


//-----------------------------------------------------------------------------
// Function: BitMappingView::onLogicalPortChanged()
//-----------------------------------------------------------------------------
void BitMappingView::onLogicalPortChanged(QString const& portName)
{
    logicalPort_ = portName;
}

//-----------------------------------------------------------------------------
// Function: BitMappingView::dropEvent()
//-----------------------------------------------------------------------------
void BitMappingView::dropEvent(QDropEvent *event)
{
    // If drag-drop with right mouse button, open menu.
    if (event->mouseButtons() == Qt::RightButton)
    {
        dropPoint_ = event->pos();
        QModelIndex index = indexAt(dropPoint_);
        QMenu menu(this);

        if (index.isValid()) {
            menu.addAction(&selectBitsAction_);		
        }	
        if (event->mimeData()->hasText())
        {
            droppedData_ = event->mimeData(); 
            menu.exec(mapToGlobal(event->pos() + QPoint(0, horizontalHeader()->height())));
        }
        event->accept();
    }
    else
    {
        QTableView::dropEvent(event);
    }
}

