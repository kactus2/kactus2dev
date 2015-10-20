//-----------------------------------------------------------------------------
// File: ChoiceReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 21.09.2015
//
// Description:
// Reader class for IP-XACT Choice element.
//-----------------------------------------------------------------------------

#include "ChoiceReader.h"
#include "../XmlUtils.h"

//-----------------------------------------------------------------------------
// Function: ChoiceReader::ChoiceReader()
//-----------------------------------------------------------------------------
ChoiceReader::ChoiceReader(QObject* parent /* = 0 */) :
QObject(parent)
{

}

//-----------------------------------------------------------------------------
// Function: ChoiceReader::~ChoiceReader()
//-----------------------------------------------------------------------------
ChoiceReader::~ChoiceReader()
{

}

//-----------------------------------------------------------------------------
// Function: ChoiceReader::createCPUFrom()
//-----------------------------------------------------------------------------
QSharedPointer<Choice> ChoiceReader::createChoiceFrom(QDomNode const& ChoiceNode) const
{
	// Create the new CPU.
	QSharedPointer<Choice> newChoice (new Choice());

	QDomNodeList children = ChoiceNode.childNodes();

	for (int i = 0; i < children.size(); ++i)
	{
		// Get the name,
		if (children.at(i).nodeName() == QString("ipxact:name"))
		{
			QString choiceName = children.at(i).childNodes().at(0).nodeValue();
			newChoice->setName( XmlUtils::removeWhiteSpace(choiceName) );
		}

		// Get the enumerations.
		else if (children.at(i).nodeName() == QString("ipxact:enumeration"))
		{
			QSharedPointer<Enumeration> enumr(new Enumeration);

			QDomNode enumerationNode = children.at(i);

			// get the name of the enumeration
			enumr->setValue( enumerationNode.childNodes().at(0).nodeValue() );

			QDomNamedNodeMap attributeMap = enumerationNode.attributes();
			enumr->setText( attributeMap.namedItem("text").nodeValue() );
			enumr->setHelp( attributeMap.namedItem("help").nodeValue() );

			newChoice->enumerations()->append(enumr);
		}
	}

    return newChoice;
}