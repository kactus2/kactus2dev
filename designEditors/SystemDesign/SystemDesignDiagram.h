//-----------------------------------------------------------------------------
// File: SystemDesignDiagram.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 23.4.2012
//
// Description:
// System design diagram to work as a drawing canvas.
//-----------------------------------------------------------------------------

#ifndef SYSTEMDESIGNDIAGRAM_H
#define SYSTEMDESIGNDIAGRAM_H

#include <designEditors/common/ComponentDesignDiagram.h>

#include <common/graphicsItems/ConnectionEndpoint.h>

#include <QSharedPointer>

class Component;
class Design;
class LibraryInterface;
class IGraphicsItemStack;
class GenericEditProvider;
class SystemDesignWidget;
class HWMappingItem;
class SystemComponentItem;
class GraphicsConnection;
class SWComponentItem;
class SWPortItem;
class SWConnectionEndpoint;

//-----------------------------------------------------------------------------
//! SystemDesignDiagram class.
//-----------------------------------------------------------------------------
class SystemDesignDiagram : public ComponentDesignDiagram
{
    Q_OBJECT

public:
    // Column widths.
    enum
    {
		SYSTEM_COLUMN_WIDTH = 379,
        SW_COLUMN_WIDTH = 319,
		IO_COLUMN_WIDTH = 119
    };


    //-----------------------------------------------------------------------------
    //! Clipboard copy data for a single API/COM port.
    //-----------------------------------------------------------------------------
	struct PortCopyData
	{
        QString name;                              //!< The name of the interface.
        // Only one of these is valid at one time.
		QSharedPointer<ApiInterface> apiInterface; //!< API interface.
		QSharedPointer<ComInterface> comInterface; //!< COM interface.
        QPointF pos;                               //!< The original position.

		/*!
         *  Default constructor.
         */
        PortCopyData() : apiInterface(), comInterface()
        {
        }

        /*!
         *  Constructor.
         */
		PortCopyData(QSharedPointer<ApiInterface> apiInterface, QSharedPointer<ComInterface> comInterface)
            : apiInterface(apiInterface),
              comInterface(comInterface)
        {
        }
	};

    //-----------------------------------------------------------------------------
    //! Clipboard copy data for a collection of API/COM ports.
    //-----------------------------------------------------------------------------
    struct PortCollectionCopyData
    {
        QList<PortCopyData> ports;

        /*!
         *  Constructor.
         */
        PortCollectionCopyData()
            : ports()
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
        QMap<QString, QString> propertyValues;          //!< Property values.
        QString fileSetRef;                             //!< File set reference.
        QMap<QString, QPointF> apiInterfacePositions;   //!< API interface positions.
        QMap<QString, QPointF> comInterfacePositions;   //!< COM interface positions.
        bool isDraft;                                   //!< Identifier for draft component instances.

        ComponentInstanceCopyData()
            : component(),
              instanceName(),
              displayName(),
              description(),
              propertyValues(),
              fileSetRef(),
              apiInterfacePositions(),
              comInterfacePositions()
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
        PortCollectionCopyData interfaces;          //!< COM/API interfaces.

        /*!
         *  Constructor.
         */
        ColumnCopyData()
            : desc(),
              components()/*,
              interfaces()*/
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
     *
     *      @param [in] lh            The library interface.
     *      @param [in] editProvider  The edit provider.
     *      @param [in] parent        The parent widget.
     */
    SystemDesignDiagram(bool onlySW, LibraryInterface* lh,
                        GenericEditProvider& editProvider, SystemDesignWidget* parent = 0);

    /*!
     *  Destructor.
     */
    ~SystemDesignDiagram();

    /*! 
     *  Creates a design based on the contents in the diagram.
     *
     *      @param [in] vlnv The vlnv for the design.
     *
     *      @return The created design.
     */
    virtual QSharedPointer<Design> createDesign(VLNV const& vlnv) const;

    /*!
     *  Reflects the changes in the design to the top-level component.
     */
    virtual void updateHierComponent();

    /*!
     *  Adds a new column to the diagram.
     *
     *      @param [in] desc The column description.
     */
    void addColumn(ColumnDesc const& desc);

public slots:

    /*!
     *  Called when copy is selected from the context menu.
     */
	void onCopyAction();

    /*!
     *  Called when paste is selected from the context menu.
     */
	void onPasteAction();

    /*!
     *  Called when add to library is selected from the context menu.
     */
	virtual void onAddToLibraryAction();

protected:

