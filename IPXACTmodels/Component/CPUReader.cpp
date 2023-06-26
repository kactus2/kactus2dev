//-----------------------------------------------------------------------------
// File: CPUReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 15.09.2015
//
// Description:
// Reader class for IP-XACT CPU element.
//-----------------------------------------------------------------------------

#include "CPUReader.h"

#include <IPXACTmodels/common/NameGroupReader.h>
#include <IPXACTmodels/common/ParameterReader.h>

#include <IPXACTmodels/common/FileBuilderReader.h>

//-----------------------------------------------------------------------------
// Function: CPUReader::createCPUFrom()
//-----------------------------------------------------------------------------
QSharedPointer<Cpu> CPUReader::createCPUFrom(QDomNode const& cpuNode, Document::Revision docRevision)
{
	// Create the new CPU.
	QSharedPointer<Cpu> newCpu (new Cpu());

    NameGroupReader::parseNameGroup(cpuNode, newCpu);

    if (docRevision == Document::Revision::Std14)
    {
        Details::parseIsPresent(cpuNode, newCpu);

        Details::parseAddressSpaceRefs(cpuNode, newCpu);
    }
    else if (docRevision == Document::Revision::Std22)
    {
        Details::parseBlockSize(cpuNode, newCpu);

        Details::parseRegions(cpuNode, newCpu);

        Details::parseAddressUnitBits(cpuNode, newCpu);

        Details::parseExecutableImages(cpuNode, newCpu);

        Details::parseMemoryMapReference(cpuNode, newCpu);
    }

    Details::parseParameters(cpuNode, newCpu);

    CommonItemsReader::parseVendorExtensions( cpuNode, newCpu );

    return newCpu;
}

//-----------------------------------------------------------------------------
// Function: CPUReader::parseIsPresent()
//-----------------------------------------------------------------------------
void CPUReader::Details::parseIsPresent(QDomNode const& cpuNode, QSharedPointer<Cpu> newCPU)
{
    newCPU->setIsPresent(CommonItemsReader::parseIsPresent(
        cpuNode.firstChildElement(QStringLiteral("ipxact:isPresent"))));
}

//-----------------------------------------------------------------------------
// Function: CPUReader::parseAddressSpaceRefs()
//-----------------------------------------------------------------------------
void CPUReader::Details::parseAddressSpaceRefs(QDomNode const& cpuNode, QSharedPointer<Cpu> newCpu)
{
    QDomNodeList referenceNodes = cpuNode.toElement().elementsByTagName(QStringLiteral("ipxact:addressSpaceRef"));

    for (int spaceIndex = 0; spaceIndex < referenceNodes.count(); ++spaceIndex)
    {
        QDomElement addressSpaceElement = referenceNodes.at(spaceIndex).toElement();

        QString reference = addressSpaceElement.attribute(QStringLiteral("addressSpaceRef"));
        QSharedPointer<Cpu::AddressSpaceRef> newAddressSpaceReference(new Cpu::AddressSpaceRef(reference));

        QDomElement isPresentElement = addressSpaceElement.firstChildElement(QStringLiteral("ipxact:isPresent"));
        newAddressSpaceReference->setIsPresent(CommonItemsReader::parseIsPresent(isPresentElement));

        newCpu->getAddressSpaceReferences()->append(newAddressSpaceReference);
    }
}

//-----------------------------------------------------------------------------
// Function: CPUReader::Details::parseBlockSize()
//-----------------------------------------------------------------------------
void CPUReader::Details::parseBlockSize(QDomNode const& cpuNode, QSharedPointer<Cpu> newCpu)
{
    QDomElement rangeElement = cpuNode.firstChildElement(QStringLiteral("ipxact:range"));
    newCpu->setRange(rangeElement.firstChild().nodeValue());

    QDomElement widthElement = cpuNode.firstChildElement(QStringLiteral("ipxact:width"));
    newCpu->setWidth(widthElement.firstChild().nodeValue());
}

//-----------------------------------------------------------------------------
// Function: CPUReader::Details::parseRegions()
//-----------------------------------------------------------------------------
void CPUReader::Details::parseRegions(QDomNode const& cpuNode, QSharedPointer<Cpu> newCpu)
{
    QDomNode regionsNode = cpuNode.firstChildElement(QStringLiteral("ipxact:regions"));

    QDomNodeList regionNodes = regionsNode.childNodes();
    const int REGION_COUNT = regionNodes.count();
    for (int i = 0; i < REGION_COUNT; ++i)
    {
        QDomNode regionNode = regionNodes.at(i);
        QSharedPointer<Region> newRegion(new Region());

        NameGroupReader::parseNameGroup(regionNode, newRegion);

        QDomElement offsetElement = regionNode.firstChildElement(QStringLiteral("ipxact:addressOffset"));
        newRegion->setOffset(offsetElement.firstChild().nodeValue());

        QDomElement rangeElement = regionNode.firstChildElement(QStringLiteral("ipxact:range"));
        newRegion->setRange(rangeElement.firstChild().nodeValue());

        CommonItemsReader::parseVendorExtensions(regionNode, newRegion);

        newCpu->getRegions()->append(newRegion);
    }
}

//-----------------------------------------------------------------------------
// Function: CPUReader::Details::parseAddressUnitBits()
//-----------------------------------------------------------------------------
void CPUReader::Details::parseAddressUnitBits(QDomNode const& cpuNode, QSharedPointer<Cpu> newCpu)
{
    QDomElement aubElement = cpuNode.firstChildElement(QStringLiteral("ipxact:addressUnitBits"));
    newCpu->setAddressUnitBits(aubElement.firstChild().nodeValue());
}

