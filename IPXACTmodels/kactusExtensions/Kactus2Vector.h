//-----------------------------------------------------------------------------
// File: Kactus2Vector.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 07.04.2015
//
// Description:
// Kactus2 vendor extension for vectors.
//-----------------------------------------------------------------------------

#ifndef KACTUS2VECTOR_H
#define KACTUS2VECTOR_H

#include <IPXACTmodels/kactusExtensions/Kactus2Array.h>

//-----------------------------------------------------------------------------
// Kactus2 vendor extension for vectors.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT Kactus2Vector : public Kactus2Array
{
public:

    /*!
     *  The constructor.
     *
     *      @param [in] left    The left side of the array.
     *      @param [in] right   The right side of the array.
     */
    Kactus2Vector(QString left, QString right);

    /*!
     *  The destructor.
     */
    virtual ~Kactus2Vector();

    /*!
     *  Clones the vendor extension.
     *
     *      @return The clone copy of the vendor extension.
     */
    virtual Kactus2Vector* clone() const;

    /*!
     *  Returns a type identifier for the vendor extension.
     *
     *      @return A type identifier of the vendor extension.
     */
    virtual QString type() const;

private:
    //! Disable copying.
    Kactus2Vector(Kactus2Vector const& other);

	// Disable assignment.
    Kactus2Vector& operator=(Kactus2Vector const& rhs);

};
#endif // KACTUS2VECTOR_H
