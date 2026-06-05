//-----------------------------------------------------------------------------
// File: MemoryBlockBase.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 29.09.2015
//
// Description:
// Describes the base class for ipxact:addressBlock, ipxact:bank and ipxact:subspaceMap elements.
//-----------------------------------------------------------------------------

#ifndef MEMORYBLOCKBASE_H
#define MEMORYBLOCKBASE_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/common/Extendable.h>
#include <IPXACTmodels/common/NameGroup.h>
#include <IPXACTmodels/common/BooleanValue.h>
#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/common/AccessTypes.h>

#include <QMap>
#include <QString>
#include <QSharedPointer>
#include <QList>

class Parameter;
class MemoryArray;
class AccessPolicy;
class AccessHandle;

//-----------------------------------------------------------------------------
//! Describes the base class for ipxact:addressBlock, ipxact:bank and ipxact:subspaceMap elements.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT MemoryBlockBase : public NameGroup, public Extendable
{

public:

	/*!
	 *  The constructor.
	 *
	 *    @param [in] name            Name of the memory block.
	 *    @param [in] baseAddress     Base address of the memory block.
	 */
	MemoryBlockBase(QString const& name = QString(), QString const& baseAddress = QString());

	//! Copy constructor.
	MemoryBlockBase(const MemoryBlockBase& other);

	//! The assignment operator.
	MemoryBlockBase& operator=(const MemoryBlockBase& other);

	//! The destructor.
	virtual ~MemoryBlockBase();

	/*!
     *  Clone this memory map item and return pointer to the copy.
	 * 
	 *    @return Pointer to the cloned memory map item.
	 */
	virtual QSharedPointer<MemoryBlockBase> clone() const = 0;

    /*!
     *  Get the presence of the memory block.
     *
     *    @return The isPresent value.
     */
    QString getIsPresent() const;

    /*!
     *  Set the presence of the memory block.
     *
     *    @param [in] newIsPresent    The new isPresent value.
     */
    void setIsPresent(QString const& newIsPresent);

    /*! 
     *  Get the access handles associated with the memory block.
     *
     *    @return Shared pointer to the list of access handles.
     */
    QSharedPointer<QList<QSharedPointer<AccessHandle> > > getAccessHandles() const;

    /*! 
     *  Set the access handles associated with the memory block.
     *
     *    @param [in] accessHandles  Shared pointer to the access handle list.
     */
    void setAccessHandles(QSharedPointer<QList<QSharedPointer<AccessHandle> > > accessHandles);

    /*!
     *  Get the base address of the block.
	 *
	 *    @return The base address
	 */
    QString getBaseAddress() const;

	/*!
     *  Set base address for the block.
	 *
	 *    @param [in} baseAddress     The new base address
	 */
	virtual void setBaseAddress(QString const& baseAddress);

    /*!
     *  Get the type of usage for the memory block.
	 *
	 *    @return Usage.
	 */
	General::Usage getUsage() const;

	/*!
     *  Set the type of usage for the memory block.
	 *
	 *    @param [in] newUsage    The new usage type.
	 */
	void setUsage(General::Usage newUsage);

	/*!
     *  Get the volatile information.
	 *
	 *      return Volatile information. Empty string means unspecified volatile state.
	 */
    QString getVolatile() const;

	/*!
     *  Set the volatile setting.
	 *
	 *    @param [in] newVolatileSetting  True for volatile memory block, false otherwise.
	 */
    void setVolatile(bool newVolatileValue);

    /*!
     *  Set the volatile value to unspecified.
     */
    void clearVolatile();

	/*!
     *  Get the access information.
	 *
	 *    @return Accessibility of the memory block data.
	 */
    AccessTypes::Access getAccess() const;

	/*!
     *  Set the accessibility of the memory block.
	 *
	 *    @param [in] newAccess   The new access type of the memory block.
	 */
    void setAccess(AccessTypes::Access newAccess);

    /*!
     *	Get the access policies of the memory block.
     *	    
     * 	    @return Pointer to a list of access policies contained within the memory block.
     */
    QSharedPointer<QList<QSharedPointer<AccessPolicy> > > getAccessPolicies() const;

    /*!
     *	Set the access policies of the memory block.
     *  
     *    @param [in] newAccessPolicies     The access policies to set.
     */
    void setAccessPolicies(QSharedPointer<QList<QSharedPointer<AccessPolicy> > > newAccessPolicies);

    /*!
     *  Get the contained parameters.
     *
     *    @return Pointer to a list containing the parameters.
     */
    QSharedPointer<QList<QSharedPointer<Parameter> > > getParameters() const;

    /*!
     *  Set the parameters.
     *
     *    @param [in] newParameters   Pointer to a list containing the new parameters.
     */
    void setParameters(QSharedPointer<QList<QSharedPointer<Parameter> > > newParameters);

private:

    //! Contains the presence of the block.
    QString isPresent_;

	//! Contains the base address of the block.
    QString baseAddress_;

    //! Access handles of the memory block.
    QSharedPointer<QList<QSharedPointer<AccessHandle> > > accessHandles_ =
        QSharedPointer<QList<QSharedPointer<AccessHandle> > >(new QList<QSharedPointer<AccessHandle> >());

    //! Memory block data:

    //! Specifies the type of usage.
    General::Usage usage_;

    //! Indicates the volatility of the memory block.
    BooleanValue volatile_;
    
    //! Specifies the accessibility of the memory block data (std14).
    AccessTypes::Access access_;

    //! The access poclicies of the memory block (std22).
    QSharedPointer<QList<QSharedPointer<AccessPolicy> > > accessPolicies_ =
        QSharedPointer<QList<QSharedPointer<AccessPolicy> > >(new QList<QSharedPointer<AccessPolicy> >());

    //! Contains the parameters of the block.
    QSharedPointer<QList<QSharedPointer<Parameter> > > parameters_ =
        QSharedPointer<QList<QSharedPointer<Parameter> > >(new QList<QSharedPointer<Parameter> >());
};

Q_DECLARE_METATYPE(QSharedPointer<MemoryBlockBase>);

#endif // MEMORYBLOCKBASE_H
