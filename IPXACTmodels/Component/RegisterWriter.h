//-----------------------------------------------------------------------------
// File: RegisterWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 28.09.2015
//
// Description:
// Writer for IP-XACT register element.
//-----------------------------------------------------------------------------

#ifndef REGISTERWRITER_H
#define REGISTERWRITER_H

#include <IPXACTmodels/ipxactmodels_global.h>
#include <IPXACTmodels/common/CommonItemsWriter.h>

#include <QXmlStreamWriter>
#include <QSharedPointer>

class Register;
class RegisterFile;
class RegisterBase;
class RegisterDefinition;
class AlternateRegister;

//-----------------------------------------------------------------------------
//! Writer for IP-XACT register element.
//-----------------------------------------------------------------------------
namespace RegisterWriter
{

    /*!
     *  Write a register or a register file to an XML file.
     *
     *    @param [in] writer          The used XML writer.
     *    @param [in] registerData    The register data to be written.
     */
    IPXACTMODELS_EXPORT void writeRegisterData(QXmlStreamWriter& writer, QSharedPointer<RegisterBase> registerData, Document::Revision docRevision);

    namespace Details
    {
        /*!
         *  Write a register.
         *
         *    @param [in] writer          The used XML writer.
         *    @param [in] targetRegister  The selected register.
         */
        void writeRegister(QXmlStreamWriter& writer, QSharedPointer<Register> targetRegister, Document::Revision docRevision);

        /*!
         *  Write the address offset.
         *
         *    @param [in] writer          Used XML writer.
         *    @param [in] addressOffset   Address offset to be written.
         */
        void writeAddressOffset(QXmlStreamWriter& writer, QString const& addressOffset);

        /*!
         *  Write the type identifier.
         *
         *    @param [in] writer          Used XML writer.
         *    @param [in] registerData    Register data containing the type identifier.
         */
        void writeTypeIdentifier(QXmlStreamWriter& writer, QSharedPointer<RegisterBase> registerData);

        /*!
         *  Write the volatile value.
         *
         *    @param [in] writer              Used XML writer.
         *    @param [in] registerDefinition  The selected register definition.
         */
        void writeVolatile(QXmlStreamWriter& writer, QSharedPointer<RegisterDefinition> registerDefinition);

        /*!
         *  Write the access value.
         *
         *    @param [in] writer              Used XML writer.
         *    @param [in] registerDefinition  The selected register definition.
         */
        void writeAccess(QXmlStreamWriter& writer, QSharedPointer<RegisterDefinition> registerDefinition);

        /*!
         *  Write the fields.
         *
         *    @param [in] writer              Used XML writer.
         *    @param [in] registerDefinition  The selected register definition.
         */
        void writeFields(QXmlStreamWriter& writer, QSharedPointer<RegisterDefinition> registerDefinition, Document::Revision docRevision);

        /*!
         *  Write the register alternate registers.
         *
         *    @param [in] writer          Used XML writer.
         *    @param [in] targetRegister  The selected register.
         */
        void writeAlternateRegisters(QXmlStreamWriter& writer, QSharedPointer<Register> targetRegister, Document::Revision docRevision);

        /*!
         *  Write a single alternate register.
         *
         *    @param [in] writer              Used XML writer.
         *    @param [in] alternateRegister   The selected alternate register.
         */
        void writeSingleAlternateRegister(QXmlStreamWriter& writer,
            QSharedPointer<AlternateRegister> alternateRegister,
            Document::Revision docRevision);

        /*!
         *  Write the alternate register alternate groups.
         *
         *    @param [in] writer              Used XML writer.
         *    @param [in] alternateRegister   The selected alternate register.
         */
        void writeAlternateGroups(QXmlStreamWriter& writer, QSharedPointer<AlternateRegister> alternateRegister);

        /*!
         *  Write a register file.
         *
         *    @param [in] writer          Used XML writer.
         *    @param [in] registerFile    The register file to be written.
         */
        void writeRegisterFile(QXmlStreamWriter& writer, QSharedPointer<RegisterFile> registerFile, Document::Revision docRevision);

        /*!
         *  Write register data contained within a register file.
         *
         *    @param [in] writer          Used XML writer.
         *    @param [in] registerFile    The register file to be written.
         */
        void writeRegisterFileRegisterData(QXmlStreamWriter& writer, QSharedPointer<RegisterFile> registerFile, Document::Revision docRevision);

        /*!
         *	Write the access policies of the selected register base.
         *  
         *    @param [in] writer          XML writer to use.
         *    @param [in] targetRegister  The selected register base.
         */
        void writeAccessPolicies(QXmlStreamWriter& writer, QSharedPointer<RegisterBase> targetRegisterBase);

        /*!
         *	Write the register definition reference.
         *  
         *    @param [in] writer          XML writer to use.
         *    @param [in] targetRegister  The selected register.
         */
        void writeRegisterDefinitionReference(QXmlStreamWriter& writer, QSharedPointer<Register> targetRegister);
    
        /*!
         *	Write the register file definition reference.
         *  
         *    @param [in] writer          XML writer to use.
         *    @param [in] registerFile    The register file to be written.
         */
        void writeRegisterFileDefinitionReference(QXmlStreamWriter& writer, QSharedPointer<RegisterFile> registerFile);
    }
}

#endif // REGISTERWRITER_H