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
class DiagramInterconnection;
class DiagramComponent;
class DiagramConnectionEndPoint;
class LibraryInterface;

//-----------------------------------------------------------------------------
//! DiagramOffPageConnector class.
//-----------------------------------------------------------------------------
class DiagramOffPageConnector : public DiagramConnectionEndPoint
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
     *      @param [in] parent The parent diagram connection end point.
     */
    DiagramOffPageConnector(DiagramConnectionEndPoint* parent);

	/*!
     *  Destructor.
     */
	virtual ~DiagramOffPageConnector();

    /*!
     *  Sets the bus and abstraction types and the interface mode for the end point.
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
    // DiagramConnectionEndPoint implementation.
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
     *  Attaches the end point to a connection.
     *
     *      @param [in] connection The connection.
     */
    virtual void addInterconnection(DiagramInterconnection* connection);

    /*!
     *  Unattaches the end point from a connection.
     *
     *      @param [in] connection The connection.
     */
    virtual void removeInterconnection(DiagramInterconnection* connection);

    /*!
     *  Called when a connection between this and another end point is done.
     *
     *      @param [in] other The other end point of the connection.
     *
     *      @return False if there was an error in the connection. Otherwise true.
     */
    virtual bool onConnect(DiagramConnectionEndPoint const* other);

    /*!
     *  Called when a connection has been removed from between this and another end point.
     *
     *      @param [in] other The other end point of the connection.
     */
    virtual void onDisconnect(DiagramConnectionEndPoint const* other);

    /*! 
     *  Returns true if the connector can be connected to the given end point.
     *
     *      @param [in] other The end point to which to connect.
     */
    virtual bool canConnect(DiagramConnectionEndPoint const* other) const;

    /*!
     *  Sets the draw direction of the end point.
     *
     *      @param [in] dir The draw direction to set.
     *
     *      @remarks The direction can be changed only if isDirectionFixed() returns false.
     */
    virtual void setDirection(QVector2D const& dir);

    /*!
	 *	Returns the draw direction of the end point.
	 */
	virtual QVector2D const& getDirection() const;

    /*! 
     *  Returns the encompassing component, if it represents a bus interface on a component.
     */
    virtual DiagramComponent *encompassingComp() const;

	/*!
	 *  Returns pointer to the top component that owns this interface.
	 *
	 *      @return Pointer to the component to which this interface belongs to.
	*/
	virtual QSharedPointer<Component> ownerComponent() const;

    /*! 
     *  Returns the IP-XACT bus interface model of the parent diagram connection end point.
     */
    virtual QSharedPointer<BusInterface> getBusInterface() const;

    /*!
     *  Returns the ad-hoc port of the end point.
     *
     *      @remarks The function returns a null pointer if the end point is a bus interface.
     *               Use isBus() function to check for ad-hoc support (isBus() == false).
     */
    virtual Port* getPort() const;

    /*!
     *  Returns true if the port represents a hierarchical connection.
     */
    virtual bool isHierarchical() const;

    /*!
     *  Returns true if the end point is a bus interface end point.
     */
    virtual bool isBus() const;

	/*! 
	 *  Sets the interface mode for the end point.
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

    //! The parent diagram connection end point.
    DiagramConnectionEndPoint* parent_;
};

//-----------------------------------------------------------------------------

#endif // DIAGRAMOFFPAGECONNECTOR_H
