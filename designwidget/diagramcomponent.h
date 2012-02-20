/* 
 *
 * 		filename: diagramcomponent.h
 */

#ifndef DIAGRAMCOMPONENT_H
#define DIAGRAMCOMPONENT_H

#include <QSharedPointer>
#include <common/graphicsItems/ComponentItem.h>

class DiagramPort;
class DiagramConnectionEndPoint;
class DiagramColumn;
class LibraryInterface;

/*! \brief DiagramComponent represents graphically an IP-XACT component instance
 *
 */
class DiagramComponent : public ComponentItem
{
    Q_OBJECT 

public:
    enum { Type = UserType + 1 };

    DiagramComponent(LibraryInterface* lh,
                     QSharedPointer<Component> component,
                     const QString &instanceName = QString("unnamed"),
                     const QString &displayName = QString(),
                     const QString &description = QString(),
                     const QMap<QString, QString> &configurableElementValues = QMap<QString, QString>(),
                     QMap<QString, bool> const& portAdHocVisibilities = QMap<QString, bool>(),
                     QGraphicsItem *parent = 0);

	//! \brief The destructor
	virtual ~DiagramComponent();

    /*!
     *  Adds a new, empty port to the component. This function creates automatically an empty
     *  bus interface to the component model.
     *
     *      @param [in] pos The position hint for the port.
     *
     *      @return The newly created port.
     */
    DiagramPort* addPort(QPointF const& pos);

    /*!
     *  Adds an already created port to the component.
     *
     *      @param [in] port The port to add. Must not be used in any other component.
     */
    void addPort(DiagramPort* port);

    /*!
     *  Removes the given port from the component.
     *
     *      @param [in] port The port to remove.
     */
    void removePort(DiagramPort* port);

    /*! \brief Get the DiagramPort that corresponds to the given bus interface name
     *
     */
    DiagramPort *getBusPort(const QString &name);

    int type() const { return Type; }

    /*! Called when a port is being moved.
     *
     *      @param [in] port The port that is being moved.
     */
    void onMovePort(DiagramConnectionEndPoint* port);

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
     *  Sets the ad-hoc visibility of the given port.
     *
     *      @param [in] portName The name of the port.
     *      @param [in] visible  If true, the port is set visible for ad-hoc.
     */
    void setPortAdHocVisible(QString const& portName, bool visible);

    /*!
     *  Returns true if the given port is currently visible for ad-hoc.
     *
     *      @param [in] portName The name of the port.
     */
    bool isPortAdHocVisible(QString const& portName) const;

    /*!
     *  Returns the ad-hoc visibility map.
     */
    QMap<QString, bool> const& getPortAdHocVisibilities() const;

signals:
    //! \brief Emitted right before this diagram component is destroyed.
	void destroyed(DiagramComponent* diaComp);

protected:
    // Called when the user presses the mouse button.
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

    //! Called when the user moves the column with the mouse.
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);

    //! Called when the user release the mouse.
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

private:
	
	//! \brief No copying
	DiagramComponent(const DiagramComponent& other);

	//! No assignment
	DiagramComponent& operator=(const DiagramComponent& other);

    /*!
     *  Adds a port to the correct port stack.
     *
     *      @param [in] port   The port to add.
     *      @param [in] right  If true, the port is added to the right port stack. If false, it is
     *                         added to the left port stack.
     */
    void onAddPort(DiagramConnectionEndPoint* port, bool right);

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

    LibraryInterface* lh_;
    QGraphicsPixmapItem* hierIcon_;

    //! The old column from where the mouse drag event began.
    DiagramColumn* oldColumn_;

    //! The left and right port stacks.
    QList<DiagramConnectionEndPoint*> leftPorts_;
    QList<DiagramConnectionEndPoint*> rightPorts_;
    bool connUpdateDisabled_;
    QPointF oldPos_;

    //! The ad-hoc visibility flags for ports.
    QMap<QString, bool> portAdHocVisibilities_;
};

#endif // DIAGRAMCOMPONENT_H
