//-----------------------------------------------------------------------------
// File: MemoryMapReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 01.10.2015
//
// Description:
// Reader class for ipxact:memoryMap element.
//-----------------------------------------------------------------------------

#ifndef MEMORYMAPREADER_H
#define MEMORYMAPREADER_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/common/CommonItemsReader.h>
#include "MemoryMapBaseReader.h"

#include <QObject>
#include <QSharedPointer>
#include <QDomNode>

class MemoryMap;
class MemoryMapBase;
class MemoryRemap;

//-----------------------------------------------------------------------------
//! Reader class for ipxact:memoryMap element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT MemoryMapReader : public MemoryMapBaseReader
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent  The owner of this reader.
     */
    MemoryMapReader(QObject* parent = 0);

    /*!
     *  The destructor.
     */
    ~MemoryMapReader();

    /*!
     *  Creates a new memory map from a given memory map node.
     *
     *      @param [in] memoryMapNode   XML description of the memory map.
     *
     *      @return The created memory map.
     */
    QSharedPointer<MemoryMap> createMemoryMapFrom(QDomNode const& memoryMapNode) const;

private:

    //! No copying allowed.
    MemoryMapReader(MemoryMapReader const& rhs);
    MemoryMapReader& operator=(MemoryMapReader const& rhs);

    /*!
     *  Reads the contained memory blocks.
     *
     *      @param [in] memoryMapBaseNode   XML description of the memory map base.
     *      @param [in] newMemoryMapBase    The new memory map base item.
     */
    void parseMemoryBlocks(QDomNode const& memoryMapBaseNode, QSharedPointer<MemoryMapBase> newMemoryMapBase)
        const;

    /*!
     *  Reads the memory remaps.
     *
     *      @param [in] memoryMapNode   XML description of the memory map.
     *      @param [in] newMemoryMap    The new memory map item.
     */
    void parseMemoryRemaps(QDomNode const& memoryMapNode, QSharedPointer<MemoryMap> newMemoryMap) const;

    /*!
     *  Creates a new memory remap from a given memory remap element.
     *
     *      @param [in] memoryRemapElement  XML description of the memory remap.
     *
     *      @return Pointer to the created memory remap.
     */
    QSharedPointer<MemoryRemap> createSingleMemoryRemap(QDomElement const& memoryRemapElement) const;

    /*!
     *  Reads the address unit bits.
     *
     *      @param [in] memoryMapNode   XML description of the memory map.
     *      @param [in] newMemoryMap    The new memory map item.
     */
    void parseAddressUnitBits(QDomNode const& memoryMapNode, QSharedPointer<MemoryMap> newMemoryMap) const;

    /*!
     *  Reads the shared value.
     *
     *      @param [in] memoryMapNode   XML description of the memory map.
     *      @param [in] newMemoryMap    The new memory map item.
     */
    void parseShared(QDomNode const& memoryMapNode, QSharedPointer<MemoryMap> newMemoryMap) const;
};

#endif // MEMORYMAPREADER_H