//-----------------------------------------------------------------------------
// Function: CPUReader::Details::parseExecutableImages()
//-----------------------------------------------------------------------------
void CPUReader::Details::parseExecutableImages(QDomNode const& cpuNode, QSharedPointer<Cpu> newCpu)
{
    QDomNodeList imageNodes = cpuNode.toElement().elementsByTagName(QStringLiteral("ipxact:executableImage"));

    const int IMAGE_COUNT = imageNodes.count();
    for (int i = 0; i < IMAGE_COUNT; ++i)
    {
        QDomElement imageElement = imageNodes.at(i).toElement();
        QSharedPointer<ExecutableImage> image(new ExecutableImage);

        NameGroupReader::parseNameGroup(imageElement, image);

        QString id = imageElement.attribute(QStringLiteral("imageId"));
        image->setImageId(id);

        QString type = imageElement.attribute(QStringLiteral("imageType"));
        image->setImageType(type);

        image->setParameters(CommonItemsReader::parseAndCreateParameters(imageElement, Document::Revision::Std22));

        auto languageToolsElement = imageElement.firstChildElement(QStringLiteral("ipxact:languageTools"));
        Details::parseLanguageTools(languageToolsElement, image);

        newCpu->getExecutableImages()->append(image);
    }
}

//-----------------------------------------------------------------------------
// Function: CPUReader::Details::parseLanguageTools()
//-----------------------------------------------------------------------------
void CPUReader::Details::parseLanguageTools(QDomElement const& languageToolsElement,
    QSharedPointer<ExecutableImage> image)
{
    QDomNodeList fileBuilderNodes = languageToolsElement.elementsByTagName(QStringLiteral("ipxact:fileBuilder"));

    QSharedPointer<LanguageTools> languageTools(new LanguageTools);
    
    const int BUILDER_COUNT = fileBuilderNodes.count();
    for (int i = 0; i < BUILDER_COUNT; ++i)
    {
        QDomNode builderElement = fileBuilderNodes.at(i);

        auto fileBuilder = FileBuilderReader::createFileBuilderFrom(builderElement, Document::Revision::Std22);
        languageTools->getFileBuilders()->append(fileBuilder);
    }

    auto linkerElement = languageToolsElement.firstChildElement(QStringLiteral("ipxact:linker"));
    languageTools->setLinker(linkerElement.firstChild().nodeValue());

    auto linkerFlagsElement = languageToolsElement.firstChildElement(QStringLiteral("ipxact:linkerFlags"));
    languageTools->setLinkerFlags(linkerFlagsElement.firstChild().nodeValue());

    auto linkerCommandFileElement = languageToolsElement.firstChildElement(QStringLiteral("ipxact:linkerCommandFile"));

    parseLinkerCommandFile(linkerCommandFileElement, languageTools);

    image->setLanguageTools(languageTools);
}

//-----------------------------------------------------------------------------
// Function: CPUReader::Details::parseLinkerCommandFile()
//-----------------------------------------------------------------------------
void CPUReader::Details::parseLinkerCommandFile(QDomElement const& linkerCommandFileElement,
    QSharedPointer<LanguageTools> languageTools)
{
    auto linkerCommandFile = languageTools->getLinkerCommandFile();

    auto nameElement = linkerCommandFileElement.firstChildElement(QStringLiteral("ipxact:name"));
    linkerCommandFile->name_ = nameElement.firstChild().nodeValue();

    auto switchElement = linkerCommandFileElement.firstChildElement(QStringLiteral("ipxact:commandLineSwitch"));
    linkerCommandFile->commandLineSwitch_ = switchElement.firstChild().nodeValue();

    auto enableElement = linkerCommandFileElement.firstChildElement(QStringLiteral("ipxact:enable"));
    linkerCommandFile->enable_ = enableElement.firstChild().nodeValue();

    auto generatorRefs = linkerCommandFileElement.elementsByTagName(QStringLiteral("ipxact:generatorRef"));
    const int GENERATOR_COUNT = generatorRefs.count();
    for (int i = 0; i < GENERATOR_COUNT; ++i)
    {
        QDomNode generatorNode = generatorRefs.at(i);
        linkerCommandFile->generatorRefs_.append(generatorNode.firstChild().nodeValue());
    }

    CommonItemsReader::parseVendorExtensions(linkerCommandFileElement, linkerCommandFile);
}

//-----------------------------------------------------------------------------
// Function: CPUReader::Details::parseMemoryMapReference()
//-----------------------------------------------------------------------------
void CPUReader::Details::parseMemoryMapReference(QDomNode const& cpuNode, QSharedPointer<Cpu> newCpu)
{
    QDomElement memoryMapRefElement = cpuNode.firstChildElement(QStringLiteral("ipxact:memoryMapRef"));
    newCpu->setMemoryMapReference(memoryMapRefElement.firstChild().nodeValue());
}

//-----------------------------------------------------------------------------
// Function: CPUReader::parseParameters()
//-----------------------------------------------------------------------------
void CPUReader::Details::parseParameters(QDomNode const& cpuNode, QSharedPointer<Cpu> newCpu)
{
    QSharedPointer<QList<QSharedPointer<Parameter> > > parameters = CommonItemsReader::parseAndCreateParameters(cpuNode);

    if (!parameters->isEmpty())
    {
        newCpu->getParameters()->append(*parameters);
    }
}
