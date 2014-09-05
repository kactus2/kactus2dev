//-----------------------------------------------------------------------------
// File: Enumeration.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 04.09.2014
//
// Description:
// Enumeration matches the spirit:enumeration element in IP-XACT.
//-----------------------------------------------------------------------------

#ifndef ENUMERATION_H
#define ENUMERATION_H

#include "ipxactmodels_global.h"

#include <QDomNode>
#include <QString>
#include <QXmlStreamWriter>

//-----------------------------------------------------------------------------
//! Enumeration matches the spirit:enumeration element in IP-XACT.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT Enumeration 
{
public:
    
    //! The constructor.
    Enumeration(QDomNode const& enumerationNode);

	//! The destructor.
	~Enumeration();

    /*!
     *  Writes the element to XML.
     *
     *      @param [in] writer   The writer to use for writing.
     */
    void write(QXmlStreamWriter& writer) const;

private:

	// Disable copying.
	Enumeration(Enumeration const& rhs);
	Enumeration& operator=(Enumeration const& rhs);

    /*!
	 * MANDATORY spirit:enumeration.
	 * Value of the enumeration.
	 */
	QString value_;

	/*!
	 * OPTIONAL spirit:text.
	 * If specified, text is displayed in place of the value. 
	 */
	QString text_;

	/*!
	 * OPTIONAL spirit:help.
	 * If specified, help is used to clarify the meaning of the enumeration.
	 */
	QString help_;
};

#endif // ENUMERATION_H
