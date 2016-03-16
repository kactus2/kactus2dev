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

//-----------------------------------------------------------------------------
// Function: CPUReader::CPUReader()
//-----------------------------------------------------------------------------
CPUReader::CPUReader(QObject* parent /* = 0 */) :
CommonItemsReader(parent)
{

}

//-----------------------------------------------------------------------------
// Function: CPUReader::~CPUReader()
//-----------------------------------------------------------------------------
CPUReader::~CPUReader()
{

}

//-----------------------------------------------------------------------------
// Function: CPUReader::createCPUFrom()
//-----------------------------------------------------------------------------
QSharedPointer<Cpu> CPUReader::createCPUFrom(QDomNode const& cpuNode) const
{
	// Create the new CPU.
	QSharedPointer<Cpu> newCpu (new Cpu());

	parseNameGroup(cpuNode, newCpu);

    parseIsPresent(cpuNode, newCpu);

    parseAddressSpaceRefs(cpuNode, newCpu);

    parseParameters(cpuNode, newCpu);

    parseVendorExtensions( cpuNode, newCpu );

    return newCpu;
}

//-----------------------------------------------------------------------------
// Function: CPUReader::parseNameGroup()
//-----------------------------------------------------------------------------
void CPUReader::parseNameGroup(QDomNode const& cpuNode, QSharedPointer<Cpu> newCpu) const
{
    NameGroupReader nameReader;
    nameReader.parseNameGroup(cpuNode, newCpu);
}

//-----------------------------------------------------------------------------
// Function: CPUReader::parseIsPresent()
//-----------------------------------------------------------------------------
void CPUReader::parseIsPresent(QDomNode const& cpuNode, QSharedPointer<Cpu> newCPU) const
{
	QString newIsPresent = cpuNode.firstChildElement("ipxact:isPresent").firstChild().nodeValue();
	if (!newIsPresent.isEmpty())
	{
		newCPU->setIsPresent(newIsPresent);
	}
}

//-----------------------------------------------------------------------------
// Function: CPUReader::parseAddressSpaceRefs()
//-----------------------------------------------------------------------------
void CPUReader::parseAddressSpaceRefs(QDomNode const& cpuNode, QSharedPointer<Cpu> newCpu) const
{
    QDomNodeList referenceNodes = cpuNode.toElement().elementsByTagName("ipxact:addressSpaceRef");

    for (int spaceIndex = 0; spaceIndex < referenceNodes.count(); ++spaceIndex)
    {
        QDomElement addressSpaceElement = referenceNodes.at(spaceIndex).toElement();

        QString reference = addressSpaceElement.attribute("addressSpaceRef");
        QSharedPointer<Cpu::AddressSpaceRef> newAddressSpaceReference (new Cpu::AddressSpaceRef(reference));

        QDomElement isPresentElement = addressSpaceElement.firstChildElement("ipxact:isPresent");
        if (!isPresentElement.isNull())
        {
            QString isPresent = isPresentElement.firstChild().nodeValue();
            newAddressSpaceReference->setIsPresent(isPresent);
        }

        newCpu->getAddressSpaceReferences()->append(newAddressSpaceReference);
    }
}

//-----------------------------------------------------------------------------
// Function: CPUReader::parseParameters()
//-----------------------------------------------------------------------------
void CPUReader::parseParameters(QDomNode const& cpuNode, QSharedPointer<Cpu> newCpu) const
{
    QSharedPointer<QList<QSharedPointer<Parameter> > > parameters = parseAndCreateParameters(cpuNode);

    if (!parameters->isEmpty())
    {
        newCpu->getParameters()->append(*parameters);
    }
}