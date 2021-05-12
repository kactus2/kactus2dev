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

#include <library/LibraryInterface.h>

#include <editors/ComponentEditor/common/SystemVerilogExpressionParser.h>

#include <editors/MemoryDesigner/ConnectivityConnection.h>
#include <editors/MemoryDesigner/ConnectivityGraph.h>
#include <editors/MemoryDesigner/ConnectivityGraphFactory.h>
#include <editors/MemoryDesigner/ConnectivityInterface.h>
#include <editors/MemoryDesigner/MasterSlavePathSearch.h>
#include <editors/MemoryDesigner/MemoryItem.h>
#include <editors/MemoryDesigner/MemoryDesignerConstants.h>
#include <editors/MemoryDesigner/MemoryConnectionAddressCalculator.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/Component/MemoryBlockBase.h>
#include <IPXACTmodels/Component/AddressBlock.h>
#include <IPXACTmodels/Component/RegisterBase.h>
#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/Field.h>
#include <IPXACTmodels/Component/EnumeratedValue.h>

#include <IPXACTmodels/Design/Design.h>

#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>

#include <QXmlStreamWriter>

//-----------------------------------------------------------------------------
// Function: SVDGenerator::SVDGenerator()
//-----------------------------------------------------------------------------
SVDGenerator::SVDGenerator(LibraryInterface* library):
library_(library),
graphFactory_(library),
generatedFiles_()
{

}

