//-----------------------------------------------------------------------------
// File: Kactus2Placeholder.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 19.5.2014
//
// Description:
// Kactus2 vendor extension for single values.
//-----------------------------------------------------------------------------

#ifndef KACTUS2PLACEHOLDER_H
#define KACTUS2PLACEHOLDER_H

#include "VendorExtension.h"

//-----------------------------------------------------------------------------
// class Kactus2Placeholder.
//-----------------------------------------------------------------------------
class Kactus2Placeholder : public VendorExtension
{
public:
	/*!
	 *  The constructor.
	 *
	 *      @param [in] name   The name of the placeholder.
	 */
    Kactus2Placeholder(QString name);

	/*!
	 *  The destructor.
	*/
    virtual ~Kactus2Placeholder();

    /*!
     *  Writes the vendor extension to XML.
     *
     *      @param [in] writer   The writer used for writing the XML.
     */
    virtual void write(QXmlStreamWriter& writer) const;

protected:
	

private:
	// Disable copying.
	Kactus2Placeholder(Kactus2Placeholder const& rhs);

	// Disable assignment.
	Kactus2Placeholder& operator=(Kactus2Placeholder const& rhs);

	//-----------------------------------------------------------------------------
	// Data.
	//-----------------------------------------------------------------------------

    //! Name of the placeholder extension.
    QString name_;

};
#endif // KACTUS2PLACEHOLDER_H
