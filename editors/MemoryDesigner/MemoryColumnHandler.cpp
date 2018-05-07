//-----------------------------------------------------------------------------
// File: MemoryColumnHandler.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 02.01.2017
//
// Description:
// Constructs the columns for the memory design diagram.
//-----------------------------------------------------------------------------

#include "MemoryColumnHandler.h"

#include <editors/MemoryDesigner/MemoryColumn.h>
#include <editors/MemoryDesigner/MemoryDesignerConstants.h>

//-----------------------------------------------------------------------------
// Function: MemoryColumnHandler::MemoryColumnHandler()
//-----------------------------------------------------------------------------
MemoryColumnHandler::MemoryColumnHandler(QSharedPointer<GraphicsColumnLayout> layout):
spaceColumnWidth_(MemoryDesignerConstants::SPACECOLUMNWIDTH),
memoryMapColumnWidth_(MemoryDesignerConstants::MEMORYMAPCOLUMNWIDTH),
layout_(layout)
{

}

//-----------------------------------------------------------------------------
// Function: MemoryColumnHandler::~MemoryColumnHandler()
//-----------------------------------------------------------------------------
MemoryColumnHandler::~MemoryColumnHandler()
{

}

//-----------------------------------------------------------------------------
// Function: MemoryColumnHandler::setNewMemoryMapColumnWidth()
//-----------------------------------------------------------------------------
void MemoryColumnHandler::setNewMemoryMapColumnWidth(int newMapColumnWidth)
{
    memoryMapColumnWidth_ = newMapColumnWidth;
}

//-----------------------------------------------------------------------------
// Function: MemoryColumnHandler::getMemoryMapColumnWidth()
//-----------------------------------------------------------------------------
int MemoryColumnHandler::getMemoryMapColumnWidth() const
{
    return memoryMapColumnWidth_;
}

//-----------------------------------------------------------------------------
// Function: MemoryColumnHandler::setNewLayout()
//-----------------------------------------------------------------------------
void MemoryColumnHandler::setNewLayout(QSharedPointer<GraphicsColumnLayout> newLayout)
{
    layout_ = newLayout;
}

//-----------------------------------------------------------------------------
// Function: MemoryColumnHandler::getMemoryColumns()
//-----------------------------------------------------------------------------
QVector<MemoryColumn*> MemoryColumnHandler::getMemoryColumns() const
{
    QVector<MemoryColumn*> columns;

    foreach (GraphicsColumn* currentColumn, layout_->getColumns())
    {
        MemoryColumn* memoryColumn = dynamic_cast<MemoryColumn*>(currentColumn);
        if (memoryColumn)
        {
            columns.append(memoryColumn);
        }
    }

    return columns;
}

//-----------------------------------------------------------------------------
// Function: MemoryColumnHandler::getAddressSpaceColumns()
//-----------------------------------------------------------------------------
QVector<MemoryColumn*> MemoryColumnHandler::getAddressSpaceColumns() const
{
    QVector<MemoryColumn*> addressSpaceColumns =
        getSpecifiedColumns(MemoryDesignerConstants::ADDRESSSPACECOLUMN_NAME);
    return addressSpaceColumns;
}

//-----------------------------------------------------------------------------
// Function: MemoryColumnHandler::getMemoryMapColumns()
//-----------------------------------------------------------------------------
QVector<MemoryColumn*> MemoryColumnHandler::getMemoryMapColumns() const
{
    QVector<MemoryColumn*> memoryColumns =
        getSpecifiedColumns(MemoryDesignerConstants::MEMORYMAPCOLUMNCOMMON_NAME);
    return memoryColumns;
}

//-----------------------------------------------------------------------------
// Function: MemoryColumnHandler::getMapOverlapColumns()
//-----------------------------------------------------------------------------
QVector<MemoryColumn*> MemoryColumnHandler::getMapOverlapColumns() const
{
    QVector<MemoryColumn*> overlapColumns =
        getSpecifiedColumns(MemoryDesignerConstants::MEMORYMAPOVERLAPCOLUMN_NAME);
    return overlapColumns;
}

//-----------------------------------------------------------------------------
// Function: MemoryColumnHandler::findColumnAtPosition()
//-----------------------------------------------------------------------------
MemoryColumn* MemoryColumnHandler::findColumnAtPosition(QPointF position) const
{
    MemoryColumn* memoryColumnAtPosition = 0;

    GraphicsColumn* columnAtPosition = layout_->findColumnAt(position);
    if (columnAtPosition)
    {
        memoryColumnAtPosition = dynamic_cast<MemoryColumn*>(columnAtPosition);
    }

    return memoryColumnAtPosition;
}

//-----------------------------------------------------------------------------
// Function: MemoryColumnHandler::removeColumnFromLayout()
//-----------------------------------------------------------------------------
void MemoryColumnHandler::removeColumnFromLayout(MemoryColumn* selectedColumn)
{
    layout_->removeColumn(selectedColumn);
}

//-----------------------------------------------------------------------------
// Function: MemoryColumnHandler::createInitialColumns()
//-----------------------------------------------------------------------------
void MemoryColumnHandler::createInitialColumns()
{
    createAddressSpaceColumn();
    createMemoryColumns();
}

