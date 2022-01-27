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

#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/common/Extendable.h>
#include <IPXACTmodels/common/NameGroup.h>

//-----------------------------------------------------------------------------
//! Contains elements for sub space maps.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT SubSpaceMap : public NameGroup, public Extendable
{

public:

    /*!
     *  Constructor for sub space maps.
     *
     *      @param [in] name    Name of the memory map base.
     */
    SubSpaceMap(QString const& name = QString());

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
	virtual QSharedPointer<SubSpaceMap> clone() const;

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

    /*!
     *  Get the presence of the subspace map.
     *
     *      @return The presence of the subspace map.
     */
    QString getIsPresent() const;

    /*!
     *  Set a new presence for the subspace map.
     *
     *      @param [in] newIsPresent    The new presence value.
     */
    void setIsPresent(QString const& newIsPresent);

    /*!
     *  Get the base address of the subspace map.
     *
     *      @return The base address of the subspace map.
     */
    QString getBaseAddress() const;

    /*!
     *  Set a new base address for the subspace map.
     *
     *      @param [in] newBaseAddress  The new base address.
     */
    void setBaseAddress(QString const& newBaseAddress);

    /*!
     *  Get the contained parameters.
     *
     *      @return Pointer to a list containing the parameters.
     */
    QSharedPointer<QList<QSharedPointer<Parameter> > > getParameters() const;

    /*!
     *  Set the parameters.
     *
     *      @param [in] newParameters   Pointer to a list containing the new parameters.
     */
    void setParameters(QSharedPointer<QList<QSharedPointer<Parameter> > > newParameters);

private:

    /*!
     *  Copies parameters from another subspace map.
     *
     *      @param [in] other   The subspace map to copy parameters from.
     */
    void copyParameters(SubSpaceMap const& other);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    // The reference to the master interface whose address space is being mapped.
    QString masterReference_;

    //! Reference to a segment in the address space of the referenced master reference.
    QString segmentRef_;

    //! Presence of the subspace map.
    QString isPresent_;

    //! Base address of the subspace map.
    QString baseAddress_;

    //! List of contained parameters.
    QSharedPointer<QList<QSharedPointer<Parameter> > > parameters_;
};

#endif // SUBSPACEMAP_H
