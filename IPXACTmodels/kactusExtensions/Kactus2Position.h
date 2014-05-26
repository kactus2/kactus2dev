//-----------------------------------------------------------------------------
// File: Kactus2Position.h
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

#include <IPXACTmodels/VendorExtension.h>

#include <QDomNode>

//-----------------------------------------------------------------------------
// class Kactus2Position.
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
     *  Constructor.
     *
     *      @param [in] node A DomNode where the initial position is parsed from.
     */
    Kactus2Position(QDomNode const& node);

	/*!
	 *  The destructor.
	*/
    virtual ~Kactus2Position();

    /*!
     *  Clones the vendor extension.
     *
     *      @return The clone copy of the vendor extension.
     */
    virtual Kactus2Position* clone() const;

    /*!
     *  Returns a type identifier for the vendor extension.
     *
     *      @return A type identifier of the vendor extension.
     */
    virtual QString type() const;

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
