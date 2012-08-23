//-----------------------------------------------------------------------------
// File: SystemComponentItem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 24.11.2011
//
// Description:
// Base class for all components visualized in a system design.
//-----------------------------------------------------------------------------

#ifndef SYSTEMCOMPONENTITEM_H
#define SYSTEMCOMPONENTITEM_H

#include "SWPortItem.h"

#include <common/graphicsItems/ComponentItem.h>

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
     *      @param [in] size                      The initial rectangle size.
     *      @param [in] libInterface              The library interface.
     *      @param [in] component                 The component model.
     *      @param [in] instanceName              The name of the component instance.
     *      @param [in] displayName               The component instance's display name.
     *      @param [in] description               The component instance's description.
     *      @param [in] configurableElementValue  The component instance's configurable element values.
     *      @param [in] parent                    The parent graphics item.
     */
    SystemComponentItem(QRectF const& size,
                    LibraryInterface* libInterface,
                    QSharedPointer<Component> component,
                    QString const& instanceName = QString("instance"),
                    QString const& displayName = QString(),
                    QString const& description = QString(),
                    QMap<QString, QString> const& configurableElementValues = QMap<QString, QString>(),
                    QGraphicsItem *parent = 0);

    /*!
     *  Destructor.
     */
    virtual ~SystemComponentItem();

    /*!
     *  Sets the property values.
     *
     *      @param [in] values The property values.
     */
    void setPropertyValues(QMap<QString, QString> const& values);

    /*!
     *  Returns the property values.
     */
    QMap<QString, QString> const& getPropertyValues() const;

    /*!
     *  Adds a new, empty port to the component.
     *
     *      @param [in] pos The position hint for the port.
     *
     *      @return The newly created port.
     */
    SWPortItem* addPort(QPointF const& pos);

    /*!
     *  Adds an already created port to the component.
     *
     *      @param [in] port The port to add. Must not be used in any other component.
     */
    void addPort(SWPortItem* port);

    /*!
     *  Removes the given port from the component.
     *
     *      @param [in] port The port to remove.
     */
    void removePort(SWPortItem* port);

    /*!
     *  Updates the diagram component to reflect the current state of the component model.
     */
    virtual void updateComponent();

    /*!
     *  Sets the flag whether the component has been imported or not.
     *
     *      @param [in] imported If true, the component is marked as imported.
     */
    void setImported(bool imported);

    /*!
     *  Sets the name of the import source instance.
     *
     *      @param [in] nameRef The name of the import source instance.
     */
    void setImportRef(QString const& nameRef);

    /*!
     *  Returns true if the component has been marked as imported.
     */
    bool isImported() const;

    /*!
     *  Returns the name of the import source instance.
     */
    QString const& getImportRef() const;

    /*!
     *  Called when a port is being moved.
     *
     *      @param [in] port The port that is being moved.
     */
    void onMovePort(SWPortItem* port);

    /*!
     *  Updates the size of the component box.
     */
    void updateSize();

    /*!
     *  Returns true if the connections should not be updated automatically in
     *  the port's itemChange() function. Otherwise false.
     */
    bool isConnectionUpdateDisabled() const;

    /*!
     *  Sets the connection update disabled/enabled.
     *
     *      @param [in] disabled If true, the connection update is set disabled, otherwise it is enabled.
     */
    void setConnectionUpdateDisabled(bool disabled);

    /*!
     *  Re-validates existing connections.
     */
    void revalidateConnections();

    /*!
     *  Retrieves the port with the given name and type (API/COM).
     *
     *      @param [in] name  The name of the port.
     *      @param [in] type  The endpoint type.
     *
     *      @return The corresponding port item, or null if no match was found.
     */
    SWPortItem* getSWPort(QString const& name, SWConnectionEndpoint::EndpointType type) const;

    /*!
     *  Returns the underlying HW linked with this component.
     */
    virtual HWMappingItem const* getLinkedHW() const = 0;

signals:
    //! Occurs when the property values have been changed.
    void propertyValuesChanged(QMap<QString, QString> const& propertyValues);

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    /*!
     *  Returns the height for the component box.
     */
    virtual qreal getHeight() const;

    /*!
     *  Updates the port positions so that they have at minimum the given Y coordinate value.
     */
    void offsetPortPositions(qreal minY);

private:
    // Disable copying.
    SystemComponentItem(SystemComponentItem const& rhs);
    SystemComponentItem& operator=(SystemComponentItem const& rhs);

    /*!
     *  Adds the given port to the component.
     *
     *      @param [in] port  The port to add.
     *      @param [in] dir   The port direction which determines to which side to port will be placed.
     */
    void onAddPort(SWPortItem* port, PortDirection dir);
    
    enum
    {
        SPACING = 8,
        MIN_Y_PLACEMENT = 3 * GridSize,
        BOTTOM_MARGIN = 3 * GridSize
    };

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The boolean flag for imported property.
    bool imported_;

    //! The name of the import source instance, if this component is an imported one.
    QString importRef_;

    //! The left, right and bottom port stacks.
    QList<SWPortItem*> leftPorts_;
    QList<SWPortItem*> rightPorts_;
    QList<SWPortItem*> bottomPorts_;

    //! If true, connection updates coming from ports are disabled.
    bool connUpdateDisabled_;

    //! The set property values.
    QMap<QString, QString> propertyValues_;
};

//-----------------------------------------------------------------------------

#endif // SYSTEMCOMPONENTITEM_H
