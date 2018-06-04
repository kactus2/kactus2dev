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
    Array(QString const& left, QString const& right);

    //! Disable copying.
    Array(Array const& other) = delete;

	/*!
	 *  The destructor.
	*/
    virtual ~Array() = default;

    // Disable assignment.
    Array& operator=(Array const& rhs) = delete;

};
#endif // ARRAY_H
