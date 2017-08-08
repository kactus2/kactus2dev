//-----------------------------------------------------------------------------
// File: IndirectInterfaceWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 07.08.2017
//
// Description:
// XML writer class for IP-XACT Indirect Interface element.
//-----------------------------------------------------------------------------

#ifndef INDIRECTINTERFACEWriter_H
#define INDIRECTINTERFACEWriter_H

#include <QSharedPointer>
#include <QDomNode>

#include <IPXACTmodels/common/CommonItemsWriter.h>

class IndirectInterface;

//-----------------------------------------------------------------------------
//! XML writer class for IP-XACT Indirect Interface element.
//-----------------------------------------------------------------------------
class IndirectInterfaceWriter : public CommonItemsWriter
{
public:

	//! The constructor.
	IndirectInterfaceWriter();

	//! The destructor.
	virtual ~IndirectInterfaceWriter();

    void writeIndirectInterface(QXmlStreamWriter& writer, QSharedPointer<IndirectInterface> indirectInterface) const;

private:

	// Disable copying.
	IndirectInterfaceWriter(IndirectInterfaceWriter const& rhs);
	IndirectInterfaceWriter& operator=(IndirectInterfaceWriter const& rhs);

    /*!
     *  Write the name group of the indirect interface.
     *
     *      @param [in] writer              The used XML writer.
     *      @param [in] indirectInterface   The indirect interface being written.
     */
    void writeNameGroup(QXmlStreamWriter& writer, QSharedPointer<IndirectInterface> indirectInterface) const;

    /*!
     *  Write the indirect address and data fields of the indirect interface.
     *
     *      @param [in] writer              The used XML writer.
     *      @param [in] indirectInterface   The indirect interface being written.
     */
    void writeIndirectFields(QXmlStreamWriter &writer, QSharedPointer<IndirectInterface> indirectInterface) const;
    
    /*!
     *  Write the memory map reference of the indirect interface.
     *
     *      @param [in] writer              The used XML writer.
     *      @param [in] indirectInterface   The indirect interface being written.
     */
    void writeMemoryMapReference(QXmlStreamWriter& writer,
        QSharedPointer<IndirectInterface> indirectInterface) const;
    
    /*!
     *  Write the transparent bridges of the indirect interface.
     *
     *      @param [in] writer              The used XML writer.
     *      @param [in] indirectInterface   The indirect interface being written.
     */
    void writeTransparentBridges(QXmlStreamWriter& writer,
        QSharedPointer<IndirectInterface> indirectInterface) const;
         
    /*!
     *  Write the bits in lau of the indirect interface.
     *
     *      @param [in] writer              The used XML writer.
     *      @param [in] indirectInterface   The indirect interface being written.
     */
    void writeBitsInLau(QXmlStreamWriter& writer, QSharedPointer<IndirectInterface> indirectInterface) const;

    /*!
     *  Write the endianness of the indirect interface.
     *
     *      @param [in] writer              The used XML writer.
     *      @param [in] indirectInterface   The indirect interface being written.
     */
    void writeEndianness(QXmlStreamWriter& writer, QSharedPointer<IndirectInterface> indirectInterface) const;
};

#endif // INDIRECTINTERFACEWriter_H
