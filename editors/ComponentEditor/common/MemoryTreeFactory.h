//-----------------------------------------------------------------------------
// File: MemoryTreeFactory.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 31.8.2023
//
// Description:
// Creates a tree representation of component memory items.
//-----------------------------------------------------------------------------

#ifndef MEMORYTREEFACTORY_H
#define MEMORYTREEFACTORY_H

#include <common/widgets/TreeItemSelector/TreeItem.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/MemoryMapBase.h>
#include <IPXACTmodels/Component/MemoryBlockBase.h>
#include <IPXACTmodels/Component/RegisterBase.h>

#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Creates a tree representation of component memory items.
//-----------------------------------------------------------------------------
class MemoryTreeFactory
{
public:


    /*!
     *  The constructor.
     *
     *      @param [in] memoryMaps      The memory maps available in the tree.
     *      @param [in] addressSpaces   The address spaces available in the tree.
     */
    explicit MemoryTreeFactory(QSharedPointer<QList<QSharedPointer<MemoryMap> > > memoryMaps,
        QSharedPointer<QList<QSharedPointer<AddressSpace> > > addressSpaces);

    //! The destructor.
    ~MemoryTreeFactory() = default;

    //! No copy constructor.
    MemoryTreeFactory(MemoryTreeFactory const&) = delete;

    //! No assignment operator.
    MemoryTreeFactory& operator=(MemoryTreeFactory const&) = delete;
    
    /*!
     *  Creates a tree of component memory items.
     *
     *      @return The root item of the tree.
     */
    TreeItem* create() const;

private:
  
    /*!
     *  Creates a tree item for an address block.
     *
     *      @param [in] memoryBlock The represented address block.
     *      @param [in] parentItem  The parent item.
     */
    void createAddressBlockItem(QSharedPointer<MemoryBlockBase> memoryBlock, TreeItem* parentItem) const;

    /*!
     *  Creates a tree item for a register file.
     *
     *      @param [in] registerFile The represented register or register file.
     *      @param [in] parentItem   The parent item.
     */
    void createRegisterItem(QSharedPointer<RegisterBase> registerBase, TreeItem* parentItem) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The available memory maps.
    QSharedPointer<QList<QSharedPointer<MemoryMap> > > memoryMaps_;

    //! The available address spaces.
    QSharedPointer<QList<QSharedPointer<AddressSpace> > > addressSpaces_;
};

#endif // MEMORYTREEFACTORY_H