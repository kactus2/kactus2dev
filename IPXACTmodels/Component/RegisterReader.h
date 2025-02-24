//-----------------------------------------------------------------------------
// File: RegisterReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 29.09.2015
//
// Description:
// Reader for ipxact:register element.
//-----------------------------------------------------------------------------

#ifndef REGISTERREADER_H
#define REGISTERREADER_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/common/CommonItemsReader.h>

#include <QSharedPointer>
#include <QDomNode>

class RegisterBase;
class Register;
class RegisterDefinition;
class AlternateRegister;
class RegisterFile;

//-----------------------------------------------------------------------------
//! Reader for ipxact:register element.
//-----------------------------------------------------------------------------
namespace RegisterReader
{
    /*!
     *  Creates a new register from a given register node.
     *
     *    @param [in] registerNode    XML description of the register.
     *
     *    @return The created register.
     */
    IPXACTMODELS_EXPORT QSharedPointer<Register> createRegisterfrom(QDomNode const& registerNode, Document::Revision docRevision);

    /*!
     *  Creates a new register file from a given register file node.
     *
     *    @param [in] registerFileNode    XML description fo the register file.
     *
     *    @return The created register file.
     */
    IPXACTMODELS_EXPORT QSharedPointer<RegisterFile> createRegisterFileFrom(QDomNode const& registerFileNode, Document::Revision docRevision);

    namespace Details
    {

        /*!
         *  Create address offset from a given register base node.
         *
         *    @param [in] registerBaseNode    XML description of the register base.
         *
         *    @return The parsed address offset.
         */
        QString createAddressOffsetFrom(QDomNode const& registerBaseNode);

        /*!
         *  Reads the register size.
         *
         *    @param [in] registerNode        XML description of the register.
         *    @param [in] selectedRegister    The selected register.
         */
        void parseRegisterSize(QDomNode const& registerNode, QSharedPointer<Register> selectedRegister);

        /*!
         *  Reads the is present value.
         *
         *    @param [in] registerBaseNode    XML description of the register base.
         *    @param [in] registerBase        Selected register base.
         */
        void parsePresence(QDomNode const& registerBaseNode, QSharedPointer<RegisterBase> registerBase);

        /*!
         *  Reads the register dimension.
         *
         *    @param [in] registerNode        XML description of the register.
         *    @param [in] selectedRegister    Selected register.
         */
        void parseRegisterDimension(QDomNode const& registerNode, QSharedPointer<Register> selectedRegister);

        /*!
         *  Reads the type identifier.
         *
         *    @param [in] registerBaseNode    XML description of the register base.
         *    @param [in] registerBase        Selected register base.
         */
        void parseTypeIdentifier(QDomNode const& registerBaseNode, QSharedPointer<RegisterBase> registerBase);

        /*!
         *  Reads the volatile value.
         *
         *    @param [in] registerDefinitionNode  XML description of the register definition.
         *    @param [in] registerDefinition      Selected register definition.
         */
        void parseVolatile(QDomNode const& registerDefinitionNode,
            QSharedPointer<RegisterDefinition> registerDefinition);

        /*!
         *  Reads the access value.
         *
         *    @param [in] registerDefinitionNode  XML description of the register definition.
         *    @param [in] registerDefinition      Selected register definition.
         */
        void parseAccess(QDomNode const& definitionNode, QSharedPointer<RegisterDefinition> registerDefinition);

        /*!
         *  Reads the fields.
         *
         *    @param [in] registerDefinitionElement   XML description of the register definition.
         *    @param [in] registerDefinition          Selected register definition.
         */
        void parseFields(QDomElement const& registerDefinitionElement,
            QSharedPointer<RegisterDefinition> registerDefinition,
            Document::Revision docRevision);

        /*!
         *  Reads the alternate registers.
         *
         *    @param [in] registerElement     XML description of the register.
         *    @param [in] targetRegister      Selected register.
         */
        void parseAlternateRegisters(QDomElement const& registerElement, QSharedPointer<Register> targetRegister, Document::Revision docRevision);

