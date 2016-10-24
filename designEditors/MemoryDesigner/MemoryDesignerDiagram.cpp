//-----------------------------------------------------------------------------
// File: MemoryDesignerDiagram.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 07.07.2016
//
// Description:
// Declares the memory design diagram class.
//-----------------------------------------------------------------------------

#include "MemoryDesignerDiagram.h"

#include <library/LibraryManager/libraryinterface.h>

#include <common/graphicsItems/GraphicsColumnLayout.h>

#include <designEditors/common/diagramgrid.h>

#include <designEditors/MemoryDesigner/MemoryDesignDocument.h>
#include <designEditors/MemoryDesigner/MasterSlavePathSearch.h>
#include <designEditors/MemoryDesigner/ConnectivityGraph.h>
#include <designEditors/MemoryDesigner/ConnectivityConnection.h>
#include <designEditors/MemoryDesigner/ConnectivityInterface.h>
#include <designEditors/MemoryDesigner/AddressSpaceGraphicsItem.h>
#include <designEditors/MemoryDesigner/MemoryMapGraphicsItem.h>
#include <designEditors/MemoryDesigner/MainMemoryGraphicsItem.h>
#include <designEditors/MemoryDesigner/MemoryExtensionGraphicsItem.h>
#include <designEditors/MemoryDesigner/MemoryConnectionItem.h>
#include <designEditors/MemoryDesigner/ConnectivityGraph.h>
#include <designEditors/MemoryDesigner/ConnectivityComponent.h>
#include <designEditors/MemoryDesigner/MemoryItem.h>
#include <designEditors/MemoryDesigner/MemoryCollisionItem.h>
#include <designEditors/MemoryDesigner/MemoryColumn.h>
#include <designEditors/MemoryDesigner/MemoryDesignerConstants.h>

#include <IPXACTmodels/kactusExtensions/ColumnDesc.h>
#include <IPXACTmodels/common/VLNV.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>

