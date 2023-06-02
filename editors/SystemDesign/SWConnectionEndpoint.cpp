//-----------------------------------------------------------------------------
// File: SWConnectionEndpoint.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti Maatta
// Date: 2.5.2012
//
// Description:
// Common interface for SW connection endpoints.
//-----------------------------------------------------------------------------

#include "SWConnectionEndpoint.h"

#include <IPXACTmodels/kactusExtensions/ComInterface.h>

#include <common/KactusColors.h>

#include <editors/common/diagramgrid.h>
#include <editors/HWDesign/OffPageConnectorItem.h>
#include <editors/SystemDesign/SWComponentItem.h>

#include <QPen>
#include <QFont>
#include <QGraphicsDropShadowEffect>

//-----------------------------------------------------------------------------
// Function: SWConnectionEndpoint()
//-----------------------------------------------------------------------------
SWConnectionEndpoint::SWConnectionEndpoint(QSharedPointer<Component> component, std::string_view name,
    QGraphicsItem* parent, QVector2D const& dir):
ConnectionEndpoint(parent),
type_(ConnectionEndpoint::ENDPOINT_TYPE_UNDEFINED),
nameLabel_(new GraphicsItemLabel(QString(), this)),
containingComponent_(component)
{
    nameLabel_->setText(QString::fromStdString(std::string(name)));

    ConnectionEndpoint::setDirection(dir);

    if (parent)
    {
        auto parentSW = dynamic_cast<SystemComponentItem*>(parent);
        if (parentSW)
        {
            parentItem_ = parentSW;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: SWConnectionEndpoint::encompassingComp()
//-----------------------------------------------------------------------------
ComponentItem* SWConnectionEndpoint::encompassingComp() const
{
    return parentItem_;
}

//-----------------------------------------------------------------------------
// Function: SWConnectionEndpoint::getOwnerComponent()
//-----------------------------------------------------------------------------
QSharedPointer<Component> SWConnectionEndpoint::getOwnerComponent() const
{
    return containingComponent_;
}

//-----------------------------------------------------------------------------
// Function: SWConnectionEndpoint::getNameLabel()
//-----------------------------------------------------------------------------
GraphicsItemLabel* SWConnectionEndpoint::getNameLabel() const
{
    return nameLabel_;
}

//-----------------------------------------------------------------------------
// Function: SWConnectionEndpoint::getOffPageConnector()
//-----------------------------------------------------------------------------
ConnectionEndpoint* SWConnectionEndpoint::getOffPageConnector()
{
    return offPageConnector_;
}

//-----------------------------------------------------------------------------
// Function: SWConnectionEndpoint::updateInterface()
//-----------------------------------------------------------------------------
void SWConnectionEndpoint::updateInterface()
{
    ConnectionEndpoint::updateInterface();

    if (isInvalid())
    {
        setBrush(QBrush(Qt::red));
        return;
    }

    ConnectionEndpoint::EndpointType endPointType = getType();
    if (endPointType == ENDPOINT_TYPE_UNDEFINED)
    {
        setBrush(QBrush(KactusColors::INVALID_INTERFACE));
    }
    else if (endPointType == ENDPOINT_TYPE_API)
    {
        setBrush(QBrush(KactusColors::ENDPOINT_TYPE_API));
    }
    else if (endPointType == ENDPOINT_TYPE_COM)
    {
        // Generate a hexadecimal color based on the transfer type.
        unsigned int color = qHash(getComInterface()->getTransferType()) & 0x00FFFFFF;

        // Extract the color components and make small modifications.
        unsigned char r = (color - 0xDEADC0DE) >> 16;
        unsigned char g = 255 - ((color & 0x0000FF00) >> 8);
        unsigned char b = (color & 0x000000FF);

        setBrush(QBrush(QColor(r, g, b)));
    }
    else
    {
        Q_ASSERT(false);
    }
}

//-----------------------------------------------------------------------------
// Function: SWConnectionEndpoint::isConnectionValid()
//-----------------------------------------------------------------------------
bool SWConnectionEndpoint::isConnectionValid(ConnectionEndpoint const* other) const
{
    if (!ConnectionEndpoint::isConnectionValid(other))
    {
        return false;
    }

    bool fullyDefined = getType() != ENDPOINT_TYPE_UNDEFINED && other->getType() != ENDPOINT_TYPE_UNDEFINED;

    // Check if the types do not match.
    if (fullyDefined && getType() != other->getType())
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: SWConnectionEndpoint::isCom()
//-----------------------------------------------------------------------------
bool SWConnectionEndpoint::isCom() const
{
    return type_ == ConnectionEndpoint::ENDPOINT_TYPE_COM;
}

//-----------------------------------------------------------------------------
// Function: SWConnectionEndpoint::isApi()
//-----------------------------------------------------------------------------
bool SWConnectionEndpoint::isApi() const
{
    return type_ == ConnectionEndpoint::ENDPOINT_TYPE_API;
}

//-----------------------------------------------------------------------------
// Function: SWConnectionEndpoint::getType()
//-----------------------------------------------------------------------------
ConnectionEndpoint::EndpointType SWConnectionEndpoint::getType() const
{
    return type_;
}

//-----------------------------------------------------------------------------
// Function: SWConnectionEndpoint::setType()
//-----------------------------------------------------------------------------
void SWConnectionEndpoint::setType(ConnectionEndpoint::EndpointType newType)
{
    type_ = newType;
}

//-----------------------------------------------------------------------------
// Function: SWConnectionEndpoint::initialize()
//-----------------------------------------------------------------------------
void SWConnectionEndpoint::initialize()
{
    QFont font = nameLabel_->font();
    font.setPointSize(8);
    nameLabel_->setFont(font);
    nameLabel_->setFlag(ItemStacksBehindParent);
    nameLabel_->setRotation(-rotation());

    setFlag(ItemIsMovable);
    setFlag(ItemIsSelectable);
    setFlag(ItemSendsGeometryChanges);
    setFlag(ItemSendsScenePositionChanges);

    // Create the off-page connector.
    offPageConnector_ = new OffPageConnectorItem(this);
    offPageConnector_->setPos(0.0, -GridSize * 3);
    offPageConnector_->setFlag(ItemStacksBehindParent);
    offPageConnector_->setVisible(false);
}
