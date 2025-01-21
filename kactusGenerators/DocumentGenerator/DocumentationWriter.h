//-----------------------------------------------------------------------------
// File: DocumentationWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Anton Hagqvist
// Date: 12.4.2023
//
// Description:
// Base class for documentation writers
//-----------------------------------------------------------------------------

#ifndef DOCUMENTATIONWRITER_H
#define DOCUMENTATIONWRITER_H

#include <KactusAPI/include/ExpressionFormatterFactory.h>

#include <IPXACTmodels/DesignConfiguration/DesignConfiguration.h>

#include <QTextStream>
#include <QString>
#include <QSharedPointer>

class ExpressionFormatter;
class AddressBlock;
class Register;
class Design;
class Field;
class MemoryMap;
class Parameter;
class ComponentInstantiation;
class ListParameterFinder;
class DesignConfigurationInstantiation;
class ConfigurableVLNVReference;
class RegisterFile;
class RegisterBase;

using ParameterList = QSharedPointer<QList<QSharedPointer<Parameter> > >;

class DocumentationWriter
{
public:

    //! Table header constants
    static const QStringList ADDRESS_BLOCK_HEADERS;

    static const QStringList DEFAULT_FILE_BUILDER_HEADERS;

    static const QStringList FIELD_HEADERS;
    
    static const QStringList FILE_HEADERS;

    static const QStringList PARAMETER_HEADERS;

    static const QStringList MODULE_PARAMETER_HEADERS;

    static const QStringList PORT_HEADERS;

    static const QStringList REGISTER_HEADERS;

    static const QStringList REGISTER_FILE_HEADERS;

    static const QStringList DESIGN_INSTANCE_HEADERS;
    
    DocumentationWriter(ExpressionFormatter* formatter, ExpressionFormatterFactory* expressionFormatterFactory);
    virtual ~DocumentationWriter() = default;

    /*!
     *  Write the document header.
     *
     *    @param [in] stream  The text stream to write the header into.
     */
    virtual void writeHeader(QTextStream& stream) = 0;
    
    /*!
     *  Write a header for the component.
     *
     *    @param [in] stream  The text stream to write the header into.
     */
    virtual void writeComponentHeader(QTextStream& stream) = 0;
    
    /*!
     *  Write the component info.
     *
     *    @param [in] stream  The text stream to write into.
     */
    virtual void writeComponentInfo(QTextStream& stream) = 0;

    /*!
     *  Write component kactus2 attributes.
     *
     *    @param [in] stream              The text stream to write into.
     *    @param [in] subHeaderNumber     The current subheader number.
     */
    virtual void writeKactusAttributes(QTextStream& stream, int subHeaderNumber) = 0;

    /*!
     *  Write the table of contents header.
     *
     *    @param [in] stream  The text stream to write into.
     */
    virtual void writeTableOfContentsHeader(QTextStream& stream) = 0;

    /*!
     *  Write the table of contents of the current component.
     *
     *    @param [in] stream  The text stream to write into.
     */
    virtual void writeTableOfContents(QTextStream& stream) = 0;

    /*!
     *  Write the parameters of the component.
     *
     *    @param [in] stream              The text stream to write into.
     *    @param [in] subHeaderNumber     The current subheader number.
     */
    virtual void writeParameters(QTextStream& stream, int subHeaderNumber) = 0;

    /*!
     *  Write the memory maps of the component.
     *
     *    @param [in] stream              The text stream to write into.
     *    @param [in] subHeaderNumber     The current subheader number.
     */
    virtual void writeMemoryMaps(QTextStream& stream, int subHeaderNumber) = 0;

    /*!
     *  Write the given address blocks.
     *
     *    @param [in] stream              The text stream to write into.
     *    @param [in] addressBlocks       The address blocks to be written.
     *    @param [in] subHeaderNumber     The current subheader number.
     *    @param [in] memoryMapNumber     The current memory map number.
     */
    virtual void writeAddressBlocks(QTextStream& stream, QList<QSharedPointer <AddressBlock> > addressBlocks,
        int subHeaderNumber, int memoryMapNumber) = 0;
    
    /*!
     *  Write the given register files
     *
     *    @param [in] stream               The text stream to write into.
     *    @param [in] registerFiles        The registers to be written.
     *    @param [in] subHeaderNumbers     The current subheader number.
     *    @param [out] registerDataNumber  The current address block number.
     */
    virtual void writeRegisterFiles(QTextStream& stream,
        QList<QSharedPointer<RegisterFile> > registerFiles,
        QList<int> subHeaderNumbers, int& registerDataNumber) = 0;

