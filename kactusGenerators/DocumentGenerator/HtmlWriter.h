//-----------------------------------------------------------------------------
// File: GeneralDocumentGenerator.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Anton Hagqvist
// Date: 12.4.2023
//
// Description:
// Writes HTML documentation
//-----------------------------------------------------------------------------

#ifndef HTMLWRITER_H
#define HTMLWRITER_H

#include <IPXACTmodels/Component/Component.h>

#include <kactusGenerators/DocumentGenerator/DocumentationWriter.h>

#include <QTextStream>
#include <QSharedPointer>

class HtmlWriter : public DocumentationWriter
{
public:
    HtmlWriter(QSharedPointer<Component> component);
    virtual ~HtmlWriter();
    void writeHeader(QTextStream& stream);
    void writeTableOfContents(unsigned int& componentNumber, QTextStream& stream);
    //void writeParameters();
private:

    // The current component
    QSharedPointer<Component> component_;
};

#endif // HTMLWRITER_H