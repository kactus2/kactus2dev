//-----------------------------------------------------------------------------
// File: SWComponentItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 24.11.2011
//
// Description:
// Base class for all SW components visualized in a system design.
//-----------------------------------------------------------------------------

#include "SWComponentItem.h"

#include <models/component.h>

//-----------------------------------------------------------------------------
// Function: SWComponentItem()
//-----------------------------------------------------------------------------
SWComponentItem::SWComponentItem(QRectF const& size, QSharedPointer<Component> component,
                                 QString const& instanceName, QString const& displayName,
                                 QString const& description,
                                 QMap<QString, QString> const& configurableElementValues,
                                 QGraphicsItem *parent)
    : ComponentItem(size, component, instanceName, displayName, description, configurableElementValues, parent),
      imported_(false)
{
}

//-----------------------------------------------------------------------------
// Function: ~SWComponentItem()
//-----------------------------------------------------------------------------
SWComponentItem::~SWComponentItem()
{

}

//-----------------------------------------------------------------------------
// Function: updateComponent()
//-----------------------------------------------------------------------------
void SWComponentItem::updateComponent()
{
    ComponentItem::updateComponent();
}


//-----------------------------------------------------------------------------
// Function: setImported()
//-----------------------------------------------------------------------------
void SWComponentItem::setImported(bool imported)
{
    imported_ = imported;
    updateComponent();
}

//-----------------------------------------------------------------------------
// Function: isImported()
//-----------------------------------------------------------------------------
bool SWComponentItem::isImported() const
{
    return imported_;
}
