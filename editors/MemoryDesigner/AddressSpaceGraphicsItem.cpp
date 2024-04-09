//-----------------------------------------------------------------------------
// File: AddressSpaceGraphicsItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 08.07.2016
//
// Description:
// Graphics item for visualizing an address space in the memory designer.
//-----------------------------------------------------------------------------

#include "AddressSpaceGraphicsItem.h"

#include <common/KactusColors.h>

#include <editors/MemoryDesigner/MemoryItem.h>
#include <editors/MemoryDesigner/ConnectivityComponent.h>
#include <editors/MemoryDesigner/AddressSegmentGraphicsItem.h>
#include <editors/MemoryDesigner/MemoryDesignerConstants.h>
#include <editors/MemoryDesigner/MemoryConnectionItem.h>
#include <editors/MemoryDesigner/MemoryExtensionGraphicsItem.h>

#include <QBrush>
#include <QFont>
#include <QPen>
#include <QtMath>

namespace AddressSpaceItemConstants
{
    const int MINIMUMSUBITEMHEIGHT = 3 * MemoryDesignerConstants::RANGEINTERVAL;
};

//-----------------------------------------------------------------------------
// Function: AddressSpaceGraphicsItem::AddressSpaceGraphicsItem()
//-----------------------------------------------------------------------------
AddressSpaceGraphicsItem::AddressSpaceGraphicsItem(QSharedPointer<MemoryItem const> memoryItem,
    QVector<QString> identifierChain, QSharedPointer<ConnectivityComponent const> containingInstance,
    bool filterSegments, QGraphicsItem* parent):
MainMemoryGraphicsItem(memoryItem, containingInstance, MemoryDesignerConstants::ADDRESSSEGMENT_TYPE,
    filterSegments, identifierChain, parent),
cpuIcon_(new QGraphicsPixmapItem(QPixmap(":icons/common/graphics/settings-code_editor.png"), this))
{
    QBrush addressSpaceBrush(KactusColors::ADDRESS_SEGMENT);
    setBrush(addressSpaceBrush);

    quint64 spaceRangeInt = memoryItem->getRange().toULongLong();
    int spaceWidth = 250;

    setGraphicsRectangle(spaceWidth + 1, spaceRangeInt);

    setupGraphicsItem(0, spaceRangeInt - 1, QStringLiteral("Address Space"));

    cpuIcon_->setPos(spaceWidth / 2 - cpuIcon_->pixmap().width() - GridSize, GridSize * 3);
    cpuIcon_->setVisible(false);

    qreal segmentPositionX = - boundingRect().width() / 4;
    setupSubItems(segmentPositionX, getSubItemType(), memoryItem);

    setLabelPositions();
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceGraphicsItem::setLabelPositions()
//-----------------------------------------------------------------------------
void AddressSpaceGraphicsItem::setLabelPositions()
{
    qreal nameLabelY = boundingRect().height() / 2 - (getNameLabel()->boundingRect().height() * 3 / 4);
    qreal instanceNameLabelY = nameLabelY + (getNameLabel()->boundingRect().height() / 2);

    qreal nameLabelX = 0;
    if (subItemsAreFiltered())
    {
        nameLabelX = boundingRect().left();
    }

    getNameLabel()->setPos(nameLabelX, nameLabelY);
    getInstanceNameLabel()->setPos(nameLabelX, instanceNameLabelY);

    fitLabel(getNameLabel());
    fitLabel(getInstanceNameLabel());

    qreal rangeStartLabelX = boundingRect().right() - getRangeStartLabel()->boundingRect().width();
    qreal rangeStartLabelY = boundingRect().top();
    qreal rangeEndLabelX = boundingRect().right() - getRangeEndLabel()->boundingRect().width();
    qreal rangeEndLabelY = boundingRect().bottom() - getRangeEndLabel()->boundingRect().height();

    getRangeStartLabel()->setPos(rangeStartLabelX, rangeStartLabelY);
    getRangeEndLabel()->setPos(rangeEndLabelX, rangeEndLabelY);
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceGraphicsItem::createNewSubItem()
//-----------------------------------------------------------------------------
MemoryDesignerChildGraphicsItem* AddressSpaceGraphicsItem::createNewSubItem(
    QSharedPointer<MemoryItem const> subMemoryItem, bool isEmpty)
{
    auto segmentItem = new AddressSegmentGraphicsItem(subMemoryItem, isEmpty, getIdentifierChain(), getContainingInstance(), this);

    connect(segmentItem, SIGNAL(openComponentDocument(VLNV const&, QVector<QString>)),
        this, SIGNAL(openComponentDocument(VLNV const&, QVector<QString>)), Qt::UniqueConnection);

    return segmentItem;
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceGraphicsItem::createEmptySubItem()
//-----------------------------------------------------------------------------
MemoryDesignerChildGraphicsItem* AddressSpaceGraphicsItem::createEmptySubItem(quint64 beginAddress,
    quint64 rangeEnd)
{
    QString emptySegmentOffset = QString::number(beginAddress);
    QString emptySegmentRange = QString::number(rangeEnd - beginAddress + 1);

    QSharedPointer<MemoryItem> emptySegment(new MemoryItem("Empty", MemoryDesignerConstants::ADDRESSSEGMENT_TYPE));
    emptySegment->setOffset(emptySegmentOffset);
    emptySegment->setRange(emptySegmentRange);

    return createNewSubItem(emptySegment, true);
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceGraphicsItem::getAllConnectedSpaceItems()
//-----------------------------------------------------------------------------
QVector<MainMemoryGraphicsItem*> AddressSpaceGraphicsItem::getAllConnectedSpaceItems() const
{
    QVector<MainMemoryGraphicsItem*> allConnectedSpaceItems;

    QSharedPointer<QVector<MainMemoryGraphicsItem*> > visitedItems (new QVector<MainMemoryGraphicsItem*> ());

    QMultiMap<quint64, MemoryConnectionItem*> allConnections = getAllConnectionsFromConnectedItems(visitedItems);
    QMultiMapIterator<quint64, MemoryConnectionItem*> connectionIterator (allConnections);
    while (connectionIterator.hasNext())
    {
        connectionIterator.next();

        MemoryConnectionItem const* connectionItem = connectionIterator.value();
        if (auto connectionStartItem = connectionItem->getConnectionStartItem();
            connectionStartItem && connectionStartItem != this &&
            dynamic_cast<AddressSpaceGraphicsItem*>(connectionStartItem) &&
            !allConnectedSpaceItems.contains(connectionStartItem))
        {
            allConnectedSpaceItems.append(connectionStartItem);
        }

        if (auto connectionEndItem = connectionItem->getConnectionEndItem();
            connectionEndItem && connectionEndItem != this &&
            dynamic_cast<AddressSpaceGraphicsItem*>(connectionEndItem) &&
            !allConnectedSpaceItems.contains(connectionEndItem))
        {
            allConnectedSpaceItems.append(connectionEndItem);
        }
    }

    return allConnectedSpaceItems;

}

//-----------------------------------------------------------------------------
// Function: AddressSpaceGraphicsItem::getMinimumHeightForSubItems()
//-----------------------------------------------------------------------------
qreal AddressSpaceGraphicsItem::getMinimumHeightForSubItems() const
{
    return AddressSpaceItemConstants::MINIMUMSUBITEMHEIGHT;
}
