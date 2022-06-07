//-----------------------------------------------------------------------------
// File: PortsInterface.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 25.02.2020
//
// Description:
// Interface for editing component ports.
//-----------------------------------------------------------------------------

#include "PortsInterface.h"

#include <IPXACTmodels/common/TransactionalTypes.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Model.h>
#include <IPXACTmodels/Component/Port.h>
#include <IPXACTmodels/Component/validators/PortValidator.h>

using namespace std;

//-----------------------------------------------------------------------------
// Function: PortsInterface::PortsInterface()
//-----------------------------------------------------------------------------
PortsInterface::PortsInterface(QSharedPointer<PortValidator> validator,
    QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<ExpressionFormatter> expressionFormatter):
ParameterizableInterface(expressionParser, expressionFormatter),
MasterPortInterface(),
ports_(),
portValidator_(validator)
{

}

//-----------------------------------------------------------------------------
// Function: PortsInterface::setPorts()
//-----------------------------------------------------------------------------
void PortsInterface::setPorts(QSharedPointer<Component> component)
{
    ports_ = component->getPorts();
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::hasPorts()
//-----------------------------------------------------------------------------
bool PortsInterface::hasPorts() const
{
    if (ports_ && itemCount() > 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getPortIndex()
//-----------------------------------------------------------------------------
int PortsInterface::getItemIndex(string const& itemName) const
{
    for (int i = 0; i < ports_->size(); ++i)
    {
        if (ports_->at(i)->name().toStdString() == itemName)
        {
            return i;
        }
    }

    return -1;
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getIndexedPortName()
//-----------------------------------------------------------------------------
string PortsInterface::getIndexedItemName(int const& itemIndex) const
{
    string portName("");
    if (itemIndex >= 0 && itemIndex < ports_->size())
    {
        portName = ports_->at(itemIndex)->name().toStdString();
    }

    return portName;
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::itemCount()
//-----------------------------------------------------------------------------
int PortsInterface::itemCount() const
{
    return ports_->count();
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getItemNames()
//-----------------------------------------------------------------------------
vector<string> PortsInterface::getItemNames() const
{
    vector<string> portNames;
    for (auto port : *ports_)
    {
        portNames.push_back(port->name().toStdString());
    }

    return portNames;
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getPort()
//-----------------------------------------------------------------------------
QSharedPointer<Port> PortsInterface::getPort(string const& portName) const
{
    if (ports_)
    {
        for (auto port : *ports_)
        {
            if (port->name().toStdString() == portName)
            {
                return port;
            }
        }
    }

    return QSharedPointer<Port>();
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::setName()
//-----------------------------------------------------------------------------
bool PortsInterface::setName(string const& currentPortName, string const& newPortName)
{
    QSharedPointer<Port> editedPort = getPort(currentPortName);
    if (!editedPort)
    {
        return false;
    }

    QString uniqueNewName(getUniqueName(newPortName, "port"));

    editedPort->setName(uniqueNewName);
    return true;
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getDescription()
//-----------------------------------------------------------------------------
string PortsInterface::getDescription(string const& portName) const
{
    QSharedPointer<Port> editedPort = getPort(portName);
    if (editedPort)
    {
        return editedPort->description().toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::setDescription()
//-----------------------------------------------------------------------------
bool PortsInterface::setDescription(string const& portName, string const& newDescription)
{
    QSharedPointer<Port> editedPort = getPort(portName);
    if (!editedPort)
    {
        return false;
    }

    editedPort->setDescription(QString::fromStdString(newDescription));
    return true;
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getTypeName()
//-----------------------------------------------------------------------------
string PortsInterface::getTypeName(string const& portName)
{
    QSharedPointer<Port> port = getPort(portName);

    QSharedPointer<QList<QSharedPointer<WireTypeDef> > > definitionList;
    if (port->getWire())
    {
        definitionList = port->getWire()->getWireTypeDefs();
    }
    else if (port->getTransactional())
    {
        definitionList = port->getTransactional()->getTransTypeDef();
    }

    if (definitionList)
    {
        if (definitionList->count() > 1)
        {
            string combinedType("");
            foreach(QSharedPointer<WireTypeDef> typeDefinition, *definitionList)
            {
                combinedType.append(typeDefinition->getTypeName().toStdString());
                if (typeDefinition != definitionList->last())
                {
                    combinedType.append(", ");
                }
            }

            return combinedType;
        }
        else if (!definitionList->isEmpty())
        {
            return definitionList->first()->getTypeName().toStdString();
        }
    }
    return string("");
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::setTypeName()
//-----------------------------------------------------------------------------
bool PortsInterface::setTypeName(string const& portName, string const& newType)
{
    QSharedPointer<Port> port = getPort(portName);
    if (newType.empty())
    {
        port->getTypes()->clear();
    }
    else
    {
        port->setTypeName(QString::fromStdString(newType));

        // update the type definition for the new type name.
        port->setTypeDefinition(QString::fromStdString(newType), QString());
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getArrayLeftValue()
//-----------------------------------------------------------------------------
string PortsInterface::getArrayLeftValue(string const& portName, int const& baseNumber) const
{
    QSharedPointer<Port> editedPort = getPort(portName);
    if (editedPort)
    {
        return parseExpressionToBaseNumber(editedPort->getArrayLeft(), baseNumber).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getArrayLeftFormattedExpression()
//-----------------------------------------------------------------------------
string PortsInterface::getArrayLeftFormattedExpression(string const& portName) const
{
    QSharedPointer<Port> editedPort = getPort(portName);
    if (editedPort)
    {
        return formattedValueFor(editedPort->getArrayLeft()).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getArrayLeftExpression()
//-----------------------------------------------------------------------------
string PortsInterface::getArrayLeftExpression(string const& portName) const
{
    QSharedPointer<Port> editedPort = getPort(portName);
    if (editedPort)
    {
        return editedPort->getArrayLeft().toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::setArrayLeft()
//-----------------------------------------------------------------------------
bool PortsInterface::setArrayLeft(string const& portName, string const& newArrayLeft)
{
    QSharedPointer<Port> editedPort = getPort(portName);
    if (!editedPort)
    {
        return false;
    }

    editedPort->setArrayLeft(QString::fromStdString(newArrayLeft));
    return true;
}


//-----------------------------------------------------------------------------
// Function: PortsInterface::getArrayRightValue()
//-----------------------------------------------------------------------------
string PortsInterface::getArrayRightValue(string const& portName, int const& baseNumber) const
{
    QSharedPointer<Port> editedPort = getPort(portName);
    if (editedPort)
    {
        return parseExpressionToBaseNumber(editedPort->getArrayRight(), baseNumber).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getArrayRight()
//-----------------------------------------------------------------------------
string PortsInterface::getArrayRightFormattedExpression(string const& portName) const
{
    QSharedPointer<Port> editedPort = getPort(portName);
    if (editedPort)
    {
        return formattedValueFor(editedPort->getArrayRight()).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getArrayRightExpression()
//-----------------------------------------------------------------------------
string PortsInterface::getArrayRightExpression(string const& portName) const
{
    QSharedPointer<Port> editedPort = getPort(portName);
    if (editedPort)
    {
        return editedPort->getArrayRight().toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::setArrayRight()
//-----------------------------------------------------------------------------
bool PortsInterface::setArrayRight(string const& portName, string const& newArrayRight)
{
    QSharedPointer<Port> editedPort = getPort(portName);
    if (!editedPort)
    {
        return false;
    }

    editedPort->setArrayRight(QString::fromStdString(newArrayRight));
    return true;
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getTags()
//-----------------------------------------------------------------------------
string PortsInterface::getTags(string const& portName) const
{
    QSharedPointer<Port> editedPort = getPort(portName);
    if (editedPort)
    {
        return editedPort->getPortTags().toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::setTags()
//-----------------------------------------------------------------------------
bool PortsInterface::setTags(string const& portName, string const& tagList)
{
    QSharedPointer<Port> editedPort = getPort(portName);
    if (!editedPort)
    {
        return false;
    }

    editedPort->setPortTags(QString::fromStdString(tagList));
    return true;
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::isAdHoc()
//-----------------------------------------------------------------------------
bool PortsInterface::isAdHoc(string const& portName) const
{
    QSharedPointer<Port> editedPort = getPort(portName);
    if (editedPort)
    {
        return editedPort->isAdHocVisible();
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::setAdHoc()
//-----------------------------------------------------------------------------
bool PortsInterface::setAdHoc(string const& portName, bool newAdHocVisibility)
{
    QSharedPointer<Port> editedPort = getPort(portName);
    if (!editedPort)
    {
        return false;
    }

    editedPort->setAdHocVisible(newAdHocVisibility);
    return true;
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::portIsWire()
//-----------------------------------------------------------------------------
bool PortsInterface::portIsWire(string const& portName) const
{
    QSharedPointer<Port> port = getPort(portName);
    if (port->getWire())
    {
        return true;
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getDirection()
//-----------------------------------------------------------------------------
string PortsInterface::getDirection(string const& portName) const
{
    QSharedPointer<Port> selectedPort = getPort(portName);
    if (selectedPort && portIsWire(portName))
    {
        return DirectionTypes::direction2Str(selectedPort->getDirection()).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getDirectionType()
//-----------------------------------------------------------------------------
DirectionTypes::Direction PortsInterface::getDirectionType(std::string const& portName) const
{
    QSharedPointer<Port> selectedPort = getPort(portName);
    if (selectedPort && portIsWire(portName))
    {
        return selectedPort->getDirection();
    }

    return DirectionTypes::DIRECTION_INVALID;
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::setDirection()
//-----------------------------------------------------------------------------
bool PortsInterface::setDirection(string const& portName, string const& newDirection)
{
    QSharedPointer<Port> editedPort = getPort(portName);
    if (editedPort && portIsWire(portName))
    {
        editedPort->setDirection(DirectionTypes::str2Direction(
            QString::fromStdString(newDirection), DirectionTypes::DIRECTION_INVALID));
        return true;
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getLeftBoundValue()
//-----------------------------------------------------------------------------
string PortsInterface::getLeftBoundValue(string const& portName, int const& baseNumber) const
{
    QSharedPointer<Port> editedPort = getPort(portName);
    if (editedPort && portIsWire(portName))
    {
        QSharedPointer<Vector> wireVector = editedPort->getWire()->getVector();
        if (wireVector)
        {
            return parseExpressionToBaseNumber(wireVector->getLeft(), baseNumber).toStdString();
        }
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getLeftBoundFormattedExpression()
//-----------------------------------------------------------------------------
string PortsInterface::getLeftBoundFormattedExpression(string const& portName) const
{
    QSharedPointer<Port> editedPort = getPort(portName);
    if (editedPort && portIsWire(portName))
    {
        QSharedPointer<Vector> wireVector = editedPort->getWire()->getVector();
        if (wireVector)
        {
            return formattedValueFor(wireVector->getLeft()).toStdString();
        }
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getLeftBoundExpression()
//-----------------------------------------------------------------------------
string PortsInterface::getLeftBoundExpression(string const& portName) const
{
    QSharedPointer<Port> editedPort = getPort(portName);
    if (editedPort && portIsWire(portName))
    {
        QSharedPointer<Vector> wireVector = editedPort->getWire()->getVector();
        if (wireVector)
        {
            return wireVector->getLeft().toStdString();
        }
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::setLeftBound()
//-----------------------------------------------------------------------------
bool PortsInterface::setLeftBound(string const& portName, string const& newLeftBound)
{
    QSharedPointer<Port> editedPort = getPort(portName);
    if (editedPort && portIsWire(portName))
    {
        editedPort->setLeftBound(QString::fromStdString(newLeftBound));
        setTypeNameAndDefinition(editedPort);
        
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getRightBoundValue()
//-----------------------------------------------------------------------------
string PortsInterface::getRightBoundValue(string const& portName, int const& baseNumber) const
{
    QSharedPointer<Port> editedPort = getPort(portName);
    if (editedPort && portIsWire(portName))
    {
        QSharedPointer<Vector> wireVector = editedPort->getWire()->getVector();
        if (wireVector)
        {
            return parseExpressionToBaseNumber(wireVector->getRight(), baseNumber).toStdString();
        }
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getRightBoundFormattedExpression()
//-----------------------------------------------------------------------------
string PortsInterface::getRightBoundFormattedExpression(string const& portName) const
{
    QSharedPointer<Port> editedPort = getPort(portName);
    if (editedPort && portIsWire(portName))
    {
        QSharedPointer<Vector> wireVector = editedPort->getWire()->getVector();
        if (wireVector)
        {
            return formattedValueFor(wireVector->getRight()).toStdString();
        }
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getRightBoundExpression()
//-----------------------------------------------------------------------------
string PortsInterface::getRightBoundExpression(string const& portName) const
{
    QSharedPointer<Port> editedPort = getPort(portName);
    if (editedPort && portIsWire(portName))
    {
        QSharedPointer<Vector> wireVector = editedPort->getWire()->getVector();
        if (wireVector)
        {
            return wireVector->getRight().toStdString();
        }
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::setRightBound()
//-----------------------------------------------------------------------------
bool PortsInterface::setRightBound(string const& portName, string const& newRightBound)
{
    QSharedPointer<Port> editedPort = getPort(portName);
    if (editedPort && portIsWire(portName))
    {
        editedPort->setRightBound(QString::fromStdString(newRightBound));
        setTypeNameAndDefinition(editedPort);

        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getWidth()
//-----------------------------------------------------------------------------
string PortsInterface::getWidth(string const& portName) const
{
    QSharedPointer<Port> port = getPort(portName);

    qint64 calculatedLeftBound = parseExpressionToDecimal(port->getLeftBound()).toULongLong();
    qint64 calculatedRightBound = parseExpressionToDecimal(port->getRightBound()).toULongLong();

    qint64 portWidth = abs(calculatedLeftBound - calculatedRightBound) + 1;
    return QString::number(portWidth).toStdString();
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::setWidth()
//-----------------------------------------------------------------------------
bool PortsInterface::setWidth(string const& portName, string const& newWidth)
{
    QSharedPointer<Port> port = getPort(portName);
    if (hasExpressionInLeftOrRightBound(portName))
    {
        return false;
    }

    int size = parseExpressionToDecimal(QString::fromStdString(newWidth)).toInt();
    port->setPortSize(size);

    setTypeNameAndDefinition(port);

    return true;
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::hasExpressionInLeftOrRightBound()
//-----------------------------------------------------------------------------
bool PortsInterface::hasExpressionInLeftOrRightBound(string const& portName) const
{
    QSharedPointer<Port> port = getPort(portName);

    QString left(QString::fromStdString(getLeftBoundValue(portName)));
    QString right(QString::fromStdString(getRightBoundValue(portName)));

    if (left.isEmpty() && right.isEmpty())
    {
        return false;
    }

    bool leftNumber = false;
    bool rightNumber = false;

    left.toLongLong(&leftNumber);
    right.toLongLong(&rightNumber);

    return leftNumber || rightNumber;
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::setTypeNameAndDefinition()
//-----------------------------------------------------------------------------
void PortsInterface::setTypeNameAndDefinition(QSharedPointer<Port> port)
{
    qint64 calculatedLeftBound = parseExpressionToDecimal(port->getLeftBound()).toLongLong();
    qint64 calculatedRightBound = parseExpressionToDecimal(port->getRightBound()).toLongLong();

    int portWidth = abs(calculatedLeftBound - calculatedRightBound) + 1;
    // if port is vectored and previous type was std_logic
    if (portWidth > 1 && port->getTypeName() == QString("std_logic"))
    {
        // change the type to vectored
        port->setTypeName("std_logic_vector");
        port->setTypeDefinition("std_logic_vector", "IEEE.std_logic_1164.all");
    }
    // if port is not vectored but previous type was std_logic_vector
    else if (portWidth < 2 && port->getTypeName() == QString("std_logic_vector"))
    {
        port->setTypeName(QStringLiteral("std_logic"));
        port->setTypeDefinition(QStringLiteral("std_logic"), "IEEE.std_logic_1164.all");
    }
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getDefaultValue()
//-----------------------------------------------------------------------------
string PortsInterface::getDefaultValue(string const& portName, int const& baseNumber) const
{
    QSharedPointer<Port> editedPort = getPort(portName);
    if (editedPort && portIsWire(portName))
    {
        return parseExpressionToBaseNumber(
            editedPort->getWire()->getDefaultDriverValue(), baseNumber).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getDefaultValueFormattedExpression()
//-----------------------------------------------------------------------------
string PortsInterface::getDefaultValueFormattedExpression(string const& portName) const
{
    QSharedPointer<Port> editedPort = getPort(portName);
    if (editedPort && portIsWire(portName))
    {
        return formattedValueFor(editedPort->getWire()->getDefaultDriverValue()).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getDefaultValueExpression()
//-----------------------------------------------------------------------------
string PortsInterface::getDefaultValueExpression(string const& portName) const
{
    QSharedPointer<Port> editedPort = getPort(portName);
    if (editedPort && portIsWire(portName))
    {
        return editedPort->getWire()->getDefaultDriverValue().toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::setDefaultValue()
//-----------------------------------------------------------------------------
bool PortsInterface::setDefaultValue(string const& portName, string const& newDefaultValue)
{
    QSharedPointer<Port> editedPort = getPort(portName);
    if (editedPort && portIsWire(portName))
    {
        editedPort->getWire()->setDefaultDriverValue(QString::fromStdString(newDefaultValue));
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::portIsTransactional()
//-----------------------------------------------------------------------------
bool PortsInterface::portIsTransactional(string const& portName) const
{
    QSharedPointer<Port> port = getPort(portName);
    if (port->getTransactional())
    {
        return true;
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getBusWidthValue()
//-----------------------------------------------------------------------------
string PortsInterface::getBusWidthValue(string const& portName, int const& baseNumber) const
{
    QSharedPointer<Port> editedPort = getPort(portName);
    if (editedPort && portIsTransactional(portName))
    {
        return
            parseExpressionToBaseNumber(editedPort->getTransactional()->getBusWidth(), baseNumber).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getBusWidth()
//-----------------------------------------------------------------------------
string PortsInterface::getBusWidthFormattedExpression(string const& portName) const
{
    QSharedPointer<Port> editedPort = getPort(portName);
    if (editedPort && portIsTransactional(portName))
    {
        return formattedValueFor(editedPort->getTransactional()->getBusWidth()).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getBusWidthExpression()
//-----------------------------------------------------------------------------
string PortsInterface::getBusWidthExpression(string const& portName) const
{
    QSharedPointer<Port> editedPort = getPort(portName);
    if (editedPort && portIsTransactional(portName))
    {
        return editedPort->getTransactional()->getBusWidth().toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::setBusWidth()
//-----------------------------------------------------------------------------
bool PortsInterface::setBusWidth(string const& portName, string const& newBusWidth)
{
    QSharedPointer<Port> editedPort = getPort(portName);
    if (editedPort && portIsTransactional(portName))
    {
        editedPort->getTransactional()->setBusWidth(QString::fromStdString(newBusWidth));
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getInitiative()
//-----------------------------------------------------------------------------
string PortsInterface::getInitiative(string const& portName) const
{
    QSharedPointer<Port> editedPort = getPort(portName);
    if (editedPort && portIsTransactional(portName))
    {
        return editedPort->getTransactional()->getInitiative().toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::setInitiative()
//-----------------------------------------------------------------------------
bool PortsInterface::setInitiative(string const& portName, string const& newInitiative)
{
    QSharedPointer<Port> editedPort = getPort(portName);
    if (editedPort && portIsTransactional(portName))
    {
        editedPort->getTransactional()->setInitiative(QString::fromStdString(newInitiative));
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getKind()
//-----------------------------------------------------------------------------
string PortsInterface::getKind(string const& portName) const
{
    QSharedPointer<Port> editedPort = getPort(portName);
    if (editedPort && portIsTransactional(portName))
    {
        return editedPort->getTransactional()->getKind().toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::setKind()
//-----------------------------------------------------------------------------
bool PortsInterface::setKind(string const& portName, string const& newKind)
{
    QSharedPointer<Port> editedPort = getPort(portName);
    if (editedPort && portIsTransactional(portName))
    {
        editedPort->getTransactional()->setKind(QString::fromStdString(newKind));
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getProtocolType()
//-----------------------------------------------------------------------------
string PortsInterface::getProtocolType(string const& portName) const
{
    QSharedPointer<Port> editedPort = getPort(portName);
    if (editedPort && portIsTransactional(portName))
    {
        QSharedPointer<Protocol> portProtocol = editedPort->getTransactional()->getProtocol();
        if (portProtocol)
        {
            return portProtocol->getProtocolType().toStdString();
        }
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::setProtocolType()
//-----------------------------------------------------------------------------
bool PortsInterface::setProtocolType(string const& portName, string const& newProtocolType)
{
    QSharedPointer<Port> editedPort = getPort(portName);
    if (editedPort && portIsTransactional(portName))
    {
        QSharedPointer<Protocol> portProtocol = editedPort->getTransactional()->getProtocol();
        if (!portProtocol)
        {
            portProtocol = QSharedPointer<Protocol>(new Protocol());
            editedPort->getTransactional()->setProtocol(portProtocol);
        }

        portProtocol->setProtocolType(QString::fromStdString(newProtocolType));
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getMaxConnectionsValue()
//-----------------------------------------------------------------------------
string PortsInterface::getMaxConnectionsValue(string const& portName, int const& baseNumber) const
{
    QSharedPointer<Port> editedPort = getPort(portName);
    if (editedPort && portIsTransactional(portName))
    {
        return parseExpressionToBaseNumber(
            editedPort->getTransactional()->getMaxConnections(), baseNumber).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getMaxConnections()
//-----------------------------------------------------------------------------
string PortsInterface::getMaxConnectionsFormattedExpression(string const& portName) const
{
    QSharedPointer<Port> editedPort = getPort(portName);
    if (editedPort && portIsTransactional(portName))
    {
        return formattedValueFor(editedPort->getTransactional()->getMaxConnections()).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getMaxConnectionsExpression()
//-----------------------------------------------------------------------------
string PortsInterface::getMaxConnectionsExpression(string const& portName) const
{
    QSharedPointer<Port> editedPort = getPort(portName);
    if (editedPort && portIsTransactional(portName))
    {
        return editedPort->getTransactional()->getMaxConnections().toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::setMaxConnections()
//-----------------------------------------------------------------------------
bool PortsInterface::setMaxConnections(string const& portName, string const& newMaxConnections) const
{
    QSharedPointer<Port> editedPort = getPort(portName);
    if (editedPort && portIsTransactional(portName))
    {
        editedPort->getTransactional()->setMaxConnections(QString::fromStdString(newMaxConnections));
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getMinConnectionsValue()
//-----------------------------------------------------------------------------
string PortsInterface::getMinConnectionsValue(string const& portName, int const& baseNumber) const
{
    QSharedPointer<Port> editedPort = getPort(portName);
    if (editedPort && portIsTransactional(portName))
    {
        return parseExpressionToBaseNumber(
            editedPort->getTransactional()->getMinConnections(), baseNumber).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getMinConnections()
//-----------------------------------------------------------------------------
string PortsInterface::getMinConnectionsFormattedExpression(string const& portName) const
{
    QSharedPointer<Port> editedPort = getPort(portName);
    if (editedPort && portIsTransactional(portName))
    {
        return formattedValueFor(editedPort->getTransactional()->getMinConnections()).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getMinConnectionsExpression()
//-----------------------------------------------------------------------------
string PortsInterface::getMinConnectionsExpression(string const& portName) const
{
    QSharedPointer<Port> editedPort = getPort(portName);
    if (editedPort && portIsTransactional(portName))
    {
        return editedPort->getTransactional()->getMinConnections().toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::setMinConnections()
//-----------------------------------------------------------------------------
bool PortsInterface::setMinConnections(string const& portName, string const& newMinConnections) const
{
    QSharedPointer<Port> editedPort = getPort(portName);
    if (editedPort && portIsTransactional(portName))
    {
        editedPort->getTransactional()->setMinConnections(QString::fromStdString(newMinConnections));
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getAllReferencesToIdInPort()
//-----------------------------------------------------------------------------
int PortsInterface::getAllReferencesToIdInItem(const string& itemName, string const& valueID) const
{
    QSharedPointer<Port> port = getPort(itemName);
    if (!port)
    {
        return 0;
    }

    QString idString(QString::fromStdString(valueID));

    int referencesInArrayLeft = port->getArrayLeft().count(idString);
    int referencesInArrayRight = port->getArrayRight().count(idString);

    int totalReferences = referencesInArrayLeft + referencesInArrayRight;

    if (portIsWire(itemName))
    {
        QSharedPointer<Wire> portWire = port->getWire();

        int referencesInLeftBound = port->getWire()->getVectorLeftBound().count(idString);
        int referencesInRightBound = port->getWire()->getVectorRightBound().count(idString);
        int referencesInDefaultValue = port->getWire()->getDefaultDriverValue().count(idString);

        totalReferences += referencesInLeftBound + referencesInRightBound + referencesInDefaultValue;
    }
    else if (portIsTransactional(itemName))
    {
        QSharedPointer<Transactional> portTransactional = port->getTransactional();

        int refrencesInWidth = portTransactional->getBusWidth().count(idString);
        int referencesInMaxConnections = portTransactional->getMaxConnections().count(idString);
        int referencesInMinConnections = portTransactional->getMinConnections().count(idString);

        totalReferences += refrencesInWidth + referencesInMaxConnections + referencesInMinConnections;
    }

    return totalReferences;
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::addWirePort()
//-----------------------------------------------------------------------------
void PortsInterface::addWirePort(string const& newPortName)
{
    QString portName(getUniqueName(newPortName, "port"));

    QSharedPointer<Port> newPort(new Port(portName));
    newPort->setWire(QSharedPointer<Wire>(new Wire()));

    ports_->append(newPort);
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::addTransactionalPort()
//-----------------------------------------------------------------------------
void PortsInterface::addTransactionalPort(string const& newPortName)
{
    QString portName(getUniqueName(newPortName, "port"));

    QSharedPointer<Port> newPort(new Port(portName));
    newPort->setTransactional(QSharedPointer<Transactional>(new Transactional()));

    ports_->append(newPort);
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::removePort()
//-----------------------------------------------------------------------------
bool PortsInterface::removePort(string const& portName)
{
    QSharedPointer<Port> removedPort = getPort(portName);
    if (!removedPort)
    {
        return false;
    }

    return ports_->removeOne(removedPort);
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::portExists()
//-----------------------------------------------------------------------------
bool PortsInterface::portExists(std::string const& portName) const
{
    for (auto name : getItemNames())
    {
        if (name == portName)
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::validateItems()
//-----------------------------------------------------------------------------
bool PortsInterface::validateItems() const
{
    for(auto port : *ports_)
    {
        if (!portValidator_->validate(port))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::itemHasValidName()
//-----------------------------------------------------------------------------
bool PortsInterface::itemHasValidName(string const& portName) const
{
    return portValidator_->hasValidName(QString::fromStdString(portName));
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::portLeftArrayValueIsValid()
//-----------------------------------------------------------------------------
bool PortsInterface::portLeftArrayValueIsValid(string const& portName) const
{
    return portValidator_->arrayValueIsValid(getPort(portName)->getArrayLeft());
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::portRightArrayValueIsValid()
//-----------------------------------------------------------------------------
bool PortsInterface::portRightArrayValueIsValid(string const& portName) const
{
    return portValidator_->arrayValueIsValid(getPort(portName)->getArrayRight());
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::portHasValidTypes()
//-----------------------------------------------------------------------------
bool PortsInterface::portHasValidTypes(string const& portName) const
{
    return portValidator_->hasValidTypes(getPort(portName));
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::portHasValidLeftBound()
//-----------------------------------------------------------------------------
bool PortsInterface::portHasValidLeftBound(string const& portName) const
{
    QSharedPointer<Port> port = getPort(portName);
    if (port->getLeftBound().isEmpty() && port->getRightBound().isEmpty())
    {
        return true;
    }

    return portValidator_->portBoundIsValid(port->getLeftBound());
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::portHasValidRightBound()
//-----------------------------------------------------------------------------
bool PortsInterface::portHasValidRightBound(string const& portName) const
{
    QSharedPointer<Port> port = getPort(portName);
    if (port->getLeftBound().isEmpty() && port->getRightBound().isEmpty())
    {
        return true;
    }

    return portValidator_->portBoundIsValid(port->getRightBound());
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::portHasValidDefaultValue()
//-----------------------------------------------------------------------------
bool PortsInterface::portHasValidDefaultValue(string const& portName) const
{
    return portValidator_->hasValidDefaultValue(getPort(portName));
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::portHasValidBusWidth()
//-----------------------------------------------------------------------------
bool PortsInterface::portHasValidBusWidth(string const& portName) const
{
    QSharedPointer<Port> port = getPort(portName);
    if (!port->getTransactional())
    {
        return false;
    }

    return portValidator_->hasValidTransactionalWidth(port->getTransactional());
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::portHasValidInitiative()
//-----------------------------------------------------------------------------
bool PortsInterface::portHasValidInitiative(string const& portName) const
{
    QSharedPointer<Port> port = getPort(portName);
    if (!port->getTransactional())
    {
        return false;
    }

    return portValidator_->hasValidTransactionalInitiative(port->getTransactional());
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::portHasValidKind()
//-----------------------------------------------------------------------------
bool PortsInterface::portHasValidKind(string const& portName) const
{
    QSharedPointer<Port> port = getPort(portName);
    if (!port->getTransactional())
    {
        return false;
    }

    return portValidator_->hasValidTransactionalKind(port->getTransactional());
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::portHasValidProtocol()
//-----------------------------------------------------------------------------
bool PortsInterface::portHasValidProtocol(string const& portName) const
{
    QSharedPointer<Port> port = getPort(portName);
    if (!port->getTransactional())
    {
        return false;
    }

    return portValidator_->hasValidTransactionalProtocol(port->getTransactional());
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::portHasValidMaxConnections()
//-----------------------------------------------------------------------------
bool PortsInterface::portHasValidMaxConnections(string const& portName) const
{
    QSharedPointer<Port> port = getPort(portName);
    if (!port->getTransactional())
    {
        return false;
    }

    return portValidator_->hasValidTransactionalMaxConnections(port->getTransactional());
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::portHasValidMinConnections()
//-----------------------------------------------------------------------------
bool PortsInterface::portHasValidMinConnections(string const& portName) const
{
    QSharedPointer<Port> port = getPort(portName);
    if (!port->getTransactional())
    {
        return false;
    }

    return portValidator_->hasValidTransactionalMinConnections(port->getTransactional());
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getPathForIcon()
//-----------------------------------------------------------------------------
std::string PortsInterface::getIconPathForPort(std::string const& portName) const
{
    std::string path("");
    QSharedPointer<Port> selectedPort = getPort(portName);
    if (!selectedPort)
    {
        path = getIconPathForMissingPort();
    }
    else
    {
        if (selectedPort->getWire())
        {
            DirectionTypes::Direction direction = DirectionTypes::DIRECTION_INVALID;
            direction = selectedPort->getDirection();
            
            path = getIconPathForDirection(direction);
        }
        else if (selectedPort->getTransactional())
        {
            QString initiative = selectedPort->getTransactional()->getInitiative();
            path = getIconPathForInitiative(initiative);
        }
    }

    return path;
}
