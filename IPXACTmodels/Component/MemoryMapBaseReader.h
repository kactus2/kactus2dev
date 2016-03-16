//-----------------------------------------------------------------------------
// File: MemoryMapBaseReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 01.10.2015
//
// Description:
// Reader class for ipxact:MemoryMapBase element.
//-----------------------------------------------------------------------------

#ifndef MemoryMapBaseREADER_H
#define MemoryMapBaseREADER_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/common/CommonItemsReader.h>

#include <QObject>
#include <QSharedPointer>
#include <QDomNode>

class MemoryMapBase;
class MemoryRemap;

//-----------------------------------------------------------------------------
//! Reader class for ipxact:MemoryMapBase element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT MemoryMapBaseReader : public CommonItemsReader
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent  The owner of this reader.
     */
    MemoryMapBaseReader(QObject* parent = 0);

    /*!
     *  The destructor.
     */
    ~MemoryMapBaseReader();

    /*!
     *  Creates a new memory map from a given memory map node.
     *
     *      @param [in] MemoryMapBaseNode   XML description of the memory map.
     *
     *      @return The created memory map.
     */
    void readMemoryMapBase(QDomNode const& MemoryMapBaseNode, QSharedPointer<MemoryMapBase> newMemoryMapBase) const;

protected:

    /*!
     *  Reads the name group.
     *
     *      @param [in] MemoryMapBaseBaseNode   XML description of the memory map base.
     *      @param [in] newMemoryMapBaseBase    The new memory map base item.
     */
    void parseNameGroup(QDomNode const& MemoryMapBaseBaseNode, QSharedPointer<MemoryMapBase> newMemoryMapBase) const;

    /*!
     *  Reads the isPresent value.
     *
     *      @param [in] MemoryMapBaseBaseNode   XML description of the memory map base.
     *      @param [in] newMemoryMapBaseBase    The new memory map base item.
     */
    void parsePresence(QDomNode const& MemoryMapBaseBaseNode, QSharedPointer<MemoryMapBase> newMemoryMapBase) const;

    /*!
     *  Reads the contained memory blocks.
     *
     *      @param [in] MemoryMapBaseBaseNode   XML description of the memory map base.
     *      @param [in] newMemoryMapBaseBase    The new memory map base item.
     */
    void parseMemoryBlocks(QDomNode const& MemoryMapBaseBaseNode, QSharedPointer<MemoryMapBase> newMemoryMapBase)
        const;

private:

    //! No copying allowed.
    MemoryMapBaseReader(MemoryMapBaseReader const& rhs);
    MemoryMapBaseReader& operator=(MemoryMapBaseReader const& rhs);
};

#endif // MemoryMapBaseREADER_H