    /*!
     *  Opens a design for a given component.
     *
     *      @param [in] component   The component whose design to open.
     *      @param [in] viewName    The name of the view to open.
     */
    virtual void openDesignForComponent(ComponentItem* component, QString const& viewName);

    //! Called when the mouse is double-clicked.
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* mouseEvent);

    //! Called when an drag enters the diagram.
    void dragEnterEvent(QGraphicsSceneDragDropEvent* event);

    //! Called when the drag leaves the diagram.
    void dragLeaveEvent(QGraphicsSceneDragDropEvent* event);

    //! Called when an object is dropped to the diagram.
    void dropEvent(QGraphicsSceneDragDropEvent* event);

    //! Updates the dropAction and highlight according to underlying element.
    virtual void updateDropAction(QGraphicsSceneDragDropEvent* event);

    /*!
     *  Checks if open component action should be enabled.
     *
     *      @return True, if action should be enabled, otherwise false.
     */
    virtual bool openComponentActionEnabled() const;

    /*!
     *  Checks if the given item is a hierarchical component.
     *
     *      @param [in] item   The item to check.
     *
     *      @return True, if item is a hierarchical component, otherwise false.
     */
    virtual bool isHierarchicalComponent(QGraphicsItem* item) const;

    /*!
     *  Checks if copy action should be enabled.
     *
     *      @return True, if action should be enabled, otherwise false.
     */
    virtual bool copyActionEnabled() const;

    /*!
     *  Checks if paste action should be enabled.
     *
     *      @return True, if action should be enabled, otherwise false.
     */
    virtual bool pasteActionEnabled() const;

    /*!
     *  Opens the given component according to the active view of the component.
     *
     *      @param [in] comp   The component to open.
     */
    virtual void openComponentByActiveView(ComponentItem* comp);

   /*!
    *  Gets the names of hierarchical views of a component.
    *
    *      @param [in] component   The component whose hierarchical views to get.
    *
    *      @return The names of the hierarchical views.
    */
    virtual QStringList hierarchicalViewsOf(ComponentItem* component) const;

    /*!
     *  Gets the graphics item type of the components in the diagram.
     *     
     *      @return The type of the components.
     */
    virtual int componentType() const;

    /*!
     *  Gets the graphics item type of the off page connectors in the diagram.
     *     
     *      @return The type of the components.
     */
    virtual int offpageConnectorType() const;

    /*!
     *  Gets the graphics item type of the connections in the diagram.
     *     
     *      @return The type of the components.
     */
    virtual int connectionType() const;

private:
    // Disable copying.
    SystemDesignDiagram(SystemDesignDiagram const& rhs);
    SystemDesignDiagram& operator=(SystemDesignDiagram const& rhs);

    /*!
     *  Called when an item has been selected in the diagram.
     *
     *      @param [in] newSelection The selected item.
     */
    virtual void onSelected(QGraphicsItem* newSelection);

    /*!
     *  Opens the system design for editing.
     */
    void loadDesign(QSharedPointer<Design> design);

    /*!
     *  Loads the COM connections from the given design.
     */
    void loadComConnections(QSharedPointer<Design> design);

    /*!
     *  Loads the API dependencies from the given design.
     */
    void loadApiDependencies(QSharedPointer<Design> design);

    /*!
     *  Returns the HW component instance with the given name.
     */
    HWMappingItem* getHWComponent(QString const& instanceName);

    /*!
     *  Returns the HW component instance with the given UUID.
     */
    HWMappingItem* getHWComponentByUUID(QString const& uuid);

    /*!
     *  Returns the component with the given name (either HW or SW).
     *
     *      @param [in] instanceName The name of the instance to search for.
     *
     *      @return The corresponding component, or null if not found.
     */
    SystemComponentItem* getComponent(QString const& instanceName);

    /*!
     *  Creates a missing port to the given component item.
     *
     *      @param [in] portName   The name of the port to create.
     *      @param [in] type       The port endpoint type.
     *      @param [in] component  The parent component.
     *      @param [in] design     The design containing related information.
     */
    SWPortItem* createMissingPort(QString const& portName, ConnectionEndpoint::EndpointType type,
                                  SystemComponentItem* component, QSharedPointer<Design> design);

    /*!
     *  Imports SW instances and related connections from the given design to the diagram.
     *
     *      @param [in] design    The design to import.
     *      @param [in] stack     The stack where to place the imported elements.
     *      @param [in] guidePos  Position used as a guide where to place the imported instances.
     */
    void importDesign(QSharedPointer<Design> design, IGraphicsItemStack* stack, QPointF const& guidePos);

