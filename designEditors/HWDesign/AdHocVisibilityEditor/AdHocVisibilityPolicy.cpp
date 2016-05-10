//-----------------------------------------------------------------------------
// File: AdHocVisibilityPolicy.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 09.05.2016
//
// Description:
// Policy handler for ad hoc port visibilities.
//-----------------------------------------------------------------------------

#include "AdHocVisibilityPolicy.h"

#include <designEditors/HWDesign/AdHocEnabled.h>
#include <designEditors/HWDesign/HWConnectionEndpoint.h>

#include <IPXACTmodels/Design/AdHocConnection.h>
#include <IPXACTmodels/Component/Port.h>

//-----------------------------------------------------------------------------
// Function: AdHocVisibilityPolicy::AdHocVisibilityPolicy()
//-----------------------------------------------------------------------------
AdHocVisibilityPolicy::AdHocVisibilityPolicy():
dataSource_(0),
adhocConnections_(new QList<QSharedPointer<AdHocConnection> > ())
{

}

//-----------------------------------------------------------------------------
// Function: AdHocVisibilityPolicy::~AdHocVisibilityPolicy()
//-----------------------------------------------------------------------------
AdHocVisibilityPolicy::~AdHocVisibilityPolicy()
{

}

//-----------------------------------------------------------------------------
// Function: AdHocVisibilityPolicy::setDataSource()
//-----------------------------------------------------------------------------
void AdHocVisibilityPolicy::setDataSource(AdHocEnabled* dataSource,
    QSharedPointer<QList<QSharedPointer<AdHocConnection> > > connections)
{
    dataSource_ = dataSource;
    adhocConnections_ = connections;
}

//-----------------------------------------------------------------------------
// Function: AdHocVisibilityPolicy::canChangeVisibility()
//-----------------------------------------------------------------------------
bool AdHocVisibilityPolicy::canChangeVisibility(QSharedPointer<Port> port) const
{
    if (portHasConnections(port) || portHasTieOffConnection(port) || port->isAdHocVisible())
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AdHocVisibilityPolicy::portHasConnections()
//-----------------------------------------------------------------------------
bool AdHocVisibilityPolicy::portHasConnections(QSharedPointer<Port> port) const
{
    if (dataSource_)
    {
        HWConnectionEndpoint* endpoint = dataSource_->getDiagramAdHocPort(port->name());
        if (endpoint)
        {
            if (!endpoint->getConnections().isEmpty() ||
                (endpoint->getOffPageConnector() && !endpoint->getOffPageConnector()->getConnections().isEmpty()))
            {
                return true;
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: AdHocVisibilityPolicy::portHasTieOffConnection()
//-----------------------------------------------------------------------------
bool AdHocVisibilityPolicy::portHasTieOffConnection(QSharedPointer<Port> port) const
{
    if (adhocConnections_ && !adhocConnections_->isEmpty())
    {
        QSharedPointer<AdHocConnection> connection = getTieOffConnection(port);

        if (connection)
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: AdHocVisibilityPolicy::getTieOffConnection()
//-----------------------------------------------------------------------------
QSharedPointer<AdHocConnection> AdHocVisibilityPolicy::getTieOffConnection(QSharedPointer<Port> port) const
{
    QString adHocId = dataSource_->adHocIdentifier();

    if (adHocId == QObject::tr("top-level"))
    {
        return getTopComponentTieOffConnection(port);
    }
    else
    {
        return getInstanceTieOffConnection(port);
    }
}

//-----------------------------------------------------------------------------
// Function: AdHocVisibilityPolicy::getTopComponentTieOff()
//-----------------------------------------------------------------------------
QSharedPointer<AdHocConnection> AdHocVisibilityPolicy::getTopComponentTieOffConnection(QSharedPointer<Port> port)
    const
{
    foreach (QSharedPointer<AdHocConnection> connection, *adhocConnections_)
    {
        foreach (QSharedPointer<PortReference> externalReference, *connection->getExternalPortReferences())
        {
            if (externalReference->getPortRef() == port->name() && !connection->getTiedValue().isEmpty())
            {
                return connection;
            }
        }
    }

    return QSharedPointer<AdHocConnection>();
}

//-----------------------------------------------------------------------------
// Function: AdHocVisibilityPolicy::getInstanceTieOff()
//-----------------------------------------------------------------------------
QSharedPointer<AdHocConnection> AdHocVisibilityPolicy::getInstanceTieOffConnection(QSharedPointer<Port> port) const
{
    foreach (QSharedPointer<AdHocConnection> connection, *adhocConnections_)
    {
        foreach (QSharedPointer<PortReference> internalReference, *connection->getInternalPortReferences())
        {
            if (dataSource_->adHocIdentifier() == internalReference->getComponentRef() &&
                internalReference->getPortRef() == port->name() && !connection->getTiedValue().isEmpty())
            {
                return connection;
            }
        }
    }

    return QSharedPointer<AdHocConnection>();
}

//-----------------------------------------------------------------------------
// Function: AdHocVisibilityPolicy::isPortAdhocVisible()
//-----------------------------------------------------------------------------
bool AdHocVisibilityPolicy::isPortAdhocVisible(QString const& portName) const
{
    if (dataSource_)
    {
        return dataSource_->isPortAdHocVisible(portName);
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: AdHocVisibilityPolicy::getDataSource()
//-----------------------------------------------------------------------------
AdHocEnabled* AdHocVisibilityPolicy::getDataSource()
{
    return dataSource_;
}
