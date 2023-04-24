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
class LibraryInterface;
class ComponentInstantiation;
class ListParameterFinder;

class MarkdownWriter : public DocumentationWriter
{
public:
    MarkdownWriter(QSharedPointer<Component> component, ExpressionFormatter* formatter,
        LibraryInterface* libraryHandler, int componentNumber);

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

    // Writes a component subheader for linked subheaders
    void writeSubHeader(QTextStream& stream, int subHeaderNumber,
        QString const& title, QString const& headerId) const override;

    // Writes a subheader of specified level and numbering for non-linked subheaders
    void writeSubHeader(QTextStream& stream, QList<int> const& subHeaderNumbers,
        QString const& title, int level) const override;

    void writeDescription(QTextStream& stream, QString const& description) override;
    
    void writeErrorMessage(QTextStream& stream, QString const& message) override;

    void writeReferencedComponentInstantiation(
        QTextStream& stream,
        QSharedPointer<ComponentInstantiation> instantiation,
        QSharedPointer<ExpressionFormatter> instantiationFormatter,
        QSharedPointer<QList<QSharedPointer<Parameter> > > moduleParameters,
        QSharedPointer<QList<QSharedPointer<Parameter> > > parameters) override;

    void writeReferencedDesignConfigurationInstantiation(QTextStream& stream,
        QSharedPointer<ListParameterFinder> configurationFinder,
        QSharedPointer<DesignConfigurationInstantiation> instantiation,
        QSharedPointer<ExpressionFormatter> instantiationFormatter) override;

    void writeReferencedDesignInstantiation(QTextStream& stream,
        QSharedPointer<ConfigurableVLNVReference> designVLNV, QSharedPointer<Design> instantiatedDesign,
        ExpressionFormatter* designFormatter, QSharedPointer<ExpressionFormatter> instantiationFormatter)
        override;

    void writeDocumentReference(QTextStream& stream, QString const& documentType,
        QSharedPointer<ConfigurableVLNVReference> vlnvReference) override;

private:


    // Writes a line with specified cells to a MD table
    void writeTableRow(QTextStream& stream, QStringList const& cells) const;

    // Writes the table separator to a MD table
    void writeTableSeparator(QTextStream& stream, int columns) const;

    // Writes the table header and separator lines
    void writeTableHeader(QTextStream& stream, QStringList const& headers) const;

    // Writes a port table for specified ports
    void writePortTable(QTextStream& stream, QList<QSharedPointer<Port> > ports) const;

    void writeFileSetGroupdIdentifiers(QTextStream& stream, QSharedPointer<FileSet> fileSet) const;

    void writeDefaultFileBuilders(QTextStream& stream, QSharedPointer<FileSet> fileSet) const;

    void writeFiles(QTextStream& stream, QSharedPointer<FileSet> fileSet,
        int subHeaderNumber, int fileSetNumber);

    void writeSingleFile(QTextStream& stream, QSharedPointer<File> file);

    void writeImplementationDetails(QTextStream& stream, QSharedPointer<ComponentInstantiation> instantiation);

    void writeFileSetReferences(QTextStream& stream, QSharedPointer<ComponentInstantiation> instantiation);

    void writeFileBuildCommands(QTextStream& stream, QSharedPointer<ComponentInstantiation> instantiation,
        ExpressionFormatter* formatter);

    // Writes a parameter table for given parameters
    void writeParameterTable(QTextStream& stream, QString const& tableHeading,
        QSharedPointer<QList<QSharedPointer<Parameter> > > parameters,
        ExpressionFormatter* formatter);

    void writeConfigurableElementValues(QTextStream& stream,
        QSharedPointer<ConfigurableVLNVReference> vlnvReference,
        ExpressionFormatter* instantiationFormatter);

    //! The expression formatter, used to change parameter IDs into names.
    ExpressionFormatter* expressionFormatter_;

    // The library handler
    LibraryInterface* libraryHandler_;

    // Stringified VLNV of the component
    QString vlnvString_;
    
    // The current component
    QSharedPointer<Component> component_;

    // Component number in the hierarchy
    int componentNumber_;
};

#endif // MARKDOWNWRITER_H