        /*!
         *  Reads a single alternate register.
         *
         *    @param [in] alternateRegisterElement    XML description of the alternate register.
         *    @param [in] newAlternateRegister        Selected alternate register.
         */
        void parseSingleAlternateRegister(QDomElement const& alternateRegisterElement,
            QSharedPointer<AlternateRegister> newAlternateRegister,
            Document::Revision docRevision);

        /*!
         *  Reads alternate register alternate groups.
         *
         *    @param [in] alternateRegisterElement    XML description of the alternate register.
         *    @param [in] newAlternateRegister        Selected alternate register.
         */
        void parseAlternateGroups(QDomElement const& alternateRegisterElement,
            QSharedPointer<AlternateRegister> newAlternateRegister);

        /*!
         *  Reads the parameters.
         *
         *    @param [in] registerBaseNode    XML description of the register base.
         *    @param [in] newRegisterBase     Selected register base.
         */
        void parseParameters(QDomNode const& registerBaseNode, QSharedPointer<RegisterBase> newRegisterBase);

        /*!
         *  Reads the register file range.
         *
         *    @param [in] registerFileNode    XML description of the register file.
         *    @param [in] newRegisterFile     Selected register file.
         */
        void parseRegisterFileRange(QDomNode const& registerFileNode, QSharedPointer<RegisterFile> newRegisterFile)
           ;

        /*!
         *  Reads the register file dimension.
         *
         *    @param [in] registerFileNode    XML description of the register file.
         *    @param [in] newRegisterFile     Selected register file.
         */
        void parseRegisterFileDimension(QDomNode const& registerFileNode,
            QSharedPointer<RegisterFile> newRegisterFile);

        /*!
         *  Reads the contained register data of a register file.
         *
         *    @param [in] registerFileNode    XML description of the register file.
         *    @param [in] newRegisterFile     Selected register file.
         */
        void parseRegisterFileRegisterData(QDomNode const& registerFileNode,
            QSharedPointer<RegisterFile> newRegisterFile,
            Document::Revision docRevision);

        /*!
         *	Reads the register definition refernece of a register.
         *  
         *    @param [in] registerNode     XML description of the register.
         *    @param [in] newRegister      The selected register.
         */
        void parseRegisterDefinitionReference(QDomNode const& registerNode,
            QSharedPointer<Register> newRegister);

        /*!
         *	Reads the memory array of a register.
         *  
         *    @param [in] registerNode     XML description of the register.
         *    @param [in] newRegister      The selected register.
         */
        void parseRegisterMemoryArray(QDomNode const& registereNode, QSharedPointer<Register> newRegister);

        /*!
         *	Reads the memory array of a register file.
         *  
         *    @param [in] registerFileNode     XML description of the register file.
         *    @param [in] newRegisterFile      The selected register file.
         */
        void parseRegisterFileMemoryArray(QDomNode const& registerFileNode, QSharedPointer<RegisterFile> newRegisterFile);
        
        /*!
         *	Reads the definition reference of a register file.
         *  
         *    @param [in] registerFileNode     XML description of the register file.
         *    @param [in] newRegisterFile      The selected register file.
         */
        void parseRegisterFileDefinitionReference(QDomNode const& registerFileNode,
            QSharedPointer<RegisterFile> newRegisterFile);

        /*!
         *	Reads the access policies for a register, register file or alternate register.
         *  
         *    @param [in] registerBaseNode    XML description of the register base.
         *    @param [in] newRegisterBase     Selected register base.
         */
        void parseAccessPolicies(QDomNode const& registerBaseNode, QSharedPointer<RegisterBase> newRegisterBase);

        /*!
         *	Reads the mode references of an alternate register.
         *  
         *    @param [in] alternateRegisterNode       XML description of the alternate register.
         *    @param [in] newAlternateRegister        Selected alternate register.
         */
        void parseAlternateRegisterModeRefs(QDomNode const& alternateRegisterNode, QSharedPointer<AlternateRegister> newAlternateRegister);
    }
}

#endif // REGISTERREADER_H