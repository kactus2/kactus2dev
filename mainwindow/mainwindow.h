//-----------------------------------------------------------------------------
// File: mainwindow.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti Maatta
// Date: 24.2.2011
//
// Description:
// The main window of Kactus2.
//-----------------------------------------------------------------------------

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <KactusAPI/include/PluginManager.h>

#include <common/widgets/tabDocument/TabDocument.h>

#include <editors/common/DrawMode.h>

#include <IPXACTmodels/common/TagData.h>

#include <IPXACTmodels/kactusExtensions/KactusAttribute.h>

#include "WorkspaceSettings.h"

#include <QMainWindow>
#include <QActionGroup>
#include <QMenu>
#include <QShowEvent>
#include <QHideEvent>

class Component;
class VLNV;

class LibraryHandler;
class ComponentItem;
class DrawingBoard;
class ConnectionEndpoint;
class GraphicsConnection;
class Ribbon;
class RibbonGroup;
class DockWidgetHandler;
class MessageMediator;

//-----------------------------------------------------------------------------
//! The main window of Kactus2.
//-----------------------------------------------------------------------------
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] library   The used IP-XACT library instance.
     *      @param [in] parent    The parent widget.
     */
    MainWindow(LibraryHandler* library, MessageMediator* messageChannel, QWidget *parent = 0);
    
	/*!
     *  The destructor.
     */
	virtual ~MainWindow();

