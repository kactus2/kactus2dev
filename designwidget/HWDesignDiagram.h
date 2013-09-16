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
    // Column widths.
    enum
    {
        COMPONENT_COLUMN_WIDTH = 259,
        IO_COLUMN_WIDTH = 119
    };
		

    //-----------------------------------------------------------------------------
    //! Clipboard copy data for a single bus interface instance.
    //-----------------------------------------------------------------------------
    struct BusInterfaceCopyData
    {
        QSharedPointer<Component> srcComponent;         //!< The origin component.
        QSharedPointer<BusInterface> busInterface;      //!< The bus interface.
        General::InterfaceMode mode;                    //!< The bus interface mode.
        QString instanceName;                           //!< The bus instance name.
        QString description;                            //!< The description of the instance.
        QPointF pos;                                    //!< Original position of the instance.

        /*!
        *  Constructor.
        */
        BusInterfaceCopyData()
            : srcComponent(),
            busInterface(),
            mode(),
            instanceName(),
            description()
        {
        }
    };

    //-----------------------------------------------------------------------------
    //! Clipboard copy data for a collection of copied bus port instances.
    //-----------------------------------------------------------------------------
    struct BusPortCollectionCopyData
    {
        QList<BusInterfaceCopyData> instances;

        /*!
        *  Constructor.
        */
        BusPortCollectionCopyData()
            : instances()
        {
        }
    };

    //-----------------------------------------------------------------------------
    //! Clipboard copy data for a collection of copied bus interface instances.
    //-----------------------------------------------------------------------------
    struct BusInterfaceCollectionCopyData
    {
        QList<BusInterfaceCopyData> instances;

        /*!
        *  Constructor.
        */
        BusInterfaceCollectionCopyData()
            : instances()
        {
        }
    };


    //-----------------------------------------------------------------------------
    //! Clipboard copy data for a single component instance.
    //-----------------------------------------------------------------------------
    struct ComponentInstanceCopyData
    {
        QSharedPointer<Component> component;            //!< The referenced component.
        QString instanceName;                           //!< The instance name.
        QString displayName;                            //!< The display name.
        QString description;                            //!< The description of the instance.
        QPointF pos;                                    //!< Original position of the instance.
        QMap<QString, QString> configurableElements;    //!< Configurable element values.
        QMap<QString, QPointF> busInterfacePositions;   //!< Bus interface positions.
        QMap<QString, QPointF> adHocPortPositions;      //!< Ad-hoc port positions.
        QMap<QString, bool> adHocVisibilities;          //!< Ad-hoc visibilities.

        ComponentInstanceCopyData()
            : component(),
              instanceName(),
              displayName(),
              description(),
              configurableElements(),
              busInterfacePositions(),
              adHocPortPositions(),
              adHocVisibilities()
        {
        }
    };

    //-----------------------------------------------------------------------------
    //! Clipboard copy data for a collection of copied component instances.
    //-----------------------------------------------------------------------------
    struct ComponentCollectionCopyData
    {
        QList<ComponentInstanceCopyData> instances;

        /*!
         *  Constructor.
         */
        ComponentCollectionCopyData()
            : instances()
        {
        }
    };

    //-----------------------------------------------------------------------------
    //! Clipboard copy data for a single column.
    //-----------------------------------------------------------------------------
    struct ColumnCopyData
    {
        ColumnDesc desc;                            //!< Column description.
        ComponentCollectionCopyData components;     //!< Components.
        BusInterfaceCollectionCopyData interfaces;  //!< Top-level bus interfaces.

        /*!
         *  Constructor.
         */
        ColumnCopyData()
            : desc(),
              components(),
              interfaces()
        {
        }
    };

    //-----------------------------------------------------------------------------
    //! Clipboard copy data for a collection of columns.
    //-----------------------------------------------------------------------------
    struct ColumnCollectionCopyData
    {
        QList<ColumnCopyData> columns;

        /*!
         *  Constructor.
         */
        ColumnCollectionCopyData()
            : columns()
        {
        }
    };
    
    /*!
     *  Constructor.
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
    /*!
     *  Selects all (columns) in the design.
     */
    void selectAll();

    /*!
     *  Brings the selected item to front.
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

	/*!
     *  Called when copy is selected from the context menu.
     */
	virtual void onCopyAction();

    /*!
     *  Called when paste is selected from the context menu.
     */
	virtual void onPasteAction();


    /*!
     *  Called when add to library is selected from the context menu.
     */
	virtual void onAddAction();

    /*!
     *  Called when open component is selected from the context menu.
     */
	virtual void onOpenComponentAction();


    /*!
     *  Called when open HW design is selected from the context menu.
     */
	virtual void onOpenDesignAction();


protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);

    void endConnect();

    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void wheelEvent(QGraphicsSceneWheelEvent *event);

    //! Called when a key has been pressed.
    void keyPressEvent(QKeyEvent *event);

    //! Called when a key has been release.
    void keyReleaseEvent(QKeyEvent *event);

    void dropEvent(QGraphicsSceneDragDropEvent *event);
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
    void dragMoveEvent(QGraphicsSceneDragDropEvent *event);

    void updateDropAction(QGraphicsSceneDragDropEvent* event);

    void dragLeaveEvent(QGraphicsSceneDragDropEvent * event);

    void disableHighlight();

	/*!
     *  Creates the context menu for function contextMenuEvent().
     *
     *      @param [in] pos Mouse position when the menu is requested.
	 *
	 *      @return The menu with allowed actions or 0 if no menu is allowed or nothing to show.
     */
	virtual QMenu* createContextMenu(QPointF const& pos);

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

    /*!
     *  Initializes the context menu actions.
     */
	void setupActions();

    /*!
     *  Copies component instances in a format which can be saved to clipboard.
     *
     *      @param [in]  items       The bus port instance items to copy.
     *      @param [out] collection  The resulted collection of bus port instance copy data.
     */
    void copyInterfaces(QList<QGraphicsItem*> const& items, BusPortCollectionCopyData &collection);

    /*!
     *  Copies component instances in a format which can be saved to clipboard.
     *
     *      @param [in]  items       The top-level bus interface instance items to copy.
     *      @param [out] collection  The resulted collection of bus interface instance copy data.
     */
    void copyInterfaces(QList<QGraphicsItem*> const& items, BusInterfaceCollectionCopyData &collection);

    /*!
     *  Copies component instances in a format which can be saved to clipboard.
     *
     *      @param [in]  items       The component instance items to copy.
     *      @param [out] collection  The resulted collection of component instance copy data.
     */
    void copyInstances(QList<QGraphicsItem*> const& items, ComponentCollectionCopyData &collection);
 
    /*!
     *  Pastes bus port instances from a copy data collection.
     *
     *      @param [in] collection     The collection of bus port instance copy data.
     *      @param [in] component      The component where to place the ports.
     *      @param [in] cmd            The parent undo command for the paste undo commands.
     */
   void pasteInterfaces(BusPortCollectionCopyData const& collection,
                        HWComponentItem* component, QUndoCommand* cmd);

   /*!
     *  Pastes top-level bus interface instances from a copy data collection.
     *
     *      @param [in] collection     The collection of bus interface instance copy data.
     *      @param [in] column         The column where to place the instances.
     *      @param [in] cmd            The parent undo command for the paste undo commands.
     *      @param [in] userCursorPos  If true, the instances are placed close to the cursor position.
     *                                 Otherwise the original positions are used.
     */
    void pasteInterfaces(BusInterfaceCollectionCopyData const& collection,
                         GraphicsColumn* column, QUndoCommand* cmd, bool useCursorPos);

    /*!
     *  Pastes component instances from a copy data collection.
     *
     *      @param [in] collection     The collection of component instance copy data.
     *      @param [in] column         The column where to place the instances.
     *      @param [in] cmd            The parent undo command for the paste undo commands.
     *      @param [in] userCursorPos  If true, the instances are placed close to the cursor position.
     *                                 Otherwise the original positions are used.
     */
    void pasteInstances(ComponentCollectionCopyData const& collection,
                        GraphicsColumn* column, QUndoCommand* cmd, bool useCursorPos);

    /*!
     *  Enables/disable context menu actions based on the current selection.
     */
    void prepareContextMenuActions();

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

    //! Old selected items.
    QList<QGraphicsItem*> oldSelectedItems_;

    //! Context menu select all action.
    QAction selectAllAction_;

    //! Context menu copy action.
    QAction copyAction_;

    //! Context menu paste action.
    QAction pasteAction_;

    //! Context menu action for adding a draft component to library.
    QAction addAction_;

    //! Context menu action for opening a component.
    QAction openComponentAction_;

    //! Context menu action for opening a component design.
    QAction openDesignAction_;

	//! If true, context menu is enabled.
	bool showContextMenu_;

    //! Cursor position where the user right-presses to open the context menu.
    QPoint contextPos_;
};

#endif // HWDESIGNDIAGRAM_H
