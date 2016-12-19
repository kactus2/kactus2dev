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
instanceLocator_(library),
condenseMemoryItems_(true),
filterAddressSpaceChains_(true),
filterAddressSpaceSegments_(true),
filterAddressBlocks_(true),
filterRegisters_(true),
filterFields_(true),
spaceColumnWidth_(MemoryDesignerConstants::SPACECOLUMNWIDTH),
memoryMapColumnWidth_(MemoryDesignerConstants::MEMORYMAPCOLUMNWIDTH),
widthBoundary_(0),
connectionsToMemoryMaps_(),
memoryCollisions_()
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
// Function: MemoryDesignerDiagram::setCondenseMemoryItems()
//-----------------------------------------------------------------------------
void MemoryDesignerDiagram::setCondenseMemoryItems(bool condenseMemoryItems)
{
    condenseMemoryItems_ = condenseMemoryItems;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::memoryItemsAreCondensed()
//-----------------------------------------------------------------------------
bool MemoryDesignerDiagram::memoryItemsAreCondensed() const
{
    return condenseMemoryItems_;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::setFilterAddressSpaceChains()
//-----------------------------------------------------------------------------
void MemoryDesignerDiagram::setFilterAddressSpaceChains(bool filterChains)
{
    filterAddressSpaceChains_ = filterChains;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::addressSpaceChainsAreFiltered()
//-----------------------------------------------------------------------------
bool MemoryDesignerDiagram::addressSpaceChainsAreFiltered() const
{
    return filterAddressSpaceChains_;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::condenseFieldItems()
//-----------------------------------------------------------------------------
void MemoryDesignerDiagram::condenseFieldItems()
{
    if (!addressBlockRegistersAreFiltered() && !fieldsAreFiltered()  && widthBoundary_ > 0)
    {
        changeMemoryMapWidths(-widthBoundary_);
        widthBoundary_ = 0;
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::extendFieldItems()
//-----------------------------------------------------------------------------
void MemoryDesignerDiagram::extendFieldItems()
{
    if (!addressBlockRegistersAreFiltered() && !fieldsAreFiltered())
    {
        qreal widthChange = 0;

        foreach (GraphicsColumn* column, layout_->getColumns())
        {
            MemoryColumn* memoryColumn = dynamic_cast<MemoryColumn*>(column);
            if (memoryColumn && memoryColumn->containsMemoryMapItems())
            {
                widthChange = qMax(widthChange, memoryColumn->getMaximumNeededChangeInWidth());
            }
        }

        if (widthChange > widthBoundary_)
        {
            int futureColumnWidth = memoryMapColumnWidth_ + widthChange;
            int widthRemainder = futureColumnWidth % 10;
            if (widthRemainder != 9)
            {
                widthRemainder = 9 - widthRemainder;
                widthChange += widthRemainder;
            }

            changeMemoryMapWidths(widthChange);
            widthBoundary_ += widthChange;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::setFilterAddressSpaceSegments()
//-----------------------------------------------------------------------------
void MemoryDesignerDiagram::setFilterAddressSpaceSegments(bool filterSegments)
{
    filterAddressSpaceSegments_ = filterSegments;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::addressSpaceSegmentsAreFiltered()
//-----------------------------------------------------------------------------
bool MemoryDesignerDiagram::addressSpaceSegmentsAreFiltered() const
{
    return filterAddressSpaceSegments_;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::setFilterAddressBlocks()
//-----------------------------------------------------------------------------
void MemoryDesignerDiagram::setFilterAddressBlocks(bool filterBlocks)
{
    filterAddressBlocks_ = filterBlocks;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::addressBlocksAreFiltered()
//-----------------------------------------------------------------------------
bool MemoryDesignerDiagram::addressBlocksAreFiltered() const
{
    return filterAddressBlocks_;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::setFilterAddressBlockRegisters()
//-----------------------------------------------------------------------------
void MemoryDesignerDiagram::setFilterAddressBlockRegisters(bool filterRegisters)
{
    filterRegisters_ = filterRegisters;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::addressBlockRegistersAreFiltered()
//-----------------------------------------------------------------------------
bool MemoryDesignerDiagram::addressBlockRegistersAreFiltered() const
{
    return filterRegisters_;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::setFilterFields()
//-----------------------------------------------------------------------------
void MemoryDesignerDiagram::setFilterFields(bool filterFields)
{
    filterFields_ = filterFields;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::fieldsAreFiltered()
//-----------------------------------------------------------------------------
bool MemoryDesignerDiagram::fieldsAreFiltered() const
{
    return filterFields_;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::loadDesign()
//-----------------------------------------------------------------------------
bool MemoryDesignerDiagram::loadDesignFromCurrentView(QSharedPointer<const Component> component,
    QString const& viewName)
{
    clearScene();
    connectionsToMemoryMaps_.clear();
    memoryCollisions_.clear();
    widthBoundary_ = 0;

    if (filterRegisters_ || (filterAddressBlocks_ && filterFields_))
    {
        memoryMapColumnWidth_ = MemoryDesignerConstants::SPACECOLUMNWIDTH;
    }
    else if (filterFields_)
    {
        memoryMapColumnWidth_ =
            MemoryDesignerConstants::SPACECOLUMNWIDTH + (MemoryDesignerConstants::MAPSUBITEMPOSITIONX * 2 - 5);
    }
    else if (filterAddressBlocks_)
    {
        memoryMapColumnWidth_ =
            MemoryDesignerConstants::MEMORYMAPCOLUMNWIDTH - (MemoryDesignerConstants::MAPSUBITEMPOSITIONX * 2 - 5);
    }
    else
    {
        memoryMapColumnWidth_ = MemoryDesignerConstants::MEMORYMAPCOLUMNWIDTH;
    }

    if (createConnectionGraph(component, viewName))
    {
        createInitialColumns();

        MemoryColumn* addressSpaceColumn = findColumnByName(MemoryDesignerConstants::ADDRESSSPACECOLUMN_NAME);
        MemoryColumn* memoryMapColumn = findColumnByName(MemoryDesignerConstants::MEMORYMAPCOLUM_NAME);
        if (addressSpaceColumn && memoryMapColumn)
        {
            createMemoryItems(addressSpaceColumn, memoryMapColumn);
            createMemoryConnections(addressSpaceColumn, memoryMapColumn);

            return true;
        }
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
// Function: MemoryDesignerDiagram::createInitialColumns()
//-----------------------------------------------------------------------------
void MemoryDesignerDiagram::createInitialColumns()
{
    createAddressSpaceColumn();
    createMemoryColumns();
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::createAddressSpaceColumn()
//-----------------------------------------------------------------------------
MemoryColumn* MemoryDesignerDiagram::createAddressSpaceColumn()
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
    memoryMapColumnDescription->setName(MemoryDesignerConstants::MEMORYMAPCOLUM_NAME);
    memoryMapColumnDescription->setMinimumWidth(memoryMapColumnWidth_);
    memoryMapColumnDescription->setWidth(memoryMapColumnWidth_);

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
// Function: MemoryDesignerDiagram::createMemoryItems()
//-----------------------------------------------------------------------------
void MemoryDesignerDiagram::createMemoryItems(MemoryColumn* spaceColumn, MemoryColumn* memoryMapColumn)
{
    foreach (QSharedPointer<ConnectivityComponent> componentInstance, connectionGraph_->getInstances())
    {
        foreach (QSharedPointer<MemoryItem> memoryItem, componentInstance->getMemories())
        {
            if (memoryItem->getType().compare(
                MemoryDesignerConstants::ADDRESSSPACE_TYPE, Qt::CaseInsensitive) == 0)
            {
                createAddressSpaceItem(memoryItem, componentInstance, spaceColumn);
            }
            else if (memoryItem->getType().compare(
                MemoryDesignerConstants::MEMORYMAP_TYPE, Qt::CaseInsensitive) == 0)
            {
                createMemoryMapItem(memoryItem, componentInstance, memoryMapColumn);
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
        AddressSpaceGraphicsItem* spaceGraphicsItem = new AddressSpaceGraphicsItem(
            spaceItem, containingInstance, filterAddressSpaceSegments_, containingColumn);
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
        MemoryMapGraphicsItem* mapGraphicsItem = new MemoryMapGraphicsItem(
            mapItem, filterAddressBlocks_, filterRegisters_, filterFields_, containingInstance, containingColumn);
        containingColumn->addItem(mapGraphicsItem);
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::createMemoryConnections()
//-----------------------------------------------------------------------------
void MemoryDesignerDiagram::createMemoryConnections(MemoryColumn* spaceColumn, MemoryColumn* memoryMapColumn)
{
    MasterSlavePathSearch pathSearcher;

    QVector<QVector<QSharedPointer<ConnectivityInterface> > > masterSlavePaths =
        pathSearcher.findMasterSlavePaths(connectionGraph_);

    int spaceYPlacement = MemoryDesignerConstants::SPACEITEMINTERVAL;

    QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMapItems(
        new QVector<MainMemoryGraphicsItem*> ());
    QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems(
        new QVector<MainMemoryGraphicsItem*> ());

    foreach (QVector<QSharedPointer<ConnectivityInterface> > singlePath, masterSlavePaths)
    {
        createConnection(
            singlePath, placedMapItems, memoryMapColumn, spaceYPlacement, placedSpaceItems, spaceColumn);
    }

    compressGraphicsItems(placedSpaceItems, spaceYPlacement, spaceColumn);
    if (condenseMemoryItems_)
    {
        repositionCompressedMemoryMaps(placedMapItems, memoryMapColumn);
    }

    moveUnconnectedAddressSpaces(placedSpaceItems, spaceColumn);
    moveUnconnectedMemoryMaps(placedMapItems, memoryMapColumn);

    reDrawConnections(placedSpaceItems);

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
        MainMemoryGraphicsItem* connectionStartItem =
            getMainGraphicsItem(startInterface, MemoryDesignerConstants::ADDRESSSPACECOLUMN_NAME);
        MainMemoryGraphicsItem* connectionEndItem =
            getMainGraphicsItem(endInterface, MemoryDesignerConstants::MEMORYMAPCOLUMNCOMMON_NAME);
        if (connectionStartItem && connectionEndItem)
        {
            connectionStartItem->hideMemoryRangeLabels();

            if (!placedSpaceItems->contains(connectionStartItem))
            {
                spaceColumn->moveGraphicsItem(
                    connectionStartItem, spaceYPlacement, MemoryDesignerConstants::SPACEITEMINTERVAL);
            }

            quint64 baseAddressNumber = startInterface->getBaseAddress().toULongLong();

            bool hasRemapRange = false;
            quint64 mirroredSlaveAddressChange = 0;
            quint64 memoryMapEndAddress = 0;

            QVector<MainMemoryGraphicsItem*> spaceItemChain;
            spaceItemChain.append(connectionStartItem);

            QVector<MemoryConnectionItem*> connectionChain;

            foreach(QSharedPointer<ConnectivityInterface> pathInterface, connectionPath)
            {
                if (pathInterface != startInterface && pathInterface != endInterface)
                {
                    if (pathInterface->getMode().compare(
                        QStringLiteral("mirroredSlave"), Qt::CaseInsensitive) == 0)
                    {
                        mirroredSlaveAddressChange += pathInterface->getRemapAddress().toULongLong();

                        memoryMapEndAddress = pathInterface->getRemapRange().toULongLong() - 1;
                        hasRemapRange = true;
                    }
                    else if (pathInterface->getMode().compare(QStringLiteral("master"), Qt::CaseInsensitive) == 0)
                    {
                        if (pathInterface->isConnectedToMemory())
                        {
                            MainMemoryGraphicsItem* connectionMiddleItem = getMainGraphicsItem(
                                pathInterface, MemoryDesignerConstants::ADDRESSSPACECOLUMN_NAME);
                            if (connectionMiddleItem)
                            {
                                if (filterAddressSpaceChains_)
                                {
                                    connectionMiddleItem->hide();
                                }
                                else
                                {
                                    MemoryConnectionItem* newSpaceConnection = createSpaceConnection(
                                        connectionStartItem, connectionMiddleItem, pathInterface, spaceColumn,
                                        placedSpaceItems, spaceItemChain, spaceYPlacement);
                                    if (newSpaceConnection)
                                    {
                                        connectionChain.append(newSpaceConnection);
                                    }

                                    spaceItemChain.append(connectionMiddleItem);

                                    connectionStartItem = connectionMiddleItem;
                                }
                            }
                        }

                        if (!placedSpaceItems->contains(connectionStartItem) && !filterAddressSpaceChains_)
                        {
                            baseAddressNumber = pathInterface->getBaseAddress().toULongLong();
                        }
                        else
                        {
                            mirroredSlaveAddressChange += pathInterface->getBaseAddress().toULongLong();
                        }
                    }
                }
            }

            if (!connectionStartItem->isVisible())
            {
                connectionStartItem->setVisible(true);
            }

            bool spaceItemPlaced = false;
            if (!placedSpaceItems->contains(connectionStartItem))
            {
                placedSpaceItems->append(connectionStartItem);
                spaceItemPlaced = true;
            }

            quint64 memoryMapBaseAddress = connectionEndItem->getBaseAddress();

            if (!hasRemapRange)
            {
                memoryMapEndAddress = connectionEndItem->getLastAddress();
            }

            quint64 endAddressNumber = baseAddressNumber + memoryMapEndAddress;

            quint64 remappedAddress = baseAddressNumber + mirroredSlaveAddressChange;
            quint64 remappedEndAddress = endAddressNumber + mirroredSlaveAddressChange;

            quint64 yTransfer = (memoryMapBaseAddress + mirroredSlaveAddressChange) *
                MemoryDesignerConstants::RANGEINTERVAL;

            QPointF startConnectionPosBefore = connectionStartItem->pos();

            if (!placedMapItems->contains(connectionEndItem))
            {
                connectionEndItem->hideMemoryRangeLabels();
                connectionEndItem->setPos(connectionEndItem->pos().x(), connectionStartItem->pos().y() + yTransfer);

                QPointF startItemPositionBefore = connectionStartItem->pos();

                if (spaceItemPlaced)
                {
                    repositionMemoryMap(placedMapItems, placedSpaceItems, connectionStartItem, spaceColumn,
                        connectionEndItem, yTransfer);
                }
                else
                {
                    checkMemoryMapRepositionToOverlapColumn(placedMapItems, connectionEndItem, memoryMapColumn,
                        remappedAddress, remappedEndAddress, connectionStartItem);
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

            MemoryConnectionItem* newConnectionItem =
                new MemoryConnectionItem(connectionStartItem, remappedAddress, remappedEndAddress,
                connectionEndItem, spaceColumn->scene(), condenseMemoryItems_, yTransfer);
            connectionChain.append(newConnectionItem);
            setHeightForConnectionChain(connectionChain);
            connectionsToMemoryMaps_.append(newConnectionItem);

            qreal spaceItemEndPointAfter = connectionStartItem->getSceneEndPoint();

            spaceYPlacement += spaceItemEndPointAfter - spaceItemEndPointBefore;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::getMainGraphicsItem()
//-----------------------------------------------------------------------------
MainMemoryGraphicsItem* MemoryDesignerDiagram::getMainGraphicsItem(
    QSharedPointer<ConnectivityInterface> connectionInterface, QString columnType) const
{
    MainMemoryGraphicsItem* graphicsItem = 0;

    QSharedPointer<MemoryItem> memoryItem = connectionInterface->getConnectedMemory();
    QSharedPointer<ConnectivityComponent> connectionInstance = connectionInterface->getInstance();
    if (memoryItem && connectionInstance)
    {
        QVector<MemoryColumn*> matchingColumns = getSpecifiedColumns(columnType);
        foreach (MemoryColumn* currentColumn, matchingColumns)
        {
            graphicsItem = currentColumn->findGraphicsItemByMemoryItem(memoryItem);
            if (graphicsItem)
            {
                break;
            }
        }
    }

    return graphicsItem;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::createSpaceConnection()
//-----------------------------------------------------------------------------
MemoryConnectionItem* MemoryDesignerDiagram::createSpaceConnection(MainMemoryGraphicsItem* connectionStartItem,
    MainMemoryGraphicsItem* connectionMiddleItem, QSharedPointer<ConnectivityInterface> newSpaceInterface,
    MemoryColumn* spaceColumn, QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems,
    QVector<MainMemoryGraphicsItem*> spaceItemChain, int& spaceYPlacement)
{
    MemoryConnectionItem* addressSpaceConnection = 0;

    if (connectionMiddleItem)
    {
        connectionMiddleItem->hideMemoryRangeLabels();

        int startItemPositionY = connectionStartItem->pos().y();
        if (!placedSpaceItems->contains(connectionMiddleItem))
        {
            spaceColumn->moveGraphicsItem(connectionMiddleItem, startItemPositionY,
                MemoryDesignerConstants::SPACEITEMINTERVAL);
            connectionMiddleItem->hideFirstAndLastSegmentRange();
        }

        bool recalculateYPosition = false;
        if (!placedSpaceItems->contains(connectionStartItem))
        {
            quint64 middleItemY = connectionMiddleItem->pos().y();

            changeMasterAddressSpaceColumn(connectionStartItem, middleItemY, spaceColumn, spaceItemChain);

            placedSpaceItems->append(connectionStartItem);
            recalculateYPosition = true;
        }

        quint64 interfaceBaseAddress = newSpaceInterface->getBaseAddress().toULongLong();
        quint64 startItemBaseAddress = connectionStartItem->getBaseAddress();
        quint64 baseAddressDifference = interfaceBaseAddress - startItemBaseAddress;
        int yTransfer = baseAddressDifference * MemoryDesignerConstants::RANGEINTERVAL;

        quint64 middleItemRangeStart = connectionMiddleItem->getBaseAddress() + interfaceBaseAddress;
        quint64 middleItemRangeEnd = connectionMiddleItem->getLastAddress() + interfaceBaseAddress;

        if (!placedSpaceItems->contains(connectionMiddleItem))
        {
            connectionMiddleItem->setPos(
                connectionMiddleItem->pos().x(), connectionMiddleItem->pos().y() + yTransfer);
        }

        if (recalculateYPosition)
        {
            quint64 newYPosition =
                connectionMiddleItem->sceneBoundingRect().height() + MemoryDesignerConstants::SPACEITEMINTERVAL;
            if (newYPosition > spaceYPlacement)
            {
                spaceYPlacement = newYPosition;
            }
        }

        if (!placedSpaceItems->contains(connectionMiddleItem) || !placedSpaceItems->contains(connectionStartItem))
        {
            addressSpaceConnection = new MemoryConnectionItem(connectionStartItem, middleItemRangeStart,
                middleItemRangeEnd, connectionMiddleItem, spaceColumn->scene(), condenseMemoryItems_, yTransfer);
        }
        else
        {
            addressSpaceConnection = getAddressSpaceChainConnection(connectionStartItem, connectionMiddleItem);
        }
    }

    return addressSpaceConnection;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::changeMasterAddressSpaceColumn()
//-----------------------------------------------------------------------------
void MemoryDesignerDiagram::changeMasterAddressSpaceColumn(MainMemoryGraphicsItem* masterSpaceItem,
    qreal spaceItemY, GraphicsColumn* originalColumn, QVector<MainMemoryGraphicsItem*> spaceItemChain)
{
    originalColumn->removeItem(masterSpaceItem);

    qreal columnWidth = originalColumn->boundingRect().width();
    qreal currentColumnPosition = originalColumn->pos().x() - columnWidth;
    GraphicsColumn* currentColumn = 0;
    while (currentColumnPosition >= 0)
    {
        currentColumn = layout_->findColumnAt(QPointF(currentColumnPosition, 0));
        if (currentColumn &&
            currentColumn->name().contains(MemoryDesignerConstants::ADDRESSSPACECOLUMN_NAME, Qt::CaseInsensitive))
        {
            currentColumn->addItem(masterSpaceItem);
            masterSpaceItem->setPos(masterSpaceItem->pos().x(), spaceItemY);

            if (!masterSpaceItem->itemCollidesWithSimilarItems())
            {
                return;
            }
            else
            {
                changeCollidingMasterAddressSpaceColumn(currentColumn, spaceItemChain);

                if (!masterSpaceItem->itemCollidesWithSimilarItems())
                {
                    return;
                }
                else
                {
                    currentColumn->removeItem(masterSpaceItem);
                }
            }
        }

        currentColumnPosition -= columnWidth;
    }

    MemoryColumn* newSpaceColumn = createAddressSpaceColumn();
    newSpaceColumn->setZValue(-1);
    newSpaceColumn->addItem(masterSpaceItem);
    masterSpaceItem->setPos(masterSpaceItem->pos().x(), spaceItemY);
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::changeCollidingMasterAddressSpaceColumn()
//-----------------------------------------------------------------------------
void MemoryDesignerDiagram::changeCollidingMasterAddressSpaceColumn(GraphicsColumn* currentColumn,
    QVector<MainMemoryGraphicsItem*> spaceItemChain)
{
    foreach (MainMemoryGraphicsItem* currentSpaceChainItem, spaceItemChain)
    {
        GraphicsColumn* chainParent =
            dynamic_cast<GraphicsColumn*>(currentSpaceChainItem->parentItem());
        if (chainParent && chainParent == currentColumn)
        {
            changeMasterAddressSpaceColumn(
                currentSpaceChainItem, currentSpaceChainItem->pos().y(), currentColumn, spaceItemChain);
            return;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::getAddressSpaceChainConnection()
//-----------------------------------------------------------------------------
MemoryConnectionItem* MemoryDesignerDiagram::getAddressSpaceChainConnection(
    MainMemoryGraphicsItem* connectionStartItem, MainMemoryGraphicsItem* connectionMiddleItem) const
{
    QMapIterator<quint64, MemoryConnectionItem*> connectionIterator(connectionStartItem->getMemoryConnections());
    while (connectionIterator.hasNext())
    {
        connectionIterator.next();

        MemoryConnectionItem* connectionItem = connectionIterator.value();
        if (connectionMiddleItem->hasConnection(connectionItem))
        {
            return connectionItem;
        }
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::setHeightForConnectionChain()
//-----------------------------------------------------------------------------
void MemoryDesignerDiagram::setHeightForConnectionChain(QVector<MemoryConnectionItem*> connectionChain)
{
    foreach (MemoryConnectionItem* connectionItem, connectionChain)
    {
        connectionItem->setConnectionWidth();
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::checkMemoryMapRepositionToOverlapColumn()
//-----------------------------------------------------------------------------
void MemoryDesignerDiagram::checkMemoryMapRepositionToOverlapColumn(
    QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMaps, MainMemoryGraphicsItem* memoryItem,
    MemoryColumn* originalColumn, quint64 mapBaseAddress, quint64 mapLastAddress,
    MainMemoryGraphicsItem* connectionStartItem)
{
    QRectF selectedItemRect = memoryItem->sceneBoundingRect();

    int selectedItemPenWidth = memoryItem->pen().width();

    QVector<MainMemoryGraphicsItem*> connectedSpaceItems;
    connectedSpaceItems.append(connectionStartItem);

    if (originalColumn->memoryMapOverlapsInColumn(memoryItem, mapBaseAddress, mapLastAddress, selectedItemRect,
        selectedItemPenWidth, connectedSpaceItems, placedMaps))
    {
        foreach (GraphicsColumn* column, layout_->getColumns())
        {
            if (column->name().contains(QStringLiteral("Overlap"), Qt::CaseInsensitive))
            {
                MemoryColumn* memoryColumn = dynamic_cast<MemoryColumn*>(column);
                if (memoryColumn)
                {
                    selectedItemRect.setX(selectedItemRect.x() + memoryColumn->boundingRect().width());

                    if (!memoryColumn->memoryMapOverlapsInColumn(memoryItem, mapBaseAddress, mapLastAddress,
                        selectedItemRect, selectedItemPenWidth, connectedSpaceItems, placedMaps))
                    {
                        originalColumn->removeItem(memoryItem);
                        memoryColumn->addItem(memoryItem);
                        return;
                    }
                }
            }
        }

        MemoryColumn* overlapColumn = createMemoryOverlapColumn();
        originalColumn->removeItem(memoryItem);
        overlapColumn->addItem(memoryItem);
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::repositionMemoryMap()
//-----------------------------------------------------------------------------
void MemoryDesignerDiagram::repositionMemoryMap(QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMapItems,
    QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems,
    MainMemoryGraphicsItem* startConnectionItem, MemoryColumn* addressSpaceColumn,
    MainMemoryGraphicsItem* endConnectionItem, int memoryMapYTransfer)
{
    QRectF endConnectionRectangle = endConnectionItem->sceneBoundingRect();
    int endConnectionPenWidth = endConnectionItem->pen().width();
    
    foreach (MemoryDesignerGraphicsItem* memoryMapItem, *placedMapItems)
    {
        QRectF comparisonRectangle = memoryMapItem->sceneBoundingRect();
        int comparisonPenWidth = memoryMapItem->pen().width();

        if (endConnectionItem != memoryMapItem && MemoryDesignerConstants::itemOverlapsAnotherItem(
            endConnectionRectangle, endConnectionPenWidth, comparisonRectangle, comparisonPenWidth))
        {
            int mapItemYTransfer = memoryMapItem->sceneBoundingRect().bottom();

            endConnectionItem->setPos(endConnectionItem->x(), mapItemYTransfer);

            startConnectionItem->setPos(
                startConnectionItem->pos().x(), endConnectionItem->pos().y() - memoryMapYTransfer);

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
            int spaceItemYTransfer =
                spaceItem->sceneBoundingRect().bottom() + MemoryDesignerConstants::SPACEITEMINTERVAL;

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

    foreach (GraphicsColumn* column, layout_->getColumns())
    {
        MemoryColumn* currentSpaceColumn = dynamic_cast<MemoryColumn*>(column);
        if (currentSpaceColumn && currentSpaceColumn->name().contains(
            MemoryDesignerConstants::ADDRESSSPACECOLUMN_NAME, Qt::CaseInsensitive))
        {
            currentSpaceColumn->addItem(spaceItem);
            spaceItem->setPos(spaceItem->pos().x(), targetItem->pos().y() + yTransfer);

            if (spaceItem->itemCollidesWithSimilarItems())
            {
                currentSpaceColumn->removeItem(spaceItem);
            }
            else
            {
                return;
            }
        }
    }

    MemoryColumn* newSpaceColumn = createAddressSpaceColumn();
    newSpaceColumn->addItem(spaceItem);
    spaceItem->setPos(spaceItem->pos().x(), targetItem->pos().y());
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::reDrawConnections()
//-----------------------------------------------------------------------------
void MemoryDesignerDiagram::reDrawConnections(QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems)
{
    foreach (MainMemoryGraphicsItem* spaceItem, *placedSpaceItems)
    {
        spaceItem->reDrawConnections();
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::moveUnconnectedAddressSpaces()
//-----------------------------------------------------------------------------
void MemoryDesignerDiagram::moveUnconnectedAddressSpaces(
    QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems, MemoryColumn* spaceColumn)
{
    if (!placedSpaceItems->isEmpty())
    {
        spaceColumn->moveUnconnectedMemoryItems(placedSpaceItems);
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::moveUnconnectedMemoryMaps()
//-----------------------------------------------------------------------------
void MemoryDesignerDiagram::moveUnconnectedMemoryMaps(
    QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMapItems, MemoryColumn* memoryMapColumn)
{
    if (!placedMapItems->isEmpty())
    {
        memoryMapColumn->moveUnconnectedMemoryItems(placedMapItems);
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
        if (memoryColumn && memoryColumn->name().compare(columnName) == 0)
        {
            return memoryColumn;
        }
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::createOverlappingConnectionMarkers()
//-----------------------------------------------------------------------------
void MemoryDesignerDiagram::createOverlappingConnectionMarkers(
    QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems)
{
    foreach (MainMemoryGraphicsItem* spaceItem, *placedSpaceItems)
    {
        spaceItem->createOverlappingConnectionMarkers();
        
        foreach (MemoryCollisionItem* collisionItem, spaceItem->getMemoryCollisions())
        {
            memoryCollisions_.append(collisionItem);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::compressGraphicsItems()
//-----------------------------------------------------------------------------
void MemoryDesignerDiagram::compressGraphicsItems(
    QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems, int& spaceYPlacement,
    MemoryColumn* spaceColumn)
{
    QSharedPointer<QVector<MemoryConnectionItem*> > movedConnectionItems (new QVector<MemoryConnectionItem*>());

    foreach (GraphicsColumn* graphicsColumn, layout_->getColumns())
    {
        MemoryColumn* memoryColumn = dynamic_cast<MemoryColumn*>(graphicsColumn);
        if (memoryColumn)
        {
            memoryColumn->compressGraphicsItems(
                condenseMemoryItems_, spaceYPlacement, spaceColumn, placedSpaceItems, movedConnectionItems);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::repositionCompressedMemoryMaps()
//-----------------------------------------------------------------------------
void MemoryDesignerDiagram::repositionCompressedMemoryMaps(
    QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMapItems, MemoryColumn* memoryMapColumn)
{
    foreach (MainMemoryGraphicsItem* mapItem, *placedMapItems)
    {
        QGraphicsItem* mapParentItem = mapItem->parentItem();
        MemoryColumn* originalColumn = dynamic_cast<MemoryColumn*>(mapParentItem);
        if (originalColumn && originalColumn != memoryMapColumn)
        {
            quint64 mapBaseAddress = mapItem->getBaseAddress();
            quint64 mapLastAddress = mapItem->getLastAddress();

            QRectF mapRectangle = mapItem->sceneBoundingRect();
            int mapPenWidth = mapItem->pen().width();

            if (mapItem->hasExtensionItem())
            {
                mapRectangle.setHeight(mapRectangle.height() + mapItem->getExtensionItem()->boundingRect().height());
            }

            QVector<MainMemoryGraphicsItem*> connectedSpaceItems;
            foreach (MemoryConnectionItem* connectionItem, mapItem->getMemoryConnections())
            {
                MainMemoryGraphicsItem* connectionStartItem = connectionItem->getConnectionStartItem();
                if (!connectedSpaceItems.contains(connectionStartItem))
                {
                    connectedSpaceItems.append(connectionStartItem);
                }
            }

            int columnWidth = originalColumn->sceneBoundingRect().width();

            QPointF columnPoint (originalColumn->pos().x() - columnWidth, mapRectangle.y());
            MemoryColumn* comparisonColumn = dynamic_cast<MemoryColumn*>(layout_->findColumnAt(columnPoint));
            if (comparisonColumn)
            {
                while (comparisonColumn && !comparisonColumn->name().contains(
                    MemoryDesignerConstants::ADDRESSSPACECOLUMN_NAME, Qt::CaseInsensitive))
                {
                    mapRectangle.setX(mapRectangle.x() - columnWidth);

                    if (!comparisonColumn->memoryMapOverlapsInColumn(mapItem, mapBaseAddress, mapLastAddress,
                        mapRectangle, mapPenWidth, connectedSpaceItems, placedMapItems))
                    {
                        originalColumn->removeItem(mapItem);
                        comparisonColumn->addItem(mapItem, true);

                        if (originalColumn->getItems().isEmpty() && originalColumn->name().contains(
                            MemoryDesignerConstants::MEMORYMAPOVERLAPCOLUMN_NAME, Qt::CaseInsensitive))
                        {
                            layout_->removeColumn(originalColumn);
                        }

                        break;
                    }

                    columnPoint.setX(columnPoint.x() - columnWidth);
                    comparisonColumn = dynamic_cast<MemoryColumn*>(layout_->findColumnAt(columnPoint));
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::getSpecifiedColumns()
//-----------------------------------------------------------------------------
QVector<MemoryColumn*> MemoryDesignerDiagram::getSpecifiedColumns(QString const& columnSpecification) const
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

        event->accept();
    }
    else if (event->modifiers() == Qt::ALT && !addressBlockRegistersAreFiltered() && !fieldsAreFiltered())
    {
        qreal deltaWidth = event->delta();
        deltaWidth = -deltaWidth;

        qreal previousWidthBoundary = widthBoundary_;
        widthBoundary_ += deltaWidth;
        if (widthBoundary_ < 0)
        {
            deltaWidth = previousWidthBoundary;
            widthBoundary_ = 0;
        }

        if (deltaWidth != 0)
        {
            changeMemoryMapWidths(deltaWidth);
        }

        event->accept();
    }
    else
    {
        QGraphicsScene::wheelEvent(event);
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::changeWidths()
//-----------------------------------------------------------------------------
void MemoryDesignerDiagram::changeMemoryMapWidths(qreal deltaWidth)
{
    foreach (GraphicsColumn* column, layout_->getColumns())
    {
        MemoryColumn* memoryColumn = dynamic_cast<MemoryColumn*>(column);
        if (memoryColumn && memoryColumn->containsMemoryMapItems())
        {
            memoryColumn->changeWidth(deltaWidth);
        }
    }
    foreach (MemoryConnectionItem* connectionItem, connectionsToMemoryMaps_)
    {
        connectionItem->reDrawConnection();
    }
    foreach (MemoryCollisionItem* collisionItem, memoryCollisions_)
    {
        collisionItem->reDrawCollision();
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
