//-----------------------------------------------------------------------------
// File: Association.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 10.6.2014
//
// Description:
// Defines an association between to items in a design.
//-----------------------------------------------------------------------------

#ifndef ASSOCIATION_H
#define ASSOCIATION_H

#include <QGraphicsLineItem>

#include "Associable.h"

#include <common/graphicsItems/GraphicsItemTypes.h>

#include <QGraphicsItem>
#include <QRectF>
#include <QSharedPointer>
#include <QPainter>
#include <QStyleOption>
#include <QWidget>

class Kactus2Position;
//-----------------------------------------------------------------------------
// class Association.
//-----------------------------------------------------------------------------
class Association: public QGraphicsLineItem
{
public:

    enum { Type = GFX_TYPE_DIAGRAM_ASSOCIATION };

	/*!
	 *  The constructor.
	 *
	 *      @param [in] startItem   The item at the starting end of the association.
	 *      @param [in] endItem     The item at the end of the association.
	 *      @param [in] parent      The parent item.
	 */
	Association(Associable* startItem, Associable* endItem, QSharedPointer<Kactus2Position> endpointExtension, 
        QGraphicsItem* parent = 0);

	/*!
	 *  The destructor.
	*/
	virtual ~Association();

    /*!
     *  Draws the item.
     *
     *      @param [in] painter     The painter to use.
     *      @param [in] option      Painting style options.
     *      @param [in] widget      The widget to paint on.
     */
    void paint(QPainter* painter,  QStyleOptionGraphicsItem const* option, QWidget* widget);

    //! Identifies the type of the item.
    virtual int type() const { return Type; }

    //! Connects the association to both ends.  
    void connect();

    //! Disconnects the association from both ends.     
    void disconnect();

    /*!
     *  Changes the item at the end of the association.
     *
     *      @param [in] endItem   The item to set as the new end item.
     */
    void setEndItem(Associable* endItem);

    /*!
     *  Gets the vendor extension representing the endpoint position.
     *
     *      @return The endpoint vendor extension.
     */
    QSharedPointer<Kactus2Position> getEndpointExtension() const;

private:
	// Disable copying.
	Association(Association const& rhs);

	// Disable assignment.
	Association& operator=(Association const& rhs);

	//-----------------------------------------------------------------------------
	// Data.
	//-----------------------------------------------------------------------------

    //! The item at the starting point of the association.
    Associable* startItem_;

    //! The item at the ending point of the association.
    Associable* endItem_;
    
    //! Vendor extension for tracking the endpoint position.
    QSharedPointer<Kactus2Position> endpointPosition_;
};
#endif // ASSOCIATION_H

