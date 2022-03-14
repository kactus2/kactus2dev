//-----------------------------------------------------------------------------
// File: SubSpaceMap.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 17.01.2022
//
// Description:
// Contains elements for sub space maps.
//-----------------------------------------------------------------------------

#ifndef SUBSPACEMAP_H
#define SUBSPACEMAP_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/Component/MemoryBlockBase.h>

//-----------------------------------------------------------------------------
//! Contains elements for sub space maps.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT SubSpaceMap : public MemoryBlockBase
{

public:

    /*!
     *  Constructor for sub space maps.
     *
     *      @param [in] name    Name of the memory map base.
     */
    SubSpaceMap(QString const& name = QString(), QString const& baseAddress = QString());

    /*!
     *  Copy constructor.
     *
     *      @param [in] other   The subspace map to be copied.
     */
    SubSpaceMap(const SubSpaceMap& other);

    /*!
     *  Assignment operator.
     *
     *      @param [in] other   The subspace map to be assigned.
     */
    SubSpaceMap &operator=(const SubSpaceMap& other);

    /*!
     *  The destructor.
     */
    virtual ~SubSpaceMap() = default;

    /*!
     *  Clone this subspace map item and return pointer to the copy.
	 * 
	 *      @return Pointer to the cloned subspace map item.
	 */
	virtual QSharedPointer<MemoryBlockBase> clone() const;

    /*!
     *  Get the master interface reference.
     *
     *      @return The master reference.
     */
    QString getMasterReference() const;

    /*!
     *  Set the reference to the master interface.
     *
     *      @param [in] newMasterReference  The new master interface reference.
     */
    void setMasterReference(QString const& newMasterReference);

    /*!
     *  Get the segment reference.
     *
     *      @return The segment reference.
     */
    QString getSegmentReference() const;

    /*!
     *  Set the segment reference.
     *
     *      @param [in] newSegmentReference     The new segment reference.
     */
    void setSegmentReference(QString const& newSegmentReference);

private:

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    // The reference to the master interface whose address space is being mapped.
    QString masterReference_;

    //! Reference to a segment in the address space of the referenced master reference.
    QString segmentRef_;
};

#endif // SUBSPACEMAP_H
