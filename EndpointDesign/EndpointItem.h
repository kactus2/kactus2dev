//-----------------------------------------------------------------------------
// File: EndpointItem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 26.2.2011
//
// Description:
// Endpoint UI item.
//-----------------------------------------------------------------------------

#ifndef ENDPOINTITEM_H
#define ENDPOINTITEM_H

#include <QObject>
#include <QAbstractGraphicsShapeItem>
#include <QVector2D>

#include "../MCAPI/MCAPIDesignerTypes.h"

class ProgramEntityItem;
class EndpointConnection;
class MappingComponentItem;

//-----------------------------------------------------------------------------
//! EndpointItem class.
//-----------------------------------------------------------------------------
class EndpointItem : public QObject, public QAbstractGraphicsShapeItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:
    enum { Type = UserType + 12 };

    // Public constants.
    enum
    {
        WIDTH = 240,
        HEIGHT = 40
    };

    //-----------------------------------------------------------------------------
    //! HighlightMode enumeration.
    //-----------------------------------------------------------------------------
    enum HighlightMode
    {
        HIGHLIGHT_OFF = 0,
        HIGHLIGHT_ALLOWED,
        HIGHLIGHT_HOVER,
    };

    //-----------------------------------------------------------------------------
    //! Direction enumeration.
    //-----------------------------------------------------------------------------
    enum DrawDirection
    {
        DIR_LEFT = 0,
        DIR_RIGHT
    };

    /*!
     *  Constructor.
     *
     *      @param [in] parentNode  The parent node. Must be valid.
     *      @param [in] name        The endpoint's name.
     *      @param [in] type        The endpoint type.
     *      @param [in] connType    The connection type.
     *      @param [in] portID      The endpoint's port ID.
     *      @param [in] parent      The graphics item parent. Can be null.
     */
    EndpointItem(ProgramEntityItem* parentNode, QString const& name, MCAPIEndpointDirection type,
                 MCAPIDataType connType, unsigned int portID, QGraphicsItem* parent = 0);

    /*!
     *  Destructor.
     */
    ~EndpointItem();

    /*!
     *  Sets the endpoint's name.
     *
     *      @param [in] name The new name for the endpoint.
     */
    void setName(QString const& name);

    /*!
     *  Sets the endpoint's type.
     *
     *      @param [in] type The endpoint's type to set.
     */
    void setType(MCAPIEndpointDirection type);

    /*!
     *  Sets the endpoint's connection type.
     *
     *      @param [in] connType The connection type.
     */
    void setConnectionType(MCAPIDataType connType);

    /*!
     *  Sets the endpoint's port id.
     *
     *      @param [in] portID The endpoint's port id.
     */
    void setPortID(unsigned int portID);

    /*!
     *  Sets the endpoint visualization as a horizontally mirrored/non-mirrored one.
     *
     *      @param [in] mirrored True for mirrored, false for non-mirrored.
     */
    void setMirrored(bool mirrored);

    /*!
     *  Sets the highlighting mode.
     *
     *      @param [in] mode The highlight mode.
     */
    void setHighlight(HighlightMode mode);

    /*!
     *  Attaches the endpoint to a connection.
     *
     *      @param [in] connection The connection.
     */
    void addConnection(EndpointConnection* connection);

    /*!
     *  Unattaches the endpoint from a connection.
     *
     *      @param [in] connection The connection.
     */
    void removeConnection(EndpointConnection* connection);

    /*!
     *  Creates a corresponding bus interfaces to the mapping component.
     *
     *      @param [in] mappingComp The mapping component to which to create the bus interface.
     */
    void createBusInterface(MappingComponentItem* mappingComp);

    /*!
     *  Removes the corresponding bus interface from the mapping component.
     */
    void removeBusInterface(MappingComponentItem* mappingComp);

    /*!
     *  Returns the list of connections that are connected to this endpoint.
     */
    QList<EndpointConnection*> const& getConnections() const;

    /*!
     *  Returns true if the endpoint is connected.
     */
    bool isConnected() const;

    /*!
     *  Called when a connection between this and another endpoint is done.
     *
     *      @param [in] other The other endpoint of the connection.
     *
     *      @return False if there was an error in the connection. Otherwise true.
     */
    bool onConnect(EndpointItem const* other);

    /*!
     *  Called when a connection has been removed from between this and another endpoint.
     *
     *      @param [in] other The other endpoint of the connection.
     */
    void onDisconnect(EndpointItem const* other);

    /*! 
     *  Returns true if this endpoint can be connected to the given endpoint.
     *
     *      @param [in] other The endpoint to which to connect.
     */
    bool canConnect(EndpointItem const* other) const;

    /*!
     *  Sets the draw direction of the endpoint.
     *
     *      @param [in] dir The draw direction to set.
     */
    void setDirection(DrawDirection dir);

    /*!
     *  Returns the draw direction as an enumeration.
     */
    DrawDirection getDirection() const;

    /*!
	 *	Returns the draw direction of the endpoint.
	 */
	QVector2D getDirectionVector() const;

    /*!
     *  Returns the endpoint's name.
     */
    QString const& getName() const;

    /*!
     *  Returns the endpoint's full (unique) name.
     */
    QString getFullName() const;

    /*!
     *  Returns the endpoint's type.
     */
    MCAPIEndpointDirection getMCAPIDirection() const;

    /*!
     *  Returns the connection type.
     */
    MCAPIDataType getConnectionType() const;

    /*!
     *  Returns the node id.
     */
    unsigned int getPortID() const;

    /*!
     *  Returns the parent program entity (endpoints component).
     */
    ProgramEntityItem* getParentProgramEntity() const;

    /*!
     *  Returns the parent mapping component.
     */
    MappingComponentItem* getParentMappingComp() const;

    //-----------------------------------------------------------------------------
    // QGraphicsItem implementation.
    //-----------------------------------------------------------------------------

    /*!
     *  Returns the bounding rect of the endpoint's visualization.
     */
    QRectF boundingRect() const;

    /*!
     *  Returns the endpoint visualization shape.
     */
    QPainterPath shape() const;

    /*!
     *  Draws the endpoint.
     */
    void paint(QPainter* painter, QStyleOptionGraphicsItem const* option, QWidget* widget);

    /*!
     *  Returns the graphics item type.
     */
    int type() const { return Type; }

public slots:
    //! Invokes the edit endpoint dialog.
    void editEndpoint();

signals:
    //! Signaled when the endpoint item has changed.
    void contentChanged();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

private:
    // Disable copying.
    EndpointItem(EndpointItem const& rhs);
    EndpointItem& operator=(EndpointItem const& rhs);

    /*!
     *  Updates the endpoint's text label.
     */
    void updateText();

    /*!
     *  Updates the visuals of the endpoint.
     */
    void updateVisuals();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    enum
    {
        THIN_LINE = 1,
        ARROW_WIDTH = 20
    };

    //! Parent program entity (endpoints component).
    ProgramEntityItem* parentProgEntity_;

    //! The endpoint's name.
    QString name_;

    //! The endpoint's type.
    MCAPIEndpointDirection type_;
    
    //! The connection type.
    MCAPIDataType connType_;

    //! The endpoint's ID.
    unsigned int portID_;

    //! The endpoint's UI text label.
    QGraphicsTextItem* textLabel_;

    //! The connections to this endpoint.
    QList<EndpointConnection*> connections_;

    //! The endpoint's draw direction.
    DrawDirection dir_;

    //! The endpoint's position before mouse move.
    QPointF oldPos_;
};

//-----------------------------------------------------------------------------

#endif // ENDPOINTITEM_H
