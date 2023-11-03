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

#include <PortsInterface.h>

#include <IPXACTmodels/common/TransactionalTypes.h>
#include <IPXACTmodels/common/Vector.h>

#include <IPXACTmodels/Component/Port.h>
#include <IPXACTmodels/Component/validators/PortValidator.h>

#include <IPXACTmodels/utilities/Search.h>

#include <KactusAPI/include/ListHelper.h>

//-----------------------------------------------------------------------------
// Function: PortsInterface::PortsInterface()
//-----------------------------------------------------------------------------
PortsInterface::PortsInterface(QSharedPointer<PortValidator> validator,
    QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<ExpressionFormatter> expressionFormatter):
ParameterizableInterface(expressionParser, expressionFormatter),
MasterPortInterface(),
portValidator_(validator)
{

}

//-----------------------------------------------------------------------------
// Function: PortsInterface::setPorts()
//-----------------------------------------------------------------------------
void PortsInterface::setPorts(Port::List ports)
{
    ports_ = ports;
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::hasPorts()
//-----------------------------------------------------------------------------
bool PortsInterface::hasPorts() const
{
    return ports_ && itemCount() > 0;
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getPortIndex()
//-----------------------------------------------------------------------------
int PortsInterface::getItemIndex(std::string const& itemName) const
{
    return ListHelper::itemIndex(itemName, ports_);
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getIndexedPortName()
//-----------------------------------------------------------------------------
std::string PortsInterface::getIndexedItemName(int itemIndex) const
{
    if (itemIndex >= 0 && itemIndex < ports_->size())
    {
        return ports_->at(itemIndex)->name().toStdString();
    }

    return std::string();
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
std::vector<std::string> PortsInterface::getItemNames() const
{
    return ListHelper::listNames(ports_);
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getPort()
//-----------------------------------------------------------------------------
QSharedPointer<Port> PortsInterface::getPort(std::string const& portName) const
{
    return Search::findByName(QString::fromStdString(portName), ports_);
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getItem()
//-----------------------------------------------------------------------------
QSharedPointer<NameGroup> PortsInterface::getItem(std::string const& portName) const
{
    return getPort(portName);
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::setName()
//-----------------------------------------------------------------------------
bool PortsInterface::setName(std::string const& currentPortName, std::string const& newPortName)
{
    QSharedPointer<Port> editedPort = getPort(currentPortName);
    if (editedPort && nameHasChanged(newPortName, currentPortName))
    {
        QString uniqueNewName(getUniqueName(newPortName, "port"));

        editedPort->setName(uniqueNewName);
        return true;
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getTypeName()
//-----------------------------------------------------------------------------
std::string PortsInterface::getTypeName(std::string const& portName) const
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

    if (!definitionList)
    {
        return std::string();
    }

    if (definitionList->count() == 1)
    {
        return definitionList->first()->getTypeName().toStdString();
    }
    else
    {
        std::string combinedType;
        for (auto const& typeDefinition : *definitionList)
        {
            combinedType.append(typeDefinition->getTypeName().toStdString());
            if (typeDefinition != definitionList->last())
            {
                combinedType.append(", ");
            }
        }

        return combinedType;
    }
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::setTypeName()
//-----------------------------------------------------------------------------
bool PortsInterface::setTypeName(std::string const& portName, std::string const& newType) const
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
std::string PortsInterface::getArrayLeftValue(std::string const& portName, unsigned int baseNumber) const
{
    if (QSharedPointer<Port> editedPort = getPort(portName); 
        editedPort)
    {
        return parseExpressionToBaseNumber(editedPort->getArrayLeft(), baseNumber).toStdString();
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getArrayLeftFormattedExpression()
//-----------------------------------------------------------------------------
std::string PortsInterface::getArrayLeftFormattedExpression(std::string const& portName) const
{
    if (QSharedPointer<Port> editedPort = getPort(portName); 
        editedPort)
    {
        return formattedValueFor(editedPort->getArrayLeft()).toStdString();
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getArrayLeftExpression()
//-----------------------------------------------------------------------------
std::string PortsInterface::getArrayLeftExpression(std::string const& portName) const
{
    if (QSharedPointer<Port> editedPort = getPort(portName); 
        editedPort)
    {
        return editedPort->getArrayLeft().toStdString();
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::setArrayLeft()
//-----------------------------------------------------------------------------
bool PortsInterface::setArrayLeft(std::string const& portName, std::string const& newArrayLeft) const
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
std::string PortsInterface::getArrayRightValue(std::string const& portName, unsigned int baseNumber) const
{
    if (QSharedPointer<Port> editedPort = getPort(portName);
        editedPort)
    {
        return parseExpressionToBaseNumber(editedPort->getArrayRight(), baseNumber).toStdString();
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getArrayRight()
//-----------------------------------------------------------------------------
std::string PortsInterface::getArrayRightFormattedExpression(std::string const& portName) const
{
    if (QSharedPointer<Port> editedPort = getPort(portName); 
        editedPort)
    {
        return formattedValueFor(editedPort->getArrayRight()).toStdString();
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getArrayRightExpression()
//-----------------------------------------------------------------------------
std::string PortsInterface::getArrayRightExpression(std::string const& portName) const
{
    if (QSharedPointer<Port> editedPort = getPort(portName); 
        editedPort)
    {
        return editedPort->getArrayRight().toStdString();
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::setArrayRight()
//-----------------------------------------------------------------------------
bool PortsInterface::setArrayRight(std::string const& portName, std::string const& newArrayRight) const
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
std::string PortsInterface::getTags(std::string const& portName) const
{
    if (QSharedPointer<Port> editedPort = getPort(portName); 
        editedPort)
    {
        return editedPort->getPortTags().toStdString();
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::setTags()
//-----------------------------------------------------------------------------
bool PortsInterface::setTags(std::string const& portName, std::string const& tagList) const
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
bool PortsInterface::isAdHoc(std::string const& portName) const
{
    if (QSharedPointer<Port> editedPort = getPort(portName); 
        editedPort)
    {
        return editedPort->isAdHocVisible();
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::setAdHoc()
//-----------------------------------------------------------------------------
bool PortsInterface::setAdHoc(std::string const& portName, bool newAdHocVisibility) const
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
bool PortsInterface::portIsWire(std::string const& portName) const
{
    return portIsWire(getPort(portName));
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::portIsWire()
//-----------------------------------------------------------------------------
bool PortsInterface::portIsWire(QSharedPointer<Port> port) const
{
    return port.isNull() == false && port->getWire().isNull() == false;
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getDirection()
//-----------------------------------------------------------------------------
std::string PortsInterface::getDirection(std::string const& portName) const
{
    if (QSharedPointer<Port> selectedPort = getPort(portName); 
        portIsWire(selectedPort))
    {
        return DirectionTypes::direction2Str(selectedPort->getDirection()).toStdString();
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getDirectionType()
//-----------------------------------------------------------------------------
DirectionTypes::Direction PortsInterface::getDirectionType(std::string const& portName) const
{
    if (QSharedPointer<Port> selectedPort = getPort(portName); 
        portIsWire(selectedPort))
    {
        return selectedPort->getDirection();
    }

    return DirectionTypes::DIRECTION_INVALID;
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::setDirection()
//-----------------------------------------------------------------------------
bool PortsInterface::setDirection(std::string const& portName, std::string const& newDirection) const
{
    if (QSharedPointer<Port> editedPort = getPort(portName); 
        portIsWire(editedPort))
    {
        editedPort->setDirection(DirectionTypes::str2Direction(
            QString::fromStdString(newDirection), DirectionTypes::DIRECTION_INVALID));
        return true;
    }
    
    return false;
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getLeftBoundValue()
//-----------------------------------------------------------------------------
std::string PortsInterface::getLeftBoundValue(std::string const& portName, unsigned int baseNumber) const
{
    if (QSharedPointer<Port> editedPort = getPort(portName); 
        portIsWire(editedPort))
    {
        QSharedPointer<Vector> wireVector = editedPort->getWire()->getVector();
        if (wireVector)
        {
            return parseExpressionToBaseNumber(wireVector->getLeft(), baseNumber).toStdString();
        }
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getLeftBoundFormattedExpression()
//-----------------------------------------------------------------------------
std::string PortsInterface::getLeftBoundFormattedExpression(std::string const& portName) const
{
    if (QSharedPointer<Port> editedPort = getPort(portName); 
        portIsWire(editedPort))
    {
        QSharedPointer<Vector> wireVector = editedPort->getWire()->getVector();
        if (wireVector)
        {
            return formattedValueFor(wireVector->getLeft()).toStdString();
        }
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getLeftBoundExpression()
//-----------------------------------------------------------------------------
std::string PortsInterface::getLeftBoundExpression(std::string const& portName) const
{
    if (QSharedPointer<Port> editedPort = getPort(portName); 
        portIsWire(editedPort))
    {
        QSharedPointer<Vector> wireVector = editedPort->getWire()->getVector();
        if (wireVector)
        {
            return wireVector->getLeft().toStdString();
        }
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::setLeftBound()
//-----------------------------------------------------------------------------
bool PortsInterface::setLeftBound(std::string const& portName, std::string const& newLeftBound) const
{
    if (QSharedPointer<Port> editedPort = getPort(portName); 
        portIsWire(editedPort))
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
std::string PortsInterface::getRightBoundValue(std::string const& portName, unsigned int baseNumber) const
{
    if (QSharedPointer<Port> editedPort = getPort(portName); 
        portIsWire(editedPort))
    {
        QSharedPointer<Vector> wireVector = editedPort->getWire()->getVector();
        if (wireVector)
        {
            return parseExpressionToBaseNumber(wireVector->getRight(), baseNumber).toStdString();
        }
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getRightBoundFormattedExpression()
//-----------------------------------------------------------------------------
std::string PortsInterface::getRightBoundFormattedExpression(std::string const& portName) const
{
    if (QSharedPointer<Port> editedPort = getPort(portName); 
        portIsWire(editedPort))
    {
        QSharedPointer<Vector> wireVector = editedPort->getWire()->getVector();
        if (wireVector)
        {
            return formattedValueFor(wireVector->getRight()).toStdString();
        }
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getRightBoundExpression()
//-----------------------------------------------------------------------------
std::string PortsInterface::getRightBoundExpression(std::string const& portName) const
{
    if (QSharedPointer<Port> editedPort = getPort(portName); 
        portIsWire(editedPort))
    {
        QSharedPointer<Vector> wireVector = editedPort->getWire()->getVector();
        if (wireVector)
        {
            return wireVector->getRight().toStdString();
        }
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::setRightBound()
//-----------------------------------------------------------------------------
bool PortsInterface::setRightBound(std::string const& portName, std::string const& newRightBound) const
{
    if (QSharedPointer<Port> editedPort = getPort(portName); 
        portIsWire(editedPort))
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
std::string PortsInterface::getWidth(std::string const& portName) const
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
bool PortsInterface::setWidth(std::string const& portName, std::string const& newWidth) const
{
    if (hasExpressionInLeftOrRightBound(portName))
    {
        return false;
    }

    int size = parseExpressionToDecimal(QString::fromStdString(newWidth)).toInt();

    QSharedPointer<Port> port = getPort(portName);
    port->setPortSize(size);

    setTypeNameAndDefinition(port);

    return true;
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::hasExpressionInLeftOrRightBound()
//-----------------------------------------------------------------------------
bool PortsInterface::hasExpressionInLeftOrRightBound(std::string const& portName) const
{
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
void PortsInterface::setTypeNameAndDefinition(QSharedPointer<Port> port) const
{
    qint64 calculatedLeftBound = parseExpressionToDecimal(port->getLeftBound()).toLongLong();
    qint64 calculatedRightBound = parseExpressionToDecimal(port->getRightBound()).toLongLong();

    auto portWidth = abs(calculatedLeftBound - calculatedRightBound) + 1;
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
std::string PortsInterface::getDefaultValue(std::string const& portName, unsigned int baseNumber) const
{
    if (QSharedPointer<Port> editedPort = getPort(portName); 
        portIsWire(editedPort))
    {
        return parseExpressionToBaseNumber(
            editedPort->getWire()->getDefaultDriverValue(), baseNumber).toStdString();
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getDefaultValueFormattedExpression()
//-----------------------------------------------------------------------------
std::string PortsInterface::getDefaultValueFormattedExpression(std::string const& portName) const
{
    if (QSharedPointer<Port> editedPort = getPort(portName); 
        portIsWire(editedPort))
    {
        return formattedValueFor(editedPort->getWire()->getDefaultDriverValue()).toStdString();
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getDefaultValueExpression()
//-----------------------------------------------------------------------------
std::string PortsInterface::getDefaultValueExpression(std::string const& portName) const
{
    if (QSharedPointer<Port> editedPort = getPort(portName); 
        portIsWire(editedPort))
    {
        return editedPort->getWire()->getDefaultDriverValue().toStdString();
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::setDefaultValue()
//-----------------------------------------------------------------------------
bool PortsInterface::setDefaultValue(std::string const& portName, std::string const& newDefaultValue) const
{
    if (QSharedPointer<Port> editedPort = getPort(portName); 
        portIsWire(editedPort))
    {
        editedPort->getWire()->setDefaultDriverValue(QString::fromStdString(newDefaultValue));
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::portIsTransactional()
//-----------------------------------------------------------------------------
bool PortsInterface::portIsTransactional(std::string const& portName) const
{
    return portIsTransactional(getPort(portName));
}
//-----------------------------------------------------------------------------
// Function: PortsInterface::portIsTransactional()
//-----------------------------------------------------------------------------
bool PortsInterface::portIsTransactional(QSharedPointer<Port> port) const
{
    return port.isNull() == false && port->getTransactional().isNull() == false;
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getBusWidthValue()
//-----------------------------------------------------------------------------
std::string PortsInterface::getBusWidthValue(std::string const& portName, unsigned int baseNumber) const
{
    QSharedPointer<Port> editedPort = getPort(portName);
    if (portIsTransactional(editedPort))
    {
        return
            parseExpressionToBaseNumber(editedPort->getTransactional()->getBusWidth(), baseNumber).toStdString();
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getBusWidth()
//-----------------------------------------------------------------------------
std::string PortsInterface::getBusWidthFormattedExpression(std::string const& portName) const
{
    if (QSharedPointer<Port> editedPort = getPort(portName); 
        portIsTransactional(editedPort))
    {
        return formattedValueFor(editedPort->getTransactional()->getBusWidth()).toStdString();
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getBusWidthExpression()
//-----------------------------------------------------------------------------
std::string PortsInterface::getBusWidthExpression(std::string const& portName) const
{
    if (QSharedPointer<Port> editedPort = getPort(portName); 
        portIsTransactional(editedPort))
    {
        return editedPort->getTransactional()->getBusWidth().toStdString();
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::setBusWidth()
//-----------------------------------------------------------------------------
bool PortsInterface::setBusWidth(std::string const& portName, std::string const& newBusWidth) const
{
    if (QSharedPointer<Port> editedPort = getPort(portName); 
        portIsTransactional(editedPort))
    {
        editedPort->getTransactional()->setBusWidth(QString::fromStdString(newBusWidth));
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getInitiative()
//-----------------------------------------------------------------------------
std::string PortsInterface::getInitiative(std::string const& portName) const
{
    if (QSharedPointer<Port> editedPort = getPort(portName); 
        portIsTransactional(editedPort))
    {
        return editedPort->getTransactional()->getInitiative().toStdString();
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::setInitiative()
//-----------------------------------------------------------------------------
bool PortsInterface::setInitiative(std::string const& portName, std::string const& newInitiative) const
{
    if (QSharedPointer<Port> editedPort = getPort(portName); 
        portIsTransactional(editedPort))
    {
        editedPort->getTransactional()->setInitiative(QString::fromStdString(newInitiative));
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getKind()
//-----------------------------------------------------------------------------
std::string PortsInterface::getKind(std::string const& portName) const
{
    if (QSharedPointer<Port> editedPort = getPort(portName); 
        portIsTransactional(editedPort))
    {
        return editedPort->getTransactional()->getKind().toStdString();
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::setKind()
//-----------------------------------------------------------------------------
bool PortsInterface::setKind(std::string const& portName, std::string const& newKind) const
{
    if (QSharedPointer<Port> editedPort = getPort(portName); 
        portIsTransactional(editedPort))
    {
        editedPort->getTransactional()->setKind(QString::fromStdString(newKind));
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getProtocolType()
//-----------------------------------------------------------------------------
std::string PortsInterface::getProtocolType(std::string const& portName) const
{
    if (QSharedPointer<Port> editedPort = getPort(portName); 
        portIsTransactional(editedPort))
    {
        QSharedPointer<Protocol> portProtocol = editedPort->getTransactional()->getProtocol();
        if (portProtocol)
        {
            return portProtocol->getProtocolType().toStdString();
        }
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::setProtocolType()
//-----------------------------------------------------------------------------
bool PortsInterface::setProtocolType(std::string const& portName, std::string const& newProtocolType) const
{
    if (QSharedPointer<Port> editedPort = getPort(portName); 
        portIsTransactional(editedPort))
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
std::string PortsInterface::getMaxConnectionsValue(std::string const& portName, unsigned int baseNumber) const
{
    QSharedPointer<Port> editedPort = getPort(portName);
    if (portIsTransactional(editedPort))
    {
        return parseExpressionToBaseNumber(
            editedPort->getTransactional()->getMaxConnections(), baseNumber).toStdString();
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getMaxConnections()
//-----------------------------------------------------------------------------
std::string PortsInterface::getMaxConnectionsFormattedExpression(std::string const& portName) const
{
    if (QSharedPointer<Port> editedPort = getPort(portName); 
        portIsTransactional(editedPort))
    {
        return formattedValueFor(editedPort->getTransactional()->getMaxConnections()).toStdString();
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getMaxConnectionsExpression()
//-----------------------------------------------------------------------------
std::string PortsInterface::getMaxConnectionsExpression(std::string const& portName) const
{
    if (QSharedPointer<Port> editedPort = getPort(portName); 
        portIsTransactional(editedPort))
    {
        return editedPort->getTransactional()->getMaxConnections().toStdString();
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::setMaxConnections()
//-----------------------------------------------------------------------------
bool PortsInterface::setMaxConnections(std::string const& portName, std::string const& newMaxConnections) const
{
    if (QSharedPointer<Port> editedPort = getPort(portName); 
        portIsTransactional(editedPort))
    {
        editedPort->getTransactional()->setMaxConnections(QString::fromStdString(newMaxConnections));
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getMinConnectionsValue()
//-----------------------------------------------------------------------------
std::string PortsInterface::getMinConnectionsValue(std::string const& portName, unsigned int baseNumber) const
{
    if (QSharedPointer<Port> editedPort = getPort(portName); 
        portIsTransactional(editedPort))
    {
        return parseExpressionToBaseNumber(
            editedPort->getTransactional()->getMinConnections(), baseNumber).toStdString();
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getMinConnections()
//-----------------------------------------------------------------------------
std::string PortsInterface::getMinConnectionsFormattedExpression(std::string const& portName) const
{
    if (QSharedPointer<Port> editedPort = getPort(portName); 
        portIsTransactional(editedPort))
    {
        return formattedValueFor(editedPort->getTransactional()->getMinConnections()).toStdString();
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getMinConnectionsExpression()
//-----------------------------------------------------------------------------
std::string PortsInterface::getMinConnectionsExpression(std::string const& portName) const
{
   if (QSharedPointer<Port> editedPort = getPort(portName); 
       portIsTransactional(editedPort))
    {
        return editedPort->getTransactional()->getMinConnections().toStdString();
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::setMinConnections()
//-----------------------------------------------------------------------------
bool PortsInterface::setMinConnections(std::string const& portName, std::string const& newMinConnections) const
{
    if (QSharedPointer<Port> editedPort = getPort(portName);
        portIsTransactional(editedPort))
    {
        editedPort->getTransactional()->setMinConnections(QString::fromStdString(newMinConnections));
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::getAllReferencesToIdInPort()
//-----------------------------------------------------------------------------
int PortsInterface::getAllReferencesToIdInItem(const std::string& itemName, std::string const& valueID) const
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

    if (portIsWire(port))
    {
        QSharedPointer<Wire> portWire = port->getWire();

        totalReferences += port->getWire()->getVectorLeftBound().count(idString);
        totalReferences += port->getWire()->getVectorRightBound().count(idString);
        totalReferences += port->getWire()->getDefaultDriverValue().count(idString);
    }
    else if (portIsTransactional(port))
    {
        QSharedPointer<Transactional> portTransactional = port->getTransactional();

        totalReferences += portTransactional->getBusWidth().count(idString);
        totalReferences += portTransactional->getMaxConnections().count(idString);
        totalReferences += portTransactional->getMinConnections().count(idString);
    }

    return totalReferences;
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::addWirePort()
//-----------------------------------------------------------------------------
void PortsInterface::addWirePort(std::string const& newPortName)
{
    QString portName(getUniqueName(newPortName, "port"));

    QSharedPointer<Port> newPort(new Port(portName));
    newPort->setWire(QSharedPointer<Wire>(new Wire()));

    ports_->append(newPort);
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::addTransactionalPort()
//-----------------------------------------------------------------------------
void PortsInterface::addTransactionalPort(std::string const& newPortName)
{
    QString portName(getUniqueName(newPortName, "port"));

    QSharedPointer<Port> newPort(new Port(portName));
    newPort->setTransactional(QSharedPointer<Transactional>(new Transactional()));

    ports_->append(newPort);
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::removePort()
//-----------------------------------------------------------------------------
bool PortsInterface::removePort(std::string const& portName) const
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
    return Search::findByName(QString::fromStdString(portName), ports_) != nullptr;
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::validateItems()
//-----------------------------------------------------------------------------
bool PortsInterface::validateItems() const
{
    return std::all_of(ports_->cbegin(), ports_->cend(),
        [this](auto const& port) { return portValidator_->validate(port); });
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::itemHasValidName()
//-----------------------------------------------------------------------------
bool PortsInterface::itemHasValidName(std::string const& portName) const
{
    return portValidator_->hasValidName(QString::fromStdString(portName));
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::portLeftArrayValueIsValid()
//-----------------------------------------------------------------------------
bool PortsInterface::portLeftArrayValueIsValid(std::string const& portName) const
{
    return portValidator_->arrayValueIsValid(getPort(portName)->getArrayLeft());
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::portRightArrayValueIsValid()
//-----------------------------------------------------------------------------
bool PortsInterface::portRightArrayValueIsValid(std::string const& portName) const
{
    return portValidator_->arrayValueIsValid(getPort(portName)->getArrayRight());
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::portHasValidTypes()
//-----------------------------------------------------------------------------
bool PortsInterface::portHasValidTypes(std::string const& portName) const
{
    return portValidator_->hasValidTypes(getPort(portName));
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::portHasValidLeftBound()
//-----------------------------------------------------------------------------
bool PortsInterface::portHasValidLeftBound(std::string const& portName) const
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
bool PortsInterface::portHasValidRightBound(std::string const& portName) const
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
bool PortsInterface::portHasValidDefaultValue(std::string const& portName) const
{
    return portValidator_->hasValidDefaultValue(getPort(portName));
}

//-----------------------------------------------------------------------------
// Function: PortsInterface::portHasValidBusWidth()
//-----------------------------------------------------------------------------
bool PortsInterface::portHasValidBusWidth(std::string const& portName) const
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
bool PortsInterface::portHasValidInitiative(std::string const& portName) const
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
bool PortsInterface::portHasValidKind(std::string const& portName) const
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
bool PortsInterface::portHasValidProtocol(std::string const& portName) const
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
bool PortsInterface::portHasValidMaxConnections(std::string const& portName) const
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
bool PortsInterface::portHasValidMinConnections(std::string const& portName) const
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
    std::string path;
    if (QSharedPointer<Port> selectedPort = getPort(portName); 
        !selectedPort)
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
