//-----------------------------------------------------------------------------
// File: DiagramOffPageConnector.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 9.1.2012
//
// Description:
// Off-page connector for the block diagram.
//-----------------------------------------------------------------------------

#ifndef DIAGRAMOFFPAGECONNECTOR_H
#define DIAGRAMOFFPAGECONNECTOR_H

#include "DiagramConnectionEndpoint.h"

#include <common/graphicsItems/GraphicsItemTypes.h>

#include <QSharedPointer>
#include <QVector2D>
#include <QPolygonF>

class BusInterface;
class DiagramComponent;
class DiagramConnectionEndpoint;
class LibraryInterface;

//-----------------------------------------------------------------------------
//! DiagramOffPageConnector class.
//-----------------------------------------------------------------------------
class DiagramOffPageConnector : public DiagramConnectionEndpoint
{
    Q_OBJECT

public:
    enum
    {
        Type = GFX_TYPE_DIAGRAM_OFF_PAGE_CONNECTOR
    };

    /*!
     *  Constructor.
     *
     *      @param [in] parent The parent diagram connection endpoint.
     */
    DiagramOffPageConnector(DiagramConnectionEndpoint* parent);

	/*!
     *  Destructor.
     */
	virtual ~DiagramOffPageConnector();

    /*!
     *  Sets the bus and abstraction types and the interface mode for the endpoint.
     *
     *      @param [in] busType  The bus type (bus definition VLNV).
     *      @param [in] absType  The abstraction type (abstraction definition VLNV).
     *      @param [in] mode     The interface mode.
     */
    void setTypes(VLNV const& busType, VLNV const& absType, General::InterfaceMode mode);

    /*! 
     *  Updates the graphics to match the IP-XACT bus interface.
     */
    void updateInterface();

	int type() const { return Type; }

    //-----------------------------------------------------------------------------
    // DiagramConnectionEndpoint implementation.
    //-----------------------------------------------------------------------------

    /*!
     *  Returns true if the draw direction is fixed and thus, cannot be changed.
     */
    virtual bool isDirectionFixed() const;

    /*! 
     *  Returns the name of the parent bus interface.
     */
    virtual QString name() const;

	/*!
	 *  Set the name of the parent bus interface.
	 *
     *      @param [in] name The name to set for the port.
	 *
	*/
	virtual void setName(const QString& name);

	/*! 
     *  Get the description of the port.
	 *
     *      @return QString containing the description.
	 */
	virtual QString description() const;

	/*!
	 *  Set the description for the port.
     *
	 *      @param [in] description The description to set.
	 */
	virtual void setDescription(const QString& description);

    /*!
     *  Attaches the endpoint to a connection.
     *
     *      @param [in] connection The connection.
     */
    virtual void addConnection(GraphicsConnection* connection);

    /*!
     *  Unattaches the endpoint from a connection.
     *
     *      @param [in] connection The connection.
     */
    virtual void removeConnection(GraphicsConnection* connection);

    /*!
     *  Called when a connection between this and another endpoint is done.
     *
     *      @param [in] other The other endpoint of the connection.
     *
     *      @return False if there was an error in the connection. Otherwise true.
     */
    virtual bool onConnect(ConnectionEndpoint const* other);

    /*!
     *  Called when a connection has been removed from between this and another endpoint.
     *
     *      @param [in] other The other endpoint of the connection.
     */
    virtual void onDisconnect(ConnectionEndpoint const* other);

    /*! 
     *  Returns true if the connector can be connected to the given endpoint.
     *
     *      @param [in] other The endpoint to which to connect.
     */
    virtual bool canConnect(ConnectionEndpoint const* other) const;

    /*!
     *  Sets the draw direction of the endpoint.
     *
     *      @param [in] dir The draw direction to set.
     *
     *      @remarks The direction can be changed only if isDirectionFixed() returns false.
     */
    virtual void setDirection(QVector2D const& dir);

    /*!
	 *	Returns the draw direction of the endpoint.
	 */
	virtual QVector2D const& getDirection() const;

    /*! 
     *  Returns the encompassing component, if it represents a bus interface on a component.
     */
    virtual ComponentItem* encompassingComp() const;

	/*!
	 *  Returns pointer to the top component that owns this interface.
	 *
	 *      @return Pointer to the component to which this interface belongs to.
	*/
	virtual QSharedPointer<Component> getOwnerComponent() const;

    /*! 
     *  Returns the IP-XACT bus interface model of the parent diagram connection endpoint.
     */
    virtual QSharedPointer<BusInterface> getBusInterface() const;

    /*!
     *  Returns the ad-hoc port of the endpoint.
     *
     *      @remarks The function returns a null pointer if the endpoint is a bus interface.
     *               Use isBus() function to check for ad-hoc support (isBus() == false).
     */
    virtual Port* getPort() const;

    /*!
     *  Returns true if the port represents a hierarchical connection.
     */
    virtual bool isHierarchical() const;

    /*!
     *  Returns true if the endpoint is a bus interface endpoint.
     */
    virtual bool isBus() const;

	/*! 
	 *  Sets the interface mode for the endpoint.
	 *
     *      @param [in] The interface mode to set.
	 *
	*/
	virtual void setInterfaceMode(General::InterfaceMode mode);

protected:
    virtual QVariant itemChange(GraphicsItemChange change,
                                const QVariant &value);

    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);

private:
    // Disable copying.
    DiagramOffPageConnector(DiagramOffPageConnector const& rhs);
    DiagramOffPageConnector& operator=(DiagramOffPageConnector const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The parent diagram connection endpoint.
    DiagramConnectionEndpoint* parent_;
};

//-----------------------------------------------------------------------------

#endif // DIAGRAMOFFPAGECONNECTOR_H
