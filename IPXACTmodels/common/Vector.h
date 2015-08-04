//-----------------------------------------------------------------------------
// File: Vector.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 31.07.2015
//
// Description:
// Implementation of ipxact:vector element.
//-----------------------------------------------------------------------------

#ifndef VECTOR_H
#define VECTOR_H

#include "Range.h"

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QString>

//-----------------------------------------------------------------------------
//! Implementation of ipxact:vector element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT Vector : public Range
{
public:

    /*!
     *  The constructor.
     *
     *      @param [in] left    The left side of the vector.
     *      @param [in] right   The right side of the vector.
     */
    Vector(QString left, QString right);

    //! Copy constructor.
    Vector(Vector const& other);

	/*!
	 *  The destructor.
	*/
    virtual ~Vector();

private:

	// Disable assignment.
    Vector& operator=(Vector const& rhs);

};
#endif // VECTOR_H
