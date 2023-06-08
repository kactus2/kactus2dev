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

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QList>
#include <QSharedPointer>

class VendorExtension;

//-----------------------------------------------------------------------------
//! Base class for all IP-XACT implementations with vendor extensions.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT Extendable 
{
public:

	//! The destructor.
	virtual ~Extendable() {};
            
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

    /*!
     *  Get a list of grouped extensions.
     *
     *      @param [in] groupName       Identifier of the extension group.
     *      @param [in] extensionType   Identifier of the extension item.
     *
     *      @return A list of extensions belonging to a group.
     */
    QList<QSharedPointer<VendorExtension> > getGroupedExtensionsByType(QString const& groupName,
        QString const& extensionType) const;

    /*!
     * Creates a Kactus2 vendor extension with given type, if it does not exist already.
     * If value is false, the extension is removed.
     *
     *     @param [in] value            The value to set.
     *     @param [in] extensionType    The type of the vendor extension.
     */
    void setPlaceholderExtension(bool value, QString const& extensionType);

    /*!
     *  Find a vendor extension by given type.
     *
     *      @param [in] type        The type of extension to find.
     *
     *      @return The first found extension with the given type.
     */
    QSharedPointer<VendorExtension> findVendorExtension(QString const& type) const;

private:

    //! Any vendor extensions for the element.
    QSharedPointer<QList<QSharedPointer<VendorExtension> > > vendorExtensions_;
};

#endif // EXTENDABLE_H
