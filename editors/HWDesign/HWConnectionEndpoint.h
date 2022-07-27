//-----------------------------------------------------------------------------
// File: HWConnectionEndpoint.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti Maatta
// Date: 18.3.2011
//
// Description:
// Interface class for the diagram connection end points.
//-----------------------------------------------------------------------------

#ifndef HWCONNECTIONENDPOINT_H
#define HWCONNECTIONENDPOINT_H

#include <IPXACTmodels/common/DirectionTypes.h>
#include <IPXACTmodels/common/TransactionalTypes.h>
#include <IPXACTmodels/Component/Component.h>

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
class OffPageConnectorItem;
class HWComponentItem;
class GraphicsItemLabel;

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
     *      @param [in] name                    Name of the end point.
     *      @param [in] containingComponent     Component containing the end point item.
     *      @param [in] parent                  The parent graphics item.
     *      @param [in] dir                     The initial direction for the endpoint.
     */
    HWConnectionEndpoint(QString const& name, QSharedPointer<Component> containingComponent,
        QGraphicsItem* parent = 0, QVector2D const& dir = QVector2D(0.0f, -1.0f));

    /*!
     *  Destructor.
     */
    virtual ~HWConnectionEndpoint();

    virtual void updateInterface();

	/*!
	 *  Gives the length of the text in the name label.
	 */
	virtual qreal getNameLength();

    /*!
     *  Get the component containing this end point item.
     *
     *      @return The component containing this end point item.
     */
    virtual QSharedPointer<Component> getOwnerComponent() const;

    /*!
     *  Set the position for the bus interface name label.
     */
    virtual void setLabelPosition() = 0;

    /*!
     *  Set a new name for the connection end point.
     *
     *      @param [in] name    The selected name.
     */
    virtual void setName(QString const& name);

    /*!
     *  Get the off page connector item.
     *
     *      @return the off page connector item.
     */
    virtual ConnectionEndpoint* getOffPageConnector();

    /*!
     *  Check if the scene is locked.
     *
     *      @return True, if the scene is locked, false otherwise.
     */
    bool sceneIsLocked() const;

    /*!
     *  Get the component item containing this end point item.
     *
     *      @return The component item containing this end point item.
     */
    virtual ComponentItem* encompassingComp() const;

protected:

    /*!
     *  Get the label containing the name of this end point.
     *
     *      @return The name label.
     */
    GraphicsItemLabel* getNameLabel() const;

    /*!
     *  Handles the mouse move events.
     *
     *      @param [in] event   The mouse move event.
     */
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);


private:
    // Disable copying.
    HWConnectionEndpoint(HWConnectionEndpoint const& rhs);
    HWConnectionEndpoint& operator=(HWConnectionEndpoint const& rhs);

    /*!
     *  Update the graphics of the end point item.
     */
    virtual void updateEndPointGraphics() = 0;

    /*!
     *  Move the item by dragging with the mouse.
     */
    virtual void moveItemByMouse() = 0;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Component containing this end point.
    QSharedPointer<Component> containingComponent_;

    //! Component item containing this end point item.
    HWComponentItem* parentComponentItem_;

    //! The name label.
    GraphicsItemLabel* nameLabel_;

    //! The off page connector item.
    OffPageConnectorItem* offPageConnector_;
};

//-----------------------------------------------------------------------------

#endif // HWCONNECTIONENDPOINT_H
