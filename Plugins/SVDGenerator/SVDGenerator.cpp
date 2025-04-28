//-----------------------------------------------------------------------------
// File: SVDGenerator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 14.04.2021
//
// Description:
// Creates a CMSIS System View Description listing.
//-----------------------------------------------------------------------------

#include "SVDGenerator.h"

#include <KactusAPI/include/LibraryInterface.h>

#include <KactusAPI/include/SystemVerilogExpressionParser.h>
#include <KactusAPI/include/IPluginUtility.h>

#include <editors/MemoryDesigner/ConnectivityConnection.h>
#include <editors/MemoryDesigner/ConnectivityGraph.h>
#include <editors/MemoryDesigner/ConnectivityGraphFactory.h>
#include <editors/MemoryDesigner/ConnectivityInterface.h>
#include <editors/MemoryDesigner/MasterSlavePathSearch.h>
#include <editors/MemoryDesigner/MemoryItem.h>
#include <editors/MemoryDesigner/MemoryDesignerConstants.h>
#include <editors/MemoryDesigner/MemoryConnectionAddressCalculator.h>

#include <Plugins/SVDGenerator/CPUDialog/SVDCpuRoutesContainer.h>
#include <Plugins/SVDGenerator/CPUDialog/SVDUtilities.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/Component/MemoryBlockBase.h>
#include <IPXACTmodels/Component/AddressBlock.h>
#include <IPXACTmodels/Component/RegisterBase.h>
#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/Field.h>
#include <IPXACTmodels/Component/EnumeratedValue.h>
#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/DesignConfiguration/DesignConfiguration.h>

#include <QXmlStreamWriter>
#include <QRegularExpression>

//-----------------------------------------------------------------------------
// Function: SVDGenerator::SVDGenerator()
//-----------------------------------------------------------------------------
SVDGenerator::SVDGenerator(IPluginUtility* utility):
utility_(utility),
library_(utility->getLibraryInterface()),
graphFactory_(utility->getLibraryInterface())
{

}

