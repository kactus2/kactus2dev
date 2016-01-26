//-----------------------------------------------------------------------------
// File: Array.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 03.08.2015
//
// Description:
// Implementation of ipxact:array element.
//-----------------------------------------------------------------------------

#ifndef ARRAY_H
#define ARRAY_H

#include "Range.h"

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QString>
#include <QXmlStreamWriter>

//-----------------------------------------------------------------------------
// Kactus2 vendor extension for arrays.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT Array : public Range
{
public:

    /*!
     *  The constructor.
     *
     *      @param [in] left    The left side of the array.
     *      @param [in] right   The right side of the array.
     */
    Array(QString left, QString right);

	/*!
	 *  The destructor.
	*/
    virtual ~Array();

private:
    //! Disable copying.
    Array(Array const& other);

	// Disable assignment.
    Array& operator=(Array const& rhs);

};
#endif // ARRAY_H