//-----------------------------------------------------------------------------
// Function: MemoryColumnHandler::createAddressSpaceColumn()
//-----------------------------------------------------------------------------
MemoryColumn* MemoryColumnHandler::createAddressSpaceColumn()
{
    QString spaceName = MemoryDesignerConstants::ADDRESSSPACECOLUMN_NAME;

    QPointF columnPosition (0,0);

    int spaceCounter = 0;
    foreach (GraphicsColumn* column, layout_->getColumns())
    {
        if (column->name().contains(spaceName, Qt::CaseInsensitive))
        {
            spaceCounter += 1;
        }
    }
    if (spaceCounter != 0)
    {
        spaceName.append(QStringLiteral("_") + QString::number(spaceCounter));
    }

    QSharedPointer<ColumnDesc> addressSpaceColumnDescription(new ColumnDesc());
    addressSpaceColumnDescription->setName(spaceName);
    addressSpaceColumnDescription->setMinimumWidth(spaceColumnWidth_);
    addressSpaceColumnDescription->setWidth(spaceColumnWidth_);

    MemoryColumn* spaceColumn(new MemoryColumn(addressSpaceColumnDescription, layout_.data(), 0));
    layout_->addColumn(spaceColumn);

    spaceColumn->setPos(columnPosition);

    layout_->onMoveColumn(spaceColumn);
    layout_->onReleaseColumn(spaceColumn);

    return spaceColumn;
}

//-----------------------------------------------------------------------------
// Function: MemoryColumnHandler::createMemoryColumns()
//-----------------------------------------------------------------------------
void MemoryColumnHandler::createMemoryColumns()
{
    QPointF columnPlacement (0,0);
    foreach (GraphicsColumn* column, layout_->getColumns())
    {
        columnPlacement.setX(columnPlacement.x() + column->boundingRect().width());
    }

    QSharedPointer<ColumnDesc> memoryMapColumnDescription(new ColumnDesc());
    memoryMapColumnDescription->setName(MemoryDesignerConstants::MEMORYMAPCOLUM_NAME);
    memoryMapColumnDescription->setMinimumWidth(memoryMapColumnWidth_);
    memoryMapColumnDescription->setWidth(memoryMapColumnWidth_);

    MemoryColumn* memoryMapColumn(new MemoryColumn(memoryMapColumnDescription, layout_.data(), 0));

    memoryMapColumn->setPos(columnPlacement);
    layout_->addColumn(memoryMapColumn);

    createMemoryOverlapColumn();
}

//-----------------------------------------------------------------------------
// Function: MemoryColumnHandler::createMemoryOverlapColumn()
//-----------------------------------------------------------------------------
MemoryColumn* MemoryColumnHandler::createMemoryOverlapColumn()
{
    QString overlapName = MemoryDesignerConstants::MEMORYMAPOVERLAPCOLUMN_NAME;

    int overlapCounter = 0;
    int columnXPosition = 0;
    foreach (GraphicsColumn* column, layout_->getColumns())
    {
        if (column->name().contains(overlapName))
        {
            overlapCounter += 1;
        }

        columnXPosition += column->boundingRect().width();
    }

    if (overlapCounter > 0)
    {
        QString overlapNameExtension = QStringLiteral("_") + QString::number(overlapCounter);
        overlapName.append(overlapNameExtension);
    }

    QSharedPointer<ColumnDesc> memoryMapOverlapColumnDescription(new ColumnDesc());
    memoryMapOverlapColumnDescription->setName(overlapName);
    memoryMapOverlapColumnDescription->setMinimumWidth(memoryMapColumnWidth_);
    memoryMapOverlapColumnDescription->setWidth(memoryMapColumnWidth_);

    MemoryColumn* memoryMapOverlapColumn(new MemoryColumn(memoryMapOverlapColumnDescription, layout_.data(), 0));
    QPointF columnPosition (columnXPosition, 0);
    memoryMapOverlapColumn->setPos(columnPosition);
    layout_->addColumn(memoryMapOverlapColumn);

    return memoryMapOverlapColumn;
}

//-----------------------------------------------------------------------------
// Function: MemoryColumnHandler::findColumnByName()
//-----------------------------------------------------------------------------
MemoryColumn* MemoryColumnHandler::findColumnByName(QString const& columnName) const
{
    foreach (GraphicsColumn* column, layout_->getColumns())
    {
        MemoryColumn* memoryColumn = dynamic_cast<MemoryColumn*>(column);
        if (memoryColumn && memoryColumn->name().compare(columnName) == 0)
        {
            return memoryColumn;
        }
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: MemoryColumnHandler::getSpecifiedColumns()
//-----------------------------------------------------------------------------
QVector<MemoryColumn*> MemoryColumnHandler::getSpecifiedColumns(QString const& columnSpecification) const
{
    QVector<MemoryColumn*> foundColumns;
    
    foreach (GraphicsColumn* column, layout_->getColumns())
    {
        MemoryColumn* currentColumn = dynamic_cast<MemoryColumn*>(column);
        if (currentColumn && currentColumn->name().contains(columnSpecification, Qt::CaseInsensitive))
        {
            foundColumns.append(currentColumn);
        }
    }

    return foundColumns;
}

//-----------------------------------------------------------------------------
// Function: MemoryColumnHandler::changeColumnWidths()
//-----------------------------------------------------------------------------
void MemoryColumnHandler::changeColumnWidths(qreal deltaWidth)
{
    foreach (GraphicsColumn* column, layout_->getColumns())
    {
        MemoryColumn* memoryColumn = dynamic_cast<MemoryColumn*>(column);
        if (memoryColumn && memoryColumn->containsMemoryMapItems())
        {
            memoryColumn->changeWidth(deltaWidth);
        }
    }
}
