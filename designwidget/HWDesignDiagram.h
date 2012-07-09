/* 
 *
 * 		filename: HWDesignDiagram.h
 */

#ifndef HWDESIGNDIAGRAM_H
#define HWDESIGNDIAGRAM_H

#include "AdHocEnabled.h"

#include <common/DesignDiagram.h>
#include <models/businterface.h>
#include <models/ColumnDesc.h>

#include <QMap>
#include <QVector>
#include <QSharedPointer>
#include <QUndoCommand>

class LibraryInterface;
class Component;
class Design;
class DesignConfiguration;
class HWComponentItem;
class ComponentItem;
class BusPortItem;
class BusInterfaceItem;
class AdHocPortItem;
class AdHocInterfaceItem;
class HWConnection;
class HWConnectionEndpoint;
class GraphicsColumn;
class GraphicsColumnLayout;
class AbstractionDefinition;
class VLNV;
class GenericEditProvider;
class HWDesignWidget;
class ConnectionEndpoint;
class GraphicsConnection;

/*! \brief HWDesignDiagram is a graphical view to a design
 *
 */
class HWDesignDiagram : public DesignDiagram
{
    Q_OBJECT

public:
    /*! \brief The constructor
     *
     */
    HWDesignDiagram(LibraryInterface *lh, GenericEditProvider& editProvider, HWDesignWidget *parent = 0);

	//! \brief The destructor
	virtual ~HWDesignDiagram();

    /*!
     *  Clears the scene.
     */
    virtual void clearScene();

    /*! 
     *  Creates a design based on the contents in the diagram.
     *
     *      @param [in] vlnv The vlnv for the design.
     *
     *      @return The created design.
     */
    virtual QSharedPointer<Design> createDesign(VLNV const& vlnv) const;

    /*!
     *  Sets the draw mode.
     */
    virtual void setMode(DrawMode mode);


    /*! \brief Set the IP-XACT document that is viewed in HWDesignDiagram
     *
     */
    void loadDesign(QSharedPointer<Design> design);

    /*! \brief Get HWComponentItem that has the given instance name
     *
     */
    HWComponentItem *getComponent(const QString &instanceName);

	/*! \brief Get the component instances contained in this scene.
	 *
	 * \return QList containing pointers to the component instances.
	*/
	QList<ComponentItem*> getInstances() const;

    /*!
     *  Changes the state of a visibility control.
     *
     *      @param [in] name   The name of the visibility control.
     *      @param [in] state  The new state for the visibility control.
     */
    virtual void setVisibilityControlState(QString const& name, bool state);

	/*!
     *  Reflects the changes in the design to the top-level component.
     */
    virtual void updateHierComponent();

    /*!
     *  Adds a column to the diagram's layout.
     *
     *      @param [in] desc The column description.
     */
    void addColumn(ColumnDesc const& desc);

    /*!
     *  Removes a column from the diagram's layout.
     *
     *      @param [in] column The column to remove.
     */
    void removeColumn(GraphicsColumn* column);

    /*!
     *  Returns the diagram column layout.
     */
    GraphicsColumnLayout* getColumnLayout();

    /*! \brief Get pointer to the parent of this scene.
	 *
	 * \return Pointer to the design widget that owns this scene.
	*/
	virtual HWDesignWidget* parent() const;

    /*!
     *  Called when a port's ad-hoc visibility has been changed.
     *
     *      @param [in] portName  The name of the port.
     *      @param [in] visible   The new ad-hoc visibility.
     */
    virtual void onAdHocVisibilityChanged(QString const& portName, bool visible);

    /*!
     *  Returns the ad-hoc port with the given name or null if not found.
     */
    virtual HWConnectionEndpoint* getDiagramAdHocPort(QString const& portName);

public slots:
    /*! \brief Bring the selected item to front
     *
     */
    void selectionToFront();

    /*!
     *  Called when the view has been scrolled vertically.
     */
    virtual void onVerticalScroll(qreal y);

    /*!
     *  Called when the selection changes in the diagram.
     */
    void onSelectionChanged();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);

    void endConnect();

    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void wheelEvent(QGraphicsSceneWheelEvent *event);

    void keyReleaseEvent(QKeyEvent *event);
    
    void dropEvent(QGraphicsSceneDragDropEvent *event);
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
    void dragMoveEvent(QGraphicsSceneDragDropEvent *event);
    void dragLeaveEvent(QGraphicsSceneDragDropEvent * event);

    void disableHighlight();

private:
    // Disable copying.
    HWDesignDiagram(HWDesignDiagram const& rhs);
    HWDesignDiagram& operator=(HWDesignDiagram const& rhs);

    /*!
     *  Called when an item has been selected in the diagram.
     *
     *      @param [in] newSelection The selected item.
     */
    virtual void onSelected(QGraphicsItem* newSelection);

    /*!
     *  Adds a new interface to the given diagram column.
     *
     *      @param [in] column The column where to add the interface.
     *      @param [in] pos    The interface position.
     */

    void addInterface(GraphicsColumn* column, QPointF const& pos);

    /*!
     *  Toggles the connection style of the given connection between normal and off-page style.
     *
     *      @param [in] conn      The connection.
     *      @param [in] parentCmd The parent undo command.
     */
    void toggleConnectionStyle(GraphicsConnection* conn, QUndoCommand* parentCmd);

    /*!
     *  Hides all visible off-page connections.
     */
    void hideOffPageConnections();

    /*!
     *  Destroys all connections in the diagram.
     */
    void destroyConnections();

    /*!
     *  Creates the currently drawn connection.
     *
     *      @param [in] event The ending mouse press event.
     */
    void createConnection(QGraphicsSceneMouseEvent* event);

    /*!
     *  Creates a missing port to the given component item.
     *
     *      @param [in] portName   The name of the port to create.
     *      @param [in] component  The parent component.
     *      @param [in] design     The design containing related information.
     */
    BusPortItem* createMissingPort(QString const& portName, HWComponentItem* component, QSharedPointer<Design> design);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! \brief Pointer to the parent of this scene.
	HWDesignWidget* parent_;

    //! The connection that is being drawn.
    HWConnection *tempConnection_;

    //! The starting end point of a connection that is being drawn.
    ConnectionEndpoint* tempConnEndPoint_;

    //! The potential end points that can be connected to the starting end point.
    QVector<ConnectionEndpoint*> tempPotentialEndingEndPoints_;

    //! The highlighted end point to which the connection could be snapped automatically.
    HWConnectionEndpoint* highlightedEndPoint_;

    //! The column layout.
    QSharedPointer<GraphicsColumnLayout> layout_;

    //! The type of the item being dragged.
    ColumnItemType dragCompType_;
    bool dragBus_;

    //! If true, the off-page connection mode is active.
    bool offPageMode_;

    //! If true, we're in replace component mode.
    bool replaceMode_;

    //! The component that is used to replace another component in replace mode.
    HWComponentItem* sourceComp_;

    QGraphicsItem* oldSelection_;
};

#endif // HWDESIGNDIAGRAM_H
