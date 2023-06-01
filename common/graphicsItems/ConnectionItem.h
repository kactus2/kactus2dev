//-----------------------------------------------------------------------------
// File: ConnectionItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 01.06.2023
//
// Description:
// Template for classes inheriting GraphicsConnection.
//-----------------------------------------------------------------------------

#ifndef CONNECTION_ITEM_H
#define CONNECTION_ITEM_H

#include "GraphicsConnection.h"

#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Template for classes inheriting GraphicsConnection.
//-----------------------------------------------------------------------------
template <typename T> 
class ConnectionItem : public GraphicsConnection
{
public:

    /*!
     *  Constructor.
     */
    ConnectionItem(ConnectionEndpoint* endpoint1, ConnectionEndpoint* endpoint2,
        QSharedPointer<T> interconnection,
        QSharedPointer<ConnectionRoute> route, bool autoConnect,
        DesignDiagram* parent);

    /*!
     *  Constructor which creates an open-ended connection.
     */
    ConnectionItem(QPointF const& p1, QVector2D const& dir1,
        QPointF const& p2, QVector2D const& dir2,
        DesignDiagram* parent);

    // Disable copying.
    ConnectionItem(ConnectionItem const& rhs) = delete;
    ConnectionItem& operator=(ConnectionItem const& rhs) = delete;

    /*!
     *  Set the name for the connection item.
     *
     *      @param [in] name    The new connection item name.
     */
    void setName(QString const& name) final;

    /*!
     *  Get the name of the connection item.
     *
     *      @return The name of the connection item.
     */
    QString name() const final;

    /*!
     *  Set the description for the connection item.
     *
     *      @param [in] description     The new description.
     */
    void setDescription(QString const& description) final;

    /*!
     *  Get the description for the connection item.
     *
     *      @return The description of the connection item.
     */
    QString description() const final;

    /*!
     *  Get the contained interconnection.
     *
     *      @return The interconnection contained within this item.
     */
    QSharedPointer<T> getInterconnection();

    /*!
     *  Change the component reference of a contained interface.
     *
     *      @param [in] oldName     The old component reference.
     *      @param [in] newName     The new component reference.
     */
    void changeConnectionComponentReference(std::string const& oldName, std::string const& newName) final;

private:

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The contained interconnection.
    QSharedPointer<T> interconnection_;
};

#include "ConnectionItem.inl"
//-----------------------------------------------------------------------------

#endif // CONNECTION_ITEM_H
