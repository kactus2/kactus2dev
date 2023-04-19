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

class DocumentationWriter
{
public:
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
