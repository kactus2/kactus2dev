//-----------------------------------------------------------------------------
// File: RegisterReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 29.09.2015
//
// Description:
// Reader class for ipxact:register element.
//-----------------------------------------------------------------------------

#ifndef REGISTERREADER_H
#define REGISTERREADER_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/common/CommonItemsReader.h>

#include <QObject>
#include <QSharedPointer>
#include <QDomNode>

class RegisterBase;
class Register;
class RegisterDefinition;
class AlternateRegister;
class RegisterFile;

//-----------------------------------------------------------------------------
//! Reader class for ipxact:register element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT RegisterReader : public CommonItemsReader
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent  The owner of this reader.
     */
    RegisterReader(QObject* parent = 0);

    /*!
     *  The destructor.
     */
    ~RegisterReader();

    /*!
     *  Creates a new register from a given register node.
     *
     *      @param [in] registerNode    XML description of the register.
     *
     *      @return The created register.
     */
    QSharedPointer<Register> createRegisterfrom(QDomNode const& registerNode) const;

    /*!
     *  Creates a new register file from a given register file node.
     *
     *      @param [in] registerFileNode    XML description fo the register file.
     *
     *      @return The created register file.
     */
    QSharedPointer<RegisterFile> createRegisterFileFrom(QDomNode const& registerFileNode) const;

private:

    //! No copying allowed.
    RegisterReader(RegisterReader const& rhs);
    RegisterReader& operator=(RegisterReader const& rhs);

    /*!
     *  Reads the name group.
     *
     *      @param [in] nameGroupNode   XML description of the name group.
     *      @param [in] registerData    Selected register base.
     */
    void parseNameGroup(QDomNode const& nameGroupNode, QSharedPointer<RegisterBase> registerData) const;

    /*!
     *  Create address offset from a given register base node.
     *
     *      @param [in] registerBaseNode    XML description of the register base.
     *
     *      @return The parsed address offset.
     */
    QString createAddressOffsetFrom(QDomNode const& registerBaseNode) const;

    /*!
     *  Reads the register size.
     *
     *      @param [in] registerNode        XML description of the register.
     *      @param [in] selectedRegister    The selected register.
     */
    void parseRegisterSize(QDomNode const& registerNode, QSharedPointer<Register> selectedRegister) const;

    /*!
     *  Reads the is present value.
     *
     *      @param [in] registerBaseNode    XML description of the register base.
     *      @param [in] registerBase        Selected register base.
     */
    void parsePresence(QDomNode const& registerBaseNode, QSharedPointer<RegisterBase> registerBase) const;

    /*!
     *  Reads the register dimension.
     *
     *      @param [in] registerNode        XML description of the register.
     *      @param [in] selectedRegister    Selected register.
     */
    void parseRegisterDimension(QDomNode const& registerNode, QSharedPointer<Register> selectedRegister) const;

    /*!
     *  Reads the type identifier.
     *
     *      @param [in] registerBaseNode    XML description of the register base.
     *      @param [in] registerBase        Selected register base.
     */
    void parseTypeIdentifier(QDomNode const& registerBaseNode, QSharedPointer<RegisterBase> registerBase) const;

    /*!
     *  Reads the volatile value.
     *
     *      @param [in] registerDefinitionNode  XML description of the register definition.
     *      @param [in] registerDefinition      Selected register definition.
     */
    void parseVolatile(QDomNode const& registerDefinitionNode,
        QSharedPointer<RegisterDefinition> registerDefinition) const;

    /*!
     *  Reads the access value.
     *
     *      @param [in] registerDefinitionNode  XML description of the register definition.
     *      @param [in] registerDefinition      Selected register definition.
     */
    void parseAccess(QDomNode const& definitionNode, QSharedPointer<RegisterDefinition> registerDefinition) const;

    /*!
     *  Reads the fields.
     *
     *      @param [in] registerDefinitionElement   XML description of the register definition.
     *      @param [in] registerDefinition          Selected register definition.
     */
    void parseFields(QDomElement const& registerDefinitionElement,
        QSharedPointer<RegisterDefinition> registerDefinition) const;

    /*!
     *  Reads the alternate registers.
     *
     *      @param [in] registerElement     XML description of the register.
     *      @param [in] targetRegister      Selected register.
     */
    void parseAlternateRegisters(QDomElement const& registerElement, QSharedPointer<Register> targetRegister) const;

    /*!
     *  Reads a single alternate register.
     *
     *      @param [in] alternateRegisterElement    XML description of the alternate register.
     *      @param [in] newAlternateRegister        Selected alternate register.
     */
    void parseSingleAlternateRegister(QDomElement const& alternateRegisterElement,
        QSharedPointer<AlternateRegister> newAlternateRegister) const;

    /*!
     *  Reads alternate register alternate groups.
     *
     *      @param [in] alternateRegisterElement    XML description of the alternate register.
     *      @param [in] newAlternateRegister        Selected alternate register.
     */
    void parseAlternateGroups(QDomElement const& alternateRegisterElement,
        QSharedPointer<AlternateRegister> newAlternateRegister) const;

    /*!
     *  Reads the parameters.
     *
     *      @param [in] registerBaseNode    XML description of the register base.
     *      @param [in] newRegisterBase     Selected register base.
     */
    void parseParameters(QDomNode const& registerBaseNode, QSharedPointer<RegisterBase> newRegisterBase) const;

    /*!
     *  Reads the register file range.
     *
     *      @param [in] registerFileNode    XML description of the register file.
     *      @param [in] newRegisterFile     Selected register file.
     */
    void parseRegisterFileRange(QDomNode const& registerFileNode, QSharedPointer<RegisterFile> newRegisterFile)
        const;

    /*!
     *  Reads the register file dimension.
     *
     *      @param [in] registerFileNode    XML description of the register file.
     *      @param [in] newRegisterFile     Selected register file.
     */
    void parseRegisterFileDimension(QDomNode const& registerFileNode,
        QSharedPointer<RegisterFile> newRegisterFile) const;

    /*!
     *  Reads the contained register data of a register file.
     *
     *      @param [in] registerFileNode    XML description of the register file.
     *      @param [in] newRegisterFile     Selected register file.
     */
    void parseRegisterFileRegisterData(QDomNode const& registerFileNode,
        QSharedPointer<RegisterFile> newRegisterFile) const;
};

#endif // REGISTERREADER_H