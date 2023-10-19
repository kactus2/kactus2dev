//-----------------------------------------------------------------------------
// File: SWConnectionEndpoint.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti Maatta
// Date: 2.5.2012
//
// Description:
// Common interface for SW connection endpoints.
//-----------------------------------------------------------------------------

#ifndef SWCONNECTIONENDPOINT_H
#define SWCONNECTIONENDPOINT_H

#include <common/graphicsItems/ConnectionEndpoint.h>

#include <editors/common/GraphicsItemLabel.h>

#include <QVector2D>
#include <QSharedPointer>

class VLNV;
class OffPageConnectorItem;
class Component;

//-----------------------------------------------------------------------------
//! Common interface for SW connection endpoints.
//-----------------------------------------------------------------------------
class SWConnectionEndpoint : public ConnectionEndpoint
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] component   Component containing the end point.
     *      @param [in] name        Name of the item.
     *      @param [in] parent      The parent graphics item.
     *      @param [in] dir         The initial direction for the endpoint.
     */
    SWConnectionEndpoint(QSharedPointer<Component> component, QString const& name = QString(),
        QGraphicsItem* parent = 0, QVector2D const& dir = QVector2D(0.0f, -1.0f));

    /*!
     *  Destructor.
     */
    ~SWConnectionEndpoint() override = default;

    // Disable copying.
    SWConnectionEndpoint(SWConnectionEndpoint const& rhs) = delete;
    SWConnectionEndpoint& operator=(SWConnectionEndpoint const& rhs) = delete;

    /*!
     *  Sets the COM/API type. The type of the VLNV determines the type of the endpoint.
     *
     *      @param [in] type The VLNV of the COM/API definition.
     */
    virtual void setTypeDefinition(VLNV const& type) = 0;

    /*!
     *  Returns the currently set COM/API definition.
     */
    virtual VLNV getTypeDefinition() const = 0;

    /*!
     *  Called when creating of a connection for this port has begun.
     */
    virtual void onBeginConnect() = 0;

    /*!
     *  Called when creating of a connection has ended.
     */
    virtual void onEndConnect() = 0;

    /*!
     *  Returns true if a connection is valid between the two endpoints.
     *
     *      @param [in] other The other endpoint.
     *
     *      @remarks Does not take existing connections into account but simply
     *               validates whether a connection between the endpoints would be valid
     *               in a general case.
     */
    virtual bool isConnectionValid(ConnectionEndpoint const* other) const;

    /*!
     *  Updates the endpoint interface.
     */
    virtual void updateInterface();

    /*!
     *  Returns true if the endpoint is a COM interface endpoint.
     */
    bool isCom() const noexcept override;

    /*!
     *  Returns true if the endpoint is an API interface endpoint.
     */
    bool isApi() const noexcept override;

    ConnectionEndpoint::EndpointType getType() const noexcept override;

    virtual ConnectionEndpoint* getOffPageConnector();
    
    /*! 
     *  Returns the encompassing component.
     */
    virtual ComponentItem* encompassingComp() const;

	/*!
     *  Returns a pointer to the top component that owns this interface.
	 */
	virtual QSharedPointer<Component> getOwnerComponent() const;

protected:

    /*!
     *  Set the type for the end point.
     *
     *      @param [in] newType     The new end point type.
     */
    void setType(ConnectionEndpoint::EndpointType newType);

    /*!
     *  Get the name label.
     *
     *      @return The name label.
     */
    GraphicsItemLabel* getNameLabel() const;

    /*!
     *  Makes basic initializations common to all constructors.
     */
    virtual void initialize();

private:

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The type of the end point.
    ConnectionEndpoint::EndpointType type_ = ConnectionEndpoint::ENDPOINT_TYPE_UNDEFINED;

    //! The name label.
    GraphicsItemLabel* nameLabel_ = new GraphicsItemLabel(QString(), this);

    //! The off page connector item.
    OffPageConnectorItem* offPageConnector_ = nullptr;

    //! Component item containing this end point item.
    ComponentItem* parentItem_= nullptr;

    //! Component containing this item.
    QSharedPointer<Component> containingComponent_= nullptr;
};

//-----------------------------------------------------------------------------

#endif // SWCONNECTIONENDPOINT_H
