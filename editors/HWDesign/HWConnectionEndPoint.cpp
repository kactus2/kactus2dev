//-----------------------------------------------------------------------------
// File: HWConnectionEndpoint.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti Maatta
// Date: 18.3.2011
//
// Description:
// Interface class for the diagram connection end points.
//-----------------------------------------------------------------------------

#include "HWConnectionEndpoint.h"

#include <editors/common/diagramgrid.h>
#include <editors/common/DesignDiagram.h>

#include <editors/HWDesign/HWComponentItem.h>
#include <editors/HWDesign/OffPageConnectorItem.h>

#include <common/KactusColors.h>

#include <IPXACTmodels/generaldeclarations.h>

#include <QPen>
#include <QFont>
#include <QGraphicsDropShadowEffect>

//-----------------------------------------------------------------------------
// Function: HWConnectionEndpont::HWConnectionEndpoint()
//-----------------------------------------------------------------------------
HWConnectionEndpoint::HWConnectionEndpoint(QString const& name, QSharedPointer<Component> containingComponent,
    QGraphicsItem* parent, QVector2D const& dir) :
    ConnectionEndpoint(parent),
    containingComponent_(containingComponent)
{
    setDirection(dir);

    QFont font = nameLabel_->font();
    font.setPointSize(8);
    nameLabel_->setFont(font);
    nameLabel_->setFlag(ItemStacksBehindParent);
    nameLabel_->setText(name);

    setFlag(ItemIsMovable);
    setFlag(ItemIsSelectable);
    setFlag(ItemSendsGeometryChanges);
    setFlag(ItemSendsScenePositionChanges);

    offPageConnector_ = new OffPageConnectorItem(this);
    offPageConnector_->setPos(0.0, -GridSize * 3);
    offPageConnector_->setFlag(ItemStacksBehindParent);
    offPageConnector_->setVisible(false);

    if (parent)
    {
        auto parentHW = dynamic_cast<HWComponentItem*>(parent);
        if (parentHW)
        {
            parentComponentItem_ = parentHW;
        }
    }
}


//-----------------------------------------------------------------------------
// Function: HWConnectionEndPoint::HWConnectionEndpoint()
//-----------------------------------------------------------------------------
ComponentItem* HWConnectionEndpoint::encompassingComp() const
{
    return parentComponentItem_;
}

//-----------------------------------------------------------------------------
// Function: HWConnectionEndpoint::updateInterface()
//-----------------------------------------------------------------------------
void HWConnectionEndpoint::updateInterface()
{
    ConnectionEndpoint::updateInterface();

    General::InterfaceMode mode = getInterfaceMode();

    if (mode == General::MASTER)
    {
        setBrush(QBrush(KactusColors::MASTER_INTERFACE));
    }
    else if (mode == General::SLAVE)
    {
        setBrush(QBrush(KactusColors::SLAVE_INTERFACE));
    }
    else if (mode == General::MIRRORED_MASTER)
    {
        setBrush(QBrush(KactusColors::MIRROREDMASTER_INTERFACE));
    }
    else if (mode == General::MIRRORED_SLAVE)
    {
        setBrush(QBrush(KactusColors::MIRROREDSLAVE_INTERFACE));
    }
    else if (mode == General::SYSTEM)
    {
        setBrush(QBrush(KactusColors::SYSTEM_INTERFACE));
    }
    else if (mode == General::MIRRORED_SYSTEM)
    {
        setBrush(QBrush(KactusColors::MIRROREDSYSTEM_INTERFACE));
    }
    else if (mode == General::MONITOR)
    {
        setBrush(QBrush(KactusColors::MONITOR_INTERFACE));
    }
    else // if undefined
    {
        setBrush(QBrush(KactusColors::INVALID_INTERFACE));
    }

    updateEndPointGraphics();

    nameLabel_->updateLabelGeometry();

    setLabelPosition();

    offPageConnector_->updateInterface();
}

//-----------------------------------------------------------------------------
// Function: HWConnectionEndpont::getNameLength()
//-----------------------------------------------------------------------------
qreal HWConnectionEndpoint::getNameLength()
{
	return name().length();
}

//-----------------------------------------------------------------------------
// Function: HWConnectionEndPoint::getNameLabel()
//-----------------------------------------------------------------------------
GraphicsItemLabel* HWConnectionEndpoint::getNameLabel() const
{
    return nameLabel_;
}

//-----------------------------------------------------------------------------
// Function: HWConnectionEndPoint::getOwnerComponent()
//-----------------------------------------------------------------------------
QSharedPointer<Component> HWConnectionEndpoint::getOwnerComponent() const
{
    return containingComponent_;
}

//-----------------------------------------------------------------------------
// Function: HWConnectionEndPoint::getOffPageConnector()
//-----------------------------------------------------------------------------
ConnectionEndpoint* HWConnectionEndpoint::getOffPageConnector()
{
    return offPageConnector_;
}

//-----------------------------------------------------------------------------
// Function: HWConnectionEndPoint::sceneIsLocked()
//-----------------------------------------------------------------------------
bool HWConnectionEndpoint::sceneIsLocked() const
{
    auto diagram = dynamic_cast<DesignDiagram*>(scene());
    if (diagram != nullptr && diagram->isProtected())
    {
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: HWConnectionEndPoint::mouseMoveEvent()
//-----------------------------------------------------------------------------
void HWConnectionEndpoint::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (!sceneIsLocked())
    {
        ConnectionEndpoint::mouseMoveEvent(event);

        moveItemByMouse();
    }
}

//-----------------------------------------------------------------------------
// Function: HWConnectionEndPoint::setName()
//-----------------------------------------------------------------------------
void HWConnectionEndpoint::setName(QString const& /*name*/)
{
    // Intentionally empty.
}
