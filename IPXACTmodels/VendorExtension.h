//-----------------------------------------------------------------------------
// File: VendorExtension.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 8.5.2014
//
// Description:
// Common interface for IP-XACT vendor extensions.
//-----------------------------------------------------------------------------

#ifndef VENDOREXTENSION_H
#define VENDOREXTENSION_H

#include <common/Global.h>

#include <QXmlStreamWriter>

//-----------------------------------------------------------------------------
// class VendorExtension.
//-----------------------------------------------------------------------------
class KACTUS2_API VendorExtension
{

public:

    //! The destructor.
    virtual ~VendorExtension() {}

    /*!
     *  Clones the vendor extension.
     *
     *      @return The clone copy of the vendor extension.
     */
    virtual VendorExtension* clone() const = 0;

    /*!
     *  Returns a type identifier for the vendor extension.
     *
     *      @return A type identifier of the vendor extension.
     */
    virtual QString type() const = 0;

    /*!
     *  Writes the vendor extension to XML.
     *
     *      @param [in] writer   The writer used for writing the XML.
     */
    virtual void write(QXmlStreamWriter& writer) const = 0;

};

#endif // VENDOREXTENSION_H
