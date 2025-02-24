//-----------------------------------------------------------------------------
// File: DockWidgetHandler.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 10.10.2017
//
// Description:
// Manages the dockable widgets for the main window.
//-----------------------------------------------------------------------------

#ifndef DOCKWIDGETHANDLER_H
#define DOCKWIDGETHANDLER_H

#include <KactusAPI/include/utils.h>
#include <common/widgets/tabDocument/TabDocument.h>
#include <editors/PythonSourceEditor/PythonSourceEditor.h>

class LibraryHandler;
class LibraryWidget;
class MessageConsole;
class ContextHelpBrowser;
class DesignParameterReferenceCounter;
class ParameterGroupBox;
class DesignParameterReferenceTree;
class ListParameterFinder;
class MultipleParameterFinder;
class ComponentInstanceEditor;
class AdhocEditor;
class ConfigurationEditor;
class SystemDetailsEditor;
class InterfaceEditor;
class ConnectionEditor;
class HelpWindow;
class HWDesignWidget;
class ComponentItem;
class ConnectionEndpoint;
class GraphicsConnection;
class Design;
class MessageMediator;
class VendorExtensionsEditor;
class ScriptingConsole;

#include <QSharedPointer>
#include <QDockWidget>
#include <QObject>
#include <QMainWindow>
#include <QSettings>

//-----------------------------------------------------------------------------
//! Manages the dockable widgets for the main window.
//-----------------------------------------------------------------------------
class DockWidgetHandler: public QObject
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *    @param [in] library         The used IP-XACT library.
     *    @param [in] messageChannel  The message channel for notifications and errors.
     *    @param [in] leftToolbar     The left side toolbar in mainwindow.
     *    @param [in] rightToolbar    The right side toolbar in mainwindow.
     *    @param [in] parent          The main window.
     */
    DockWidgetHandler(LibraryHandler* library, MessageMediator* messageChannel, 
        QToolBar* leftToolbar, QToolBar* rightToolbar, QMainWindow* parent);

    /*!
     *  The destructor.
     */
    virtual ~DockWidgetHandler() = default;

    //! No copying. No Assignment.
    DockWidgetHandler(const DockWidgetHandler& other) = delete;
    DockWidgetHandler& operator=(const DockWidgetHandler& other) = delete;

    /*!
     *  Setup the dock widgets.
     */
    void setupDockWidgets();

    /*!
     *  Setup the visibility actions for the visibility menu.
     *
     *    @param [in] visibilityMenu  The visibility menu.
     */
    void setupVisibilityActionMenu(QMenu& visibilityMenu);

    /*!
     *  Get the library handler created by the library widget.
     *
     *    @return The library handler.
     */
    LibraryHandler* getLibraryHandler() const;

    /*!
     *  Load the visibilities for the dockable items.
     *
     *    @param [in] settings    Settings containing the visibilities for the dockable items.
     */
    void loadVisiblities(QSettings const& settings);

    /*!
     *  Save settings for dock widget visibilities.
     *
     *    @param [in] settings    The settings.
     */
    void saveVisibilitySettings(QSettings& settings) const;

    /*!
     *  Set filter settings for the library.
     *
     *    @param [in] settings    Settings containing the library filters.
     */
    void loadFilterSettings(QSettings& settings);

    /*!
     *  Save settings for dock widget filters.
     *
     *    @param [in] settings    The settings.
     */
    void saveFilterSettings(QSettings& settings) const;

    //! Apply the current application settings to widgets.
    void applySettings();

    /*!
     *  Clear the item selection in design.
     *
     *    @param [in] designWidget    Design widget containing the HW design.
     */
    void clearItemSelection(HWDesignWidget* designWidget);

    /*!
     *  Select a component item in design.
     *
     *    @param [in] currentTabWidget    The currently active tab widget.
     *    @param [in] component           The selected component item.
     */
    void selectComponent(QWidget* currentTabWidget, ComponentItem* component);

    /*!
     *  Select a connection interface.
     *
     *    @param [in] currentTabWidget    The currently active tab widget.
     *    @param [in] interface           The selected interface.
     */
    void selectConnectionInterface(QWidget* currentTabWidget, ConnectionEndpoint* interface);

    /*!
     *  Select a connection item.
     *
     *    @param [in] currentTabWidget    The currently active tab widget.
     *    @param [in] connection          The selected connection item.
     */
    void selectGraphicsConnection(QWidget* currentTabWidget, GraphicsConnection* connection);

    /*!
     *  Update the visibility of the dock widgets.
     *
     *    @param [in] tabCount            Number of items in the tab queue.
     *    @param [in] currentTabWidget    The currently active tab widget.
     */
    void updateWindows(QWidget* currentTabWidget);

    /*!
     *  Update the visibility of the selected dock widget.
     *
     *    @param [in] currentTabWidget    The currently active tab widget.
     *    @param [in] windowType          Type selected dock widget.
     *    @param [in] dock                The selected dock widget.
     */
    void updateWindowAndControlVisibility(QWidget* currentTabWidget,
        TabDocument::SupportedWindows windowType, QDockWidget* dock);

    /*!
     *  Handle the change of the currently active tab document.
     *
     *    @param [in] doc     The new active tab document.
     */
    void documentChanged(TabDocument* doc);

    /*!
     *  Connect the dock widget visibility controls.
     */
    void connectVisibilityControls();

    /*!
     *  Disconnect the dock widget visibility controls.
     */
    void disconnectVisibilityControls();

    /*!
     *  Set visibility for the supported window types.
     *
     *    @param [in] currentTabWidget    The currently active tab widget.
     *    @param [in] type                Type of the selected dock widget.
     *    @param [in] show                The new visibility value.
     */
    void setWindowVisibilityForSupportedWindow(QWidget* currentTabWidget,
        TabDocument::SupportedWindows type, bool show);

    /*!
     *  Change the locking status of the dock widgets.
     *
     *    @param [in] doc     The currently active tab document.
     *    @param [in] locked  The new lock value.
     */
    void changeProtection(TabDocument* doc, bool locked);

    /*!
     *  Get the design parameter reference tree finder.
     *
     *    @return The design parameter reference tree finder.
     */
    QSharedPointer<MultipleParameterFinder> getDesignAndInstanceParameterFinder() const;

    /*!
     *  Get the design parameter finder.
     *
     *    @return The design parameter finder.
     */
    QSharedPointer<ListParameterFinder> getDesignParameterFinder() const;

    /*!
     *  Refresh the design document.
     *
     *    @param [in] currentTabWidget    The currently active tab widget.
     */
    void refreshDesignDocument(QWidget* currentTabWidget);

    /*!
     *  Setup the design parameter finder.
     *
     *    @param [in] newDesign   The design of the currently active design tab document.
     */
    void setupDesignParameterFinder(QSharedPointer<Design> newDesign);

