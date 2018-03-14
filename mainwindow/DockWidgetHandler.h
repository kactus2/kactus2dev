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

#include <common/utils.h>
#include <common/widgets/tabDocument/TabDocument.h>

class LibraryHandler;
class LibraryWidget;
class ComponentPreviewBox;
class MessageConsole;
class ContextHelpBrowser;
class DesignParameterReferenceCounter;
class ParameterGroupBox;
class DesignParameterReferenceTree;
class ListParameterFinder;
class MultipleParameterFinder;
class ComponentInstanceEditor;
class AdHocVisibilityEditor;
class AdHocEditor;
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
     *      @param [in] library The used IP-XACT library.
     *      @param [in] parent  The main window.
     */
    DockWidgetHandler(LibraryHandler* library, MessageMediator* messageChannel, QMainWindow* parent);

    /*!
     *  The destructor.
     */
    ~DockWidgetHandler();

    /*!
     *  Setup the dock widgets.
     */
    void setupDockWidgets();

    /*!
     *  Get the library handler created by the library widget.
     *
     *      @return The library handler.
     */
    LibraryHandler* getLibraryHandler() const;

    /*!
     *  Load the visibilities for the dockable items.
     *
     *      @param [in] settings    Settings containing the visibilities for the dockable items.
     */
    void loadVisiblities(QSettings& settings);

    /*!
     *  Set filter settings for the library.
     *
     *      @param [in] settings    Settings containing the library filters.
     */
    void setFilterSettings(QSettings& settings);

    /*!
     *  Setup the visibility actions for the visibility menu.
     *
     *      @param [in] visibilityMenu  The visibility menu.
     */
    void setupVisibilityActionMenu(QMenu& visibilityMenu) const;

    /*!
     *  Clear the item selection in design.
     *
     *      @param [in] designWidget    Design widget containing the HW design.
     */
    void clearItemSelection(HWDesignWidget* designWidget);

    /*!
     *  Select a component item in design.
     *
     *      @param [in] currentTabWidget    The currently active tab widget.
     *      @param [in] component           The selected component item.
     */
    void selectComponent(QWidget* currentTabWidget, ComponentItem* component);

    /*!
     *  Select a connection interface.
     *
     *      @param [in] currentTabWidget    The currently active tab widget.
     *      @param [in] interface           The selected interface.
     */
    void selectConnectionInterface(QWidget* currentTabWidget, ConnectionEndpoint* interface);

    /*!
     *  Select a connection item.
     *
     *      @param [in] currentTabWidget    The currently active tab widget.
     *      @param [in] connection          The selected connection item.
     */
    void selectGraphicsConnection(QWidget* currentTabWidget, GraphicsConnection* connection);

    /*!
     *  Update the visibility of the dock widgets.
     *
     *      @param [in] tabCount            Number of items in the tab queue.
     *      @param [in] currentTabWidget    The currently active tab widget.
     */
    void updateWindows(int const& tabCount, QWidget* currentTabWidget);

    /*!
     *  Update the visibility of the selected dock widget.
     *
     *      @param [in] tabCount            Number of items in the tab queue.
     *      @param [in] currentTabWidget    The currently active tab widget.
     *      @param [in] windowType          Type selected dock widget.
     *      @param [in] dock                The selected dock widget.
     */
    void updateWindowAndControlVisibility(int const& tabCount, QWidget* currentTabWidget,
        TabDocument::SupportedWindows windowType, QDockWidget* dock);

    /*!
     *  Handle the change of the currently active tab document.
     *
     *      @param [in] doc     The new active tab document.
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
     *      @param [in] tabCount            Number of items in the tab queue.
     *      @param [in] currentTabWidget    The currently active tab widget.
     *      @param [in] type                Type of the selected dock widget.
     *      @param [in] show                The new visibility value.
     */
    void setWindowVisibilityForSupportedWindow(int const& tabCount, QWidget* currentTabWidget,
        TabDocument::SupportedWindows type, bool show);

    /*!
     *  Change the locking status of the dock widgets.
     *
     *      @param [in] doc     The currently active tab document.
     *      @param [in] locked  The new lock value.
     */
    void changeProtection(TabDocument* doc, bool locked);

    /*!
     *  Get the design parameter reference tree finder.
     *
     *      @return The design parameter reference tree finder.
     */
    QSharedPointer<MultipleParameterFinder> getDesignAndInstanceParameterFinder() const;

    /*!
     *  Get the design parameter finder.
     *
     *      @return The design parameter finder.
     */
    QSharedPointer<ListParameterFinder> getDesignParameterFinder() const;

    /*!
     *  Refresh the design document.
     *
     *      @param [in] currentTabWidget    The currently active tab widget.
     */
    void refreshDesignDocument(QWidget* currentTabWidget);

    /*!
     *  Set filters for the library widget.
     *
     *      @param [in] options     The selected library filters.
     */
    void setLibraryFilters(Utils::FilterOptions options);

    /*!
     *  Get the current library filters.
     *
     *      @return The current library filters.
     */
    Utils::FilterOptions getLibraryFilters() const;

    /*!
     *  Create settings for dock widget visibility and filters.
     *
     *      @param [in] settings    The settings.
     */
    void createVisibilityAndFilterSettings(QSettings& settings) const;

    /*!
     *  Setup the design parameter finder.
     *
     *      @param [in] newDesign   The design of the currently active design tab document.
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
     *      @param [in] message     The error message.
     */
    void errorMessage(QString const& message);

    /*!
     *  Transfers notice messages between the main window and the message console.
     *
     *      @param [in] message     The notice message.
     */
    void noticeMessage(QString const& message);

    /*!
     *  Transfers URL context help requirement between the main window and the message console.
     *
     *      @param [in] message     The URL required for the context help window.
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
     *      @param [in] type    Type of the selected dock widget.
     *      @param [in] show    Value for the display of the dock widget.
     */
    void adjustVisibilityInWindow(TabDocument::SupportedWindows type, bool show);

