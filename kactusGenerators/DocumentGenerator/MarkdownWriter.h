//-----------------------------------------------------------------------------
// File: GeneralDocumentGenerator.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Anton Hagqvist
// Date: 12.4.2023
//
// Description:
// Writes markdown documentation
//-----------------------------------------------------------------------------

#ifndef MARKDOWNWRITER_H
#define MARKDOWNWRITER_H

#include <kactusGenerators/DocumentGenerator/DocumentationWriter.h>

#include <QTextStream>

class MarkdownWriter : public DocumentationWriter
{
public:
    MarkdownWriter();
    virtual ~MarkdownWriter();
    void writeHeader(QTextStream& stream);
};

#endif // MARKDOWNWRITER_H