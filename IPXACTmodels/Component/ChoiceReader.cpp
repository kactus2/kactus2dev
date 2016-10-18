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
ChoiceReader::ChoiceReader(QObject* parent) :
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
QSharedPointer<Choice> ChoiceReader::createChoiceFrom(QDomNode const& choiceNode) const
{
	// Create the new CPU.
	QSharedPointer<Choice> newChoice (new Choice());

	QDomNodeList children = choiceNode.childNodes();

	for (int i = 0; i < children.size(); ++i)
	{
		// Get the name,
		if (children.at(i).nodeName() == QLatin1String("ipxact:name"))
		{
			QString choiceName = children.at(i).childNodes().at(0).nodeValue();
			newChoice->setName(XmlUtils::removeWhiteSpace(choiceName) );
		}

		// Get the enumerations.
		else if (children.at(i).nodeName() == QLatin1String("ipxact:enumeration"))
		{
			QSharedPointer<Enumeration> enumeration(new Enumeration);

			QDomNode enumerationNode = children.at(i);

			// get the name of the enumeration
			enumeration->setValue(enumerationNode.childNodes().at(0).nodeValue());

			QDomNamedNodeMap attributeMap = enumerationNode.attributes();
			enumeration->setText(attributeMap.namedItem(QStringLiteral("text")).nodeValue());
			enumeration->setHelp(attributeMap.namedItem(QStringLiteral("help")).nodeValue());

			newChoice->enumerations()->append(enumeration);
		}
	}

    return newChoice;
}
