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
class AddressBlock;
class MemoryMap;
class Register;
class Field;

class HtmlWriter : public DocumentationWriter
{
public:
    HtmlWriter(QSharedPointer<Component> component, ExpressionFormatter* formatter);

    ~HtmlWriter() override;

    void writeHeader(QTextStream& stream) override;

    void writeKactusAttributes(QTextStream& stream, int subHeaderNumber) override;

    void writeTableOfContents(QTextStream& stream) override;

    void writeParameters(QTextStream& stream, int subHeaderNumber) override;

    void writeSubHeader(unsigned int subHeaderNumber, QTextStream& stream,
        QString const& headerText, QString const& headerId) override;

    void writeMemoryMaps(QTextStream& stream, int subHeaderNumber) override;

    void writeAddressBlocks(QTextStream& stream, QList<QSharedPointer <AddressBlock> > addressBlocks,
        int subHeaderNumber, int memoryMapNumber) override;

    void writeRegisters(QTextStream& stream, QList<QSharedPointer <Register> > registers,
        int subHeaderNumber, int memoryMapNumber, int addressBlockNumber) override;

    void writeFields(QTextStream& stream, QSharedPointer <Register> currentRegister) override;

    void writePorts(QTextStream& stream, int subHeaderNumber) override;

    void setComponentNumber(unsigned int componentNumber) override;

private:
    
    // TODO: Move these "finder" methods to DocumentationWriter
    // Finds the address blocks of the memory map
    QList<QSharedPointer <AddressBlock> > getMemoryMapAddressBlocks(QSharedPointer<MemoryMap> memoryMap) const;

    // Finds the registers of an address block
    QList<QSharedPointer <Register> > getAddressBlockRegisters(QSharedPointer<AddressBlock> addressBlock) const;

    // Get the information for the reset values of the selected field.
    QString getFieldResetInfo(QSharedPointer<Field> field) const;

    // Returns n tabs for indenting HTML
    QString indent(int n) const;

    // Writes a table row with chosen indentation. Indentation is the number of tabs for
    // the parent element.
    void writeTableRow(QTextStream& stream, QStringList const& fields, int indentation);

    // Writes a table header row with chosen indentation. Indentation is the number of tabs for
    // the parent element.
    void writeTableHeader(QTextStream& stream, QStringList const& headerFields, int indentation);

    //! The expression formatter, used to change parameter IDs into names.
    ExpressionFormatter* expressionFormatter_;

    // The current component
    QSharedPointer<Component> component_;

    // Stringified VLNV of the component
    QString vlnvString_;

    // Component number for table of contents
    unsigned int componentNumber_;
};

#endif // HTMLWRITER_H