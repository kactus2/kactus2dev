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

#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>

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

    static const QStringList PORT_HEADERS;

    static const QStringList REGISTER_HEADERS;

    static const QStringList DESIGN_INSTANCE_HEADERS;
    
    DocumentationWriter(ExpressionFormatter* formatter, ExpressionFormatterFactory* expressionFormatterFactory);
    virtual ~DocumentationWriter() = default;

    /*!
     *  Write the document header.
     *
     *      @param [in] stream  The text stream to write the header into.
     */
    virtual void writeHeader(QTextStream& stream) = 0;
    
    /*!
     *  Write a header for the component.
     *
     *      @param [in] stream  The text stream to write the header into.
     */
    virtual void writeComponentHeader(QTextStream& stream) = 0;
    
    /*!
     *  Write the component info.
     *
     *      @param [in] stream  The text stream to write into.
     */
    virtual void writeComponentInfo(QTextStream& stream) = 0;

    /*!
     *  Write component kactus2 attributes.
     *
     *      @param [in] stream              The text stream to write into.
     *      @param [in] subHeaderNumber     The current subheader number.
     */
    virtual void writeKactusAttributes(QTextStream& stream, int subHeaderNumber) = 0;

    /*!
     *  Write the table of contents header.
     *
     *      @param [in] stream  The text stream to write into.
     */
    virtual void writeTableOfContentsHeader(QTextStream& stream) = 0;

    /*!
     *  Write the table of contents of the current component.
     *
     *      @param [in] stream  The text stream to write into.
     */
    virtual void writeTableOfContents(QTextStream& stream) = 0;

    /*!
     *  Write the parameters of the component.
     *
     *      @param [in] stream              The text stream to write into.
     *      @param [in] subHeaderNumber     The current subheader number.
     */
    virtual void writeParameters(QTextStream& stream, int subHeaderNumber) = 0;

    /*!
     *  Write the memory maps of the component.
     *
     *      @param [in] stream              The text stream to write into.
     *      @param [in] subHeaderNumber     The current subheader number.
     */
    virtual void writeMemoryMaps(QTextStream& stream, int subHeaderNumber) = 0;

    /*!
     *  Write the given address blocks.
     *
     *      @param [in] stream              The text stream to write into.
     *      @param [in] addressBlocks       The address blocks to be written.
     *      @param [in] subHeaderNumber     The current subheader number.
     *      @param [in] memoryMapNumber     The current memory map number.
     */
    virtual void writeAddressBlocks(QTextStream& stream, QList<QSharedPointer <AddressBlock> > addressBlocks,
        int subHeaderNumber, int memoryMapNumber) = 0;
    
    /*!
     *  Write the given register files
     *
     *      @param [in] stream               The text stream to write into.
     *      @param [in] registerFiles        The registers to be written.
     *      @param [in] subHeaderNumbers     The current subheader number.
     *      @param [out] registerDataNumber  The current address block number.
     */
    virtual void writeRegisterFiles(QTextStream& stream,
        QList<QSharedPointer<RegisterFile> > registerFiles,
        QList<int> subHeaderNumbers, int& registerDataNumber) = 0;

    /*!
     *  Write the given registers.
     *
     *      @param [in] stream              The text stream to write into.
     *      @param [in] registers           The registers to be written.
     *      @param [in] subHeaderNumber     The current subheader number.
     *      @param [in] memoryMapNumber     The current memory map number.
     *      @param [in] addressBlockNumber  The current address block number.
     */
    virtual void writeRegisters(QTextStream& stream, QList<QSharedPointer <Register> > registers,
        int subHeaderNumber, int memoryMapNumber, int addressBlockNumber, int& registerDataNumber) = 0;

    /*!
     *  Write the given register fields.
     *
     *      @param [in] stream              The text stream to write into.
     *      @param [in] register            The register whose fields are to be written.
     */
    virtual void writeFields(QTextStream& stream, QSharedPointer <Register> currentRegister, QList<int> registerSubHeaderNumbers) = 0;

    /*!
     *  Write the ports of the component.
     *
     *      @param [in] stream              The text stream to write into.
     *      @param [in] subHeaderNumber     The current subheader number.
     */
    virtual void writePorts(QTextStream& stream, int subHeaderNumber) = 0;

    /*!
     *  Write the bus interfaces of the component.
     *
     *      @param [in] stream              The text stream to write into.
     *      @param [in] subHeaderNumber     The current subheader number.
     */
    virtual void writeInterfaces(QTextStream& stream, int& subHeaderNumber) = 0;

    /*!
     *  Write the file sets of the component.
     *
     *      @param [in] stream              The text stream to write into.
     *      @param [in] subHeaderNumber     The current subheader number.
     */
    virtual void writeFileSets(QTextStream& stream, int& subHeaderNumber) = 0;

    /*!
     *  Set the number of the component in the hierarchy.
     *
     *      @param [in] componentNumber     The new component number.
     */
    virtual void setComponentNumber(int componentNumber) = 0;

    /*!
     *  Write a referenced component subheader.
     *
     *      @param [in] stream              The text stream to write into.
     *      @param [in] subHeaderNumber     The current subheader number.
     *      @param [in] headerText          The displayed header text.
     *      @param [in] headerId            The id for referencing within the document.
     */
    virtual void writeSubHeader(QTextStream& stream, int subHeaderNumber,
        QString const& headerText, QString const& headerId) const = 0;
    
    /*!
     *  Write a non-referenced component subheader with specified header level.
     *
     *      @param [in] stream              The text stream to write into.
     *      @param [in] subHeaderNumbers    The subheader numbers.
     *      @param [in] title               The displayed header text.
     *      @param [in] headerId            The id for referencing within the document.
     */
    virtual void writeSubHeader(QTextStream& stream, QList<int> const& subHeaderNumbers,
        QString const& title, int level) const = 0;

    /*!
     *  Write a description text.
     *
     *      @param [in] stream              The text stream to write into.
     *      @param [in] description         The description to write.
     */
    virtual void writeDescription(QTextStream& stream, QString const& description) = 0;

    /*!
     *  Write an error text to the document.
     *
     *      @param [in] stream              The text stream to write into..
     *      @param [in] message             The description to write.
     */
    virtual void writeErrorMessage(QTextStream& stream, QString const& message) = 0;

    /*!
     *  Write the document contained within the selected VLNV.
     *
     *      @param [in] stream                  Text stream to write the document.
     *      @param [in] documentType            Type of the document.
     *      @param [in] vlnvReference           VLNV of the selected document.
     */
    virtual void writeDocumentReference(QTextStream& stream, QString const& documentType,
        QSharedPointer<ConfigurableVLNVReference> vlnvReference) = 0;

    /*!
     *  Write the referenced component instantiation.
     *
     *      @param [in] stream                  Text stream to write the component instantiation.
     *      @param [in] instantiation           The component instantiation.
     *      @param [in] instantiationFormatter  The component instantiation formatter.
     *      @param [in] moduleParameters        The instantiation module parameters.
     *      @param [in] parameters              The instantiation parameters.
     */
    virtual void writeReferencedComponentInstantiation(
        QTextStream& stream,
        QSharedPointer<ComponentInstantiation> instantiation,
        QSharedPointer<ExpressionFormatter> instantiationFormatter,
        ParameterList moduleParameters,
        ParameterList parameters) = 0;

    /*!
     *  Write the referenced design configuration instantiation.
     *
     *      @param [in] stream                  Text stream to write the component instantiation.
     *      @param [in] configurationFinder     The parameter finder for the instantiation.
     *      @param [in] instantiationFormatter  The instantiation formatter.
     */
    virtual void writeReferencedDesignConfigurationInstantiation(QTextStream& stream,
        QSharedPointer<ListParameterFinder> configurationFinder,
        QSharedPointer<DesignConfigurationInstantiation> instantiation,
        QSharedPointer<ExpressionFormatter> instantiationFormatter) = 0;

    /*!
     *  Write the referenced design instantiation.
     *
     *      @param [in] stream                  Text stream to write the component instantiation.
     *      @param [in] designVLNV              The VLNV of the design instantiation.
     *      @param [in] instantiatedDesign      The instantiated design.
     *      @param [in] designFormatter         Expression formatter for the design.
     *      @param [in] instantiationFormatter  Expression formatter for the instantiation.
     */
    virtual void writeReferencedDesignInstantiation(QTextStream& stream,
        QSharedPointer<ConfigurableVLNVReference> designVLNV, QSharedPointer<Design> instantiatedDesign,
        ExpressionFormatter* designFormatter, QSharedPointer<ExpressionFormatter> instantiationFormatter) = 0;

    /*!
     *  Write the design diagram.
     *
     *      @param [in] stream                  Text stream to write the diagram to.
     *      @param [in] title                   Diagram title.
     *      @param [in] link                    Diagram picture link or path.
     *      @param [in] altText                 Diagram alt text.
     */
    virtual void writeDiagram(QTextStream& stream, QString const& title, QString const& link, QString const& altText) = 0;

    /*!
     *  Write the component instances contained within the selected design.
     *
     *      @param [in] stream          Text stream to write the component instances.
     *      @param [in] design          The selected design.
     *      @param [in] configuration   The selected design configuration.
     */
    virtual void writeDesignInstances(QTextStream& stream, QSharedPointer<Design> design,
        QSharedPointer<DesignConfiguration> configuration) = 0;

    /*!
     *  Write the end of the document, if the format requires it.
     *
     *      @param [in] stream  The text stream to write the documentation into.
     */
    virtual void writeEndOfDocument(QTextStream& stream) = 0;

    /*!
     *  Sets the target path for the document.
     *
     *      @param [in] path    The new target path.
     */
    void setTargetPath(QString const& path);

    /*!
     *  Get the target path of the document.
     *
     *      @returns The target path.
     */
    QString getTargetPath() const;

