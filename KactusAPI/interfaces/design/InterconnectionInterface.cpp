//-----------------------------------------------------------------------------
// File: InterconnectionInterface.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 09.09.2021
//
// Description:
// Interface for accessing design interconnections.
//-----------------------------------------------------------------------------

#include <InterconnectionInterface.h>

#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/Design/ActiveInterface.h>
#include <IPXACTmodels/Design/Interconnection.h>
#include <IPXACTmodels/Design/HierInterface.h>

#include <IPXACTmodels/utilities/Search.h>
#include "ListHelper.h"

namespace
{
    QString const DEFAULT_NAME = QLatin1String("interconnection");
};

//-----------------------------------------------------------------------------
// Function: InterconnectionInterface::InterconnectionInterface()
//-----------------------------------------------------------------------------
InterconnectionInterface::InterconnectionInterface():
NameGroupInterface()
{

}

//-----------------------------------------------------------------------------
// Function: InterconnectionInterface::setInterconnections()
//-----------------------------------------------------------------------------
void InterconnectionInterface::setInterconnections(QSharedPointer<Design> newDesign)
{
    interconnections_ = newDesign->getInterconnections();
}

//-----------------------------------------------------------------------------
// Function: InterconnectionInterface::getInterconnection()
//-----------------------------------------------------------------------------
QSharedPointer<Interconnection> InterconnectionInterface::getInterconnection(std::string const& connectionName)
const
{
    return Search::findByName(QString::fromStdString(connectionName), interconnections_);
}

//-----------------------------------------------------------------------------
// Function: InterconnectionInterface::getItem()
//-----------------------------------------------------------------------------
QSharedPointer<NameGroup> InterconnectionInterface::getItem(std::string const& connectionName) const
{
    return getInterconnection(connectionName);
}

//-----------------------------------------------------------------------------
// Function: InterconnectionInterface::getItemIndex()
//-----------------------------------------------------------------------------
int InterconnectionInterface::getItemIndex(std::string const& itemName) const
{
    QString itemNameQ = QString::fromStdString(itemName);
    for (int i = 0; i < interconnections_->size(); ++i)
    {
        if (interconnections_->at(i)->name() == itemNameQ)
        {
            return i;
        }
    }

    return -1;
}

//-----------------------------------------------------------------------------
// Function: InterconnectionInterface::getIndexedItemName()
//-----------------------------------------------------------------------------
std::string InterconnectionInterface::getIndexedItemName(int itemIndex) const
{
    std::string connectionName = "";
    if (itemIndex >= 0 && itemIndex < itemCount())
    {
        connectionName = interconnections_->at(itemIndex)->name().toStdString();
    }

    return connectionName;
}

