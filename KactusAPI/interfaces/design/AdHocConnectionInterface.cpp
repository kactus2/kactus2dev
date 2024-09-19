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

#include <IPXACTmodels/utilities/Search.h>
#include "ListHelper.h"

namespace
{
    QString const DEFAULT_NAME = QLatin1String("adhocconnection");
};

//-----------------------------------------------------------------------------
// Function: AdHocConnectionInterface::AdHocConnectionInterface()
//-----------------------------------------------------------------------------
AdHocConnectionInterface::AdHocConnectionInterface():
NameGroupInterface()
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
    return Search::findByName(QString::fromStdString(connectionName), connections_);
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionInterface::getItem()
//-----------------------------------------------------------------------------
QSharedPointer<NameGroup> AdHocConnectionInterface::getItem(std::string const& connectionName) const
{
    return getAdHocConnection(connectionName);
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionInterface::getItemIndex()
//-----------------------------------------------------------------------------
int AdHocConnectionInterface::getItemIndex(std::string const& itemName) const
{
    QString itemNameQ = QString::fromStdString(itemName);

    const auto COUNT = connections_->size();
    for (int i = 0; i < COUNT; ++i)
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
std::string AdHocConnectionInterface::getIndexedItemName(int itemIndex) const
{
    std::string connectionName = "";
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
    QString firstInstance = QString::fromStdString(startInstanceName);
    QString firstPort = QString::fromStdString(startPortName);
    QString secondInstance = QString::fromStdString(endInstanceName);
    QString secondPort = QString::fromStdString(endPortName);

    for (auto const& connection : *connections_)
    {
        bool foundFirst = false;
        bool foundSecond = false;

        for (auto const& portReference : *connection->getInternalPortReferences())
        {
            if (portReference->getComponentRef() == firstInstance && portReference->getPortRef() == firstPort)
            {
                foundFirst = true;
            }
            else if (portReference->getComponentRef() == secondInstance &&
                portReference->getPortRef() == secondPort)
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
    QString instanceNameQ = QString::fromStdString(instanceName);
    QString instancePortQ = QString::fromStdString(instancePort);
    QString topPortQ = QString::fromStdString(topPort);

    for (auto const& connection : *connections_)
    {
        bool foundInstance = false;
        bool foundTop = false;

        for (auto const& portReference : *connection->getInternalPortReferences())
        {
            if (portReference->getComponentRef() == instanceNameQ && portReference->getPortRef() == instancePortQ)
            {
                foundInstance = true;
                break;
            }
        }
        for (auto const& portReference : *connection->getExternalPortReferences())
        {
            if (portReference->getPortRef() == topPortQ)
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

    return std::string();
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
    std::vector<std::string> connectionNames;
    for (auto const& connection : *connections_)
    {
        connectionNames.push_back(connection->name().toStdString());
    }
    return ListHelper::listNames(connections_);
    //return connectionNames;
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionInterface::setName()
//-----------------------------------------------------------------------------
bool AdHocConnectionInterface::setName(std::string const& currentName, std::string const& newName)
{
    QSharedPointer<AdHocConnection> editedConnection = getAdHocConnection(currentName);
    if (editedConnection && nameHasChanged(newName, currentName))
    {
        QString uniqueNewName = getUniqueName(newName, DEFAULT_NAME.toStdString());
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
    if (QSharedPointer<AdHocConnection> editedConnection = getAdHocConnection(itemName); 
        editedConnection)
    {
        return editedConnection->description().toStdString();
    }

    return std::string();
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
    QString newConnectionName = QString::fromStdString(connectionName);

    QString startInstanceQ = QString::fromStdString(startComponentInstance);
    QString startPortQ = QString::fromStdString(startPort);
    QString endInstanceQ = QString::fromStdString(endComponentInstance);
    QString endPortQ = QString::fromStdString(endPort);

    if (newConnectionName.isEmpty())
    {
        newConnectionName = startInstanceQ + QStringLiteral("_") + startPortQ + QStringLiteral("_to_") +
            endInstanceQ + QStringLiteral("_") + endPortQ;
    }

    QSharedPointer<PortReference> startReference(new PortReference(startPortQ, startInstanceQ));
    QSharedPointer<PortReference> endReference(new PortReference(endPortQ, endInstanceQ));

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
    QString instanceQ = QString::fromStdString(instanceName);
    QString instancePortQ = QString::fromStdString(instancePort);
    QString topPortQ = QString::fromStdString(topPort);

    QString newConnectionName = QString::fromStdString(connectionName);
    if (newConnectionName.isEmpty())
    {
        newConnectionName = instanceQ + QStringLiteral("_") + instancePortQ + QStringLiteral("_to_") + topPortQ;
    }

    QSharedPointer<PortReference> instanceReference(new PortReference(instancePortQ, instanceQ));
    QSharedPointer<PortReference> topReference(new PortReference(topPortQ));

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
    QString instanceNameQ = QString::fromStdString(instanceName);

    QVector<QSharedPointer<AdHocConnection> > removedConnections;

    for (auto const& connection : *connections_)
    {
        const auto connectionSize = connection->getInternalPortReferences()->size();
        for (int connectionIndex = connectionSize - 1; connectionIndex >= 0; --connectionIndex)
        {
            auto portReference = connection->getInternalPortReferences()->at(connectionIndex);
            if (portReference->getComponentRef() == instanceNameQ)
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

    for (auto const& connection : removedConnections)
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
    return getAdHocConnection(connectionName) != nullptr;
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionInterface::renameComponentReferences()
//-----------------------------------------------------------------------------
void AdHocConnectionInterface::renameComponentReferences(std::string const& currentName,
    std::string const& newName)
{
    QString oldInstanceName = QString::fromStdString(currentName);
    QString newInstanceName = QString::fromStdString(newName);

    for (auto const& connection : *connections_)
    {
        for (auto& portReference : *connection->getInternalPortReferences())
        {
            if (portReference->getComponentRef() == oldInstanceName)
            {
                portReference->setComponentRef(newInstanceName);
            }
        }
    }
}
