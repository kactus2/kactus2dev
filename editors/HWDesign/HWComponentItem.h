/* 
 *
 * 		filename: HWComponentItem.h
 */

#ifndef HWCOMPONENTITEM_H
#define HWCOMPONENTITEM_H

#include "AdHocEnabled.h"
#include <common/graphicsItems/ComponentItem.h>
#include <common/graphicsItems/GraphicsItemTypes.h>
#include <common/layouts/IVGraphicsLayout.h>

#include <QSharedPointer>

class BusPortItem;
class AdHocPortItem;
class HWConnectionEndpoint;
class HWColumn;
class LibraryInterface;

/*! \brief HWComponentItem represents graphically an IP-XACT component instance
 *
 */
class HWComponentItem : public ComponentItem, public AdHocEnabled
{
    Q_OBJECT 

public:
    enum { Type = GFX_TYPE_DIAGRAM_COMPONENT };

    HWComponentItem(LibraryInterface* lh,
                     QSharedPointer<Component> component,
                     const QString &instanceName = QString("instance"),
                     const QString &displayName = QString(),
                     const QString &description = QString(),
							const QString& uuid = QString(),
                     const QMap<QString, QString> &configurableElementValues = QMap<QString, QString>(),
                     QMap<QString, bool> const& portAdHocVisibilities = QMap<QString, bool>(),
                     QGraphicsItem *parent = 0);

	//! \brief The destructor
	virtual ~HWComponentItem();

    /*!
     *  Adds a new, empty port to the component. This function creates automatically an empty
     *  bus interface to the component model.
     *
     *      @param [in] pos The position hint for the port.
     *
     *      @return The newly created port.
     */
    BusPortItem* addPort(QPointF const& pos);

    /*!
     *  Adds an already created port to the component.
     *
     *      @param [in] port The port to add. Must not be used in any other component.
     */
    void addPort(HWConnectionEndpoint* port);

    /*!
     *  Removes the given port from the component.
     *
     *      @param [in] port The port to remove.
     */
    void removePort(HWConnectionEndpoint* port);

    /*! \brief Get the BusPortItem that corresponds to the given bus interface name
     *
     */
    BusPortItem* getBusPort(const QString &name);

    /*! \brief Get the BusPortItem that corresponds to the given bus interface name
     *
     */
    BusPortItem const* getBusPort(const QString &name) const;

    /*
     *  Returns the ad-hoc port with the given name, or null if not found.
     */
    AdHocPortItem* getAdHocPort(QString const& portName);

    /*
     *  Returns the ad-hoc port with the given name, or null if not found.
     */
    AdHocPortItem const* getAdHocPort(QString const& portName) const;

    int type() const { return Type; }

    /*! Called when a port is being moved.
     *
     *      @param [in] port The port that is being moved.
     */
    void onMovePort(HWConnectionEndpoint* port);

    /*!
     *  Updates the diagram component to reflect the current state of the component model.
     */
    virtual void updateComponent();

    /*!
     *  Returns true if the connections should not be updated automatically in
     *  the diagram port's itemChange() function. Otherwise false.
     */
    bool isConnectionUpdateDisabled() const;

    /*!
     *  Called when a port's ad-hoc visibility has been changed.
     *
     *      @param [in] portName  The name of the port.
     *      @param [in] visible   The new ad-hoc visibility.
     */
    virtual void onAdHocVisibilityChanged(QString const& portName, bool visible);

    /*!
     *  Attaches the data source to an ad-hoc editor.
     */
    virtual void attach(AdHocEditor* editor);

    /*!
     *  Detaches the data source from the ad-hoc editor.
     */
    virtual void detach(AdHocEditor* editor);

    /*!
     *  Returns true if the ad-hoc visibilities are protected.
     */
    virtual bool isProtected() const;

    /*!
     *  Returns the edit provider for the data.
     */
    virtual GenericEditProvider& getEditProvider();

    /*!
     *  Returns the ad-hoc port with the given name or null if not found.
     */
    virtual HWConnectionEndpoint* getDiagramAdHocPort(QString const& portName);

    /*!
     *  Sets the bus interface positions.
     *
     *      @param [in] positions      The positions to set.
     *      @param [in] createMissing  If true, the missing bus interfaces are created.
     */
    void setBusInterfacePositions(QMap<QString, QPointF> const& positions, bool createMissing = false);

    /*!
     *  Sets the ad-hoc port positions.
     *
     *      @param [in] positions      The positions to set.
     */
    void setAdHocPortPositions(QMap<QString, QPointF> const& positions);

    /*!
     *  Returns the bus interface positions.
     */
    QMap<QString, QPointF> getBusInterfacePositions() const;

    /*!
     *  Returns the ad-hoc port positions.
     */
    QMap<QString, QPointF> getAdHocPortPositions() const;

signals:
    //! Emitted when the ad-hoc visibilities have been changed.
    void adHocVisibilitiesChanged();

    //! \brief Emitted right before this diagram component is destroyed.
	void destroyed(HWComponentItem* diaComp);

protected:
    // Called when the user presses the mouse button.
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

    //! Called when the user moves the column with the mouse.
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);

    //! Called when the user release the mouse.
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

private:
	
	//! \brief No copying
	HWComponentItem(const HWComponentItem& other);

	//! No assignment
	HWComponentItem& operator=(const HWComponentItem& other);

    /*!
     *  Adds a port to the correct port stack.
     *
     *      @param [in] port   The port to add.
     *      @param [in] right  If true, the port is added to the right port stack. If false, it is
     *                         added to the left port stack.
     */
    void onAddPort(HWConnectionEndpoint* port, bool right);

    /*!
     *  Updates the size of the component based on the port positions.
     */
    void updateSize();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    enum
    {
        SPACING = 8,
        MIN_Y_PLACEMENT = 3 * GridSize
    };

    QGraphicsPixmapItem* hierIcon_;

    //! The old column from where the mouse drag event began.
    HWColumn* oldColumn_;

    //! The layout for ports.
    QSharedPointer< IVGraphicsLayout<HWConnectionEndpoint> > portLayout_;

    //! The left and right port stacks.
    QList<HWConnectionEndpoint*> leftPorts_;
    QList<HWConnectionEndpoint*> rightPorts_;
    bool connUpdateDisabled_;
    QPointF oldPos_;
};

#endif // HWCOMPONENTITEM_H
