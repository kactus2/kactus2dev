//-----------------------------------------------------------------------------
// File: HWConnectionEndpoint.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 18.3.2011
//
// Description:
// Interface class for the diagram connection end points.
//-----------------------------------------------------------------------------

#ifndef HWCONNECTIONENDPOINT_H
#define HWCONNECTIONENDPOINT_H

#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/component.h>

#include <common/graphicsItems/ConnectionEndpoint.h>

#include <QList>
#include <QPointF>
#include <QVector2D>
#include <QSharedPointer>
#include <QGraphicsPolygonItem>

class HWConnection;
class ComponentItem;
class BusInterface;
class VLNV;

//-----------------------------------------------------------------------------
//! HWConnectionEndpoint interface.
//-----------------------------------------------------------------------------
class HWConnectionEndpoint : public ConnectionEndpoint
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] parent     The parent graphics item.
     *      @param [in] temporary  If true, the endpoint is set as temporary.
     *      @param [in] dir        The initial direction for the endpoint.
     */
    HWConnectionEndpoint(QGraphicsItem* parent = 0, bool temporary = false,
                         QVector2D const& dir = QVector2D(0.0f, -1.0f));

    /*!
     *  Destructor.
     */
    virtual ~HWConnectionEndpoint();

    /*!
     *  Sets the bus and abstraction types and the interface mode for the endpoint.
     *
     *      @param [in] busType  The bus type (bus definition VLNV).
     *      @param [in] absType  The abstraction type (abstraction definition VLNV).
     *      @param [in] mode     The interface mode.
     */
    virtual void setTypes(VLNV const& busType, VLNV const& absType,
                          General::InterfaceMode mode) = 0;

    /*! \brief Set the interface mode for the endpoint.
	 *
	 * \param mode The interface mode to set.
	 *
	*/
	virtual void setInterfaceMode(General::InterfaceMode mode) = 0;

	/*!
	 *  Gives the length of the text in the name label.
	 */
	virtual qreal getNameLength();

	/*!
	 *  Shortens the name label to better fit in the component.
	 *
	 *      @param [in] width   The width of the shortened name.
	 */
	virtual void shortenNameLabel(qreal width);

private:
    // Disable copying.
    HWConnectionEndpoint(HWConnectionEndpoint const& rhs);
    HWConnectionEndpoint& operator=(HWConnectionEndpoint const& rhs);
};

//-----------------------------------------------------------------------------

#endif // HWCONNECTIONENDPOINT_H
