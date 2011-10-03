/* 
 *
 *  Created on: 24.2.2011
 * 		filename: mainwindow.h
 */


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "NewSWComponentPage.h"

#include <common/DrawMode.h>
#include <common/KactusAttribute.h>

#include <LibraryManager/vlnv.h>
#include <LibraryManager/VLNVDialer/vlnvdialer.h>
#include <GCF/MenuStrip>

#include <QMainWindow>
#include <QActionGroup>

class DesignWidget;
class LibraryHandler;
class DiagramComponent;
class DiagramPort;
class ProgramEntityItem;
class ComponentPreviewBox;
class MessageConsole;
class DiagramInterface;
class ComponentInstanceEditor;
class ConfigurationEditor;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    
	//! \brief The destructor
	virtual ~MainWindow();

public slots:
    /*!
     *  Opens the "New" dialog.
     */
    void createNew();

    /*!
     *  Opens the source file of the given node.
     *
     *      @param [in] node The node.
     */
    void openSource(ProgramEntityItem* node);
    
    /*!
     *  Opens a HW design.
     *
     *      @param [in] vlnv           The VLNV of the top-level component.
     *      @param [in] viewName       The name of the view to open.
     *      @param [in] forceUnlocked  Forces the design to be opened in unlocked mode.
     */
	void openDesign(const VLNV& vlnv = VLNV(), const QString& viewName = QString(),
                    bool forceUnlocked = false);

    /*!
     *  Opens a system design.
     *
     *      @param [in] vlnv           The VLNV of the system component whose system design to open.
     *      @param [in] forceUnlocked  Forces the system to be opened in unlocked mode.
     */
    void openSystem(VLNV const &vlnv, bool forceUnlocked = false);

	/*! \brief Open bus to be edited
	 *
	 *      @param [in] vlnv           Identifies the bus definition
     *      @param [in] forceUnlocked  Forces the bus to be opened in unlocked mode.
	 */
	void openBus(const VLNV& busDefVLNV, const VLNV& absDefVLNV = VLNV(),
                 bool disableBusDef = false, bool forceUnlocked = false);

	/*! \brief Open component editor to edit a component details.
	 *
	 *      @param [in] vlnv           Identifies the component to edit.
     *      @param [in] forceUnlocked  Forces the component to be opened in unlocked mode.
	 */
	void openComponent(const VLNV& vlnv, bool forceUnlocked = false);

    /*! \brief Called when user wants to close a design tab
	 *
	 * \param index Index of the tab to close
	 *
	*/
	void onTabCloseRequested(int index);

    //! Called when the user has selected another tab.
    void onTabChanged(int index);

	/*! \brief Search the file system for new IP-Xact libraries.
	 *
	 * This slot is called when user clicks to search for IP-Xact files.
	*/
	void onLibrarySearch();

	/*! \brief Called when user selects a component on the draw board.
	 *
	 * \param component Pointer to the DiagramComponent instance that is selected.
	 *
	*/
	void onComponentSelected(DiagramComponent* component);

	/*! \brief Called when user selects a port on the draw board.
	 *
	 * \param port Pointer to the DiagramPort instance that is selected.
	 *
	*/
	void onPortSelected(DiagramPort* port);

	/*! \brief Called when user selects an interface on the draw board.
	 *
	 * \param interface Pointer to the DiagramInterface instance that is selected.
	 *
	*/
	void onInterfaceSelected(DiagramInterface* interface);

    //! \brief Import a library file to this library.
	void onImportLibFile();

	//! \brief Export the library file to another location.
	void onExportLibFile();

    //! Called when the menu strip needs to be updated.
    void updateMenuStrip();

    //! Called when the zoom has changed.
    void updateZoomTools();

    //! Saves the current tab document.
    void saveCurrent();

	//! \brief Saves the current tab document as new object
	void saveCurrentAs();

    //! Saves all modified tab documents.
    void saveAll();

    //! Prints the current tab document.
    void printCurrent();

    //! Adds a new column to the current HW/System design.
    void addColumn();

    //! Generates the VHDL from the current design.
    void generateVHDL();

    //! Generates the ModelSim makefile from the current design.
    void generateModelSim();

    //! Generates the Quartus project from the current design.
    void generateQuartus();

	//! \brief Generates the documentation for the current component/design.
	void generateDoc();

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
     *      @param [in] vlnv        The component's VLNV.
     *      @param [in] directory   The directory where to save the component.
     *
     *      @remarks The component editor is opened automatically after successful creation.
     */
    void createComponent(KactusAttribute::ProductHierarchy prodHier,
                         KactusAttribute::Firmness firmness,
                         VLNV const& vlnv, QString const& directory);

    /*!
     *  Creates a SW component to the library.
     *
     *      @param [in] createType  The type of component to create.
     *      @param [in] vlnv        The component's VLNV.
     *      @param [in] directory   The directory where to save the component.
     *
     *      @remarks Based on the create type, either the platform component editor,
     *               platform stack designer or the application designer is opened automatically
     *               after successful creation.
     */
    void createSWComponent(SWCreateType createType, VLNV const& vlnv,
                           QString const& directory);

    /*!
     *  Creates a new design.
     *
     *      @param [in] prodHier    The product hierarchy attribute.
     *      @param [in] firmness	The firmness attribute.
     *      @param [in] vlnv        The component's VLNV.
     *      @param [in] directory   The directory where to save the component.
     *
     *      @remarks The design widget is opened automatically after successful creation.
     */
    void createDesign(KactusAttribute::ProductHierarchy prodHier,
                      KactusAttribute::Firmness firmness,
                      VLNV const& vlnv, QString const& directory);

    /*!
     *  Creates a new system design.
     *
     *      @param [in] compVLNV   The VLNV of an existing component.
     *      @param [in] sysVLNV    The VLNV of the system to be created.
     *      @param [in] directory  The directory where to save the system.
     */
    void createSystem(VLNV const& compVLNV, VLNV const& sysVLNV, QString const& directory);

    /*! \brief Creates a new bus definition and abstraction definition
	 *
	 * \param vlnv The vlnv for the bus definition
	 * \param directory The directory where to save the bus definition and abstraction
	 * definition.
	 *
	*/
	void createBus(VLNV const& vlnv, QString const& directory);

	/*! \brief Creates a new abstraction definition for given bus definition.
	 *
	 * \param busDefVLNV Identifies the bus definition.
	 * \param directory The directory where to save the abstraction definition.
	 * \param disableBusDef If true then the bus definition editor is disabled when editor
	 * is opened.
	 *
	*/
	void createAbsDef(const VLNV& busDefVLNV, const QString& directory, bool disableBusDef);

    /*!
     *  Opens the settings dialog.
     */
    void openSettings();

    /*!
     *  Shows the about box.
     */
    void showAbout();