public slots:
    /*!
     *  Opens the "New" dialog.
     */
    void createNew();

    /*!
     *  Opens the source file of the given component.
     *
     *      @param [in] compItem The component item.
     */
    void openCSource(ComponentItem* compItem);

    /*!
     *  Opens the specific C source file of the given component.
     *
     *      @param [in] filename   The name of the file to open.
     *      @param [in] component  The component containing the file.
     */
    void openCSource(QString const& filename, QSharedPointer<Component> component);

    /*!
     *  Opens a HW design.
     *
     *      @param [in] vlnv           The VLNV of the top-level component.
     *      @param [in] viewName       The name of the view to open.
     */
    void openDesign(VLNV const& vlnv = VLNV(), QString const& viewName = QString());

    /*!
     *  Opens the memory design of the current HW design.
     *  
     */
    void openMemoryDesign(); 

    /*!
     *  Opens the memory design of the HW design specified by the component VLNV and view name.
     *  
     *      @param [in] vlnv            The VLNV of the top-level component.
     *      @param [in] viewName        Name of the view to open.
     */
    void openMemoryDesign(const VLNV& vlnv, const QString& viewName = QString());    

    /*!
     *  Opens a SW design.
     *
     *      @param [in] vlnv           The VLNV of the top-level component.
     *      @param [in] viewName       The name of the view to open.
     */
	void openSWDesign(const VLNV& vlnv = VLNV(), QString const& viewName = QString());

    /*!
     *  Opens a system design.
     *
     *      @param [in] vlnv           The VLNV of the system component whose system design to open.
     *      @param [in] viewName       The name of the view to open.
     */
    void openSystemDesign(VLNV const &vlnv, QString const& viewName = QString());

    /*!
     *  Open bus to be edited.
	 *
	 *      @param [in] busDefVLNV      Identifies the bus definition.
	 *      @param [in] absDefVLNV      Identifies the abstraction definition.
     *      @param [in] disableBusDef   Flag for forcing disable on bus definitino edits.
	 */
	void openBus(const VLNV& busDefVLNV, const VLNV& absDefVLNV = VLNV(), bool disableBusDef = false);

	/*!
     *  Open catalog editor to edit catalog details.
	 *
	 *      @param [in] vlnv           Identifies the catalog to edit.
	 */
	void openCatalog(const VLNV& vlnv);

	/*!
     *  Open component editor to edit a component details.
	 *
	 *      @param [in] vlnv           Identifies the component to edit.
	 */
	void openComponent(const VLNV& vlnv);

    /*!
     *  Opens a COM definition for editing.
     *
     *      @param [in] vlnv           The VLNV of the COM definition to open.
     */
    void openComDefinition(VLNV const& vlnv);

    /*!
     *  Opens an API definition for editing.
     *
     *      @param [in] vlnv           The VLNV of the API definition to open.
     */
    void openApiDefinition(VLNV const& vlnv);

    //! Called when user closes the last design tab.
    void onLastDocumentClosed();

    //! Called when the user has selected another tab.
    void onDocumentChanged(int index);

	/*!
     *  Search the file system for new IP-Xact libraries.
	 *
	 *  This slot is called when user clicks to search for IP-Xact files.
	 */
	void onLibrarySearch();

	/*!
     *  Called when user selects a component on the draw board.
	 *
	 *      @param [in] component   Pointer to the DiagramComponent instance that is selected.
	 */
	void onComponentSelected(ComponentItem* component);

	/*!
     *  Called when user selects an interface on the draw board.
	 *
	 *      @param [in] interface   Pointer to the interface instance that is selected.
	 */
	void onInterfaceSelected(ConnectionEndpoint* interface);

	/*!
     *  Called when user selects a connection on the draw board.
	 *
	 *      @param [in] connection  Pointer to the connection that was selected.
	 */
	void onConnectionSelected(GraphicsConnection* connection);

    //! Called when the menu strip needs to be updated.
    void updateMenuStrip();

    //! Called when the zoom has changed.
    void updateZoomTools();

    //! Saves all modified documents.
    void saveAll();

    //! Adds a new column to the current HW/System design.
    void addColumn();

	//! Generates the documentation for the current component/design.
	void generateDoc();

    /*!
     *  Runs the plugin generator specified by the given action.
     *
     *      @param [in] action The action corresponding to the plugin.
     */
    void runGeneratorPlugin(QAction* action);

    //! Zooms in the current document.
    void zoomIn();

    //! Zooms out the current document.
    void zoomOut();

    //! Resets the zoom of the current document to the original 1:1 ratio.
    void zoomOriginal();

    //! Fits the current document fully in view.
    void fitInView();

    //! Undoes one operation in the current document.
    void undo();

    //! Redoes one operation in the current document.
    void redo();

    //! Called when the draw mode has changed.
    void onDrawModeChanged(DrawMode mode);
    void drawModeChange(QAction *action);

    //! Called when the state of the protection button changes.
    void onProtectionChanged(bool locked);

    //! Changes the protection of the current document.
    void changeProtection(bool locked);

    /*!
     *  Creates a new component to the library.
     *
     *      @param [in] prodHier    The product hierarchy attribute.
     *      @param [in] firmness	The firmness attribute.
     *      @param [in] tags        Document tags.
     *      @param [in] vlnv        The component's VLNV.
     *      @param [in] directory   The directory where to save the component.
     *
     *      @remarks The component editor is opened automatically after successful creation.
     */
    void createComponent(KactusAttribute::ProductHierarchy prodHier, KactusAttribute::Firmness firmness,
        QVector<TagData> tags, VLNV const& vlnv, QString const& directory);

    /*!
     *  Creates a SW component to the library.
     *
     *      @param [in] vlnv        The component's VLNV.
     *      @param [in] directory   The directory where to save the component.
     *
     *      @remarks The component editor is opened automatically after successful creation.
     */
    void createSWComponent(VLNV const& vlnv, QString const& directory);

    /*!
     *  Creates a new design.
     *
     *      @param [in] prodHier    The product hierarchy attribute.
     *      @param [in] firmness	The firmness attribute.
     *      @param [in] tags        Document tags.
     *      @param [in] vlnv        The component's VLNV.
     *      @param [in] directory   The directory where to save the component.
     *
     *      @remarks The HW design editor is opened automatically after successful creation.
     */
    void createDesign(KactusAttribute::ProductHierarchy prodHier, KactusAttribute::Firmness firmness,
        QVector<TagData> tags, VLNV const& vlnv, QString const& directory);

    /*!
     *  Creates a new design for an existing component.
     *
     *      @param [in] vlnv        The component's VLNV.
     *
     *      @remarks The HW design editor is opened automatically after successful creation.
     */
    void createDesignForExistingComponent(VLNV const& vlnv);

    void unlockNewlyCreatedDocument(VLNV const& vlnv);

    void createCatalog(VLNV const& catalogVLNV, QString const& directory);

    /*!
     *  Creates a new hierarchical SW component with an SW design.
     *
     *      @param [in] vlnv        The component's VLNV.
     *      @param [in] directory   The directory where to save the component.
     *
     *      @remarks The SW design editor is opened automatically after successful creation.
     */
    void createSWDesign(VLNV const& vlnv, QString const& directory);

    /*!
     *  Creates a SW design for an existing component.
     *
     *      @param [in] vlnv        The component's VLNV.
     *
     *      @remarks The SW design editor is opened automatically after successful creation.
     */
    void createSWDesign(VLNV const& vlnv);

    /*!
     *  Creates a new system design.
     *
     *      @param [in] compVLNV   The VLNV of an existing component.
     *      @param [in] viewName   The name of the view to which the system design will be based on.
     *      @param [in] sysVLNV    The VLNV of the system to be created.
     *      @param [in] directory  The directory where to save the system.
     */
    void createSystem(VLNV const& compVLNV, QString const& viewName, VLNV const& sysVLNV,
        QString const& directory);

    void createSystemDesign(VLNV const& vlnv);

    /*!
     *  Creates a new bus definition and abstraction definition
	 *
	 *      @param [in] vlnv        The vlnv for the bus definition
	 *      @param [in] directory   The directory where to save the bus definition and abstraction definition.
	 */
	void createBus(VLNV const& vlnv, QString const& directory);

	/*!
     *  Creates a new abstraction definition for given bus definition.
	 *
	 *      @param [in] busDefVLNV      Identifies the bus definition.
	 *      @param [in] directory       The directory where to save the abstraction definition.
	 *      @param [in] disableBusDef   If true then the bus definition editor is disabled when editor is opened.
	 */
	void createAbsDef(const VLNV& busDefVLNV, const QString& directory, bool disableBusDef);

    /*!
     *  Creates a new COM definition.
     *
     *      @param [in] vlnv        VLNV for the COM definition.
     *      @param [in] directory   The directory where to save the design.
     *
     *      @remarks The COM definition editor is opened automatically after successful creation.
     */
    void createComDefinition(VLNV const& vlnv, QString const& directory);

    /*!
     *  Creates a new API definition.
     *
     *      @param [in] vlnv        VLNV for the API definition.
     *      @param [in] directory   The directory where to save the design.
     *
     *      @remarks The API definition editor is opened automatically after successful creation.
     */
    void createApiDefinition(VLNV const& vlnv, QString const& directory);

    /*!
     *  Opens the settings dialog.
     */
    void openSettings();

    /*!
     *  Shows the about box.
     */
    void showAbout();

