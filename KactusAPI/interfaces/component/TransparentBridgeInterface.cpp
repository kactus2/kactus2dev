//-----------------------------------------------------------------------------
// File: TransparentBridgeInterface.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 24.02.2021
//
// Description:
// Interface for accessing transparent bridges.
//-----------------------------------------------------------------------------

#include <TransparentBridgeInterface.h>

#include <IPXACTmodels/Component/TransparentBridge.h>
#include <IPXACTmodels/Component/SlaveInterface.h>
#include <IPXACTmodels/Component/IndirectInterface.h>
#include <IPXACTmodels/Component/validators/BusInterfaceValidator.h>
#include <IPXACTmodels/Component/validators/IndirectInterfaceValidator.h>

namespace
{
    QString const DEFAULT_NAME = QLatin1String("");
};

//-----------------------------------------------------------------------------
// Function: TransparentBridgeInterface::TransparentBridgeInterface()
//-----------------------------------------------------------------------------
TransparentBridgeInterface::TransparentBridgeInterface(QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<ExpressionFormatter> expressionFormatter):
ParameterizableInterface(expressionParser, expressionFormatter),
bridges_(),
busValidator_(),
indirectValidator_()
{

}

//-----------------------------------------------------------------------------
// Function: TransparentBridgeInterface::setBridges()
//-----------------------------------------------------------------------------
void TransparentBridgeInterface::setBridges(QSharedPointer<QList<QSharedPointer<TransparentBridge> > > newBridges)
{
    bridges_ = newBridges;
}

//-----------------------------------------------------------------------------
// Function: TransparentBridgeInterface::getTransparentBridge()
//-----------------------------------------------------------------------------
QSharedPointer<TransparentBridge> TransparentBridgeInterface::getTransparentBridge(
    std::string const& masterReference) const
{
    QString masterQ = QString::fromStdString(masterReference);
    for (auto bridge : *bridges_)
    {
        if (bridge->getMasterRef() == masterQ)
        {
            return bridge;
        }
    }

    return QSharedPointer<TransparentBridge>();
}

//-----------------------------------------------------------------------------
// Function: TransparentBridgeInterface::getItemIndex()
//-----------------------------------------------------------------------------
int TransparentBridgeInterface::getItemIndex(std::string const& masterReference) const
{
    QString master = QString::fromStdString(masterReference);

    if (bridges_)
    {
        for (int i = 0; i < bridges_->count(); ++i)
        {
            auto bridge = bridges_->at(i);
            if (bridge && bridge->getMasterRef() == master)
            {
                return i;
            }
        }
    }
    return -1;
}

//-----------------------------------------------------------------------------
// Function: TransparentBridgeInterface::getIndexedMasterReference()
//-----------------------------------------------------------------------------
std::string TransparentBridgeInterface::getIndexedMasterReference(int const& itemIndex) const
{
    std::string masterReference = "";
    if (bridges_ && itemIndex >= 0 && itemIndex < bridges_->count())
    {
        masterReference = bridges_->at(itemIndex)->getMasterRef().toStdString();
    }

    return masterReference;
}

//-----------------------------------------------------------------------------
// Function: TransparentBridgeInterface::itemCount()
//-----------------------------------------------------------------------------
int TransparentBridgeInterface::itemCount() const
{
    if (bridges_)
    {
        return bridges_->count();
    }
    else
    {
        return 0;
    }
}

//-----------------------------------------------------------------------------
// Function: TransparentBridgeInterface::getItemNames()
//-----------------------------------------------------------------------------
std::vector<std::string> TransparentBridgeInterface::getItemNames() const
{
    QVector<QString> bridgeMastersQ;
    if (bridges_)
    {
        for (auto bridge : *bridges_)
        {
            QString bridgeMaster = bridge->getMasterRef();
            if (!bridgeMastersQ.contains(bridgeMaster))
            {
                bridgeMastersQ.append(bridgeMaster);
            }
        }
    }

    std::vector<std::string> bridgeMasters;
    for (auto master : bridgeMastersQ)
    {
        bridgeMasters.push_back(master.toStdString());
    }

    return bridgeMasters;
}

//-----------------------------------------------------------------------------
// Function: TransparentBridgeInterface::setMasterReference()
//-----------------------------------------------------------------------------
bool TransparentBridgeInterface::setMasterReference(std::string const& currentMaster, std::string const& newMaster)
{
    QSharedPointer<TransparentBridge> selectedBridge = getTransparentBridge(currentMaster);
    if (!selectedBridge)
    {
        return false;
    }

    selectedBridge->setMasterRef(QString::fromStdString(newMaster));
    return true;
}

