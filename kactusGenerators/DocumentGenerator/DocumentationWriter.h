//-----------------------------------------------------------------------------
// File: GeneralDocumentGenerator.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Anton Hagqvist
// Date: 12.4.2023
//
// Description:
// Base class for documentation writers
//-----------------------------------------------------------------------------

#ifndef DOCUMENTATIONWRITER_H
#define DOCUMENTATIONWRITER_H

#include <QTextStream>

class DocumentationWriter
{
public:
    DocumentationWriter() = default;
    virtual ~DocumentationWriter() = default;

    virtual void writeHeader(QTextStream& stream) = 0;
    virtual void writeTableOfContents(unsigned int& componentNumber, QTextStream& stream) = 0;
};

#endif // DOCUMENTATIONWRITER_H
