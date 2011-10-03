//-----------------------------------------------------------------------------
// File: PlatformComponentItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 26.9.2011
//
// Description:
// Graphics item for representing SW platform components.
//-----------------------------------------------------------------------------

#include "PlatformComponentItem.h"

#include "EndpointDesignDiagram.h"

#include <models/component.h>
#include <models/model.h>

#include <QBrush>

//-----------------------------------------------------------------------------
// Function: PlatformComponentItem()
//-----------------------------------------------------------------------------
PlatformComponentItem::PlatformComponentItem(QSharedPointer<Component> component,
                                             QString const& instanceName,
                                             QString const& displayName,
                                             QString const& description,
                                             QMap<QString, QString> const& configurableElementValues,
                                             QGraphicsItem *parent) :
SWComponentItem(QRectF(-100, 0, 200, 40), component, instanceName, displayName,
                description, configurableElementValues, parent), hierIcon_(0)
{
    Q_ASSERT_X(component, "PlatformComponentItem constructor",
               "Null component-pointer given as parameter");

    updateComponent();
}

//-----------------------------------------------------------------------------
// Function: ~PlatformComponentItem()
//-----------------------------------------------------------------------------
PlatformComponentItem::~PlatformComponentItem()
{
}

//-----------------------------------------------------------------------------
// Function: updateComponent()
//-----------------------------------------------------------------------------
void PlatformComponentItem::updateComponent()
{
    SWComponentItem::updateComponent();

    VLNV* vlnv = componentModel()->getVlnv();

    // Check whether the component is packaged (valid vlnv) or not.
    if (vlnv->isValid())
    {
        setBrush(QBrush(QColor(211, 236, 255)));
    }
    else
    {
        setBrush(QBrush(QColor(217, 217, 217)));
    }

    // Create a hierarchy icon if the component is a hierarchical one.
    if (componentModel()->getModel()->hasHierView() && hierIcon_ == 0)
    {
        hierIcon_ = new QGraphicsPixmapItem(QPixmap(":icons/graphics/hierarchy.png"), this);
        hierIcon_->setPos(75, 6);
    }
    else if (hierIcon_ != 0)
    {
        delete hierIcon_;
    }
}
