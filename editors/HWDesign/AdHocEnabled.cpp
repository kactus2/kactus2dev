//-----------------------------------------------------------------------------
// File: AdHocEnabled.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti Maatta
// Date: 27.2.2012
//
// Description:
// Base class for adding ad-hoc support to a derived class.
//-----------------------------------------------------------------------------

#include "AdHocEnabled.h"

#include <editors/HWDesign/AdHocItem.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Port.h>

//-----------------------------------------------------------------------------
// Function: AdHocEnabled::AdHocEnabled()
//-----------------------------------------------------------------------------
AdHocEnabled::AdHocEnabled()
{
}

//-----------------------------------------------------------------------------
// Function: AdHocEnabled::setAdHocData()
//-----------------------------------------------------------------------------
void AdHocEnabled::setAdHocData(QSharedPointer<Component> component, QMap<std::string, bool> const& portAdHocVisibilities)
{
    component_ = component;
    portAdHocVisibilities_.clear();

    // Parse port ad-hoc visibilities.
    if (component->hasPorts())
    {
        for (auto const& adhocPort : *component->getPorts())
        {
            bool visible = adhocPort->isAdHocVisible();

            if (portAdHocVisibilities.contains(adhocPort->name().toStdString()))
            {
                visible = portAdHocVisibilities.value(adhocPort->name().toStdString());
            }

            portAdHocVisibilities_.insert(adhocPort->name().toStdString(), visible);
        }
    }

    QMapIterator<std::string, bool> visibilityIterator(portAdHocVisibilities);
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
void AdHocEnabled::setPortAdHocVisible(std::string const& portName, bool visible)
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
bool AdHocEnabled::isPortAdHocVisible(std::string const& portName) const
{
    return portAdHocVisibilities_.value(portName, false);
}

//-----------------------------------------------------------------------------
// Function: AdHocEnabled::onAdHocVisibilityChanged()
//-----------------------------------------------------------------------------
void AdHocEnabled::onAdHocVisibilityChanged(std::string const&, bool)
{
    // Default implementation is empty.
}

//-----------------------------------------------------------------------------
// Function: AdHocEnabled::getPortAdHocVisibilities()
//-----------------------------------------------------------------------------
QMap<std::string, bool> AdHocEnabled::getPortAdHocVisibilities() const
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
HWConnectionEndpoint* AdHocEnabled::getDiagramAdHocPort(std::string const&)
{
    return nullptr;
}

//-----------------------------------------------------------------------------
// Function: AdHocEnabled::createAdhocItem()
//-----------------------------------------------------------------------------
AdHocItem* AdHocEnabled::createAdhocItem(std::string const&)
{
    return nullptr;
}

//-----------------------------------------------------------------------------
// Function: AdHocEnabled::changeAdhocVisibility()
//-----------------------------------------------------------------------------
void AdHocEnabled::changeAdhocVisibility(AdHocItem* portItem, bool newVisibility)
{
    // Check if the visibility has changed.
    if (portAdHocVisibilities_.value(portItem->name().toStdString()) != newVisibility || portItem->getPort()->isAdHocVisible())
    {
        if (!portAdHocVisibilities_.contains(portItem->name().toStdString()) || !portItem->getPort()->isAdHocVisible())
        {
            // Update the value and call onAdHocVisiblityChanged().
            portAdHocVisibilities_.insert(portItem->name().toStdString(), newVisibility);
        }

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
