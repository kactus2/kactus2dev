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
class LibraryInterface;
class AddressBlock;
class MemoryMap;
class Register;
class Field;

class HtmlWriter : public DocumentationWriter
{
public:
    HtmlWriter(QSharedPointer<Component> component, ExpressionFormatter* formatter, LibraryInterface* libraryHhandler);

    ~HtmlWriter() override;

    void writeHeader(QTextStream& stream) override;

    void writeKactusAttributes(QTextStream& stream, int subHeaderNumber) override;

    void writeTableOfContents(QTextStream& stream) override;

    void writeParameters(QTextStream& stream, int subHeaderNumber) override;

    void writeMemoryMaps(QTextStream& stream, int subHeaderNumber) override;

    void writeAddressBlocks(QTextStream& stream, QList<QSharedPointer <AddressBlock> > addressBlocks,
        int subHeaderNumber, int memoryMapNumber) override;

    void writeRegisters(QTextStream& stream, QList<QSharedPointer <Register> > registers,
        int subHeaderNumber, int memoryMapNumber, int addressBlockNumber) override;

    void writeFields(QTextStream& stream, QSharedPointer <Register> currentRegister) override;

    void writePorts(QTextStream& stream, int subHeaderNumber) override;

    void writeInterfaces(QTextStream& stream, int& subHeaderNumber) override;

    void writeFileSets(QTextStream& stream, int& subHeaderNumber) override;

    void setComponentNumber(int componentNumber) override;

private:
    // Returns n tabs for indenting HTML
    QString indent(int n) const;

    // Writes a subheader of specified level and numbering for non-linked subheaders
    void writeSubHeader(QTextStream& stream, QList<int> const& subHeaderNumbers,
        QString const& title, int level) const;

    // Writes a component subheader for linked subheaders
    void writeSubHeader(QTextStream& stream, int subHeaderNumber,
        QString const& headerText, QString const& headerId) const;

    // Writes a table row with chosen indentation. Indentation is the number of tabs for
    // the parent element.
    void writeTableRow(QTextStream& stream, QStringList const& fields, int indentation);

    // Writes a table header row with chosen indentation. Indentation is the number of tabs for
    // the parent element.
    void writeTableHeader(QTextStream& stream, QStringList const& headerFields, int indentation);

    // Writes a port table for specified ports
    void writePortTable(QTextStream& stream, QString const& tableTitle, QList<QSharedPointer<Port> > ports);
    
    void writeFileSetGroupdIdentifiers(QTextStream& stream, QSharedPointer<FileSet> fileSet) const;

    void writeDefaultFileBuilders(QTextStream& stream, QSharedPointer<FileSet> fileSet);

    void writeFiles(QTextStream& stream, QSharedPointer<FileSet> fileSet,
        int subHeaderNumber, int fileSetNumber);

    void writeSingleFile(QTextStream& stream, QSharedPointer<File> file);

    //! The expression formatter, used to change parameter IDs into names.
    ExpressionFormatter* expressionFormatter_;

    // The library handler
    LibraryInterface* libraryHandler_;

    // The current component
    QSharedPointer<Component> component_;

    // Stringified VLNV of the component
    QString vlnvString_;

    // Component number for table of contents
    int componentNumber_;
};

#endif // HTMLWRITER_H