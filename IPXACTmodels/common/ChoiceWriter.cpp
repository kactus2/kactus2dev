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
#include "CommonItemsWriter.h"

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

    CommonItemsWriter::writeNonEmptyElement(writer, QStringLiteral("text"), enumeration->getText());

    CommonItemsWriter::writeNonEmptyElement(writer, QStringLiteral("help"), enumeration->getHelp());

    writer.writeCharacters(enumeration->getValue());

    writer.writeEndElement(); // ipxact:enumeration
}