    /*!
     *  Creates a connection between the given two endpoints.
     *
     *      @param [in] startPoint  The starting connection end point.
     *      @param [in] endPoint    The ending connection end point.
     *
     *      @return The created connection.
     */
    virtual  GraphicsConnection* createConnection(ConnectionEndpoint* startPoint, ConnectionEndpoint* endPoint);

    /*!
     *  Creates a connection between the given endpoint and a coordinate point.
     *
     *      @param [in] startPoint  The starting connection end point.
     *      @param [in] endPoint    The ending coordinate point.
     *
     *      @return The created connection.
     */
    virtual GraphicsConnection* createConnection(ConnectionEndpoint* startPoint, QPointF const& endPoint);

    /*!
     *  Creates an add command for a given connection.
     *
     *      @param [in] connection  The connection to create a command for.     
     *
     *      @return The created add command.
     */
    virtual QSharedPointer<QUndoCommand> createAddCommandForConnection(GraphicsConnection* connection);

    /*!
     *  Adds a new top-level interface to the given diagram column.
     *
     *      @param [in] column The column where to add the interface.
     *      @param [in] pos    The interface position.
     */
    virtual void addTopLevelInterface(GraphicsColumn* column, QPointF const& pos);

    /*!
     *  Handler for draft tool clicks. Creates a draft component instance or a draft interface according to the
     *  clicked position.
     *
     *      @param [in] clickedPosition   The position to create the draft item to.     
     */
    virtual void draftAt(QPointF const& clickedPosition);

    /*!
     *  Performs the replacing of destination component with source component.
     *
     *      @param [in] destComp        The component to replace.
     *      @param [in] sourceComp      The replacing component.     
     */
    virtual void replace(ComponentItem* destComp, ComponentItem* sourceComp);

    /*!
     *  Copies SW component instances in a format which can be saved to clipboard.
     *
     *      @param [in]  items       The component instance items to copy.
     *      @param [out] collection  The resulted collection of component instance copy data.
     */
    void copySWInstances(QList<QGraphicsItem*> const& items, ComponentCollectionCopyData &collection);

    /*!
     *  Copies interfaces in a format which can be saved to clipboard.
     *
     *      @param [in]  items       The interface items to copy.
     *      @param [out] collection  The resulted collection of interface copy data.
     */
    void copyInterfaces(QList<QGraphicsItem*> const& items, PortCollectionCopyData& collection);

    /*!
     *  Pastes component instances from a copy data collection.
     *
     *      @param [in] collection     The collection of component instance copy data.
     *      @param [in] column         The item stack where to place the instances.
     *      @param [in] cmd            The parent undo command for the paste undo commands.
     *      @param [in] userCursorPos  If true, the instances are placed close to the cursor position.
     *                                 Otherwise the original positions are used.
     */
    void pasteSWInstances(ComponentCollectionCopyData const& collection,
                          IGraphicsItemStack* stack, QUndoCommand* cmd, bool useCursorPos);

    /*!
     *  Pastes interfaces from a copy data collection.
     *
     *      @param [in] collection  The collection of interface copy data.
     *      @param [in] targetComp  The target component. If null, the interfaces are pasted to the top level component.
     *      @param [in] cmd         The parent undo command for the paste undo commands.
     */
    void pasteInterfaces(PortCollectionCopyData const& collection, SWComponentItem* targetComp, QUndoCommand* cmd);

    /*!
     *  Pastes interfaces from a copy data collection to a top-level item stack.
     *
     *      @param [in] collection     The collection of interface copy data.
     *      @param [in] stack          The target item tack.
     *      @param [in] cmd            The parent undo command for the paste undo commands.
     *      @param [in] userCursorPos  If true, the interfaces are placed close to the cursor position.
     *                                 Otherwise the original positions are used.
     */
    void pasteInterfaces(PortCollectionCopyData const& collection, IGraphicsItemStack* stack, QUndoCommand* cmd,
                         bool useCursorPos);

    //-----------------------------------------------------------------------------
    //! Drag type enumeration.
    //-----------------------------------------------------------------------------
    enum DragType
    {
        DRAG_TYPE_NONE = 0,
        DRAG_TYPE_SW,
        DRAG_TYPE_HW,
        DRAG_TYPE_DEFINITION,
        DRAG_TYPE_DESIGN
    };

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! If true, the widget is used for editing SW designs.
    bool onlySW_;

    //! Indicates what type of object is being dragged into the diagram.
    DragType dragType_;

    //! The possible end point under cursor while performing drag.
    SWConnectionEndpoint* dragEndPoint_;
};

//-----------------------------------------------------------------------------

#endif // SYSTEMDESIGNDIAGRAM_H
