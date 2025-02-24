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

#include <IPXACTmodels/common/VendorExtension.h>
#include <IPXACTmodels/ipxactmodels_global.h>

#include <QDomNode>
#include <QPointF>

//-----------------------------------------------------------------------------
// class Kactus2Position.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT Kactus2Position : public VendorExtension
{
public:
	/*!
	 *  The constructor.
	 *
	 *    @param [in] position   The initial position of represented by the extension.
	 */
    Kactus2Position(QPointF position);

	/*!
	 *  The destructor.
	*/
    virtual ~Kactus2Position();

    // Disable copying.
    Kactus2Position(Kactus2Position const& rhs) = delete;

    // Disable assignment.
    Kactus2Position& operator=(Kactus2Position const& rhs) = delete;

    /*!
     *  Clones the vendor extension.
     *
     *    @return The clone copy of the vendor extension.
     */
    virtual Kactus2Position* clone() const;

    /*!
     *  Returns a type identifier for the vendor extension.
     *
     *    @return A type identifier of the vendor extension.
     */
    virtual QString type() const;

    /*!
     *  Writes the vendor extension to XML.
     *
     *    @param [in] writer   The writer used for writing the XML.
     */
    virtual void write(QXmlStreamWriter& writer) const;

    void parsePosition(QDomNode const& positionNode);

    /*!
     *  Gets the position.
     *
     *    @return The position represented by the extension.
     */
    QPointF position() const;

    /*!
     *  Sets the position.
     *
     *    @param [in] pos   The position to set.
     */
    void setPosition(QPointF const& pos);

private:
	//-----------------------------------------------------------------------------
	// Data.
	//-----------------------------------------------------------------------------

    //! The position of represented by the extension.
    QPointF position_;

};
#endif // KACTUS2POSITION_H