private slots:

    /*!
     *  Handles the action for the visibility of the output dock widget.
     *
     *      @param [in] show    Value for the visibility of the output dock widget.
     */
    void onOutputAction(bool show);

    /*!
     *  Handles the action for the visibility of the context help dock widget.
     *
     *      @param [in] show    Value for the visibility of the context help dock widget.
     */
    void onContextHelpAction(bool show);

    /*!
     *  Handles the action for the visibility of the component preview dock widget.
     *
     *      @param [in] show    Value for the visibility of the component preview dock widget.
     */
    void onPreviewAction(bool show);

    /*!
     *  Handles the action for the visibility of the library dock widget.
     *
     *      @param [in] show    Value for the visibility of the library dock widget.
     */
    void onLibraryAction(bool show);

    /*!
     *  Handles the action for the visibility of the design configuration dock widget.
     *
     *      @param [in] show    Value for the visibility of the design configuration dock widget.
     */
    void onConfigurationAction(bool show);

    /*!
     *  Handles the action for the visibility of the system details dock widget.
     *
     *      @param [in] show    Value for the visibility of the system details dock widget.
     */
    void onSystemDetailsAction(bool show);

    /*!
     *  Handles the action for the visibility of the connection editor dock widget.
     *
     *      @param [in] show    Value for the visibility of the connection editor dock widget.
     */
    void onConnectionAction(bool show);

    /*!
     *  Handles the action for the visibility of the interface editor dock widget.
     *
     *      @param [in] show    Value for the visibility of the interface editor dock widget.
     */
    void onInterfaceAction(bool show);

    /*!
     *  Handles the action for the visibility of the component instance dock widget.
     *
     *      @param [in] show    Value for the visibility of the component instance dock widget.
     */
    void onInstanceAction(bool show);

    /*!
     *  Handles the action for the visibility of the design parameters editor dock widget.
     *
     *      @param [in] show    Value for the visibility of the design parameters editor dock widget.
     */
    void onDesignParametersAction(bool show);

    /*!
     *  Handles the action for the visibility of the ad hoc port visibility editor dock widget.
     *
     *      @param [in] show    Value for the visibility of the ad hoc port visibility editor dock widget.
     */
    void onAdHocVisibilityAction(bool show);

    /*!
     *  Handles the action for the visibility of the ad hoc port editor dock widget.
     *
     *      @param [in] show    Value for the visibility of the ad hoc port editor dock widget.
     */
    void onAdHocEditorAction(bool show);

private:
    //! No copying. No Assignment.
    DockWidgetHandler(const DockWidgetHandler &other);
    DockWidgetHandler &operator=(const DockWidgetHandler &other);

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
     *  Setup the component preview dock widget.
     */
    void setupComponentPreview();

    /*!
     *  Setup the design parameters editor dock widget.
     */
    void setupDesignParametersEditor();

    /*!
     *  Setup the component instance editor dock widget.
     */
    void setupInstanceEditor();

    /*!
     *  Setup the ad hoc port visibility editor dock widget.
     */
    void setupAdHocVisibilityEditor();

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
     *  Check if the selected window type is supported by the currently active tab widget.
     *
     *      @param [in] tabCount            Number of items in the tab queue.
     *      @param [in] currentTabWidget    The currently active tab widget.
     *      @param [in] windowType          Type of the selected window.
     *
     *      @return True, if the window type is supported by the active document, false otherwise.
     */
    bool isSupportedWindowType(int const& tabCount, QWidget* currentTabWidget,
        TabDocument::SupportedWindows windowType);

    /*!
     *  Get the supported windows for the currently active tab widget.
     *
     *      @param [in] tabCount            Number of items in the tab queue.
     *      @param [in] currentTabWidget    The currently active tab widget.
     *
     *      @return The currently supported dock widgets.
     */
    unsigned int currentlySupportedWindows(int const& tabCount, QWidget* currentTabWidget);

    /*!
     *  Get the dock widgets that are visible regardless of the currently active tad document.
     *
     *      @return The default dock widgets.
     */
    unsigned int defaultWindows();

    /*!
     *  Set the visibility for the selected dock widget.
     *
     *      @param [in] windowType  The type of the selected dock widget.
     *      @param [in] show        The new visibility value.
     */
    void setWindowVisibility(TabDocument::SupportedWindows windowType, bool show);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The instance that manages the IP-XACT library
    LibraryHandler *libraryHandler_;

    //! The dock widget that contains the library widgets.
    QDockWidget* libraryDock_;

    //! The widget containing library navigation.
    LibraryWidget* libraryWidget_;

    //! The widget to display a preview of component.
    ComponentPreviewBox* previewBox_;

    //! The dock widget that contains the preview box.
    QDockWidget* previewDock_;

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

    //! The ad-hoc visibility editor.
    AdHocVisibilityEditor* adHocVisibilityEditor_;

    //! Ad hoc dock widget.
    QDockWidget* adHocVisibilityDock_;

    //! Ad hoc editor.
    AdHocEditor* adhocEditor_;

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

    //! The help window.
    HelpWindow* helpWnd_;

    //! Contains the visibility for the windows.
    //! Used to maintain the visibility information when windows are hidden by change of the active document.
    QMap<TabDocument::SupportedWindows, bool> visibilities_;

    //! The main window that should contain all of the dock widgets constructed in here.
    QMainWindow* mainWindow_;

    MessageMediator* messageChannel_;
};

#endif // DOCKWIDGETHANDLER_H
