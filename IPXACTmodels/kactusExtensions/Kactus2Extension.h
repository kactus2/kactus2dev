//-----------------------------------------------------------------------------
// File: Kactus2Extension.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 20.5.2014
//
// Description:
// Kactus2 vendor extension for name-value pairs.
//-----------------------------------------------------------------------------

#ifndef KACTUS2EXTENSION_H
#define KACTUS2EXTENSION_H

#include <IPXACTmodels/VendorExtension.h>

//-----------------------------------------------------------------------------
// class Kactus2Extension.
//-----------------------------------------------------------------------------
class Kactus2Extension : public VendorExtension
{
public:
	    //! The destructor.
    virtual ~Kactus2Extension() {}

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
#endif // KACTUS2EXTENSION_H
