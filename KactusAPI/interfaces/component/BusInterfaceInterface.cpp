//-----------------------------------------------------------------------------
// File: BusInterfaceInterface.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 17.02.2021
//
// Description:
// Interface for accessing bus interfaces.
//-----------------------------------------------------------------------------

#include "BusInterfaceInterface.h"

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/InitiatorInterface.h>
#include <IPXACTmodels/Component/TargetInterface.h>
#include <IPXACTmodels/Component/TransparentBridge.h>
#include <IPXACTmodels/Component/validators/BusInterfaceValidator.h>

#include <TransparentBridgeInterface.h>
#include <AbstractionTypeInterface.h>
#include <ParametersInterface.h>
#include <FileSetInterface.h>
#include <MemoryMapInterface.h>

#include <QMimeData>
#include <QApplication>
#include <QClipboard>

namespace
{
    QString const DEFAULT_NAME = QLatin1String("bus_interface");
    QString const DEFAULT_FILEGROUP = QLatin1String("default");
};

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::BusInterfaceInterface()
//-----------------------------------------------------------------------------
BusInterfaceInterface::BusInterfaceInterface(QSharedPointer<BusInterfaceValidator> busValidator,
    QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<ExpressionFormatter> expressionFormatter):
ParameterizableInterface(expressionParser, expressionFormatter),
NameGroupInterface(),
busInterfaces_(0),
busValidator_(busValidator),
bridgeInterface_(0),
fileSetInterface_(0),
mapInterface_(0),
abstractionTypeInterface_(0),
parameterInterface_(0)
{

}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::setFileSetInterface()
//-----------------------------------------------------------------------------
void BusInterfaceInterface::setFileSetInterface(FileSetInterface* newFileSetInterface)
{
    fileSetInterface_ = newFileSetInterface;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::setMemoryMapInterface()
//-----------------------------------------------------------------------------
void BusInterfaceInterface::setMemoryMapInterface(MemoryMapInterface* newMapInterface)
{
    mapInterface_ = newMapInterface;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::setAbstractionTypeInterface()
//-----------------------------------------------------------------------------
void BusInterfaceInterface::setAbstractionTypeInterface(AbstractionTypeInterface* newAbstractionTypeInterface)
{
    abstractionTypeInterface_ = newAbstractionTypeInterface;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::setTransparentBridgeInterface()
//-----------------------------------------------------------------------------
void BusInterfaceInterface::setTransparentBridgeInterface(TransparentBridgeInterface* newBridgeInterface)
{
    bridgeInterface_ = newBridgeInterface;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::setParameterInterface()
//-----------------------------------------------------------------------------
void BusInterfaceInterface::setParameterInterface(ParametersInterface* newParameterInterface)
{
    parameterInterface_ = newParameterInterface;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::setBusInterfaces()
//-----------------------------------------------------------------------------
void BusInterfaceInterface::setBusInterfaces(QSharedPointer<Component> newComponent)
{
    busInterfaces_ = newComponent->getBusInterfaces();
    docRevision_ = newComponent->getRevision();

    if (fileSetInterface_ != NULL)
    {
        fileSetInterface_->setFileSets(newComponent->getFileSets());
    }
    if (mapInterface_ != NULL)
    {
        mapInterface_->setMemoryMaps(newComponent);
    }

    busValidator_->componentChange(newComponent->getChoices(), newComponent->getViews(), newComponent->getPorts(),
        newComponent->getAddressSpaces(), newComponent->getMemoryMaps(), busInterfaces_,
        newComponent->getFileSets(), newComponent->getRemapStates(), newComponent->getModes());
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::setupSubInterfaces()
//-----------------------------------------------------------------------------
void BusInterfaceInterface::setupSubInterfaces(std::string const& busName)
{
    QSharedPointer<BusInterface> selectedBus = getBusInterface(busName);
    if (selectedBus)
    {
        auto mode = selectedBus->getInterfaceMode();
        if (mode == General::SLAVE || mode == General::TARGET)
        {
            bridgeInterface_->setBridges(selectedBus->getSlave()->getBridges());
        }
        else
        {
            bridgeInterface_->setBridges(QSharedPointer<QList<QSharedPointer<TransparentBridge> > >());
        }

        General::InterfaceMode busMode = getActiveMode(busName);
        QString systemGroup = getActiveSystemGroup(busName);

        abstractionTypeInterface_->setAbstractionTypes(selectedBus->getAbstractionTypes(), busMode, systemGroup);
        parameterInterface_->setParameters(selectedBus->getParameters());
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::getActiveMode()
//-----------------------------------------------------------------------------
General::InterfaceMode BusInterfaceInterface::getActiveMode(std::string const& busName) const
{
    General::InterfaceMode busMode = getMode(busName);
    if (busMode == General::MONITOR)
    {
        busMode = getMonitorMode(busName);
    }

    return busMode;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::getActiveSystemGroup()
//-----------------------------------------------------------------------------
QString BusInterfaceInterface::getActiveSystemGroup(std::string const& busName) const
{
    std::string systemGroup = getSystemGroup(busName);
    General::InterfaceMode busMode = getMode(busName);
    if (busMode == General::MONITOR)
    {
        systemGroup = getMonitorGroup(busName);
    }

    return QString::fromStdString(systemGroup);
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::getBusInterface()
//-----------------------------------------------------------------------------
QSharedPointer<BusInterface> BusInterfaceInterface::getBusInterface(std::string const& busName) const
{
    QString busNameQ = QString::fromStdString(busName);
    for (auto bus : *busInterfaces_)
    {
        if (bus->name() == busNameQ)
        {
            return bus;
        }
    }

    return QSharedPointer<BusInterface>();
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::getItem()
//-----------------------------------------------------------------------------
QSharedPointer<NameGroup> BusInterfaceInterface::getItem(std::string const& busName) const
{
    return getBusInterface(busName);
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::getInitiatorInterface()
//-----------------------------------------------------------------------------
QSharedPointer<InitiatorInterface> BusInterfaceInterface::getInitiatorInterface(std::string const& busName) const
{
    QSharedPointer<InitiatorInterface> selectedInitiator;

    QSharedPointer<BusInterface> selectedBus = getBusInterface(busName);
    if (selectedBus)
    {
        selectedInitiator = selectedBus->getInitiator();
    }

    return selectedInitiator;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::createInitiatorForBusInterface()
//-----------------------------------------------------------------------------
QSharedPointer<InitiatorInterface> BusInterfaceInterface::createInitiatorForBusInterface(std::string const& busName)
{
    QSharedPointer<InitiatorInterface> selectedInitiator = getInitiatorInterface(busName);
    if (!selectedInitiator)
    {
        QSharedPointer<BusInterface> containingBus = getBusInterface(busName);
        if (containingBus)
        {
            selectedInitiator = QSharedPointer<InitiatorInterface>(new InitiatorInterface());

            if (docRevision_ == Document::Revision::Std22)
            {
                containingBus->setInitiator(selectedInitiator);
            }
            else
            {
                containingBus->setMaster(selectedInitiator);
            }
        }
    }

    return selectedInitiator;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::getTargetInterface()
//-----------------------------------------------------------------------------
QSharedPointer<TargetInterface> BusInterfaceInterface::getTargetInterface(std::string const& busName) const
{
    QSharedPointer<TargetInterface> selectedTarget;

    QSharedPointer<BusInterface> selectedBus = getBusInterface(busName);
    if (selectedBus)
    {
        selectedTarget = selectedBus->getTarget();
    }

    return selectedTarget;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::createTargetForBusInterface()
//-----------------------------------------------------------------------------
QSharedPointer<TargetInterface> BusInterfaceInterface::createTargetForBusInterface(std::string const& busName)
{
    QSharedPointer<TargetInterface> selectedTarget = getTargetInterface(busName);
    if (!selectedTarget)
    {
        QSharedPointer<BusInterface> containingBus = getBusInterface(busName);
        if (containingBus)
        {
            selectedTarget = QSharedPointer<TargetInterface>(new TargetInterface());
            if (docRevision_ == Document::Revision::Std22)
            {
                containingBus->setTarget(selectedTarget);
            }
            else
            {
                containingBus->setSlave(selectedTarget);
            }
        }
    }

    return selectedTarget;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::getMirroredSlaveInterface()
//-----------------------------------------------------------------------------
QSharedPointer<MirroredTargetInterface> BusInterfaceInterface::getMirroredSlaveInterface(std::string const& busName)
const
{
    QSharedPointer<MirroredTargetInterface> selectedMirrorSlave;

    QSharedPointer<BusInterface> selectedBus = getBusInterface(busName);
    if (selectedBus)
    {
        selectedMirrorSlave = selectedBus->getMirroredSlave();
    }

    return selectedMirrorSlave;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::createMirrorSlaveForBusInterface()
//-----------------------------------------------------------------------------
QSharedPointer<MirroredTargetInterface> BusInterfaceInterface::createMirrorSlaveForBusInterface(
    std::string const& busName)
{
    QSharedPointer<MirroredTargetInterface> mirrorSlave = getMirroredSlaveInterface(busName);
    if (!mirrorSlave)
    {
        QSharedPointer<BusInterface> containingBus = getBusInterface(busName);
        if (containingBus)
        {
            mirrorSlave = QSharedPointer<MirroredTargetInterface>(new MirroredTargetInterface());
            containingBus->setMirroredSlave(mirrorSlave);
        }
    }

    return mirrorSlave;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::getMonitorInterface()
//-----------------------------------------------------------------------------
QSharedPointer<BusInterface::MonitorInterface> BusInterfaceInterface::getMonitorInterface(
    std::string const& busName) const
{
    QSharedPointer<BusInterface::MonitorInterface> selectedMonitor;

    QSharedPointer<BusInterface> selectedBus = getBusInterface(busName);
    if (selectedBus)
    {
        selectedMonitor = selectedBus->getMonitor();
    }

    return selectedMonitor;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::createMonitorForBusInterface()
//-----------------------------------------------------------------------------
QSharedPointer<BusInterface::MonitorInterface> BusInterfaceInterface::createMonitorForBusInterface(
    std::string const& busName)
{
    QSharedPointer<BusInterface::MonitorInterface> selectedMonitor = getMonitorInterface(busName);
    if (!selectedMonitor)
    {
        QSharedPointer<BusInterface> containingBus = getBusInterface(busName);
        if (containingBus)
        {
            selectedMonitor = QSharedPointer<BusInterface::MonitorInterface>(new BusInterface::MonitorInterface());
            containingBus->setMonitor(selectedMonitor);
        }
    }

    return selectedMonitor;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::getItemIndex()
//-----------------------------------------------------------------------------
int BusInterfaceInterface::getItemIndex(std::string const& itemName) const
{
    QString itemNameQ = QString::fromStdString(itemName);
    for (int i = 0; i < busInterfaces_->size(); ++i)
    {
        if (busInterfaces_->at(i)->name() == itemNameQ)
        {
            return i;
        }
    }

    return -1;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::getIndexedItemName()
//-----------------------------------------------------------------------------
std::string BusInterfaceInterface::getIndexedItemName(int const& itemIndex) const
{
    std::string portName = "";
    if (itemIndex >= 0 && itemIndex < itemCount())
    {
        portName = busInterfaces_->at(itemIndex)->name().toStdString();
    }

    return portName;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::itemCount()
//-----------------------------------------------------------------------------
int BusInterfaceInterface::itemCount() const
{
    return busInterfaces_->count();
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::getItemNames()
//-----------------------------------------------------------------------------
std::vector<std::string> BusInterfaceInterface::getItemNames() const
{
    QVector<QString> busNamesQ;
    for (auto bus : *busInterfaces_)
    {
        if (!busNamesQ.contains(bus->name()))
        {
            busNamesQ.append(bus->name());
        }
    }

    std::vector<std::string> busNames;
    for (auto busName : busNamesQ)
    {
        busNames.push_back(busName.toStdString());
    }

    return busNames;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::setName()
//-----------------------------------------------------------------------------
bool BusInterfaceInterface::setName(std::string const& currentName, std::string const& newName)
{
    QSharedPointer<BusInterface> editedBus = getBusInterface(currentName);
    if (editedBus && nameHasChanged(newName, currentName))
    {
        QString uniqueNewName = getUniqueName(newName, DEFAULT_NAME.toStdString());

        editedBus->setName(uniqueNewName);
        return true;
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::validateItems()
//-----------------------------------------------------------------------------
bool BusInterfaceInterface::validateItems() const
{
    for (auto bus : *busInterfaces_)
    {
        if (!busValidator_->validate(bus, Document::Revision::Std14))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::validateBusInterface()
//-----------------------------------------------------------------------------
bool BusInterfaceInterface::validateBusInterface(std::string const& busName) const
{
    QSharedPointer<BusInterface> selectedBus = getBusInterface(busName);
    if (!selectedBus)
    {
        return false;
    }

    return busValidator_->validate(selectedBus, Document::Revision::Std14);
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::itemHasValidName()
//-----------------------------------------------------------------------------
bool BusInterfaceInterface::itemHasValidName(std::string const& itemName) const
{
    QSharedPointer<BusInterface> selectedBus = getBusInterface(itemName);
    if (selectedBus)
    {
        return busValidator_->hasValidName(selectedBus);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::hasValidBusType()
//-----------------------------------------------------------------------------
bool BusInterfaceInterface::hasValidBusType(std::string const& busName) const
{
    QSharedPointer<BusInterface> selectedBus = getBusInterface(busName);
    if (selectedBus)
    {
        return busValidator_->hasValidBusType(selectedBus);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::hasValidAbstractionTypes()
//-----------------------------------------------------------------------------
bool BusInterfaceInterface::hasValidAbstractionTypes(std::string const& busName) const
{
    QSharedPointer<BusInterface> selectedBus = getBusInterface(busName);
    if (selectedBus)
    {
        return busValidator_->hasValidAbstractionTypes(selectedBus);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::getMode()
//-----------------------------------------------------------------------------
General::InterfaceMode BusInterfaceInterface::getMode(std::string const& busName) const
{
    QSharedPointer<BusInterface> selectedBus = getBusInterface(busName);
    if (!selectedBus)
    {
        return General::INTERFACE_MODE_COUNT;
    }

    return selectedBus->getInterfaceMode();
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::getModeString()
//-----------------------------------------------------------------------------
std::string BusInterfaceInterface::getModeString(std::string const& busName) const
{
    QSharedPointer<BusInterface> selectedBus = getBusInterface(busName);
    if (!selectedBus)
    {
        return std::string();
    }

    return General::interfaceMode2Str(selectedBus->getInterfaceMode()).toStdString();
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::setMode()
//-----------------------------------------------------------------------------
bool BusInterfaceInterface::setMode(std::string const& busName, std::string const& newMode)
{
    QSharedPointer<BusInterface> selectedBus = getBusInterface(busName);
    if (!selectedBus)
    {
        return false;
    }

    General::InterfaceMode newInterfaceMode =
        General::str2Interfacemode(QString::fromStdString(newMode), General::INTERFACE_MODE_COUNT);
    selectedBus->setInterfaceMode(newInterfaceMode);

    abstractionTypeInterface_->setBusMode(newInterfaceMode);

    return true;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::getSystemGroup()
//-----------------------------------------------------------------------------
std::string BusInterfaceInterface::getSystemGroup(std::string const& busName) const
{
    std::string systemGroup = "";
    QSharedPointer<BusInterface> selectedBus = getBusInterface(busName);
    if (selectedBus)
    {
        systemGroup = selectedBus->getSystem().toStdString();
    }

    return systemGroup;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::setSystemGroup()
//-----------------------------------------------------------------------------
bool BusInterfaceInterface::setSystemGroup(std::string const& busName, std::string const& newSystem)
{
    QSharedPointer<BusInterface> selectedBus = getBusInterface(busName);
    if (!selectedBus)
    {
        return false;
    }

    QString newSystemQ = QString::fromStdString(newSystem);

    selectedBus->setSystem(newSystemQ);

    abstractionTypeInterface_->setSystemGroup(newSystemQ);

    return true;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::connectionIsRequired()
//-----------------------------------------------------------------------------
std::string BusInterfaceInterface::connectionIsRequired(std::string const& busName) const
{
    std::string requirement = "";

    QSharedPointer<BusInterface> selectedBus = getBusInterface(busName);
    if (selectedBus)
    {
        requirement = selectedBus->getConnectionRequired().toStdString();
    }

    return requirement;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::setConnectionIsRequired()
//-----------------------------------------------------------------------------
bool BusInterfaceInterface::setConnectionIsRequired(std::string const& busName, bool newValue)
{
    QSharedPointer<BusInterface> selectedBus = getBusInterface(busName);
    if (!selectedBus)
    {
        return false;
    }

    selectedBus->setConnectionRequired(newValue);
    return true;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::getBitsInLau()
//-----------------------------------------------------------------------------
std::string BusInterfaceInterface::getBitsInLau(std::string const& busName) const
{
    std::string bitsInLau = "";

    QSharedPointer<BusInterface> selectedBus = getBusInterface(busName);
    if (selectedBus)
    {
        bitsInLau = selectedBus->getBitsInLau().toStdString();
    }

    return bitsInLau;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::setBitsInLau()
//-----------------------------------------------------------------------------
bool BusInterfaceInterface::setBitsInLau(std::string const& busName, std::string const& newBits)
{
    QSharedPointer<BusInterface> selectedBus = getBusInterface(busName);
    if (!selectedBus)
    {
        return false;
    }

    selectedBus->setBitsInLau(QString::fromStdString(newBits));
    return true;

}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::getEndianness()
//-----------------------------------------------------------------------------
BusInterface::Endianness BusInterfaceInterface::getEndianness(std::string const& busName) const
{
    BusInterface::Endianness endianness = BusInterface::ENDIANNESS_UNSPECIFIED;

    QSharedPointer<BusInterface> selectedBus = getBusInterface(busName);
    if (selectedBus)
    {
        endianness = selectedBus->getEndianness();
    }

    return endianness;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::setEndianness()
//-----------------------------------------------------------------------------
bool BusInterfaceInterface::setEndianness(std::string const& busName, std::string const& newEndianness)
{
    QSharedPointer<BusInterface> selectedBus = getBusInterface(busName);
    if (!selectedBus)
    {
        return false;
    }

    QString newEndiannessQ = QString::fromStdString(newEndianness);
    if (newEndiannessQ == QLatin1String("little"))
    {
        selectedBus->setEndianness(BusInterface::LITTLE);
    }
    else if (newEndiannessQ == QLatin1String("big"))
    {
        selectedBus->setEndianness(BusInterface::BIG);
    }
    else
    {
        selectedBus->setEndianness(BusInterface::ENDIANNESS_UNSPECIFIED);
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::getBitSteering()
//-----------------------------------------------------------------------------
std::string BusInterfaceInterface::getBitSteering(std::string const& busName) const
{
    auto bitSteering = QString();

    QSharedPointer<BusInterface> selectedBus = getBusInterface(busName);
    if (selectedBus)
    {
        bitSteering = selectedBus->getBitSteering();
    }

    return bitSteering.toStdString();
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::setBitSteering()
//-----------------------------------------------------------------------------
bool BusInterfaceInterface::setBitSteering(std::string const& busName, std::string const& newBitSteering)
{
    QSharedPointer<BusInterface> selectedBus = getBusInterface(busName);
    if (!selectedBus)
    {
        return false;
    }

    QString newBitsQ = QString::fromStdString(newBitSteering);

    selectedBus->setBitSteering(newBitsQ);

    return true;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::addBusInterface()
//-----------------------------------------------------------------------------
void BusInterfaceInterface::addBusInterface(int const& newBusIndex, std::string const& busName)
{
    QSharedPointer<BusInterface> newBus(new BusInterface());
    newBus->setName(getUniqueName(busName, DEFAULT_NAME.toStdString()));

    busInterfaces_->insert(newBusIndex, newBus);
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::removeBusInterface()
//-----------------------------------------------------------------------------
bool BusInterfaceInterface::removeBusInterface(std::string const& busName)
{
    QSharedPointer<BusInterface> removedItem = getBusInterface(busName);
    if (!removedItem)
    {
        return false;
    }

    return busInterfaces_->removeOne(removedItem);
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::busInterfaceExists()
//-----------------------------------------------------------------------------
bool BusInterfaceInterface::busInterfaceExists(std::string const& busName) const
{
    for (auto name : getItemNames())
    {
        if (name == busName)
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::swapBusInterfaces()
//-----------------------------------------------------------------------------
void BusInterfaceInterface::swapBusInterfaces(int const& firstIndex, int const& secondIndex)
{
    if (firstIndex < 0 || firstIndex >= itemCount())
    {
        return;
    }
    if (secondIndex < 0 || secondIndex >= itemCount())
    {
        QSharedPointer<BusInterface> selectedBus = busInterfaces_->takeAt(firstIndex);
        busInterfaces_->append(selectedBus);
    }
    else
    {
        busInterfaces_->swapItemsAt(firstIndex, secondIndex);
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::getAddressSpaceReference()
//-----------------------------------------------------------------------------
std::string BusInterfaceInterface::getAddressSpaceReference(std::string const& busName) const
{
    std::string spaceReference = "";
    QSharedPointer<BusInterface> selectedBus = getBusInterface(busName);
    if (selectedBus)
    {
        spaceReference = selectedBus->getAddressSpaceRef().toStdString();
    }

    return spaceReference;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::setAddressSpaceReferemce()
//-----------------------------------------------------------------------------
bool BusInterfaceInterface::setAddressSpaceReference(std::string const& busName, std::string const& newSpace)
{
    QSharedPointer<InitiatorInterface> master = createInitiatorForBusInterface(busName);
    if (!master)
    {
        return false;
    }

    master->setAddressSpaceRef(QString::fromStdString(newSpace));
    return true;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::getBaseAddressValue()
//-----------------------------------------------------------------------------
std::string BusInterfaceInterface::getBaseAddressValue(std::string const& busName, int const& baseNumber) const
{
    QSharedPointer<InitiatorInterface> selectedMaster = getInitiatorInterface(busName);
    if (selectedMaster)
    {
        QString baseAddress = selectedMaster->getBaseAddress();
        return parseExpressionToBaseNumber(baseAddress, baseNumber).toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::getBaseAddressFormattedExpression()
//-----------------------------------------------------------------------------
std::string BusInterfaceInterface::getBaseAddressFormattedExpression(std::string const& busName) const
{
    QSharedPointer<InitiatorInterface> selectedMaster = getInitiatorInterface(busName);
    if (selectedMaster)
    {
        QString baseAddress = selectedMaster->getBaseAddress();
        return formattedValueFor(baseAddress).toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::getBaseAddressExpression()
//-----------------------------------------------------------------------------
std::string BusInterfaceInterface::getBaseAddressExpression(std::string const& busName) const
{
    QSharedPointer<InitiatorInterface> selectedMaster = getInitiatorInterface(busName);
    if (selectedMaster)
    {
        return selectedMaster->getBaseAddress().toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::setBaseAddress()
//-----------------------------------------------------------------------------
bool BusInterfaceInterface::setBaseAddress(std::string const& busName, std::string const& newBaseAddress)
{
    QSharedPointer<InitiatorInterface> master = createInitiatorForBusInterface(busName);
    if (!master)
    {
        return false;
    }

    master->setBaseAddress(QString::fromStdString(newBaseAddress));
    return true;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::getMemoryMapReference()
//-----------------------------------------------------------------------------
std::string BusInterfaceInterface::getMemoryMapReference(std::string const& busName) const
{
    QSharedPointer<TargetInterface> slave = getTargetInterface(busName);
    if (slave)
    {
        return slave->getMemoryMapRef().toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::setMemoryMapReference()
//-----------------------------------------------------------------------------
bool BusInterfaceInterface::setMemoryMapReference(std::string const& busName, std::string const& newMapReference)
{
    QSharedPointer<TargetInterface> slave = createTargetForBusInterface(busName);
    if (!slave)
    {
        return false;
    }

    slave->setMemoryMapRef(QString::fromStdString(newMapReference));
    return true;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::getFileSetReferences()
//-----------------------------------------------------------------------------
std::vector<std::string> BusInterfaceInterface::getFileSetReferences(std::string const& busName) const
{
    std::vector<std::string> newFileSetItems;

    QSharedPointer<TargetInterface> slave = getTargetInterface(busName);
    if (slave && slave->getFileSetRefGroup())
    {
        for (auto fileGroup : *slave->getFileSetRefGroup())
        {
            if (fileGroup->group_.compare(DEFAULT_FILEGROUP) == 0)
            {
                for (auto fileReference : *fileGroup->fileSetRefs_)
                {
                    newFileSetItems.push_back(fileReference->getReference().toStdString());
                }
            }
        }
    }

    return newFileSetItems;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::setFileSetReferences()
//-----------------------------------------------------------------------------
bool BusInterfaceInterface::setFileSetReferences(std::string const& busName,
    std::vector<std::string>  const& newFileSetReferences)
{
    QSharedPointer<TargetInterface> slave = createTargetForBusInterface(busName);
    if (!slave)
    {
        return false;
    }

    // Convert to QString once.
    QStringList newFileSetReferencesQ;
    for (auto const& ref : newFileSetReferences)
    {
        newFileSetReferencesQ << QString::fromStdString(ref);
    }

    QSharedPointer<QList<QSharedPointer<FileSetRef> > > allExistingFileSetRefs(
        new QList<QSharedPointer<FileSetRef> >());

    // References as strings.
    QStringList allExistingReferencesStr;
    
    // Group all file set refs together.
    for (auto const& fileSetRefGroup : *slave->getFileSetRefGroup())
    {
        for (auto const& fileSetInGroup : *fileSetRefGroup->fileSetRefs_)
        {
            allExistingFileSetRefs->append(fileSetInGroup);
            allExistingReferencesStr << fileSetInGroup->getReference();
        }
    }

    std::vector<int> indexesToDelete;

    // Check for removed references.
    for (int i = 0; i < allExistingReferencesStr.size(); ++i)
    {
        auto const& currentExistingRef = allExistingReferencesStr.at(i);

        bool currentItemStillExists = false;

        for (auto const& newRef : newFileSetReferencesQ)
        {
            if (currentExistingRef == newRef)
            {
                currentItemStillExists = true;
            }
        }

        if (!currentItemStillExists)
        {
            indexesToDelete.push_back(i);
        }
    }

    // Remove deleted file set refs.
    for (auto index : indexesToDelete)
    {
        allExistingFileSetRefs->removeAt(index);
        allExistingReferencesStr.removeAt(index);
    }
    
    // Check for new refs.
    for (auto const& newRef : newFileSetReferencesQ)
    {
        if (allExistingReferencesStr.contains(newRef) == false)
        {
            QSharedPointer<FileSetRef> newFileSetRef(new FileSetRef());
            newFileSetRef->setReference(newRef);
            allExistingFileSetRefs->append(newFileSetRef);
        }
    }

    slave->getFileSetRefGroup()->clear();
    
    // Put all file set refs to same group, if there are file set references.
    if (!allExistingFileSetRefs->isEmpty())
    {
        QSharedPointer<TargetInterface::FileSetRefGroup> fileSetRefGroup(new TargetInterface::FileSetRefGroup());
        fileSetRefGroup->fileSetRefs_ = allExistingFileSetRefs;
        fileSetRefGroup->group_ = DEFAULT_FILEGROUP;
    
        slave->getFileSetRefGroup()->append(fileSetRefGroup);
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::getBusType()
//-----------------------------------------------------------------------------
ConfigurableVLNVReference BusInterfaceInterface::getBusType(std::string const& busName) const
{
    QSharedPointer<BusInterface> selectedBus = getBusInterface(busName);
    if(selectedBus)
    {
        return selectedBus->getBusType();
    }
    else
    {
        return ConfigurableVLNVReference();
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::getBusTypeString()
//-----------------------------------------------------------------------------
std::string BusInterfaceInterface::getBusTypeString(std::string const& busName) const
{
    QSharedPointer<BusInterface> selectedBus = getBusInterface(busName);
    if (selectedBus)
    {
        return selectedBus->getBusType().toString().toStdString();
    }

    return "";
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::setBustype()
//-----------------------------------------------------------------------------
bool BusInterfaceInterface::setBustype(std::string const& busName, ConfigurableVLNVReference const& newVLNV)
{
    QSharedPointer<BusInterface> selectedBus = getBusInterface(busName);
    if (!selectedBus)
    {
        return false;
    }

    selectedBus->setBusType(newVLNV);
    return true;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::setBustype()
//-----------------------------------------------------------------------------
bool BusInterfaceInterface::setBustype(std::string const& busName, std::string const& newVendor,
    std::string const& newLibrary, std::string const& newName, std::string const& newVersion) const
{
    QSharedPointer<BusInterface> selectedBus = getBusInterface(busName);
    if (!selectedBus)
    {
        return false;
    }

    ConfigurableVLNVReference newAbstractionReference;
    newAbstractionReference.setVendor(QString::fromStdString(newVendor));
    newAbstractionReference.setLibrary(QString::fromStdString(newLibrary));
    newAbstractionReference.setName(QString::fromStdString(newName));
    newAbstractionReference.setVersion(QString::fromStdString(newVersion));

    newAbstractionReference.setType(VLNV::BUSDEFINITION);

    selectedBus->setBusType(newAbstractionReference);
    return true;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::getAbstractionReferenceString()
//-----------------------------------------------------------------------------
std::string BusInterfaceInterface::getAbstractionReferenceString(std::string const& busName) const
{
    QSharedPointer<BusInterface> busInterface = getBusInterface(busName);
    if (busInterface)
    {
        General::InterfaceMode busMode = getActiveMode(busName);
        QString systemGroup = getActiveSystemGroup(busName);

        abstractionTypeInterface_->setAbstractionTypes(busInterface->getAbstractionTypes(), busMode, systemGroup);
        return abstractionTypeInterface_->getAbstractionReferenceString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::getAbstractionReferences()
//-----------------------------------------------------------------------------
std::vector<std::string> BusInterfaceInterface::getAbstractionReferences(std::string const& busName) const
{
    std::vector<std::string> references;

    QSharedPointer<BusInterface> busInterface = getBusInterface(busName);
    if (busInterface)
    {
        General::InterfaceMode busMode = getActiveMode(busName);
        QString systemGroup = getActiveSystemGroup(busName);

        abstractionTypeInterface_->setAbstractionTypes(busInterface->getAbstractionTypes(), busMode, systemGroup);
        references = abstractionTypeInterface_->getAbstractionReferences();
    }

    return references;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::addAbstractionType()
//-----------------------------------------------------------------------------
bool BusInterfaceInterface::addAbstractionType(std::string const& busName, std::string const& newVendor,
    std::string const& newLibrary, std::string const& newName, std::string const& newVersion) const
{
    QSharedPointer<BusInterface> busInterface = getBusInterface(busName);
    if (!busInterface)
    {
        return false;
    }

    General::InterfaceMode busMode = getActiveMode(busName);
    QString systemGroup = getActiveSystemGroup(busName);

    abstractionTypeInterface_->setAbstractionTypes(busInterface->getAbstractionTypes(), busMode, systemGroup);
    abstractionTypeInterface_->addAbstractionType(newVendor, newLibrary, newName, newVersion);

    return true;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::removeAbstractionTypes()
//-----------------------------------------------------------------------------
bool BusInterfaceInterface::removeAbstractionTypes(std::string const& busName)
{
    QSharedPointer<BusInterface> busInterface = getBusInterface(busName);
    if (!busInterface)
    {
        return false;
    }

    General::InterfaceMode busMode = getActiveMode(busName);
    QString systemGroup = getActiveSystemGroup(busName);

    abstractionTypeInterface_->setAbstractionTypes(busInterface->getAbstractionTypes(), busMode, systemGroup);
    for (int i = abstractionTypeInterface_->itemCount(); i >= 0; i--)
    {
        abstractionTypeInterface_->removeAbstraction(i);
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::getRangeValue()
//-----------------------------------------------------------------------------
std::string BusInterfaceInterface::getRangeValue(std::string const& busName, int const& baseNumber) const
{
    QSharedPointer<MirroredTargetInterface> selectedMirrorSlave = getMirroredSlaveInterface(busName);
    if (selectedMirrorSlave)
    {
        QString range = selectedMirrorSlave->getRange();
        return parseExpressionToBaseNumber(range, baseNumber).toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::getRangeFormattedExpression()
//-----------------------------------------------------------------------------
std::string BusInterfaceInterface::getRangeFormattedExpression(std::string const& busName) const
{
    QSharedPointer<MirroredTargetInterface> selectedMirrorSlave = getMirroredSlaveInterface(busName);
    if (selectedMirrorSlave)
    {
        QString range = selectedMirrorSlave->getRange();
        return formattedValueFor(range).toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::getRangeExpression()
//-----------------------------------------------------------------------------
std::string BusInterfaceInterface::getRangeExpression(std::string const& busName) const
{
    QSharedPointer<MirroredTargetInterface> selectedMirrorSlave = getMirroredSlaveInterface(busName);
    if (selectedMirrorSlave)
    {
        return selectedMirrorSlave->getRange().toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::setRange()
//-----------------------------------------------------------------------------
bool BusInterfaceInterface::setRange(std::string const& busName, std::string const& newRange)
{
    QSharedPointer<MirroredTargetInterface> mirrorSlave = createMirrorSlaveForBusInterface(busName);
    if (!mirrorSlave)
    {
        return false;
    }

    mirrorSlave->setRange(QString::fromStdString(newRange));
    return true;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::getRemapAddressValue()
//-----------------------------------------------------------------------------
std::string BusInterfaceInterface::getRemapAddressValue(std::string const& busName, int const& baseNumber) const
{
    QSharedPointer<MirroredTargetInterface> selectedMirrorSlave = getMirroredSlaveInterface(busName);
    if (selectedMirrorSlave && !selectedMirrorSlave->getRemapAddresses()->isEmpty())
    {
        QString remapAddress = selectedMirrorSlave->getRemapAddresses()->first()->remapAddress_;
        return parseExpressionToBaseNumber(remapAddress, baseNumber).toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::getRemapAddressFormattedExpression()
//-----------------------------------------------------------------------------
std::string BusInterfaceInterface::getRemapAddressFormattedExpression(std::string const& busName) const
{
    QSharedPointer<MirroredTargetInterface> selectedMirrorSlave = getMirroredSlaveInterface(busName);
    if (selectedMirrorSlave && !selectedMirrorSlave->getRemapAddresses()->isEmpty())
    {
        QString remapAddress = selectedMirrorSlave->getRemapAddresses()->first()->remapAddress_;
        return formattedValueFor(remapAddress).toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::getRemapAddressExpression()
//-----------------------------------------------------------------------------
std::string BusInterfaceInterface::getRemapAddressExpression(std::string const& busName) const
{
    QSharedPointer<MirroredTargetInterface> selectedMirrorSlave = getMirroredSlaveInterface(busName);
    if (selectedMirrorSlave && !selectedMirrorSlave->getRemapAddresses()->isEmpty())
    {
        return selectedMirrorSlave->getRemapAddresses()->first()->remapAddress_.toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::setRemapAddress()
//-----------------------------------------------------------------------------
bool BusInterfaceInterface::setRemapAddress(std::string const& busName, std::string const& newRemapAddress)
{
    QSharedPointer<MirroredTargetInterface> selectedMirrorSlave = createMirrorSlaveForBusInterface(busName);
    if (!selectedMirrorSlave)
    {
        return false;
    }

    selectedMirrorSlave->setRemapAddress(QString::fromStdString(newRemapAddress));
    return true;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::getMonitorMode()
//-----------------------------------------------------------------------------
General::InterfaceMode BusInterfaceInterface::getMonitorMode(std::string const& busName) const
{
    QSharedPointer<BusInterface::MonitorInterface> selectedMonitor = getMonitorInterface(busName);
    if (selectedMonitor)
    {
        return selectedMonitor->interfaceMode_;
    }

    return General::INTERFACE_MODE_COUNT;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::setMonitorMode()
//-----------------------------------------------------------------------------
bool BusInterfaceInterface::setMonitorMode(std::string const& busName, std::string const& newMode)
{
    QSharedPointer<BusInterface::MonitorInterface> selectedMonitor = createMonitorForBusInterface(busName);
    if (!selectedMonitor)
    {
        return false;
    }

    General::InterfaceMode newInterfaceMode =
        General::str2Interfacemode(QString::fromStdString(newMode), General::INTERFACE_MODE_COUNT);
    selectedMonitor->interfaceMode_ = newInterfaceMode;

    abstractionTypeInterface_->setBusMode(newInterfaceMode);

    return true;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::getMonitorGroup()
//-----------------------------------------------------------------------------
std::string BusInterfaceInterface::getMonitorGroup(std::string const& busName) const
{
    QSharedPointer<BusInterface::MonitorInterface> selectedMonitor = getMonitorInterface(busName);
    if (selectedMonitor)
    {
        return selectedMonitor->group_.toStdString();
    }

    return "";
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::setMonitorGroup()
//-----------------------------------------------------------------------------
bool BusInterfaceInterface::setMonitorGroup(std::string const& busName, std::string const& newGroup)
{
    QSharedPointer<BusInterface::MonitorInterface> selectedMonitor = createMonitorForBusInterface(busName);
    if (!selectedMonitor)
    {
        return false;
    }

    QString newGroupQ = QString::fromStdString(newGroup);

    selectedMonitor->group_ = newGroupQ;

    abstractionTypeInterface_->setSystemGroup(newGroupQ);

    return true;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::getBridgeInterface()
//-----------------------------------------------------------------------------
TransparentBridgeInterface* BusInterfaceInterface::getBridgeInterface()
{
    return bridgeInterface_;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::getFileSetInterface()
//-----------------------------------------------------------------------------
FileSetInterface* BusInterfaceInterface::getFileSetInterface() const
{
    return fileSetInterface_;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::getMemoryMapInterface()
//-----------------------------------------------------------------------------
MemoryMapInterface* BusInterfaceInterface::getMemoryMapInterface() const
{
    return mapInterface_;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::getAbstractionTypeInterface()
//-----------------------------------------------------------------------------
AbstractionTypeInterface* BusInterfaceInterface::getAbstractionTypeInterface() const
{
    return abstractionTypeInterface_;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::getValidator()
//-----------------------------------------------------------------------------
QSharedPointer<BusInterfaceValidator> BusInterfaceInterface::getValidator() const
{
    return busValidator_;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::getBridges()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<TransparentBridge> > > BusInterfaceInterface::getBridges(
    std::string const& busName) const
{
    QSharedPointer<TargetInterface> slave = getTargetInterface(busName);
    if (slave)
    {
        return slave->getBridges();
    }

    return QSharedPointer<QList<QSharedPointer<TransparentBridge> > >();
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::createBridges()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<TransparentBridge> > > BusInterfaceInterface::createBridges(
    std::string const& busName)
{
    QSharedPointer<TargetInterface> slave = createTargetForBusInterface(busName);
    if (slave)
    {
        return slave->getBridges();
    }

    return QSharedPointer<QList<QSharedPointer<TransparentBridge> > >();
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::getAllReferencesToIdInItem()
//-----------------------------------------------------------------------------
int BusInterfaceInterface::getAllReferencesToIdInItem(const std::string& itemName, std::string const& valueID) const
{
    int totalReferencesToParameter = 0;

    QSharedPointer<BusInterface> selectedBus = getBusInterface(itemName);
    if (selectedBus)
    {
        QString idString = QString::fromStdString(valueID);

        QSharedPointer<InitiatorInterface> master = getInitiatorInterface(itemName);
        if (master)
        {
            totalReferencesToParameter += QString::fromStdString(getBaseAddressExpression(itemName)).count(idString);
        }


        QSharedPointer<TargetInterface> slave = getTargetInterface(itemName);
        if (slave)
        {
            totalReferencesToParameter += QString::fromStdString(getRangeExpression(itemName)).count(idString);
            totalReferencesToParameter += QString::fromStdString(getRemapAddressExpression(itemName)).count(idString);

            QSharedPointer<QList<QSharedPointer<TransparentBridge> > > bridges = slave->getBridges();
            if (bridges)
            {
                bridgeInterface_->setBridges(bridges);
                for (auto bridge : *bridges)
                {
                    totalReferencesToParameter +=
                        bridgeInterface_->getAllReferencesToIdInItem(bridge->getMasterRef().toStdString(), valueID);
                }
            }
        }

        parameterInterface_->setParameters(selectedBus->getParameters());
        for (auto parameterName : parameterInterface_->getItemNames())
        {
            totalReferencesToParameter += parameterInterface_->getAllReferencesToIdInItem(parameterName, valueID);
        }
    }

    return totalReferencesToParameter;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::getAllExpressions()
//-----------------------------------------------------------------------------
std::vector<std::string> BusInterfaceInterface::getAllExpressions(std::string const& busName) const
{
    std::vector<std::string> expressionList;

    QSharedPointer<BusInterface> selectedBus = getBusInterface(busName);
    if (selectedBus)
    {
        QString baseAddress = QString::fromStdString(getBaseAddressExpression(busName));

        if (!baseAddress.isEmpty())
        {
            expressionList.push_back(baseAddress.toStdString());
        }

        QSharedPointer<TargetInterface> slave = getTargetInterface(busName);
        if (slave)
        {
            QString range = QString::fromStdString(getRangeExpression(busName));
            QString remapAddress = QString::fromStdString(getRemapAddressExpression(busName));
            if (!range.isEmpty())
            {
                expressionList.push_back(range.toStdString());
            }
            if (!remapAddress.isEmpty())
            {
                expressionList.push_back(remapAddress.toStdString());
            }

            QSharedPointer<QList<QSharedPointer<TransparentBridge> > > bridges = slave->getBridges();
            if (bridges)
            {
                bridgeInterface_->setBridges(bridges);
                for (auto bridge : *bridges)
                {
                    for (auto bridgeExpression :
                        bridgeInterface_->getAllExpressions(bridge->getMasterRef().toStdString()))
                    {
                        expressionList.push_back(bridgeExpression);
                    }
                }
            }
        }

        parameterInterface_->setParameters(selectedBus->getParameters());
        for (auto expression :
            parameterInterface_->getExpressionsInSelectedItems(parameterInterface_->getItemNames()))
        {
            expressionList.push_back(expression);
        }
    }

    return expressionList;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::copyRows()
//-----------------------------------------------------------------------------
void BusInterfaceInterface::copyRows(std::vector<int> selectedRows)
{
    QList<QSharedPointer<BusInterface> > copiedBuses;
    for (auto index : selectedRows)
    {
        QSharedPointer<BusInterface> currentBus = busInterfaces_->at(index);
        copiedBuses.append(currentBus);
    }

    QVariant registerVariant;
    registerVariant.setValue(copiedBuses);

    QMimeData* newMimeData = new QMimeData();
    newMimeData->setData("text/xml/ipxact:register", QByteArray());
    newMimeData->setImageData(registerVariant);

    QApplication::clipboard()->setMimeData(newMimeData);
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::pasteRows()
//-----------------------------------------------------------------------------
std::vector<std::string> BusInterfaceInterface::pasteRows()
{
    std::vector<std::string> pastedBusNames;

    const QMimeData* pasteData = QApplication::clipboard()->mimeData();

    if (pasteData->hasImage())
    {
        QVariant pasteVariant = pasteData->imageData();
        if (pasteVariant.canConvert<QList<QSharedPointer<BusInterface> > >())
        {
            QList<QSharedPointer<BusInterface> > copiedBusList =
                pasteVariant.value<QList<QSharedPointer<BusInterface>>>();

            for (auto copiedBus : copiedBusList)
            {
                QSharedPointer<BusInterface> newBus(new BusInterface(*copiedBus.data()));
                newBus->setName(getUniqueName(newBus->name().toStdString(), DEFAULT_NAME.toStdString()));
                busInterfaces_->append(newBus);

                pastedBusNames.push_back(newBus->name().toStdString());
            }
        }
    }

    return pastedBusNames;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceInterface::getPasteRowCount()
//-----------------------------------------------------------------------------
int BusInterfaceInterface::getPasteRowCount() const
{
    const QMimeData* pasteData = QApplication::clipboard()->mimeData();
    if (pasteData->hasImage())
    {
        QVariant pasteVariant = pasteData->imageData();
        if (pasteVariant.canConvert<QList<QSharedPointer<BusInterface> > >())
        {
            QList<QSharedPointer<BusInterface> > newBuses =
                pasteVariant.value<QList<QSharedPointer<BusInterface> > >();
            return newBuses.count();
        }
    }

    return 0;
}
