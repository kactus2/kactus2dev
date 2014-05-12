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
     *  Writes the vendor extension to XML.
     *
     *      @param [in] writer   The writer used for writing the XML.
     */
    virtual void write(QXmlStreamWriter& writer) const = 0;

};

#endif // VENDOREXTENSION_H