//-----------------------------------------------------------------------------
// Function: SVDGenerator::generate()
//-----------------------------------------------------------------------------
void SVDGenerator::generate(QSharedPointer<Component> topComponent, QString const& componentPath,
    QVector<QSharedPointer<ConnectivityGraphUtilities::cpuCheckInterface> > const& cpuRoutes,
    bool peripheralsAreBlocks, bool peripheralsAreMaps)
{
    QStringList cpuNames;
    for (auto cpuMasterRoute : cpuRoutes)
    {
        if (cpuMasterRoute->routes_.size() > 0)
        {
            writeFile(topComponent, componentPath, cpuMasterRoute, peripheralsAreBlocks, peripheralsAreMaps,
                cpuNames);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: SVDGenerator::getGeneratedFiles()
//-----------------------------------------------------------------------------
QStringList SVDGenerator::getGeneratedFiles()
{
    return generatedFiles_;
}

//-----------------------------------------------------------------------------
// Function: SVDGenerator::writeFile()
//-----------------------------------------------------------------------------
void SVDGenerator::writeFile(QSharedPointer<Component> topComponent, QString const& componentPath,
    QSharedPointer<ConnectivityGraphUtilities::cpuCheckInterface> cpuRoute, bool peripheralsAreBlocks,
    bool peripheralsAreMaps, QStringList& fileNames)
{
    QSharedPointer<const ConnectivityInterface> cpuInterface = cpuRoute->cpuInterface_;
    QSharedPointer<const ConnectivityComponent> routeComponent = cpuInterface->getInstance();
    QSharedPointer<const Component> interfaceComponent =
        ConnectivityGraphUtilities::getInterfacedComponent(library_, routeComponent);
    if (interfaceComponent)
    {
        QSharedPointer<Cpu> interfaceCPU =
            ConnectivityGraphUtilities::getInterfacedCPU(interfaceComponent, cpuInterface);

        QString fileName = topComponent->getVlnv().getName() + "_" + interfaceCPU->name();
        if (fileNames.contains(interfaceCPU->name()))
        {
            fileName = fileName + "_" + QString::number(getFileNumberExtension(fileNames, fileName));
        }

        QString svdFilePath = componentPath + "/" + fileName;
        fileNames.append(fileName);

        svdFilePath += ".svd";

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
        writeCPU(xmlWriter, interfaceCPU, cpuRoute);
        writeAddressSpaceData(xmlWriter, cpuInterface);
        writePeripherals(xmlWriter, cpuRoute, peripheralsAreBlocks, peripheralsAreMaps);

        xmlWriter.writeEndElement();    //! device

        generatedFiles_.append(svdFilePath);
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
    writer.writeTextElement(QLatin1String("vendor"), topVLNV.getVendor());
    writer.writeTextElement(QLatin1String("name"), topVLNV.getName());
    writer.writeTextElement(QLatin1String("version"), topVLNV.getVersion());
    writeOptionalElement(writer, "description", topComponent->getDescription());
}

//-----------------------------------------------------------------------------
// Function: SVDGenerator::writeCPU()
//-----------------------------------------------------------------------------
void SVDGenerator::writeCPU(QXmlStreamWriter& writer, QSharedPointer<Cpu> currentCPU,
    QSharedPointer<ConnectivityGraphUtilities::cpuCheckInterface> cpuContainer)
{
    writer.writeStartElement("cpu");

    QString cpuName = currentCPU->name();
    if (cpuName != "CM0" && cpuName != "CM0PLUS" && cpuName != "CM0+" && cpuName != "CM1" && cpuName != "SC000" &&
        cpuName != "CM23" && cpuName != "CM3" && cpuName != "CM33" && cpuName != "CM35P" && cpuName != "SC300" &&
        cpuName != "CM4" && cpuName != "CM7" && cpuName != "CA5" && cpuName != "CA7" && cpuName != "CA8" &&
        cpuName != "CA9" && cpuName != "CA15" && cpuName != "CA17" && cpuName != "CA53" && cpuName != "CA57" &&
        cpuName != "CA72")
    {
        cpuName = "other";
    }

    writer.writeTextElement("name", cpuName);

    writer.writeTextElement("revision", cpuContainer->revision_);
    writer.writeTextElement("endian", cpuContainer->endian_);
    writeBoolean(writer, "mpuPresent", cpuContainer->mpuPresent_);
    writeBoolean(writer, "fpuPresent", cpuContainer->fpuPresent_);
    writer.writeTextElement("nvicPrioBits", cpuContainer->nvicPrioBits_);
    writeBoolean(writer, "vendorSystickConfig", cpuContainer->vendorSystickConfig_);

    writer.writeEndElement(); //! cpu
}

//-----------------------------------------------------------------------------
// Function: SVDGenerator::writeBoolean()
//-----------------------------------------------------------------------------
void SVDGenerator::writeBoolean(QXmlStreamWriter& writer, QString const& elementName, bool state)
{
    if (state == true)
    {
        writer.writeTextElement(elementName, "true");
    }
    else
    {
        writer.writeTextElement(elementName, "false");
    }
}

//-----------------------------------------------------------------------------
// Function: SVDGenerator::writeAddressSpaceData()
//-----------------------------------------------------------------------------
void SVDGenerator::writeAddressSpaceData(QXmlStreamWriter& writer,
    QSharedPointer<const ConnectivityInterface> cpuInterface)
{
    writer.writeTextElement("addressUnitBits", cpuInterface->getConnectedMemory()->getAUB());
    writer.writeTextElement("width", cpuInterface->getConnectedMemory()->getWidth());
    writer.writeTextElement("size", cpuInterface->getConnectedMemory()->getSize());
}

//-----------------------------------------------------------------------------
// Function: SVDGenerator::writePeripherals()
//-----------------------------------------------------------------------------
void SVDGenerator::writePeripherals(QXmlStreamWriter& writer,
    QSharedPointer<ConnectivityGraphUtilities::cpuCheckInterface> routeCollection, bool peripheralsAreBlocks,
    bool peripheralsAreMaps)
{
    writer.writeStartElement("peripherals");
    QSharedPointer<const ConnectivityInterface> cpuInterface = routeCollection->cpuInterface_;

    for (auto masterSlaveRoute : routeCollection->routes_)
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

                MemoryConnectionAddressCalculator::ConnectionPathVariables pathAddresses =
                    MemoryConnectionAddressCalculator::calculatePathAddresses(cpuInterface, routeInterface,
                        masterSlaveRoute);

                quint64 memoryBaseAddress = pathAddresses.remappedAddress_;
                QString baseAddressInHexa = valueToHexa(memoryBaseAddress);

                if (peripheralsAreMaps)
                {
                    writeMapPeripheral(writer, component, interfaceMemory, baseAddressInHexa);
                }
                else
                {
                    writeBlockPeripherals(writer, component, interfaceMemory, memoryBaseAddress);
                }
            }
        }
    }

    writer.writeEndElement(); //! peripherals
}

//-----------------------------------------------------------------------------
// Function: SVDGenerator::writeMapPeripheral()
//-----------------------------------------------------------------------------
void SVDGenerator::writeMapPeripheral(QXmlStreamWriter& writer, QSharedPointer<const Component> component,
    QSharedPointer<MemoryItem> mapItem, QString const& mapBaseAddressInHexa)
{
    QSharedPointer<MemoryMap> memoryMap = getMemoryMap(mapItem, component);
    if (!memoryMap)
    {
        return;
    }

    writer.writeStartElement("peripheral");

    writer.writeTextElement("name", memoryMap->name());
    writer.writeTextElement("version", component->getVlnv().getVersion());
    writeOptionalElement(writer, "description", memoryMap->description());
    writer.writeTextElement("baseAddress", mapBaseAddressInHexa);
    writeAddressBlocks(writer, component, mapItem);

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
void SVDGenerator::writeAddressBlocks(QXmlStreamWriter& writer, QSharedPointer<const Component> containingComponent,
    QSharedPointer<MemoryItem> mapItem)
{
    for (auto blockItem : getAddressBlockItems(mapItem))
    {
        QSharedPointer<AddressBlock> containingBlock =
            getAddressBlock(containingComponent, mapItem, blockItem);
        if (!containingBlock)
        {
            continue;
        }

        writeSingleAddressBlock(
            writer, blockItem->getAddress().toULongLong(), containingBlock, blockItem);
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
    QSharedPointer<AddressBlock> containingBlock, QSharedPointer<MemoryItem> blockItem)
{
    writer.writeStartElement("addressBlock");

    QString addressOffsetInHexa = valueToHexa(offset);
    QString rangeInHexa = valueToHexa(blockItem->getRange().toULongLong());

    writer.writeTextElement("offset", addressOffsetInHexa);
    writer.writeTextElement("size", rangeInHexa);

    QString usageString = "buffer";
    General::Usage blockUsage = blockItem->getUsage();
    if (blockUsage == General::REGISTER || blockUsage == General::USAGE_COUNT)
    {
        usageString = "registers";
    }
    if (blockUsage == General::RESERVED)
    {
        usageString = "reserved";
    }

    writer.writeTextElement("usage", usageString);

    writer.writeEndElement(); //! addressBlock

    writeRegisters(writer, containingBlock, blockItem);
}

//-----------------------------------------------------------------------------
// Function: SVDGenerator::writeRegisters()
//-----------------------------------------------------------------------------
void SVDGenerator::writeRegisters(QXmlStreamWriter& writer, QSharedPointer<AddressBlock> containingBlock,
    QSharedPointer<MemoryItem> blockItem)
{
    QVector<QSharedPointer<MemoryItem> > registerItems =
        getSubMemoryItems(blockItem, MemoryDesignerConstants::REGISTER_TYPE);
    if (registerItems.isEmpty())
    {
        return;
    }

    writer.writeStartElement("registers");

    for (auto registerItem : registerItems)
    {
        QSharedPointer<Register> realRegister = getRegister(containingBlock, registerItem);
        if (!realRegister)
        {
            continue;
        }

        writer.writeStartElement("register");

        writer.writeTextElement("name", registerItem->getName());
        writeOptionalElement(writer, "description", realRegister->description());

        QString addressOffsetInHexa = valueToHexa(registerItem->getOffset().toULongLong());
        QString sizeString = registerItem->getSize();

        writer.writeTextElement("addressOffset", addressOffsetInHexa);
        writer.writeTextElement("size", sizeString);
        writeOptionalElement(writer, "access", AccessTypes::access2Str(realRegister->getAccess()));

        writeFields(writer, realRegister, registerItem);

        writer.writeEndElement(); //! register
    }

    writer.writeEndElement(); //! registers
}

//-----------------------------------------------------------------------------
// Function: SVDGenerator::writeFields()
//-----------------------------------------------------------------------------
void SVDGenerator::writeFields(QXmlStreamWriter& writer, QSharedPointer<Register> containingRegister,
    QSharedPointer<MemoryItem> registerItem)
{
    QVector<QSharedPointer<MemoryItem> > fieldItems =
        getSubMemoryItems(registerItem, MemoryDesignerConstants::FIELD_TYPE);
    if (fieldItems.isEmpty())
    {
        return;
    }

    writer.writeStartElement("fields");

    quint64 registerOffset = registerItem->getAddress().toULongLong();

    for (auto fieldItem : fieldItems)
    {
        QSharedPointer<Field> actualField = getField(containingRegister, fieldItem);
        if (actualField)
        {
            writer.writeStartElement("field");

            writer.writeTextElement("name", fieldItem->getName());
            writeOptionalElement(writer, "description", actualField->description());

            int aub = fieldItem->getAUB().toInt();

            quint64 fieldAddress = fieldItem->getAddress().toInt();
            quint64 fieldOffset = fieldItem->getOffset().toInt();
            quint64 fieldWidth = fieldItem->getWidth().toInt();

            fieldOffset = (fieldAddress - registerOffset) * aub + fieldOffset;
            QString fieldStart = QString::number(fieldOffset);
            QString fieldEnd = "";
            if (fieldWidth > 0)
            {
                quint64 fieldEndNumber = fieldOffset + fieldWidth - 1;
                fieldEnd = QString::number(fieldEndNumber);
            }

            writer.writeTextElement("bitRange", "[" + fieldEnd + ":" + fieldStart + "]");
            writeOptionalElement(writer, "access", AccessTypes::access2Str(actualField->getAccess()));

            writeEnumeratedValues(writer, actualField, fieldItem);

            writer.writeEndElement(); //! field
        }
    }

    writer.writeEndElement(); //! fields
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

    writer.writeStartElement("enumeratedValues");

    for (auto enumeratedItem : enumItems)
    {
        QSharedPointer<EnumeratedValue> actualEnumeration = getEnumeratedValue(containingField, enumeratedItem);
        if (actualEnumeration)
        {

            writer.writeStartElement("enumeratedValue");

            writer.writeTextElement("name", enumeratedItem->getName());
            writeOptionalElement(writer, "description", actualEnumeration->description());
            writer.writeTextElement("value", enumeratedItem->getValue());
            writeOptionalElement(writer, "usage", EnumeratedValue::usage2Str(actualEnumeration->getUsage()));

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

    valueInHexa = "0x" +
        MemoryDesignerConstants::getValueWithZerosAdded(valueInHexa, valueNumbers);

    return valueInHexa;
}

//-----------------------------------------------------------------------------
// Function: SVDGenerator::writeBlockPeripherals()
//-----------------------------------------------------------------------------
void SVDGenerator::writeBlockPeripherals(QXmlStreamWriter& writer,
    QSharedPointer<const Component> containingComponent, QSharedPointer<MemoryItem> mapItem,
    quint64 mapBaseAddress)
{
    for (auto blockItem : getAddressBlockItems(mapItem))
    {
        QSharedPointer<AddressBlock> block = getAddressBlock(containingComponent, mapItem, blockItem);
        if (!block)
        {
            continue;
        }

        writer.writeStartElement("peripheral");

        writer.writeTextElement("name", block->name());
        writer.writeTextElement("version", containingComponent->getVlnv().getVersion());
        writeOptionalElement(writer, "description", block->description());

        quint64 addressOffset = blockItem->getAddress().toULongLong() + mapBaseAddress;
        QString addressOffsetInHexa = valueToHexa(addressOffset);

        writer.writeTextElement("baseAddress", addressOffsetInHexa);

        writeSingleAddressBlock(writer, 0, block, blockItem);

        writer.writeEndElement(); //! peripheral
    }
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
