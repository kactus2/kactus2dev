//-----------------------------------------------------------------------------
// File: ResetType.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 02.01.2019
//
// Description:
// Describes the ipxact:resetType element.
//-----------------------------------------------------------------------------

#ifndef RESETTYPE_H
#define RESETTYPE_H

#include <IPXACTmodels/common/NameGroup.h>
#include <IPXACTmodels/common/Extendable.h>

//-----------------------------------------------------------------------------
//! Describes the ipxact:resetType element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT ResetType: public NameGroup, public Extendable
{

public:

	/*!
	 *  The constructor.
	 */
    ResetType() = default;

    /*!
     *  The destructor.
     */
    virtual ~ResetType() = default;

	/*!
     *  Copy constructor.
     */
    ResetType(const ResetType& other) = default;

	/*!
     *  Assignment operator.
     */
    ResetType& operator=(const ResetType& other) = default;
};

#endif // RESETTYPE_H
