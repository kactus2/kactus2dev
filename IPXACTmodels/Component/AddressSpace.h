//-----------------------------------------------------------------------------
// File: AddressSpace.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: 
// Date:
//
// Description:
// Describes the ipxact:addressSpace element.
//-----------------------------------------------------------------------------

#ifndef ADDRESSSPACE_H
#define ADDRESSSPACE_H

#include "MemoryMapBase.h"

#include <IPXACTmodels/Component/Segment.h>

#include <IPXACTmodels/ipxactmodels_global.h>
#include <IPXACTmodels/common/Extendable.h>

#include <QSharedPointer>
#include <QString>
#include <QList>

class Parameter;

//-----------------------------------------------------------------------------
//! Describes the ipxact:addressSpace element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT AddressSpace : public NameGroup, public Extendable
{

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] name    Name of the address space.
	 *      @param [in] range   The range.
	 *      @param [in] width   The width.
	 */
	AddressSpace(QString const& name = QString(), QString const& range = QString(),
        QString const& width = QString());

	//! The copy constructor.
	AddressSpace(const AddressSpace &other);

	//! The assignment operator.
	AddressSpace &operator=(const AddressSpace &other);

	//! The destructor.
	~AddressSpace();

    /*!
     *  Gets the presence.
     *
     *      @return The presence value.
     */
    QString getIsPresent() const;

    /*!
     *  Set the presence.
     *
     *      @param [in] newIsPresent    The new presence value.
     */
    void setIsPresent(QString const& newIsPresent);

	/*!
     *  Get the address unit bits
	 *
	 *      @return The address unit bits.
	 */
    QString getAddressUnitBits() const;

	/*!
     *  Get the range of the address space.
	 *
	 *      @return The range of the address space
	 */
    QString getRange() const;

	/*!
     *  Get the width of a row in an address space.
	 *
	 *      @return The width of the address space.
	 */
    QString getWidth() const;

	/*!
     *  Set the address unit bits.
	 *
	 *      @param [in] newAddressUnitBits  The new address unit bits.
	 */
    void setAddressUnitBits(QString const& newAddressUnitBits);

	/*!
     *  Set the range for the address space.
	 *
	 *      @param [in] newRange    The new range.
	 */
    void setRange(QString const& newRange);

	/*!
     *  Set the width of the address space.
	 *
	 *      @param [in] newWidth    The new width.
	 */
    void setWidth(QString const& newWidth);

	/*!
     *  Get the pointer to the local memory map.
	 *
	 *      @return A pointer to the local memory map.
	 */
	QSharedPointer<MemoryMapBase> getLocalMemoryMap();

	/*!
     *  Check if the address space contains a local memory map.
	 *
	 *      @return True if a local memory map is found.
	 */
	bool hasLocalMemoryMap() const;

	/*!
     *  Set the local memory map.
	 *
	 *      @param [in] localMemoryMap  A pointer to the new local memory map
	 */
	void setLocalMemoryMap(QSharedPointer<MemoryMapBase> localMemoryMap);

	/*!
     *  Get the parameters of the address space.
	 *
	 *      @return Pointer to a lis containing pointers to the parameters.
	 */
    QSharedPointer<QList<QSharedPointer<Parameter> > > getParameters() const;

    /*!
     *  Set the parameters.
     *
     *      @param [in] newParameters   Pointer to a list containing the new parameters.
     */
    void setParameters(QSharedPointer<QList<QSharedPointer<Parameter> > > newParameters);

	/*!
     *  Get the segments contained in this address space.
	 *
	 *      @return Pointer to a list containing pointers to the segments.
	 */
    QSharedPointer<QList<QSharedPointer<Segment> > > getSegments() const;

    /*!
     *  Set the segments for this address space.
 	 *
	 *      @param [in] segments    Pointer to a list containing the new segments for the address space.
	 */
    void setSegments(QSharedPointer<QList<QSharedPointer<Segment> > > segments);

private:

    /*!
     *  Copy the local memory map.
     *
     *      @param [in] other   The address space being copied.
     */
    void copyLocalMemoryMap(const AddressSpace& other);

    /*!
     *  Copy the segments.
     *
     *      @param [in] other   The address space being copied.
     */
    void copySegments(const AddressSpace& other);

    /*!
     *  Copy the parameters.
     *
     *      @param [in] other   The address space being copied.
     */
    void copyParameters(const AddressSpace& other);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Presence of the address space.
	QString isPresent_;

	//! Address range of an address space.
    QString range_;

	//! The bit width of a row in the address space.
    QString width_;

	//! Contains the segments of the address space.
    QSharedPointer<QList<QSharedPointer<Segment> > > segments_;

	//! The number of data bits in each address increment.
    QString addressUnitBits_;

	//! Block of memory that can only be accessed by the master interfaces.
	QSharedPointer<MemoryMapBase> localMemoryMap_;

	//! Contains the parameters for the addressSpace.
    QSharedPointer<QList<QSharedPointer<Parameter> > > parameters_;
};

Q_DECLARE_METATYPE(QSharedPointer<AddressSpace>);

#endif // ADDRESSSPACE_H
