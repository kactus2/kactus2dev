//-----------------------------------------------------------------------------
// File: HWComponentItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: 
// Date:
//
// Description:
// HWComponentItem represents graphically an IP-XACT component instance.
//-----------------------------------------------------------------------------


#ifndef HWCOMPONENTITEM_H
#define HWCOMPONENTITEM_H

#include "AdHocEnabled.h"

#include <common/graphicsItems/ComponentItem.h>
#include <common/graphicsItems/GraphicsItemTypes.h>
#include <common/layouts/IVGraphicsLayout.h>

#include <QSharedPointer>

class AdHocVisibilityEditor;
class ActivePortItem;
class ActiveBusInterfaceItem;
class ComponentInstance;
class HWConnectionEndpoint;
class HWColumn;
class LibraryInterface;
class VendorExtension;
class AdHocItem;

//-----------------------------------------------------------------------------
//! HWComponentItem represents graphically an IP-XACT component instance
//-----------------------------------------------------------------------------
class HWComponentItem : public ComponentItem, public AdHocEnabled
{
    Q_OBJECT

public:
    enum { Type = GFX_TYPE_DIAGRAM_COMPONENT };

    /*!
     *  The constructor.
     *
     *    @param [in] libInterface    The library in use.
     *    @param [in] instance        The component instance represented by the item.
     *    @param [in] component       The component represented by the instance.
     *    @param [in] parent          The parent object.
     */
    HWComponentItem(LibraryInterface* libInterface, QSharedPointer<ComponentInstance> instance,
        QSharedPointer<Component> component, QGraphicsItem* parent = nullptr);

	//! The destructor
	~HWComponentItem() final;

    int type() const { return Type; }

    /*!
     *  Updates the diagram component to reflect the current state of the component model.
     */
    virtual void updateComponent();

    /*!
     *  Called when a port's ad-hoc visibility has been changed.
     *
     *    @param [in] portName  The name of the port.
     *    @param [in] visible   The new ad-hoc visibility.
     */
    virtual void onAdHocVisibilityChanged(QString const& portName, bool visible);

    /*!
     *  Attaches the data source to an ad-hoc editor.
     */
    virtual void attach(AdHocVisibilityEditor* editor);

    /*!
     *  Detaches the data source from the ad-hoc editor.
     */
    virtual void detach(AdHocVisibilityEditor* editor);

    virtual QString adHocIdentifier() const;

    /*!
     *  Returns the ad-hoc port with the given name or null if not found.
     */
    virtual HWConnectionEndpoint* getDiagramAdHocPort(QString const& portName);

    /*!
     *  Adds a new, empty bus interface to the component. This function creates automatically an empty
     *  bus interface to the component.
     *
     *    @param [in] pos The position hint for the port.
     *
     *    @return The newly created port.
     */
    ActiveBusInterfaceItem* addPort(QPointF const& pos);

    /*!
     *  Adds an already created bus interface to the component.
     *
     *    @param [in] port The interface to add. Must not be used in any other component.
     */
    void addPort(HWConnectionEndpoint* port);

    /*!
     *  Removes the given port from the component.
     *
     *    @param [in] port The port to remove.
     */
    void removePort(HWConnectionEndpoint* port);

    /*! Get the BusPortItem that corresponds to the given bus interface name
     *
     */
    ActiveBusInterfaceItem* getBusPort(QString const& name) const;

    /*
     *  Returns the ad-hoc port with the given name, or null if not found.
     */
    ActivePortItem* getAdHocPort(QString const& portName) const;


    /*! Called when a port is being moved.
     *
     *    @param [in] port The port that is being moved.
     */
    virtual void onMovePort(ConnectionEndpoint* port);

    /*!
     *  Returns the bus interface positions.
     */
    QMap<QString, QPointF> getBusInterfacePositions() const;

    /*!
     *  Returns the ad-hoc port positions.
     */
    QMap<QString, QPointF> getAdHocPortPositions() const;

    /*!
     *  Marks the component as a packaged component.
     */
    virtual void setPackaged();

    /*!
     *  Marks the component as a draft component.
     */
    virtual void setDraft();

    /*!
     *  Checks if the item is a draft component.
     *
     *    @return True, if the component is a draft component, otherwise false.
     */
    bool isDraft() const;

    /*!
     *  Create an ad hoc port item with the given name.
     *
     *    @param [in] portName    The name of the ad hoc port item.
     *
     *    @return The created ad hoc port item.
     */
    virtual AdHocItem* createAdhocItem(QString const& portName);

signals:
    //! Emitted when the ad-hoc visibilities have been changed.
    void adHocVisibilitiesChanged();

    //! Emitted right before this diagram component is destroyed.
	void destroyed(HWComponentItem* diaComp);

protected:
    // Called when the user presses the mouse button.
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

    //! Called when the user moves the column with the mouse.
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);

    //! Called when the user release the mouse.
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

private:
	
	//! No copying
	HWComponentItem(const HWComponentItem& other);    

    //! No assignment
	HWComponentItem& operator=(const HWComponentItem& other);

   
    //! Positions the bus interface items for the component.        
    void positionBusInterfaceTerminals();

    //! Positions the adhoc port items for the component.
    void positionAdHocPortTerminals();

    /*!
     *  Show the selected ad hoc port item.
     *
     *    @param [in] portItem    The selected port item.
     */
    virtual void showAdhocPort(AdHocItem* portItem);

    /*!
     *  Hide the selected ad hoc port item.
     *
     *    @param [in] portItem    The selected port item.
     */
    virtual void hideAdhocPort(AdHocItem* portItem);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! Hierarchy icon for the component.
    QGraphicsPixmapItem* hierIcon_ =
        new QGraphicsPixmapItem(QPixmap(QStringLiteral(":icons/common/graphics/hierarchy.png")), this);

    //! The old column from where the mouse drag event began.
    HWColumn* oldColumn_ = nullptr;

    //! The old position.
    QPointF oldPos_;
};

#endif // HWCOMPONENTITEM_H