    /*!
     *  Write the given registers.
     *
     *    @param [in] stream              The text stream to write into.
     *    @param [in] registers           The registers to be written.
     *    @param [in] subHeaderNumber     The current subheader number.
     *    @param [in] memoryMapNumber     The current memory map number.
     *    @param [in] addressBlockNumber  The current address block number.
     */
    virtual void writeRegisters(QTextStream& stream, QList<QSharedPointer <Register> > registers,
        int subHeaderNumber, int memoryMapNumber, int addressBlockNumber, int& registerDataNumber) = 0;

    /*!
     *  Write the given register fields.
     *
     *    @param [in] stream              The text stream to write into.
     *    @param [in] register            The register whose fields are to be written.
     */
    virtual void writeFields(QTextStream& stream, QSharedPointer <Register> currentRegister,
        QList<int> registerSubHeaderNumbers) = 0;

    /*!
     *  Write the ports of the component.
     *
     *    @param [in] stream              The text stream to write into.
     *    @param [in] subHeaderNumber     The current subheader number.
     */
    virtual void writePorts(QTextStream& stream, int subHeaderNumber) = 0;

    /*!
     *  Write the bus interfaces of the component.
     *
     *    @param [in] stream              The text stream to write into.
     *    @param [in] subHeaderNumber     The current subheader number.
     */
    virtual void writeInterfaces(QTextStream& stream, int& subHeaderNumber) = 0;

    /*!
     *  Write the file sets of the component.
     *
     *    @param [in] stream              The text stream to write into.
     *    @param [in] subHeaderNumber     The current subheader number.
     */
    virtual void writeFileSets(QTextStream& stream, int& subHeaderNumber) = 0;

    /*!
     *  Set the number of the component in the hierarchy.
     *
     *    @param [in] componentNumber     The new component number.
     */
    virtual void setComponentNumber(int componentNumber) = 0;

    /*!
     *  Write a referenced component subheader.
     *
     *    @param [in] stream              The text stream to write into.
     *    @param [in] subHeaderNumber     The current subheader number.
     *    @param [in] headerText          The displayed header text.
     *    @param [in] headerId            The id for referencing within the document.
     */
    virtual void writeSubHeader(QTextStream& stream, int subHeaderNumber,
        QString const& headerText, QString const& headerId) const = 0;
    
    /*!
     *  Write a non-referenced component subheader with specified header level.
     *
     *    @param [in] stream              The text stream to write into.
     *    @param [in] subHeaderNumbers    The subheader numbers.
     *    @param [in] title               The displayed header text.
     *    @param [in] headerId            The id for referencing within the document.
     */
    virtual void writeSubHeader(QTextStream& stream, QList<int> const& subHeaderNumbers,
        QString const& title, int level) const = 0;

    /*!
     *  Write a description text.
     *
     *    @param [in] stream              The text stream to write into.
     *    @param [in] description         The description to write.
     */
    virtual void writeDescription(QTextStream& stream, QString const& description) = 0;

    /*!
     *  Write an error text to the document.
     *
     *    @param [in] stream              The text stream to write into..
     *    @param [in] message             The description to write.
     */
    virtual void writeErrorMessage(QTextStream& stream, QString const& message) = 0;

    /*!
     *  Write the document contained within the selected VLNV.
     *
     *    @param [in] stream                  Text stream to write the document.
     *    @param [in] documentType            Type of the document.
     *    @param [in] vlnvReference           VLNV of the selected document.
     */
    virtual void writeDocumentReference(QTextStream& stream, QString const& documentType,
        QSharedPointer<ConfigurableVLNVReference> vlnvReference) = 0;

    /*!
     *  Write the design diagram.
     *
     *    @param [in] stream                  Text stream to write the diagram to.
     *    @param [in] title                   Diagram title.
     *    @param [in] link                    Diagram picture link or path.
     *    @param [in] altText                 Diagram alt text.
     */
    virtual void writeDiagram(QTextStream& stream, QString const& title,
        QString const& link, QString const& altText) = 0;

    /*!
     *  Write the component instances contained within the selected design.
     *
     *    @param [in] stream          Text stream to write the component instances.
     *    @param [in] design          The selected design.
     *    @param [in] configuration   The selected design configuration.
     */
    virtual void writeDesignInstances(QTextStream& stream, QSharedPointer<Design> design,
        QSharedPointer<DesignConfiguration> configuration) = 0;

    /*!
     *  Write the end of the document, if the format requires it.
     *
     *    @param [in] stream  The text stream to write the documentation into.
     */
    virtual void writeEndOfDocument(QTextStream& stream) = 0;