public slots:

    /*!
     *  Handles the display of the help window.
     */
    void showHelp();

signals:

    /*!
     *  Transfers error messages between the main window and the message console.
     *
     *    @param [in] message     The error message.
     */
    void errorMessage(QString const& message);

    /*!
     *  Transfers notice messages between the main window and the message console.
     *
     *    @param [in] message     The notice message.
     */
    void noticeMessage(QString const& message);

    void statusMessage(QString const& message);

    /*!
     *  Transfers URL context help requirement between the main window and the message console.
     *
     *    @param [in] message     The URL required for the context help window.
     */
    void helpUrlRequested(QString const& helpURL);

    /*!
     *  Transfers design change information from the main window to the dock widgets.
     */
    void designChanged();

    /*!
     *  Transfers the visibility change in the selected dock widget type to main window in order to receive
     *  required tab document data.
     *
     *    @param [in] type    Type of the selected dock widget.
     *    @param [in] show    Value for the display of the dock widget.
     */
    void adjustVisibilityInWindow(TabDocument::SupportedWindows type, bool show);

    void generateIntegrityReport();

private slots:

    /*!
     *  Handles the action for the visibility of the output dock widget.
     *
     *    @param [in] show    Value for the visibility of the output dock widget.
     */
    void onOutputAction(bool show);

    /*!
     *  Handles the action for the visibility of the context help dock widget.
     *
     *    @param [in] show    Value for the visibility of the context help dock widget.
     */
    void onContextHelpAction(bool show);

    /*!
     *  Handles the action for the visibility of the library dock widget.
     *
     *    @param [in] show    Value for the visibility of the library dock widget.
     */
    void onLibraryAction(bool show);

    /*!
     *  Handles the action for the visibility of the design configuration dock widget.
     *
     *    @param [in] show    Value for the visibility of the design configuration dock widget.
     */
    void onConfigurationAction(bool show);

    /*!
     *  Handles the action for the visibility of the system details dock widget.
     *
     *    @param [in] show    Value for the visibility of the system details dock widget.
     */
    void onSystemDetailsAction(bool show);

    /*!
     *  Handles the action for the visibility of the connection editor dock widget.
     *
     *    @param [in] show    Value for the visibility of the connection editor dock widget.
     */
    void onConnectionAction(bool show);

    /*!
    *  Handles the action for the visibility of the vendor extension editor dock widget.
    *
    *    @param [in] show    Value for the visibility of the vendor extension editor dock widget.
    */
    void onVendorExtensionVisibilityAction(bool show);

    /*!
     *  Handles the action for the visibility of the interface editor dock widget.
     *
     *    @param [in] show    Value for the visibility of the interface editor dock widget.
     */
    void onInterfaceAction(bool show);

    /*!
     *  Handles the action for the visibility of the component instance dock widget.
     *
     *    @param [in] show    Value for the visibility of the component instance dock widget.
     */
    void onInstanceAction(bool show);

    /*!
     *  Handles the action for the visibility of the design parameters editor dock widget.
     *
     *    @param [in] show    Value for the visibility of the design parameters editor dock widget.
     */
    void onDesignParametersAction(bool show);

    /*!
     *  Handles the action for the visibility of the ad hoc port editor dock widget.
     *
     *    @param [in] show    Value for the visibility of the ad hoc port editor dock widget.
     */
    void onAdHocEditorAction(bool show);

    /*!
     *  Handles the change for the dock position.
     *
     *    @param [in] area    The area where the dock has been positioned.
     */
    void onDockLocationChanged(Qt::DockWidgetArea area);


