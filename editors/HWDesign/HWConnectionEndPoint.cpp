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
// Function: HWConnectionEndpoint::updateInterface()
//-----------------------------------------------------------------------------
void HWConnectionEndpoint::updateInterface()
{
    ConnectionEndpoint::updateInterface();

    General::InterfaceMode mode = getInterfaceMode();

    static const QMap<General::InterfaceMode, QColor> interfaceColors({
        {General::MASTER, KactusColors::MASTER_INTERFACE},
        {General::INITIATOR, KactusColors::MASTER_INTERFACE},
        {General::SLAVE, KactusColors::SLAVE_INTERFACE},
        {General::TARGET, KactusColors::SLAVE_INTERFACE},
        {General::MIRRORED_MASTER, KactusColors::MIRROREDMASTER_INTERFACE},
        {General::MIRRORED_INITIATOR, KactusColors::MIRROREDSLAVE_INTERFACE},
        {General::MIRRORED_SLAVE, KactusColors::MIRROREDSLAVE_INTERFACE},
        {General::MIRRORED_TARGET, KactusColors::MIRROREDSLAVE_INTERFACE},
        {General::SYSTEM, KactusColors::SYSTEM_INTERFACE},
        {General::MIRRORED_SYSTEM, KactusColors::MIRROREDSYSTEM_INTERFACE},
        {General::MONITOR, KactusColors::MONITOR_INTERFACE}
    });

    auto const brushColor = interfaceColors.value(mode, KactusColors::INVALID_INTERFACE);
    setBrush(QBrush(brushColor));

    updateEndPointGraphics();

    nameLabel_->updateLabelGeometry();

    setLabelPosition();

    offPageConnector_->updateInterface();
}

//-----------------------------------------------------------------------------
// Function: HWConnectionEndPoint::getOwnerComponent()
//-----------------------------------------------------------------------------
QSharedPointer<Component> HWConnectionEndpoint::getOwnerComponent() const
{
    return containingComponent_;
}

//-----------------------------------------------------------------------------
// Function: HWConnectionEndPoint::setName()
//-----------------------------------------------------------------------------
void HWConnectionEndpoint::setName(QString const& /*name*/)
{
    // Intentionally empty.
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
// Function: HWConnectionEndPoint::getOffPageConnector()
//-----------------------------------------------------------------------------
ConnectionEndpoint* HWConnectionEndpoint::getOffPageConnector()
{
    return offPageConnector_;
}

//-----------------------------------------------------------------------------
// Function: HWConnectionEndPoint::HWConnectionEndpoint()
//-----------------------------------------------------------------------------
ComponentItem* HWConnectionEndpoint::encompassingComp() const
{
    return parentComponentItem_;
}

//-----------------------------------------------------------------------------
// Function: HWConnectionEndPoint::sceneIsLocked()
//-----------------------------------------------------------------------------
bool HWConnectionEndpoint::sceneIsLocked() const
{
    auto diagram = dynamic_cast<DesignDiagram*>(scene());
    return diagram != nullptr && diagram->isProtected();
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
