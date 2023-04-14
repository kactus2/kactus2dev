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

class ExpressionFormatter;

class HtmlWriter : public DocumentationWriter
{
public:
    HtmlWriter(QSharedPointer<Component> component);
    virtual ~HtmlWriter();
    void writeHeader(QTextStream& stream);
    void writeKactusAttributes(QTextStream& stream, int subHeaderNumber);
    void writeTableOfContents(QTextStream& stream);
    void writeParameters(QTextStream& stream, ExpressionFormatter* formatter,
        int subHeaderNumber);
    void writeSubHeader(unsigned int subHeaderNumber, QTextStream& stream,
        QString const& headerText, QString const& headerId);

    void setComponentNumber(unsigned int componentNumber);
private:

    // Returns n tabs for indenting HTML
    QString indent(int n) const;

    // The current component
    QSharedPointer<Component> component_;

    // Component number for table of contents
    unsigned int componentNumber_;
};

#endif // HTMLWRITER_H