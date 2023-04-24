//-----------------------------------------------------------------------------
// File: GeneralDocumentGenerator.h
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

#include <QTextStream>
#include <QString>
#include <QSharedPointer>

class ExpressionFormatter;
class AddressBlock;
class Register;
class Field;
class MemoryMap;
class Parameter;
class ComponentInstantiation;
class ListParameterFinder;
class DesignConfigurationInstantiation;

using ParameterList = QSharedPointer<QList<QSharedPointer<Parameter> > >;

class DocumentationWriter
{
public:
    static const QStringList ADDRESS_BLOCK_HEADERS;

    static const QStringList DEFAULT_FILE_BUILDER_HEADERS;

    static const QStringList FIELD_HEADERS;
    
    static const QStringList FILE_HEADERS;

    static const QStringList PARAMETER_HEADERS;

    static const QStringList PORT_HEADERS;

    static const QStringList REGISTER_HEADERS;
    
    DocumentationWriter(ExpressionFormatter* formatter);
    virtual ~DocumentationWriter() = default;

    virtual void writeHeader(QTextStream& stream) = 0;

    virtual void writeKactusAttributes(QTextStream& stream, int subHeaderNumber) = 0;

    virtual void writeTableOfContents(QTextStream& stream) = 0;

    virtual void writeParameters(QTextStream& stream, int subHeaderNumber) = 0;

    virtual void writeMemoryMaps(QTextStream& stream, int subHeaderNumber) = 0;

    virtual void writeAddressBlocks(QTextStream& stream, QList<QSharedPointer <AddressBlock> > addressBlocks,
        int subHeaderNumber, int memoryMapNumber) = 0;

    virtual void writeRegisters(QTextStream& stream, QList<QSharedPointer <Register> > registers,
        int subHeaderNumber, int memoryMapNumber, int addressBlockNumber) = 0;

    virtual void writeFields(QTextStream& stream, QSharedPointer <Register> currentRegister) = 0;

    virtual void writePorts(QTextStream& stream, int subHeaderNumber) = 0;

    virtual void writeInterfaces(QTextStream& stream, int& subHeaderNumber) = 0;

    virtual void writeFileSets(QTextStream& stream, int& subHeaderNumber) = 0;

    virtual void setComponentNumber(int componentNumber) = 0;

    // Writes a component subheader for linked subheaders
    virtual void writeSubHeader(QTextStream& stream, int subHeaderNumber,
        QString const& headerText, QString const& headerId) const = 0;
    
    // Writes a subheader of specified level and numbering for non-linked subheaders
    virtual void writeSubHeader(QTextStream& stream, QList<int> const& subHeaderNumbers,
        QString const& title, int level) const = 0;

    virtual void writeViewDescription(QTextStream& stream, QString const& description) = 0;

    virtual void writeErrorMessage(QTextStream& stream, QString const& message) = 0;

    virtual void writeReferencedComponentInstantiation(
        QTextStream& stream,
        QSharedPointer<ComponentInstantiation> instantiation,
        QSharedPointer<ExpressionFormatter> instantiationFormatter,
        ParameterList moduleParameters,
        ParameterList parameters
    ) = 0;

    virtual void writeReferencedDesignConfigurationInstantiation(QTextStream& stream,
        QSharedPointer<ListParameterFinder> configurationFinder,
        QSharedPointer<DesignConfigurationInstantiation> instantiation,
        QSharedPointer<ExpressionFormatter> instantiationFormatter) = 0;

    void setTargetPath(QString const& path);

    QString getTargetPath() const;

protected:
    // Finds the address blocks of a memory map
    QList<QSharedPointer <AddressBlock> > getMemoryMapAddressBlocks(QSharedPointer<MemoryMap> memoryMap) const;

    // Finds the registers of an address block
    QList<QSharedPointer <Register> > getAddressBlockRegisters(QSharedPointer<AddressBlock> addressBlock) const;

    // Get the information for the reset values of the selected field.
    QString getFieldResetInfo(QSharedPointer<Field> field) const;

private:
    //! The expression formatter, used to change parameter IDs into names.
    ExpressionFormatter* expressionFormatter_;

    QString targetPath_;
};

#endif // DOCUMENTATIONWRITER_H