signals:

	//! Print a notice message to the user.
	void noticeMessage(const QString& msg);

	//! Print an error message to the user.
	void errorMessage(const QString& msg);

    //! Emitted when a help page should be changed in the context help window.
    void helpUrlRequested(QString const& url);

protected:

	//! Called when the user requests to close the program.
    virtual void closeEvent(QCloseEvent* event);

	//! Called when user i.e minimizes the main window.
	virtual void hideEvent(QHideEvent* event);

	//! Called when user i.e maximizes the main window after it has been minimized.
	virtual void showEvent(QShowEvent* event);


private slots:
	//! Handler for design widget's clearItemSelection signal.
	void onClearItemSelection();

	//! Handler for signals that inform that design has changed.
	void onDesignChanged();

    //! Saves the document hierarchy under current document.
    void saveCurrentDocumentHierarchy();

	//! Create a pop up menu to select which dock widgets to display.
	void selectVisibleDocks();

    //! Opens the visibility control menu.
    void openVisibilityControlMenu();

    //! Opens the workspace management menu.
    void openWorkspaceMenu();

    /*!
     *  Handles the situation when a workspace has been changed.
     */
    void onWorkspaceChanged(QString const& workspace);

    /*!
     *  Handles the toggling of visibility controls.
     */
    void onVisibilityControlToggled(QAction*);

    //! Opens a dialog for setting library locations.
    void setLibraryLocations();

    //! Runs an import wizard for the currently open component.
    void onRunImportWizard();

    /*!
     *  Update the instance editor, when the design document is refreshed.
     */
    void onDesignDocumentRefreshed();

    /*!
     *  Configure the used views.
     */
    void onConfigureViews();

    /*!
     *  Set the filtering of chained address space memory connections within a memory design.
     *
     *      @param [in] filterChains    Value for the filter.
     */
    void onFilterAddressSpaceChains(bool filterChains);

    /*!
     *  Set the condensing of memory graphics items within a memory design.
     *
     *      @param [in] condenseMemoryItems     Value for condensing the memory graphics items.
     */
    void onCondenseMemoryItems(bool condenseMemoryItems);

    /*!
     *  Condense all the field graphics items to their original width.
     */
    void onCondenseFieldItems();

    /*!
     *  Extend all the field graphics items to fully display their name label.
     */
    void onExtendFieldItems();

    /*!
     *  Set the filtering of address space segments within a memory design.
     *
     *      @param [in] filterSegments  Value for the filter.
     */
    void onFilterSegments(bool filterSegments);

    /*!
     *  Set the filtering of address blocks within a memory design.
     *
     *      @param [in] filterBlocks    Value for the filter.
     */
    void onFilterAddressBlocks(bool filterBlocks);

    /*!
     *  Set the filtering of registers within a memory design.
     *
     *      @param [in] filterRegisters     Value for the filter.
     */
    void onFilterRegisters(bool filterRegisters);

    /*!
     *  Set the filtering of fields within a memory design.
     *
     *      @param [in] filterFields    Value for the filter.
     */
    void onFilterFields(bool filterFields);

    /*!
     *  Set the filtering of the unconnected memory items within a memory design.
     *
     *      @param [in] filterUnconnected   Value for the filter.
     */
    void onFilterUnconnectedMemoryItems(bool filterUnconnected);

    /*!
     *  Open the document containing the selected component and select the identified component item editor.
     *
     *      @param [in] componentVLNV       VLNV of the selected component.
     *      @param [in] identifierChain     List of string identifying the selected component item.
     */
    void onOpenComponentItem(const VLNV& componentVLNV, QVector<QString> identifierChain);

    /*!
     *  Adjust the visibility of the selected dock widget.
     *
     *      @param [in] type    Type of the selected dock widget.
     *      @param [in] show    New value for the visibility of the dock widget.
     */
    void onAdjustVisibilityInWindow(TabDocument::SupportedWindows type, bool show);

    /*!
     *  Updates the workspace menu.
     */
    void updateWorkspaceMenu();

