//-----------------------------------------------------------------------------
// File: ChoiceWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 19.05.2023
//
// Description:
// Writer for choice element.
//-----------------------------------------------------------------------------

#include "ChoiceWriter.h"

//-----------------------------------------------------------------------------
// Function: ChoiceWriter::writeChoice()
//-----------------------------------------------------------------------------
void ChoiceWriter::writeChoice(QXmlStreamWriter& writer, QSharedPointer<Choice> choice)
{
    writer.writeStartElement(QStringLiteral("ipxact:choice"));

    writer.writeTextElement(QStringLiteral("ipxact:name"), choice->name());

    for (auto const& enumeration : *choice->enumerations())
    {
        Details::writeEnumeration(writer, enumeration);
    }

    writer.writeEndElement(); // ipxact:choice
}

//-----------------------------------------------------------------------------
// Function: ChoiceWriter::Details::writeEnumeration()
//-----------------------------------------------------------------------------
void ChoiceWriter::Details::writeEnumeration(QXmlStreamWriter& writer, QSharedPointer<Enumeration> enumeration)
{
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

    writer.writeEndElement(); // ipxact:enumeration
}