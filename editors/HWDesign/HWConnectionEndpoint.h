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

#include <editors/common/GraphicsItemLabel.h>

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
        QGraphicsItem* parent = nullptr, QVector2D const& dir = QVector2D(0.0f, -1.0f));

    /*!
     *  Destructor.
     */
    ~HWConnectionEndpoint() override = default;

    // Disable copying.
    HWConnectionEndpoint(HWConnectionEndpoint const& rhs) = delete;
    HWConnectionEndpoint& operator=(HWConnectionEndpoint const& rhs) = delete;

    void updateInterface() override;

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
    void setName(QString const& name) override;

    /*!
     *  Get the off page connector item.
     *
     *      @return the off page connector item.
     */
    ConnectionEndpoint* getOffPageConnector() override;

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
    ComponentItem* encompassingComp() const override;

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
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;


private:

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
    QSharedPointer<Component> containingComponent_ = nullptr;

    //! Component item containing this end point item.
    HWComponentItem* parentComponentItem_ = nullptr;

    //! The name label.
    GraphicsItemLabel* nameLabel_ = new GraphicsItemLabel(QString(), this);

    //! The off page connector item.
    OffPageConnectorItem* offPageConnector_ = nullptr;
};

//-----------------------------------------------------------------------------

#endif // HWCONNECTIONENDPOINT_H
