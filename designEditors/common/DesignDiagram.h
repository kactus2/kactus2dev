//-----------------------------------------------------------------------------
// File: DesignDiagram.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 24.4.2012
//
// Description:
// Base class for all design diagrams.
//-----------------------------------------------------------------------------

#ifndef DESIGNDIAGRAM_H
#define DESIGNDIAGRAM_H

#include <designEditors/HWDesign/AdHocEnabled.h>
#include <designEditors/common/DrawMode.h>

#include <IPXACTmodels/designconfiguration.h>
#include <IPXACTmodels/ColumnDesc.h>

#include <QAction>
#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QSharedPointer>
#include <QStringList>
#include <QUndoCommand>

class LibraryInterface;
class GenericEditProvider;
class Component;
class ConnectionEndpoint;
class GraphicsConnection;
class GraphicsColumnLayout;
class ComponentItem;
class Design;
class VLNV;
class DesignWidget;
class Associable;
class StickyNote;
class Kactus2Group;
class StickyNoteAddCommand;
//-----------------------------------------------------------------------------
//! Base class for all design diagrams.
//-----------------------------------------------------------------------------
class DesignDiagram : public QGraphicsScene, public AdHocEnabled
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] lh            The library interface.
     *      @param [in] editProvider  The edit provider.
     *      @param [in] parent        The parent widget.
     */
    DesignDiagram(LibraryInterface* lh, GenericEditProvider& editProvider, DesignWidget* parent = 0);

    /*!
     *  Destructor.
     */
    virtual ~DesignDiagram();

    /*!
     *  Clears the scene.
     */
    virtual void clearScene();

    /*!
     *  Opens a design for editing.
     *
     *      @param [in] component  The component whose design to edit.
     *      @param [in] design     The design to edit.
     *      @param [in] designConf The design configuration if one exists.
     */
    bool setDesign(QSharedPointer<Component> component, QSharedPointer<Design> design,
                   QSharedPointer<DesignConfiguration> designConf = QSharedPointer<DesignConfiguration>());

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
    virtual void updateHierComponent() = 0;

    /*!
     *  Attaches the data source to an ad-hoc editor.
     */
    virtual void attach(AdHocEditor* editor);

    /*!
     *  Detaches the data source from the ad-hoc editor.
     */
    virtual void detach(AdHocEditor* editor);

    /*!
     *  Adds an instance name to the list of used instance names.
     *
     *      @param [in] name The name of the instance.
     *
     *      @remarks This function should be called when a component instance is added to the diagram.
     */
    void addInstanceName(QString const& name);

    /*!
     *  Removes the instance name from the list so the name can be used again in the design.
	 *
	 *      @param name The name of the removed instance.
	 *
     *      @remarks This function should be called when user removes component instance.
	 */
	void removeInstanceName(const QString& name);

	/*!
     *  Updates the list of instance names.
	 *
	 *      @param oldName The old name of the component instance.
	 *      @param newName The new name of the component instance.
	 */
	void updateInstanceName(const QString& oldName, const QString& newName);

    /*!
     *  Changes the state of a visibility control.
     *
     *      @param [in] name   The name of the visibility control.
     *      @param [in] state  The new state for the visibility control.
     */
    virtual void setVisibilityControlState(QString const& name, bool state);

    /*!
     *  Sets the draw mode of the diagram.
     *
     *      @param [in] mode The draw mode.
     */
    virtual void setMode(DrawMode mode);

    /*!
     *  Sets the block diagram locked/unlocked.
     *
     *      @param [in] locked If true, the diagram is locked. Otherwise it is unlocked.
     */
    void setProtection(bool locked);

    /*!
     *  Adds a column to the diagram's layout.
     *
     *      @param [in] desc The column description.
     */
    virtual void addColumn(ColumnDesc const& desc) = 0;

    /*!
     *  Returns the current draw mode.
     */
    DrawMode getMode() const;

    /*!
     *  Returns true if the diagram is being loaded.
     */
    bool isLoading() const;

    /*!
     *  Returns true if the diagram is in locked state.
     */
    bool isProtected() const;

    /*!
     *  Returns the library interface.
     */
    LibraryInterface* getLibraryInterface() const;

    /*!
     *  Returns the parent design widget.
     */
     DesignWidget* getParent() const;

    /*!
     *  Returns the edit provider.
     */
    GenericEditProvider& getEditProvider();

    /*!
     *  Returns the component whose design is being edited.
     */
    QSharedPointer<Component> getEditedComponent() const;

    /*!
     *  Returns the design configuration.
     */
    QSharedPointer<DesignConfiguration> getDesignConfiguration() const;

    /*!
     *  Returns a list of instances currently in the design.
     */
    QList<ComponentItem*> getInstances() const;

    /*!
     *  Ensures that all items in a given list have the same type. Items with different type are unselected.
     *
     *      @param [in] items   The list to check.
     */
    void ensureOneTypeSelected(QList<QGraphicsItem*> const& items);

    /*!
     *  Returns the type of the given items if they all are of the same type.
     *
     *      @return The common type, or -1 if the items are of different type.
     */
    static int getCommonItemType(QList<QGraphicsItem*> const& items);

    /*!
     *  Sorts graphics items by X coordinate.
     */
    static bool sortByX(QGraphicsItem* lhs, QGraphicsItem* rhs);

     /*!
      *  Gets the graphics column layout of the design.
      *
      *      @return The column layout of the design.
      */
     QSharedPointer<GraphicsColumnLayout> getLayout() const;

