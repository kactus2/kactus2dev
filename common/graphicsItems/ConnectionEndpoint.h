//-----------------------------------------------------------------------------
// File: ConnectionEndpoint.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 4.6.2012
//
// Description:
// Base class for connection endpoints (both SW and HW).
//-----------------------------------------------------------------------------

#ifndef CONNECTIONENDPOINT_H
#define CONNECTIONENDPOINT_H

#include <models/generaldeclarations.h>
#include <models/component.h>

#include <QList>
#include <QPointF>
#include <QVector2D>
#include <QSharedPointer>
#include <QGraphicsPolygonItem>

class ComponentItem;
class ApiInterface;
class ComInterface;
class BusInterface;

//-----------------------------------------------------------------------------
//! Base class for connection endpoints (both SW and HW).
//-----------------------------------------------------------------------------
class ConnectionEndpoint : public QObject, public QGraphicsPolygonItem
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
     */
    ConnectionEndpoint(QGraphicsItem* parent = 0);

    /*!
     *  Destructor.
     */
    virtual ~ConnectionEndpoint();

    /*!
     *  Sets the highlighting mode.
     *
     *      @param [in] mode The highlight mode.
     */
    void setHighlight(HighlightMode mode);

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
     *  Returns true if the draw direction is fixed and thus, cannot be changed.
     */
    virtual bool isDirectionFixed() const;

    /*! 
     *  Returns the name of the endpoint.
     */
    virtual QString name() const = 0;

	/*! \brief Set the name of the endpoint.
	 *
	 * \param name The name to set for the endpoint.
	 *
	*/
	virtual void setName(const QString& name) = 0;

	/*! \brief Get the description of the endpoint.
	 *
	 *
	 * \return QString contains the description.
	*/
	virtual QString description() const = 0;

	/*! \brief Set the description for the endpoint.
	 *
	 * \param description Contains the description to set.
	 *
	*/
	virtual void setDescription(const QString& description) = 0;

    /*!
     *  Updates the endpoint interface.
     */
    virtual void updateInterface() = 0;

    /*! 
     *  Returns the encompassing component, if the endpoint represents a COM/API interface on a component.
     */
    virtual ComponentItem* encompassingComp() const = 0;

	/*!
     *  Returns pointer to the top component that owns this interface.
	 */
	virtual QSharedPointer<Component> getOwnerComponent() const = 0;

    /*! 
     *  Returns the IP-XACT bus interface model of the endpoint.
     *
     *      @remarks The function returns a null pointer if the endpoint is not a bus interface.
     *               Use isBus() function to check for bus interface support.
     */
    virtual QSharedPointer<BusInterface> getBusInterface() const;

    /*! 
     *  Returns the COM interface model of the endpoint.
     *
     *      @remarks The function returns a null pointer if the endpoint is not a COM interface.
     *               Use isCom() function to check for COM interface support.
     */
    virtual QSharedPointer<ComInterface> getComInterface() const;

    /*! 
     *  Returns the API interface model of the endpoint.
     *
     *      @remarks The function returns a null pointer if the endpoint is not a API interface.
     *               Use isApi() function to check for API interface support.
     */
    virtual QSharedPointer<ApiInterface> getApiInterface() const;

    /*!
     *  Returns true if the endpoint represents a hierarchical connection.
     */
    virtual bool isHierarchical() const = 0;

    /*!
     *  Returns true if the endpoint is a bus interface endpoint.
     */
    virtual bool isBus() const;

    /*!
     *  Returns true if the endpoint is an ad-hoc port.
     */
    virtual bool isAdHoc() const;

    /*!
     *  Returns true if the endpoint is a COM interface endpoint.
     */
    virtual bool isCom() const;

    /*!
     *  Returns true if the endpoint is an API interface endpoint.
     */
    virtual bool isApi() const;

signals:
    //! Signals that the contents of the interface have been changed.
    void contentChanged();

    //! \brief Emitted when the endpoint is destroyed.
    void destroyed(ConnectionEndpoint* endpoint);

private:
    // Disable copying.
    ConnectionEndpoint(ConnectionEndpoint const& rhs);
    ConnectionEndpoint& operator=(ConnectionEndpoint const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The endpoint's direction.
    QVector2D dir_;
};

//-----------------------------------------------------------------------------

#endif // CONNECTIONENDPOINT_H
