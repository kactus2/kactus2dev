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

#include <IPXACTmodels/Component/Component.h>

#include <kactusGenerators/DocumentGenerator/DocumentationWriter.h>

#include <QTextStream>
#include <QSharedPointer>

class MarkdownWriter : public DocumentationWriter
{
public:
    MarkdownWriter(QSharedPointer<Component> component);
    virtual ~MarkdownWriter();
    void writeHeader(QTextStream& stream);
    void writeTableOfContents(unsigned int& componentNumber, QTextStream& stream);
private:
    // The current component
    QSharedPointer<Component> component_;
};

#endif // MARKDOWNWRITER_H