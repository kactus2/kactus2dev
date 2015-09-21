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
ChoiceWriter::ChoiceWriter(QObject* parent /* = 0 */) :
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
	writer.writeStartElement("ipxact:choice");

	writer.writeTextElement("ipxact:name", choice->name());

	QSharedPointer<QList<QSharedPointer<Enumeration> > > enumrs = choice->enumerations();

	for ( int i = 0; i < enumrs->size(); ++i )
	{
		QSharedPointer<Enumeration> enumr = enumrs->at(i);

		writer.writeStartElement("ipxact:enumeration");

		if (!enumr->getText().isEmpty())
		{
			writer.writeAttribute("text", enumr->getText());
		}

		if (!enumr->getHelp().isEmpty())
		{
			writer.writeAttribute("help", enumr->getHelp());
		}

		writer.writeCharacters(enumr->getValue());

		writer.writeEndElement();
	}

	writer.writeEndElement();

	return;
}