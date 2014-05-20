//-----------------------------------------------------------------------------
// File: Kactus2Placeholder.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 19.5.2014
//
// Description:
// Kactus2 vendor extension for position.
//-----------------------------------------------------------------------------

#ifndef KACTUS2POSITION_H
#define KACTUS2POSITION_H

#include "VendorExtension.h"

//-----------------------------------------------------------------------------
// class Kactus2Placeholder.
//-----------------------------------------------------------------------------
class Kactus2Position : public VendorExtension
{
public:
	/*!
	 *  The constructor.
	 *
	 *      @param [in] position   The initial position of represented by the extension.
	 */
    Kactus2Position(QPointF position);

	/*!
	 *  The destructor.
	*/
    virtual ~Kactus2Position();

    /*!
     *  Writes the vendor extension to XML.
     *
     *      @param [in] writer   The writer used for writing the XML.
     */
    virtual void write(QXmlStreamWriter& writer) const;

    /*!
     *  Gets the position.
     *
     *      @return The position represented by the extension.
     */
    QPointF position() const;

    /*!
     *  Sets the position.
     *
     *      @param [in] pos   The position to set.
     */
    void setPosition(QPointF const& pos);

private:
	// Disable copying.
	Kactus2Position(Kactus2Position const& rhs);

	// Disable assignment.
	Kactus2Position& operator=(Kactus2Position const& rhs);
    
	//-----------------------------------------------------------------------------
	// Data.
	//-----------------------------------------------------------------------------

    //! The position of represented by the extension.
    QPointF position_;

};
#endif // KACTUS2POSITION_H
