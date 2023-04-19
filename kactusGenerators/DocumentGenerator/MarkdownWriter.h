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
#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/Field.h>

#include <kactusGenerators/DocumentGenerator/DocumentationWriter.h>

#include <QTextStream>
#include <QSharedPointer>
#include <QList>

class ExpressionFormatter;
class Port;

class MarkdownWriter : public DocumentationWriter
{
public:
    MarkdownWriter(QSharedPointer<Component> component, ExpressionFormatter* formatter);

    ~MarkdownWriter() override;

    void writeHeader(QTextStream& stream) override;

    void writeKactusAttributes(QTextStream& stream, int subHeaderNumber) override;

    void writeTableOfContents(QTextStream& stream) override;

    void writeParameters(QTextStream& stream, int subHeaderNumber) override;

    void writeMemoryMaps(QTextStream& stream, int subHeaderNumber) override;

    void writeAddressBlocks(QTextStream& stream, QList<QSharedPointer <AddressBlock> > addressBlocks,
        int subHeaderNumber,int memoryMapNumber) override;

    void writeRegisters(QTextStream& stream, QList<QSharedPointer <Register> > registers,
        int subHeaderNumber, int memoryMapNumber, int addressBlockNumber) override;

    void writeFields(QTextStream& stream, QSharedPointer <Register> currentRegister) override;

    void writePorts(QTextStream& stream, int subHeaderNumber) override;

    void writeInterfaces(QTextStream& stream, int& subHeaderNumber) override;

    void writeFileSets(QTextStream& stream, int& subHeaderNumber) override;

    void setComponentNumber(int componentNumber) override;

private:

    // TODO: Move these "finder" methods to DocumentationWriter
    // Finds the address blocks of a memory map
    QList<QSharedPointer <AddressBlock> > getMemoryMapAddressBlocks(QSharedPointer<MemoryMap> memoryMap) const;

    // Finds the registers of an address block
    QList<QSharedPointer <Register> > getAddressBlockRegisters(QSharedPointer<AddressBlock> addressBlock) const;

    // Get the information for the reset values of the selected field.
    QString getFieldResetInfo(QSharedPointer<Field> field) const;

    // Writes a subheader of specified level and numbering for non-linked subheaders
    void writeSubHeader(QTextStream& stream, QList<int> const& subHeaderNumbers,
        QString const& title, int level) const;

    // Writes a component subheader for linked subheaders
    void writeSubHeader(QTextStream& stream, int subHeaderNumber,
        QString const& title, QString const& headerId) const;

    // Writes a line with specified cells to a MD table
    void writeTableRow(QTextStream& stream, QStringList const& cells) const;

    // Writes the table separator to a MD table
    void writeTableSeparator(QTextStream& stream, int columns) const;

    // Writes the table header and separator lines
    void writeTableHeader(QTextStream& stream, QStringList const& headers) const;

    // Writes a port table for specified ports
    void writePortTable(QTextStream& stream, QList<QSharedPointer<Port> > ports) const;

    // Writes a **Description:** <text> line
    void writeDescription(QTextStream& stream, QString const& description) const;

    //! The expression formatter, used to change parameter IDs into names.
    ExpressionFormatter* expressionFormatter_;

    // Stringified VLNV of the component
    QString vlnvString_;
    
    // The current component
    QSharedPointer<Component> component_;

    // Component number in the hierarchy
    int componentNumber_;
};

#endif // MARKDOWNWRITER_H