public slots:
    //! Called when the diagram is shown.
    void onShow();

    /*!
     *  Selects all (columns) in the design.
     */
    void selectAll();

    //! Called when the view has been scrolled vertically.
    virtual void onVerticalScroll(qreal y);

    //! Called when a component instance is added to the diagram.
    virtual void onComponentInstanceAdded(ComponentItem* item);

    //! Called when a component instance is remove from the diagram.
    virtual void onComponentInstanceRemoved(ComponentItem* item);

    //! Called when a vendor extension is added to the design.
    virtual void onVendorExtensionAdded(QSharedPointer<VendorExtension> extension);

    //! Called when a vendor extension is removed from the design.
    virtual void onVendorExtensionRemoved(QSharedPointer<VendorExtension> extension);

    void onBeginAssociation(Associable* startingPoint);

signals:
    //! Emitted when component with given vlnv should be opened in editor.
    void openComponent(const VLNV& vlnv);

    //! \brief Emitted when the user double-clicks a hierarchical SW component.
    void openSWDesign(const VLNV& vlnv, const QString& viewName);

    //! Emitted when a C source file should be opened for editing.
    void openCSource(ComponentItem* compItem);

    //! Emitted when user double clicks on a hierarchical component.
    void openDesign(const VLNV& vlnv, const QString& viewName);

    //! Signaled when the bus with the given vlnv should be opened for editing.
    void openBus(VLNV const& vlnv, VLNV const& absDefVLNV, bool disableBusDef);

    //! Signaled when the draw mode has changed.
    void modeChanged(DrawMode mode);

    //! Signaled when the contents of the diagram have changed.
    void contentChanged();

    //! Sends an error message to the user.
    void errorMessage(const QString& errorMessage);

    //! Sends a notification to the user.
    void noticeMessage(const QString& noticeMessage);

    //! Emitted when a help page should be changed in the context help window.
    void helpUrlRequested(QString const& url);

    //! Signaled when a new component is instantiated to the design.
    void componentInstantiated(ComponentItem* item);

    //! Signaled when a component instance is removed from the design.
    void componentInstanceRemoved(ComponentItem* item);

    //! Signaled when user selects a component on the draw board.
    void componentSelected(ComponentItem* component);

    //! Signaled when the user selects an interface on the draw board.
    void interfaceSelected(ConnectionEndpoint* endpoint);

    //! Signaled when the user selects a connection on the draw board.
	void connectionSelected(GraphicsConnection* connection);

    //! Signaled when all items are deselected.
    void clearItemSelection();

