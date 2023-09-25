//-----------------------------------------------------------------------------
// File: AddressBlockWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 30.09.2015
//
// Description:
// Writer for ipxact:addressBlock element.
//-----------------------------------------------------------------------------

#ifndef ADDRESSBLOCKWRITER_H
#define ADDRESSBLOCKWRITER_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/Component/MemoryBlockBaseWriter.h>

#include <QXmlStreamWriter>
#include <QObject>
#include <QSharedPointer>

class AddressBlock;

//-----------------------------------------------------------------------------
//! Writer for ipxact:addressBlock element.
//-----------------------------------------------------------------------------
namespace AddressBlockWriter
{

    /*!
     *  Write an address block to an XML file.
     *
     *      @param [in] writer          The used XML writer.
     *      @param [in] addressBlock    The address block to be written.
     */
    IPXACTMODELS_EXPORT void writeAddressBlock(QXmlStreamWriter& writer, QSharedPointer<AddressBlock> addressBlock, Document::Revision docRevision);

    namespace Details
    {

        /*!
         *  Write the type identifier.
         *
         *      @param [in] writer          Used XML writer.
         *      @param [in] addressBlock    Selected address block.
         */
        void writeTypeIdentifier(QXmlStreamWriter& writer, QSharedPointer<AddressBlock> addressBlock);

        /*!
         *  Write the usage.
         *
         *      @param [in] writer          Used XML writer.
         *      @param [in] addressBlock    Selected address block.
         */
        void writeUsage(QXmlStreamWriter& writer, QSharedPointer<AddressBlock> addressBlock);

        /*!
         *  Write the volatile status.
         *
         *      @param [in] writer          Used XML writer.
         *      @param [in] addressBlock    Selected address block.
         */
        void writeVolatile(QXmlStreamWriter& writer, QSharedPointer<AddressBlock> addressBlock);

        /*!
         *  Write the access value.
         *
         *      @param [in] writer          Used XML writer.
         *      @param [in] addressBlock    Selected address block.
         */
        void writeAccess(QXmlStreamWriter& writer, QSharedPointer<AddressBlock> addressBlock);

        /*!
         *  Write register data.
         *
         *      @param [in] writer          Used XML writer.
         *      @param [in] addressBlock    Selected address block.
         */
        void writeRegisterData(QXmlStreamWriter& writer, QSharedPointer<AddressBlock> addressBlock, Document::Revision docRevision);

        /*!
         *	Write the address block definition reference.
         *  
         *      @param [in] writer          Used XML writer.
         *      @param [in] addressBlock    Selected address block.
         */
        void writeDefinitionReference(QXmlStreamWriter& writer, QSharedPointer<AddressBlock> addressBlock);

        /*!
         *	Write the access policies of the address block.
         *  
         *      @param [in] writer          Used XML writer.
         *      @param [in] addressBlock    Selected address block.
         */
        void writeAccessPolicies(QXmlStreamWriter& writer, QSharedPointer<AddressBlock> addressBlock);
    }
}

#endif // ADDRESSBLOCKWRITER_H