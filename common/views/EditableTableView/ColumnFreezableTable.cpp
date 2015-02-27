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
ColumnFreezableTable::ColumnFreezableTable(int numberOfFrozenColumns, QSharedPointer<EditableTableView> frozenColumns, QWidget* parent):
EditableTableView(parent),
frozenColumns_(frozenColumns),
numberOfFrozenColumns_(numberOfFrozenColumns)
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

    connect(frozenColumns_->verticalScrollBar(), SIGNAL(valueChanged(int)),
        verticalScrollBar(), SLOT(setValue(int)));
    connect(verticalScrollBar(), SIGNAL(valueChanged(int)),
        frozenColumns_->verticalScrollBar(), SLOT(setValue(int)));

    connect(frozenColumns_.data(), SIGNAL(addItem(const QModelIndex&)),
        this, SIGNAL(addItem(const QModelIndex&)), Qt::UniqueConnection);
    connect(frozenColumns_.data(), SIGNAL(removeItem(const QModelIndex&)),
        this, SIGNAL(removeItem(const QModelIndex&)), Qt::UniqueConnection);
    connect(frozenColumns_.data(), SIGNAL(moveItem(const QModelIndex&, const QModelIndex&)),
        this, SIGNAL(moveItem(const QModelIndex&, const QModelIndex&)), Qt::UniqueConnection);

    connect(frozenColumns_->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(closeSortingSection(int)));
    connect(horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(closeSortingSection(int)));
}

//-----------------------------------------------------------------------------
// Function: ColumnFreezableTable::setAllowImportExport()
//-----------------------------------------------------------------------------
void ColumnFreezableTable::setAllowImportExport(bool allow)
{
    EditableTableView::setAllowImportExport(allow);
    frozenColumns_->setAllowImportExport(allow);
}

//-----------------------------------------------------------------------------
// Function: ColumnFreezableTable::setDefaultImportExportPath()
//-----------------------------------------------------------------------------
void ColumnFreezableTable::setDefaultImportExportPath(const QString& path)
{
    EditableTableView::setDefaultImportExportPath(path);
    frozenColumns_->setDefaultImportExportPath(path);
}

//-----------------------------------------------------------------------------
// Function: ColumnFreezableTable::setDelegate()
//-----------------------------------------------------------------------------
void ColumnFreezableTable::setDelegate(QAbstractItemDelegate *delegateItem)
{
    setItemDelegate(delegateItem);
    frozenColumns_->setItemDelegate(delegateItem);
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

    int totalWidth = 0;
    for (int col = 0; col < numberOfFrozenColumns_; ++col)
    {
        totalWidth += frozenColumns_->columnWidth(col);
    }

    totalWidth += numberOfFrozenColumns_;

    if (cursorAction == MoveLeft && current.column() > 0 && visualRect(current).topLeft().x() <
        totalWidth)
    {
        const int newValue = horizontalScrollBar()->value() + visualRect(current).topLeft().x() - totalWidth;
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
    frozenColumns_->horizontalHeader()->setSectionsClickable(true);
    frozenColumns_->horizontalHeader()->setStretchLastSection(true);
    frozenColumns_->setSortingEnabled(true);
    frozenColumns_->setItemsDraggable(false);
    frozenColumns_->sortByColumn(0, Qt::AscendingOrder);

    frozenColumns_->setMinimumHeight(minimumHeight() - horizontalScrollBar()->height());

    frozenColumns_->setModel(model());
    
    resizeColumnsToContents();
    frozenColumns_->resizeColumnsToContents();

    frozenColumns_->horizontalHeader()->setMinimumHeight(horizontalHeader()->height());
    frozenColumns_->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    frozenColumns_->setFocusPolicy(Qt::NoFocus);

    stackUnder(frozenColumns_.data());

    frozenColumns_->setSelectionModel(selectionModel());
    for (int col=numberOfFrozenColumns_; col < model()->columnCount(); col++)
    {
        frozenColumns_->setColumnHidden(col, true);
    }

    frozenColumns_->setColumnWidth(0, columnWidth(0));

    frozenColumns_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    frozenColumns_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    frozenColumns_->show();

    updateColumnFreezableTableGeometry();
}

//-----------------------------------------------------------------------------
// Function: ColumnFreezableTable::updateColumnFreezableTableGeometry()
//-----------------------------------------------------------------------------
void ColumnFreezableTable::updateColumnFreezableTableGeometry()
{
    int totalWidth = 0;
    for (int col = 0; col < numberOfFrozenColumns_; ++col)
    {
        totalWidth += columnWidth(col);
    }

    totalWidth += numberOfFrozenColumns_;

    frozenColumns_->setGeometry(pos().x(), pos().y(), totalWidth,
        viewport()->height() + horizontalHeader()->height());
}

//-----------------------------------------------------------------------------
// Function: ColumnFreezableTable::updateSectionWidth()
//-----------------------------------------------------------------------------
void ColumnFreezableTable::updateSectionWidth(int logicalIndex, int, int newSize)
{
    if (logicalIndex < numberOfFrozenColumns_)
    {
        frozenColumns_->setColumnWidth(numberOfFrozenColumns_ - 1, newSize);
        updateColumnFreezableTableGeometry();
    }
}

//-----------------------------------------------------------------------------
// Function: ColumnFreezableTable::updateSectionHeight()
//-----------------------------------------------------------------------------
void ColumnFreezableTable::updateSectionHeight(int logicalIndex, int, int newSize)
{
    frozenColumns_->setRowHeight(logicalIndex, newSize);
}

//-----------------------------------------------------------------------------
// Function: ColumnFreezableTable::closeSortingSection()
//-----------------------------------------------------------------------------
void ColumnFreezableTable::closeSortingSection(int logicalIndex)
{
    horizontalHeader()->setSortIndicatorShown(logicalIndex >= numberOfFrozenColumns_);
    frozenColumns_->horizontalHeader()->setSortIndicatorShown(logicalIndex < numberOfFrozenColumns_);
}