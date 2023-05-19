//-----------------------------------------------------------------------------
// File: MarkdownWriter.h
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

#include <IPXACTmodels/DesignConfiguration/DesignConfiguration.h>

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
        ExpressionFormatterFactory* expressionFormatterFactory,
        LibraryInterface* libraryHhandler, int componentNumber);

    ~MarkdownWriter() override;

    /*!
     *  Write the document header.
     *
     *      @param [in] stream  The text stream to write the header into.
     */
    void writeHeader(QTextStream& stream) override;

    /*!
     *  Write a header for the component.
     *
     *      @param [in] stream  The text stream to write the header into.
     */
    void writeComponentHeader(QTextStream& stream) override;

    /*!
     *  Write the component info.
     *
     *      @param [in] stream  The text stream to write into.
     */
    void writeComponentInfo(QTextStream& stream) override;

    /*!
     *  Write component kactus2 attributes.
     *
     *      @param [in] stream              The text stream to write into.
     *      @param [in] subHeaderNumber     The current subheader number.
     */
    void writeKactusAttributes(QTextStream& stream, int subHeaderNumber) override;

    /*!
     *  Write the table of contents header.
     *
     *      @param [in] stream  The text stream to write into.
     */
    void writeTableOfContentsHeader(QTextStream& stream) override;

    /*!
     *  Write the table of contents of the current component.
     *
     *      @param [in] stream  The text stream to write into.
     */
    void writeTableOfContents(QTextStream& stream) override;

    /*!
     *  Write the parameters of the component
     *
     *      @param [in] stream              The text stream to write into.
     *      @param [in] subHeaderNumber     The current subheader number.
     */
    void writeParameters(QTextStream& stream, int subHeaderNumber) override;

    /*!
     *  Write the memory maps of the component
     *
     *      @param [in] stream              The text stream to write into.
     *      @param [in] subHeaderNumber     The current subheader number.
     */
    void writeMemoryMaps(QTextStream& stream, int subHeaderNumber) override;

    /*!
     *  Write the given address blocks
     *
     *      @param [in] stream              The text stream to write into.
     *      @param [in] addressBlocks       The address blocks to be written.
     *      @param [in] subHeaderNumber     The current subheader number.
     *      @param [in] memoryMapNumber     The current memory map number.
     */
    void writeAddressBlocks(QTextStream& stream, QList<QSharedPointer <AddressBlock> > addressBlocks,
        int subHeaderNumber,int memoryMapNumber) override;

    /*!
     *  Write the given register files
     *
     *      @param [in] stream               The text stream to write into.
     *      @param [in] registerFiles        The registers to be written.
     *      @param [in] subHeaderNumbers     The current subheader number.
     *      @param [out] registerDataNumber  The current address block number.
     */
    void writeRegisterFiles(QTextStream& stream,
        QList<QSharedPointer<RegisterFile> > registerFiles,
        QList<int> subHeaderNumbers, int& registerDataNumber) override;

    /*!
     *  Write the given registers
     *
     *      @param [in] stream              The text stream to write into.
     *      @param [in] registers           The registers to be written.
     *      @param [in] subHeaderNumber     The current subheader number.
     *      @param [in] memoryMapNumber     The current memory map number.
     *      @param [in] addressBlockNumber  The current address block number.
     */
    void writeRegisters(QTextStream& stream, QList<QSharedPointer <Register> > registers,
        int subHeaderNumber, int memoryMapNumber, int addressBlockNumber, int& registerDataNumber) override;

    /*!
     *  Write the given register fields
     *
     *      @param [in] stream                     The text stream to write into.
     *      @param [in] register                   The register whose fields are to be written.
     *      @param [in] registerSubHeaderNumbers   The parent register's subheader numbering.
     */
    void writeFields(QTextStream& stream, QSharedPointer <Register> currentRegister,
        QList<int> registerSubHeaderNumbers) override;

    /*!
     *  Write the ports of the component
     *
     *      @param [in] stream              The text stream to write into.
     *      @param [in] subHeaderNumber     The current subheader number.
     */
    void writePorts(QTextStream& stream, int subHeaderNumber) override;

    /*!
     *  Write the bus interfaces of the component
     *
     *      @param [in] stream              The text stream to write into.
     *      @param [in] subHeaderNumber     The current subheader number.
     */
    void writeInterfaces(QTextStream& stream, int& subHeaderNumber) override;

    /*!
     *  Write the file sets of the component
     *
     *      @param [in] stream              The text stream to write into.
     *      @param [in] subHeaderNumber     The current subheader number.
     */
    void writeFileSets(QTextStream& stream, int& subHeaderNumber) override;

    /*!
     *  Set the number of the component in the hierarchy
     *
     *      @param [in] componentNumber     The new component number.
     */
    void setComponentNumber(int componentNumber) override;

    /*!
     *  Write a referenced component subheader
     *
     *      @param [in] stream              The text stream to write into.
     *      @param [in] subHeaderNumber     The current subheader number.
     *      @param [in] headerText          The displayed header text.
     *      @param [in] headerId            The id for referencing within the document.
     */
    void writeSubHeader(QTextStream& stream, int subHeaderNumber,
        QString const& title, QString const& headerId) const override;

    /*!
     *  Write a non-referenced component subheader with specified header level
     *
     *      @param [in] stream              The text stream to write into.
     *      @param [in] subHeaderNumbers    The subheader numbers.
     *      @param [in] title               The displayed header text.
     *      @param [in] headerId            The id for referencing within the document.
     */
    void writeSubHeader(QTextStream& stream, QList<int> const& subHeaderNumbers,
        QString const& title, int level) const override;

    /*!
     *  Write a description text
     *
     *      @param [in] stream              The text stream to write into.
     *      @param [in] description         The description to write.
     */
    void writeDescription(QTextStream& stream, QString const& description) override;

    /*!
     *  Write a paragraph in which specific information is listed.
     *
     *      @param [in] stream              The text stream to write into.
     *      @param [in] names               The info item names.
     *      @param [in] values              The info item values.
     */
    void writeInfoParagraph(QTextStream& stream, QStringList const& names, QStringList const& values) override;

    /*!
     *  Write an error text to the document.
     *
     *      @param [in] stream              The text stream to write into..
     *      @param [in] message             The description to write.
     */
    void writeErrorMessage(QTextStream& stream, QString const& message) override;

    /*!
     *  Write the document contained within the selected VLNV.
     *
     *      @param [in] stream                  Text stream to write the document.
     *      @param [in] documentType            Type of the document.
     *      @param [in] vlnvReference           VLNV of the selected document.
     */
    void writeDocumentReference(QTextStream& stream, QString const& documentType,
        QSharedPointer<ConfigurableVLNVReference> vlnvReference) override;

    /*!
     *  Write the design diagram.
     *
     *      @param [in] stream                  Text stream to write the diagram to.
     *      @param [in] title                   Diagram title.
     *      @param [in] link                    Diagram picture link or path.
     *      @param [in] altText                 Diagram alt text.
     */
    void writeDiagram(QTextStream& stream, QString const& title, QString const& link, QString const& altText)
        override;

    /*!
     *  Write the component instances contained within the selected design.
     *
     *      @param [in] stream          Text stream to write the component instances.
     *      @param [in] design          The selected design.
     *      @param [in] configuration   The selected design configuration.
     */
    void writeDesignInstances(QTextStream& stream, QSharedPointer<Design> design,
        QSharedPointer<DesignConfiguration> configuration) override;

    /*!
     *  Empty function for markdown writing.
     *
     *      @param [in] stream  The text stream to write the documentation into.
     */
    void writeEndOfDocument(QTextStream& stream) override;

