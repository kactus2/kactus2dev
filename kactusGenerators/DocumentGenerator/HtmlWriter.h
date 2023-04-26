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
#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>


#include <kactusGenerators/DocumentGenerator/DocumentationWriter.h>


#include <QTextStream>
#include <QSharedPointer>

class ExpressionFormatter;
class LibraryInterface;
class AddressBlock;
class MemoryMap;
class Register;
class Field;
class ListParameterFinder;
class ComponentInstantiation;

class HtmlWriter : public DocumentationWriter
{
public:
    HtmlWriter(QSharedPointer<Component> component, ExpressionFormatter* formatter,
        ExpressionFormatterFactory* expressionFormatterFactory,
        LibraryInterface* libraryHhandler, int componentNumber);

    ~HtmlWriter() override;

    void writeHeader(QTextStream& stream) override;

    void writeComponentHeader(QTextStream& stream) override;

    void writeComponentInfo(QTextStream& stream) override;

    void writeKactusAttributes(QTextStream& stream, int subHeaderNumber) override;

    void writeTableOfContentsHeader(QTextStream& stream) override;

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

    // Writes a component subheader for linked subheaders
    void writeSubHeader(QTextStream& stream, int subHeaderNumber,
        QString const& headerText, QString const& headerId) const override;

    // Writes a subheader of specified level and numbering for non-linked subheaders
    void writeSubHeader(QTextStream& stream, QList<int> const& subHeaderNumbers,
        QString const& title, int level) const override;

    void writeDescription(QTextStream& stream, QString const& description) override;
    
    void writeReferencedComponentInstantiation(
        QTextStream& stream,
        QSharedPointer<ComponentInstantiation> instantiation,
        QSharedPointer<ExpressionFormatter> instantiationFormatter,
        QSharedPointer<QList<QSharedPointer<Parameter>>> moduleParameters,
        QSharedPointer<QList<QSharedPointer<Parameter>>> parameters) override;

    void writeReferencedDesignConfigurationInstantiation(QTextStream& stream,
        QSharedPointer<ListParameterFinder> configurationFinder,
        QSharedPointer<DesignConfigurationInstantiation> instantiation,
        QSharedPointer<ExpressionFormatter> instantiationFormatter) override;

    void writeReferencedDesignInstantiation(QTextStream& stream,
        QSharedPointer<ConfigurableVLNVReference> designVLNV, QSharedPointer<Design> instantiatedDesign,
        ExpressionFormatter* designFormatter, QSharedPointer<ExpressionFormatter> instantiationFormatter)
        override;

    void writeErrorMessage(QTextStream& stream, QString const& message) override;

    void writeDocumentReference(QTextStream& stream, QString const& documentType,
        QSharedPointer<ConfigurableVLNVReference> vlnvReference) override;

    void writeDiagram(QTextStream& stream, QString const& title, QString const& link, QString const& altText)
        override;

    void writeDesignInstances(QTextStream& stream, QSharedPointer<Design> design,
        QSharedPointer<DesignConfiguration> configuration) override;

private:
    // Returns n tabs for indenting HTML
    QString indent(int n) const;

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

    void writeImplementationDetails(QTextStream& stream, QSharedPointer<ComponentInstantiation> instantiation);

    void writeFileSetReferences(QTextStream& stream, QSharedPointer<ComponentInstantiation> instantiation);

    void writeFileBuildCommands(QTextStream& stream, QSharedPointer<ComponentInstantiation> instantiation,
        ExpressionFormatter* instantiationFormatter);

    void writeParameterTable(QTextStream& stream, QString const& title,
        QSharedPointer<QList<QSharedPointer<Parameter> > > parameters,
        ExpressionFormatter* formatter);

    void writeConfigurableElementValues(QTextStream& stream,
        QSharedPointer<ConfigurableVLNVReference> vlnvReference,
        ExpressionFormatter* instantiationFormatter);

    QString getComponentInstanceConfigurableElements(QSharedPointer<ComponentInstance> instance,
        QSharedPointer<Design> design);

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