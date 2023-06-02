//-----------------------------------------------------------------------------
// File: HWConnection.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 01.06.2023
//
// Description:
// Template for classes inheriting GraphicsConnection.
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Function: ConnectionItem::ConnectionItem()
//-----------------------------------------------------------------------------
template <typename T> ConnectionItem<T>::ConnectionItem(ConnectionEndpoint* endpoint1, ConnectionEndpoint* endpoint2,
    QSharedPointer<T> interconnection,
    QSharedPointer<ConnectionRoute> route, bool autoConnect,
    DesignDiagram* parent) :
    GraphicsConnection(endpoint1, endpoint2, route, autoConnect, parent),
    interconnection_(interconnection)
{

}

//-----------------------------------------------------------------------------
// Function: ConnectionItem::ConnectionItem()
//-----------------------------------------------------------------------------
template <typename T> ConnectionItem<T>::ConnectionItem(QPointF const& p1, QVector2D const& dir1,
    QPointF const& p2, QVector2D const& dir2,
    DesignDiagram* parent):
    GraphicsConnection(p1, dir1, p2, dir2, parent)
{

}

//-----------------------------------------------------------------------------
// Function: ConnectionItem::setName()
//-----------------------------------------------------------------------------
template <typename T> void ConnectionItem<T>::setName(std::string const& name)
{
    GraphicsConnection::setName(name);
    interconnection_->setName(name);

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ConnectionItem::name()
//-----------------------------------------------------------------------------
template <typename T> std::string ConnectionItem<T>::name() const
{
    return interconnection_->nameStd();
}

//-----------------------------------------------------------------------------
// Function: ConnectionItem::setDescription()
//-----------------------------------------------------------------------------
template <typename T> void ConnectionItem<T>::setDescription(std::string const& description)
{
    interconnection_->setDescription(description);

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ConnectionItem::description()
//-----------------------------------------------------------------------------
template <typename T> std::string ConnectionItem<T>::description() const
{
    return interconnection_->descriptionStd();
}

//-----------------------------------------------------------------------------
// Function: ConnectionItem::getInterconnection()
//-----------------------------------------------------------------------------
template <typename T> QSharedPointer<T> ConnectionItem<T>::getInterconnection()
{
    return interconnection_;
}

//-----------------------------------------------------------------------------
// Function: ConnectionItem::changeConnectionComponentReference()
//-----------------------------------------------------------------------------
template <typename T> void ConnectionItem<T>::changeConnectionComponentReference(std::string const& oldName, 
    std::string const& newName)
{
    interconnection_->changeInterfaceComponentReferences(oldName, newName);
}
