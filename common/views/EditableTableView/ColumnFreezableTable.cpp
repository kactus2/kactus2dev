//-----------------------------------------------------------------------------
// File: ColumnFreezableTable.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 25.02.2015
//
// Description:
// Editable table view, with a possibility for freezing a column in place.
//-----------------------------------------------------------------------------

#include "ColumnFreezableTable.h"
#include <QHeaderView>
#include <QScrollBar>

//-----------------------------------------------------------------------------
// Function: ColumnFreezableTable::ColumnFreezableTable()
//-----------------------------------------------------------------------------
ColumnFreezableTable::ColumnFreezableTable(QWidget *parent):
EditableTableView(parent),
freezedColumns_(parent)
{

}

//-----------------------------------------------------------------------------
// Function: ColumnFreezableTable::~ColumnFreezableTable()
//-----------------------------------------------------------------------------
ColumnFreezableTable::~ColumnFreezableTable()
{

}

//-----------------------------------------------------------------------------
// Function: ColumnFreezableTable::setModel()
//-----------------------------------------------------------------------------
void ColumnFreezableTable::setModel(QAbstractItemModel* model)
{
    QTableView::setModel(model);

    init();

    connect(horizontalHeader(), SIGNAL(sectionResized(int, int, int)),
        this, SLOT(updateSectionWidth(int, int, int)));
    connect(verticalHeader(), SIGNAL(sectionResized(int, int, int)),
        this, SLOT(updateSectionHeight(int, int, int)));

    connect(freezedColumns_.verticalScrollBar(), SIGNAL(valueChanged(int)),
        verticalScrollBar(), SLOT(setValue(int)));
    connect(verticalScrollBar(), SIGNAL(valueChanged(int)),
        freezedColumns_.verticalScrollBar(), SLOT(setValue(int)));

    connect(&freezedColumns_, SIGNAL(addItem(const QModelIndex&)),
        this, SIGNAL(addItem(const QModelIndex&)), Qt::UniqueConnection);
    connect(&freezedColumns_, SIGNAL(removeItem(const QModelIndex&)),
        this, SIGNAL(removeItem(const QModelIndex&)), Qt::UniqueConnection);
    connect(&freezedColumns_, SIGNAL(moveItem(const QModelIndex&, const QModelIndex&)),
        this, SIGNAL(moveItem(const QModelIndex&, const QModelIndex&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ColumnFreezableTable::setAllowImportExport()
//-----------------------------------------------------------------------------
void ColumnFreezableTable::setAllowImportExport(bool allow)
{
    EditableTableView::setAllowImportExport(allow);
    freezedColumns_.setAllowImportExport(allow);
}

//-----------------------------------------------------------------------------
// Function: ColumnFreezableTable::setDefaultImportExportPath()
//-----------------------------------------------------------------------------
void ColumnFreezableTable::setDefaultImportExportPath(const QString& path)
{
    EditableTableView::setDefaultImportExportPath(path);
    freezedColumns_.setDefaultImportExportPath(path);
}

//-----------------------------------------------------------------------------
// Function: ColumnFreezableTable::setDelegate()
//-----------------------------------------------------------------------------
void ColumnFreezableTable::setDelegate(QAbstractItemDelegate *delegateItem)
{
    setItemDelegate(delegateItem);
    freezedColumns_.setItemDelegate(delegateItem);
}

//-----------------------------------------------------------------------------
// Function: ColumnFreezableTable::resizeEvent()
//-----------------------------------------------------------------------------
void ColumnFreezableTable::resizeEvent(QResizeEvent *event)
{
    QTableView::resizeEvent(event);
    updateColumnFreezableTableGeometry();
}

//-----------------------------------------------------------------------------
// Function: ColumnFreezableTable::moveCursor()
//-----------------------------------------------------------------------------
QModelIndex ColumnFreezableTable::moveCursor(CursorAction cursorAction, Qt::KeyboardModifier modifiers)
{
    QModelIndex current = QTableView::moveCursor(cursorAction, modifiers);

    if (cursorAction == MoveLeft && current.column() > 0 && visualRect(current).topLeft().x() <
        freezedColumns_.columnWidth(0))
    {
        const int newValue = horizontalScrollBar()->value() + visualRect(current).topLeft().x() -
            freezedColumns_.columnWidth(0);
        horizontalScrollBar()->setValue(newValue);
    }

    return current;
}

//-----------------------------------------------------------------------------
// Function: ColumnFreezableTable::scrollTo()
//-----------------------------------------------------------------------------
void ColumnFreezableTable::scrollTo(const QModelIndex &index, QAbstractItemView::ScrollHint hint /* = EnsureVisible */)
{
    if (index.column() > 0)
    {
        QTableView::scrollTo(index, hint);
    }
}

//-----------------------------------------------------------------------------
// Function: ColumnFreezableTable::init()
//-----------------------------------------------------------------------------
void ColumnFreezableTable::init()
{
    freezedColumns_.horizontalHeader()->setSectionsClickable(true);
    freezedColumns_.horizontalHeader()->setStretchLastSection(true);
    freezedColumns_.setSortingEnabled(true);
    freezedColumns_.setItemsDraggable(false);
    freezedColumns_.sortByColumn(0, Qt::AscendingOrder);

    freezedColumns_.setMinimumHeight(minimumHeight() - horizontalScrollBar()->height());

    freezedColumns_.setModel(model());
    
    freezedColumns_.horizontalHeader()->setMinimumHeight(horizontalHeader()->height());
    freezedColumns_.horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    freezedColumns_.setFocusPolicy(Qt::NoFocus);

    viewport()->stackUnder(&freezedColumns_);

    freezedColumns_.setSelectionModel(selectionModel());
    for (int col=1; col < model()->columnCount(); col++)
    {
        freezedColumns_.setColumnHidden(col, true);
    }

    freezedColumns_.setColumnWidth(0, columnWidth(0));

    freezedColumns_.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    freezedColumns_.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    freezedColumns_.show();

    updateColumnFreezableTableGeometry();
}

//-----------------------------------------------------------------------------
// Function: ColumnFreezableTable::updateColumnFreezableTableGeometry()
//-----------------------------------------------------------------------------
void ColumnFreezableTable::updateColumnFreezableTableGeometry()
{
    freezedColumns_.setGeometry(pos().x(), pos().y(), columnWidth(0),
        viewport()->height() + horizontalHeader()->height());
}

//-----------------------------------------------------------------------------
// Function: ColumnFreezableTable::updateSectionWidth()
//-----------------------------------------------------------------------------
void ColumnFreezableTable::updateSectionWidth(int logicalIndex, int, int newSize)
{
    if (logicalIndex == 0)
    {
        freezedColumns_.setColumnWidth(0, newSize);
        updateColumnFreezableTableGeometry();
    }
}

//-----------------------------------------------------------------------------
// Function: ColumnFreezableTable::updateSectionHeight()
//-----------------------------------------------------------------------------
void ColumnFreezableTable::updateSectionHeight(int logicalIndex, int, int newSize)
{
    freezedColumns_.setRowHeight(logicalIndex, newSize);
}
