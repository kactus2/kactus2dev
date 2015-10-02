//-----------------------------------------------------------------------------
// File: AddressSpaceReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 30.09.2015
//
// Description:
// Reader class for IP-XACT AddressSpace element.
//-----------------------------------------------------------------------------

#ifndef AddressSpaceReader_H
#define AddressSpaceReader_H

#include "AddressSpace.h"

#include <QObject>
#include <QSharedPointer>
#include <QDomNode>
#include <IPXACTmodels/common/CommonItemsReader.h>

//-----------------------------------------------------------------------------
//! Reader class for IP-XACT AddressSpace element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT AddressSpaceReader : public CommonItemsReader
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent  The owner of this reader.
     */
    AddressSpaceReader(QObject* parent = 0);

    /*!
     *  The destructor.
     */
    ~AddressSpaceReader();

    /*!
     *  Creates a new AddressSpace from a given AddressSpace node.
     *
     *      @param [in] AddressSpaceNode    XML description of the address space.
	 *		@return		The created address space.
     */
    QSharedPointer<AddressSpace> createAddressSpaceFrom(QDomNode const& addressSpaceNode) const;

private:

    //! No copying allowed.
    AddressSpaceReader(AddressSpaceReader const& rhs);
	AddressSpaceReader& operator=(AddressSpaceReader const& rhs);

	/*!
	 *  Read parameters of the item.
	 *
	 *      @param [in] tempNode			XML description of the parameters.
	 *      @param [out] parameters			The extracted parameters.
	 */
	void readParameters(QDomNode &tempNode, QList<QSharedPointer<Parameter> > &parameters) const;

	/*!
	 *  Read the block size of the item.
	 *
	 *      @param [in] tempNode			XML description of the block size.
	 *      @param [in] newAddressSpace		The created address space.
	 */
	void readBlockSize(QDomNode &tempNode, QSharedPointer<AddressSpace> newAddressSpace) const;

	/*!
	 *  Read segments of the item.
	 *
	 *      @param [in] tempNode			XML description of the segments.
	 *      @param [out] segments			The extracted segments.
	 */
	void readSegments(QDomNode &tempNode, QList<QSharedPointer<Segment> > &segments) const;
};

#endif // AddressSpaceReader_H