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
#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/Component/AddressBlock.h>

#include <kactusGenerators/DocumentGenerator/DocumentationWriter.h>

#include <QTextStream>
#include <QSharedPointer>
#include <QList>

class MarkdownWriter : public DocumentationWriter
{
public:
    MarkdownWriter(QSharedPointer<Component> component);
    virtual ~MarkdownWriter();
    void writeHeader(QTextStream& stream);
    void writeKactusAttributes(QTextStream& stream, int subHeaderNumber);
    void writeTableOfContents(QTextStream& stream);
    void writeParameters(QTextStream& stream, ExpressionFormatter* formatter,
        int subHeaderNumber);
    void writeSubHeader(unsigned int subHeaderNumber, QTextStream& stream,
        QString const& headerText, QString const& headerId);
    virtual void writeMemoryMaps(QTextStream& stream, int subHeaderNumber);

    void setComponentNumber(unsigned int componentNumber);

private:

    // Finds the address blocks of the memory map
    QList<QSharedPointer <AddressBlock> > getMemoryMapAddressBlocks(QSharedPointer<MemoryMap> memoryMap) const;

    void writeTableLine(QTextStream& stream, QStringList const& cells) const;
    
    // The current component
    QSharedPointer<Component> component_;

    // Component number in the hierarchy
    unsigned int componentNumber_;
};

#endif // MARKDOWNWRITER_H