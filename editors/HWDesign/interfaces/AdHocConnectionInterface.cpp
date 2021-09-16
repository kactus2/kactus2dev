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

#include "AdHocConnectionInterface.h"

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

    for (auto connection : *connections_)
    {
        bool foundFirst = false;
        bool foundSecond = false;

        for (auto portReference : *connection->getInternalPortReferences())
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
        }

        if (foundFirst && foundSecond)
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
    if (!editedConnection || currentName == newName)
    {
        return false;
    }

    QString uniqueNewName = getUniqueName(newName, DEFAULT_NAME.toStdString());
    editedConnection->setName(uniqueNewName);

    return true;
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

    for (auto connection : *connections_)
    {
        for (auto portReference : *connection->getInternalPortReferences())
        {
            if (portReference->getComponentRef() == instanceNameQ)
            {
                removedConnections.append(connection);
                break;
            }
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