    /*!
     *  Write the referenced component instantiation.
     *
     *    @param [in] stream                  Text stream to write the component instantiation.
     *    @param [in] instantiation           The component instantiation.
     *    @param [in] instantiationFormatter  The component instantiation formatter.
     *    @param [in] moduleParameters        The instantiation module parameters.
     *    @param [in] parameters              The instantiation parameters.
     */
    void writeReferencedComponentInstantiation(
        QTextStream& stream,
        QSharedPointer<ComponentInstantiation> instantiation,
        QSharedPointer<ExpressionFormatter> instantiationFormatter,
        ParameterList moduleParameters,
        ParameterList parameters);

    /*!
     *  Write the referenced design configuration instantiation.
     *
     *    @param [in] stream                  Text stream to write the component instantiation.
     *    @param [in] configurationFinder     The parameter finder for the instantiation.
     *    @param [in] instantiationFormatter  The instantiation formatter.
     */
    void writeReferencedDesignConfigurationInstantiation(QTextStream& stream,
        QSharedPointer<ListParameterFinder> configurationFinder,
        QSharedPointer<DesignConfigurationInstantiation> instantiation,
        QSharedPointer<ExpressionFormatter> instantiationFormatter,
        LibraryInterface* libraryHandler);

    /*!
     *  Write the referenced design instantiation.
     *
     *    @param [in] stream                  Text stream to write the component instantiation.
     *    @param [in] designVLNV              The VLNV of the design instantiation.
     *    @param [in] instantiatedDesign      The instantiated design.
     *    @param [in] designFormatter         Expression formatter for the design.
     *    @param [in] instantiationFormatter  Expression formatter for the instantiation.
     */
    void writeReferencedDesignInstantiation(QTextStream& stream,
        QSharedPointer<ConfigurableVLNVReference> designVLNV,
        QSharedPointer<Design> instantiatedDesign,
        QSharedPointer<ExpressionFormatter> designFormatter,
        QSharedPointer<ExpressionFormatter> instantiationFormatter);

    /*!
     *  Sets the target path for the document.
     *
     *    @param [in] path    The new target path.
     */
    void setTargetPath(QString const& path);

    /*!
     *  Get the target path of the document.
     *
     *    @returns The target path.
     */
    QString getTargetPath() const;
    
    /*!
     *  Sets the images path.
     *
     *    @param [in] path    The new images path.
     */
    void setImagesPath(QString const& path);

    /*!
     *  Get the path of the images folder.
     *
     *    @returns The image folder path.
     */
    QString getImagesPath() const;

protected:

    /*!
     *  Writes the implementation details of a component instantiation.
     *
     *    @param [in] stream          Text stream to write to.
     *    @param [in] instantiation   The component instantiation.
     */
    virtual void writeImplementationDetails(QTextStream& stream,
        QSharedPointer<ComponentInstantiation> instantiation) = 0;

    /*!
     *  Writes the file set references contained within a component instantiation.
     *
     *    @param [in] stream          Text stream to write to.
     *    @param [in] instantiation   The component instantiation.
        */
    virtual void writeFileSetReferences(QTextStream& stream,
        QSharedPointer<ComponentInstantiation> instantiation) = 0;

    /*!
        *  Writes the file build commands of a component instantiation.
        *
        *    @param [in] stream          Text stream to write to.
        *    @param [in] instantiation   The component instantiation.
        *    @param [in] formatter       The expression formatter for the component instantiation.
        */
    virtual void writeFileBuildCommands(QTextStream& stream, QSharedPointer<ComponentInstantiation> instantiation,
        QSharedPointer<ExpressionFormatter> formatter) = 0;

    /*!
     *  Writes given parameters to a table.
     *
     *    @param [in] stream          Text stream to write to.
     *    @param [in] tableHeading    The heading above the table.
     *    @param [in] parameters      The parameters to be written.
     *    @param [in] formatter       The expression formatter for the parameters.
     */
    virtual void writeParameterTable(QTextStream& stream, QString const& tableHeading,
        QSharedPointer<QList<QSharedPointer<Parameter> > > parameters,
        QSharedPointer<ExpressionFormatter> formatter) = 0;
    
    /*!
     *  Writes given module parameters to a table.
     *
     *    @param [in] stream              Text stream to write to.
     *    @param [in] tableHeading        The heading above the table.
     *    @param [in] moduleParameters    The module parameters to be written.
     *    @param [in] formatter           The expression formatter for the parameters.
     */
    virtual void writeModuleParameterTable(QTextStream& stream, QString const& tableHeading,
        QSharedPointer<QList<QSharedPointer<Parameter> > > moduleParameters,
        QSharedPointer<ExpressionFormatter> formatter) = 0;