//-----------------------------------------------------------------------------
// Function: InterconnectionInterface::getConnectionName()
//-----------------------------------------------------------------------------
std::string InterconnectionInterface::getConnectionName(std::string const& startInstanceName,
    std::string const& startBusName, std::string const& endInstanceName, std::string const& endBusName) const
{
    QString firstInstance = QString::fromStdString(startInstanceName);
    QString firstBus = QString::fromStdString(startBusName);
    QString secondInstance = QString::fromStdString(endInstanceName);
    QString secondBus = QString::fromStdString(endBusName);

    for (auto connection : *interconnections_)
    {
        QString endInstance;
        QString endBus;

        QSharedPointer<ActiveInterface> startInterface = connection->getStartInterface();
        if (startInterface)
        {
            if (startInterface->getComponentReference() == firstInstance &&
                startInterface->getBusReference() == firstBus)
            {
                endInstance = secondInstance;
                endBus = secondBus;
            }
            else if (startInterface->getComponentReference() == secondInstance &&
                startInterface->getBusReference() == secondBus)
            {
                endInstance = firstInstance;
                endBus = firstBus;
            }
        }

        if (!endInstance.isEmpty() && !endBus.isEmpty())
        {
            for (auto connectionInterface : *connection->getActiveInterfaces())
            {
                if (connectionInterface->getComponentReference() == endInstance &&
                    connectionInterface->getBusReference() == endBus)
                {
                    return connection->name().toStdString();
                }
            }
        }
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: InterconnectionInterface::getHierarchicalConnectionName()
//-----------------------------------------------------------------------------
std::string InterconnectionInterface::getHierarchicalConnectionName(std::string const& instanceName,
    std::string const& instanceBus, std::string const& topBus) const
{
    QString instanceQ = QString::fromStdString(instanceName);
    QString instanceBusQ = QString::fromStdString(instanceBus);
    QString topBusQ = QString::fromStdString(topBus);

    for (auto connection : *interconnections_)
    {
        QSharedPointer<ActiveInterface> startInterface = connection->getStartInterface();
        if (startInterface && startInterface->getComponentReference() == instanceQ &&
            startInterface->getBusReference() == instanceBusQ)
        {
            for (auto connectionInterface : *connection->getHierInterfaces())
            {
                if (connectionInterface->getBusReference() == topBusQ)
                {
                    return connection->name().toStdString();
                }
            }
        }
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: InterconnectionInterface::itemCount()
//-----------------------------------------------------------------------------
int InterconnectionInterface::itemCount() const
{
    return interconnections_->count();
}

//-----------------------------------------------------------------------------
// Function: InterconnectionInterface::getItemNames()
//-----------------------------------------------------------------------------
std::vector<std::string> InterconnectionInterface::getItemNames() const
{
    std::vector<std::string> connectionNames;
    for (auto const& connection : *interconnections_)
    {
        connectionNames.push_back(connection->name().toStdString());
    }
    return ListHelper::listNames(interconnections_);
    //return connectionNames;
}

//-----------------------------------------------------------------------------
// Function: InterconnectionInterface::setName()
//-----------------------------------------------------------------------------
bool InterconnectionInterface::setName(std::string const& currentName, std::string const& newName)
{
    QSharedPointer<Interconnection> editedConnection = getInterconnection(currentName);
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
// Function: InterconnectionInterface::getDescription()
//-----------------------------------------------------------------------------
std::string InterconnectionInterface::getDescription(std::string const& itemName) const
{
    if (QSharedPointer<Interconnection> editedConnection = getInterconnection(itemName); 
        editedConnection)
    {
        return editedConnection->description().toStdString();
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: InterconnectionInterface::setDescription()
//-----------------------------------------------------------------------------
bool InterconnectionInterface::setDescription(std::string const& itemName, std::string const& newDescription)
{
    QSharedPointer<Interconnection> editedConnection = getInterconnection(itemName);
    if (!editedConnection)
    {
        return false;
    }

    editedConnection->setDescription(QString::fromStdString(newDescription));
    return true;
}

//-----------------------------------------------------------------------------
// Function: InterconnectionInterface::validateItems()
//-----------------------------------------------------------------------------
bool InterconnectionInterface::validateItems() const
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: InterconnectionInterface::itemHasValidName()
//-----------------------------------------------------------------------------
bool InterconnectionInterface::itemHasValidName(std::string const& /*itemName*/) const
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: InterconnectionInterface::addInterconnection()
//-----------------------------------------------------------------------------
void InterconnectionInterface::addInterconnection(std::string const& startComponentInstance,
    std::string const& startBus, std::string const& endComponentInstance, std::string const& endBus,
    std::string const& connectionName)
{
    QString startComponentQ = QString::fromStdString(startComponentInstance);
    QString startBusQ = QString::fromStdString(startBus);
    QString endComponentQ = QString::fromStdString(endComponentInstance);
    QString endBusQ = QString::fromStdString(endBus);

    QSharedPointer<Interconnection> newConnection(new Interconnection());

    QSharedPointer<ActiveInterface> startInterface(new ActiveInterface());
    startInterface->setComponentReference(QString::fromStdString(startComponentInstance));
    startInterface->setBusReference(QString::fromStdString(startBus));

    QSharedPointer<ActiveInterface> endInterface(new ActiveInterface());
    endInterface->setComponentReference(QString::fromStdString(endComponentInstance));
    endInterface->setBusReference(QString::fromStdString(endBus));

    newConnection->setStartInterface(startInterface);
    newConnection->getActiveInterfaces()->append(endInterface);

    QString newConnectionName = QString::fromStdString(connectionName);
    if (newConnectionName.isEmpty())
    {
        newConnectionName = startComponentQ + QLatin1String("_") + startBusQ + QLatin1String("_to_") +
            endComponentQ + QLatin1String("_") + endBusQ;
    }

    newConnectionName = getUniqueName(newConnectionName.toStdString(), DEFAULT_NAME.toStdString());
    newConnection->setName(newConnectionName);

    interconnections_->append(newConnection);
}

//-----------------------------------------------------------------------------
// Function: InterconnectionInterface::addHierarchicalInterconnection()
//-----------------------------------------------------------------------------
void InterconnectionInterface::addHierarchicalInterconnection(std::string const& instanceName,
    std::string const& instanceBus, std::string const& topBus, std::string const& connectionName /* = "" */)
{
    QString instanceNameQ = QString::fromStdString(instanceName);
    QString instanceBusQ = QString::fromStdString(instanceBus);
    QString topBusQ = QString::fromStdString(topBus);

    QSharedPointer<ActiveInterface> instanceInterface(new ActiveInterface(instanceNameQ, instanceBusQ));

    QSharedPointer<HierInterface> topInterface(new HierInterface(topBusQ));

    QString newConnectionName = QString::fromStdString(connectionName);
    if (newConnectionName.isEmpty())
    {
        newConnectionName = instanceNameQ + QLatin1String("_") + instanceBusQ + QLatin1String("_to_") + topBusQ;
    }
    
    QSharedPointer<Interconnection> newConnection(new Interconnection(newConnectionName, instanceInterface));
    newConnection->getHierInterfaces()->append(topInterface);

    interconnections_->append(newConnection);
}

//-----------------------------------------------------------------------------
// Function: InterconnectionInterface::removeInterconnection()
//-----------------------------------------------------------------------------
bool InterconnectionInterface::removeInterconnection(std::string const& connectionName)
{
    QSharedPointer<Interconnection> removedItem = getInterconnection(connectionName);
    if (!removedItem)
    {
        return false;
    }

    return interconnections_->removeOne(removedItem);
}

//-----------------------------------------------------------------------------
// Function: InterconnectionInterface::removeInstanceInterconnections()
//-----------------------------------------------------------------------------
bool InterconnectionInterface::removeInstanceInterconnections(std::string const& instanceName)
{
    QString instanceNameQ = QString::fromStdString(instanceName);

    QVector<QSharedPointer<Interconnection> > removedConnections;

    for (auto const& connection : *interconnections_)
    {
        if (connection->getStartInterface()->getComponentReference() == instanceNameQ)
        {
            removedConnections.append(connection);
        }
        else
        {
            int interfaceCounter = connection->getActiveInterfaces()->size() - 1;
            while (interfaceCounter >= 0)
            {
                QSharedPointer<ActiveInterface> currentInterface =
                    connection->getActiveInterfaces()->at(interfaceCounter);
                if (currentInterface->getComponentReference() == instanceNameQ)
                {
                    connection->getActiveInterfaces()->removeOne(currentInterface);
                }

                interfaceCounter--;
            }

            if (connection->getActiveInterfaces()->size() == 0)
            {
                removedConnections.append(connection);
            }
        }
    }

    if (removedConnections.isEmpty())
    {
        return false;
    }

    for (auto const& connection : removedConnections)
    {
        interconnections_->removeOne(connection);
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: InterconnectionInterface::interconnectionExists()
//-----------------------------------------------------------------------------
bool InterconnectionInterface::interconnectionExists(std::string const& connectionName) const
{
    return getInterconnection(connectionName) != nullptr;
}

//-----------------------------------------------------------------------------
// Function: InterconnectionInterface::renameComponentReferences()
//-----------------------------------------------------------------------------
void InterconnectionInterface::renameComponentReferences(std::string const& currentName,
    std::string const& newName)
{
    QString oldInstanceName = QString::fromStdString(currentName);
    QString newInstanceName = QString::fromStdString(newName);

    for (auto const& connection : *interconnections_)
    {
        if (connection->getStartInterface()->getComponentReference() == oldInstanceName)
        {
            connection->getStartInterface()->setComponentReference(newInstanceName);
        }

        for (auto const& connectionInterface : *connection->getActiveInterfaces())
        {
            if (connectionInterface->getComponentReference() == oldInstanceName)
            {
                connectionInterface->setComponentReference(newInstanceName);
            }
        }
    }
}
