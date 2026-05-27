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
#include "ExecutableImagesReader.h"

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
    newCpu->getExecutableImages()->append(*ExecutableImagesReader::parseAndCreateExecutableImages(cpuNode, Document::Revision::Std22));
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