//-----------------------------------------------------------------------------
// Function: TransparentBridgeInterface::validateItems()
//-----------------------------------------------------------------------------
bool TransparentBridgeInterface::validateItems() const
{
    if (busValidator_)
    {
        return busValidator_->slaveInterfaceHasValidBridges(bridges_);
    }
    else if (indirectValidator_)
    {
        return indirectValidator_->hasValidTransparentBridges(bridges_);
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: TransparentBridgeInterface::getIsPresentValue()
//-----------------------------------------------------------------------------
std::string TransparentBridgeInterface::getIsPresentValue(std::string const& masterReference,
    int const& baseNumber) const
{
    QSharedPointer<TransparentBridge> selectedBridge = getTransparentBridge(masterReference);
    if (selectedBridge)
    {
        return parseExpressionToBaseNumber(selectedBridge->getIsPresent(), baseNumber).toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: TransparentBridgeInterface::getIsPresentFormattedExpression()
//-----------------------------------------------------------------------------
std::string TransparentBridgeInterface::getIsPresentFormattedExpression(std::string const& masterReference) const
{
    QSharedPointer<TransparentBridge> selectedBridge = getTransparentBridge(masterReference);
    if (selectedBridge)
    {
        return formattedValueFor(selectedBridge->getIsPresent()).toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: TransparentBridgeInterface::getIsPresentExpression()
//-----------------------------------------------------------------------------
std::string TransparentBridgeInterface::getIsPresentExpression(std::string const& masterReference) const
{
    QSharedPointer<TransparentBridge> selectedBridge = getTransparentBridge(masterReference);
    if (selectedBridge)
    {
        return selectedBridge->getIsPresent().toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: TransparentBridgeInterface::setIsPresent()
//-----------------------------------------------------------------------------
bool TransparentBridgeInterface::setIsPresent(std::string const& masterReference, std::string const& newIsPresent)
{
    QSharedPointer<TransparentBridge> selectedBridge = getTransparentBridge(masterReference);
    if (!selectedBridge)
    {
        return false;
    }

    selectedBridge->setIsPresent(QString::fromStdString(newIsPresent));
    return true;

}

//-----------------------------------------------------------------------------
// Function: TransparentBridgeInterface::addBridge()
//-----------------------------------------------------------------------------
bool TransparentBridgeInterface::addBridge(int const& bridgeIndex, std::string const& bridgeName) const
{
    if (bridgeIndex < 0)
    {
        return false;
    }

    QSharedPointer<TransparentBridge> newBridge(new TransparentBridge());
    newBridge->setMasterRef(getUniqueName(bridgeName, DEFAULT_NAME.toStdString()));

    bridges_->insert(bridgeIndex, newBridge);
    return true;
}

//-----------------------------------------------------------------------------
// Function: TransparentBridgeInterface::removeTransparentBridge()
//-----------------------------------------------------------------------------
bool TransparentBridgeInterface::removeTransparentBridge(std::string const& masterReference)
{
    QSharedPointer<TransparentBridge> bridge = getTransparentBridge(masterReference);
    if (!bridge)
    {
        return false;
    }

    bridges_->removeOne(bridge);
    return true;
}

//-----------------------------------------------------------------------------
// Function: TransparentBridgeInterface::getAllReferencesToIdInItem()
//-----------------------------------------------------------------------------
int TransparentBridgeInterface::getAllReferencesToIdInItem(const std::string& itemName,
    std::string const&  valueID) const
{
    int totalReferencesToParameter = 0;
    QSharedPointer<TransparentBridge> selectedItem = getTransparentBridge(itemName);
    if (selectedItem)
    {
        QString idString = QString::fromStdString(valueID);

        int referencesInIsPresent = selectedItem->getIsPresent().count(idString);
        totalReferencesToParameter = referencesInIsPresent;
    }


    return totalReferencesToParameter;
}

//-----------------------------------------------------------------------------
// Function: TransparentBridgeInterface::getAllExpressions()
//-----------------------------------------------------------------------------
std::vector<std::string> TransparentBridgeInterface::getAllExpressions(std::string const& masterReference) const
{
    std::vector<std::string> expressions;

    QString isPresentExpression = QString::fromStdString(getIsPresentExpression(masterReference));
    if (!isPresentExpression.isEmpty())
    {
        expressions.push_back(isPresentExpression.toStdString());
    }

    return expressions;
}
