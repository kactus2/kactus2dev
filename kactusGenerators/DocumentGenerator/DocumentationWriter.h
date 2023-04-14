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

class ExpressionFormatter;

class DocumentationWriter
{
public:
    DocumentationWriter() = default;
    virtual ~DocumentationWriter() = default;

    virtual void writeHeader(QTextStream& stream) = 0;
    virtual void writeKactusAttributes(QTextStream& stream, int subHeaderNumber) = 0;
    virtual void writeTableOfContents(QTextStream& stream) = 0;
    virtual void writeParameters(QTextStream& stream, ExpressionFormatter* formatter,
        int subHeaderNumber) = 0;
    virtual void writeSubHeader(unsigned int subHeaderNumber, QTextStream& stream,
        QString const& headerText, QString const& headerId) = 0;
    
    virtual void setComponentNumber(unsigned int componentNumber) = 0;
};

#endif // DOCUMENTATIONWRITER_H
