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
#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>

#include <library/LibraryInterface.h>

#include <editors/MemoryDesigner/MemoryItem.h>
#include <editors/MemoryDesigner/ConnectivityGraph.h>
#include <editors/MemoryDesigner/MasterSlavePathSearch.h>
#include <editors/MemoryDesigner/ConnectivityInterface.h>
#include <editors/MemoryDesigner/ConnectivityComponent.h>

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
// Function: LinuxDeviceTreeGenerator::~LinuxDeviceTreeGenerator()
//-----------------------------------------------------------------------------
LinuxDeviceTreeGenerator::~LinuxDeviceTreeGenerator()
{

}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::generate()
//-----------------------------------------------------------------------------
bool LinuxDeviceTreeGenerator::generate(QSharedPointer<Component> topComponent, QString const& activeView,
    bool writeAddressBlocks, QVector<QSharedPointer<LinuxDeviceTreeCPUDetails::CPUContainer>> cpuContainers,
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
        foreach(QSharedPointer<DesignConfigurationInstantiation> instantiation,
            *containingComponent->getDesignConfigurationInstantiations())
        {
            if (instantiation->name().compare(configurationReference) == 0)
            {
                QSharedPointer<ConfigurableVLNVReference> configurationVLNV =
                    instantiation->getDesignConfigurationReference();

                QSharedPointer<const Document> configurationDocument =
                    library_->getModelReadOnly(*configurationVLNV);

                return configurationDocument.dynamicCast<const DesignConfiguration>();
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
        return library_->getModelReadOnly(designVLNV).dynamicCast<const Design>();
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
        foreach(QSharedPointer<DesignInstantiation> instantiation, *containingComponent->getDesignInstantiations())
        {
            if (instantiation->name().compare(referencedInstantiation) == 0)
            {
                return *instantiation->getDesignReference();
            }
        }
    }

    return VLNV();
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::writeFile()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeGenerator::writeFile(QString const& designVendor, QString const& designName,
    QSharedPointer<LinuxDeviceTreeCPUDetails::CPUContainer> cpuContainer, bool writeAddressBlocks,
    QString const& outputFolder)
{
    QString outputPath = outputFolder + "/" + cpuContainer->fileName_ + ".dts";
    cpuContainer->filePath_ = outputPath;

    QFile outputFile(outputPath);
    if (!outputFile.open(QIODevice::WriteOnly))
    {
        return;
    }

    QTextStream outputStream(&outputFile);

    QVector<QSharedPointer<const Cpu> > connectedCPUs;

    QString prefix = TABPREFIX;

    writeTreeStart(outputStream, designVendor, designName, cpuContainer->rootInterfaces_, prefix);
    writeContainer(outputStream, cpuContainer, writeAddressBlocks, prefix);

    prefix.remove(0, 1);
    writeLineEnding(outputStream, prefix);

    outputFile.close();
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::writeTreeStart()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeGenerator::writeTreeStart(QTextStream& outputStream, QString const& designVendor,
    QString const& designName, QVector<QSharedPointer<ConnectivityInterface>> masterRoots, QString& prefix)
{
    outputStream << "/dts-v1/;" << endl << endl << "/ {" << endl;

    QSharedPointer<ConnectivityInterface> rootNode(new ConnectivityInterface("Root"));
    for (auto masterNode : masterRoots)
    {
        rootNode->addChildInterfaceNode(masterNode);
    }

    QPair<quint64, quint64> addressSizeRequirements =
        getAddressAndSizeRequirements(rootNode, QSharedPointer<ConnectivityInterface const>(), 0, 0, false);

    int addressRequirementBits = calculateRequiredBits(addressSizeRequirements.first);
    int sizeRequirementBits = calculateRequiredBits(addressSizeRequirements.second);

    outputStream << prefix << "#address-cells = <0x" << QString::number(addressRequirementBits, 16) << ">;" <<
        endl;
    outputStream << prefix << "#size-cells = <0x" << QString::number(sizeRequirementBits, 16) << ">;" << endl;

    QString compatibaleText = "\"" + designVendor + "," + designName + "\";";
    outputStream << prefix << "model = " << compatibaleText << endl;
    outputStream << prefix << "compatible = " << compatibaleText << endl << endl;
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::writePaths()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeGenerator::writeContainer(QTextStream& outputStream,
    QSharedPointer<LinuxDeviceTreeCPUDetails::CPUContainer> cpuContainer, bool writeAddressBlocks, QString& prefix)
{
    QString componentVLNV = cpuContainer->rootInterfaces_.first()->getInstance()->getVlnv();

    writeIntroductionToCPUs(outputStream, cpuContainer->interfacedCPUs_, componentVLNV, prefix);

    prefix.remove(0, 1);
    writeLineEnding(outputStream, prefix);
    outputStream << endl;

    for (auto rootNode : cpuContainer->rootInterfaces_)
    {
        startPathWriting(outputStream, rootNode, cpuContainer, writeAddressBlocks, prefix);
    }

    writeMemories(outputStream, cpuContainer, prefix, writeAddressBlocks);
}


//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::writeIntroductionCPUs()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeGenerator::writeIntroductionToCPUs(QTextStream& outputStream,
    QVector<QSharedPointer<const Cpu>> cpus, QString const& componentVLNV, QString& prefix)
{
    outputStream << prefix << "cpus {" << endl;

    prefix.append(TABPREFIX);

    outputStream << prefix << "#address-cells = <0x1>;" << endl;
    outputStream << prefix << "#size-cells = <0x1>;" << endl << endl;

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
    outputStream << prefix << "// '" << CPUName << "' in component " << componentVLNV << endl;
    outputStream << prefix << "cpu@" << QString::number(cpuNumber) << " {" << endl;
    prefix.append(TABPREFIX);

    outputStream << prefix << "reg = <" << QString::number(cpuNumber) << ">;" << endl;

    prefix.remove(0, 1);
    writeLineEnding(outputStream, prefix);
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::startPathWriting()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeGenerator::startPathWriting(QTextStream& outputStream,
    QSharedPointer<const ConnectivityInterface> pathRootNode,
    QSharedPointer<LinuxDeviceTreeCPUDetails::CPUContainer> cpuContainer, bool writeAddressBlocks, QString& prefix)
{
    quint64 memoryItemBaseAddress = 0;
    quint64 memoryItemRange = 0;
    writePathNode(outputStream, QSharedPointer<ConnectivityInterface>(), pathRootNode, memoryItemBaseAddress,
        memoryItemRange, prefix, writeAddressBlocks);
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::writePathNode()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeGenerator::writePathNode(QTextStream& outputStream,
    QSharedPointer<const ConnectivityInterface> previousInterface,
    QSharedPointer<const ConnectivityInterface> interfaceNode, quint64 const& baseAddress,
    quint64 const& memoryItemRange, QString& prefix, bool writeAddressBlocks)
{
    if (!canWriteNode(interfaceNode, previousInterface))
    {
        return;
    }

    quint64 newBaseAddress = baseAddress;
    quint64 newMemoryRange = memoryItemRange;

    if (interfaceNode->getInstance()->isChanneled() &&
        (!previousInterface || interfaceNode->getInstance() != previousInterface->getInstance()))
    {
        writeBridge(outputStream, interfaceNode, "channel", newBaseAddress, newMemoryRange, prefix);
    }

    if (interfaceNode->getMode().compare(QLatin1String("master"), Qt::CaseInsensitive) == 0)
    {
        newBaseAddress += interfaceNode->getBaseAddress().toULongLong();
    }
    else if (interfaceNode->getMode().compare(QLatin1String("mirroredSlave"), Qt::CaseInsensitive) == 0)
    {
        newBaseAddress += interfaceNode->getRemapAddress().toULongLong();
        newMemoryRange = interfaceNode->getRemapRange().toULongLong();
    }
    else if (interfaceNode->getMode().compare(QLatin1String("slave"), Qt::CaseInsensitive) == 0)
    {
        if (interfaceNode->isBridged())
        {
            writeBridge(outputStream, interfaceNode, "bridge", newBaseAddress, newMemoryRange, prefix);
        }
        else if (interfaceNode->getConnectedMemory())
        {
            QPair<quint64, quint64> memoryAddress =
                MemoryConnectionAddressCalculator::getMemoryMapAddressRanges(
                    interfaceNode->getConnectedMemory());

            quint64 mapBaseAddress = newBaseAddress + memoryAddress.first;
            if (newMemoryRange == 0)
            {
                newMemoryRange = memoryAddress.second + 1;
            }

            writeMemoryMapItem(outputStream, interfaceNode, newBaseAddress, mapBaseAddress, newMemoryRange,
                prefix, writeAddressBlocks);
            outputStream << endl;

            return;
        }
    }

    for (auto pathNode : interfaceNode->getChildInterfaceNodes())
    {
        if (pathNode != previousInterface)
        {
            writePathNode(
                outputStream, interfaceNode, pathNode, newBaseAddress, newMemoryRange, prefix, writeAddressBlocks);

            if (pathNode->getMode().compare(QLatin1String("slave"), Qt::CaseInsensitive) == 0 &&
                pathNode->isBridged())
            {
                outputStream << endl;
            }
        }
    }

    if ((interfaceNode->getMode().compare(QLatin1String("slave"), Qt::CaseInsensitive) == 0 &&
        interfaceNode->isBridged()) ||
        (interfaceNode->getInstance()->isChanneled() &&
        (!previousInterface || interfaceNode->getInstance() != previousInterface->getInstance())))
    {
        prefix.remove(0, 1);
        writeLineEnding(outputStream, prefix);
        outputStream << endl;
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
    outputStream << prefix << "};" << endl;
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::writeBridge()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeGenerator::writeBridge(QTextStream& outputStream,
    QSharedPointer<ConnectivityInterface const> interfaceNode, QString const& bridgeType, quint64 baseAddress,
    quint64 memoryRange, QString& prefix)
{
    QString instanceName = interfaceNode->getInstance()->getName();
    QString componentVLNV = interfaceNode->getInstance()->getVlnv();
    QSharedPointer<const Component> interfacedComponent =
        LinuxDeviceTreeCPUDetails::getComponentContainingInterface(interfaceNode, library_);

    outputStream << prefix << "// Instance '" << instanceName << "' of " << bridgeType << " component " <<
        componentVLNV << endl;
    outputStream << prefix << interfacedComponent->getVlnv().getName() << " {" << endl;

    prefix.append(TABPREFIX);

    QPair<quint64, quint64> addressSizeRequirements = getAddressAndSizeRequirements(
        interfaceNode, QSharedPointer<ConnectivityInterface const>(), baseAddress, memoryRange, true);

    int addressRequirementBits = calculateRequiredBits(addressSizeRequirements.first);
    int sizeRequirementBits = calculateRequiredBits(addressSizeRequirements.second);

    outputStream << prefix << "compatible = \"simple-bus\";" << endl;
    outputStream << prefix << "#address-cells = <0x" << QString::number(addressRequirementBits, 16) << ">;" << endl;
    outputStream << prefix << "#size-cells = <0x" << QString::number(sizeRequirementBits, 16) << ">;" << endl << endl;
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::getAddressAndSizeRequirements()
//-----------------------------------------------------------------------------
QPair<quint64, quint64> LinuxDeviceTreeGenerator::getAddressAndSizeRequirements(
    QSharedPointer<ConnectivityInterface const> itemNode, QSharedPointer<ConnectivityInterface const> previousNode,
    quint64 baseAddress, quint64 memoryRange, bool stopAtBridges)
{
    QPair<quint64, quint64> addressSizeRequirements;
    addressSizeRequirements.first = baseAddress;
    addressSizeRequirements.second = memoryRange;

    if (!canWriteNode(itemNode, previousNode))
    {
        return addressSizeRequirements;
    }

    quint64 newBaseAddress = baseAddress;
    quint64 newRange = memoryRange;

    if (stopAtBridges && (itemNode->getInstance()->isChanneled() && previousNode &&
        itemNode->getInstance() != previousNode->getInstance()))
    {
        return addressSizeRequirements;
    }

    if (itemNode->getMode().compare(QLatin1String("master"), Qt::CaseInsensitive) == 0)
    {
        newBaseAddress += itemNode->getBaseAddress().toULongLong();
    }
    else if (itemNode->getMode().compare(QLatin1String("mirroredSlave"), Qt::CaseInsensitive) == 0)
    {
        newBaseAddress += itemNode->getRemapAddress().toULongLong();
        newRange = itemNode->getRemapRange().toULongLong();
    }
    else if (itemNode->getMode().compare(QLatin1String("slave"), Qt::CaseInsensitive) == 0)
    {
        if (itemNode->isBridged())
        {
            return addressSizeRequirements;
        }
        else if (itemNode->getConnectedMemory())
        {
            QPair<quint64, quint64> memoryAddress =
                MemoryConnectionAddressCalculator::getMemoryMapAddressRanges(itemNode->getConnectedMemory());

            if (newRange == 0)
            {
                newRange = memoryAddress.second + 1;
            }

            addressSizeRequirements.first += memoryAddress.first;
            addressSizeRequirements.second = newRange;

            return addressSizeRequirements;
        }
    }

    for (auto pathNode : itemNode->getChildInterfaceNodes())
    {
        if (pathNode != previousNode)
        {
            QPair<quint64, quint64> newAddressSize =
                getAddressAndSizeRequirements(pathNode, itemNode, newBaseAddress, newRange, stopAtBridges);

            if (newAddressSize.first > addressSizeRequirements.first)
            {
                addressSizeRequirements.first = newAddressSize.first;
            }
            if (newAddressSize.second > addressSizeRequirements.second)
            {
                addressSizeRequirements.second = newAddressSize.second;
            }
        }
    }

    return addressSizeRequirements;
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::calculateRequiredBits()
//-----------------------------------------------------------------------------
int LinuxDeviceTreeGenerator::calculateRequiredBits(quint64 const& requirementValue) const
{
    QString requirement = QString::number(requirementValue, 2);
    int requirementBits = requirement.size() / 32;

    if (requirement.size() > requirementBits * 32)
    {
        requirementBits += 1;
    }

    return requirementBits;
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::writeMemoryMapItem()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeGenerator::writeMemoryMapItem(QTextStream& outputStream,
    QSharedPointer<ConnectivityInterface const> interfaceNode, quint64 itemBaseAddress, quint64 mapBaseAddress,
    quint64 range, QString& prefix, bool writeAddressBlocks)
{
    QString instanceName = interfaceNode->getInstance()->getName();
    QString componentVLNV = interfaceNode->getInstance()->getVlnv();
    QString memoryItemName = interfaceNode->getConnectedMemory()->getName();

    writeMemoryData(outputStream, memoryItemName, instanceName, componentVLNV, itemBaseAddress, mapBaseAddress,
        range, false, prefix, interfaceNode, writeAddressBlocks);
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::writeMapComment()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeGenerator::writeMemoryData(QTextStream& outputStream, QString const& itemName,
    QString const& instanceName, QString const& componentVLNV, quint64 const& itemBaseAddress,
    quint64 const& mapBaseAddress, quint64 const& range, bool isMemory, QString& prefix,
    QSharedPointer<ConnectivityInterface const> memoryNode, bool writeAddressBlocks) const
{
    outputStream << prefix << "// Memory map '" << itemName << "' in instance '" << instanceName <<
        "' of component " << componentVLNV << endl;

    QString writtenName = itemName;

    if (isMemory)
    {
        writtenName = QLatin1String("memory");
    }
    outputStream << prefix << writtenName << "@" << QString::number(mapBaseAddress, 16).toUpper() << " {" << endl;
    prefix.append(TABPREFIX);

    if (isMemory)
    {
        outputStream << prefix << QLatin1String("device_type = \"memory\";") << endl;
    }

    outputStream << prefix << "reg = <0x" << QString::number(mapBaseAddress, 16).toUpper() << " 0x" <<
        QString::number(range, 16).toUpper() << ">;" << endl;

    if (writeAddressBlocks && !isMemory)
    {
        writeAddressBlocksData(outputStream, memoryNode, itemBaseAddress, prefix);
    }

    prefix.remove(0, 1);
    writeLineEnding(outputStream, prefix);
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::writeAddressBlocksData()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeGenerator::writeAddressBlocksData(QTextStream& outputStream,
    QSharedPointer<ConnectivityInterface const> memoryNode, quint64 const& itemBaseAddress, QString& prefix) const
{
    for (auto childItem : memoryNode->getConnectedMemory()->getChildItems())
    {
        if (childItem->getType() == "addressBlock")
        {
            writeSingleAddressBlock(outputStream, memoryNode, childItem, itemBaseAddress, prefix);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::writeSingleAddressBlock()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeGenerator::writeSingleAddressBlock(QTextStream& outputStream,
    QSharedPointer<ConnectivityInterface const> containingMemoryNode, QSharedPointer<MemoryItem> blockItem,
    quint64 const& itemBaseAddress, QString& prefix) const
{
    QString blockName = blockItem->getName();

    outputStream << endl;
    outputStream << prefix << "// Address block '" << blockName << "'" << endl;

    quint64 blockAddress = blockItem->getAddress().toULongLong() + itemBaseAddress;
    QString blockAddressString = QString::number(blockAddress, 16);

    outputStream << prefix << blockItem->getName() << "@" << blockAddressString << " {" <<  endl;
    prefix.append(TABPREFIX);

    QStringList vlnvList = containingMemoryNode->getInstance()->getVlnv().split(":");
    QString vendor = "";
    if (vlnvList.size() > 0)
    {
        vendor = vlnvList.first();
    }

    QString blockStatus = "disabled";
    bool canConvertPresence = false;
    int blockPresence = blockItem->getIsPresent().toInt(&canConvertPresence);
    if (canConvertPresence & blockPresence != 0)
    {
        blockStatus = "okay";
    }

    outputStream << prefix << "compatible = \"" << vendor << "," << blockName << "\";" << endl;
    outputStream << prefix << "status = \"" << blockStatus << "\";" << endl;

    General::Usage blockUsage = blockItem->getUsage();
    if (blockUsage == General::REGISTER)
    {
        quint64 blockRange = blockItem->getRange().toULongLong();
        QString blockRangeString = QString::number(blockRange, 16);

        outputStream << prefix << "reg = <0x" << blockAddressString << " 0x" << blockRangeString << ">;" << endl;
    }
    else if (blockUsage == General::MEMORY)
    {
        outputStream << prefix << "device_type = \"memory\"" << endl;
    }

    prefix.remove(0, 1);
    writeLineEnding(outputStream, prefix);
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::writeMemories()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeGenerator::writeMemories(QTextStream& outputStream,
    QSharedPointer<LinuxDeviceTreeCPUDetails::CPUContainer> cpuContainer, QString& prefix, bool writeAddressBlocks)
    const
{
    for (auto rootNode : cpuContainer->rootInterfaces_)
    {
        QVector<LinuxDeviceTreeCPUDetails::CpuMemory> containerMemories =
            LinuxDeviceTreeCPUDetails::getMemories(QSharedPointer<const ConnectivityInterface>(), rootNode, 0, 0);
        for (auto memory : containerMemories)
        {
            QString mapName = memory.memoryInterface_->getConnectedMemory()->getName();
            QString instanceName = memory.memoryInterface_->getInstance()->getName();
            QString componentVLNV = memory.memoryInterface_->getInstance()->getVlnv();

            writeMemoryData(outputStream, mapName, instanceName, componentVLNV, memory.itemBaseAddress_,
                memory.mapBaseAddress_, memory.range_, true, prefix, memory.memoryInterface_, writeAddressBlocks);

            outputStream << endl;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::getView()
//-----------------------------------------------------------------------------
QSharedPointer<View> LinuxDeviceTreeGenerator::getView(QSharedPointer<Component const> containingComponent,
    QString const& viewName) const
{
    foreach(QSharedPointer<View> view, *containingComponent->getViews())
    {
        if (view->name() == viewName)
        {
            return view;
        }
    }

    return QSharedPointer<View>();
}