protected:

    virtual void wheelEvent(QGraphicsSceneWheelEvent* event);

    /*!
     *  Creates a unique instance name for the given component.
     *
     *      @param [in] component The component whose instance is being created.
     *
     *      @return The name for the component instance.
     */
    QString createInstanceName(QSharedPointer<Component> component);

    /*!
     *  Creates a unique instance name with the given base name.
     *
     *      @param [in] baseName The base name for the instance.
     */
    QString createInstanceName(QString const& baseName);

    /*!
     *  Draws the diagram background.
     */
     virtual void drawBackground(QPainter* painter, QRectF const& rect);

    /*!
     *  Picks the top-most component at the given position.
     *
     *      @param [in] pos The picking positions.
     *
     *      @return The top-most component at the given positions, or 0 if there is no component.
     */
    ComponentItem* getTopmostComponent(QPointF const& pos);

    /*!
     *  Finds the underlying item of a given item ignoring text labels and pixmap items. 
     *
     *  This fixes the problem when the user click above a text label or a pixmap but
     *  actually wants to select the parent item (such as the actual component, not its label).
     *
     *      @param [in] item   The item whose base item to find.
     *
     *      @return The bottom-most item.
     */
    QGraphicsItem* getBaseItemOf(QGraphicsItem* item) const;

    /*!
     *  Adds a new note to the design.
     *
     *      @param [in] position   The initial position of the note.
     */
    virtual void createNoteAt(QPointF const& position);

     /*!
     *  Called when an item has been selected in the diagram.
     *
     *      @param [in] newSelection The selected item.
     */
    virtual void onSelected(QGraphicsItem* newSelection) = 0;

	/*!
     *  Creates the context menu for function contextMenuEvent().
     *
     *      @param [in] pos Mouse position when the menu is requested.
	 *
	 *      @return The menu with allowed actions or 0 if no menu is allowed.
     */
	virtual QMenu* createContextMenu(QPointF const& pos);

	/*!
     *  Event handler for receiving context menu events (e.g. right mouse click).
     *
     *      @param [in] event The triggering event.
     */
	virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent* event);

    /*!
     *  Updates the association line.
     *
     *      @param [in] cursorPosition   The current cursor position which becomes the ending point for the line.
     */
    void updateAssociationLineDraw(QPointF const& cursorPosition);

    /*!
     *  Ends the creation of a new association.
     *
     *      @param [in] endpoint   The ending point of the association.
     */
    void endAssociation(QPointF const& endpoint);

    //! Ends the drawing of a new association line.
    void endAssociationLineDraw();

    /*!
     *  Checks if two items can be associated with each other.
     *
     *      @param [in] startItem       The first item.
     *      @param [in] endItem         The second item.
     *
     *      @return True, if the items can be associated, otherwise false.
     */
    bool canAssociateItems(Associable* startItem, Associable* endItem);

    /*!
     *  Creates a command for adding an association to the diagram.
     *
     *      @param [in] startItem           The item at the starting end of the association.
     *      @param [in] endPointExtension   The vendor extension describing the ending point position.
     *
     *      @return A command for creating an association.
     */
    QSharedPointer<QUndoCommand> createAssociationAddCommand(Associable* startItem, 
        QSharedPointer<Kactus2Position> endPointExtension);

    /*!
     *  Checks if the user is currently creating a new association.
     *
     *      @return True, if an association is being created, otherwise false.
     */
    bool inAssociationMode() const;

    /*!
     *  Checks if the association can end.
     *
     *      @return True, if an association can end, otherwise false.
     */
    bool canEndAssociation() const;

    /*!
     *  Checks if context menu can be displayed.     
     *
     *      @return True, if context menu can be displayed, otherwise false.
     */
    bool contextMenuEnabled() const;

    /*!
     *  Checks if the user is currently replacing a component.
     *
     *      @return True, if a replace is in progress, otherwise false.
     */
    bool inReplaceMode() const;

    /*!
     *  Checks if the user is currently creating connections in off page mode.
     *
     *      @return True, if off page mode is set, otherwise false.
     */
    bool inOffPageMode() const;
    
    enum interactionMode{
        NORMAL = 0,
        CONTEXT_MENU,
        REPLACE,
        OFFPAGE,
        ASSOCIATE
    };

    /*!
     *  Sets the interaction mode.
     *
     *      @param [in] mode   The mode to set.
     *     
     */
    void setInteractionMode(interactionMode mode);

    //! Ends the current interaction mode.
    void endInteraction();

private:
    // Disable copying.
    DesignDiagram(DesignDiagram const& rhs);
    DesignDiagram& operator=(DesignDiagram const& rhs);

    /*!
     *  Opens the given design for editing.
     *
     *      @param [in] design The design to open.
     */
    virtual void loadDesign(QSharedPointer<Design> design) = 0;

     //! Clears and resets the current layout.
     void clearLayout();

    //! Creates sticky notes from vendor extensions.
    void loadStickyNotes();
    
    /*!
     *  Creates a command for adding a sticky note to the diagram.
     *
     *      @param [in] note   The note for which to create the command for.
     *
     *      @return The command for adding the note to diagram.
     */
    QSharedPointer<StickyNoteAddCommand> createNoteAddCommand(StickyNote* note);

    /*!
     *  Loads the associations from a sticky not from a vendor extension.
     *
     *      @param [in] note                    The note for whose associations to load.
     *      @param [in] associationsExtension   The vendor extension describing the associations.
     */
    void loadNoteAssociations(StickyNote* note, QSharedPointer<VendorExtension> associationsExtension);

    //! Enables/disables the sticky notes according to design protection state.
    void setProtectionForStickyNotes();
    void updateOpenDesignMenuFor(ComponentItem* compItem);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The parent design widget.
    DesignWidget* parent_;

    //! Library interface.
    LibraryInterface* lh_;

    //! The edit provider for undo/redo.
    GenericEditProvider& editProvider_;

    //! The component whose design is being edited.
    QSharedPointer<Component> component_;

    //! The design configuration.
    QSharedPointer<DesignConfiguration> designConf_;

    // Graphics column layout.
    QSharedPointer<GraphicsColumnLayout> layout_;

    //! The current draw mode.
    DrawMode mode_;

    //! The list of used instance names.
    QStringList instanceNames_;

    //! If true, the diagram is being loaded.
    bool loading_;

    //! If true, the diagram is locked and cannot be modified.
    bool locked_;

	//! Contains the XML header comments of the design.
	QStringList XMLComments_;

    //! The design vendor extensions.
    QList<QSharedPointer<VendorExtension> > vendorExtensions_;
    
    interactionMode interactionMode_;

    QGraphicsLineItem* associationLine_;
};

//-----------------------------------------------------------------------------

#endif // DESIGNDIAGRAM_H
