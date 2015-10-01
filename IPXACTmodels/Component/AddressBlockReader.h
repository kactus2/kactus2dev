//-----------------------------------------------------------------------------
// File: AddressBlockReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 30.09.2015
//
// Description:
// Reader class for ipxact:addressBlock element.
//-----------------------------------------------------------------------------

#ifndef ADDRESSBLOCKREADER_H
#define ADDRESSBLOCKREADER_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/common/CommonItemsReader.h>

#include <QObject>
#include <QSharedPointer>
#include <QDomNode>

class AddressBlock;

//-----------------------------------------------------------------------------
//! Reader class for ipxact:addressBlock element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT AddressBlockReader : public CommonItemsReader
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent  The owner of this reader.
     */
    AddressBlockReader(QObject* parent = 0);

    /*!
     *  The destructor.
     */
    ~AddressBlockReader();

    /*!
     *  Creates a new address block from a given address block node.
     *
     *      @param [in] addressBlockNode    XML description of the address block.
     *
     *      @return The created address block.
     */
    QSharedPointer<AddressBlock> createAddressBlockFrom(QDomNode const& addressBlockNode) const;

private:

    //! No copying allowed.
    AddressBlockReader(AddressBlockReader const& rhs);
    AddressBlockReader& operator=(AddressBlockReader const& rhs);

    /*!
     *  Reads the name group.
     *
     *      @param [in] addressBlockNode    XML description of the address block.
     *      @param [in] newAddressBlock     The new address block.
     */
    void parseNameGroup(QDomNode const& addressBlockNode, QSharedPointer<AddressBlock> newAddressBlock) const;

    /*!
     *  Reads the presence.
     *
     *      @param [in] addressBlockNode    XML description of the address block.
     *      @param [in] newAddressBlock     The new address block.
     */
    void parsePresence(QDomNode const& addressBlockNode, QSharedPointer<AddressBlock> newAddressBlock) const;

    /*!
     *  Reads the base address.
     *
     *      @param [in] addressBlockNode    XML description of the address block.
     *      @param [in] newAddressBlock     The new address block.
     */
    void parseBaseAddress(QDomNode const& addressBlockNode, QSharedPointer<AddressBlock> newAddressBlock) const;

    /*!
     *  Reads the type identifier.
     *
     *      @param [in] addressBlockNode    XML description of the address block.
     *      @param [in] newAddressBlock     The new address block.
     */
    void parseTypeIdentifier(QDomNode const& addressBlockNode, QSharedPointer<AddressBlock> newAddressBlock) const;

    /*!
     *  Reads the range.
     *
     *      @param [in] addressBlockNode    XML description of the address block.
     *      @param [in] newAddressBlock     The new address block.
     */
    void parseRange(QDomNode const& addressBlockNode, QSharedPointer<AddressBlock> newAddressBlock) const;

    /*!
     *  Reads the width.
     *
     *      @param [in] addressBlockNode    XML description of the address block.
     *      @param [in] newAddressBlock     The new address block.
     */
    void parseWidth(QDomNode const& addressBlockNode, QSharedPointer<AddressBlock> newAddressBlock) const;

    /*!
     *  Reads the usage.
     *
     *      @param [in] addressBlockNode    XML description of the address block.
     *      @param [in] newAddressBlock     The new address block.
     */
    void parseUsage(QDomNode const& addressBlockNode, QSharedPointer<AddressBlock> newAddressBlock) const;

    /*!
     *  Reads the volatile value.
     *
     *      @param [in] addressBlockNode    XML description of the address block.
     *      @param [in] newAddressBlock     The new address block.
     */
    void parseVolatile(QDomNode const& addressBlockNode, QSharedPointer<AddressBlock> newAddressBlock) const;

    /*!
     *  Reads the access value.
     *
     *      @param [in] addressBlockNode    XML description of the address block.
     *      @param [in] newAddressBlock     The new address block.
     */
    void parseAccess(QDomNode const& addressBlockNode, QSharedPointer<AddressBlock> newAddressBlock) const;

    /*!
     *  Reads the parameters.
     *
     *      @param [in] addressBlockNode    XML description of the address block.
     *      @param [in] newAddressBlock     The new address block.
     */
    void parseParameters(QDomNode const& addressBlockNode, QSharedPointer<AddressBlock> newAddressBlock) const;

    /*!
     *  Reads the register data.
     *
     *      @param [in] addressBlockNode    XML description of the address block.
     *      @param [in] newAddressBlock     The new address block.
     */
    void parseRegisterData(QDomNode const& addressBlockNode, QSharedPointer<AddressBlock> newAddressBlock) const;
};

#endif // ADDRESSBLOCKREADER_H