#include <QPainter>
#include <QPen>
#include <QGraphicsSceneWheelEvent>

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::MemoryDesignerDiagram()
//-----------------------------------------------------------------------------
MemoryDesignerDiagram::MemoryDesignerDiagram(LibraryInterface* library, MemoryDesignDocument* parent):
QGraphicsScene(parent),
parentDocument_(parent),
layout_(new GraphicsColumnLayout(this)),
libraryHandler_(library),
instanceLocator_(library)
{
    setSceneRect(0, 0, 100000, 100000);
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::~MemoryDesignDocument()
//-----------------------------------------------------------------------------
MemoryDesignerDiagram::~MemoryDesignerDiagram()
{

}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::loadDesign()
//-----------------------------------------------------------------------------
bool MemoryDesignerDiagram::loadDesignFromCurrentView(QSharedPointer<const Component> component,
    QString const& viewName)
{
    clearScene();

    if (createConnectionGraph(component, viewName))
    {
        createColumns();
        createMemoryItems();
        createMemoryConnections();

        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::clearScene()
//-----------------------------------------------------------------------------
void MemoryDesignerDiagram::clearScene()
{
    clearLayout();
    clear();
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::clearLayout()
//-----------------------------------------------------------------------------
void MemoryDesignerDiagram::clearLayout()
{
    layout_.clear();
    layout_ = QSharedPointer<GraphicsColumnLayout>(new GraphicsColumnLayout(this));
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::createConnectionGraph()
//-----------------------------------------------------------------------------
bool MemoryDesignerDiagram::createConnectionGraph(QSharedPointer<const Component> component,
    QString const& viewName)
{
    QSharedPointer<Design> selectedDesign = getContainingDesign(component, viewName);
    if (selectedDesign)
    {
        QSharedPointer<const DesignConfiguration> containingConfiguration =
            getContainingDesignConfiguration(component, viewName);
        if (containingConfiguration)
        {
            connectionGraph_ = instanceLocator_.createConnectivityGraph(selectedDesign, containingConfiguration);
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::getDesignFromComponentUsingView()
//-----------------------------------------------------------------------------
QSharedPointer<Design> MemoryDesignerDiagram::getContainingDesign(QSharedPointer<const Component> component,
    QString const& viewName) const
{
    foreach (QSharedPointer<View> componentView, *component->getViews())
    {
        if (componentView->name() == viewName)
        {
            if (componentView->isHierarchical())
            {
                VLNV designVLNV;

                QString designInstantiationReference = componentView->getDesignInstantiationRef();
                if (designInstantiationReference.isEmpty())
                {
                    QSharedPointer<const DesignConfiguration> containingConfiguration =
                        getContainingDesignConfiguration(component, viewName);
                    if (containingConfiguration)
                    {
                        designVLNV = containingConfiguration->getDesignRef();
                    }
                }
                else
                {
                    foreach (QSharedPointer<DesignInstantiation> instantiation,
                        *component->getDesignInstantiations())
                    {
                        if (instantiation->name() == viewName)
                        {
                            designVLNV = *instantiation->getDesignReference().data();
                            break;
                        }
                    }
                }

                if (designVLNV.isValid() && designVLNV.getType() == VLNV::DESIGN)
                {
                    QSharedPointer<Design> containingDesign = libraryHandler_->getDesign(designVLNV);
                    return containingDesign;
                }
            }
            else
            {
                break;
            }
        }
    }

    return QSharedPointer<Design>();
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::getContainingDesignConfiguration()
//-----------------------------------------------------------------------------
QSharedPointer<const DesignConfiguration> MemoryDesignerDiagram::getContainingDesignConfiguration(
    QSharedPointer<const Component> component, QString const& viewName) const
{
    foreach (QSharedPointer<View> activeView, *component->getViews())
    {
        if (activeView->name() == viewName)
        {
            QString designConfigurationReference = activeView->getDesignConfigurationInstantiationRef();
            if (!designConfigurationReference.isEmpty())
            {
                foreach (QSharedPointer<DesignConfigurationInstantiation> instantiation,
                    *component->getDesignConfigurationInstantiations())
                {
                    if (instantiation->name() == designConfigurationReference)
                    {
                        QSharedPointer<ConfigurableVLNVReference> designConfigurationVLNV =
                            instantiation->getDesignConfigurationReference();

                        QSharedPointer<const Document> designConfigurationDocument =
                            libraryHandler_->getModelReadOnly(*designConfigurationVLNV.data());
                        QSharedPointer<const DesignConfiguration> designConfiguration =
                            designConfigurationDocument.dynamicCast<const DesignConfiguration>();

                        return designConfiguration;
                    }
                }
            }
        }
    }

    return QSharedPointer<const DesignConfiguration>();
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::createColumns()
//-----------------------------------------------------------------------------
void MemoryDesignerDiagram::createColumns()
{
    createAddressSpaceColumn();
    createMemoryColumns();
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::createAddressSpaceColumn()
//-----------------------------------------------------------------------------
MemoryColumn* MemoryDesignerDiagram::createAddressSpaceColumn()
{
    QString const SPACENAME = "Address Space";

    QSharedPointer<ColumnDesc> addressSpaceColumnDescription(new ColumnDesc());
    addressSpaceColumnDescription->setName(SPACENAME);
    addressSpaceColumnDescription->setMinimumWidth(COLUMNWIDTH);
    addressSpaceColumnDescription->setWidth(COLUMNWIDTH);

    QPointF columnPosition (0,0);

    int spaceCounter = 0;
    foreach (GraphicsColumn* column, layout_->getColumns())
    {
        if (column->name().contains(SPACENAME, Qt::CaseInsensitive))
        {
            spaceCounter += 1;
        }
    }
    if (spaceCounter != 0)
    {
        addressSpaceColumnDescription->setName(SPACENAME + "_" + QString::number(spaceCounter));
    }

    MemoryColumn* spaceColumn(new MemoryColumn(addressSpaceColumnDescription, layout_.data(), GridSize * 7));
    layout_->addColumn(spaceColumn);

    spaceColumn->setPos(columnPosition);

    layout_->onMoveColumn(spaceColumn);
    layout_->onReleaseColumn(spaceColumn);

    return spaceColumn;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::createMemoryColumns()
//-----------------------------------------------------------------------------
void MemoryDesignerDiagram::createMemoryColumns()
{
    QPointF columnPlacement (0,0);
    foreach (GraphicsColumn* column, layout_->getColumns())
    {
        columnPlacement.setX(columnPlacement.x() + column->boundingRect().width());
    }

    QSharedPointer<ColumnDesc> memoryMapColumnDescription(new ColumnDesc());
    memoryMapColumnDescription->setName("Memory Maps");
    memoryMapColumnDescription->setMinimumWidth(COLUMNWIDTH);
    memoryMapColumnDescription->setWidth(COLUMNWIDTH);

    MemoryColumn* memoryMapColumn(new MemoryColumn(memoryMapColumnDescription, layout_.data(), 0));

    memoryMapColumn->setPos(columnPlacement);
    layout_->addColumn(memoryMapColumn);

    createMemoryOverlapColumn();
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::createMemoryOverlapColumn()
//-----------------------------------------------------------------------------
MemoryColumn* MemoryDesignerDiagram::createMemoryOverlapColumn()
{
    QString overlapName = "Memory Map Overlap";

    QSharedPointer<ColumnDesc> memoryMapOverlapColumnDescription(new ColumnDesc());
    memoryMapOverlapColumnDescription->setName(overlapName);
    memoryMapOverlapColumnDescription->setMinimumWidth(COLUMNWIDTH);
    memoryMapOverlapColumnDescription->setWidth(COLUMNWIDTH);

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
        memoryMapOverlapColumnDescription->setName(overlapName + "_" + QString::number(overlapCounter));
    }

    MemoryColumn* memoryMapOverlapColumn(new MemoryColumn(memoryMapOverlapColumnDescription, layout_.data(), 0));
    layout_->addColumn(memoryMapOverlapColumn);

    QPointF columnPosition (columnXPosition, 0);
    memoryMapOverlapColumn->setPos(columnPosition);
    layout_->onMoveColumn(memoryMapOverlapColumn);
    layout_->onReleaseColumn(memoryMapOverlapColumn);

    return memoryMapOverlapColumn;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::createMemoryItems()
//-----------------------------------------------------------------------------
void MemoryDesignerDiagram::createMemoryItems()
{
    MemoryColumn* addressSpaceColumn = findColumnByName("Address Space");
    MemoryColumn* memoryMapColumn = findColumnByName("Memory Maps");
    
    if (addressSpaceColumn && memoryMapColumn)
    {
        foreach (QSharedPointer<ConnectivityComponent> componentInstance, connectionGraph_->getInstances())
        {
            foreach (QSharedPointer<MemoryItem> memoryItem, componentInstance->getMemories())
            {
                if (memoryItem->getType().compare("addressSpace", Qt::CaseInsensitive) == 0)
                {
                    createAddressSpaceItem(memoryItem, componentInstance, addressSpaceColumn);
                }
                else if (memoryItem->getType().compare("memoryMap", Qt::CaseInsensitive) == 0)
                {
                    createMemoryMapItem(memoryItem, componentInstance, memoryMapColumn);
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::createAddressSpaceItem()
//-----------------------------------------------------------------------------
void MemoryDesignerDiagram::createAddressSpaceItem(QSharedPointer<MemoryItem> spaceItem,
    QSharedPointer<ConnectivityComponent> containingInstance, MemoryColumn* containingColumn)
{
    if (containingColumn)
    {
        AddressSpaceGraphicsItem* spaceGraphicsItem =
            new AddressSpaceGraphicsItem(spaceItem, containingInstance, containingColumn);
        containingColumn->addItem(spaceGraphicsItem);
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::createMemoryMapItem()
//-----------------------------------------------------------------------------
void MemoryDesignerDiagram::createMemoryMapItem(QSharedPointer<MemoryItem> mapItem,
    QSharedPointer<ConnectivityComponent> containingInstance, MemoryColumn* containingColumn)
{
    if (containingColumn)
    {
        MemoryMapGraphicsItem* mapGraphicsItem =
            new MemoryMapGraphicsItem(mapItem, containingInstance, containingColumn);
        containingColumn->addItem(mapGraphicsItem);
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::createMemoryConnections()
//-----------------------------------------------------------------------------
void MemoryDesignerDiagram::createMemoryConnections()
{
    MasterSlavePathSearch pathSearcher;

    QVector<QVector<QSharedPointer<ConnectivityInterface> > > masterSlavePaths =
        pathSearcher.findMasterSlavePaths(connectionGraph_);

    QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMapItems(
        new QVector<MainMemoryGraphicsItem*> ());

    int spaceYPlacement = MemoryDesignerConstants::SPACEITEMINTERVAL;

    QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems(
        new QVector<MainMemoryGraphicsItem*> ());

    MemoryColumn* spaceColumn = findColumnByName("Address Space");
    MemoryColumn* memoryMapColumn = findColumnByName("Memory Maps");

    foreach (QVector<QSharedPointer<ConnectivityInterface> > singlePath, masterSlavePaths)
    {
        createConnection(
            singlePath, placedMapItems, memoryMapColumn, spaceYPlacement, placedSpaceItems, spaceColumn);
    }

    compressGraphicsItems(placedSpaceItems, spaceYPlacement, spaceColumn);
    repositionCompressedMemoryMaps(placedMapItems, memoryMapColumn, spaceColumn);

    moveUnconnectedAddressSpaces(placedSpaceItems, spaceYPlacement, spaceColumn);
    moveUnconnectedMemoryMaps(placedMapItems, memoryMapColumn);

    reDrawConnections();

    createOverlappingConnectionMarkers(placedSpaceItems);
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::createConnection()
//-----------------------------------------------------------------------------
void MemoryDesignerDiagram::createConnection(QVector<QSharedPointer<ConnectivityInterface> > connectionPath,
    QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMapItems, MemoryColumn* memoryMapColumn,
    int& spaceYPlacement, QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems,
    MemoryColumn* spaceColumn)
{
    QSharedPointer<ConnectivityInterface> startInterface = connectionPath.first();
    QSharedPointer<ConnectivityInterface> endInterface = connectionPath.last();

    if (startInterface && endInterface && startInterface != endInterface)
    {
        QSharedPointer<MemoryItem> startMemoryItem = startInterface->getConnectedMemory();
        QSharedPointer<MemoryItem> endMemoryItem = endInterface->getConnectedMemory();

        QSharedPointer<ConnectivityComponent> startComponent = startInterface->getInstance();
        QSharedPointer<ConnectivityComponent> endComponent = endInterface->getInstance();

        if (startMemoryItem && endMemoryItem && startMemoryItem != endMemoryItem &&
            startComponent && endComponent && startComponent != endComponent)
        {
            MainMemoryGraphicsItem* connectionStartItem =
                spaceColumn->findGraphicsItemByName(startMemoryItem->getName(), startComponent->getName());

            MainMemoryGraphicsItem* connectionEndItem =
                memoryMapColumn->findGraphicsItemByName(endMemoryItem->getName(), endComponent->getName());

            if (connectionStartItem && connectionEndItem)
            {
                connectionStartItem->hideMemoryRangeLabels();

                bool spaceItemPlaced = false;
                if (!placedSpaceItems->contains(connectionStartItem))
                {
                    moveAddressSpaceItem(connectionStartItem, placedSpaceItems, spaceColumn, spaceYPlacement);
                    placedSpaceItems->append(connectionStartItem);
                    spaceItemPlaced = true;
                }

                quint64 baseAddressNumber = startInterface->getBaseAddress().toULongLong();

                quint64 mirroredSlaveAddressChange = 0;

                bool hasRemapRange = false;
                quint64 memoryMapEndAddress = 0;

                foreach(QSharedPointer<ConnectivityInterface> pathInterface, connectionPath)
                {
                    if (pathInterface != startInterface && pathInterface != endInterface)
                    {
                        if (pathInterface->getMode().compare("mirroredSlave", Qt::CaseInsensitive) == 0)
                        {
                            mirroredSlaveAddressChange += pathInterface->getRemapAddress().toULongLong();

                            memoryMapEndAddress = pathInterface->getRemapRange().toULongLong() - 1;
                            hasRemapRange = true;
                        }
                        else if (pathInterface->getMode().compare("master", Qt::CaseInsensitive) == 0)
                        {
                            mirroredSlaveAddressChange += pathInterface->getBaseAddress().toULongLong();
                        }
                    }
                }

                quint64 memoryMapBaseAddress = getMemoryMapBeginAddress(connectionEndItem);

                if (!hasRemapRange)
                {
                    memoryMapEndAddress = getMemoryMapEndAddress(connectionEndItem);
                }

                quint64 endAddressNumber = baseAddressNumber + memoryMapEndAddress;

                quint64 remappedAddress = baseAddressNumber + mirroredSlaveAddressChange;
                QString startAddress = QString::number(remappedAddress);
                QString endAddress = QString::number(endAddressNumber + mirroredSlaveAddressChange);

                quint64 yTransfer = (memoryMapBaseAddress + mirroredSlaveAddressChange) *
                    MemoryDesignerConstants::RANGEINTERVAL;

                QPointF startConnectionPosBefore = connectionStartItem->pos();

                if (!placedMapItems->contains(connectionEndItem))
                {
                    connectionEndItem->hideMemoryRangeLabels();
                    connectionEndItem->setPos(connectionStartItem->x(), connectionStartItem->y() + yTransfer);

                    QPointF startItemPositionBefore = connectionStartItem->pos();

                    if (spaceItemPlaced)
                    {
                        repositionMemoryMap(placedMapItems, placedSpaceItems, connectionStartItem, spaceColumn,
                            connectionEndItem, yTransfer);
                    }
                    else
                    {
                        checkMemoryMapRepositionToOverlapColumn(
                            placedMapItems, connectionEndItem, memoryMapColumn, startAddress, endAddress);
                    }

                    QPointF startItemPositionAfter = connectionStartItem->pos();

                    spaceYPlacement += startItemPositionAfter.y() - startItemPositionBefore.y();

                    placedMapItems->append(connectionEndItem);
                }
                else
                {
                    placeSpaceItemToOtherColumn(connectionStartItem, spaceColumn, connectionEndItem, yTransfer);

                    spaceYPlacement = spaceYPlacement - (connectionStartItem->boundingRect().height() +
                        MemoryDesignerConstants::SPACEITEMINTERVAL);
                }

                if (spaceItemPlaced)
                {
                    connectionStartItem->changeChildItemRanges(baseAddressNumber);
                }
                connectionEndItem->changeChildItemRanges(remappedAddress);

                qreal spaceItemEndPointBefore = connectionStartItem->getSceneEndPoint();

                new MemoryConnectionItem(connectionStartItem, startAddress, endAddress, connectionEndItem,
                    startAddress, endAddress, spaceColumn->scene(), yTransfer);

                qreal spaceItemEndPointAfter = connectionStartItem->getSceneEndPoint();

                spaceYPlacement += spaceItemEndPointAfter - spaceItemEndPointBefore;
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::checkMemoryMapRepositionToOverlapColumn()
//-----------------------------------------------------------------------------
void MemoryDesignerDiagram::checkMemoryMapRepositionToOverlapColumn(
    QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMaps, MainMemoryGraphicsItem* memoryItem,
    MemoryColumn* originalColumn, QString const& startAddress, QString const& endAddress)
{
    quint64 connectionWidth =
        (endAddress.toULongLong() - startAddress.toULongLong() + 1) * MemoryDesignerConstants::RANGEINTERVAL;
    QRectF memoryItemExtensionRectangle(memoryItem->scenePos().x(), memoryItem->scenePos().y(),
        memoryItem->sceneBoundingRect().width(), connectionWidth);

    QRectF selectedItemRect = memoryItem->sceneBoundingRect();
    int selectedItemPenWidth = memoryItem->pen().width();
    foreach (MainMemoryGraphicsItem* comparisonMapItem, *placedMaps)
    {
        if (memoryItem != comparisonMapItem)
        {
            if (memoryMapOverlapsAnotherMemoryMap(selectedItemRect, selectedItemPenWidth,
                memoryItemExtensionRectangle, comparisonMapItem))
            {
                foreach (GraphicsColumn* column, layout_->getColumns())
                {
                    MemoryColumn* overlapColumn = dynamic_cast<MemoryColumn*>(column);
                    if (overlapColumn && overlapColumn->name().contains("Overlap"))
                    {
                        selectedItemRect.setX(selectedItemRect.x() + overlapColumn->boundingRect().width());
                        memoryItemExtensionRectangle.setX(selectedItemRect.x());

                        foreach (MainMemoryGraphicsItem* mapItem, *placedMaps)
                        {
                            if (mapItem != memoryItem)
                            {
                                if (memoryMapOverlapsAnotherMemoryMap(selectedItemRect, selectedItemPenWidth,
                                    memoryItemExtensionRectangle, mapItem))
                                {
                                    break;
                                }
                                else if (!memoryMapOverlapsAnotherMemoryMap(selectedItemRect, selectedItemPenWidth,
                                    memoryItemExtensionRectangle, mapItem) && mapItem == placedMaps->last())
                                {
                                    originalColumn->removeItem(memoryItem);
                                    overlapColumn->addItem(memoryItem);

                                    return;
                                }
                            }
                        }
                    }
                }

                MemoryColumn* overlapColumn = createMemoryOverlapColumn();
                originalColumn->removeItem(memoryItem);
                overlapColumn->addItem(memoryItem);

                return;
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::memoryMapOverlapsAnotherMemoryMap()
//-----------------------------------------------------------------------------
bool MemoryDesignerDiagram::memoryMapOverlapsAnotherMemoryMap(QRectF selectedMapRect, int selectedMapPenWidth,
    QRectF selectedExtensionRect, MainMemoryGraphicsItem* comparisonMemoryItem) const
{
    if (selectedMapRect.x() == comparisonMemoryItem->sceneBoundingRect().x())
    {
        int mapItemLineWidth = comparisonMemoryItem->pen().width();
        int extensionLineWidth = 0;

        QGraphicsRectItem* comparisonExtension = 0;
        foreach (QGraphicsItem* comparisonChildItem, comparisonMemoryItem->childItems())
        {
            QGraphicsRectItem* comparisonChildExtensionItem = dynamic_cast<QGraphicsRectItem*>(comparisonChildItem);
            if (comparisonChildExtensionItem)
            {
                comparisonExtension = comparisonChildExtensionItem;
                extensionLineWidth = comparisonExtension->pen().width();
                break;
            }
        }

        if (itemCollidesWithAnotherItem(selectedMapRect, selectedMapPenWidth,
            comparisonMemoryItem->sceneBoundingRect(), mapItemLineWidth) ||

            itemCollidesWithAnotherItem(selectedExtensionRect, selectedMapPenWidth,
            comparisonMemoryItem->sceneBoundingRect(), mapItemLineWidth) ||

            (comparisonExtension && 
            (itemCollidesWithAnotherItem(selectedMapRect, selectedMapPenWidth,
            comparisonExtension->sceneBoundingRect(), extensionLineWidth) ||

            (itemCollidesWithAnotherItem(selectedExtensionRect, selectedMapPenWidth,
            comparisonExtension->sceneBoundingRect(), extensionLineWidth)))))
        {
            return true;
        }
    }
    return false;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::repositionMemoryMap()
//-----------------------------------------------------------------------------
void MemoryDesignerDiagram::repositionMemoryMap(QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMapItems,
    QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems,
    MainMemoryGraphicsItem* startConnectionItem, MemoryColumn* addressSpaceColumn,
    MainMemoryGraphicsItem* endConnectionItem, int memoryMapYTransfer)
{
    foreach (MemoryDesignerGraphicsItem* memoryMapItem, *placedMapItems)
    {
        if (endConnectionItem != memoryMapItem && endConnectionItem->collidesWithItem(memoryMapItem))
        {
            QPointF collidingMapItemLowLeft =
                memoryMapItem->mapToScene(memoryMapItem->boundingRect().bottomLeft());
            int mapItemYTransfer = collidingMapItemLowLeft.y() + GridSize;

            endConnectionItem->setPos(endConnectionItem->x(), mapItemYTransfer);

            startConnectionItem->setPos(
                endConnectionItem->pos().x(), endConnectionItem->pos().y() - memoryMapYTransfer);

            repositionSpaceItemToMemoryMap(
                placedSpaceItems, startConnectionItem, endConnectionItem, addressSpaceColumn, memoryMapYTransfer);

            addressSpaceColumn->onMoveItem(startConnectionItem);
            addressSpaceColumn->onReleaseItem(startConnectionItem);

            repositionMemoryMap(placedMapItems, placedSpaceItems, startConnectionItem, addressSpaceColumn,
                endConnectionItem, memoryMapYTransfer);

            return;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::repositionSpaceItemToMemoryMap()
//-----------------------------------------------------------------------------
void MemoryDesignerDiagram::repositionSpaceItemToMemoryMap(
    QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems, MainMemoryGraphicsItem* startItem,
    MainMemoryGraphicsItem* endItem, MemoryColumn* spaceColumn, int memoryMapYTransfer)
{
    foreach (MemoryDesignerGraphicsItem* spaceItem, *placedSpaceItems)
    {
        if (spaceItem != startItem && startItem->collidesWithItem(spaceItem))
        {
            QPointF collidingSpaceItemLowLeft =
                spaceItem->mapToScene(spaceItem->boundingRect().bottomLeft());
            int spaceItemYTransfer = collidingSpaceItemLowLeft.y() + MemoryDesignerConstants::SPACEITEMINTERVAL;

            startItem->setPos(startItem->x(), spaceItemYTransfer);

            endItem->setPos(endItem->x(), startItem->y() + memoryMapYTransfer);

            repositionSpaceItemToMemoryMap(placedSpaceItems, startItem, endItem, spaceColumn, memoryMapYTransfer);

            return;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::placeSpaceItemToOtherColumn()
//-----------------------------------------------------------------------------
void MemoryDesignerDiagram::placeSpaceItemToOtherColumn(MainMemoryGraphicsItem* spaceItem,
    MemoryColumn* originalColumn, MainMemoryGraphicsItem* targetItem, int yTransfer)
{
    originalColumn->removeItem(spaceItem);
    originalColumn->onMoveItem(spaceItem);
    originalColumn->onReleaseItem(spaceItem);

    foreach (GraphicsColumn* column, layout_->getColumns())
    {
        MemoryColumn* currentSpaceColumn = dynamic_cast<MemoryColumn*>(column);
        if (currentSpaceColumn && currentSpaceColumn->name().contains("Address Space", Qt::CaseInsensitive))
        {
            currentSpaceColumn->addItem(spaceItem);
            spaceItem->setPos(targetItem->pos().x(), targetItem->pos().y() + yTransfer);

            if (spaceItem->collidingItems(Qt::IntersectsItemShape).isEmpty())
            {
                return;
            }
            else
            {
                currentSpaceColumn->removeItem(spaceItem);
            }
        }
    }

    MemoryColumn* newSpaceColumn = createAddressSpaceColumn();
    newSpaceColumn->addItem(spaceItem);
    spaceItem->setPos(targetItem->pos());
    newSpaceColumn->onMoveItem(spaceItem);
    newSpaceColumn->onReleaseItem(spaceItem);
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::reDrawConnections()
//-----------------------------------------------------------------------------
void MemoryDesignerDiagram::reDrawConnections()
{
    QVector<MemoryColumn*> addressSpaceColumns = getSpecifiedColumns("Address Space");

    foreach (MemoryColumn* singleSpaceColumn, addressSpaceColumns)
    {
        foreach (QGraphicsItem* graphicsItem, singleSpaceColumn->getItems())
        {
            MainMemoryGraphicsItem* memoryGraphicsItem = dynamic_cast<MainMemoryGraphicsItem*>(graphicsItem);
            if (memoryGraphicsItem)
            {
                memoryGraphicsItem->reDrawConnections();
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::getMemoryMapBeginAddress()
//-----------------------------------------------------------------------------
quint64 MemoryDesignerDiagram::getMemoryMapBeginAddress(MainMemoryGraphicsItem* memoryItem) const
{
    MemoryMapGraphicsItem* memoryMapItem = dynamic_cast<MemoryMapGraphicsItem*>(memoryItem);
    if (memoryMapItem)
    {
        return memoryMapItem->getBaseAddress();
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::getMemoryMapEndAddress()
//-----------------------------------------------------------------------------
quint64 MemoryDesignerDiagram::getMemoryMapEndAddress(MainMemoryGraphicsItem* memoryItem) const
{
    MemoryMapGraphicsItem* memoryMapItem = dynamic_cast<MemoryMapGraphicsItem*>(memoryItem);
    if (memoryMapItem)
    {
        return memoryMapItem->getLastAddress();
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::moveUnconnectedAddressSpaces()
//-----------------------------------------------------------------------------
void MemoryDesignerDiagram::moveUnconnectedAddressSpaces(
    QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems, int& spaceYPlacement,
    MemoryColumn* spaceColumn)
{
    spaceYPlacement += GridSize * 10;

    foreach (QGraphicsItem* graphicsItem, spaceColumn->getItems())
    {
        MainMemoryGraphicsItem* memoryGraphicsItem = dynamic_cast<MainMemoryGraphicsItem*>(graphicsItem);
        if (memoryGraphicsItem && !placedSpaceItems->contains(memoryGraphicsItem))
        {
            moveAddressSpaceItem(memoryGraphicsItem, placedSpaceItems, spaceColumn, spaceYPlacement);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::moveAddressSpaceItem()
//-----------------------------------------------------------------------------
void MemoryDesignerDiagram::moveAddressSpaceItem(MainMemoryGraphicsItem* spaceItem,
    QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems, MemoryColumn* spaceColumn,
    int& spaceYPlacement)
{
    spaceItem->setPos(spaceItem->x(), spaceYPlacement);
    spaceColumn->onMoveItem(spaceItem);
    spaceColumn->onReleaseItem(spaceItem);

    spaceYPlacement += spaceItem->boundingRect().height() + MemoryDesignerConstants::SPACEITEMINTERVAL;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::moveUnconnectedMemoryMaps()
//-----------------------------------------------------------------------------
void MemoryDesignerDiagram::moveUnconnectedMemoryMaps(
    QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMapItems, MemoryColumn* memoryMapColumn)
{
    if (!placedMapItems->isEmpty())
    {
        qreal lastMapItemBottomLinePositionY = 0;
        foreach (MainMemoryGraphicsItem* memoryMapItem, *placedMapItems)
        {
            int extensionHeight = 0;
            foreach (QGraphicsItem* childItem, memoryMapItem->childItems())
            {
                MemoryExtensionGraphicsItem* extensionItem = dynamic_cast<MemoryExtensionGraphicsItem*>(childItem);
                if (extensionItem)
                {
                    extensionHeight = extensionItem->sceneBoundingRect().height();
                }
            }

            qreal mapItemBottom = memoryMapItem->sceneBoundingRect().bottom() + extensionHeight;
            if (mapItemBottom > lastMapItemBottomLinePositionY)
            {
                lastMapItemBottomLinePositionY = mapItemBottom;
            }
        }

        int separationY = GridSize * 10 + lastMapItemBottomLinePositionY;

            foreach (QGraphicsItem* graphicsItem, memoryMapColumn->getItems())
            {
                MainMemoryGraphicsItem* memoryGraphicsItem = dynamic_cast<MainMemoryGraphicsItem*>(graphicsItem);
                if (memoryGraphicsItem && !placedMapItems->contains(memoryGraphicsItem))
                {
                    memoryGraphicsItem->setPos(memoryGraphicsItem->x(), separationY);

                    separationY += memoryGraphicsItem->boundingRect().height() + GridSize;
                }
            }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::findColumnByName()
//-----------------------------------------------------------------------------
MemoryColumn* MemoryDesignerDiagram::findColumnByName(QString const& columnName) const
{
    foreach (GraphicsColumn* column, layout_->getColumns())
    {
        MemoryColumn* memoryColumn = dynamic_cast<MemoryColumn*>(column);
        if (memoryColumn)
        {
            QSharedPointer<ColumnDesc> memoryDesc = memoryColumn->getColumnDesc();
            if (memoryDesc && memoryDesc->name().compare(columnName) == 0)
            {
                return memoryColumn;
            }
        }
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::extendMemoryItem()
//-----------------------------------------------------------------------------
void MemoryDesignerDiagram::extendMemoryItem(MainMemoryGraphicsItem* memoryGraphicsItem,
    MemoryConnectionItem* connectionItem, int& spaceYplacement)
{
    QPointF spaceTopLeft = memoryGraphicsItem->boundingRect().topLeft();
    QPointF spaceLowRight = memoryGraphicsItem->boundingRect().bottomRight();

    QPointF connectionTopLeft =
        memoryGraphicsItem->mapFromItem(connectionItem, connectionItem->boundingRect().topLeft());
    QPointF connectionLowRight =
        memoryGraphicsItem->mapFromItem(connectionItem, connectionItem->boundingRect().bottomRight());

    if (connectionLowRight.y() > spaceLowRight.y())
    {
        qreal positionX = spaceTopLeft.x() + 0.5;
        qreal extensionWidth = spaceLowRight.x() - spaceTopLeft.x() - 1;
        qreal positionY = spaceLowRight.y() - 0.5;
        qreal extensionHeight = connectionLowRight.y() - spaceLowRight.y();

        new MemoryExtensionGraphicsItem(positionX, positionY, extensionWidth, extensionHeight, memoryGraphicsItem);

        spaceYplacement += extensionHeight;
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::createOverlappingConnectionMarkers()
//-----------------------------------------------------------------------------
void MemoryDesignerDiagram::createOverlappingConnectionMarkers(
    QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems)
{
    foreach (MainMemoryGraphicsItem* spaceItem, *placedSpaceItems)
    {
        if (spaceItem->getMemoryConnections().size() > 1)
        {
            QVector<MemoryConnectionItem*> spaceConnections = spaceItem->getConnectionsInVector();

            for (int selectedIndex = 0; selectedIndex < spaceItem->getMemoryConnections().size() - 1;
                ++selectedIndex)
            {
                for (int comparisonIndex = selectedIndex + 1;
                    comparisonIndex < spaceItem->getMemoryConnections().size(); ++comparisonIndex)
                {
                    MemoryConnectionItem* selectedItem = spaceConnections.at(selectedIndex);
                    MemoryConnectionItem* comparisonItem = spaceConnections.at(comparisonIndex);

                    QRectF connectionRect = selectedItem->sceneBoundingRect();
                    QRectF comparisonRect = comparisonItem->sceneBoundingRect();

                    if (selectedItem && comparisonItem && selectedItem != comparisonItem &&
                        itemCollidesWithAnotherItem(connectionRect, selectedItem->pen().width(), comparisonRect,
                        comparisonItem->pen().width()))
                    {
                        MemoryCollisionItem* newCollisionItem =
                            new MemoryCollisionItem(selectedItem, comparisonItem, spaceItem->scene());
                        spaceItem->addConnectionCollision(newCollisionItem);
                    }
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::itemCollidesWithAnotherItem()
//-----------------------------------------------------------------------------
bool MemoryDesignerDiagram::itemCollidesWithAnotherItem(QRectF firstRectangle, int firstLineWidth,
    QRectF secondRectangle, int secondLineWidth) const
{
    qreal firstItemTop = firstRectangle.topLeft().y() + firstLineWidth;
    qreal firstItemLow = firstRectangle.bottomLeft().y() - firstLineWidth;
    qreal secondItemTop = secondRectangle.topLeft().y() + secondLineWidth;
    qreal secondItemLow = secondRectangle.bottomLeft().y() - secondLineWidth;

    if (
        ((firstItemTop > secondItemTop && firstItemTop < secondItemLow) ||
        (firstItemLow < secondItemLow && firstItemLow > secondItemTop) ||

        (secondItemTop > firstItemTop && secondItemTop < firstItemLow) ||
        (secondItemLow < firstItemLow && secondItemLow > firstItemTop) ||

        (firstItemTop == secondItemTop && firstItemLow == secondItemLow)))
    {
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::compressGraphicsItems()
//-----------------------------------------------------------------------------
void MemoryDesignerDiagram::compressGraphicsItems(
    QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems, int& spaceYPlacement,
    MemoryColumn* spaceColumn)
{
    foreach (GraphicsColumn* graphicsColumn, layout_->getColumns())
    {
        MemoryColumn* memoryColumn = dynamic_cast<MemoryColumn*>(graphicsColumn);
        if (memoryColumn)
        {
            int yTransfer = 0;
            quint64 spaceItemLowAfter = 0;

            foreach (QGraphicsItem* graphicsItem, memoryColumn->getItems())
            {
                MainMemoryGraphicsItem* memoryItem = dynamic_cast<MainMemoryGraphicsItem*>(graphicsItem);
                if (memoryItem)
                {
                    int memoryItemLowBefore = memoryItem->getSceneEndPoint();

                    memoryItem->condenseItemAndChildItems();

                    MemoryConnectionItem* lastConnection = memoryItem->getLastConnection();
                    if (lastConnection)
                    {
                        extendMemoryItem(memoryItem, lastConnection, spaceYPlacement);
                    }

                    AddressSpaceGraphicsItem* spaceItem = dynamic_cast<AddressSpaceGraphicsItem*>(memoryItem);
                    if (spaceItem && placedSpaceItems->contains(memoryItem) && memoryColumn == spaceColumn)
                    {
                        quint64 spaceItemTop = spaceItem->sceneBoundingRect().top();
                        qint64 spaceInterval = (spaceItemTop + yTransfer) - spaceItemLowAfter;

                        if (spaceInterval < MemoryDesignerConstants::SPACEITEMINTERVAL)
                        {
                            quint64 yMovementAddition = MemoryDesignerConstants::SPACEITEMINTERVAL -
                                ((spaceItemTop + yTransfer) - spaceItemLowAfter);
                            yTransfer += yMovementAddition;
                        }

                        spaceItem->moveConnectedItems(yTransfer);

                        spaceItemLowAfter = spaceItem->getSceneEndPoint();

                        yTransfer = spaceItemLowAfter - memoryItemLowBefore;

                        spaceYPlacement = spaceItemLowAfter + MemoryDesignerConstants::SPACEITEMINTERVAL;
                    }

                    memoryItem->resizeSubItemNameLabels();
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::repositionCompressedMemoryMaps()
//-----------------------------------------------------------------------------
void MemoryDesignerDiagram::repositionCompressedMemoryMaps(
    QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMapItems, MemoryColumn* memoryMapColumn,
    MemoryColumn* spaceColumn)
{
    foreach (MainMemoryGraphicsItem* mapItem, *placedMapItems)
    {
        QGraphicsItem* mapParentItem = mapItem->parentItem();
        MemoryColumn* originalColumn = dynamic_cast<MemoryColumn*>(mapParentItem);
        if (originalColumn && originalColumn != memoryMapColumn)
        {
            QRectF mapRectangle = mapItem->sceneBoundingRect();
            int mapPenWidth = mapItem->pen().width();

            int columnWidth = originalColumn->sceneBoundingRect().width();

            QPointF columnPoint (originalColumn->pos().x() - columnWidth, mapRectangle.y());
            GraphicsColumn* comparisonColumn = layout_->findColumnAt(columnPoint);
            if (comparisonColumn)
            {
                while (comparisonColumn != spaceColumn)
                {
                    mapRectangle.setX(mapRectangle.x() - columnWidth);

                    if (!memoryMapCollidesWithMemoryMapsInColumn(mapRectangle, mapPenWidth, mapItem,
                        comparisonColumn, placedMapItems))
                    {
                        originalColumn->removeItem(mapItem);
                        comparisonColumn->addItem(mapItem, true);

                        if (originalColumn->getItems().isEmpty() && originalColumn->name() != "Memory Map Overlap")
                        {
                            layout_->removeColumn(originalColumn);
                        }

                        break;
                    }

                    columnPoint.setX(mapRectangle.x());
                    comparisonColumn = layout_->findColumnAt(columnPoint);
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::memoryMapCollidesWithIMemoryMapsInColumn()
//-----------------------------------------------------------------------------
bool MemoryDesignerDiagram::memoryMapCollidesWithMemoryMapsInColumn(QRectF mapRectangle, int mapPenWidth,
    MainMemoryGraphicsItem* mapItem, GraphicsColumn* comparisonColumn,
    QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMapItems)
{
    foreach (QGraphicsItem* comparisonChild, comparisonColumn->getItems())
    {
        MemoryMapGraphicsItem* comparisonMap =
            dynamic_cast<MemoryMapGraphicsItem*>(comparisonChild);
        if (comparisonMap && comparisonMap != mapItem && placedMapItems->contains(comparisonMap))
        {
            QRectF comparisonRectangle = comparisonMap->sceneBoundingRect();
            int comparisonPenWidth = comparisonMap->pen().width();

            if (itemCollidesWithAnotherItem(mapRectangle, mapPenWidth, comparisonRectangle, comparisonPenWidth))
            {
                return true;
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::getSpecifiedColumns()
//-----------------------------------------------------------------------------
QVector<MemoryColumn*> MemoryDesignerDiagram::getSpecifiedColumns(QString const& columnSpecification)
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
// Function: MemoryDesignerDiagram::onShow()
//-----------------------------------------------------------------------------
void MemoryDesignerDiagram::onShow()
{

}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::onVerticalScroll()
//-----------------------------------------------------------------------------
void MemoryDesignerDiagram::onVerticalScroll(qreal y)
{
    layout_->setOffsetY(y);
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::wheelEvent()
//-----------------------------------------------------------------------------
void MemoryDesignerDiagram::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    if (event->modifiers() == Qt::CTRL && parentDocument_)
    {
        // Set the zoom level and center the view.
        parentDocument_->setZoomLevel(parentDocument_->getZoomLevel() + event->delta() / 12);

        // Retrieve the center point in diagram coordinates.
        QPointF centerPos = itemsBoundingRect().center();
        parentDocument_->centerViewTo(centerPos);        

        event->accept();
    }
    else
    {
        QGraphicsScene::wheelEvent(event);
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::drawBackground()
//-----------------------------------------------------------------------------
void MemoryDesignerDiagram::drawBackground(QPainter *painter, const QRectF &rect)
{
    painter->setWorldMatrixEnabled(true);
    painter->setPen(QPen(Qt::black, 0));

    qreal left = int(rect.left()) - (int(rect.left()) % GridSize );
    qreal top = int(rect.top()) - (int(rect.top()) % GridSize );

    for (qreal x = left; x < rect.right(); x += GridSize )
    {
//         for (qreal y = top; y < rect.bottom(); y += 3 * GridSize )
        for (qreal y = top; y < rect.bottom(); y += GridSize * 3)
        {
            painter->drawPoint(x, y);
        }
    }
}