protected:

    /*!
     *  Finds the address blocks of a memory map.
     *      
     *      @param [in] memoryMap       The memory map.
     * 
     *      @returns The memory map address blocks.
     */
    QList<QSharedPointer <AddressBlock> > getMemoryMapAddressBlocks(QSharedPointer<MemoryMap> memoryMap) const;

    /*!
     *  Finds the registers from some register data.
     *
     *      @param [in] registerData    The register data.
     *
     *      @returns The registers of the register data.
     */
    QList<QSharedPointer <Register> > getRegisters(
        QSharedPointer<QList<QSharedPointer<RegisterBase> > > registerData) const;

    /*!
     *  Finds the register files from some register data.
     *
     *      @param [in] registerData    The register data.
     *
     *      @returns The register files of the register data.
     */
    QList<QSharedPointer <RegisterFile> > getRegisterFiles(
        QSharedPointer<QList<QSharedPointer<RegisterBase> > > registerData) const;

    /*!
     *  Finds the field reset info of a register field.
     *
     *      @param [in] field       The register field.
     *
     *      @returns The field reset info of the field.
     */
    QString getFieldResetInfo(QSharedPointer<Field> field, QString const& separator = "<br>") const;

    /*!
     *  Creates an expression formatter for a component contained within a design.
     *
     *      @param [in] design         The design.
     *      @param [in] component      The component.
     *
     *      @returns An expression formatter for .
     */
    QSharedPointer<ExpressionFormatter> createDesignInstanceFormatter(QSharedPointer<Design> design,
        QSharedPointer<Component> component);

private:
    //! The expression formatter, used to change parameter IDs into names.
    ExpressionFormatter* expressionFormatter_;

    //! The factory for creating expression formatters.
    ExpressionFormatterFactory* expressionFormatterFactory_;

    //! The path the document is written to.
    QString targetPath_;
};

#endif // DOCUMENTATIONWRITER_H
