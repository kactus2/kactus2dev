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

#include <library/LibraryInterface.h>

#include <common/graphicsItems/GraphicsColumnLayout.h>

#include <designEditors/common/diagramgrid.h>

#include <designEditors/MemoryDesigner/MemoryDesignDocument.h>
#include <designEditors/MemoryDesigner/MemoryDesignConstructor.h>

#include <IPXACTmodels/common/VLNV.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>

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
memoryConstructor_(new MemoryDesignConstructor(layout_))
{
    connect(memoryConstructor_, SIGNAL(openComponentDocument(VLNV const&, QVector<QString>)),
        this, SIGNAL(openComponentDocument(VLNV const&, QVector<QString>)), Qt::UniqueConnection);

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
    memoryConstructor_->setCondenseMemoryItems(condenseMemoryItems);
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::memoryItemsAreCondensed()
//-----------------------------------------------------------------------------
bool MemoryDesignerDiagram::memoryItemsAreCondensed() const
{
    return memoryConstructor_->memoryItemsAreCondensed();
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::setFilterAddressSpaceChains()
//-----------------------------------------------------------------------------
void MemoryDesignerDiagram::setFilterAddressSpaceChains(bool filterChains)
{
    memoryConstructor_->setFilterAddressSpaceChains(filterChains);
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::addressSpaceChainsAreFiltered()
//-----------------------------------------------------------------------------
bool MemoryDesignerDiagram::addressSpaceChainsAreFiltered() const
{
    return memoryConstructor_->addressSpaceChainsAreFiltered();
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::condenseFieldItems()
//-----------------------------------------------------------------------------
void MemoryDesignerDiagram::condenseFieldItems()
{
    memoryConstructor_->condenseFieldItems();
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::extendFieldItems()
//-----------------------------------------------------------------------------
void MemoryDesignerDiagram::extendFieldItems()
{
    memoryConstructor_->extendFieldItems();
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::setFilterAddressSpaceSegments()
//-----------------------------------------------------------------------------
void MemoryDesignerDiagram::setFilterAddressSpaceSegments(bool filterSegments)
{
    memoryConstructor_->setFilterAddressSpaceSegments(filterSegments);
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::addressSpaceSegmentsAreFiltered()
//-----------------------------------------------------------------------------
bool MemoryDesignerDiagram::addressSpaceSegmentsAreFiltered() const
{
    return memoryConstructor_->addressSpaceSegmentsAreFiltered();
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::setFilterAddressBlocks()
//-----------------------------------------------------------------------------
void MemoryDesignerDiagram::setFilterAddressBlocks(bool filterBlocks)
{
    memoryConstructor_->setFilterAddressBlocks(filterBlocks);
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::addressBlocksAreFiltered()
//-----------------------------------------------------------------------------
bool MemoryDesignerDiagram::addressBlocksAreFiltered() const
{
    return memoryConstructor_->addressBlocksAreFiltered();
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::setFilterAddressBlockRegisters()
//-----------------------------------------------------------------------------
void MemoryDesignerDiagram::setFilterAddressBlockRegisters(bool filterRegisters)
{
    memoryConstructor_->setFilterAddressBlockRegisters(filterRegisters);
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::addressBlockRegistersAreFiltered()
//-----------------------------------------------------------------------------
bool MemoryDesignerDiagram::addressBlockRegistersAreFiltered() const
{
    return memoryConstructor_->addressBlockRegistersAreFiltered();
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::setFilterFields()
//-----------------------------------------------------------------------------
void MemoryDesignerDiagram::setFilterFields(bool filterFields)
{
    memoryConstructor_->setFilterFields(filterFields);
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::fieldsAreFiltered()
//-----------------------------------------------------------------------------
bool MemoryDesignerDiagram::fieldsAreFiltered() const
{
    return memoryConstructor_->fieldsAreFiltered();
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerDiagram::loadDesign()
//-----------------------------------------------------------------------------
bool MemoryDesignerDiagram::loadDesignFromCurrentView(QSharedPointer<const Component> component,
    QString const& viewName)
{
    clearScene();

    QSharedPointer<ConnectivityGraph> connectionGraph =
        instanceLocator_.createConnectivityGraph(component, viewName);
    if (connectionGraph)
    {
        return memoryConstructor_->constructMemoryDesignItems(connectionGraph);
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
    memoryConstructor_->setNewLayout(layout_);
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

        qreal previousWidthBoundary = memoryConstructor_->getWidthBoundary();
        int newWidthBoundary = previousWidthBoundary + deltaWidth;
        if (newWidthBoundary < 0)
        {
            deltaWidth = previousWidthBoundary;
            newWidthBoundary = 0;
        }

        if (deltaWidth != 0)
        {
            memoryConstructor_->setNewWidthBoundary(newWidthBoundary);

            memoryConstructor_->changeMemoryMapWidths(deltaWidth);
        }

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
        for (qreal y = top; y < rect.bottom(); y += GridSize * 3)
        {
            painter->drawPoint(x, y);
        }
    }
}