private:

    /*!
     *  Writes a table of registers.
     *
     *      @param [in] stream              The text stream to write into.
     *      @param [in] registers           The registers to be written.
     */
    void writeRegisterTable(QTextStream& stream, QList<QSharedPointer<Register> >registers) const;

    /*!
     *  Writes a table of register fields.
     *
     *      @param [in] stream       The text stream to write into.
     *      @param [in] reg          The register whose fields are written.
     */
    void writeFieldTable(QTextStream& stream, QSharedPointer<Register> reg) const;

    /*!
     *  Writes the enumerations of a field.
     *
     *      @param [in] stream              The text stream to write into.
     *      @param [in] field               The field which enumerations are written.
     */
    void writeFieldEnumerations(QTextStream& stream, QSharedPointer<Field> field) override;

    /*!
     *  Writes a table row with specified cells to a MD table.
     *
     *      @param [in] stream          Text stream to write to.
     *      @param [in] cells           Table row cells.
     */
    void writeTableRow(QTextStream& stream, QStringList const& cells) const;

    /*!
     *  Writes a table separator to a MD table.
     *
     *      @param [in] stream          Text stream to write to.
     *      @param [in] columns         Number of columns.
     */
    void writeTableSeparator(QTextStream& stream, int columns) const;

    /*!
     *  Writes the table header cells and the separator.
     *
     *      @param [in] stream          Text stream to write to.
     *      @param [in] headers         The table headers.
     */
    void writeTableHeader(QTextStream& stream, QStringList const& headers) const;

    /*!
     *  Writes a port table for specified ports.
     *
     *      @param [in] stream          Text stream to write to.
     *      @param [in] ports           The ports to be written.
     */
    void writePortTable(QTextStream& stream, QList<QSharedPointer<Port> > ports) const;

    /*!
     *  Writes group identifiers for a file set.
     *
     *      @param [in] stream          Text stream to write to.
     *      @param [in] fileSet         File set to write identifiers for.
     */
    void writeFileSetGroupdIdentifiers(QTextStream& stream, QSharedPointer<FileSet> fileSet) const;

    /*!
     *  Writes the default file builders for a file set.
     *
     *      @param [in] stream          Text stream to write to.
     *      @param [in] fileSet         File set to write file builders for.
     */
    void writeDefaultFileBuilders(QTextStream& stream, QSharedPointer<FileSet> fileSet) const;

    /*!
     *  Writes the files of a file set.
     *
     *      @param [in] stream          Text stream to write to.
     *      @param [in] fileSet         File set whose files will be written.
     *      @param [in] subHeaderNumber The current subheader number.
     *      @param [in] fileSetNumber   The current file set number.
     */
    void writeFiles(QTextStream& stream, QSharedPointer<FileSet> fileSet,
        int subHeaderNumber, int fileSetNumber);

    /*!
     *  Writes a specified file of a file set.
     *
     *      @param [in] stream          Text stream to write to.
     *      @param [in] file            File to be written.
     */
    void writeSingleFile(QTextStream& stream, QSharedPointer<File> file) const;

    /*!
     *  Writes the implementation details of a component instantiation.
     *
     *      @param [in] stream          Text stream to write to.
     *      @param [in] instantiation   The component instantiation.
     */
    void writeImplementationDetails(QTextStream& stream,
        QSharedPointer<ComponentInstantiation> instantiation) override;
    
    /*!
     *  Writes the file set references contained within a component instantiation.
     *
     *      @param [in] stream          Text stream to write to.
     *      @param [in] instantiation   The component instantiation.
     */
    void writeFileSetReferences(QTextStream& stream,
        QSharedPointer<ComponentInstantiation> instantiation) override;

    /*!
     *  Writes the file build commands of a component instantiation.
     *
     *      @param [in] stream          Text stream to write to.
     *      @param [in] instantiation   The component instantiation.
     *      @param [in] formatter       The expression formatter for the component instantiation.
     */
    void writeFileBuildCommands(QTextStream& stream, QSharedPointer<ComponentInstantiation> instantiation,
        QSharedPointer<ExpressionFormatter> formatter) override;

    /*!
     *  Writes given parameters to a table.
     *
     *      @param [in] stream          Text stream to write to.
     *      @param [in] tableHeading    The heading above the table
     *      @param [in] parameters      The parameters to be written.
     *      @param [in] formatter       The expression formatter for the parameters.
     */
    void writeParameterTable(QTextStream& stream, QString const& tableHeading,
        QSharedPointer<QList<QSharedPointer<Parameter> > > parameters,
        QSharedPointer<ExpressionFormatter> formatter) override;

    /*!
     *  Writes given module parameters to a table.
     *
     *      @param [in] stream              Text stream to write to.
     *      @param [in] tableHeading        The heading above the table.
     *      @param [in] moduleParameters    The module parameters to be written.
     *      @param [in] formatter           The expression formatter for the parameters.
     */
    void writeModuleParameterTable(QTextStream& stream, QString const& tableHeading,
        QSharedPointer<QList<QSharedPointer<Parameter> > > moduleParameters,
        QSharedPointer<ExpressionFormatter> formatter) override;

    /*!
     *  Writes the configurable element values of a VLNV.
     *
     *      @param [in] stream                  Text stream to write the document.
     *      @param [in] vlnvReference           Configurable VLNV containing the selected configurable element
     *                                          values.
     *      @param [in] instantiationFormatter  Expression formatter for the configurable element values.
     */
    void writeConfigurableElementValues(QTextStream& stream,
        QSharedPointer<ConfigurableVLNVReference> vlnvReference,
        QSharedPointer<ExpressionFormatter> instantiationFormatter) override;

    /*!
     *  Gets the configurable element values of a component instance as a line-broken string.
     * 
     *      @param [in] stream          Text stream to write the document.
     *      @param [in] instance        The component instance.        
     *      @param [in] design          The design in which the component instance is.      
     */
    QString getComponentInstanceConfigurableElements(QSharedPointer<ComponentInstance> instance,
        QSharedPointer<Design> design);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The expression formatter, used to change parameter IDs into names.
    ExpressionFormatter* expressionFormatter_;

    //! The library handler
    LibraryInterface* libraryHandler_;

    //! Stringified VLNV of the component
    QString vlnvString_;
    
    //! The current component
    QSharedPointer<Component> component_;

    //! Component number in the hierarchy
    int componentNumber_;
};

#endif // MARKDOWNWRITER_H