private:

    /*!
     *  Setup the message console and the containing output dock widget.
     */
    void setupMessageConsole();

    /*!
     *  Setup the context help dock widget.
     */
    void setupContextHelp();

    /*!
     *  Setup the library dock widget.
     */
    void setupLibraryDock();

    /*!
     *  Setup the design parameters editor dock widget.
     */
    void setupDesignParametersEditor();

    /*!
     *  Setup the component instance editor dock widget.
     */
    void setupInstanceEditor();

    /*!
     *  Setup the ad hoc port editor dock widget.
     */
    void setupAdHocEditor();
    
    /*!
     *  Setup the design configuration editor dock widget.
     */
    void setupConfigurationEditor();
    
    /*!
     *  Setup the system details editor dock widget.
     */
    void setupSystemDetailsEditor();
    
    /*!
     *  Setup the interface editor dock widget.
     */
    void setupInterfaceEditor();
    
    /*!
     *  Setup the connection editor dock widget.
     */
    void setupConnectionEditor();

    /*!
    *  Setup the vendor extension editor dock widget.
    */
    void setupVendorExtensionEditor();

    /*!
     *  Check if the selected window type is supported by the currently active tab widget.
     *
     *    @param [in] tabCount            Number of items in the tab queue.
     *    @param [in] currentTabWidget    The currently active tab widget.
     *    @param [in] windowType          Type of the selected window.
     *
     *    @return True, if the window type is supported by the active document, false otherwise.
     */
    bool isSupportedWindowType(QWidget* currentTabWidget,
        TabDocument::SupportedWindows windowType) const;

    /*!
     *  Get the supported windows for the currently active tab widget.
     *
     *    @param [in] currentTabWidget    The currently active tab widget.
     *
     *    @return The currently supported dock widgets.
     */
    unsigned int currentlySupportedWindows(QWidget* currentTabWidget) const;

    /*!
     *  Get the dock widgets that are visible regardless of the currently active tad document.
     *
     *    @return The default dock widgets.
     */
    unsigned int defaultWindows() const;

    /*!
     *  Set the visibility for the selected dock widget.
     *
     *    @param [in] windowType  The type of the selected dock widget.
     *    @param [in] show        The new visibility value.
     */
    void setWindowVisibility(TabDocument::SupportedWindows windowType, bool show);


    /*!
     * Places the visibility control action in the correct toolbar.
     *
     *    @param [in] action   The action to place in the toolbar.
     *    @param [in] area     The area where the related dock widget is located.
     *
     *    @return 
     */
     void placeActionInToolbar(QAction* action, Qt::DockWidgetArea area);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The instance that manages the IP-XACT library
    LibraryHandler *libraryHandler_;

    //! The dock widget that contains the library widgets.
    QDockWidget* libraryDock_;

    //! The widget containing library navigation.
    LibraryWidget* libraryWidget_;

    //! The widget to print errors and notifications to user.
    MessageConsole* console_;

    //! The dock widget that contains the console.
    QDockWidget* consoleDock_;

    //! Context help browser.
    ContextHelpBrowser* contextHelpBrowser_;

    //! The dock widget for the context help browser.
    QDockWidget* contextHelpDock_;

    //! Reference counter for design parameters.
    QSharedPointer<DesignParameterReferenceCounter> designParameterReferenceCounter_;

    //! Editor for design parameters.
    ParameterGroupBox* designParametersEditor_;

    //! Dock widget for the design parameters editor.
    QDockWidget* designParameterDock_;

    //! Reference tree containing the design parameter references.
    DesignParameterReferenceTree* designParameterTree_;

    //! Parameter finder for design parameters.
    QSharedPointer<ListParameterFinder> designParameterFinder_;

    //! Parameter finder for design parameter reference tree.
    QSharedPointer<MultipleParameterFinder> designAndInstancesParameterFinder_;

    //! The widget to edit the settings of a component instance.
    ComponentInstanceEditor* instanceEditor_;

    //! The dock widget that contains the instance editor.
    QDockWidget* instanceDock_;

    //! Ad hoc editor.
    AdhocEditor* adhocEditor_;

    //! The ad hoc dock widget.
    QDockWidget* adhocDock_;

    //! The widget to edit the configuration of designs.
    ConfigurationEditor* configurationEditor_;

    //! The dock widget that contains the configuration editor.
    QDockWidget* configurationDock_;

    //! The widget to edit system details.
    SystemDetailsEditor* systemDetailsEditor_;

    //! The dock widget for the system details editor.
    QDockWidget* systemDetailsDock_;

    //! The widget to edit the interfaces
    InterfaceEditor* interfaceEditor_;

    //! The dock widget that contains the interface editor.
    QDockWidget* interfaceDock_;

    //! The widget to edit the connection
    ConnectionEditor* connectionEditor_;

    //! The dock widget that contains the connection editor.
    QDockWidget* connectionDock_;


    //! The dock widget that contains the vendor extension editor.
    QDockWidget* extensionDock_;

    //! The editor for vendor extensions.
    VendorExtensionsEditor* extensionEditor_;

    //! The help window.
    HelpWindow* helpWnd_;

    //! All the dock widgets.
    QMap<TabDocument::SupportedWindows, QDockWidget*> docks_;

    //! Visibility control actions for the dock widgets.
    QMap<TabDocument::SupportedWindows, QAction*> visibilityControls_;

    //! Contains the visibility for the windows.
    //! Used to maintain the visibility information when windows are hidden by change of the active document.
    QMap<TabDocument::SupportedWindows, bool> visibilities_;

    //! The left side toolbar.
    QToolBar* leftToolbar_;

    //! The right side toolbar.
    QToolBar* rightToolbar_;

    //! Action group for left side toolbar.
    QActionGroup* leftActions_;

    //! Action group for right side toolbar.
    QActionGroup* rightActions_;

    //! The main window that should contain all of the dock widgets constructed in here.
    QMainWindow* mainWindow_;

    MessageMediator* messageChannel_;
    
};

#endif // DOCKWIDGETHANDLER_H
