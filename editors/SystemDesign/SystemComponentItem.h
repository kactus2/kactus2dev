//-----------------------------------------------------------------------------
// File: SystemComponentItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti Maatta
// Date: 24.11.2011
//
// Description:
// Base class for all components visualized in a system design.
//-----------------------------------------------------------------------------

#ifndef SYSTEMCOMPONENTITEM_H
#define SYSTEMCOMPONENTITEM_H

#include "SWPortItem.h"

#include <common/graphicsItems/ComponentItem.h>
#include <common/layouts/IVGraphicsLayout.h>

enum PortDirection
{
    PORT_LEFT = 0,
    PORT_RIGHT,
    PORT_BOTTOM
};

class HWMappingItem;

//-----------------------------------------------------------------------------
//! SystemComponentItem class.
//-----------------------------------------------------------------------------
class SystemComponentItem : public ComponentItem
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *    @param [in] size            The initial rectangle size.
     *    @param [in] libInterface    The library interface.
     *    @param [in] instance        The component instance.
     *    @param [in] component       The component referenced by the component instance.
     *    @param [in] parent          The parent graphics item.
     */
	SystemComponentItem(QRectF const& size, LibraryInterface* libInterface,
		QSharedPointer<ComponentInstance> instance,
		QSharedPointer<Component> component,
		QGraphicsItem *parent);

    /*!
     *  Destructor.
     */
    ~SystemComponentItem() override = default;

    // Disable copying.
    SystemComponentItem(SystemComponentItem const& rhs) = delete;
    SystemComponentItem& operator=(SystemComponentItem const& rhs) = delete;

    /*!
     *  Sets the property values.
     *
     *    @param [in] values The property values.
     */ 
    void setPropertyValues(QMap<QString, QString> const& values);

    /*!
     *  Returns the property values.
     */
    QMap<QString, QString> getPropertyValues() const;

    /*!
     *  Adds a new, empty port to the component.
     *
     *    @param [in] pos The position hint for the port.
     *
     *    @return The newly created port.
     */
    SWPortItem* addPort(QPointF const& pos);

    /*!
     *  Adds an already created port to the component.
     *
     *    @param [in] port The port to add. Must not be used in any other component.
     */
    void addPort(SWPortItem* port);

    /*!
     *  Removes the given port from the component.
     *
     *    @param [in] port The port to remove.
     */
    void removePort(SWPortItem* port);

    /*!
     *  Sets the flag whether the component has been imported or not.
     *
     *    @param [in] imported If true, the component is marked as imported.
     */
    void setImported(bool imported);

    /*!
     *  Sets the name of the import source instance.
     *
     *    @param [in] nameRef The name of the import source instance.
     */
    void setImportRef(QString const& nameRef);

    /*!
     *  Returns true if the component has been marked as imported.
     */
    bool isImported() const;

    /*!
     *  Returns the name of the import source instance.
     */
    QString getImportRef() const;

    /*!
     *  Called when a port is being moved.
     *
     *    @param [in] port    The port that is being moved.
     */
    virtual void onMovePort(ConnectionEndpoint* port);

    /*!
     *  Returns true if the connections should not be updated automatically in the port's itemChange() function.
     *  Otherwise false.
     */
    bool isConnectionUpdateDisabled() const;

    /*!
     *  Sets the connection update disabled/enabled.
     *
     *    @param [in] disabled If true, the connection update is set disabled, otherwise it is enabled.
     */
    void setConnectionUpdateDisabled(bool disabled);

    /*!
     *  Re-validates existing connections.
     */
    void revalidateConnections();

    /*!
     *  Retrieves the port with the given name and type (API/COM).
     *
     *    @param [in] name  The name of the port.
     *    @param [in] type  The endpoint type.
     *
     *    @return The corresponding port item, or null if no match was found.
     */
    SWPortItem* getSWPort(QString const& name, SWConnectionEndpoint::EndpointType type) const;

    /*!
     *  Retrieves the port with properties similar to the selected end point.
     *
     *    @param [in] otherEndPoint   The selected end point.
     *
     *    @return An SW port with similar properties to the selected port.
     */
    SWPortItem* getSWPortMatchingOtherEndPoint(ConnectionEndpoint* otherEndPoint) const;

    /*!
     *  Returns the underlying HW linked with this component.
     */
    virtual HWMappingItem const* getLinkedHW() const = 0;

    /*!
     *  Sets the API interface positions.
     *
     *    @param [in] positions      The positions to set.
     *    @param [in] createMissing  If true, the missing bus interfaces are created.
     */
    void setApiInterfacePositions(QMap<QString, QPointF> const& positions, bool createMissing = false);

    /*!
     *  Sets the COM interface positions.
     *
     *    @param [in] positions      The positions to set.
     *    @param [in] createMissing  If true, the missing bus interfaces are created.
     */
    void setComInterfacePositions(QMap<QString, QPointF> const& positions, bool createMissing = false);

    /*!
     *  Returns the API interface positions.
     */
    QMap<QString, QPointF> getApiInterfacePositions() const;

    /*!
     *  Returns the COM interface positions.
     */
    QMap<QString, QPointF> getComInterfacePositions() const;

    /*!
     *  Marks the component as a packetized component.
     */
    virtual void setPackaged();

    /*!
     *  Marks the component as a draft component.
     */
    virtual void setDraft();

signals:
    //! Occurs when the property values have been changed.
    void propertyValuesChanged(QMap<QString, QString> const& propertyValues);

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);


    /*!
     *  Updates the port positions so that they have at minimum the given Y coordinate value.
     */
    void offsetPortPositions(qreal minY);

private:

    /*!
     *  Position the API interface items for the component item.
     */
    void positionAPIInterfaceTerminals();

    /*!
     *  Position the COM interface items for the component item.
     */
    void positionCOMInterfaceTerminals();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! If true, connection updates coming from ports are disabled.
    bool connUpdateDisabled_ = false;

    //! The set property values.
    QMap<QString, QString> propertyValues_;
};

//-----------------------------------------------------------------------------

#endif // SYSTEMCOMPONENTITEM_H
