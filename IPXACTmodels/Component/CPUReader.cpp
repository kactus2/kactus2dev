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

	// Parse presence, name group, and vendor extensions with pre-existing parsers.
	// CPU has no vendor extensions supported by Kactus2.
	parseIsPresent(cpuNode, newCpu);
	parseNameGroup(cpuNode, newCpu);
	parseVendorExtensions( cpuNode, newCpu );

	// The intermediate lists for parsed child nodes.
	QStringList addressSpaceRefs;
	QList<QSharedPointer<Parameter> > parameters;

	for (int i = 0; i < cpuNode.childNodes().count(); ++i)
	{
		QDomNode tempNode = cpuNode.childNodes().at(i);

		// Do not try to parse comments.
		if (tempNode.isComment())
		{
			continue;
		}

		if (tempNode.nodeName() == QString("ipxact:addressSpaceRef"))
		{
			// Get the addressSpaceRef attribute.
			QDomNamedNodeMap attributeMap = tempNode.attributes();
			QString str = attributeMap.namedItem(QString(
				"addressSpaceRef")).childNodes().at(0).nodeValue();

			// All was fine and attribute can be added.
			addressSpaceRefs.append(str);
		}
		else if (tempNode.nodeName() == QString("ipxact:parameters"))
		{
			// Use pre-existing parameter reader for reading parameters.
			ParameterReader reader;

			// Go through all parameters
			for (int j = 0; j < tempNode.childNodes().count(); ++j)
			{
				QDomNode parameterNode = tempNode.childNodes().at(j);

				// Do not try to parse comments.
				if (!parameterNode.isComment())
				{
					parameters.append(QSharedPointer<Parameter>(reader.createParameterFrom(parameterNode)));
				}
			}
		}
	}

	// Finally, set the parsed data as the CPU data.
	newCpu->setAddressSpaceRefs( addressSpaceRefs );
	newCpu->setParameters( parameters );

    return newCpu;
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
// Function: CPUReader::parseNameGroup()
//-----------------------------------------------------------------------------
void CPUReader::parseNameGroup(QDomNode const& cpuNode, QSharedPointer<Cpu> newCpu) const
{
    NameGroupReader nameReader;
    nameReader.parseNameGroup(cpuNode, newCpu);
}