signals:

	//! \brief Print a notice message to the user
	void noticeMessage(const QString& msg);

	//! \brief Print an error message to the user
	void errorMessage(const QString& msg);

protected:
    // Called when the user requests to close the program.
    void closeEvent(QCloseEvent* event);

private slots:

	//! \brief Handler for design widget's clearItemSelection signal.
	void onClearItemSelection();

	//! \brief Handler for signals that inform that design has changed.
	void onDesignChanged();

private:

	//! \brief No copying
	MainWindow(const MainWindow& other);

	//! No assignment
	MainWindow& operator=(const MainWindow& other);

	/*! \brief Restore the mainwindow's settings.
	 *
	*/
	void restoreSettings();

	/*! \brief Save the mainwindow's settings
	 *
	*/
	void saveSettings();

	/*! \brief Set up the actions in the tool bars
	 *
	*/
	void setupActions();

	/*! \brief Set up the menus for the main window
	 *
	 * This function is used by the setupActions() after it has created the 
	 * actions for the menus and should not be called directly.
	*/
	void setupMenus();

	/*! \brief Set up the drawing view where hierarchical components are edited.
	 *
	*/
	void setupDrawBoard();

	/*! \brief Set up the dock for the LibraryHandler
	 *
	*/
	void setupLibraryDock();

	//! \brief Set up the message console.
	void setupMessageConsole();

	//! \brief Set up the component instance editor.
	void setupInstanceEditor();

	//! \brief Set up the configuration editor.
	void setupConfigurationEditor();

	//! \brief The instance that manages the IP-Xact library
    LibraryHandler *libraryHandler_;
    
	//! \brief Contains the designWidgets as each in it's own tab
	QTabWidget *designTabs_;

	//! \brief The widget to set the display rules for library items in library handler.
	VLNVDialer* dialer_;

	//! \brief The widget to display a preview of component.
	ComponentPreviewBox* previewBox_;

	//! \brief The widget to print errors and notifications to user.
	MessageConsole* console_;

	//! \brief Pointer to the dock widget used to display the details of a port.
	QDockWidget* portSummaryDock_;

	//! \brief Pointer to the dock widget used to display the details of an interface.
	QDockWidget* interfaceSummaryDock_;

	//! \brief The widget to edit the settings of a component instance.
	ComponentInstanceEditor* instanceEditor_;

	//! \brief The widget to edit the configuration of designs.
	ConfigurationEditor* configurationEditor_;

	/*******************************************************************/
	// the actions in the menus

    GCF::MenuStrip* menuStrip_;

	//! \brief Create a new hierarchical component in the IP-Xact library
	QAction *actNew_;

    //! Action to save the current design.
    QAction* actSave_;

	//! \brief Action to save the current design as new object
	QAction* actSaveAs_;

    //! Action to save all designs.
    QAction* actSaveAll_;

    //! Action to print the current document.
    QAction* actPrint_;

    //! The Edit group.
    GCF::MenuStripGroup* editGroup_;

    //! Action to undo an operation.
    QAction* actUndo_;

    //! Action to redo an operation.
    QAction* actRedo_;

	//! \brief Search for IP-Xact files in the file system
	QAction* actLibrarySearch_;

	//! \brief Import a library file 
	QAction* actImportLibFile_;

	//! \brief Export a library file
	QAction* actExportLibFile_;

	//! \brief Check the library integrity
	QAction* actCheckIntegrity_;

    //! The HW Design group.
    GCF::MenuStripGroup* hwDesignGroup_;

    //! Action to generate VHDL from a design.
    QAction* actGenVHDL_;

    //! Action to generate a ModelSim makefile from a design.
    QAction* actGenModelSim_;

    //! Action to generate a Quartus project from a design.
    QAction* actGenQuartus_;

	//! \brief Action to generate the documentation for a component.
	QAction* actGenDocumentation_;

    //! The SW Design group.
    GCF::MenuStripGroup* swDesignGroup_;

    //! The Diagram Tools group.
    GCF::MenuStripGroup* diagramToolsGroup_;
    //! Action to add a new column to the current diagram.
    QAction* actAddColumn_;

    //! Action to select objects in a diagram.
    QAction* actToolSelect_;

    //! Action to draw connections in a diagram.
    QAction* actToolConnect_;

    //! Action to add interfaces in a diagram.
    QAction* actToolInterface_;
    QAction* actToolDraft_;

    //! Action to zoom in.
    QAction* actZoomIn_;

    //! Action to zoom out.
    QAction* actZoomOut_;

    //! Action to reset the zoom to original 1:1 mapping.
    QAction* actZoomOriginal_;

    //! Action to adjust zoom to fit the document fully in view.
    QAction* actFitInView_;

    //! Action to set protection on/off.
    QAction* actProtect_;

    //! Action to open Kactus2 settings.
    QAction* actSettings_;

    //! Action to show the about box.
    QAction* actAbout_;

    //! Action to exit the program.
    QAction* actExit_;

    QActionGroup* modeActionGroup_;
};

#endif // MAINWINDOW_H
