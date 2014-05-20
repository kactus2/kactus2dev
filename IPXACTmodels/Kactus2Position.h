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
	 *      @param [in] name   The name of the placeholder.
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
     *      @return 
     */
    QPointF position() const;

    /*!
     *  <Description>.
     *
     *      @param [in] pos   <Argument description>.
     *
     *      @return 
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

    //! Name of the placeholder extension.
    QPointF position_;

};
#endif // KACTUS2POSITION_H