private:
	// Disable copying.
	MainWindow(MainWindow const& rhs);
    MainWindow& operator=(MainWindow const& rhs);

    /*!
     *  Returns true if a design with the given vlnv is already open.
     *
     *      @remarks If the design is already open, it is displayed.
     */
    bool isDesignOpen(VLNV const& vlnv, KactusAttribute::Implementation implementation) const;

    /*!
     *  Checks if the given list of VLVNs contain invalid references.
     *
     *      @param [in] hierRefs            The list of VLNVs to check
     *      @param [in] referencingVlnv     The referencing VLNV.
     *
     *      @return True, if at least one invalid reference is found, otherwise false.
     */
    bool hasInvalidReferences(QList<VLNV> hierRefs, VLNV const& referencingVlnv);

    /*!
     *  Restores the program's settings.
	 */
	void restoreSettings();

	/*! 
     *  Save the program's settings.
	 */
	void saveSettings();

    /*!
     *  Restores the settings for the given workspace.
     *
     *      @param [in] workspaceName The name of the workspace.
     */
    void loadWorkspace(QString const& workspaceName);

	/*!
     *  Set up the actions in the tool bars
	 */
	void setupActions();

	/*!
     *  Set up the menus for the main window.
	 *
	 *  This function is used by the setupActions() after it has created the actions for the menus and should not
     *  be called directly.
	 */
	void setupMenus();

    void setupToolbars();

    /*!
     *  Adds generator plugin actions to the generation group.
     */
    void createGeneratorPluginActions();

    /*!
     *  Updates the generation group.
     */
    void updateGeneratorPluginActions();

	/*!
     *  Set up the drawing view where hierarchical components are edited.
	 */
	void setupDrawBoard();

    /*!
     *  Connect the necessary signals of the dock widget handler.
     */
    void connectDockHandler();

    /*!
     *  Get the library handler from the dock widget handler and connected the necessary signals.
     */
    void setupAndConnectLibraryHandler();

	/*!
     *  Check if a document with the given vlnv is already open.
	 *
	 *      @param [in] vlnv    The vlnv that identifies the document.
	 * 
	 *      @remark If a document with given vlnv is open then the document is shown.
	 * 
	 *      @return True if the document was already open.
	 */
	bool isOpen(const VLNV& vlnv) const;

	/*!
	 *  Update the windows menu to contain the supported windows and visibility of the windows.
	 */
    void updateWindows();

    /*!
     *  Updates the visibility control menu according to the open document.
     *
     *      @param [in] document   The currently open document.
     */
    void updateVisibilityControlMenu(TabDocument* document);

    /*!
     *  Connects all the visibility controls for windows.     
     */
    void connectVisibilityControls();

    /*!
     *  Disconnects all the visibility controls for windows.     
     */
    void disconnectVisibilityControls();

    /*!
     *  Runs the component editor for the given component.
     *
     *      @param [in] component   The component to run the wizard for.
     *      @param [in] directory   The directory containing the component xml file.
     */
    void runComponentWizard(QSharedPointer<Component> component, QString const& directory);
    

	/*!
	 *  Set the visibilities for the plug ins.
	 */
	void setPluginVisibilities();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! The instance that manages the IP-Xact library
    LibraryHandler *libraryHandler_;
    
	//! Contains the open documents as each in it's own tab.
	DrawingBoard* designTabs_;

    QToolBar* leftToolbar_;

    QToolBar* rightToolbar_;

    //! The dock widget handler.
    DockWidgetHandler* dockHandler_;

	/*******************************************************************/
	// the actions in the menus

    Ribbon* ribbon_;


    QStatusBar* statusBar_;

	//! Create a new document in the IP-Xact library
	QAction* actNew_;

    //! Action to save the current design.
    QAction* actSave_;

	//! Action to save the current design as new object
	QAction* actSaveAs_;

    QAction* actSaveHierarchy_;

    //! Action to save all designs.
    QAction* actSaveAll_;

    //! Action to print the current document.
    QAction* actPrint_;

    //! Action to export an image of the current document.
    QAction* actImageExport_;

    //! The Edit group.
    RibbonGroup* editGroup_;

    //! Action for controlling edit group visibility.
    QAction* editAction_ = nullptr;

    //! Action to undo an operation.
    QAction* actUndo_;

    //! Action to redo an operation.
    QAction* actRedo_;

    //! Action to set library locations.
    QAction* actLibraryLocations_;

	//! Search for IP-Xact files in the file system
	QAction* actLibrarySearch_;

	//! Check the library integrity
	QAction* actCheckIntegrity_;

    //! The generation group.
    RibbonGroup* generationGroup_;

    //! Action for controlling generation group visibility.
    QAction* generationAction_ = nullptr;

    //! Action group for plugin generators.
    QActionGroup* pluginActionGroup_;

	//! Action to generate the documentation for a component.
	QAction* actGenDocumentation_;

    //! Action to run import wizard.
    QAction* actRunImport_;

    //! The Diagram Tools group.
    RibbonGroup* diagramToolsGroup_;

    //! Action for controlling diagram tools visibility.
    QAction* diagramToolsAction_ = nullptr;

    //! Action to add a new column to the current diagram.
    QAction* actAddColumn_;

    //! Action to select objects in a diagram.
    QAction* actToolSelect_;

    //! Action to draw connections in a diagram.
    QAction* actToolConnect_;

    //! Action to add interfaces in a diagram.
    QAction* actToolInterface_;

    //! Action to drafting.
    QAction* actToolDraft_;

    //! Action to toggle off-page connection in a diagram.
    QAction* actToolToggleOffPage_;

    //! Action to create labels.
    QAction* actToolLabel_;

    //! Action to zoom in.
    QAction* actZoomIn_;

    //! Action to zoom out.
    QAction* actZoomOut_;

    //! Action to reset the zoom to original 1:1 mapping.
    QAction* actZoomOriginal_;

    //! Action to adjust zoom to fit the document fully in view.
    QAction* actFitInView_;

	//! Action to select which dock widgets are visible.
	QAction* actVisibleDocks_;

    //! Action to manage visibility control.
    QAction* actVisibilityControl_;

    //! Action to open memory designer.
    QAction* openMemoryDesignerAction_;

    //! Action to manage the workspaces.
    QAction* actWorkspaces_;

    //! The protection group.
    RibbonGroup* protectGroup_; 

    //! Action for controlling protection visibility.
    QAction* protectAction_ = nullptr;

    //! Action to refresh the current document.
    QAction* actRefresh_;

    //! Action to set protection on/off.
    QAction* actProtect_;

    //! Action to open Kactus2 settings.
    QAction* actSettings_;

    //! Action to show the about box.
    QAction* actAbout_;

    //! Action to show the help window.
    QAction* actHelp_;

    //! Action to exit the program.
    QAction* actExit_;

    QActionGroup* modeActionGroup_;

    //! The group for configuration tools.
    RibbonGroup* configurationToolsGroup_;

    //! Action for controlling configurationGroup visibility.
    QAction* configurationToolsAction_ = nullptr;

    //! The action to configure the views.
    QAction* actionConfigureViews_;

    //! The group for filtering tools.
    RibbonGroup* filteringGroup_;

    //! Action for controlling filtering visibility.
    QAction* filteringAction_ = nullptr;

    //! The action to filter chained address space memory connections in a memory designer.
    QAction* actionFilterAddressSpaceChains_;

    //! The action to condense memory graphics items in a memory designer.
    QAction* actionCondenseMemoryItems_;

    //! The action to condense field graphics items in a memory designer.
    QAction* actionCondenseFieldItems_;

    //! The action to extend field graphics items in a memory designer.
    QAction* actionExtendFieldItems_;

    //! The action to filter address space segments in a memory designer.
    QAction* actionFilterSegments_;

    //! The action to filter address blocks in a memory designer.
    QAction* actionFilterAddressBlocks_;

    //! The action to filter registers in a memory designer.
    QAction* actionFilterRegisters_;

    //! The action to filter fields in a memory designer.
    QAction* actionFilterFields_;

    //! The action to filter unconnected memory items in a memory design.
    QAction* actionFilterUnconnectedMemoryItems_;

	//! The menu containing the actions to select which windows to display.
	QMenu windowsMenu_;

    //! Menu which contains the actions for visibility control.
    QMenu visibilityMenu_;

    //! Menu which contains the actions for managing workspaces.
    QMenu workspaceMenu_;

    //! Settings for workspaces.
    WorkspaceSettings workspace_;

    MessageMediator* messageChannel_;
};

#endif // MAINWINDOW_H
