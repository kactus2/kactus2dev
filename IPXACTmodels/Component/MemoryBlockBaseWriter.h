//-----------------------------------------------------------------------------
// File: MemoryBlockBaseWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 28.01.2022
//
// Description:
// Writer class for base memory block element.
//-----------------------------------------------------------------------------

#ifndef MEMORYBLOCKBASEWRITER_H
#define MEMORYBLOCKBASEWRITER_H

#include <IPXACTmodels/ipxactmodels_global.h>
#include <IPXACTmodels/common/CommonItemsWriter.h>

#include <QXmlStreamWriter>
#include <QSharedPointer>
#include <QObject>

class MemoryBlockBase;

//-----------------------------------------------------------------------------
//! Writer class for base memory block element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT MemoryBlockBaseWriter : public CommonItemsWriter
{
public:

    /*!
     *  The constructor.
     */
    MemoryBlockBaseWriter();

    /*!
     *  The destructor.
     */
    ~MemoryBlockBaseWriter() = default;

    //! No copying allowed.
    MemoryBlockBaseWriter(MemoryBlockBaseWriter const& rhs);
    MemoryBlockBaseWriter& operator=(MemoryBlockBaseWriter const& rhs);

protected:

    /*!
     *  Write the name group.
     *
     *      @param [in] writer          Used XML writer.
     *      @param [in] memoryBlock     Selected memory block.
     */
    void writeNameGroup(QXmlStreamWriter& writer, QSharedPointer<MemoryBlockBase> memoryBlock) const;

    /*!
     *  Write the base address.
     *
     *      @param [in] writer          Used XML writer.
     *      @param [in] memoryBlock     Selected memory block.
     */
    void writeBaseAddress(QXmlStreamWriter& writer, QSharedPointer<MemoryBlockBase> memoryBlock) const;
};

#endif // ADDRESSBLOCKWRITER_H