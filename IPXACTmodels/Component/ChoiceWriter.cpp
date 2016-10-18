//-----------------------------------------------------------------------------
// File: ChoiceWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 21.09.2015
//
// Description:
// Writer class for IP-XACT Choice element.
//-----------------------------------------------------------------------------

#include "ChoiceWriter.h"

#include <IPXACTmodels/common/NameGroupWriter.h>

//-----------------------------------------------------------------------------
// Function: ChoiceWriter::ChoiceWriter()
//-----------------------------------------------------------------------------
ChoiceWriter::ChoiceWriter(QObject* parent) :
QObject(parent)
{

}

//-----------------------------------------------------------------------------
// Function: ChoiceWriter::~ChoiceWriter()
//-----------------------------------------------------------------------------
ChoiceWriter::~ChoiceWriter()
{

}

//-----------------------------------------------------------------------------
// Function: ChoiceWriter::createChoiceFrom()
//-----------------------------------------------------------------------------
void ChoiceWriter::writeChoice(QXmlStreamWriter& writer, QSharedPointer<Choice> choice) const
{
	writer.writeStartElement(QStringLiteral("ipxact:choice"));

	writer.writeTextElement(QStringLiteral("ipxact:name"), choice->name());

	QSharedPointer<QList<QSharedPointer<Enumeration> > > enumerations = choice->enumerations();

    int enumerationCount = enumerations->size();
	for (int i = 0; i < enumerationCount; i++)
	{
		QSharedPointer<Enumeration> enumeration = enumerations->at(i);

		writer.writeStartElement(QStringLiteral("ipxact:enumeration"));

		if (!enumeration->getText().isEmpty())
		{
			writer.writeAttribute(QStringLiteral("text"), enumeration->getText());
		}

		if (!enumeration->getHelp().isEmpty())
		{
			writer.writeAttribute(QStringLiteral("help"), enumeration->getHelp());
		}

		writer.writeCharacters(enumeration->getValue());

		writer.writeEndElement();
	}

	writer.writeEndElement();

	return;
}
