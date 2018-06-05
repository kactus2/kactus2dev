//-----------------------------------------------------------------------------
// File: MemoryMapBase.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 30.09.2015
//
// Description:
// Contains common memory-map elements.
//-----------------------------------------------------------------------------

#ifndef MEMORYMAPBASE_H
#define MEMORYMAPBASE_H

#include "MemoryBlockBase.h"

#include <IPXACTmodels/common/NameGroup.h>
#include <IPXACTmodels/ipxactmodels_global.h>

#include <QList>
#include <QString>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Contains common memory-map elements.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT MemoryMapBase : public NameGroup
{

public:

    /*!
     *  [Description].
     *
     *      @param [in] name    Name of the memory map base.
     */
    MemoryMapBase(QString const& name = QString());

    /*!
     *  Copy constructor.
     *
     *      @param [in] other   The abstract memory map to be copied.
     */
    MemoryMapBase(const MemoryMapBase& other);

    /*!
     *  Assignment operator.
     *
     *      @param [in] other   The abstract memory map to be assigned.
     */
    MemoryMapBase &operator=(const MemoryMapBase& other);

    /*!
     *  The destructor.
     */
    virtual ~MemoryMapBase();
    
    /*!
     *  Get the element name of the memory map.
     *
     *      @return Memory map for default memory maps.
     */
    virtual QString elementName() const;

    /*!
     *  Get the isPresent value.
     *
     *      @return The isPresent value.
     */
    QString getIsPresent() const;

    /*!
     *  Set the value for the isPresent.
     *
     *      @param [in] newIsPresent    The new IsPresent value.
     */
    void setIsPresent(QString const& newIsPresent);

    /*!
     *  Get a list of the contained memory map blocks.
     *
     *      @return Pointer to a list containing the memory blocks stored in this instance.
     */
    QSharedPointer<QList<QSharedPointer<MemoryBlockBase> > > getMemoryBlocks() const;

    /*!
     *  Set the memory map blocks for this memory map.
     *
     *      @param [in] newItems    A Pointer to a list containing the blocks to be stored in this memory map.
     */
    void setMemoryBlocks(QSharedPointer<QList<QSharedPointer<MemoryBlockBase> > > newMemoryBlocks);

    /*!
     *  Check if the memory map contains any sub items.
     *
     *      @return True, if the memory map contains sub items, false otherwise.
     */
    bool hasMemoryBlocks() const;

private:

    /*!
     *  Copy the contained memory blocks.
     *
     *      @param [in] other   The memory map base being copied.
     */
    void copyMemoryBlocks(const MemoryMapBase& other);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Defines the presence.
    QString isPresent_;

    //! Contains pointers to the contained memory blocks.
    QSharedPointer<QList<QSharedPointer<MemoryBlockBase> > > memoryBlocks_;
};

Q_DECLARE_METATYPE(QSharedPointer<MemoryMapBase>);

#endif // MEMORYMAPBASE_H
