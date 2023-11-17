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

//-----------------------------------------------------------------------------
// Function: ChoiceReader::createChoiceFrom()
//-----------------------------------------------------------------------------
QSharedPointer<Choice> ChoiceReader::createChoiceFrom(QDomNode const& choiceNode) const
{
	// Create the new CPU.
	QSharedPointer<Choice> newChoice (new Choice());

	QDomNodeList children = choiceNode.childNodes();

	const auto CHILD_COUNT = children.size();
	for (int i = 0; i < CHILD_COUNT; ++i)
    {
        const QDomNode currentNode = children.at(i);
		// Get the name,
		if (currentNode.nodeName() == QLatin1String("ipxact:name"))
		{
			QString choiceName = currentNode.childNodes().at(0).nodeValue();
			newChoice->setName(choiceName);
		}

		// Get the enumerations.
		else if (currentNode.nodeName() == QLatin1String("ipxact:enumeration"))
		{
			QSharedPointer<Enumeration> enumeration(new Enumeration);


			// get the name of the enumeration
			enumeration->setValue(currentNode.childNodes().at(0).nodeValue());

			QDomNamedNodeMap attributeMap = currentNode.attributes();
			enumeration->setText(attributeMap.namedItem(QStringLiteral("text")).nodeValue());
			enumeration->setHelp(attributeMap.namedItem(QStringLiteral("help")).nodeValue());

			newChoice->enumerations()->append(enumeration);
		}
	}

    return newChoice;
}
