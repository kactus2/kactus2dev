//-----------------------------------------------------------------------------
// File: DiagramConnectionEndPoint.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 18.3.2011
//
// Description:
// Interface class for the diagram connection end points.
//-----------------------------------------------------------------------------

#ifndef DIAGRAMCONNECTIONENDPOINT_H
#define DIAGRAMCONNECTIONENDPOINT_H

#include <models/generaldeclarations.h>

#include <QList>
#include <QPointF>
#include <QVector2D>
#include <QGraphicsPolygonItem>

class DiagramInterconnection;
class DiagramComponent;
class BusInterface;
class VLNV;

//-----------------------------------------------------------------------------
//! DiagramConnectionEndPoint interface.
//-----------------------------------------------------------------------------
class DiagramConnectionEndPoint : public QObject, public QGraphicsPolygonItem
{
    Q_OBJECT

public:
    //-----------------------------------------------------------------------------
    //! HighlightMode enumeration.
    //-----------------------------------------------------------------------------
    enum HighlightMode
    {
        HIGHLIGHT_OFF = 0,
        HIGHLIGHT_ALLOWED,
        HIGHLIGHT_HOVER,
    };

    /*!
     *  Constructor.
     *
     *      @param [in] parent The parent graphics item.
     *      @param [in] dir    The initial direction for the end point.
     */
    DiagramConnectionEndPoint(QGraphicsItem* parent = 0,
                              QVector2D const& rhs = QVector2D(0.0f, -1.0f));

    /*!
     *  Destructor.
     */
    virtual ~DiagramConnectionEndPoint();

    /*!
     *  Sets the highlighting mode.
     *
     *      @param [in] mode The highlight mode.
     */
    void setHighlight(HighlightMode mode);

    /*!
     *  Sets the bus and abstraction types and the interface mode for the end point.
     *
     *      @param [in] busType  The bus type (bus definition VLNV).
     *      @param [in] absType  The abstraction type (abstraction definition VLNV).
     *      @param [in] mode     The interface mode.
     */
    virtual void setTypes(VLNV const& busType, VLNV const& absType,
                          General::InterfaceMode mode) = 0;

    /*!
     *  Attaches the end point to a connection.
     *
     *      @param [in] connection The connection.
     */
    void addInterconnection(DiagramInterconnection* connection);

    /*!
     *  Unattaches the end point from a connection.
     *
     *      @param [in] connection The connection.
     */
    void removeInterconnection(DiagramInterconnection* connection);

    /*!
     *  Returns the list of connections that are connected to this end point.
     */
    QList<DiagramInterconnection*> const& getInterconnections() const;

    /*!
     *  Returns true if the end point has at least one connection.
     */
    bool isConnected() const;

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
     *  Returns true if the draw direction is fixed and thus, cannot be changed.
     */
    virtual bool isDirectionFixed() const;

    /*! 
     *  Returns the name of the end point.
     */
    virtual QString name() const = 0;

    /*!
     *  Called when a connection between this and another end point is done.
     *
     *      @param [in] other The other end point of the connection.
     *
     *      @return False if there was an error in the connection. Otherwise true.
     */
    virtual bool onConnect(DiagramConnectionEndPoint const* other) = 0;

    /*!
     *  Called when a connection has been removed from between this and another end point.
     *
     *      @param [in] other The other end point of the connection.
     */
    virtual void onDisconnect(DiagramConnectionEndPoint const* other) = 0;

    /*! 
     *  Returns true if this end point can be connected to the given end point.
     *
     *      @param [in] other The end point to which to connect.
     */
    virtual bool canConnect(DiagramConnectionEndPoint const* other) const = 0;

    /*!
     *  Updates the end point interface.
     */
    virtual void updateInterface() = 0;

    /*! 
     *  Returns the encompassing port, if this DiagramComponent represents
     *  a bus interface on a component.
     */
    virtual DiagramComponent *encompassingComp() const = 0;

    /*! 
     *  brief Returns the IP-XACT bus interface model of the end point.
     */
    virtual QSharedPointer<BusInterface> getBusInterface() const = 0;

    /*! \brief Returns true if the port represents a hierarchical connection
     *
     */
    virtual bool isHierarchical() const = 0;

signals:
    //! Signals that the contents of the interface have been changed.
    void contentChanged();

private:
    // Disable copying.
    DiagramConnectionEndPoint(DiagramConnectionEndPoint const& rhs);
    DiagramConnectionEndPoint& operator=(DiagramConnectionEndPoint const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The connections to this end point.
    QList<DiagramInterconnection*> connections_;

    //! The end point's direction.
    QVector2D dir_;
};

//-----------------------------------------------------------------------------

#endif // DIAGRAMCONNECTIONENDPOINT_H
