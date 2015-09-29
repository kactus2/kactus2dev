//-----------------------------------------------------------------------------
// File: RegisterWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 28.09.2015
//
// Description:
// Writer class for IP-XACT register element.
//-----------------------------------------------------------------------------

#ifndef REGISTERWRITER_H
#define REGISTERWRITER_H

#include <IPXACTmodels/ipxactmodels_global.h>
#include <IPXACTmodels/common/CommonItemsWriter.h>

#include <QXmlStreamWriter>
#include <QObject>
#include <QSharedPointer>

class Register;
class RegisterFile;
class RegisterBase;
class RegisterDefinition;
class AlternateRegister;

//-----------------------------------------------------------------------------
//! Writer class for IP-XACT register element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT RegisterWriter : public CommonItemsWriter
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent  Owner of this writer.
     */
    RegisterWriter(QObject* parent = 0);

    /*!
     *  The destructor.
     */
    ~RegisterWriter();

    /*!
     *  Write a register or a register file to an XML file.
     *
     *      @param [in] writer          The used XML writer.
     *      @param [in] registerData    The register data to be written.
     */
    void writeRegisterData(QXmlStreamWriter& writer, QSharedPointer<RegisterBase> registerData) const;

private:

    //! No copying allowed.
    RegisterWriter(RegisterWriter const& rhs);
    RegisterWriter& operator=(RegisterWriter const& rhs);

    /*!
     *  Write a register.
     *
     *      @param [in] writer          The used XML writer.
     *      @param [in] targetRegister  The selected register.
     */
    void writeRegister(QXmlStreamWriter& writer, QSharedPointer<Register> targetRegister) const;

    /*!
     *  Write the name group.
     *
     *      @param [in] writer          Used XML writer.
     *      @param [in] registerData    Register data containing the name group to be written.
     */
    void writeNameGroup(QXmlStreamWriter& writer, QSharedPointer<RegisterBase> registerData) const;

    /*!
     *  Write the dimension.
     *
     *      @param [in] writer      Used XML writer.
     *      @param [in] dimension   The dimension to be written.
     */
    void writeDimension(QXmlStreamWriter& writer, QString const& dimension) const;

    /*!
     *  Write the address offset.
     *
     *      @param [in] writer          Used XML writer.
     *      @param [in] addressOffset   Address offset to be written.
     */
    void writeAddressOffset(QXmlStreamWriter& writer, QString const& addressOffset) const;

    /*!
     *  Write the type identifier.
     *
     *      @param [in] writer          Used XML writer.
     *      @param [in] registerData    Register data containing the type identifier.
     */
    void writeTypeIdentifier(QXmlStreamWriter& writer, QSharedPointer<RegisterBase> registerData) const;

    /*!
     *  Write the volatile value.
     *
     *      @param [in] writer              Used XML writer.
     *      @param [in] registerDefinition  The selected register definition.
     */
    void writeVolatile(QXmlStreamWriter& writer, QSharedPointer<RegisterDefinition> registerDefinition) const;

    /*!
     *  Write the access value.
     *
     *      @param [in] writer              Used XML writer.
     *      @param [in] registerDefinition  The selected register definition.
     */
    void writeAccess(QXmlStreamWriter& writer, QSharedPointer<RegisterDefinition> registerDefinition) const;

    /*!
     *  Write the fields.
     *
     *      @param [in] writer              Used XML writer.
     *      @param [in] registerDefinition  The selected register definition.
     */
    void writeFields(QXmlStreamWriter& writer, QSharedPointer<RegisterDefinition> registerDefinition) const;

    /*!
     *  Write the register alternate registers.
     *
     *      @param [in] writer          Used XML writer.
     *      @param [in] targetRegister  The selected register.
     */
    void writeAlternateRegisters(QXmlStreamWriter& writer, QSharedPointer<Register> targetRegister) const;

    /*!
     *  Write a single alternate register.
     *
     *      @param [in] writer              Used XML writer.
     *      @param [in] alternateRegister   The selected alternate register.
     */
    void writeSingleAlternateRegister(QXmlStreamWriter& writer,
        QSharedPointer<AlternateRegister> alternateRegister) const;

    /*!
     *  Write the alternate register alternate groups.
     *
     *      @param [in] writer              Used XML writer.
     *      @param [in] alternateRegister   The selected alternate register.
     */
    void writeAlternateGroups(QXmlStreamWriter& writer, QSharedPointer<AlternateRegister> alternateRegister) const;

    /*!
     *  Write a register file.
     *
     *      @param [in] writer          Used XML writer.
     *      @param [in] registerFile    The register file to be written.
     */
    void writeRegisterFile(QXmlStreamWriter& writer, QSharedPointer<RegisterFile> registerFile) const;

    /*!
     *  Write register data contained within a register file.
     *
     *      @param [in] writer          Used XML writer.
     *      @param [in] registerFile    The register file to be written.
     */
    void writeRegisterFileRegisterData(QXmlStreamWriter& writer, QSharedPointer<RegisterFile> registerFile) const;
};

#endif // REGISTERWRITER_H