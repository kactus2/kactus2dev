//-----------------------------------------------------------------------------
// File: AddressBlockWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 30.09.2015
//
// Description:
// Writer class for ipxact:addressBlock element.
//-----------------------------------------------------------------------------

#ifndef ADDRESSBLOCKWRITER_H
#define ADDRESSBLOCKWRITER_H

#include <IPXACTmodels/ipxactmodels_global.h>
#include <IPXACTmodels/common/CommonItemsWriter.h>

#include <QXmlStreamWriter>
#include <QObject>
#include <QSharedPointer>

class AddressBlock;

//-----------------------------------------------------------------------------
//! Writer class for ipxact:addressBlock element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT AddressBlockWriter : public CommonItemsWriter
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent  Owner of this writer.
     */
    AddressBlockWriter(QObject* parent = 0);

    /*!
     *  The destructor.
     */
    ~AddressBlockWriter();

    /*!
     *  Write an address block to an XML file.
     *
     *      @param [in] writer          The used XML writer.
     *      @param [in] addressBlock    The address block to be written.
     */
    void writeAddressBlock(QXmlStreamWriter& writer, QSharedPointer<AddressBlock> addressBlock) const;

private:

    //! No copying allowed.
    AddressBlockWriter(AddressBlockWriter const& rhs);
    AddressBlockWriter& operator=(AddressBlockWriter const& rhs);

    /*!
     *  Write the name group.
     *
     *      @param [in] writer          Used XML writer.
     *      @param [in] addressBlock    Selected address block.
     */
    void writeNameGroup(QXmlStreamWriter& writer, QSharedPointer<AddressBlock> addressBlock) const;

    /*!
     *  Write the type identifier.
     *
     *      @param [in] writer          Used XML writer.
     *      @param [in] addressBlock    Selected address block.
     */
    void writeTypeIdentifier(QXmlStreamWriter& writer, QSharedPointer<AddressBlock> addressBlock) const;

    /*!
     *  Write the usage.
     *
     *      @param [in] writer          Used XML writer.
     *      @param [in] addressBlock    Selected address block.
     */
    void writeUsage(QXmlStreamWriter& writer, QSharedPointer<AddressBlock> addressBlock) const;

    /*!
     *  Write the volatile status.
     *
     *      @param [in] writer          Used XML writer.
     *      @param [in] addressBlock    Selected address block.
     */
    void writeVolatile(QXmlStreamWriter& writer, QSharedPointer<AddressBlock> addressBlock) const;

    /*!
     *  Write the access value.
     *
     *      @param [in] writer          Used XML writer.
     *      @param [in] addressBlock    Selected address block.
     */
    void writeAccess(QXmlStreamWriter& writer, QSharedPointer<AddressBlock> addressBlock) const;

    /*!
     *  Write register data.
     *
     *      @param [in] writer          Used XML writer.
     *      @param [in] addressBlock    Selected address block.
     */
    void writeRegisterData(QXmlStreamWriter& writer, QSharedPointer<AddressBlock> addressBlock) const;
};

#endif // ADDRESSBLOCKWRITER_H