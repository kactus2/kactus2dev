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

    //! Disable copying.
    Array(Array const& other) = delete;

	// Disable assignment.
    Array& operator=(Array const& rhs) = delete;

    /*!
     *	Set the array id.
     *  
     *      @param [in] id     The new array id.
     */
    void setId(QString const& id);

    /*!
     *	Get the array id.
     *  
     * 	    @return The array id.
     */
    QString getId() const;

private:

    //! Name for referencing the array.
    QString arrayId_;
};
#endif // ARRAY_H
