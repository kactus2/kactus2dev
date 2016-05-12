//-----------------------------------------------------------------------------
// File: AdHocEnabled.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 27.2.2012
//
// Description:
// Base class for adding ad-hoc support to a derived class.
//-----------------------------------------------------------------------------

#include "AdHocEnabled.h"

#include <designEditors/HWDesign/AdHocItem.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Port.h>

//-----------------------------------------------------------------------------
// Function: AdHocEnabled::AdHocEnabled()
//-----------------------------------------------------------------------------
AdHocEnabled::AdHocEnabled() :
component_(),
portAdHocVisibilities_()
{
}

//-----------------------------------------------------------------------------
// Function: AdHocEnabled::setAdHocData()
//-----------------------------------------------------------------------------
void AdHocEnabled::setAdHocData(QSharedPointer<Component> component, QMap<QString, bool> const& portAdHocVisibilities)
{
    component_ = component;
    portAdHocVisibilities_.clear();

    // Parse port ad-hoc visibilities.
    if (component->hasPorts())
    {
        foreach (QSharedPointer<Port> adhocPort, *component->getPorts())
        {
            bool visible = adhocPort->isAdHocVisible();

            if (portAdHocVisibilities.contains(adhocPort->name()))
            {
                visible = portAdHocVisibilities.value(adhocPort->name());
            }

            portAdHocVisibilities_.insert(adhocPort->name(), visible);
        }
    }

    QMapIterator<QString, bool> visibilityIterator(portAdHocVisibilities);
    while (visibilityIterator.hasNext())
    {
        visibilityIterator.next();

        if (!portAdHocVisibilities_.contains(visibilityIterator.key()))
        {
            portAdHocVisibilities_.insert(visibilityIterator.key(), visibilityIterator.value());
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AdHocEnabled::setPortAdHocVisible()
//-----------------------------------------------------------------------------
void AdHocEnabled::setPortAdHocVisible(QString const& portName, bool visible)
{
    // Check if the visibility has changed.
    if (portAdHocVisibilities_.value(portName) != visible)
    {
        // Update the value and call onAdHocVisiblityChanged().
        portAdHocVisibilities_.insert(portName, visible);
        onAdHocVisibilityChanged(portName, visible);
    }
}

//-----------------------------------------------------------------------------
// Function: AdHocEnabled::isPortAdHocVisible()
//-----------------------------------------------------------------------------
bool AdHocEnabled::isPortAdHocVisible(QString const& portName) const
{
    return portAdHocVisibilities_.value(portName, false);
}

//-----------------------------------------------------------------------------
// Function: AdHocEnabled::onAdHocVisibilityChanged()
//-----------------------------------------------------------------------------
void AdHocEnabled::onAdHocVisibilityChanged(QString const&, bool)
{
    // Default implementation is empty.
}

//-----------------------------------------------------------------------------
// Function: AdHocEnabled::getPortAdHocVisibilities()
//-----------------------------------------------------------------------------
QMap<QString, bool> AdHocEnabled::getPortAdHocVisibilities() const
{
    return portAdHocVisibilities_;
}

//-----------------------------------------------------------------------------
// Function: AdHocEnabled::getPorts()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<Port> > > AdHocEnabled::getPorts() const
{
    Q_ASSERT(component_ != 0);
    return component_->getPorts();
}

//-----------------------------------------------------------------------------
// Function: AdHocEnabled::getDiagramAdHocPort()
//-----------------------------------------------------------------------------
HWConnectionEndpoint* AdHocEnabled::getDiagramAdHocPort(QString const&)
{
    return 0;
}

//-----------------------------------------------------------------------------
// Function: AdHocEnabled::createAdhocItem()
//-----------------------------------------------------------------------------
AdHocItem* AdHocEnabled::createAdhocItem(QString const&)
{
    return 0;
}

//-----------------------------------------------------------------------------
// Function: AdHocEnabled::changeAdhocVisibility()
//-----------------------------------------------------------------------------
void AdHocEnabled::changeAdhocVisibility(AdHocItem* portItem, bool newVisibility)
{
    // Check if the visibility has changed.
    if (portAdHocVisibilities_.value(portItem->name()) != newVisibility)
    {
        // Update the value and call onAdHocVisiblityChanged().
        portAdHocVisibilities_.insert(portItem->name(), newVisibility);

        if (newVisibility == true)
        {
            showAdhocPort(portItem);
        }
        else
        {
            hideAdhocPort(portItem);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AdHocEnabled::showAdhocPort()
//-----------------------------------------------------------------------------
void AdHocEnabled::showAdhocPort(AdHocItem*)
{

}

//-----------------------------------------------------------------------------
// Function: AdHocEnabled::hideAdhocPort()
//-----------------------------------------------------------------------------
void AdHocEnabled::hideAdhocPort(AdHocItem*)
{

}
