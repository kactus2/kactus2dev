//-----------------------------------------------------------------------------
// File: AdHocconnectionInterface.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 10.09.2021
//
// Description:
// Interface for accessing design ad hoc connections.
//-----------------------------------------------------------------------------

#include <AdHocConnectionInterface.h>

#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/Design/AdHocConnection.h>
#include <IPXACTmodels/Design/PortReference.h>

namespace
{
    QString const DEFAULT_NAME = QLatin1String("adhocconnection");
};

//-----------------------------------------------------------------------------
// Function: AdHocConnectionInterface::AdHocConnectionInterface()
//-----------------------------------------------------------------------------
AdHocConnectionInterface::AdHocConnectionInterface():
NameGroupInterface(),
connections_()
{

}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionInterface::setConnections()
//-----------------------------------------------------------------------------
void AdHocConnectionInterface::setConnections(QSharedPointer<Design> newDesign)
{
    connections_= newDesign->getAdHocConnections();
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionInterface::getAdHocConnection()
//-----------------------------------------------------------------------------
QSharedPointer<AdHocConnection> AdHocConnectionInterface::getAdHocConnection(std::string const& connectionName)
const
{
    QString connectionNameQ = QString::fromStdString(connectionName);
    for (auto connection : *connections_)
    {
        if (connection->name() == connectionNameQ)
        {
            return connection;
        }
    }

    return QSharedPointer<AdHocConnection>();
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionInterface::getItemIndex()
//-----------------------------------------------------------------------------
int AdHocConnectionInterface::getItemIndex(std::string const& itemName) const
{
    QString itemNameQ = QString::fromStdString(itemName);
    for (int i = 0; i < connections_->size(); ++i)
    {
        if (connections_->at(i)->name() == itemNameQ)
        {
            return i;
        }
    }

    return -1;
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionInterface::getIndexedItemName()
//-----------------------------------------------------------------------------
std::string AdHocConnectionInterface::getIndexedItemName(int const& itemIndex) const
{
    std::string connectionName;
    if (itemIndex >= 0 && itemIndex < itemCount())
    {
        connectionName = connections_->at(itemIndex)->name().toStdString();
    }

    return connectionName;
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionInterface::getConnectionName()
//-----------------------------------------------------------------------------
std::string AdHocConnectionInterface::getConnectionName(std::string const& startInstanceName,
    std::string const& startPortName, std::string const& endInstanceName, std::string const& endPortName) const
{
    for (auto connection : *connections_)
    {
        bool foundFirst = false;
        bool foundSecond = false;

        for (auto portReference : *connection->getInternalPortReferences())
        {
            if (portReference->getComponentRef() == startInstanceName && portReference->getPortRef() == startPortName)
            {
                foundFirst = true;
            }
            else if (portReference->getComponentRef() == endInstanceName &&
                portReference->getPortRef() == endPortName)
            {
                foundSecond = true;
            }

            if (foundFirst && foundSecond)
            {
                break;
            }
        }

        if (foundFirst && foundSecond)
        {
            return connection->name().toStdString();
        }
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionInterface::getHierarchicalConnectionName()
//-----------------------------------------------------------------------------
std::string AdHocConnectionInterface::getHierarchicalConnectionName(std::string const& instanceName,
    std::string const& instancePort, std::string const& topPort) const
{
    for (auto connection : *connections_)
    {
        bool foundInstance = false;
        bool foundTop = false;

        for (auto portReference : *connection->getInternalPortReferences())
        {
            if (portReference->getComponentRef() == instanceName && portReference->getPortRef() == instancePort)
            {
                foundInstance = true;
                break;
            }
        }
        for (auto portReference : *connection->getExternalPortReferences())
        {
            if (portReference->getPortRef() == topPort)
            {
                foundTop = true;
                break;
            }
        }

        if (foundInstance && foundTop)
        {
            return connection->name().toStdString();
        }
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionInterface::itemCount()
//-----------------------------------------------------------------------------
int AdHocConnectionInterface::itemCount() const
{
    return connections_->count();
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionInterface::getItemNames()
//-----------------------------------------------------------------------------
std::vector<std::string> AdHocConnectionInterface::getItemNames() const
{
    QVector<QString> connectionNamesQ;
    for (auto connection : *connections_)
    {
        if (!connectionNamesQ.contains(connection->name()))
        {
            connectionNamesQ.append(connection->name());
        }
    }

    std::vector<std::string> connectionNames;
    for (auto connectionName : connectionNamesQ)
    {
        connectionNames.push_back(connectionName.toStdString());
    }

    return connectionNames;
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionInterface::setName()
//-----------------------------------------------------------------------------
bool AdHocConnectionInterface::setName(std::string const& currentName, std::string const& newName)
{
    QSharedPointer<AdHocConnection> editedConnection = getAdHocConnection(currentName);
    if (editedConnection && nameHasChanged(newName, currentName))
    {
        auto uniqueNewName = getUniqueName(newName, DEFAULT_NAME.toStdString());
        editedConnection->setName(uniqueNewName);

        return true;
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionInterface::getDescription()
//-----------------------------------------------------------------------------
std::string AdHocConnectionInterface::getDescription(std::string const& itemName) const
{
    QSharedPointer<AdHocConnection> editedConnection = getAdHocConnection(itemName);
    if (editedConnection)
    {
        return editedConnection->description().toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionInterface::setDescription()
//-----------------------------------------------------------------------------
bool AdHocConnectionInterface::setDescription(std::string const& itemName, std::string const& newDescription)
{
    QSharedPointer<AdHocConnection> editedConnection = getAdHocConnection(itemName);
    if (!editedConnection)
    {
        return false;
    }

    editedConnection->setDescription(QString::fromStdString(newDescription));
    return true;
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionInterface::validateItems()
//-----------------------------------------------------------------------------
bool AdHocConnectionInterface::validateItems() const
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionInterface::itemHasValidName()
//-----------------------------------------------------------------------------
bool AdHocConnectionInterface::itemHasValidName(std::string const& /*itemName*/) const
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionInterface::addAdHocConnection()
//-----------------------------------------------------------------------------
void AdHocConnectionInterface::addAdHocConnection(std::string const& startComponentInstance,
    std::string const& startPort, std::string const& endComponentInstance, std::string const& endPort,
    std::string const& connectionName)
{
    std::string newConnectionName = connectionName;
    if (connectionName.empty())
    {
        newConnectionName = startComponentInstance + "_" + startPort + "_to_" + endComponentInstance + "_" + endPort;
    }

    QSharedPointer<PortReference> startReference(new PortReference(startPort, startComponentInstance));
    QSharedPointer<PortReference> endReference(new PortReference(endPort, endComponentInstance));

    QSharedPointer<AdHocConnection> newConnection(new AdHocConnection(newConnectionName));
    newConnection->getInternalPortReferences()->append(startReference);
    newConnection->getInternalPortReferences()->append(endReference);

    connections_->append(newConnection);
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionInterface::addHierarchicalAdHocConnection()
//-----------------------------------------------------------------------------
void AdHocConnectionInterface::addHierarchicalAdHocConnection(std::string const& instanceName,
    std::string const& instancePort, std::string const& topPort, std::string const& connectionName /* = "" */)
{
    std::string newConnectionName = connectionName;
    if (newConnectionName.empty())
    {
        newConnectionName = instanceName + "_" + instancePort + "_to_" + topPort;
    }

    QSharedPointer<PortReference> instanceReference(new PortReference(instancePort, instanceName));
    QSharedPointer<PortReference> topReference(new PortReference(topPort));

    QSharedPointer<AdHocConnection> newConnection(new AdHocConnection(newConnectionName));
    newConnection->getInternalPortReferences()->append(instanceReference);
    newConnection->getExternalPortReferences()->append(topReference);

    connections_->append(newConnection);
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionInterface::removeAdHocConnection()
//-----------------------------------------------------------------------------
bool AdHocConnectionInterface::removeAdHocConnection(std::string const& connectionName)
{
    QSharedPointer<AdHocConnection> removedItem = getAdHocConnection(connectionName);
    if (!removedItem)
    {
        return false;
    }

    return connections_->removeOne(removedItem);
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionInterface::removeInstanceAdHocConnections()
//-----------------------------------------------------------------------------
bool AdHocConnectionInterface::removeInstanceAdHocConnections(std::string const& instanceName)
{
    QVector<QSharedPointer<AdHocConnection> > removedConnections;

    for (auto connection : *connections_)
    {
        int connectionSize = connection->getInternalPortReferences()->size();
        for (int connectionIndex = connectionSize - 1; connectionIndex >= 0; connectionIndex--)
        {
            QSharedPointer<PortReference> portReference =
                connection->getInternalPortReferences()->at(connectionIndex);
            if (portReference->getComponentRef() == instanceName)
            {
                connection->getInternalPortReferences()->removeOne(portReference);
            }
        }

        if (connection->getInternalPortReferences()->isEmpty())
        {
            removedConnections.append(connection);
        }
    }

    if (removedConnections.isEmpty())
    {
        return false;
    }

    for (auto connection : removedConnections)
    {
        connections_->removeOne(connection);
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionInterface::adHocConnectionExists()
//-----------------------------------------------------------------------------
bool AdHocConnectionInterface::adHocConnectionExists(std::string const& connectionName) const
{
    for (auto name : getItemNames())
    {
        if (name == connectionName)
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionInterface::renameComponentReferences()
//-----------------------------------------------------------------------------
void AdHocConnectionInterface::renameComponentReferences(std::string const& currentName,
    std::string const& newName)
{
    for (auto connection : *connections_)
    {
        for (auto portReference : *connection->getInternalPortReferences())
        {
            if (portReference->getComponentRef() == currentName)
            {
                portReference->setComponentRef(newName);
            }
        }
    }
}
