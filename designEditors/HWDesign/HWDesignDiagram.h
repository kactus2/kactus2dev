/* 
 *
 * 		filename: HWDesignDiagram.h
 */

#ifndef HWDESIGNDIAGRAM_H
#define HWDESIGNDIAGRAM_H

#include "AdHocEnabled.h"

#include <designEditors/common/ComponentDesignDiagram.h>
#include <IPXACTmodels/businterface.h>
#include <IPXACTmodels/ColumnDesc.h>

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
class HWDesignDiagram : public ComponentDesignDiagram
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
        bool topLevelIf;                                //!< Top-level or bus port interface.

        /*!
        *  Constructor.
        */
        BusInterfaceCopyData()
            : srcComponent(),
            busInterface(),
            mode(),
            instanceName(),
            description(),
            topLevelIf()
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
     *  Creates a design based on the contents in the diagram.
     *
     *      @param [in] vlnv The vlnv for the design.
     *
     *      @return The created design.
     */
    virtual QSharedPointer<Design> createDesign(VLNV const& vlnv) const;


    /*! \brief Set the IP-XACT document that is viewed in HWDesignDiagram
     *
     */
    void loadDesign(QSharedPointer<Design> design);

    /*! \brief Get HWComponentItem that has the given instance name
     *
     */
    HWComponentItem* getComponent(const QString &instanceName);

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
	virtual void onAddToLibraryAction();

protected:

    /*!
     *  Opens a design for a given component.
     *
     *      @param [in] component   The component whose design to open.
     *      @param [in] viewName    The name of the view to open.
     */
    void openDesignForComponent(ComponentItem* component, QString const& viewName);

    //! Handler for mouse double click events.
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent);

    /*!
     *  Adds a new bus to library based on the given endpoint.
     *
     *      @param [in] endpoint   The endpoint to create the bus from.
     */
    void addBusToLibrary(HWConnectionEndpoint* endpoint);

    //! Handler for drag enter event. 
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event);

    //! Handler for drag leave event.
    void dragLeaveEvent(QGraphicsSceneDragDropEvent * event);

    //! Handler for drop event.
    void dropEvent(QGraphicsSceneDragDropEvent *event);

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
    void openComponentByActiveView(ComponentItem * comp);

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
    HWDesignDiagram(HWDesignDiagram const& rhs);
    HWDesignDiagram& operator=(HWDesignDiagram const& rhs);

    /*!
     *  Called when an item has been selected in the diagram.
     *
     *      @param [in] newSelection The selected item.
     */
    virtual void onSelected(QGraphicsItem* newSelection);

    /*!
     *  Creates a connection between the given two endpoints.
     *
     *      @param [in] startPoint  The starting connection end point.
     *      @param [in] endPoint    The ending connection end point.
     *
     *      @return The created connection.
     */
    virtual GraphicsConnection* createConnection(ConnectionEndpoint* startPoint, ConnectionEndpoint* endPoint);

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
     *  Adds a new interface to the given diagram column.
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
     *  Finds the item types for the given column. If the types are ambiguous, asks the user for types.
     *
     *      @param [in] column   The column to find the types for.
     *
     *      @return The column types.
     */
    unsigned int findColumnItemType(GraphicsColumn* column) const;

    /*!
     *  Adds a draft component instance to the design.
     *
     *      @param [in] column      The column to add the instance to.
     *      @param [in] position    The initial position to add the instance at.     
     */
    void addDraftComponentInstance(GraphicsColumn* column, QPointF const& position);

    /*!
     *  Adds a draft interface in a draft component instance.
     *
     *      @param [in] targetComponent     The component item to add the interface to.
     *      @param [in] position            The initial position of the interface.     
     */
    void addDraftComponentInterface(HWComponentItem* targetComponent, QPointF const& position);

    /*!
     *  Performs the replacing of destination component with source component.
     *
     *      @param [in] destComp        The component to replace.
     *      @param [in] sourceComp      The replacing component.     
     */
    virtual void replace(ComponentItem* destComp, ComponentItem* sourceComp);

    /*!
     *  Creates a missing port to the given component item.
     *
     *      @param [in] portName   The name of the port to create.
     *      @param [in] component  The parent component.
     *      @param [in] design     The design containing related information.
     */
    BusPortItem* createMissingPort(QString const& portName, HWComponentItem* component, QSharedPointer<Design> design);

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
   void pasteInterfaces(BusInterfaceCollectionCopyData const& collection,
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


    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The type of the item being dragged.
    ColumnItemType dragCompType_;

    //! Flag for indicating that the item being dragged is a bus.
    bool dragBus_;

    //! The possible end point under cursor while performing drag.
    HWConnectionEndpoint* dragEndPoint_;
};

#endif // HWDESIGNDIAGRAM_H