    /*!
     *  Writes the configurable element values of a VLNV.
     *
     *    @param [in] stream                  Text stream to write the document.
     *    @param [in] vlnvReference           Configurable VLNV containing the selected configurable element
     *                                          values.
     *    @param [in] instantiationFormatter  Expression formatter for the configurable element values.
     */
    virtual void writeConfigurableElementValues(QTextStream& stream,
        QSharedPointer<ConfigurableVLNVReference> vlnvReference,
        QSharedPointer<ExpressionFormatter> formatter) = 0;

    /*!
     *  Write a paragraph in which specific information is listed.
     *
     *    @param [in] stream              The text stream to write into.
     *    @param [in] names               The info item names.
     *    @param [in] values              The info item values.
     */
    virtual void writeInfoParagraph(QTextStream& stream, QStringList const& names, QStringList const& values) = 0;

    /*!
     *  Writes the enumerations of a field.
     *
     *    @param [in] stream              The text stream to write into.
     *    @param [in] field               The field which enumerations are written.
     */
    virtual void writeFieldEnumerations(QTextStream& stream, QSharedPointer<Field> field) = 0;

    /*!
     *  Finds the address blocks of a memory map.
     *      
     *    @param [in] memoryMap       The memory map.
     * 
     *    @returns The memory map address blocks.
     */
    QList<QSharedPointer <AddressBlock> > getMemoryMapAddressBlocks(QSharedPointer<MemoryMap> memoryMap) const;

    /*!
     *  Finds the registers from some register data.
     *
     *    @param [in] registerData    The register data.
     *
     *    @returns The registers of the register data.
     */
    QList<QSharedPointer <Register> > getRegisters(
        QSharedPointer<QList<QSharedPointer<RegisterBase> > > registerData) const;

    /*!
     *  Finds the register files from some register data.
     *
     *    @param [in] registerData    The register data.
     *
     *    @returns The register files of the register data.
     */
    QList<QSharedPointer <RegisterFile> > getRegisterFiles(
        QSharedPointer<QList<QSharedPointer<RegisterBase> > > registerData) const;

    /*!
     *  Finds the field reset info of a register field.
     *
     *    @param [in] field       The register field.
     *
     *    @returns The field reset info of the field.
     */
    QString getFieldResetInfo(QSharedPointer<Field> field, QString const& separator = "<br>") const;

    /*!
     *  Creates an expression formatter for a component contained within a design.
     *
     *    @param [in] design         The design.
     *    @param [in] component      The component.
     *
     *    @returns An expression formatter for .
     */
    QSharedPointer<ExpressionFormatter> createDesignInstanceFormatter(QSharedPointer<Design> design,
        QSharedPointer<Component> component);

    /*!
     *  Writes info about an address block.
     *
     *    @param [in] stream              The text stream to write into.
     *    @param [in] addressBlock        The address block to write about.
     */
    void writeAddressBlockInfo(QTextStream& stream, QSharedPointer<AddressBlock> addressBlock);

    /*!
     *  Writes info about a single register.
     *
     *    @param [in] stream              The text stream to write into.
     *    @param [in] reg                 The register to be written.
     *    @param [in] subHeaderNumbers    The subheader numbers to write in the subheader.
     *    @param [in] registerDataNumber  The current address block register data number.
     */
    void writeSingleRegister(QTextStream& stream, QSharedPointer<Register> reg, QList<int> subHeaderNumbers,
        int& registerDataNumber);

    /*!
     *  Writes info about a register file.
     *
     *    @param [in] stream              The text stream to write into.
     *    @param [in] registerFile        The register file to be written.
     */
    void writeRegisterFileInfo(QTextStream& stream, QSharedPointer<RegisterFile> registerFile);

    /*!
     *  Writes a single field.
     *
     *    @param [in] stream              The text stream to write into.
     *    @param [in] field               The field to be written.
     */
    void writeSingleField(QTextStream& stream, QSharedPointer<Field> field);

    /*!
     *  Writes information about a bus interface.
     *
     *    @param [in] stream              The text stream to write into.
     *    @param [in] interface           The interface to be written.
     *    @param [in] hasPorts            Flag that indicates if interface has ports.
     */
    void writeInterfaceInfo(QTextStream& stream, QSharedPointer<BusInterface> interface, bool hasPorts);

private:
    //! The expression formatter, used to change parameter IDs into names.
    ExpressionFormatter* expressionFormatter_;

    //! The factory for creating expression formatters.
    ExpressionFormatterFactory* expressionFormatterFactory_;

    //! The path the document is written to.
    QString targetPath_;

    //! The image save path
    QString imagesPath_;
};

#endif // DOCUMENTATIONWRITER_H
