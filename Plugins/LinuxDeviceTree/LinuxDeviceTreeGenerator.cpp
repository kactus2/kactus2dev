//-----------------------------------------------------------------------------
// File: LinuxDeviceTreePlugin.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Copyright: Tampere University of Technology
// Author: Mikko Teuho
// Date: 08.05.2018
//
// Description:
// Creates a Linux Device Tree from the selected document.
//-----------------------------------------------------------------------------

#include "LinuxDeviceTreeGenerator.h"

#include <IPXACTmodels/common/Document.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Cpu.h>
#include <IPXACTmodels/Component/AddressSpace.h>
#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/Component/AddressBlock.h>
#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/DesignConfiguration/DesignConfiguration.h>

#include <KactusAPI/include/LibraryInterface.h>

#include <editors/MemoryDesigner/MemoryItem.h>
#include <editors/MemoryDesigner/ConnectivityGraph.h>
#include <editors/MemoryDesigner/MasterSlavePathSearch.h>
#include <editors/MemoryDesigner/ConnectivityInterface.h>
#include <editors/MemoryDesigner/ConnectivityComponent.h>

#include <Plugins/LinuxDeviceTree/CPUSelection/LinuxDeviceTreeCpuRoutesContainer.h>

#include <QTextStream>

namespace
{
    QString const TABPREFIX = QLatin1String("\t");
};

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::LinuxDeviceTreeGenerator()
//-----------------------------------------------------------------------------
LinuxDeviceTreeGenerator::LinuxDeviceTreeGenerator(LibraryInterface* library):
library_(library),
graphFactory_(library)
{

}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::generate()
//-----------------------------------------------------------------------------
bool LinuxDeviceTreeGenerator::generate(QSharedPointer<Component> topComponent, QString const& activeView,
    bool writeAddressBlocks, QVector<QSharedPointer<LinuxDeviceTreeCpuRoutesContainer> > cpuContainers,
    QString const& outputFolder)
{
    QSharedPointer<const Design> hierarchicalDesign;
    QSharedPointer<View> designView = getView(topComponent, activeView);
    if (designView)
    {
        QSharedPointer<const DesignConfiguration> hierarchicalConfiguration =
            getDesignConfiguration(topComponent, designView);
        hierarchicalDesign = getHierarchicalDesign(topComponent, designView, hierarchicalConfiguration);
    }

    if (!hierarchicalDesign)
    {
        return false;
    }

    QString designVendor = hierarchicalDesign->getVlnv().getVendor();
    QString designName = hierarchicalDesign->getVlnv().getName();

    for (auto cpuContainer : cpuContainers)
    {
        writeFile(designVendor, designName, cpuContainer, writeAddressBlocks, outputFolder);
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::getDesignConfiguration()
//-----------------------------------------------------------------------------
QSharedPointer<const DesignConfiguration> LinuxDeviceTreeGenerator::getDesignConfiguration(
    QSharedPointer<Component const> containingComponent, QSharedPointer<View> referencingView) const
{
    QString configurationReference = referencingView->getDesignConfigurationInstantiationRef();
    if (!configurationReference.isEmpty())
    {
        for (auto instantiation : *containingComponent->getDesignConfigurationInstantiations())
        {
            if (instantiation->name().compare(configurationReference) == 0)
            {
               auto configurationVLNV = instantiation->getDesignConfigurationReference();

                return library_->getModelReadOnly<const DesignConfiguration>(*configurationVLNV);
            }
        }
    }

    return QSharedPointer<const DesignConfiguration>();
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::getDesignInstantiation()
//-----------------------------------------------------------------------------
QSharedPointer<const Design> LinuxDeviceTreeGenerator::getHierarchicalDesign(
    QSharedPointer<Component const> containingComponent, QSharedPointer<View> referencingView,
    QSharedPointer<const DesignConfiguration> designConfiguration) const
{
    VLNV designVLNV = getHierarchicalDesignVLNV(containingComponent, referencingView);

    if (!designVLNV.isValid() && !designConfiguration.isNull())
    {
        designVLNV = designConfiguration->getDesignRef();
    }

    if (designVLNV.isValid())
    {
        return library_->getModelReadOnly<const Design>(designVLNV);
    }

    return QSharedPointer<const Design>();
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::getHierarchicalDesignVLNV()
//-----------------------------------------------------------------------------
VLNV LinuxDeviceTreeGenerator::getHierarchicalDesignVLNV(QSharedPointer<Component const> containingComponent,
    QSharedPointer<View> referencingView) const
{
    QString referencedInstantiation = referencingView->getDesignInstantiationRef();

    if (!referencedInstantiation.isEmpty())
    {
        for (QSharedPointer<DesignInstantiation> instantiation : *containingComponent->getDesignInstantiations())
        {
            if (instantiation->name().compare(referencedInstantiation) == 0)
            {
                return VLNV(*instantiation->getDesignReference());
            }
        }
    }

    return VLNV();
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::writeFile()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeGenerator::writeFile(QString const& designVendor, QString const& designName,
    QSharedPointer<LinuxDeviceTreeCpuRoutesContainer> cpuContainer, bool writeAddressBlocks,
    QString const& outputFolder)
{
    QString outputPath = outputFolder + "/" + cpuContainer->getFileName() + ".dts";
    cpuContainer->setFilePath(outputPath);

    QFile outputFile(outputPath);
    if (!outputFile.open(QIODevice::WriteOnly))
    {
        return;
    }

    QString prefix = TABPREFIX;
    QTextStream outputStream(&outputFile);

    QSharedPointer<DeviceTreeUtilities::CpuDeviceTree> rootContainer(new DeviceTreeUtilities::CpuDeviceTree());
    rootContainer->interface_ = QSharedPointer<const ConnectivityInterface>(new ConnectivityInterface("Root"));
    rootContainer->childContainers_ = cpuContainer->getDeviceTreeRoutes();

    QPair<quint64, quint64> addressSizeRequirements =
        getAddressAndSizeRequirements(rootContainer, QSharedPointer<ConnectivityInterface const>(), 0, 0, false);

    auto addressWordSize = calculateWordSize(addressSizeRequirements.first);
    auto rangeWordSize = calculateWordSize(addressSizeRequirements.second);

    writeTreeStart(outputStream, designVendor, designName, prefix, addressWordSize, rangeWordSize);
    writeContainer(outputStream, cpuContainer, writeAddressBlocks, prefix, addressWordSize, rangeWordSize);

    prefix.remove(0, 1);
    writeLineEnding(outputStream, prefix);
    outputStream << Qt::endl;

    outputFile.close();
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::writeTreeStart()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeGenerator::writeTreeStart(QTextStream& outputStream, QString const& designVendor,
    QString const& designName, QString const& prefix, int addressSize, int rangeSize) const
{
    outputStream << "/dts-v1/;" << Qt::endl << Qt::endl << "/ {" << Qt::endl;

    outputStream << prefix << "#address-cells = <" << QString::number(addressSize) << ">;" <<
        Qt::endl;
    outputStream << prefix << "#size-cells = <" << QString::number(rangeSize) << ">;" << Qt::endl;

    QString compatibaleText = "\"" + designVendor + "," + designName + "\";";
    outputStream << prefix << "model = " << compatibaleText << Qt::endl;
    outputStream << prefix << "compatible = " << compatibaleText << Qt::endl << Qt::endl;
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::writePaths()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeGenerator::writeContainer(QTextStream& outputStream,
    QSharedPointer<LinuxDeviceTreeCpuRoutesContainer> cpuContainer, bool writeAddressBlocks, QString& prefix,
    int addressSize, int rangeSize)
{
    if (cpuContainer->getDeviceTreeRoutes().isEmpty() || !cpuContainer->getDeviceTreeRoutes().first()->interface_)
    {
        return;
    }

    QSharedPointer<const ConnectivityInterface> firstInterface = cpuContainer->getDeviceTreeRoutes().first()->interface_;
    QString componentVLNV = firstInterface->getInstance()->getVlnv();

    writeIntroductionToCPUs(outputStream, cpuContainer->getCpus(), componentVLNV, prefix);

    prefix.remove(0, 1);
    writeLineEnding(outputStream, prefix);
    outputStream << Qt::endl;

    for (auto deviceTreeContainer : cpuContainer->getDeviceTreeRoutes())
    {
        startPathWriting(outputStream, deviceTreeContainer, writeAddressBlocks, prefix, addressSize, rangeSize);
    }

    writeMemories(outputStream, cpuContainer, prefix, writeAddressBlocks, addressSize, rangeSize);
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::writeIntroductionCPUs()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeGenerator::writeIntroductionToCPUs(QTextStream& outputStream,
    QVector<QSharedPointer<const Cpu>> cpus, QString const& componentVLNV, QString& prefix)
{
    outputStream << prefix << "cpus {" << Qt::endl;

    prefix.append(TABPREFIX);

    outputStream << prefix << "#address-cells = <1>;" << Qt::endl;
    outputStream << prefix << "#size-cells = <0>;" << Qt::endl << Qt::endl;

    for (int i = 0; i < cpus.size(); ++i)
    {
        QSharedPointer<Cpu const> currentCPU = cpus.at(i);
        writeCPU(outputStream, currentCPU->name(), componentVLNV, i, prefix);
    }
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::writeCPU()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeGenerator::writeCPU(QTextStream& outputStream, QString const& CPUName,
    QString const& componentVLNV, int cpuNumber, QString& prefix)
{
    outputStream << prefix << "// '" << CPUName << "' in component " << componentVLNV << Qt::endl;
    outputStream << prefix << "cpu@" << QString::number(cpuNumber) << " {" << Qt::endl;
    prefix.append(TABPREFIX);

    outputStream << prefix << "reg = <" << QString::number(cpuNumber) << ">;" << Qt::endl;

    prefix.remove(0, 1);
    writeLineEnding(outputStream, prefix);
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::startPathWriting()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeGenerator::startPathWriting(QTextStream& outputStream,
    QSharedPointer<DeviceTreeUtilities::CpuDeviceTree> nodeContainer,
    bool writeAddressBlocks, QString& prefix, int addressSize, int rangeSize)
{
    quint64 memoryItemBaseAddress = 0;
    quint64 memoryItemRange = 0;
    writePathNode(outputStream, QSharedPointer<ConnectivityInterface>(), nodeContainer, memoryItemBaseAddress,
        memoryItemRange, prefix, writeAddressBlocks, addressSize, rangeSize);
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::writePathNode()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeGenerator::writePathNode(QTextStream& outputStream, QSharedPointer<const ConnectivityInterface> previousInterface,
    QSharedPointer<DeviceTreeUtilities::CpuDeviceTree> nodeContainer, quint64 const& baseAddress, quint64 const& memoryItemRange,
    QString& prefix, bool writeAddressBlocks, int addressSize, int rangeSize)
{
    QSharedPointer<const ConnectivityInterface> containerInterface = nodeContainer->interface_;
    if (!canWriteNode(containerInterface, previousInterface))
    {
        return;
    }

    quint64 newBaseAddress = baseAddress;
    quint64 newMemoryRange = memoryItemRange;

    if (containerInterface->getInstance()->isChanneled() &&
        (!previousInterface || containerInterface->getInstance() != previousInterface->getInstance()))
    {
        writeBridge(outputStream, containerInterface, "channel", addressSize, rangeSize, prefix);
    }

    if (containerInterface->getMode().compare(QLatin1String("master"), Qt::CaseInsensitive) == 0)
    {
        newBaseAddress += containerInterface->getBaseAddress().toULongLong();
    }
    else if (containerInterface->getMode().compare(QLatin1String("mirroredSlave"), Qt::CaseInsensitive) == 0)
    {
        newBaseAddress += containerInterface->getRemapAddress().toULongLong();
        newMemoryRange = containerInterface->getRemapRange().toULongLong();
    }
    else if (containerInterface->getMode().compare(QLatin1String("slave"), Qt::CaseInsensitive) == 0)
    {
        if (containerInterface->isBridged())
        {
            //! Override current address-cells and size-cells.
            QPair<quint64, quint64> addressSizeRequirements = getAddressAndSizeRequirements(
                nodeContainer, QSharedPointer<const ConnectivityInterface>(), newBaseAddress, newMemoryRange, true);

            addressSize = calculateWordSize(addressSizeRequirements.first);
            rangeSize = calculateWordSize(addressSizeRequirements.second);

            writeBridge(outputStream, containerInterface, "bridge", addressSize, rangeSize, prefix);
        }
        else if (containerInterface->getConnectedMemory())
        {
            QPair<quint64, quint64> memoryAddress =
                MemoryConnectionAddressCalculator::getMemoryMapAddressRanges(containerInterface->getConnectedMemory());

            quint64 mapBaseAddress = newBaseAddress + memoryAddress.first;
            if (newMemoryRange == 0)
            {
                newMemoryRange = memoryAddress.second + 1;
            }

            writeMemoryData(outputStream, newBaseAddress, mapBaseAddress,
                newMemoryRange, addressSize, rangeSize, false, prefix, containerInterface, writeAddressBlocks);

            outputStream << Qt::endl;
            return;
        }
    }

    for (auto leafContainer : nodeContainer->childContainers_)
    {
        writePathNode(outputStream, containerInterface, leafContainer, newBaseAddress,
            newMemoryRange, prefix, writeAddressBlocks, addressSize, rangeSize);
    }

    if ((containerInterface->getMode().compare(QLatin1String("slave"), Qt::CaseInsensitive) == 0 &&
        containerInterface->isBridged()) ||
        (containerInterface->getInstance()->isChanneled() &&
            (!previousInterface || containerInterface->getInstance() != previousInterface->getInstance())))
    {
        prefix.remove(0, 1);
        writeLineEnding(outputStream, prefix);
        outputStream << Qt::endl;
    }
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::canWriteNode()
//-----------------------------------------------------------------------------
bool LinuxDeviceTreeGenerator::canWriteNode(QSharedPointer<const ConnectivityInterface> interfaceNode,
    QSharedPointer<const ConnectivityInterface> previousNode) const
{
    if (LinuxDeviceTreeCPUDetails::interfacedItemIsMemory(interfaceNode))
    {
        return false;
    }
    else if (interfaceNode->getChildInterfaceNodes().isEmpty())
    {
        return true;
    }

    for (auto childNode : interfaceNode->getChildInterfaceNodes())
    {
        if (childNode != previousNode && canWriteNode(childNode, interfaceNode))
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::writeLineEnding()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeGenerator::writeLineEnding(QTextStream& outputStream, QString const& prefix) const
{
    outputStream << prefix << "};" << Qt::endl;
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::writeBridge()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeGenerator::writeBridge(QTextStream& outputStream,
    QSharedPointer<ConnectivityInterface const> interfaceNode, QString const& bridgeType,
    int addressSize, int rangeSize, QString& prefix)
{
    QString instanceName = interfaceNode->getInstance()->getName();
    QString componentVLNV = interfaceNode->getInstance()->getVlnv();
    QSharedPointer<const Component> interfacedComponent =
        LinuxDeviceTreeCPUDetails::getComponentContainingInterface(interfaceNode, library_);

    outputStream << prefix << "// Instance '" << instanceName << "' of " << bridgeType << " component " <<
        componentVLNV << Qt::endl;
    outputStream << prefix << interfacedComponent->getVlnv().getName() << " {" << Qt::endl;

    prefix.append(TABPREFIX);

    outputStream << prefix << "compatible = \"simple-bus\";" << Qt::endl;
    outputStream << prefix << "#address-cells = <" << QString::number(addressSize) << ">;" << Qt::endl;
    outputStream << prefix << "#size-cells = <" << QString::number(rangeSize) << ">;" << Qt::endl << Qt::endl;
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::getAddressAndSizeRequirements()
//-----------------------------------------------------------------------------
QPair<quint64, quint64> LinuxDeviceTreeGenerator::getAddressAndSizeRequirements(
    QSharedPointer<DeviceTreeUtilities::CpuDeviceTree> rootContainer,
    QSharedPointer<ConnectivityInterface const> previousNode, quint64 baseAddress, quint64 memoryRange, bool stopAtBridges)
{
    QPair<quint64, quint64> addressSizeRequirements;
    addressSizeRequirements.first = baseAddress;
    addressSizeRequirements.second = memoryRange;

    QSharedPointer<const ConnectivityInterface> containerInterface = rootContainer->interface_;

    if (!canWriteNode(containerInterface, previousNode))
    {
        return addressSizeRequirements;
    }

    if (stopAtBridges && (containerInterface->getInstance()->isChanneled() && previousNode &&
        containerInterface->getInstance() != previousNode->getInstance()))
    {
        return addressSizeRequirements;
    }

    quint64 newBaseAddress = baseAddress;
    quint64 newRange = memoryRange;

    if (containerInterface->getMode().compare(QLatin1String("master"), Qt::CaseInsensitive) == 0)
    {
        newBaseAddress += containerInterface->getBaseAddress().toULongLong();
    }
    else if (containerInterface->getMode().compare(QLatin1String("mirroredSlave"), Qt::CaseInsensitive) == 0)
    {
        newBaseAddress += containerInterface->getRemapAddress().toULongLong();
        newRange = containerInterface->getRemapRange().toULongLong();
    }
    else if (containerInterface->getMode().compare(QLatin1String("slave"), Qt::CaseInsensitive) == 0)
    {
        if (containerInterface->isBridged())
        {
            return addressSizeRequirements;
        }
        else if (containerInterface->getConnectedMemory())
        {
            QPair<quint64, quint64> memoryAddress =
                MemoryConnectionAddressCalculator::getMemoryMapAddressRanges(containerInterface->getConnectedMemory());

            if (newRange == 0)
            {
                newRange = memoryAddress.second + 1;
            }

            addressSizeRequirements.first += memoryAddress.first;
            addressSizeRequirements.second = newRange;

            return addressSizeRequirements;
        }
    }

    for (auto pathContainer : rootContainer->childContainers_)
    {
        QPair<quint64, quint64> newAddressSize =
            getAddressAndSizeRequirements(pathContainer, containerInterface, newBaseAddress, newRange, stopAtBridges);

        if (newAddressSize.first > addressSizeRequirements.first)
        {
            addressSizeRequirements.first = newAddressSize.first;
        }
        if (newAddressSize.second > addressSizeRequirements.second)
        {
            addressSizeRequirements.second = newAddressSize.second;
        }
    }

    return addressSizeRequirements;
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::calculateWordSize()
//-----------------------------------------------------------------------------
unsigned int LinuxDeviceTreeGenerator::calculateWordSize(quint64 const& requirementValue) const
{
    QString requirement = QString::number(requirementValue, 2);
    int wordSize = requirement.size() / 32;

    if (requirement.size() > wordSize * 32)
    {
        wordSize += 1;
    }

    return wordSize;
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::writeMemoryData()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeGenerator::writeMemoryData(QTextStream& outputStream, 
    quint64 const& itemBaseAddress,
    quint64 const& mapBaseAddress, quint64 const& range,
    int addressSize, int rangeSize, bool isMemory, QString& prefix,
    QSharedPointer<ConnectivityInterface const> memoryNode, bool writeAddressBlocks) const
{
    QString instanceName = memoryNode->getInstance()->getName();
    QString componentVLNV = memoryNode->getInstance()->getVlnv();
    QString mapName = memoryNode->getConnectedMemory()->getName();

    QString writtenName = mapName;

    if (isMemory)
    {
        writtenName = QLatin1String("memory");
    }

    outputStream << prefix << "// Memory map '" << mapName << "' in instance '" << instanceName <<
        "' of component " << componentVLNV << Qt::endl;
    outputStream << prefix << writtenName << "@" << QString::number(mapBaseAddress, 16) << " {" << Qt::endl;
    prefix.append(TABPREFIX);

    if (isMemory)
    {
        outputStream << prefix << QLatin1String("device_type = \"memory\";") << Qt::endl;
    }

    outputStream << prefix << "#address-cells = <" << QString::number(addressSize) << ">;" <<
        Qt::endl;
    outputStream << prefix << "#size-cells = <" << QString::number(rangeSize) << ">;" << Qt::endl;

    writeRegister(outputStream, mapBaseAddress, range, addressSize, rangeSize, prefix);

    if (writeAddressBlocks && !isMemory)
    {
        writeAddressBlocksData(outputStream, memoryNode, itemBaseAddress, addressSize, rangeSize, prefix);
    }

    prefix.remove(0, 1);
    writeLineEnding(outputStream, prefix);
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::writeAddressBlocksData()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeGenerator::writeAddressBlocksData(QTextStream& outputStream,
    QSharedPointer<ConnectivityInterface const> memoryNode, quint64 const& itemBaseAddress, 
    int addressSize, int rangeSize, QString& prefix) const
{
    for (auto childItem : memoryNode->getConnectedMemory()->getChildItems())
    {
        if (childItem->getType() == "addressBlock")
        {
            QString vendor;
            QStringList vlnvList = memoryNode->getInstance()->getVlnv().split(":");
            if (vlnvList.isEmpty() == false)
            {
                vendor = vlnvList.first();
            }

            writeSingleAddressBlock(outputStream, vendor, childItem, itemBaseAddress, addressSize, rangeSize, 
                prefix);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::writeSingleAddressBlock()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeGenerator::writeSingleAddressBlock(QTextStream& outputStream,
    QString const& vendorName, QSharedPointer<MemoryItem> blockItem,
    quint64 const& itemBaseAddress, int addressSize, int rangeSize, QString& prefix) const
{
    QString blockName = blockItem->getName();
    quint64 address = blockItem->getAddress().toULongLong() + itemBaseAddress;
    QString addressString = QString::number(address, 16);

    QString status = getStatus(blockItem);

    outputStream << Qt::endl;
    outputStream << prefix << "// Address block '" << blockName << "'" << Qt::endl;
    outputStream << prefix << blockItem->getName() << "@" << addressString << " {" <<  Qt::endl;
    prefix.append(TABPREFIX);

    outputStream << prefix << "compatible = \"" << vendorName << "," << blockName << "\";" << Qt::endl;
    outputStream << prefix << "status = \"" << status << "\";" << Qt::endl;

    General::Usage blockUsage = blockItem->getUsage();
    if (blockUsage == General::REGISTER)
    {
        writeRegister(outputStream, address, blockItem->getRange().toULongLong(), addressSize, rangeSize, prefix);
    }
    else if (blockUsage == General::MEMORY)
    {
        outputStream << prefix << "device_type = \"memory\";" << Qt::endl;
    }

    prefix.remove(0, 1);
    writeLineEnding(outputStream, prefix);
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::getStatus()
//-----------------------------------------------------------------------------
QString LinuxDeviceTreeGenerator::getStatus(QSharedPointer<MemoryItem> blockItem) const
{
    QString blockStatus = "disabled";
    bool canConvertPresence = false;
    int blockPresence = blockItem->getIsPresent().toInt(&canConvertPresence);
    if (canConvertPresence && blockPresence != 0)
    {
        blockStatus = "okay";
    }

    return blockStatus;
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::writeRegister()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeGenerator::writeRegister(QTextStream& output, quint64 const& address, quint64 const& range,
    int addressSize, int rangeSize, QString const& prefix) const
{
    QString formattedAddress = formatValue(address, addressSize);
    QString formattedRange = formatValue(range, rangeSize);

    output << prefix << "reg = <" << formattedAddress << " " << formattedRange << ">;" << Qt::endl;
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::formatValue()
//-----------------------------------------------------------------------------
QString LinuxDeviceTreeGenerator::formatValue(quint64 const& value, int wordSize) const
{
    if (wordSize == 0)
    {
        return QString();
    }

    QString hexValue = QString::number(value, 16);
    auto length = hexValue.length();

    auto leftmostHexCount = length % 8;

    QStringList hexWords;
    if (leftmostHexCount != 0)
    {
        hexWords.append(hexValue.left(leftmostHexCount));
    }

    for (auto i = leftmostHexCount; i < length; i = i + 8)
    {
        hexWords.append(hexValue.mid(i, 8));
    }

    while (hexWords.size() < wordSize)
    {
        hexWords.prepend("0");
    }

    Q_ASSERT(hexWords.size() <= wordSize);

    return hexWords.join(" 0x").prepend("0x");
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::writeMemories()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeGenerator::writeMemories(QTextStream& outputStream,
    QSharedPointer<LinuxDeviceTreeCpuRoutesContainer> cpuContainer, QString& prefix,
    bool writeAddressBlocks,int addressSize, int rangeSize)
    const
{
    for (auto rootContainer : cpuContainer->getDeviceTreeRoutes())
    {
        QVector<LinuxDeviceTreeCPUDetails::CpuMemory> containerMemories = LinuxDeviceTreeCPUDetails::getMemories(rootContainer, 0, 0);
        for (auto const& memory : containerMemories)
        {
            QString mapName = memory.memoryInterface_->getConnectedMemory()->getName();
            QString instanceName = memory.memoryInterface_->getInstance()->getName();
            QString componentVLNV = memory.memoryInterface_->getInstance()->getVlnv();

            writeMemoryData(outputStream, memory.itemBaseAddress_,
                memory.mapBaseAddress_, memory.range_,
                addressSize, rangeSize, true, prefix, memory.memoryInterface_, writeAddressBlocks);

            outputStream << Qt::endl;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::getView()
//-----------------------------------------------------------------------------
QSharedPointer<View> LinuxDeviceTreeGenerator::getView(QSharedPointer<Component const> containingComponent,
    QString const& viewName) const
{
    for (QSharedPointer<View> view : *containingComponent->getViews())
    {
        if (view->name() == viewName)
        {
            return view;
        }
    }

    return QSharedPointer<View>();
}
