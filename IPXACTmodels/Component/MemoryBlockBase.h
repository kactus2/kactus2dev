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

#include <QMap>
#include <QString>
#include <QSharedPointer>
#include <QList>

class Parameter;

//-----------------------------------------------------------------------------
//! Describes the base class for ipxact:addressBlock, ipxact:bank and ipxact:subspaceMap elements.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT MemoryBlockBase : public NameGroup, public Extendable
{

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] name            Name of the memory block.
	 *      @param [in] baseAddress     Base address of the memory block.
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
	 *      @return Pointer to the cloned memory map item.
	 */
	virtual QSharedPointer<MemoryBlockBase> clone() const = 0;

    /*!
     *  Get the presence of the memory block.
     *
     *      @return The isPresent value.
     */
    QString getIsPresent() const;

    /*!
     *  Set the presence of the memory block.
     *
     *      @param [in] newIsPresent    The new isPresent value.
     */
    void setIsPresent(QString const& newIsPresent);

	/*!
     *  Get the base address of the block.
	 *
	 *      @return The base address
	 */
    QString getBaseAddress() const;

	/*!
     *  Set base address for the block.
	 *
	 *      @param [in} baseAddress     The new base address
	 */
	virtual void setBaseAddress(QString const& baseAddress);

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
     *  Copy the parameters.
     *
     *      @param [in] other   The memory block being copied.
     */
    void copyParameters(const MemoryBlockBase& other);

    //! Contains the presence of the block.
    QString isPresent_;

	//! Contains the base address of the block.
	QString baseAddress_;

    //! Contains the parameters of the block.
    QSharedPointer<QList<QSharedPointer<Parameter> > > parameters_;
};

Q_DECLARE_METATYPE(QSharedPointer<MemoryBlockBase>);

#endif // MEMORYBLOCKBASE_H
