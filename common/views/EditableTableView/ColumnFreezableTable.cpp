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

static const int MAXIMUMFROZENWIDTH_ = 200;

//-----------------------------------------------------------------------------
// Function: ColumnFreezableTable::ColumnFreezableTable()
//-----------------------------------------------------------------------------
ColumnFreezableTable::ColumnFreezableTable(int numberOfFrozenColumns, 
    QSharedPointer<EditableTableView> frozenColumns, QWidget* parent):
EditableTableView(parent),
frozenColumns_(frozenColumns),
numberOfFrozenColumns_(numberOfFrozenColumns)
{
    frozenColumns_->setParent(this);

    frozenColumns_->verticalHeader()->hide();

    frozenColumns_->horizontalHeader()->setMaximumSectionSize(MAXIMUMFROZENWIDTH_);
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

    connect(frozenColumns_->horizontalHeader(), SIGNAL(sectionClicked(int)),
        this, SLOT(closeSortingSectionInMainEditor()));
    connect(horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(closeSortingSectionInFrozenColumn()));
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
// Function: ColumnFreezableTable::viewportEvent()
//-----------------------------------------------------------------------------
bool ColumnFreezableTable::viewportEvent(QEvent* event)
{
    resizeEditorMargins();

    return QAbstractItemView::viewportEvent(event);
}


//-----------------------------------------------------------------------------
// Function: ColumnFreezableTable::resizeEditorMargins()
//-----------------------------------------------------------------------------
void ColumnFreezableTable::resizeEditorMargins()
{
    frozenColumns_->resizeColumnsToContents();

    updateColumnFreezableTableGeometry();

    int frozenWidth = frozenColumns_->width();
    int horizontalHeight = horizontalHeader()->height();

    if (frozenWidth > MAXIMUMFROZENWIDTH_ * numberOfFrozenColumns_)
    {
        frozenWidth = MAXIMUMFROZENWIDTH_ * numberOfFrozenColumns_;
    }

    setViewportMargins(frozenWidth, horizontalHeight, 0, 0);

    frozenColumns_->move(0,0);
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
    
    frozenColumns_->resizeColumnsToContents();

    frozenColumns_->horizontalHeader()->setMinimumHeight(horizontalHeader()->height());
    frozenColumns_->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    frozenColumns_->setFocusPolicy(Qt::NoFocus);

    frozenColumns_->setSelectionModel(selectionModel());
    for (int col = numberOfFrozenColumns_; col < model()->columnCount(); col++)
    {
        frozenColumns_->hideColumn(col);
    }

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
        totalWidth += frozenColumns_->columnWidth(col);
    }

    totalWidth += numberOfFrozenColumns_;

    totalWidth = totalWidth + verticalHeader()->width();

    if (totalWidth > MAXIMUMFROZENWIDTH_ * numberOfFrozenColumns_)
    {
        totalWidth = MAXIMUMFROZENWIDTH_ * numberOfFrozenColumns_;
    }

    frozenColumns_->setGeometry(pos().x(), pos().y(), totalWidth,
        viewport()->height() + horizontalHeader()->height());
}

//-----------------------------------------------------------------------------
// Function: ColumnFreezableTable::closeSortingSectionInFrozenColumn()
//-----------------------------------------------------------------------------
void ColumnFreezableTable::closeSortingSectionInFrozenColumn()
{
    horizontalHeader()->setSortIndicatorShown(true);

    frozenColumns_->horizontalHeader()->setSortIndicatorShown(false);
}

//-----------------------------------------------------------------------------
// Function: ColumnFreezableTable::closeSortingSectionInMainEditor()
//-----------------------------------------------------------------------------
void ColumnFreezableTable::closeSortingSectionInMainEditor()
{
    frozenColumns_->horizontalHeader()->setSortIndicatorShown(true);

    horizontalHeader()->setSortIndicatorShown(false);
}
