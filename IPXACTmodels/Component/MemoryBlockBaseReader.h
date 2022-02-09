//-----------------------------------------------------------------------------
// File: MemoryBlockBaseReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 27.01.2022
//
// Description:
// Base reader class for memory map sub elements.
//-----------------------------------------------------------------------------

#ifndef MEMORYBLOCKBASEREADER_H
#define MEMORYBLOCKBASEREADER_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/common/CommonItemsReader.h>

#include <QSharedPointer>
#include <QDomNode>

class MemoryBlockBase;

//-----------------------------------------------------------------------------
//! Base reader class for memory map sub elements.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT MemoryBlockBaseReader : public CommonItemsReader
{
public:

    /*!
     *  The constructor.
     */
    MemoryBlockBaseReader();

    /*!
     *  The destructor.
     */
    ~MemoryBlockBaseReader() = default;

    //! No copying allowed.
    MemoryBlockBaseReader(MemoryBlockBaseReader const& rhs) = delete;
    MemoryBlockBaseReader& operator=(MemoryBlockBaseReader const& rhs) = delete;

protected:

    /*!
     *  Reads the name group.
     *
     *      @param [in] memoryBlockNode     XML description of the memory block.
     *      @param [in] newMemoryBlock      The new memory block.
     */
    void parseNameGroup(QDomNode const& memoryBlockNode, QSharedPointer<MemoryBlockBase> newMemoryBlock) const;

    /*!
     *  Reads the presence.
     *
     *      @param [in] memoryBlockNode     XML description of the memory block.
     *      @param [in] newMemoryBlock      The new memory block.
     */
    void parsePresence(QDomNode const& memoryBlockNode, QSharedPointer<MemoryBlockBase> newMemoryBlock) const;

    /*!
     *  Reads the base address.
     *
     *      @param [in] memoryBlockNode     XML description of the memory block.
     *      @param [in] newMemoryBlock      The new memory block.
     */
    void parseBaseAddress(QDomNode const& memoryBlockNode, QSharedPointer<MemoryBlockBase> newMemoryBlock) const;

    /*!
     *  Reads the parameters.
     *
     *      @param [in] memoryBlockNode     XML description of the memory block.
     *      @param [in] newMemoryBlock      The new memory block.
     */
    void parseParameters(QDomNode const& addressBlockNode, QSharedPointer<MemoryBlockBase> newMemoryBlock) const;
};

#endif // MEMORYBLOCKBASEREADER_H