//-----------------------------------------------------------------------------
// Function: SVDGenerator::generate()
//-----------------------------------------------------------------------------
void SVDGenerator::generate(QSharedPointer<Component> topComponent, QString const& componentPath, QVector<QSharedPointer<SVDCpuRoutesContainer>> const& cpuRoutes)
{
    QStringList fileNames;
    for (auto cpuMasterRoute : cpuRoutes)
    {
        if (cpuMasterRoute->getRoutes().empty() == false)
        {
            writeFile(topComponent, componentPath, cpuMasterRoute, fileNames);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: SVDGenerator::getGeneratedFiles()
//-----------------------------------------------------------------------------
QStringList SVDGenerator::getGeneratedFiles() const
{
    return generatedFiles_;
}

//-----------------------------------------------------------------------------
// Function: SVDGenerator::writeFile()
//-----------------------------------------------------------------------------
void SVDGenerator::writeFile(QSharedPointer<Component> topComponent, QString const& componentPath,
    QSharedPointer<SVDCpuRoutesContainer> cpuDetails, QStringList& fileNames)
{
    QVector<QSharedPointer<CpuRouteStructs::CpuRoute> > cpuRoutes = cpuDetails->getRoutes();
    if (cpuRoutes.isEmpty())
    {
        return;
    }

    QSharedPointer<CpuRouteStructs::CpuRoute> firstCpuRouteDetails = cpuRoutes.first();

    QSharedPointer<const ConnectivityComponent> routeComponent = firstCpuRouteDetails->cpuInterface_->getInstance();
    QSharedPointer<const Component> interfaceComponent =
        ConnectivityGraphUtilities::getInterfacedComponent(library_, routeComponent);
    if (interfaceComponent)
    {
        QSharedPointer<Cpu> interfaceCPU = cpuDetails->getCpu();
        QString fileName = topComponent->getVlnv().getName() + "_" + interfaceCPU->name();
        if (fileNames.contains(interfaceCPU->name()))
        {
            fileName = fileName + "_" + QString::number(getFileNumberExtension(fileNames, fileName));
        }
        
        QString svdFilePath = componentPath + "/" + fileName;
        fileNames.append(fileName);
        
        svdFilePath += "." + SVDConstants::SVDFILETYPE;
        
        QFile outputFile(svdFilePath);
        if (!outputFile.open(QIODevice::WriteOnly))
        {
            return;
        }

        QXmlStreamWriter xmlWriter(&outputFile);
        xmlWriter.setAutoFormatting(true);
        xmlWriter.setAutoFormattingIndent(-1);

        xmlWriter.writeStartDocument();

        writeDevice(xmlWriter, topComponent);
        writeCPU(xmlWriter, interfaceCPU, cpuDetails);

        writeAddressSpaceData(xmlWriter, firstCpuRouteDetails->cpuInterface_);

        checkForErrorsInAddressSpaceData(fileName, firstCpuRouteDetails->cpuInterface_, cpuRoutes);

        writePeripherals(xmlWriter, cpuRoutes);

        xmlWriter.writeEndElement();    //! device

        generatedFiles_.append(svdFilePath);
    }
}

//-----------------------------------------------------------------------------
// Function: SVDGenerator::checkForErrorsInAddressSpaceData()
//-----------------------------------------------------------------------------
void SVDGenerator::checkForErrorsInAddressSpaceData(QString const& fileName, QSharedPointer<const ConnectivityInterface> comparisonInterface,
    QVector<QSharedPointer<CpuRouteStructs::CpuRoute>> cpuRoutes)
{
    QString comparisonAUB = comparisonInterface->getConnectedMemory()->getAUB();
    QString comparisonWidth = comparisonInterface->getConnectedMemory()->getWidth();
    QString comparisonSize = comparisonInterface->getConnectedMemory()->getSize();
    QString comparisonBase = comparisonInterface->getBaseAddress();

    for (auto currentRoute : cpuRoutes)
    {
        QSharedPointer<const ConnectivityInterface> currentInterface = currentRoute->cpuInterface_;

        QString currentAUB = currentInterface->getConnectedMemory()->getAUB();
        if (currentAUB != comparisonAUB)
        {
            utility_->printError("Mis-matching address unit bits found in " + fileName + " interface " + currentInterface->getName());
        }
        QString currentWidth = currentInterface->getConnectedMemory()->getWidth();
        if (currentWidth != comparisonWidth)
        {
            utility_->printError("Mis-matching width found in " + fileName + " interface " + currentInterface->getName());
        }
        QString currentSize = currentInterface->getConnectedMemory()->getSize();
        if (currentSize != comparisonSize)
        {
            utility_->printError("Mis-matching size found in " + fileName + " interface " + currentInterface->getName());
        }
        QString currentBase = currentInterface->getBaseAddress();
        if (currentBase != comparisonBase)
        {
            utility_->printError("Mis-matching base address found in " + fileName + " interface " + currentInterface->getName());
        }
    }
}

//-----------------------------------------------------------------------------
// Function: SVDGenerator::getFileNumberExtension()
//-----------------------------------------------------------------------------
int SVDGenerator::getFileNumberExtension(QStringList const& fileNames, QString const& fileName) const
{
    int extensionNumber = -1;
    for (auto file : fileNames)
    {
        if (file.contains(fileName))
        {
            extensionNumber++;
        }
    }

    return extensionNumber;
}

//-----------------------------------------------------------------------------
// Function: SVDGenerator::writeDevice()
//-----------------------------------------------------------------------------
void SVDGenerator::writeDevice(QXmlStreamWriter& writer, QSharedPointer<Component> topComponent)
{
    writer.writeStartElement(QLatin1String("device"));

    writer.writeAttribute(QLatin1String("schemaVersion"), QLatin1String("1.1"));
    writer.writeAttribute(QLatin1String("xmlns:xs"), QLatin1String("http://www.w3.org/2001/XMLSchema-instance"));
    writer.writeAttribute(QLatin1String("xs:noNamespaceSchemaLocation"), QLatin1String("CMSIS-SVD.xsd"));

    VLNV topVLNV = topComponent->getVlnv();
    writer.writeTextElement(SVDConstants::VENDOR, topVLNV.getVendor());
    writer.writeTextElement(SVDConstants::NAME, formatName(topVLNV.getName()));
    writer.writeTextElement(SVDConstants::VERSION, topVLNV.getVersion());
    writer.writeTextElement(SVDConstants::DESCRIPTION, topComponent->getDescription());
}

//-----------------------------------------------------------------------------
// Function: SVDGenerator::formatName()
//-----------------------------------------------------------------------------
QString SVDGenerator::formatName(QString const& name) const
{
    QString formattedName = name;
    formattedName = formattedName.trimmed();
    formattedName = formattedName.replace(" ", "_");
    return formattedName;
}

//-----------------------------------------------------------------------------
// Function: SVDGenerator::writeCPU()
//-----------------------------------------------------------------------------
void SVDGenerator::writeCPU(QXmlStreamWriter& writer, QSharedPointer<Cpu> currentCPU, QSharedPointer<SVDCpuRoutesContainer> cpuContainer)
{
    writer.writeStartElement(SVDConstants::CPUELEMENT);

    QString cpuName = formatName(currentCPU->name());
    QStringList knownNames({ "CM0", "CM0PLUS", "CM0+", "CM1", "SC000",
        "CM23", "CM3", "CM33", "CM35P", "SC300", "CM4", "CM7",
        "CA5", "CA7", "CA8", "CA9", "CA15", "CA17", "CA53", "CA57", "CA72"
        });

    if (knownNames.contains(cpuName) == false)
    {
        cpuName = SVDConstants::OTHERTYPE;
    }

    writer.writeTextElement(SVDConstants::NAME, cpuName);
    writer.writeTextElement(SVDConstants::CPUREVISION , cpuContainer->getRevision());
    writer.writeTextElement(SVDConstants::CPUENDIAN, cpuContainer->getEndian());
    writeBoolean(writer, SVDConstants::CPUMPUPRESENT, cpuContainer->isMPUPresent());
    writeBoolean(writer, SVDConstants::CPUFPUPRESENT, cpuContainer->isFPUPresent());
    writer.writeTextElement(SVDConstants::CPPUNVICPRIOBITS, cpuContainer->getNVICPrioBits());
    writeBoolean(writer, SVDConstants::CPUVENDORSYSTICKCONFIG, cpuContainer->isVendorSystickConfig());

    writer.writeEndElement(); //! cpu
}

//-----------------------------------------------------------------------------
// Function: SVDGenerator::writeBoolean()
//-----------------------------------------------------------------------------
void SVDGenerator::writeBoolean(QXmlStreamWriter& writer, QString const& elementName, bool state)
{
    if (state == true)
    {
        writer.writeTextElement(elementName, SVDConstants::BOOLEANTRUE);
    }
    else
    {
        writer.writeTextElement(elementName, SVDConstants::BOOLEANFALSE);
    }
}

//-----------------------------------------------------------------------------
// Function: SVDGenerator::writeAddressSpaceData()
//-----------------------------------------------------------------------------
void SVDGenerator::writeAddressSpaceData(QXmlStreamWriter& writer,
    QSharedPointer<const ConnectivityInterface> cpuInterface)
{
    writer.writeTextElement(SVDConstants::AUB, cpuInterface->getConnectedMemory()->getAUB());
    writer.writeTextElement(SVDConstants::WIDTH, cpuInterface->getConnectedMemory()->getWidth());
    writer.writeTextElement(SVDConstants::SIZE, cpuInterface->getConnectedMemory()->getWidth());
}

//-----------------------------------------------------------------------------
// Function: SVDGenerator::writePeripherals()
//-----------------------------------------------------------------------------
void SVDGenerator::writePeripherals(QXmlStreamWriter& writer, QVector<QSharedPointer<CpuRouteStructs::CpuRoute>> cpuRouteDetails)
{
    writer.writeStartElement(SVDConstants::PERIPHERALSELEMENT);

    for (auto singleRouteDetails : cpuRouteDetails)
    {
        for (auto& masterSlaveRoute : singleRouteDetails->routes_)
        {
            for (int i = 1; i < masterSlaveRoute.size(); ++i)
            {
                QSharedPointer<const ConnectivityInterface> routeInterface = masterSlaveRoute.at(i);
                QSharedPointer<const ConnectivityComponent> interfacedComponent = routeInterface->getInstance();
                QSharedPointer<MemoryItem> interfaceMemory = routeInterface->getConnectedMemory();

                if (interfaceMemory && interfaceMemory->getType().compare(
                    MemoryDesignerConstants::MEMORYMAP_TYPE, Qt::CaseInsensitive) == 0)
                {
                    QSharedPointer<const Component> component =
                        ConnectivityGraphUtilities::getInterfacedComponent(library_, interfacedComponent);

                    MemoryConnectionAddressCalculator::CalculatedPathAddresses pathAddresses =
                        MemoryConnectionAddressCalculator::calculatePathAddresses(singleRouteDetails->cpuInterface_, routeInterface, masterSlaveRoute);

                    quint64 memoryBaseAddress = pathAddresses.connectionBaseAddress_;
                    QString baseAddressInHexa = valueToHexa(memoryBaseAddress);

                    writePeripheral(writer, component, interfaceMemory, memoryBaseAddress, baseAddressInHexa);
                }
            }
        }
    }

    writer.writeEndElement(); //! peripherals
}

//-----------------------------------------------------------------------------
// Function: SVDGenerator::writePeripheral()
//-----------------------------------------------------------------------------
void SVDGenerator::writePeripheral(QXmlStreamWriter& writer, QSharedPointer<const Component> component,
    QSharedPointer<MemoryItem> mapItem, quint64 mapBaseAddress, QString const& mapBaseAddressInHexa)
{
    QSharedPointer<MemoryMap> memoryMap = getMemoryMap(mapItem, component);
    if (!memoryMap)
    {
        return;
    }

    writer.writeStartElement(SVDConstants::SINGLEPERIPHERALELEMENT);

    writer.writeTextElement(SVDConstants::NAME, formatName(memoryMap->name()));
    writer.writeTextElement(SVDConstants::PERIPHERALVERSION, component->getVlnv().getVersion());
    writeOptionalElement(writer, SVDConstants::DESCRIPTION, memoryMap->description());

    writer.writeTextElement(SVDConstants::PERIPHERALBASEADDRESS, mapBaseAddressInHexa);
    writeAddressBlocks(writer, mapItem, mapBaseAddress);
    writeRegisters(writer, component, mapItem, mapBaseAddress);
    writer.writeEndElement(); //! peripheral
}

//-----------------------------------------------------------------------------
// Function: SVDGenerator::getMemoryMap()
//-----------------------------------------------------------------------------
QSharedPointer<MemoryMap> SVDGenerator::getMemoryMap(QSharedPointer<MemoryItem> interfaceMemory,
    QSharedPointer<const Component> containingComponent) const
{
    if (interfaceMemory)
    {
        for (auto map : *containingComponent->getMemoryMaps())
        {
            if (map->name() == interfaceMemory->getName())
            {
                return map;
            }
        }
    }

    return QSharedPointer<MemoryMap>();
}

//-----------------------------------------------------------------------------
// Function: SVDGenerator::writeAddressBlock()
//-----------------------------------------------------------------------------
void SVDGenerator::writeAddressBlocks(QXmlStreamWriter& writer,
    QSharedPointer<MemoryItem> mapItem,
    quint64 mapBaseAddress)
{
    for (auto const& blockItem : getAddressBlockItems(mapItem))
    {
        quint64 blockBaseAddress = blockItem->getAddress().toULongLong();
        if (blockBaseAddress >= mapBaseAddress)
        {
            blockBaseAddress = blockBaseAddress - mapBaseAddress;
        }

        writeSingleAddressBlock(writer, blockBaseAddress, blockItem);
    }
}

//-----------------------------------------------------------------------------
// Function: SVDGenerator::getAddressBlockItems()
//-----------------------------------------------------------------------------
QVector<QSharedPointer<MemoryItem> > SVDGenerator::getAddressBlockItems(QSharedPointer<MemoryItem> mapItem) const
{
    QVector<QSharedPointer<MemoryItem> > blockItems;

    for (auto subItem : getSubMemoryItems(mapItem, MemoryDesignerConstants::ADDRESSBLOCK_TYPE))
    {
        if (subItem->getUsage() != General::MEMORY)
        {
            blockItems.append(subItem);
        }
    }

    return blockItems;
}

//-----------------------------------------------------------------------------
// Function: SVDGenerator::getBlockMemoryItem()
//-----------------------------------------------------------------------------
QVector<QSharedPointer<MemoryItem>> SVDGenerator::getSubMemoryItems(QSharedPointer<MemoryItem> memoryItem,
    QString const& subItemType) const
{
    QVector<QSharedPointer<MemoryItem>> subItems;

    for (auto subItem : memoryItem->getChildItems())
    {
        if (subItem->getType().compare(subItemType, Qt::CaseInsensitive) == 0)
        {
            subItems.append(subItem);
        }
    }

    return subItems;
}

//-----------------------------------------------------------------------------
// Function: SVDGenerator::writeSingleAddressBlock()
//-----------------------------------------------------------------------------
void SVDGenerator::writeSingleAddressBlock(QXmlStreamWriter& writer, quint64 const& offset,
    QSharedPointer<MemoryItem> blockItem)
{
    writer.writeStartElement(SVDConstants::ADDRESSBLOCKELEMENT);

    QString addressOffsetInHexa = valueToHexa(offset);
    QString rangeInHexa = valueToHexa(blockItem->getRange().toULongLong());

    writer.writeTextElement(SVDConstants::ADDRESSBLOCKOFFSET, addressOffsetInHexa);
    writer.writeTextElement(SVDConstants::SIZE, rangeInHexa);

    QString usageString = SVDConstants::BUFFERUSAGE;
    General::Usage blockUsage = blockItem->getUsage();
    if (blockUsage == General::REGISTER || blockUsage == General::USAGE_COUNT)
    {
        usageString = SVDConstants::REGISTERSUSAGE;
    }
    if (blockUsage == General::RESERVED)
    {
        usageString = SVDConstants::RESERVEDUSAGE;
    }

    writer.writeTextElement(SVDConstants::USAGE, usageString);

    writer.writeEndElement(); //! addressBlock
}

//-----------------------------------------------------------------------------
// Function: SVDGenerator::writeRegisters()
//-----------------------------------------------------------------------------
void SVDGenerator::writeRegisters(QXmlStreamWriter& writer, QSharedPointer<const Component> containingComponent,
    QSharedPointer<MemoryItem> mapItem, quint64 mapBaseAddress)
{
    writer.writeStartElement(SVDConstants::REGISTERSELEMENT);

    for (auto blockItem : getAddressBlockItems(mapItem))
    {
        QSharedPointer<AddressBlock> containingBlock = getAddressBlock(containingComponent, mapItem, blockItem);
        if (!containingBlock)
        {
            continue;
        }

        quint64 blockBaseAddress = blockItem->getAddress().toULongLong();
        if (blockBaseAddress >= mapBaseAddress)
        {
            blockBaseAddress = blockBaseAddress - mapBaseAddress;
        }

        QString addressOffsetInHexa = valueToHexa(blockBaseAddress);

        writeRegisterCluster(writer, containingBlock, blockItem, addressOffsetInHexa);
    }

    writer.writeEndElement(); // registers
}

//-----------------------------------------------------------------------------
// Function: SVDGenerator::writeRegisterCluster()
//-----------------------------------------------------------------------------
void SVDGenerator::writeRegisterCluster(QXmlStreamWriter& writer, QSharedPointer<AddressBlock> containingBlock,
    QSharedPointer<MemoryItem> blockItem, QString const& addressOffsetInHexa)
{
    QVector<QSharedPointer<MemoryItem> > registerItems =
        getSubMemoryItems(blockItem, MemoryDesignerConstants::REGISTER_TYPE);
    if (registerItems.isEmpty())
    {
        return;
    }

    writer.writeStartElement(SVDConstants::CLUSTER);

    writer.writeTextElement(SVDConstants::NAME, blockItem->getName());
    writer.writeTextElement(SVDConstants::ADDRESSOFFSET, addressOffsetInHexa);

    for (auto registerItem : registerItems)
    {
        QSharedPointer<Register> realRegister = getRegister(containingBlock, registerItem);
        if (!realRegister)
        {
            continue;
        }

        writeRegister(writer, registerItem, realRegister);
    }

    writer.writeEndElement(); //! cluster
}

//-----------------------------------------------------------------------------
// Function: SVDGenerator::writeRegister()
//-----------------------------------------------------------------------------
void SVDGenerator::writeRegister(QXmlStreamWriter& writer, QSharedPointer<MemoryItem> registerItem,
    QSharedPointer<Register> realRegister)
{
    // Skip all but first register in dimension array.
    if (registerItem->getDimension().isEmpty() == false && registerItem->getIdentifier().endsWith("[0]") == false)
    {
        return;
    }

    QString name = registerItem->getName();
    quint64 registerOffset = registerItem->getOffset().toULongLong();
    QString addressOffsetInHexa = valueToHexa(registerOffset);
    QString sizeString = registerItem->getSize();

    writer.writeStartElement(SVDConstants::REGISTERELEMENT);

    if (registerItem->getDimension().isEmpty() == false)
    {
        writer.writeTextElement(SVDConstants::DIM, registerItem->getDimension());
        writer.writeTextElement(SVDConstants::DIMINCREMENT, QString::number(sizeString.toInt() / 8));

        name.append("[%s]");
    }

    writer.writeTextElement(SVDConstants::NAME, formatName(name));
    writeOptionalElement(writer, SVDConstants::DESCRIPTION, realRegister->description());

    writer.writeTextElement(SVDConstants::ADDRESSOFFSET, addressOffsetInHexa);

    QSharedPointer<MemoryItem> resetItem = getResetItem(registerItem);
    if (resetItem)
    {
        writeReset(writer, resetItem);
    }

    writer.writeTextElement(SVDConstants::SIZE, sizeString);
    writeOptionalElement(writer, SVDConstants::ACCESS, AccessTypes::access2Str(realRegister->getAccess()));

    QMap<quint64, QSharedPointer<MemoryItem> > fieldItems = getFieldItemsInOrder(registerItem, registerOffset);
    writeFields(writer, realRegister, fieldItems);

    writer.writeEndElement(); //! register
}

//-----------------------------------------------------------------------------
// Function: SVDGenerator::getResetItem()
//-----------------------------------------------------------------------------
QSharedPointer<MemoryItem> SVDGenerator::getResetItem(QSharedPointer<MemoryItem> registerItem) const
{
    for (auto subItem : registerItem->getChildItems())
    {
        if (subItem->getType() == MemoryDesignerConstants::RESET_TYPE)
        {
            return subItem;
        }
    }

    return QSharedPointer<MemoryItem>();
}

//-----------------------------------------------------------------------------
// Function: SVDGenerator::writeReset()
//-----------------------------------------------------------------------------
void SVDGenerator::writeReset(QXmlStreamWriter& writer, QSharedPointer<MemoryItem> resetItem) const
{
    bool transformSuccess = true;
    QString registerResetValue = resetItem->getResetValue();
    quint64 resetValue = registerResetValue.toULongLong(&transformSuccess, 2);
    registerResetValue = QString::number(resetValue, 16).toUpper();

    int valueNumbers = MemoryDesignerConstants::getAmountOfNumbersInRange(registerResetValue, registerResetValue);

    QString registerResetMask = resetItem->getResetMask();
    quint64 resetMask = registerResetMask.toULongLong(&transformSuccess, 2);
    registerResetMask = QString::number(resetMask, 16).toUpper();

    int maskNumbers = MemoryDesignerConstants::getAmountOfNumbersInRange(registerResetMask, registerResetMask);

    int resetNumbers = valueNumbers;
    if (maskNumbers > resetNumbers)
    {
        resetNumbers = maskNumbers;
    }

    registerResetValue = SVDConstants::HEXADECIMAL + MemoryDesignerConstants::getValueWithZerosAdded(registerResetValue, resetNumbers);
    registerResetMask = SVDConstants::HEXADECIMAL + MemoryDesignerConstants::getValueWithZerosAdded(registerResetMask, resetNumbers);

    writer.writeTextElement(SVDConstants::RESETVALUE, registerResetValue);
    writer.writeTextElement(SVDConstants::RESETMASK, registerResetMask);
}

//-----------------------------------------------------------------------------
// Function: SVDGenerator::getFieldItemsInOrder()
//-----------------------------------------------------------------------------
QMap<quint64, QSharedPointer<MemoryItem> > SVDGenerator::getFieldItemsInOrder(
    QSharedPointer<MemoryItem> registerItem, quint64 const& registerOffset)
{
    QMap<quint64, QSharedPointer<MemoryItem> > orderedFieldItems;
    
    for (auto fieldItem : getSubMemoryItems(registerItem, MemoryDesignerConstants::FIELD_TYPE))
    {
        quint64 fieldStart = getFieldStart(fieldItem, registerOffset);
        orderedFieldItems.insert(fieldStart, fieldItem);
    }

    return orderedFieldItems;
}

//-----------------------------------------------------------------------------
// Function: SVDGenerator::writeFields()
//-----------------------------------------------------------------------------
void SVDGenerator::writeFields(QXmlStreamWriter& writer, QSharedPointer<Register> containingRegister,
    QMap<quint64, QSharedPointer<MemoryItem>> fieldItems)
{
    if (fieldItems.isEmpty())
    {
        return;
    }

    writer.writeStartElement(SVDConstants::FIELDSELEMENT);

    QMapIterator<quint64, QSharedPointer<MemoryItem> > fieldIterator(fieldItems);
    while (fieldIterator.hasNext())
    {
        fieldIterator.next();
        QSharedPointer<MemoryItem> fieldItem = fieldIterator.value();
        QSharedPointer<Field> actualField = getField(containingRegister, fieldItem);
        if (actualField)
        {
            writer.writeStartElement(SVDConstants::SINGLEFIELDELEMENT);

            writer.writeTextElement(SVDConstants::NAME, formatName(fieldItem->getName()));
            writeOptionalElement(writer, SVDConstants::DESCRIPTION, actualField->description());

            QString fieldStart = fieldItem->getOffset();
            QString fieldEnd = QString::number(getFieldEnd(fieldItem, fieldStart.toULongLong()));

            writer.writeTextElement(SVDConstants::BITRANGE, "[" + fieldEnd + ":" + fieldStart + "]");
            writeOptionalElement(writer, SVDConstants::ACCESS, AccessTypes::access2Str(actualField->getAccess()));

            writeEnumeratedValues(writer, actualField, fieldItem);

            writer.writeEndElement(); //! field
        }
    }

    writer.writeEndElement(); //! fields
}

//-----------------------------------------------------------------------------
// Function: SVDGenerator::getFieldStart()
//-----------------------------------------------------------------------------
quint64 SVDGenerator::getFieldStart(QSharedPointer<MemoryItem> fieldItem, quint64 registerOffset)
{
    quint64 fieldAddress = fieldItem->getAddress().toInt();
    quint64 fieldOffset = fieldItem->getOffset().toInt();

    int aub = fieldItem->getAUB().toInt();

    fieldOffset = (fieldAddress - registerOffset) * aub + fieldOffset;
    return fieldOffset;
}

//-----------------------------------------------------------------------------
// Function: SVDGenerator::getFieldEnd()
//-----------------------------------------------------------------------------
quint64 SVDGenerator::getFieldEnd(QSharedPointer<MemoryItem> fieldItem, quint64 fieldOffset)
{
    quint64 fieldWidth = fieldItem->getWidth().toInt();

    if (fieldWidth > 0)
    {
        return fieldOffset + fieldWidth - 1;
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: SVDGenerator::writeEnumeratedValues()
//-----------------------------------------------------------------------------
void SVDGenerator::writeEnumeratedValues(QXmlStreamWriter& writer, QSharedPointer<Field> containingField,
    QSharedPointer<MemoryItem> fieldItem)
{
    QVector<QSharedPointer<MemoryItem> > enumItems =
        getSubMemoryItems(fieldItem, MemoryDesignerConstants::ENUMERATED_VALUE_TYPE);
    if (enumItems.isEmpty())
    {
        return;
    }

    writer.writeStartElement(SVDConstants::ENUMERATEDVALUESELEMENT);

    for (auto enumeratedItem : enumItems)
    {
        QSharedPointer<EnumeratedValue> actualEnumeration = getEnumeratedValue(containingField, enumeratedItem);
        if (actualEnumeration)
        {
            writer.writeStartElement(SVDConstants::SINGLEENUMERATEDVALUEELEMENT);

            writer.writeTextElement(SVDConstants::NAME, formatName(enumeratedItem->getName()));
            writeOptionalElement(writer, SVDConstants::DESCRIPTION, actualEnumeration->description());

            writer.writeTextElement(SVDConstants::VALUE, enumeratedItem->getValue());
            writeOptionalElement(writer, SVDConstants::USAGE, EnumeratedValue::usage2Str(actualEnumeration->getUsage()));

            writer.writeEndElement(); //! enumeratedValue
        }
    }

    writer.writeEndElement(); //! enumeratedValues
}

//-----------------------------------------------------------------------------
// Function: SVDGenerator::valueToHexa()
//-----------------------------------------------------------------------------
QString SVDGenerator::valueToHexa(quint64 const& value) const
{
    QString valueInHexa = QString::number(value, 16).toUpper();

    int valueNumbers =
        MemoryDesignerConstants::getAmountOfNumbersInRange(valueInHexa, valueInHexa);

    valueInHexa = SVDConstants::HEXADECIMAL +
        MemoryDesignerConstants::getValueWithZerosAdded(valueInHexa, valueNumbers);

    return valueInHexa;
}

//-----------------------------------------------------------------------------
// Function: SVDGenerator::writeOptionalElement()
//-----------------------------------------------------------------------------
void SVDGenerator::writeOptionalElement(QXmlStreamWriter& writer, QString const& elementName,
    QString const& elementValue)
{
    if (!elementValue.isEmpty())
    {
        writer.writeTextElement(elementName, elementValue);
    }
}

//-----------------------------------------------------------------------------
// Function: SVDGenerator::getAddressBlock()
//-----------------------------------------------------------------------------
QSharedPointer<AddressBlock> SVDGenerator::getAddressBlock(QSharedPointer<const Component> containingComponent,
    QSharedPointer<MemoryItem> mapItem, QSharedPointer<MemoryItem> blockItem) const
{
    QSharedPointer<MemoryMap> containingMap = getMemoryMap(mapItem, containingComponent);
    if (containingMap)
    {
        for (auto blockBase : *containingMap->getMemoryBlocks())
        {
            QSharedPointer<AddressBlock> block = blockBase.dynamicCast<AddressBlock>();
            if (block && block->name() == blockItem->getName())
            {
                return block;
            }
        }
    }

    return QSharedPointer<AddressBlock>();
}

//-----------------------------------------------------------------------------
// Function: SVDGenerator::getRegister()
//-----------------------------------------------------------------------------
QSharedPointer<Register> SVDGenerator::getRegister(QSharedPointer<AddressBlock> containingBlock,
    QSharedPointer<MemoryItem> registerItem) const
{
    if (containingBlock)
    {
        for (auto baseRegister : *containingBlock->getRegisterData())
        {
            QSharedPointer<Register> currenRegister = baseRegister.dynamicCast<Register>();
            if (currenRegister && currenRegister->name() == registerItem->getName())
            {
                return currenRegister;
            }
        }
    }

    return QSharedPointer<Register>();
}

//-----------------------------------------------------------------------------
// Function: SVDGenerator::getField()
//-----------------------------------------------------------------------------
QSharedPointer<Field> SVDGenerator::getField(QSharedPointer<Register> containingRegister,
    QSharedPointer<MemoryItem> fieldItem) const
{
    for (auto field : *containingRegister->getFields())
    {
        if (field->name() == fieldItem->getName())
        {
            return field;
        }
    }

    return QSharedPointer<Field>();
}

//-----------------------------------------------------------------------------
// Function: SVDGenerator::getEnumeratedValue()
//-----------------------------------------------------------------------------
QSharedPointer<EnumeratedValue> SVDGenerator::getEnumeratedValue(QSharedPointer<Field> containingField,
    QSharedPointer<MemoryItem> enumeratedItem) const
{
    for (auto enumeration : *containingField->getEnumeratedValues())
    {
        if (enumeration->name() == enumeratedItem->getName())
        {
            return enumeration;
        }
    }

    return QSharedPointer<EnumeratedValue>();
}
