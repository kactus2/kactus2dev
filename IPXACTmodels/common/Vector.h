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
     */
    Vector() = default;

    /*!
     *  The constructor.
     *
     *      @param [in] left    The left side of the vector.
     *      @param [in] right   The right side of the vector.
     */
    Vector(QString const& left, QString const& right);
   
    //! The destructor.
    ~Vector() = default;

    /*!
     *  Create a copy of the Vector.
     *
     *      @return A Vector identical to this.
     */
    Vector* clone() const;

    /*!
     *	Sets the vector id.
     *  
     *      @param [in] id	Description
     */
    void setId(QString const& id);

    /*!
     *	Gets the vector id.
     *  
     * 		@return The vector id as string.
     */
    QString getId() const;

private:

    //! Name for referencing the vector.
    QString vectorId_;

};
#endif // VECTOR_H
