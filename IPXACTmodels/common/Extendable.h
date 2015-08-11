//-----------------------------------------------------------------------------
// File: Extendable.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 11.08.2015
//
// Description:
// Base class for all IP-XACT implementations with vendor extensions.
//-----------------------------------------------------------------------------

#ifndef EXTENDABLE_H
#define EXTENDABLE_H

#include <QList>
#include <QSharedPointer>

class VendorExtension;

//-----------------------------------------------------------------------------
//! Base class for all IP-XACT implementations with vendor extensions.
//-----------------------------------------------------------------------------
class Extendable 
{
public:

	//! The destructor.
	virtual ~Extendable();
            
    /*!
     *  Gets the vendor extensions for the element.
     *
     *      @return The vendor extensions for the element.
     */
    QSharedPointer<QList<QSharedPointer<VendorExtension> > > getVendorExtensions() const;

protected:

    //! The constructor.
    Extendable();

    //! Copy constructor.
    Extendable(Extendable const& rhs);

    //! Assignment operation.
    Extendable& operator=(Extendable const& rhs);
    
    /*!
     *  Copies vendor extensions from another element.
     *
     *      @param [in] other   The element to copy extensions from.
     */
    void copyVendorExtensions(Extendable const& other);

private:

    //! Any vendor extensions for the element.
    QSharedPointer<QList<QSharedPointer<VendorExtension> > > vendorExtensions_;
};

#endif // EXTENDABLE_H
