//-----------------------------------------------------------------------------
// File: mainwindow.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 24.2.2011
//
// Description:
// The main window of Kactus2.
//-----------------------------------------------------------------------------

#include "mainwindow.h"

#include "SplashScreen.h"

#include "NewWorkspaceDialog.h"
#include "DeleteWorkspaceDialog.h"

#include <mainwindow/Ribbon/Ribbon.h>
#include <mainwindow/Ribbon/RibbonGroup.h>
#include <mainwindow/MessageConsole/messageconsole.h>
#include <mainwindow/NewPages/NewComponentPage.h>
#include <mainwindow/NewPages/NewDesignPage.h>
#include <mainwindow/NewPages/NewSWComponentPage.h>
#include <mainwindow/NewPages/NewSWDesignPage.h>
#include <mainwindow/NewPages/NewSystemPage.h>
#include <mainwindow/NewPages/NewComDefinitionPage.h>
#include <mainwindow/NewPages/NewApiDefinitionPage.h>
#include <mainwindow/NewPages/newbuspage.h>
#include <mainwindow/EditorArea/EditorArea.h>

#include <common/KactusAttribute.h>
#include <common/dialogs/LibrarySettingsDialog/LibrarySettingsDialog.h>
#include <common/dialogs/NewDesignDialog/NewDesignDialog.h>
#include <common/dialogs/newObjectDialog/newobjectdialog.h>
#include <common/dialogs/listSelectDialog/ListSelectDialog.h>
#include <common/widgets/componentPreviewBox/ComponentPreviewBox.h>
#include <common/dialogs/propertyPageDialog/PropertyPageDialog.h>

#include <designEditors/MemoryDesigner/MemoryDesignWidget.h>

#include <designEditors/common/ComponentInstanceEditor/componentinstanceeditor.h>
#include <designEditors/common/ConfigurationEditor/configurationeditor.h>
#include <designEditors/common/InterfaceEditor/interfaceeditor.h>
#include <designEditors/common/ConnectionEditor/connectioneditor.h>
#include <designEditors/HWDesign/HWDesignWidget.h>
#include <designEditors/HWDesign/HWDesignDiagram.h>
#include <designEditors/HWDesign/HWComponentItem.h>
#include <designEditors/HWDesign/BusPortItem.h>
#include <designEditors/HWDesign/BusInterfaceItem.h>
#include <designEditors/HWDesign/AddressEditor/AddressEditor.h>
#include <designEditors/HWDesign/AdHocEditor/AdHocEditor.h>
#include <designEditors/SystemDesign/SystemDetailsEditor/SystemDetailsEditor.h>
#include <designEditors/SystemDesign/SystemDesignWidget.h>
#include <designEditors/SystemDesign/SystemDesignDiagram.h>

#include <editors/ApiDefinitionEditor/ApiDefinitionEditor.h>
#include <editors/BusEditor/buseditor.h>
#include <editors/ComDefinitionEditor/ComDefinitionEditor.h>
#include <editors/ComponentEditor/componenteditor.h>
#include <editors/CSourceEditor/CSourceWidget.h>
#include <editors/CSourceEditor/CSourceContentMatcher.h>
#include <editors/NotesEditor/NotesEditor.h>

#include <Help/HelpSystem/ContextHelpBrowser.h>
#include <Help/HelpSystem/HelpWindow.h>

#include <kactusGenerators/DocumentGenerator/documentgenerator.h>

#include <library/LibraryManager/libraryhandler.h>
#include <library/LibraryManager/vlnv.h>
#include <library/LibraryManager/LibraryUtils.h>

#include <IPXACTmodels/view.h>
#include <IPXACTmodels/component.h>
#include <IPXACTmodels/model.h>
#include <IPXACTmodels/design.h>
#include <IPXACTmodels/designconfiguration.h>
#include <IPXACTmodels/abstractiondefinition.h>
#include <IPXACTmodels/busdefinition.h>
#include <IPXACTmodels/fileset.h>
#include <IPXACTmodels/file.h>
#include <IPXACTmodels/ComDefinition.h>
#include <IPXACTmodels/ApiDefinition.h>
#include <IPXACTmodels/ApiInterface.h>
#include <IPXACTmodels/SWView.h>
#include <IPXACTmodels/SystemView.h>
#include <IPXACTmodels/SWInstance.h>

#include <Plugins/PluginSystem/IGeneratorPlugin.h>
#include <Plugins/PluginSystem/IPluginUtility.h>
#include <Plugins/PluginSystem/PluginListDialog.h>
#include <Plugins/PluginSystem/PluginUtilityAdapter.h>


#include <settings/SettingsDialog.h>
#include <settings/SettingsUpdater.h>

#include <wizards/ComponentWizard/ComponentWizard.h>

#include <QCoreApplication>
#include <QApplication>
#include <QSettings>
#include <QStringList>
#include <QToolBar>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QDockWidget>
#include <QMessageBox>
#include <QDebug>
#include <QCoreApplication>
#include <QFileInfo>
#include <QFile>
#include <QTextStream>
#include <QUrl>
#include <QDesktopServices>
#include <QCursor>
#include <QDateTime>
#include <QElapsedTimer>
#include <QDesktopServices>
#include <QDebug>
#include <QPainter>
#include <QProcess>

class LibraryItem;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      libraryHandler_(0),
      libraryDock_(0),
      designTabs_(0),  
      dialer_(0),
      previewBox_(0),
      previewDock_(0),
      console_(0),
      consoleDock_(0),
      contextHelpBrowser_(0),
      contextHelpDock_(0),
      instanceEditor_(0),
      instanceDock_(0),
      adHocEditor_(0),
      adHocDock_(0),
      addressEditor_(0),
      addressDock_(0),
      configurationEditor_(0),
      configurationDock_(0),
      systemDetailsEditor_(0),
      systemDetailsDock_(0),
      interfaceEditor_(0),
      interfaceDock_(0),
      connectionEditor_(0),
      connectionDock_(0),
      notesDock_(0),
      notesEditor_(0),
      ribbon_(0),
      actNew_(0),
      actSave_(0),
      actSaveAs_(0),
      actPrint_(0), 
      editGroup_(0),
      actUndo_(0), 
      actRedo_(0),
      actLibraryLocations_(0),
      actLibrarySearch_(0), 
      actCheckIntegrity_(0),
      generationGroup_(0),
      pluginActionGroup_(0),
      actGenVHDL_(0),
      actGenModelSim_(0),
      actGenQuartus_(0), 
      actGenDocumentation_(0),
      diagramToolsGroup_(0), 
      actAddColumn_(0),
      actToolSelect_(0), 
      actToolConnect_(0),
      actToolInterface_(0),
      actToolDraft_(0),
      actToolToggleOffPage_(0),
      actToolLabel_(0),
      actZoomIn_(0),
      actZoomOut_(0), 
      actZoomOriginal_(0), 
      actFitInView_(0),
      actVisibleDocks_(0),
      actVisibilityControl_(0),
      actWorkspaces_(0),
      protectGroup_(0),
      actRefresh_(0),
      actProtect_(0), 
      actSettings_(0),
      actAbout_(0), 
      actHelp_(0),
      actExit_(0),
      windowsMenu_(this),
      visibilityMenu_(this),
      workspaceMenu_(this),
      curWorkspaceName_("Default"),
      pluginMgr_(),
      helpWnd_(0),
      visibilities_()
{
    qRegisterMetaTypeStreamOperators<HighlightStyleDesc>("HighlightStyleDesc");

    // set the identification tags for the application
	QCoreApplication::setOrganizationDomain(tr("tut.fi"));
	QCoreApplication::setOrganizationName(tr("TUT"));
	QCoreApplication::setApplicationName(tr("Kactus2"));
	QCoreApplication::setApplicationVersion("2.0");
    QSettings::setDefaultFormat(QSettings::IniFormat);

    SettingsUpdater::runUpgrade(this);

	setWindowTitle(QCoreApplication::applicationName());
	setWindowIcon(QIcon(":icons/common/graphics/appicon.png"));

	// By default, the window is 1024x768 and set to maximized state.
	resize(1024, 768);
	setWindowState(Qt::WindowMaximized);

    // Setup windows.
    setupMessageConsole();
    setupContextHelp();
    setupNotesEditor();
	setupDrawBoard();
    setupLibraryDock();
    setupInstanceEditor();
    setupAdHocVisibilityEditor();
    setupAddressEditor();
	setupConfigurationEditor();
    setupSystemDetailsEditor();
	setupInterfaceEditor();
	setupConnectionEditor();

    // Load plugins.
    QSettings settings;
    QStringList pluginsPath = settings.value("Platform/PluginsPath", QStringList("Plugins")).toStringList();
 
    pluginMgr_ = QSharedPointer<PluginManager>(new PluginManager(pluginsPath));

	// some actions need the editors so set them up before the actions
	setupActions();

	// Restore program settings.
	restoreSettings();

	// don't display empty editors
    updateWindows();

	setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
}

//-----------------------------------------------------------------------------
// Function: ~MainWindow()
//-----------------------------------------------------------------------------
MainWindow::~MainWindow() 
{
	saveSettings();
}

void MainWindow::onLibrarySearch() 
{
	if (libraryHandler_) {
		libraryHandler_->searchForIPXactFiles();
	}
}

//-----------------------------------------------------------------------------
// Function: restoreSettings()
//-----------------------------------------------------------------------------
void MainWindow::restoreSettings()
{
    QSettings settings;

    // Load the active workspace.
    curWorkspaceName_ = settings.value("Workspaces/CurrentWorkspace", QString("Default")).toString();
    loadWorkspace(curWorkspaceName_);

    // Create default workspaces if the workspaces registry group is not found.
    settings.beginGroup("Workspaces");

    if (settings.childGroups().empty())
    {
        settings.endGroup();
        saveWorkspace("Default");
        saveWorkspace("Design");
    }
    else
    {
        settings.endGroup();
    }

    // Update the workspace menu.
    updateWorkspaceMenu();
}

//-----------------------------------------------------------------------------
// Function: saveSettings()
//-----------------------------------------------------------------------------
void MainWindow::saveSettings()
{
	QSettings settings;

    // Save the active workspace.
    settings.setValue("Workspaces/CurrentWorkspace", curWorkspaceName_);
    saveWorkspace(curWorkspaceName_);
}

//-----------------------------------------------------------------------------
// Function: updateWorkspaceMenu()
//-----------------------------------------------------------------------------
void MainWindow::updateWorkspaceMenu()
{
    // Create the workspace menu based on the settings.
    workspaceMenu_.clear();

    QSettings settings;
    settings.beginGroup("Workspaces");

    QStringList workspaceIDs = settings.childGroups();
    QActionGroup* workspaceGroup = new QActionGroup(this);
    workspaceGroup->setExclusive(true);

    foreach (QString const& workspaceID, workspaceIDs)
    {
        QString workspaceName = workspaceID;

        QAction* action = new QAction(workspaceName, this);
        action->setCheckable(true);
        action->setChecked(curWorkspaceName_ == workspaceName);

        workspaceGroup->addAction(action);
        workspaceMenu_.addAction(action);
    }

    settings.endGroup();

    connect(workspaceGroup, SIGNAL(triggered(QAction *)), this, SLOT(onWorkspaceChanged(QAction *)));

    // Add actions for creating and deleting new workspaces.
    QAction* addAction = new QAction(tr("New Workspace..."), this);
    connect(addAction, SIGNAL(triggered()), this, SLOT(onNewWorkspace()), Qt::UniqueConnection);

    QAction* deleteAction = new QAction(tr("Delete Workspace..."), this);
    connect(deleteAction, SIGNAL(triggered()), this, SLOT(onDeleteWorkspace()), Qt::UniqueConnection);

    workspaceMenu_.addSeparator();
    workspaceMenu_.addAction(addAction);
    workspaceMenu_.addAction(deleteAction);
}

//-----------------------------------------------------------------------------
// Function: loadWorkspace()
//-----------------------------------------------------------------------------
void MainWindow::loadWorkspace(QString const& workspaceName)
{
    QSettings settings;

    // Create the registry key name.
    QString keyName = "Workspaces/" + workspaceName;
    settings.beginGroup(keyName);

    if (!settings.contains("Geometry"))
    {
        settings.endGroup();
        return;
    }


    // Set the window to normal state (this fixed a weird error with window state).
    //setWindowState(Qt::WindowNoState);

    if (settings.contains("WindowPosition"))
    {
        move(settings.value("WindowPosition").toPoint());
    }

    // If geometry is saved then restore it.
    restoreGeometry(settings.value("Geometry").toByteArray());

    // If state of widgets is saved then restore it.
    if (settings.contains("WindowState")) {
        restoreState(settings.value("WindowState").toByteArray());
    }

    const bool configurationVisible = settings.value("ConfigurationVisibility", true).toBool();
    visibilities_.insert(TabDocument::CONFIGURATIONWINDOW, configurationVisible);
    configurationDock_->toggleViewAction()->setChecked(configurationVisible);

    const bool systemDetailsVisible = settings.value("SystemDetailsVisibility", true).toBool();
    visibilities_.insert(TabDocument::SYSTEM_DETAILS_WINDOW, systemDetailsVisible);
    systemDetailsDock_->toggleViewAction()->setChecked(systemDetailsVisible);

    const bool connectionVisible = settings.value("ConnectionVisibility", true).toBool();
    visibilities_.insert(TabDocument::CONNECTIONWINDOW, connectionVisible);    
    connectionDock_->toggleViewAction()->setChecked(connectionVisible);

    const bool instanceVisible = settings.value("InstanceVisibility", true).toBool();
    visibilities_.insert(TabDocument::INSTANCEWINDOW, instanceVisible); 
    instanceDock_->toggleViewAction()->setChecked(instanceVisible);

    const bool adHocVisible = settings.value("AdHocVisibility", true).toBool();
    visibilities_.insert(TabDocument::ADHOC_WINDOW, adHocVisible);
    adHocDock_->toggleViewAction()->setChecked(adHocVisible);

    const bool addressVisible = settings.value("AddressVisibility", false).toBool();
    visibilities_.insert(TabDocument::ADDRESS_WINDOW, addressVisible);
    addressDock_->toggleViewAction()->setChecked(addressVisible);

    const bool interfaceVisible = settings.value("InterfaceVisibility", true).toBool();
    visibilities_.insert(TabDocument::INTERFACEWINDOW, interfaceVisible);
    interfaceDock_->toggleViewAction()->setChecked(interfaceVisible);

    const bool libraryVisible = settings.value("LibraryVisibility", true).toBool();
    visibilities_.insert(TabDocument::LIBRARYWINDOW, libraryVisible);
    libraryDock_->toggleViewAction()->setChecked(libraryVisible);

    const bool outputVisible = settings.value("OutputVisibility", true).toBool();
    visibilities_.insert(TabDocument::OUTPUTWINDOW, outputVisible);
    consoleDock_->toggleViewAction()->setChecked(outputVisible);

    const bool contextHelpVisible = settings.value("ContextHelpVisibility", false).toBool();
    visibilities_.insert(TabDocument::CONTEXT_HELP_WINDOW, contextHelpVisible);
    contextHelpDock_->toggleViewAction()->setChecked(contextHelpVisible);

    const bool previewVisible = settings.value("PreviewVisibility", true).toBool();
    visibilities_.insert(TabDocument::PREVIEWWINDOW, previewVisible);
    previewDock_->toggleViewAction()->setChecked(previewVisible);

    /*const bool notesVisible = settings.value("NotesVisibility", true).toBool();
    visibilities_.insert(TabDocument::NOTES_WINDOW, notesVisible);
    notesDock_->toggleViewAction()->setChecked(notesVisible);*/

    updateWindows();
    

    // Load filter settings.
    FilterWidget::FilterOptions filters;
    settings.beginGroup("LibraryFilters");
    settings.beginGroup("Type");
    filters.type.components_ = settings.value("ShowComponents", true).toBool();
    filters.type.buses_ = settings.value("ShowBuses", true).toBool();
    filters.type.advanced_ = settings.value("ShowAdvanced", false).toBool();
    settings.endGroup();
    settings.beginGroup("Implementation");
    filters.implementation.hw_ = settings.value("ShowHW", true).toBool();
    filters.implementation.sw_ = settings.value("ShowSW", true).toBool();
    filters.implementation.system_ = settings.value("ShowSystem", true).toBool();
    settings.endGroup();
    settings.beginGroup("Hierarchy");
    filters.hierarchy.global_ = settings.value("ShowGlobal", true).toBool();
    filters.hierarchy.product_ = settings.value("ShowProduct", true).toBool();
    filters.hierarchy.board_ = settings.value("ShowBoard", true).toBool();
    filters.hierarchy.chip_ = settings.value("ShowChip", true).toBool();
    filters.hierarchy.soc_ = settings.value("ShowSoC", true).toBool();
    filters.hierarchy.ip_ = settings.value("ShowIP", true).toBool();
    settings.endGroup();
    settings.beginGroup("Firmness");
    filters.firmness.templates_ = settings.value("ShowTemplates", true).toBool();
    filters.firmness.mutable_ =settings.value("ShowMutable", true).toBool();
    filters.firmness.fixed_ =settings.value("ShowFixed", true).toBool();
    settings.endGroup();
    dialer_->setFilters(filters);
}

//-----------------------------------------------------------------------------
// Function: saveWorkspace()
//-----------------------------------------------------------------------------
void MainWindow::saveWorkspace(QString const& workspaceName)
{
    // Instance to save the settings.
    QSettings settings;

    // Create the registry group name.
    QString keyName = "Workspaces/" + workspaceName;


    FilterWidget::FilterOptions filters = dialer_->getFilters();

    // Save the geometry and state of windows.
    settings.beginGroup(keyName);

    settings.setValue("WindowState", saveState());
    settings.setValue("Geometry", saveGeometry());
    settings.setValue("WindowPosition", pos());
    settings.setValue("ConfigurationVisibility", visibilities_.value(TabDocument::CONFIGURATIONWINDOW));
    settings.setValue("SystemDetailsVisibility", visibilities_.value(TabDocument::SYSTEM_DETAILS_WINDOW));
    settings.setValue("ConnectionVisibility", visibilities_.value(TabDocument::CONNECTIONWINDOW));
    settings.setValue("InstanceVisibility", visibilities_.value(TabDocument::INSTANCEWINDOW));
    settings.setValue("AdHocVisibility", visibilities_.value(TabDocument::ADHOC_WINDOW));
    settings.setValue("AddressVisibility", visibilities_.value(TabDocument::ADDRESS_WINDOW));
    settings.setValue("InterfaceVisibility", visibilities_.value(TabDocument::INTERFACEWINDOW));
    settings.setValue("LibraryVisibility", visibilities_.value(TabDocument::LIBRARYWINDOW));
    settings.setValue("OutputVisibility", visibilities_.value(TabDocument::OUTPUTWINDOW));
    settings.setValue("ContextHelpVisibility", visibilities_.value(TabDocument::CONTEXT_HELP_WINDOW));
    settings.setValue("PreviewVisibility", visibilities_.value(TabDocument::PREVIEWWINDOW));
    settings.setValue("NotesVisibility", visibilities_.value(TabDocument::NOTES_WINDOW));

    // Save filters.
    settings.beginGroup("LibraryFilters");
    settings.beginGroup("Type");
    settings.setValue("ShowComponents", filters.type.components_);
    settings.setValue("ShowBuses", filters.type.buses_);
    settings.setValue("ShowAdvanced", filters.type.advanced_);
    settings.endGroup();
    settings.beginGroup("Implementation");
    settings.setValue("ShowHW", filters.implementation.hw_);
    settings.setValue("ShowSW", filters.implementation.sw_);
    settings.setValue("ShowSystem", filters.implementation.system_);
    settings.endGroup();
    settings.beginGroup("Hierarchy");
    settings.setValue("ShowGlobal", filters.hierarchy.global_);
    settings.setValue("ShowProduct", filters.hierarchy.product_);
    settings.setValue("ShowBoard", filters.hierarchy.board_);
    settings.setValue("ShowChip", filters.hierarchy.chip_);
    settings.setValue("ShowSoC", filters.hierarchy.soc_);
    settings.setValue("ShowIP", filters.hierarchy.ip_);
    settings.endGroup();
    settings.beginGroup("Firmness");
    settings.setValue("ShowTemplates", filters.firmness.templates_);
    settings.setValue("ShowMutable", filters.firmness.mutable_);
    settings.setValue("ShowFixed", filters.firmness.fixed_);
    settings.endGroup(); // Firmness
    settings.endGroup(); // LibraryFilters

    settings.endGroup();    
}

void MainWindow::setupActions() 
{
	// the action to create a new hierarchical component
	actNew_ = new QAction(QIcon(":/icons/common/graphics/file-new.png"), tr("New"), this);
	actNew_->setShortcut(QKeySequence::New);
	actNew_->setProperty("rowSpan", 2);
	actNew_->setProperty("colSpan", 2);
	connect(actNew_, SIGNAL(triggered()), this, SLOT(createNew()));

	actSave_ = new QAction(QIcon(":/icons/common/graphics/file-save.png"), tr("Save"), this);
	actSave_->setShortcut(QKeySequence::Save);
	actSave_->setProperty("rowSpan", 2);
	actSave_->setProperty("colSpan", 2);
	actSave_->setEnabled(false);
	connect(actSave_, SIGNAL(triggered()), this, SLOT(saveCurrent()));

	actSaveAs_ = new QAction(QIcon(":/icons/common/graphics/file-save.png"), tr("Save As"), this);
	actSaveAs_->setShortcut(QKeySequence::SaveAs);
	actSaveAs_->setProperty("rowSpan", 2);
	actSaveAs_->setProperty("colSpan", 2);
	actSaveAs_->setEnabled(false);
	connect(actSaveAs_, SIGNAL(triggered()), this, SLOT(saveCurrentAs()));

	actSaveAll_ = new QAction(QIcon(":/icons/common/graphics/file-save_all.png"), tr("Save All"), this);
	actSaveAll_->setShortcut(QKeySequence("Ctrl+Shift+S"));
	actSaveAll_->setProperty("rowSpan", 2);
	actSaveAll_->setProperty("colSpan", 2);
	connect(actSaveAll_, SIGNAL(triggered()), this, SLOT(saveAll()));

	actPrint_ = new QAction(QIcon(":/icons/common/graphics/file-print.png"), tr("Print"), this);
	actPrint_->setShortcut(QKeySequence::Print);
	actPrint_->setProperty("rowSpan", 2);
	actPrint_->setProperty("colSpan", 2);
	actPrint_->setEnabled(false);
	connect(actPrint_, SIGNAL(triggered()), this, SLOT(printCurrent()));

	actUndo_ = new QAction(QIcon(":/icons/common/graphics/edit-undo.png"), tr("Undo"), this);
	actUndo_->setShortcut(QKeySequence::Undo);
	actUndo_->setProperty("rowSpan", 2);
	actUndo_->setProperty("colSpan", 2);
	connect(actUndo_, SIGNAL(triggered()), this, SLOT(undo()));

	actRedo_ = new QAction(QIcon(":/icons/common/graphics/edit-redo.png"), tr("Redo"), this);
	actRedo_->setShortcut(QKeySequence::Redo);
	actRedo_->setProperty("rowSpan", 2);
	actRedo_->setProperty("colSpan", 2);
	connect(actRedo_, SIGNAL(triggered()), this, SLOT(redo()));

	actLibraryLocations_ = new QAction(QIcon(":/icons/common/graphics/library-config.png"),
		tr("Configure Library"), this);
	connect(actLibraryLocations_, SIGNAL(triggered()),
		this, SLOT(setLibraryLocations()), Qt::UniqueConnection);

	// the action to search for IP-Xact documents in file system
	actLibrarySearch_ = new QAction(QIcon(":/icons/common/graphics/library-refresh.png"),
		tr("Refresh Library"), this);
	connect(actLibrarySearch_, SIGNAL(triggered()),
		this, SLOT(onLibrarySearch()), Qt::UniqueConnection);

	// Check the library integrity
	actCheckIntegrity_ = new QAction(QIcon(":/icons/common/graphics/checkIntegrity.png"),
		tr("Check Integrity"), this);
	connect(actCheckIntegrity_, SIGNAL(triggered()),
		libraryHandler_, SLOT(onCheckLibraryIntegrity()), Qt::UniqueConnection);

	// Initialize the action to generate VHDL.
	actGenVHDL_ = new QAction(QIcon(":/icons/common/graphics/vhdl_gen.png"),
		tr("Generate Top-VHDL"), this);
	connect(actGenVHDL_, SIGNAL(triggered()), 
		this, SLOT(generateVHDL()), Qt::UniqueConnection);

	// Initialize the action to generate a ModelSim makefile.
	actGenModelSim_ = new QAction(QIcon(":/icons/common/graphics/modelsim_generator.png"),
		tr("Generate ModelSim Makefile"), this);
	connect(actGenModelSim_, SIGNAL(triggered()), 
		this, SLOT(generateModelSim()), Qt::UniqueConnection);

	// Initialize the action to generate a Quartus project.
	actGenQuartus_ = new QAction(QIcon(":/icons/common/graphics/quartus_generator.png"),
		tr("Generate Quartus Project"), this);
	connect(actGenQuartus_, SIGNAL(triggered()), 
		this, SLOT(generateQuartus()), Qt::UniqueConnection);

	// initialize the action to generate documentation for the component/design
	actGenDocumentation_ = new QAction(QIcon(":icons/common/graphics/documentation.png"),
		tr("Generate Documentation"), this);
	connect(actGenDocumentation_, SIGNAL(triggered()),
		this, SLOT(generateDoc()), Qt::UniqueConnection);

	// Initialize the action to add a new column.
	actAddColumn_ = new QAction(QIcon(":/icons/common/graphics/diagram-add-column.png"), tr("Add Column"), this);
	actAddColumn_->setProperty("rowSpan", 2);
	actAddColumn_->setProperty("colSpan", 2);
	connect(actAddColumn_, SIGNAL(triggered()), 
		this, SLOT(addColumn()), Qt::UniqueConnection);

	// Initialize the action to set draw mode to selection mode.
	actToolSelect_ = new QAction(QIcon(":/icons/common/graphics/tool-select.png"), tr("Select Tool"), this);
	actToolSelect_->setCheckable(true);
	actToolSelect_->setChecked(true);

	// Initialize the action to set draw mode to connection mode.
	actToolConnect_ = new QAction(QIcon(":/icons/common/graphics/tool-interconnection.png"),
		tr("Interconnection Tool"), this);
	actToolConnect_->setCheckable(true);

	// Initialize the action to set draw mode to interface mode.
	actToolInterface_ = new QAction(QIcon(":/icons/common/graphics/tool-interface.png"), tr("Interface Tool"), this);
	actToolInterface_->setCheckable(true);

	actToolDraft_ = new QAction(QIcon(":/icons/common/graphics/tool-drafting.png"), tr("Drafting Tool"), this);
	actToolDraft_->setCheckable(true);

	actToolToggleOffPage_ = new QAction(QIcon(":/icons/common/graphics/tool-toggle_offpage.png"),
		tr("Toggle Off-Page Tool"), this);
	actToolToggleOffPage_->setCheckable(true);

    actToolLabel_ = new QAction(QIcon(":/icons/common/graphics/balloon.png"), tr("Sticky Note Tool"), this);
    actToolLabel_->setCheckable(true);

	modeActionGroup_ = new QActionGroup(this);
	modeActionGroup_->setExclusive(true);
	modeActionGroup_->addAction(actToolSelect_);
	modeActionGroup_->addAction(actToolConnect_);
	modeActionGroup_->addAction(actToolInterface_);
	modeActionGroup_->addAction(actToolDraft_);
	modeActionGroup_->addAction(actToolToggleOffPage_);
    modeActionGroup_->addAction(actToolLabel_);
	connect(modeActionGroup_, SIGNAL(triggered(QAction *)), this, SLOT(drawModeChange(QAction *)));

	// Initialize the action to zoom in.
	actZoomIn_ = new QAction(QIcon(":/icons/common/graphics/view-zoom_in.png"), tr("Zoom In"), this);
	actZoomIn_->setEnabled(false);
	connect(actZoomIn_, SIGNAL(triggered()), this, SLOT(zoomIn()));

	// Initialize the action to zoom out.
	actZoomOut_ = new QAction(QIcon(":/icons/common/graphics/view-zoom_out.png"), tr("Zoom Out"), this);
	actZoomOut_->setEnabled(false);
	connect(actZoomOut_, SIGNAL(triggered()), this, SLOT(zoomOut()));

	// Initialize the action to reset the zoom to original 1:1 ratio.
	actZoomOriginal_ = new QAction(QIcon(":/icons/common/graphics/view-zoom_original.png"),
		tr("Original 1:1 Zoom"), this);
	actZoomOriginal_->setEnabled(false);
	connect(actZoomOriginal_, SIGNAL(triggered()), this, SLOT(zoomOriginal()));

	// Initialize the action to fit the document into the view.
	actFitInView_ = new QAction(QIcon(":/icons/common/graphics/view-fit_best.png"),
		tr("Fit Document to View"), this);
	actFitInView_->setEnabled(false);
	connect(actFitInView_, SIGNAL(triggered()), this, SLOT(fitInView()));

	// the action for user to select the visible docks
	actVisibleDocks_ = new QAction(QIcon(":icons/common/graphics/dockSelect.png"),
		tr("Visible Windows"), this);
	connect(actVisibleDocks_, SIGNAL(triggered()), this, SLOT(selectVisibleDocks()), Qt::UniqueConnection);

	// Initialize the action to manage visibility control.
	actVisibilityControl_ = new QAction(QIcon(":icons/common/graphics/visibility.png"), tr("Visibility Control"), this);
	actVisibilityControl_->setEnabled(false);
	connect(actVisibilityControl_, SIGNAL(triggered()),
		this, SLOT(openVisibilityControlMenu()), Qt::UniqueConnection);

	// Initialize the action to manage workspaces.
	actWorkspaces_ = new QAction(QIcon(":icons/common/graphics/workspace.png"),
		tr("Workspaces"), this);
	connect(actWorkspaces_, SIGNAL(triggered()), this, SLOT(openWorkspaceMenu()), Qt::UniqueConnection);

	actRefresh_ = new QAction(QIcon(":/icons/common/graphics/refresh.png"), tr("Refresh"), this);
	actRefresh_->setProperty("rowSpan", 2);
	actRefresh_->setProperty("colSpan", 2);
	actRefresh_->setShortcut(QKeySequence("F5"));
	connect(actRefresh_, SIGNAL(triggered(bool)), this, SLOT(refresh()));

	actProtect_ = new QAction(QIcon(":/icons/common/graphics/protection-unlocked.png"), tr("Unlocked"), this);
	actProtect_->setProperty("rowSpan", 2);
	actProtect_->setProperty("colSpan", 2);
	actProtect_->setCheckable(true);
	actProtect_->setEnabled(false);
	actProtect_->setShortcut(QKeySequence("Ctrl+Space"));
	connect(actProtect_, SIGNAL(triggered(bool)), this, SLOT(changeProtection(bool)));

	// Initialize the action to open Kactus2 settings.
	actSettings_ = new QAction(QIcon(":/icons/common/graphics/system-settings.png"), tr("Settings"), this);
	actSettings_->setProperty("rowSpan", 2);
	actSettings_->setProperty("colSpan", 2);
	connect(actSettings_, SIGNAL(triggered()), this, SLOT(openSettings()));

	// Initialize the action to open the about box.
	actAbout_= new QAction(QIcon(":/icons/common/graphics/system-about.png"), tr("About"), this);
	actAbout_->setProperty("rowSpan", 2);
	actAbout_->setProperty("colSpan", 2);
	connect(actAbout_, SIGNAL(triggered()), this, SLOT(showAbout()), Qt::UniqueConnection);

	// Initialize the action to open the help window.
	actHelp_= new QAction(QIcon(":/icons/common/graphics/system-help.png"), tr("Help"), this);
	actHelp_->setProperty("rowSpan", 2);
	actHelp_->setProperty("colSpan", 2);
	connect(actHelp_, SIGNAL(triggered()), this, SLOT(showHelp()), Qt::UniqueConnection);

	// Initialize the action to exit the program.
	actExit_ = new QAction(QIcon(":/icons/common/graphics/system-exit.png"), tr("Exit"), this);
	actExit_->setProperty("rowSpan", 2);
	actExit_->setProperty("colSpan", 2);
	connect(actExit_, SIGNAL(triggered()), this, SLOT(close()), Qt::UniqueConnection);

    connectVisibilityControls();

	setupMenus();
}

//-----------------------------------------------------------------------------
// Function: setupMenus()
//-----------------------------------------------------------------------------
void MainWindow::setupMenus()
{
	QDockWidget* menuDock = new QDockWidget(tr("Menu"), this);
	menuDock->setObjectName(tr("Menu"));
	menuDock->setTitleBarWidget(new QWidget(this));
	menuDock->setAllowedAreas(Qt::TopDockWidgetArea);
	menuDock->setFeatures(QDockWidget::NoDockWidgetFeatures);

    ribbon_ = new Ribbon(menuDock);

    menuDock->setWidget(ribbon_);
	addDockWidget(Qt::TopDockWidgetArea, menuDock);

	// The "File" group.
	RibbonGroup* fileGroup = ribbon_->addGroup(tr("File"));
	fileGroup->addAction(actNew_);
	fileGroup->addAction(actSave_);
	fileGroup->addAction(actSaveAs_);
	fileGroup->addAction(actSaveAll_);
	fileGroup->addAction(actPrint_);

	// The "Edit" group.
	editGroup_ = ribbon_->addGroup(tr("Edit"));
	editGroup_->addAction(actUndo_);
	editGroup_->addAction(actRedo_);
	editGroup_->setVisible(false);
	editGroup_->setEnabled(false);

	// The "Library" group.
	RibbonGroup* libGroup = ribbon_->addGroup(tr("Library"));
    libGroup->addAction(actLibraryLocations_);
	libGroup->addAction(actLibrarySearch_);
	libGroup->addAction(actCheckIntegrity_);
    libGroup->setAutoCollapse(false);

    // The "Generation" group.
    generationGroup_ = ribbon_->addGroup(tr("Generation"));
    generationGroup_->addAction(actGenVHDL_);
    generationGroup_->addAction(actGenDocumentation_);
    generationGroup_->addAction(actGenModelSim_);
    generationGroup_->addAction(actGenQuartus_);
    generationGroup_->setVisible(false);
    generationGroup_->setEnabled(false);

    createGeneratorPluginActions();
    
	//! The "Diagram Tools" group.
	diagramToolsGroup_ = ribbon_->addGroup(tr("Diagram Tools"));
	diagramToolsGroup_->addAction(actAddColumn_);
	diagramToolsGroup_->addAction(actToolSelect_);
	diagramToolsGroup_->addAction(actToolConnect_);
	diagramToolsGroup_->addAction(actToolInterface_);
	diagramToolsGroup_->addAction(actToolDraft_);
    diagramToolsGroup_->addAction(actToolToggleOffPage_);
    diagramToolsGroup_->addAction(actToolLabel_);
	diagramToolsGroup_->setVisible(false);

	//! The "View" group.
	RibbonGroup* viewGroup = ribbon_->addGroup(tr("View"));
	viewGroup->addAction(actZoomIn_);
	viewGroup->addAction(actZoomOut_);
	viewGroup->addAction(actZoomOriginal_);
	viewGroup->addAction(actFitInView_);
	viewGroup->addAction(actVisibleDocks_);
    viewGroup->addAction(actVisibilityControl_);
    viewGroup->addAction(actWorkspaces_);

	//! The Protection group.
	protectGroup_ = ribbon_->addGroup(tr("Protection"));
	protectGroup_->addAction(actProtect_);
    protectGroup_->addAction(actRefresh_);
	protectGroup_->setVisible(false);

	//! The "System" group.
	RibbonGroup* sysGroup = ribbon_->addGroup(tr("System"));
	sysGroup->addAction(actSettings_);
    sysGroup->addAction(actHelp_);
	sysGroup->addAction(actAbout_);
	sysGroup->addAction(actExit_);
    sysGroup->setAutoCollapse(false);

	// the menu to display the dock widgets
    windowsMenu_.addAction(addressDock_->toggleViewAction());	
    windowsMenu_.addAction(adHocDock_->toggleViewAction());	        
    windowsMenu_.addAction(connectionDock_->toggleViewAction());
    windowsMenu_.addAction(contextHelpDock_->toggleViewAction());    
    windowsMenu_.addAction(instanceDock_->toggleViewAction());
    windowsMenu_.addAction(previewDock_->toggleViewAction());
    windowsMenu_.addAction(configurationDock_->toggleViewAction());
    windowsMenu_.addAction(systemDetailsDock_->toggleViewAction());
    windowsMenu_.addAction(libraryDock_->toggleViewAction());       
    windowsMenu_.addAction(interfaceDock_->toggleViewAction());
    //windowsMenu_.addAction(notesDock_->toggleViewAction()); 
    windowsMenu_.addAction(consoleDock_->toggleViewAction());
    
}

void MainWindow::setupDrawBoard() {

	designTabs_ = new EditorArea();
	designTabs_->setMovable(true);
	designTabs_->setTabsClosable(true);
	setCentralWidget(designTabs_);

	connect(designTabs_, SIGNAL(tabCloseRequested(int)),
		this, SLOT(onTabCloseRequested(int)), Qt::UniqueConnection);
	connect(designTabs_, SIGNAL(currentChanged(int)),
		this, SLOT(onTabChanged(int)), Qt::UniqueConnection);
}

void MainWindow::setupLibraryDock() {

    // set up the dock widget for the library
	libraryDock_ = new QDockWidget(tr("IP-XACT Library"), this);
	libraryDock_->setObjectName(tr("Library"));
	libraryDock_->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
	libraryDock_->setFeatures(QDockWidget::AllDockWidgetFeatures);

	dialer_ = new VLNVDialer(libraryDock_);

    libraryHandler_ = new LibraryHandler(dialer_, this);

    // pass the library root to dialer.
	const LibraryItem* root = libraryHandler_->getTreeRoot();
	dialer_->setRootItem(root);

	libraryDock_->setWidget(libraryHandler_);

	// create a container widget for dialer and library display
	QWidget* container = new QWidget(this);
	QVBoxLayout* containerLayout = new QVBoxLayout(container);
	containerLayout->addWidget(dialer_, 0);
	containerLayout->addWidget(libraryHandler_, 1);
	containerLayout->setSpacing(0);
	containerLayout->setContentsMargins(0, 0, 0, 0);
	libraryDock_->setWidget(container);

	addDockWidget(Qt::LeftDockWidgetArea, libraryDock_);

	// set up the preview box to display the component preview
	previewDock_ = new QDockWidget(tr("Component Preview"), this);
	previewDock_->setObjectName(tr("ComponentPreview"));
	previewDock_->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea 
		| Qt::BottomDockWidgetArea);
	previewDock_->setFeatures(QDockWidget::AllDockWidgetFeatures);

	previewBox_ = new ComponentPreviewBox(libraryHandler_);
	previewDock_->setWidget(previewBox_);

	addDockWidget(Qt::LeftDockWidgetArea, previewDock_);

    connect(libraryHandler_, SIGNAL(errorMessage(const QString&)),
        this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
    connect(libraryHandler_, SIGNAL(noticeMessage(const QString&)),
        this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

	connect(libraryHandler_, SIGNAL(openDesign(const VLNV&, const QString&)),
		this, SLOT(openDesign(const VLNV&, const QString&)));
    connect(libraryHandler_, SIGNAL(openMemoryDesign(const VLNV&, const QString&)),
        this, SLOT(openMemoryDesign(const VLNV&, const QString&)));

	connect(libraryHandler_, SIGNAL(createBus(const VLNV&, const QString&)),
		this, SLOT(createBus(const VLNV&, const QString&)), Qt::UniqueConnection);
	connect(libraryHandler_, SIGNAL(createAbsDef(const VLNV&, const QString&, bool)),
		this, SLOT(createAbsDef(const VLNV&, const QString&, bool)), Qt::UniqueConnection);
    connect(libraryHandler_, SIGNAL(createComDef(const VLNV&, const QString&)),
        this, SLOT(createComDefinition(const VLNV&, const QString&)), Qt::UniqueConnection);
    connect(libraryHandler_, SIGNAL(createApiDef(const VLNV&, const QString&)),
        this, SLOT(createApiDefinition(const VLNV&, const QString&)), Qt::UniqueConnection);
    connect(libraryHandler_, SIGNAL(createSWDesign(const VLNV&)),
        this, SLOT(createSWDesign(const VLNV&)), Qt::UniqueConnection);
    connect(libraryHandler_, SIGNAL(createSystemDesign(const VLNV&)),
        this, SLOT(createSystemDesign(const VLNV&)), Qt::UniqueConnection);
    connect(libraryHandler_, SIGNAL(createDesignForExistingComponent(const VLNV&)),
        this, SLOT(createDesignForExistingComponent(const VLNV&)), Qt::UniqueConnection);
	connect(libraryHandler_, SIGNAL(openComponent(const VLNV&)),
		this, SLOT(openComponent(const VLNV&)), Qt::UniqueConnection);
	connect(libraryHandler_, SIGNAL(openSWDesign(const VLNV&, QString const&)),
		this, SLOT(openSWDesign(const VLNV&, QString const&)), Qt::UniqueConnection);
    connect(libraryHandler_, SIGNAL(openSystemDesign(const VLNV&, QString const&)),
        this, SLOT(openSystemDesign(const VLNV&, QString const&)), Qt::UniqueConnection);
	connect(libraryHandler_, SIGNAL(openBus(const VLNV&, const VLNV&, bool)),
		this, SLOT(openBus(const VLNV&, const VLNV&, bool)), Qt::UniqueConnection);
    connect(libraryHandler_, SIGNAL(openComDefinition(const VLNV&)),
        this, SLOT(openComDefinition(const VLNV&)), Qt::UniqueConnection);
    connect(libraryHandler_, SIGNAL(openApiDefinition(const VLNV&)),
        this, SLOT(openApiDefinition(const VLNV&)), Qt::UniqueConnection);
	connect(libraryHandler_, SIGNAL(refreshDialer()),
		dialer_, SLOT(refreshLibrary()), Qt::UniqueConnection);
	connect(libraryHandler_, SIGNAL(itemSelected(const VLNV&)),
		previewBox_, SLOT(setComponent(const VLNV&)), Qt::UniqueConnection);
	
	connect(libraryHandler_, SIGNAL(createComponent(KactusAttribute::ProductHierarchy,
		KactusAttribute::Firmness,
		const VLNV&, const QString&)),
		this, SLOT(createComponent(KactusAttribute::ProductHierarchy,
		KactusAttribute::Firmness,
		const VLNV&, const QString&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: mainwindow::setupMessageConsole()
//-----------------------------------------------------------------------------
void MainWindow::setupMessageConsole() 
{
	consoleDock_ = new QDockWidget(tr("Output"), this);
	consoleDock_->setObjectName(tr("Output"));
	consoleDock_->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
	consoleDock_->setFeatures(QDockWidget::AllDockWidgetFeatures);

	console_ = new MessageConsole(consoleDock_);
	consoleDock_->setWidget(console_);
	addDockWidget(Qt::BottomDockWidgetArea, consoleDock_);

	connect(this, SIGNAL(errorMessage(const QString&)),
		console_, SLOT(onErrorMessage(const QString&)), Qt::UniqueConnection);
	connect(this, SIGNAL(noticeMessage(const QString&)),
		console_, SLOT(onNoticeMessage(const QString&)), Qt::UniqueConnection);
    // Force console visible on new messages.
    connect(this, SIGNAL(errorMessage(const QString&)),
        consoleDock_, SLOT(show()), Qt::UniqueConnection);
    connect(this, SIGNAL(noticeMessage(const QString&)),
        consoleDock_, SLOT(show()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: MainWindow::setupContextHelp()
//-----------------------------------------------------------------------------
void MainWindow::setupContextHelp()
{
    // Create the dock widget for the context help.
    contextHelpDock_ = new QDockWidget(tr("Context Help"), this);
    contextHelpDock_->setObjectName(tr("Context Help"));
    contextHelpDock_->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
    contextHelpDock_->setFeatures(QDockWidget::AllDockWidgetFeatures);

    // Initialize the help engine.
    QSettings settings;
    QString helpPath = settings.value("Platform/HelpPath", "Help").toString();

    QHelpEngine* helpEngine = new QHelpEngine(helpPath + "/Kactus2Help.qhc", this);
    helpEngine->setupData();

    // Create the help window.
    helpWnd_ = new HelpWindow(helpEngine, this);

    // Create the context help browser.
    contextHelpBrowser_ = new ContextHelpBrowser(helpEngine, "qthelp://com.tut.kactus2.2.0/doc",
                                                 contextHelpDock_);

    QColor bgColor = QApplication::palette().color(QPalette::Window);
    QString style = QString("QTextBrowser { border: 5px solid transparent; "
                            "background-color: #%1}").arg(bgColor.rgb() & 0x00FFFFFF, 0, 16);
    contextHelpBrowser_->setStyleSheet(style);

    contextHelpDock_->setWidget(contextHelpBrowser_);
    addDockWidget(Qt::BottomDockWidgetArea, contextHelpDock_);

    connect(this, SIGNAL(helpUrlRequested(QString const&)),
            contextHelpBrowser_, SLOT(onHelpRequested(QString const&)), Qt::UniqueConnection);
    contextHelpBrowser_->onHelpRequested("index.html");
}

//-----------------------------------------------------------------------------
// Function: MainWindow::setupNotesEditor()
//-----------------------------------------------------------------------------
void MainWindow::setupNotesEditor()
{
    /*notesDock_ = new QDockWidget(tr("Notes"), this);
    notesDock_->setObjectName(tr("Notes"));
    notesDock_->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
    notesDock_->setFeatures(QDockWidget::AllDockWidgetFeatures);

    notesEditor_ = new NotesStack(notesDock_);
    notesDock_->setWidget(notesEditor_);
    addDockWidget(Qt::BottomDockWidgetArea, notesDock_);

    connect(this, SIGNAL(helpUrlRequested(QString const&)),
        notesEditor_, SLOT(onContextChanged(QString const&)), Qt::UniqueConnection);*/
}

//-----------------------------------------------------------------------------
// Function: mainwindow::setupConfigurationEditor()
//-----------------------------------------------------------------------------
void MainWindow::setupConfigurationEditor() {

	configurationDock_ = new QDockWidget(tr("Design Configuration Details"), this);
	configurationDock_->setObjectName(tr("Configuration editor"));
	configurationDock_->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	configurationDock_->setFeatures(QDockWidget::AllDockWidgetFeatures);

	configurationEditor_ = new ConfigurationEditor(libraryHandler_, configurationDock_);
	configurationDock_->setWidget(configurationEditor_);
	addDockWidget(Qt::RightDockWidgetArea, configurationDock_);

	connect(configurationEditor_, SIGNAL(contentChanged()),
		    this, SLOT(onDesignChanged()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: setupSystemDetailsEditor()
//-----------------------------------------------------------------------------
void MainWindow::setupSystemDetailsEditor()
{
    systemDetailsDock_ = new QDockWidget(tr("HW Mapping Details"), this);
    systemDetailsDock_->setObjectName(tr("HW Mapping Details Editor"));
    systemDetailsDock_->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    systemDetailsDock_->setFeatures(QDockWidget::AllDockWidgetFeatures);

    systemDetailsEditor_ = new SystemDetailsEditor(libraryHandler_, systemDetailsDock_);
    systemDetailsDock_->setWidget(systemDetailsEditor_);
    addDockWidget(Qt::RightDockWidgetArea, systemDetailsDock_);

    connect(systemDetailsEditor_, SIGNAL(contentChanged()),
            this, SLOT(onDesignChanged()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: mainwindow::setupInstanceEditor()
//-----------------------------------------------------------------------------
void MainWindow::setupInstanceEditor()
{
	instanceDock_ = new QDockWidget(tr("Component Instance Details"), this);
	instanceDock_->setObjectName(tr("Instance Editor"));
	instanceDock_->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	instanceDock_->setFeatures(QDockWidget::AllDockWidgetFeatures);

	instanceEditor_ = new ComponentInstanceEditor(instanceDock_);
	instanceDock_->setWidget(instanceEditor_);
	addDockWidget(Qt::RightDockWidgetArea, instanceDock_);

	connect(instanceEditor_, SIGNAL(contentChanged()),
		this, SLOT(onDesignChanged()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: mainwindow::setupAdHocVisibilityEditor()
//-----------------------------------------------------------------------------
void MainWindow::setupAdHocVisibilityEditor()
{
    adHocDock_ = new QDockWidget(tr("Ad-hoc Visibility"), this);
    adHocDock_->setObjectName(tr("Ad-hoc Visibility"));
    adHocDock_->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    adHocDock_->setFeatures(QDockWidget::AllDockWidgetFeatures);

    adHocEditor_ = new AdHocEditor(adHocDock_);
    adHocDock_->setWidget(adHocEditor_);
    addDockWidget(Qt::RightDockWidgetArea, adHocDock_);
}

//-----------------------------------------------------------------------------
// Function: mainwindow::setupAddressEditor()
//-----------------------------------------------------------------------------
void MainWindow::setupAddressEditor()
{
    addressDock_ = new QDockWidget(tr("Address Editor"), this);
    addressDock_->setObjectName(tr("Address Editor"));
    addressDock_->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
    addressDock_->setFeatures(QDockWidget::AllDockWidgetFeatures);

    addressEditor_ = new AddressEditor(addressDock_);
    addressDock_->setWidget(addressEditor_);
    addDockWidget(Qt::BottomDockWidgetArea, addressDock_);

    connect(addressEditor_, SIGNAL(contentChanged()), this, SLOT(onDesignChanged()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: mainwindow::setupInterfaceEditor()
//-----------------------------------------------------------------------------
void MainWindow::setupInterfaceEditor()
{
	interfaceDock_ = new QDockWidget(tr("Interface Editor"), this);
	interfaceDock_->setObjectName(tr("Interface Editor"));
	interfaceDock_->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	interfaceDock_->setFeatures(QDockWidget::AllDockWidgetFeatures);

	interfaceEditor_ = new InterfaceEditor(interfaceDock_, libraryHandler_);
	interfaceDock_->setWidget(interfaceEditor_);
	addDockWidget(Qt::RightDockWidgetArea, interfaceDock_);
}

//-----------------------------------------------------------------------------
// Function: mainwindow::setupConnectionEditor()
//-----------------------------------------------------------------------------
void MainWindow::setupConnectionEditor()
{
	connectionDock_ = new QDockWidget(tr("Connection Editor"), this);
	connectionDock_->setObjectName(tr("Connection Editor"));
	connectionDock_->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	connectionDock_->setFeatures(QDockWidget::AllDockWidgetFeatures);

	connectionEditor_ = new ConnectionEditor(connectionDock_, libraryHandler_);
	connectionDock_->setWidget(connectionEditor_);
	addDockWidget(Qt::RightDockWidgetArea, connectionDock_);
}

//-----------------------------------------------------------------------------
// Function: mainwindow::onDesignChanged()
//-----------------------------------------------------------------------------
void MainWindow::onDesignChanged() {

	// find the currently open editor
	QWidget* widget = designTabs_->currentWidget();

	// if editor was found
	if (widget) {
		DesignWidget* editor = dynamic_cast<DesignWidget*>(widget);

		// if editor is design widget then set it to be modified.
		if (editor) {
			editor->setModified(true);
			updateMenuStrip();
		}
	}
}

//-----------------------------------------------------------------------------
// Function: mainwindow::onClearItemSelection()
//-----------------------------------------------------------------------------
void MainWindow::onClearItemSelection()
{
    HWDesignWidget* designWidget = dynamic_cast<HWDesignWidget*>(designTabs_->currentWidget());

    if (designWidget != 0)
    {
        adHocEditor_->setDataSource(designWidget->getDiagram());
    }
    else
    {
        adHocEditor_->clear();
    }

    addressEditor_->clear();
	instanceEditor_->clear();
	interfaceEditor_->clear();
	connectionEditor_->clear();
}

//-----------------------------------------------------------------------------
// Function: mainwindow::onComponentSelected()
//-----------------------------------------------------------------------------
void MainWindow::onComponentSelected( ComponentItem* component ) {
	Q_ASSERT(component);

	connectionEditor_->clear();
	interfaceEditor_->clear();

	// Update the instance and ad-hoc visibility editors.
	instanceEditor_->setComponent(component);

    if (dynamic_cast<HWComponentItem*>(component) != 0)
    {
        adHocEditor_->setDataSource(dynamic_cast<HWComponentItem*>(component));
        addressEditor_->setComponent(component);
    }
    else
    {
        adHocEditor_->clear();
    }

	if (component->componentModel()->getVlnv()->isValid())
	{
		libraryHandler_->onSelectionChanged(*component->componentModel()->getVlnv());
		previewBox_->setComponent(*component->componentModel()->getVlnv());
	}
	else
	{
		libraryHandler_->onClearSelection();
	}
}

//-----------------------------------------------------------------------------
// Function: mainwindow::onInterfaceSelected()
//-----------------------------------------------------------------------------
void MainWindow::onInterfaceSelected( ConnectionEndpoint* interface ) {
	Q_ASSERT(interface);

    // if the port has an encompassing component then it is selected
    ComponentItem* component = interface->encompassingComp();

    if (component != 0 && component->componentModel()->getVlnv()->isValid()) {

        libraryHandler_->onSelectionChanged(*component->componentModel()->getVlnv());
        previewBox_->setComponent(*component->componentModel()->getVlnv());
    }
    // if no component can be identified
    else
        libraryHandler_->onClearSelection();

    adHocEditor_->clear();
	connectionEditor_->clear();
	instanceEditor_->clear();
    
    // TODO: Address editor bus interface select.

    if (!interface->isAdHoc())
    {
	    interfaceEditor_->setInterface(interface);
    }
    else
    {
        interfaceEditor_->clear();
    }
}

//-----------------------------------------------------------------------------
// Function: onConnectionSelected()
//-----------------------------------------------------------------------------
void MainWindow::onConnectionSelected( GraphicsConnection* connection ) {
	Q_ASSERT(connection);
    adHocEditor_->clear();
    addressEditor_->clear();
	instanceEditor_->clear();
	interfaceEditor_->clear();
	connectionEditor_->setConnection(connection);
}

//-----------------------------------------------------------------------------
// Function: updateMenuStrip()
//-----------------------------------------------------------------------------
void MainWindow::updateMenuStrip()
{
	TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());
	bool unlocked = doc != 0 && (!(doc->getFlags() & TabDocument::DOC_PROTECTION_SUPPORT) ||
		!doc->isProtected());

	bool isHWComp = false;
	ComponentEditor* ipEditor = dynamic_cast<ComponentEditor*>(doc);
	if (ipEditor) {
		isHWComp = ipEditor->isHWImplementation();
	}

	bool isHWDesign = dynamic_cast<HWDesignWidget*>(doc) != 0;

	actSave_->setEnabled(doc != 0 && doc->isModified());
	actSaveAs_->setEnabled(doc != 0);
	actPrint_->setEnabled(doc != 0 && (doc->getFlags() & TabDocument::DOC_PRINT_SUPPORT));

	// generation group is always visible when there is open editor but disabled when locked
	generationGroup_->setEnabled(unlocked);
	if (doc) {
		generationGroup_->setVisible(true);
	}
	else {
		generationGroup_->setVisible(false);
	}

	// if is hardware design then set all actions enabled
	if (isHWDesign) {
		actGenVHDL_->setEnabled(unlocked);
		actGenVHDL_->setVisible(true);

		actGenDocumentation_->setEnabled(unlocked);
		actGenDocumentation_->setVisible(true);
		
		actGenModelSim_->setEnabled(unlocked);
		actGenModelSim_->setVisible(true);
		
		actGenQuartus_->setEnabled(unlocked);
		actGenQuartus_->setVisible(true);
	}
	// if is hardware component then set only documentation, modelsim and vhdl enabled
	else if (isHWComp) {
		actGenVHDL_->setEnabled(unlocked);
		actGenVHDL_->setVisible(true);
		
		actGenDocumentation_->setEnabled(unlocked);
		actGenDocumentation_->setVisible(true);
		
		actGenModelSim_->setEnabled(unlocked);
		actGenModelSim_->setVisible(true);
       
		actGenQuartus_->setDisabled(true);
		actGenQuartus_->setVisible(false);
	}
	else {
		actGenVHDL_->setVisible(false);
		actGenDocumentation_->setVisible(false);
		actGenModelSim_->setVisible(false);
		actGenQuartus_->setVisible(false);
	}

	editGroup_->setVisible(doc != 0 && (doc->getFlags() & TabDocument::DOC_EDIT_SUPPORT));
	editGroup_->setEnabled(doc != 0 && (doc->getFlags() & TabDocument::DOC_EDIT_SUPPORT) && unlocked);
	actUndo_->setEnabled(doc != 0 && doc->getEditProvider() != 0 && doc->getEditProvider()->canUndo());
	actRedo_->setEnabled(doc != 0 && doc->getEditProvider() != 0 && doc->getEditProvider()->canRedo());

	diagramToolsGroup_->setVisible(doc != 0 && (doc->getFlags() & TabDocument::DOC_DRAW_MODE_SUPPORT));
	diagramToolsGroup_->setEnabled(doc != 0 && (doc->getFlags() & TabDocument::DOC_DRAW_MODE_SUPPORT) &&
		!doc->isProtected());
	actToolSelect_->setEnabled(doc != 0 && (doc->getSupportedDrawModes() & MODE_SELECT));
	actToolConnect_->setEnabled(doc != 0 && (doc->getSupportedDrawModes() & MODE_CONNECT));
	actToolInterface_->setEnabled(doc != 0 && (doc->getSupportedDrawModes() & MODE_INTERFACE));
	actToolDraft_->setEnabled(doc != 0 && (doc->getSupportedDrawModes() & MODE_DRAFT));
    actToolToggleOffPage_->setEnabled(doc != 0 && (doc->getSupportedDrawModes() & MODE_TOGGLE_OFFPAGE));
    actToolLabel_->setEnabled(doc != 0 && (doc->getSupportedDrawModes() & MODE_LABEL));

	bool oldProtectionState = actProtect_->isChecked();

	protectGroup_->setVisible(doc != 0 && (doc->getFlags() & TabDocument::DOC_PROTECTION_SUPPORT));
	actProtect_->setEnabled(doc != 0 && (doc->getFlags() & TabDocument::DOC_PROTECTION_SUPPORT));
	actProtect_->setChecked(doc != 0 && (doc->getFlags() & TabDocument::DOC_PROTECTION_SUPPORT) &&
		doc->isProtected());

	if (oldProtectionState != actProtect_->isChecked())
		onProtectionChanged(actProtect_->isChecked());

    actVisibilityControl_->setEnabled(doc != 0 && (doc->getFlags() & TabDocument::DOC_VISIBILITY_CONTROL_SUPPORT));

	 // Enable/disable the plugin generator actions based on the component being edited in the document.
	 VLNV compVLNV;
	 VLNV desVLNV;
	 QSharedPointer<LibraryComponent const> libComp;
	 QSharedPointer<LibraryComponent const> libDes;
	 QSharedPointer<LibraryComponent const> libDesConf;
	 if (doc != 0) {
		 compVLNV = doc->getDocumentVLNV();
		 desVLNV = doc->getIdentifyingVLNV();
		 libComp = libraryHandler_->getModelReadOnly(compVLNV);

		 // if the design is supported by the document type
		 DesignWidget* desWidget = qobject_cast<DesignWidget*>(doc);
		 if (desWidget) {

			 // the vlnvs must be for different objects
			 Q_ASSERT(compVLNV != desVLNV);

			 // design is the object that identifies the editor
			 libDes = libraryHandler_->getModelReadOnly(desVLNV);

			 // find the design config is one exists
			 QString viewName = desWidget->getOpenViewName();

			 QSharedPointer<Component const> comp = libComp.dynamicCast<Component const>();
			 VLNV desConfVLNV;

			 // the implementation defines where to search for the hierarchy ref
			 switch (desWidget->getImplementation()) {
			 case KactusAttribute::KTS_HW: {
				 desConfVLNV = comp->getHierRef(viewName);
				 break;
													 }
			 case KactusAttribute::KTS_SW: {
				 desConfVLNV = comp->getHierSWRef(viewName);
				 break;
													 }
			 case KactusAttribute::KTS_SYS: {
				 desConfVLNV = comp->getHierSystemRef(viewName);
				 break;
													  }
			 default: {
				 Q_ASSERT(false);
				 return;
						 }
			 }

			 // the hierarchy reference must be valid
			 Q_ASSERT(desConfVLNV.isValid());

			 // if the hierarchy ref is not directly to the design but design config is in between
			 if (desConfVLNV != desVLNV) {
				 libDesConf = libraryHandler_->getModelReadOnly(desConfVLNV);
			 }
		 }
	 }

    foreach (QAction* action, pluginActionGroup_->actions())
    {
        IGeneratorPlugin* plugin = reinterpret_cast<IGeneratorPlugin*>(action->data().value<void*>());
        Q_ASSERT(plugin != 0);

        action->setEnabled(libComp != 0 && plugin->checkGeneratorSupport(libComp, libDesConf, libDes));
    }

	updateZoomTools();
}

//-----------------------------------------------------------------------------
// Function: saveCurrentAs()
//-----------------------------------------------------------------------------
void MainWindow::saveCurrentAs() {
	TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());

	if (doc != 0)
	{
        // Validate the document before saving.
        QStringList errorList;

        if (!doc->validate(errorList))
        {
            // If the document contained errors, inform the user and give options to save or cancel.
            QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
                               tr("The document contained %1 error(s). The document can be saved but "
                                  "may not be opened with other IP-XACT tools. Continue save?").arg(errorList.size()),
                QMessageBox::Yes | QMessageBox::No, this);

            msgBox.setDetailedText("The document contained the following error(s):\n* " + errorList.join("\n* "));

            if (msgBox.exec() == QMessageBox::No)
            {
                return;
            }
        }

		if (doc->saveAs())
        {
		    onTabChanged(designTabs_->currentIndex());
		    doc->refresh();
        }
	}
}

//-----------------------------------------------------------------------------
// Function: saveCurrent()
//-----------------------------------------------------------------------------
void MainWindow::saveCurrent()
{
	TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());

	if (doc != 0)
	{
        // Validate the document before saving.
        QStringList errorList;

        if (!doc->validate(errorList))
        {
            // If the document contained errors, inform the user and give options to save or cancel.
            QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
                               tr("The document contained %1 error(s). The document can be saved but "
                                  "may not be opened with other IP-XACT tools. Continue save?").arg(errorList.size()),
                               QMessageBox::Yes | QMessageBox::No, this);

            msgBox.setDetailedText("The document contained the following error(s):\n* " + errorList.join("\n* "));

            if (msgBox.exec() == QMessageBox::No)
            {
                return;
            }
        }

		doc->save();
		onTabChanged(designTabs_->currentIndex());
	}
}

//-----------------------------------------------------------------------------
// Function: saveAll()
//-----------------------------------------------------------------------------
void MainWindow::saveAll()
{
	// Go through all documents and save those that have been modified.
	for (int i = 0; i < designTabs_->count(); ++i)
	{
		TabDocument* doc = static_cast<TabDocument*>(designTabs_->widget(i));

		if (doc->isModified())
		{
            // Validate the document before saving.
            QStringList errorList;

            if (!doc->validate(errorList))
            {
                // If the document contained errors, inform the user and give options to save or cancel.
                QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
                    tr("Document %1 contained %2 error(s). The document can be saved but "
                    "may not be opened with other IP-XACT tools. Continue save?").arg(doc->getDocumentName(), errorList.size()),
                    QMessageBox::Yes | QMessageBox::No, this);

                msgBox.setDetailedText("The document contained the following error(s):\n* " + errorList.join("\n* "));

                if (msgBox.exec() == QMessageBox::No)
                {
                    return;
                }
            }

			doc->save();
		}
	}

	if (designTabs_->currentWidget() != 0)
	{
		TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());
		actSave_->setEnabled(doc->isModified());
	}
	else
	{
		actSave_->setEnabled(false);
	}
}

//-----------------------------------------------------------------------------
// Function: printCurrent()
//-----------------------------------------------------------------------------
void MainWindow::printCurrent()
{
	TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());
	Q_ASSERT(doc != 0);

	doc->print();    
}

//-----------------------------------------------------------------------------
// Function: addColumn()
//-----------------------------------------------------------------------------
void MainWindow::addColumn()
{
	QWidget* curWidget = designTabs_->currentWidget();
	DesignWidget* designWidget = dynamic_cast<DesignWidget*>(curWidget);

	if (designWidget != 0)
	{
		designWidget->addColumn();
	}
}

//-----------------------------------------------------------------------------
// Function: generateVHDL()
//-----------------------------------------------------------------------------
void MainWindow::generateVHDL()
{
	HWDesignWidget* designWidget = dynamic_cast<HWDesignWidget*>(designTabs_->currentWidget());
	ComponentEditor* compEditor = dynamic_cast<ComponentEditor*>(designTabs_->currentWidget());

	if (designWidget != 0)
	{
		designWidget->onVhdlGenerate();
	}
	else if (compEditor) {

		// if user changed the metadata then the editor must be reopened
		if (compEditor->onVhdlGenerate()) {

			int index = designTabs_->currentIndex();
			VLNV compVLNV = compEditor->getDocumentVLNV();

			designTabs_->removeTab(index);
			delete compEditor;

			openComponent(compVLNV, true);
		}
	}
}

//-----------------------------------------------------------------------------
// Function: generateModelSim()
//-----------------------------------------------------------------------------
void MainWindow::generateModelSim() {
	HWDesignWidget* designWidget = dynamic_cast<HWDesignWidget*>(designTabs_->currentWidget());
	ComponentEditor* compEditor = dynamic_cast<ComponentEditor*>(designTabs_->currentWidget());

	if (designWidget != 0) {
		designWidget->onModelsimGenerate();
	}
	else if (compEditor) {

		// if user changed the contents of the metadata then refresh the editor
		if (compEditor->onModelsimGenerate()) {
			compEditor->refresh();
		}
	}

}

//-----------------------------------------------------------------------------
// Function: generateQuartus()
//-----------------------------------------------------------------------------
void MainWindow::generateQuartus()
{
	HWDesignWidget* designWidget = dynamic_cast<HWDesignWidget*>(designTabs_->currentWidget());

	if (designWidget != 0)
	{
		designWidget->onQuartusGenerate();
	}
}

//-----------------------------------------------------------------------------
// Function: runGeneratorPlugin()
//-----------------------------------------------------------------------------
void MainWindow::runGeneratorPlugin()
{
    TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());

    // Inform user that unsaved changes must be saved before continuing.
    if (doc->isModified())
    {
        QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
                           "The document " + doc->getDocumentName() + " has unsaved changes and needs to be "
                           "saved before generators can be run. Save and continue?",
                           QMessageBox::Yes | QMessageBox::No, this);

        if (msgBox.exec() == QMessageBox::No || !doc->save())
        {
            return;
        }
    }

    // Fill in the dialog with supported plugins.
	VLNV compVLNV = doc->getDocumentVLNV();
	VLNV desVLNV = doc->getIdentifyingVLNV();
	QSharedPointer<LibraryComponent> libComp = libraryHandler_->getModel(compVLNV);
	QSharedPointer<LibraryComponent> libDes;
	QSharedPointer<LibraryComponent> libDesConf;

	// if the design is supported by the document type
	DesignWidget* desWidget = qobject_cast<DesignWidget*>(doc);
	if (desWidget) {

		// the vlnvs must be for different objects
		Q_ASSERT(compVLNV != desVLNV);

		// design is the object that identifies the editor
		libDes = libraryHandler_->getModel(desVLNV);

		// find the design config is one exists
		QString viewName = desWidget->getOpenViewName();

		QSharedPointer<Component> comp = libComp.dynamicCast<Component>();
		VLNV desConfVLNV;

		// the implementation defines where to search for the hierarchy ref
		switch (desWidget->getImplementation()) {
		case KactusAttribute::KTS_HW: {
			desConfVLNV = comp->getHierRef(viewName);
			break;
						 }
		case KactusAttribute::KTS_SW: {
			desConfVLNV = comp->getHierSWRef(viewName);
			break;
						 }
		case KactusAttribute::KTS_SYS: {
			desConfVLNV = comp->getHierSystemRef(viewName);
			break;
						  }
		default: {
			Q_ASSERT(false);
			return;
					}
		}

		// the hierarchy reference must be valid
		Q_ASSERT(desConfVLNV.isValid());

		// if the hierarchy ref is not directly to the design but design config is in between
		if (desConfVLNV != desVLNV) {
			libDesConf = libraryHandler_->getModel(desConfVLNV);
		}
	}

    PluginListDialog dialog(this);

    foreach (IPlugin* plugin, pluginMgr_->getActivePlugins())
    {
        IGeneratorPlugin* genPlugin = dynamic_cast<IGeneratorPlugin*>(plugin);

        if (genPlugin != 0 && genPlugin->checkGeneratorSupport(libComp, libDesConf, libDes))
        {
            dialog.addPlugin(plugin);
        }
    }

    // Show the dialog and if the user pressed OK, run the selected plugin.
    if (dialog.exec() == QDialog::Accepted)
    {
        IPlugin* plugin = dialog.getSelectedPlugin();
        IGeneratorPlugin* genPlugin = dynamic_cast<IGeneratorPlugin*>(plugin);
        Q_ASSERT(genPlugin != 0);

        PluginUtilityAdapter adapter(libraryHandler_, this);

        connect(&adapter, SIGNAL(errorMessage(QString const&)), 
            this, SIGNAL(errorMessage(QString const&)), Qt::UniqueConnection);
        connect(&adapter, SIGNAL(infoMessage(QString const&)), 
            this, SIGNAL(noticeMessage(QString const&)), Qt::UniqueConnection);

        genPlugin->runGenerator(&adapter, libComp, libDesConf, libDes);
        doc->refresh();
    }
}

//-----------------------------------------------------------------------------
// Function: MainWindow::runGeneratorPlugin()
//-----------------------------------------------------------------------------
void MainWindow::runGeneratorPlugin(QAction* action)
{
    TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());

    // Inform user that unsaved changes must be saved before continuing.
    if (doc->isModified())
    {
        QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
            "The document " + doc->getDocumentName() + " has unsaved changes and needs to be "
            "saved before generators can be run. Save and continue?",
            QMessageBox::Yes | QMessageBox::No, this);

        if (msgBox.exec() == QMessageBox::No || !doc->save())
        {
            return;
        }
    }

    // Retrieve the library component.
	 VLNV compVLNV = doc->getDocumentVLNV();
	 VLNV desVLNV = doc->getIdentifyingVLNV();
	 QSharedPointer<LibraryComponent> libComp = libraryHandler_->getModel(compVLNV);
	 QSharedPointer<LibraryComponent> libDes;
	 QSharedPointer<LibraryComponent> libDesConf;

	 // if the design is supported by the document type
	 DesignWidget* desWidget = qobject_cast<DesignWidget*>(doc);
	 if (desWidget) {

		 // the vlnvs must be for different objects
		 Q_ASSERT(compVLNV != desVLNV);

		 // design is the object that identifies the editor
		 libDes = libraryHandler_->getModel(desVLNV);

		 // find the design config is one exists
		 QString viewName = desWidget->getOpenViewName();

		 QSharedPointer<Component> comp = libComp.dynamicCast<Component>();
		 VLNV desConfVLNV;

		 // the implementation defines where to search for the hierarchy ref
		 switch (desWidget->getImplementation()) {
		 case KactusAttribute::KTS_HW: {
			 desConfVLNV = comp->getHierRef(viewName);
			 break;
												 }
		 case KactusAttribute::KTS_SW: {
			 desConfVLNV = comp->getHierSWRef(viewName);
			 break;
												 }
		 case KactusAttribute::KTS_SYS: {
			 desConfVLNV = comp->getHierSystemRef(viewName);
			 break;
												  }
		 default: {
			 Q_ASSERT(false);
			 return;
					 }
		 }

		 // the hierarchy reference must be valid
		 Q_ASSERT(desConfVLNV.isValid());

		 // if the hierarchy ref is not directly to the design but design config is in between
		 if (desConfVLNV != desVLNV) {
			 libDesConf = libraryHandler_->getModel(desConfVLNV);
		 }
	 }

    // Retrieve the plugin pointer from the action.
    IGeneratorPlugin* plugin = reinterpret_cast<IGeneratorPlugin*>(action->data().value<void*>());
    Q_ASSERT(plugin != 0);

    PluginUtilityAdapter adapter(libraryHandler_, this);

    connect(&adapter, SIGNAL(errorMessage(QString const&)), 
        this, SIGNAL(errorMessage(QString const&)), Qt::UniqueConnection);
    connect(&adapter, SIGNAL(infoMessage(QString const&)), 
        this, SIGNAL(noticeMessage(QString const&)), Qt::UniqueConnection);

    // Run the generator and refresh the document.
    plugin->runGenerator(&adapter, libComp, libDesConf, libDes);
    doc->refresh();
}

//-----------------------------------------------------------------------------
// Function: generateDoc()
//-----------------------------------------------------------------------------
void MainWindow::generateDoc() {
	// get the vlnv of the current component
	TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());
	Q_ASSERT(doc);
	VLNV vlnv = doc->getDocumentVLNV();
	Q_ASSERT(vlnv.isValid());

	// if changes have been made to the component
	if (doc->isModified()) {
		QMessageBox::information(this, QCoreApplication::applicationName(), 
			tr("Changes have been made to the component. Save the changes before "
			   "generating documentation."));
		return;
	}

	DocumentGenerator generator(libraryHandler_, this);
	connect(&generator, SIGNAL(errorMessage(const QString&)),
		console_, SLOT(onErrorMessage(const QString&)), Qt::UniqueConnection);
	connect(&generator, SIGNAL(noticeMessage(const QString&)),
		console_, SLOT(onNoticeMessage(const QString&)), Qt::UniqueConnection);

	// set the component that's documentation is generated.
	generator.setComponent(vlnv);

	// write the documentation
	QString htmlFilePath = generator.writeDocumentation();

	// if user canceled
	if (htmlFilePath.isEmpty()) {
		return;
	}

	// open the generated document in user's default browser
	QDesktopServices::openUrl(QUrl::fromLocalFile(htmlFilePath));

	// if the editor was component editor then it must be refreshed to make the
	// changes to metadata visible
	ComponentEditor* compEditor = dynamic_cast<ComponentEditor*>(doc);

	if (compEditor) {

		// close the editor
		bool wasLocked = compEditor->isProtected();
		int index = designTabs_->currentIndex();
		designTabs_->removeTab(index);
		delete compEditor;

		// open the component again in the editor
		openComponent(vlnv, !wasLocked);
	}
}

//-----------------------------------------------------------------------------
// Function: zoomIn()
//-----------------------------------------------------------------------------
void MainWindow::zoomIn()
{
	TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());
	Q_ASSERT(doc->getFlags() & TabDocument::DOC_ZOOM_SUPPORT);
	doc->setZoomLevel(doc->getZoomLevel() + 10);
	updateZoomTools();
}

//-----------------------------------------------------------------------------
// Function: zoomOut()
//-----------------------------------------------------------------------------
void MainWindow::zoomOut()
{
	TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());
	Q_ASSERT(doc->getFlags() & TabDocument::DOC_ZOOM_SUPPORT);
	doc->setZoomLevel(doc->getZoomLevel() - 10);
	updateZoomTools();
}

//-----------------------------------------------------------------------------
// Function: zoomOriginal()
//-----------------------------------------------------------------------------
void MainWindow::zoomOriginal()
{
	TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());
	Q_ASSERT(doc->getFlags() & TabDocument::DOC_ZOOM_SUPPORT);
	doc->setZoomLevel(100);
	updateZoomTools();
}

//-----------------------------------------------------------------------------
// Function: fitInView()
//-----------------------------------------------------------------------------
void MainWindow::fitInView()
{
	TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());
	Q_ASSERT(doc->getFlags() & TabDocument::DOC_ZOOM_SUPPORT);
	doc->fitInView();
	updateZoomTools();
}

//-----------------------------------------------------------------------------
// Function: drawModeChange()
//-----------------------------------------------------------------------------
void MainWindow::drawModeChange(QAction *action)
{
	TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());

	if (action == actToolSelect_)
	{
		doc->setMode(MODE_SELECT);
	}
	else if (action == actToolConnect_)
	{
		doc->setMode(MODE_CONNECT);
	}
	else if (action == actToolInterface_)
	{
		doc->setMode(MODE_INTERFACE);
	}
	else if (action == actToolDraft_)
	{
		doc->setMode(MODE_DRAFT);
	}
    else if (action == actToolToggleOffPage_)
    {
        doc->setMode(MODE_TOGGLE_OFFPAGE);
    }
    else if (action == actToolLabel_)
    {
        doc->setMode(MODE_LABEL);
    }
}

//-----------------------------------------------------------------------------
// Function: onDrawModeChanged()
//-----------------------------------------------------------------------------
void MainWindow::onDrawModeChanged(DrawMode mode)
{
	TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());

    if (doc != 0 && (doc->getFlags() & TabDocument::DOC_DRAW_MODE_SUPPORT))
    {
        if (mode == MODE_SELECT)
        {
            doc->setCursor(Qt::ArrowCursor);
        }
        else
        {
            doc->setCursor(Qt::CrossCursor);
        }
    }	

    actToolSelect_->setChecked(mode == MODE_SELECT);
    actToolConnect_->setChecked(mode == MODE_CONNECT);
    actToolInterface_->setChecked(mode == MODE_INTERFACE);
    actToolDraft_->setChecked(mode == MODE_DRAFT);
    actToolToggleOffPage_->setChecked(mode == MODE_TOGGLE_OFFPAGE);
    actToolLabel_->setChecked(mode == MODE_LABEL);
}

void MainWindow::onTabCloseRequested( int index )
{	
	Q_ASSERT(designTabs_->widget(index) != 0);
	TabDocument* doc = static_cast<TabDocument*>(designTabs_->widget(index));

	// If the document has been modified, ask a confirmation from the user.
	if (doc->isModified())
	{
		QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
			"Do you want to save changes to " + doc->getDocumentName() + "?",
			QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, this);

		int ret = msgBox.exec();

		switch (ret)
		{
		case QMessageBox::Yes:
			{
				// Try to save and if it fails, cancel the close operation.
				if (!doc->save())
				{
					return;
				}
				break;
			}

		case QMessageBox::No:
			{
				doc->setModified(false);
				break;
			}

		case QMessageBox::Cancel:
			{
				// Cancel the close operation.
				return;
			}
		}
	}

    /*VLNV vlnv = doc->getIdentifyingVLNV();
    notesEditor_->onVLNVClosed(vlnv);*/

	// remove the widget from the tabs
	designTabs_->removeTab(index);

	// delete the document.
	delete doc;

	// if there are no more tabs in the tab widget
	if (designTabs_->count() == 0) {
        updateWindows();
	}
}

//-----------------------------------------------------------------------------
// Function: onTabChanged()
//-----------------------------------------------------------------------------
void MainWindow::onTabChanged(int index)
{
	TabDocument* doc = static_cast<TabDocument*>(designTabs_->widget(index));

	// update the menu 
	if (doc) {
		updateWindows();
        /*VLNV vlnv = doc->getIdentifyingVLNV();
        notesEditor_->onVLNVChanged(vlnv, libraryHandler_->getPath(vlnv));*/
	}

	// if the new tab is designWidget
	DesignWidget* designwidget = dynamic_cast<DesignWidget*>(doc);
	// set the configuration manager to edit the active design
	if (designwidget)
    {
		configurationEditor_->setConfiguration(designwidget);

        if (doc->getSupportedWindows() & TabDocument::SYSTEM_DETAILS_WINDOW)
        {
            systemDetailsEditor_->setSystem(designwidget);
        }
        else
        {
            systemDetailsEditor_->clear();
        }
	}
	// active tab is not design widget so clear the editors associated with design widget
	else {
		configurationEditor_->clear();
        systemDetailsEditor_->clear();
        addressEditor_->clear();
		instanceEditor_->clear();
        adHocEditor_->clear();
		interfaceEditor_->clear();
		connectionEditor_->clear();
	}

	// Reset the draw mode to select.
	if (doc != 0 && (doc->getFlags() & TabDocument::DOC_DRAW_MODE_SUPPORT))
	{
		doc->setMode(MODE_SELECT);
	}
	else
	{
		onDrawModeChanged(MODE_SELECT);
	}

	// Update the menu strip.
	updateMenuStrip();
}

//-----------------------------------------------------------------------------
// Function: closeEvent()
//-----------------------------------------------------------------------------
void MainWindow::closeEvent(QCloseEvent* event)
{
	// disconnect the signals that would otherwise change the window states to be saved
	disconnectVisibilityControls();

	// Go through all tab documents and ask the user what to do if they are not saved.
	while (designTabs_->count() > 0)
	{
		TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());

		if (doc->isModified())
		{
			QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
				"Do you want to save changes to " + doc->getDocumentName() + "?",
				QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, this);

			int ret = msgBox.exec();

			switch (ret)
			{
			case QMessageBox::Yes:
				{
					// Try to save and if it fails, cancel the close event.
					if (!doc->save())
					{
						event->ignore();
						return;
					}
					break;
				}

			case QMessageBox::No:
				{
					doc->setModified(false);
					break;
				}

			case QMessageBox::Cancel:
				{
					event->ignore();
					return;
				}
			}
		}

		designTabs_->removeTab(designTabs_->currentIndex());
		delete doc;
		doc = 0;
	}
}

//-----------------------------------------------------------------------------
// Function: updateZoomTools()
//-----------------------------------------------------------------------------
void MainWindow::updateZoomTools()
{
	TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());
	bool canZoom = doc != 0 && (doc->getFlags() & TabDocument::DOC_ZOOM_SUPPORT);

	actZoomIn_->setEnabled(canZoom && doc->getZoomLevel() < doc->getMaxZoomLevel());
	actZoomOut_->setEnabled(canZoom && doc->getZoomLevel() > doc->getMinZoomLevel());
	actZoomOriginal_->setEnabled(canZoom);
	actFitInView_->setEnabled(canZoom);
}

//-----------------------------------------------------------------------------
// Function: createNew()
//-----------------------------------------------------------------------------
void MainWindow::createNew()
{
    // Create a property page dialog to work as a "New" dialog.
	PropertyPageDialog dialog(QSize(48, 48), 1, PropertyPageDialog::VIEW_ICONS,
                              PropertyPageDialog::APPLY_CURRENT, this);
	dialog.setFixedWidth(620);
	//dialog.resize(620, 690);
	dialog.setWindowTitle(tr("New"));

    // Add pages to the dialog.
	NewComponentPage* compPage = new NewComponentPage(libraryHandler_, &dialog);
	connect(compPage, SIGNAL(createComponent(KactusAttribute::ProductHierarchy,
		KactusAttribute::Firmness,
		VLNV const&, QString const&)),
		this, SLOT(createComponent(KactusAttribute::ProductHierarchy,
		KactusAttribute::Firmness,
		VLNV const&, QString const&)));
	dialog.addPage(QIcon(":icons/common/graphics/hw-component.png"), tr("HW Component"), compPage);

	NewDesignPage* designPage = new NewDesignPage(libraryHandler_, &dialog);
	connect(designPage, SIGNAL(createDesign(KactusAttribute::ProductHierarchy,
		KactusAttribute::Firmness,
		VLNV const&, QString const&)),
		this, SLOT(createDesign(KactusAttribute::ProductHierarchy,
		KactusAttribute::Firmness,
		VLNV const&, QString const&)));
	dialog.addPage(QIcon(":icons/common/graphics/hw-design.png"), tr("HW Design"), designPage);

	NewSWComponentPage* swCompPage = new NewSWComponentPage(libraryHandler_, &dialog);
	connect(swCompPage, SIGNAL(createSWComponent(VLNV const&, QString const&)),
		this, SLOT(createSWComponent(VLNV const&, QString const&)));
	dialog.addPage(QIcon(":icons/common/graphics/sw-component48x48.png"), tr("SW Component"), swCompPage);

	NewSWDesignPage* swDesignPage = new NewSWDesignPage(libraryHandler_, &dialog);
	connect(swDesignPage, SIGNAL(createSWDesign(VLNV const&, QString const&)),
		    this, SLOT(createSWDesign(VLNV const&, QString const&)), Qt::UniqueConnection);
	dialog.addPage(QIcon(":icons/common/graphics/sw-design48x48.png"), tr("SW Design"), swDesignPage);

	NewSystemPage* sysPage = new NewSystemPage(libraryHandler_, &dialog);
	connect(sysPage, SIGNAL(createSystem(VLNV const&, QString const&, VLNV const&, QString const&)),
		this, SLOT(createSystem(VLNV const&, QString const&, VLNV const&, QString const&)));
	dialog.addPage(QIcon(":icons/common/graphics/system-component.png"), tr("System"), sysPage);

	NewBusPage* busPage = new NewBusPage(libraryHandler_, &dialog);
	connect(busPage, SIGNAL(createBus(VLNV const&, QString const&)),
		this, SLOT(createBus(VLNV const&, QString const&)), Qt::UniqueConnection);
	dialog.addPage(QIcon(":icons/common/graphics/new-bus.png"), tr("Bus"), busPage);

    NewComDefinitionPage* comDefPage = new NewComDefinitionPage(libraryHandler_, &dialog);
    connect(comDefPage, SIGNAL(createComDefinition(VLNV const&, QString const&)),
            this, SLOT(createComDefinition(VLNV const&, QString const&)), Qt::UniqueConnection);
    dialog.addPage(QIcon(":icons/common/graphics/new-com_definition.png"), tr("COM Definition"), comDefPage);

    NewApiDefinitionPage* apiDefPage = new NewApiDefinitionPage(libraryHandler_, &dialog);
    connect(apiDefPage, SIGNAL(createApiDefinition(VLNV const&, QString const&)),
        this, SLOT(createApiDefinition(VLNV const&, QString const&)), Qt::UniqueConnection);
    dialog.addPage(QIcon(":icons/common/graphics/new-api_definition.png"), tr("API Definition"), apiDefPage);

    dialog.finalizePages();

	dialog.exec();
}

//-----------------------------------------------------------------------------
// Function: createComponent()
//-----------------------------------------------------------------------------
void MainWindow::createComponent(KactusAttribute::ProductHierarchy prodHier,
								 KactusAttribute::Firmness firmness,
								 VLNV const& vlnv, QString const& directory)
{
	Q_ASSERT(vlnv.isValid());

	// Create a component.
	QSharedPointer<Component> component = QSharedPointer<Component>(new Component());
	component->setVlnv(vlnv);

	// Set Kactus attributes.
	component->setComponentHierarchy(prodHier);
	component->setComponentFirmness(firmness);
	component->setComponentImplementation(KactusAttribute::KTS_HW);
	component->createEmptyFlatView();

    // Create the file.
    if (!libraryHandler_->writeModelToFile(directory, component))
    {
        emit errorMessage("Error saving file to disk.");
        return;
    }

    // Open the component wizard.
    ComponentWizard wizard(component, directory, *pluginMgr_, libraryHandler_, this);
    wizard.exec();

//     VHDLtoIPXACT editor(component, directory, this);
//     editor.exec();

	// Save wizard changes.
	if (!libraryHandler_->writeModelToFile(component))
    {
        emit errorMessage("Error saving file to disk.");
        return;
    }

	// Open the component editor.
	openComponent(vlnv, true);
}

//-----------------------------------------------------------------------------
// Function: createDesign()
//-----------------------------------------------------------------------------
void MainWindow::createDesign(KactusAttribute::ProductHierarchy prodHier,
							  KactusAttribute::Firmness firmness,
							  VLNV const& vlnv, QString const& directory)
{
	Q_ASSERT(vlnv.isValid());

	VLNV designVLNV(VLNV::DESIGN, vlnv.getVendor(), vlnv.getLibrary(),
		vlnv.getName().remove(".comp") + ".design", vlnv.getVersion());
	VLNV desConfVLNV(VLNV::DESIGNCONFIGURATION, vlnv.getVendor(), vlnv.getLibrary(),
		vlnv.getName().remove(".comp") + ".designcfg", vlnv.getVersion());

	// Create a component and a hierarchical view .
	QSharedPointer<Component> component(new Component(vlnv));

	// Set Kactus attributes.
	component->setComponentHierarchy(prodHier);
	component->setComponentFirmness(firmness);
	component->setComponentImplementation(KactusAttribute::KTS_HW);

	component->createHierarchicalView(desConfVLNV);
	QStringList viewNames = component->getHierViews();
	Q_ASSERT(!viewNames.isEmpty());

	// Create the design and design configuration.
	QSharedPointer<Design> design(new Design(designVLNV));
    design->setDesignImplementation(KactusAttribute::KTS_HW);

	QSharedPointer<DesignConfiguration> designConf(new DesignConfiguration(desConfVLNV));
	designConf->setDesignRef(designVLNV);
    designConf->setDesignConfigImplementation(KactusAttribute::KTS_HW);

	// Create the files.
	libraryHandler_->beginSave();

    bool success = true;

	if (!libraryHandler_->writeModelToFile(directory, designConf))
    {
        success = false;
    }

	if (!libraryHandler_->writeModelToFile(directory, design))
    {
        success = false;
    }

	if (!libraryHandler_->writeModelToFile(directory, component))
    {
        success = false;
    }

	libraryHandler_->endSave();

    if (success)
    {
	    // Open the design.
	    openDesign(vlnv, viewNames.first(), true);
    }
    else
    {
        emit errorMessage("Error saving files to disk.");
    }
}

//-----------------------------------------------------------------------------
// Function: MainWindow::createDesignForExistingComponent()
//-----------------------------------------------------------------------------
void MainWindow::createDesignForExistingComponent(VLNV const& vlnv)
{
    Q_ASSERT(libraryHandler_->contains(vlnv));
    Q_ASSERT(libraryHandler_->getDocumentType(vlnv) == VLNV::COMPONENT);

    // Retrieve the component to which the design will be created.
    QSharedPointer<LibraryComponent> libComp = libraryHandler_->getModel(vlnv);
    QSharedPointer<Component> component = libComp.staticCast<Component>();

    // Ask the user the VLNV, target path and view name.
    NewDesignDialog dialog(libraryHandler_, component, KactusAttribute::KTS_HW, this);
    dialog.setVLNV(VLNV(VLNV::DESIGN, component->getVlnv()->getVendor(), component->getVlnv()->getLibrary(), "", ""));

    QSettings settings;
    QStringList suggestions = settings.value("Policies/HWViewNames").toStringList();
    dialog.setViewNameSuggestions(suggestions);

    QString baseViewName = "";
    QString viewName = "";

    if (!suggestions.isEmpty())
    {
        baseViewName = suggestions.first();

        viewName = baseViewName;
        unsigned int runningNumber = 1;

        while (component->findView(viewName) != 0)
        {
            ++runningNumber;
            viewName = baseViewName + QString::number(runningNumber);
        }
    }

    dialog.setViewName(viewName);

    if (dialog.exec() == QDialog::Rejected)
    {
        return;
    }

    // Create the view.
    View* view = new View(dialog.getViewName());
    view->setHierarchyRef(dialog.getDesignConfVLNV());
    view->addEnvIdentifier("::");
    component->addView(view);

    // Create the design and design configuration objects.
    QSharedPointer<DesignConfiguration> designConf(new DesignConfiguration(dialog.getDesignConfVLNV()));
    designConf->setDesignRef(dialog.getDesignVLNV());
    designConf->setDesignConfigImplementation(KactusAttribute::KTS_HW);

    QSharedPointer<Design> newDesign = QSharedPointer<Design>(new Design(dialog.getDesignVLNV()));
    newDesign->setDesignImplementation(KactusAttribute::KTS_HW);
    QList<ColumnDesc> columns;

    columns.append(ColumnDesc("IO", COLUMN_CONTENT_IO, 0, HWDesignDiagram::IO_COLUMN_WIDTH));
    columns.append(ColumnDesc("Buses", COLUMN_CONTENT_BUSES, 0, HWDesignDiagram::COMPONENT_COLUMN_WIDTH));
    columns.append(ColumnDesc("Components", COLUMN_CONTENT_COMPONENTS, 0, HWDesignDiagram::COMPONENT_COLUMN_WIDTH));
    columns.append(ColumnDesc("IO", COLUMN_CONTENT_IO, 0, HWDesignDiagram::IO_COLUMN_WIDTH));

    newDesign->setColumns(columns);

    libraryHandler_->beginSave();

    bool success = true;

    if (!libraryHandler_->writeModelToFile(dialog.getPath(), newDesign))
    {
        success = false;
    }

    if (!libraryHandler_->writeModelToFile(dialog.getPath(), designConf))
    {
        success = false;
    }

    if (!libraryHandler_->writeModelToFile(component))
    {
        success = false;
    }

    libraryHandler_->endSave();

    if (success)
    {
        // Open the design.
        openDesign(vlnv, view->getName(), true);
    }
    else
    {
        emit errorMessage("Error saving files to disk.");
    }
}

//-----------------------------------------------------------------------------
// Function: MainWindow::createSWDesign()
//-----------------------------------------------------------------------------
void MainWindow::createSWDesign(VLNV const& vlnv, QString const& directory)
{
    Q_ASSERT(vlnv.isValid());

    VLNV designVLNV(VLNV::DESIGN, vlnv.getVendor(), vlnv.getLibrary(),
        vlnv.getName().remove(".comp") + ".swdesign", vlnv.getVersion());
    VLNV desConfVLNV(VLNV::DESIGNCONFIGURATION, vlnv.getVendor(), vlnv.getLibrary(),
        vlnv.getName().remove(".comp") + ".swdesigncfg", vlnv.getVersion());

    // Create a component and a hierarchical view .
    QSharedPointer<Component> component(new Component(vlnv));

    // Set Kactus attributes.
    component->setComponentImplementation(KactusAttribute::KTS_SW);

    // Create the view.
    SWView* view = new SWView("software");
    view->setHierarchyRef(desConfVLNV);
    component->addSWView(view);

    // Create the design and design configuration.
    QSharedPointer<Design> design(new Design(designVLNV));
    design->setDesignImplementation(KactusAttribute::KTS_SW);
    QSharedPointer<DesignConfiguration> designConf(new DesignConfiguration(desConfVLNV));
    designConf->setDesignRef(designVLNV);
    designConf->setDesignConfigImplementation(KactusAttribute::KTS_SW);

    QList<ColumnDesc> columns;

    columns.append(ColumnDesc("Low-level", COLUMN_CONTENT_COMPONENTS, 0, SystemDesignDiagram::SW_COLUMN_WIDTH));
    columns.append(ColumnDesc("Middle-level", COLUMN_CONTENT_COMPONENTS, 0, SystemDesignDiagram::SW_COLUMN_WIDTH));
    columns.append(ColumnDesc("High-level", COLUMN_CONTENT_COMPONENTS, 0, SystemDesignDiagram::SW_COLUMN_WIDTH));
    columns.append(ColumnDesc("Out", COLUMN_CONTENT_IO, 0, SystemDesignDiagram::IO_COLUMN_WIDTH));
    design->setColumns(columns);

    // Create the files.
    libraryHandler_->beginSave();

    bool success = true;

    if (!libraryHandler_->writeModelToFile(directory, designConf))
    {
        success = false;
    }

    if (!libraryHandler_->writeModelToFile(directory, design))
    {
        success = false;
    }

    if (!libraryHandler_->writeModelToFile(directory, component))
    {
        success = false;
    }

    libraryHandler_->endSave();

    if (success)
    {
        // Open the design.
        openSWDesign(vlnv, component->getSWViewNames().first(), true);
    }
    else
    {
        emit errorMessage("Error saving files to disk.");

    }
}

//-----------------------------------------------------------------------------
// Function: MainWindow::createSWDesign()
//-----------------------------------------------------------------------------
void MainWindow::createSWDesign(VLNV const& vlnv)
{
    Q_ASSERT(libraryHandler_->contains(vlnv));
    Q_ASSERT(libraryHandler_->getDocumentType(vlnv) == VLNV::COMPONENT);

    // Retrieve the component to which the SW design will be created.
    QSharedPointer<LibraryComponent> libComp = libraryHandler_->getModel(vlnv);
    QSharedPointer<Component> component = libComp.staticCast<Component>();

    // Ask the user the VLNV, target path and view name.
    NewDesignDialog dialog(libraryHandler_, component, KactusAttribute::KTS_SW, this);
    dialog.setWindowTitle(tr("New SW Design"));
    dialog.setVLNV(VLNV(VLNV::DESIGN, component->getVlnv()->getVendor(), component->getVlnv()->getLibrary(), "", ""));

    QSettings settings;
    QStringList suggestions = settings.value("Policies/SWViewNames").toStringList();
    dialog.setViewNameSuggestions(suggestions);

    QString baseViewName = "";
    QString viewName = "";

    if (!suggestions.isEmpty())
    {
        baseViewName = suggestions.first();

        viewName = baseViewName;
        unsigned int runningNumber = 1;

        while (component->findSWView(viewName) != 0)
        {
            ++runningNumber;
            viewName = baseViewName + QString::number(runningNumber);
        }
    }

    dialog.setViewName(viewName);

    if (dialog.exec() == QDialog::Rejected)
    {
        return;
    }

    // Create the view.
    SWView* view = new SWView(dialog.getViewName());
    view->setHierarchyRef(dialog.getDesignConfVLNV());   
    component->addSWView(view);

    // Create the design and design configuration objects.
    QSharedPointer<DesignConfiguration> designConf(new DesignConfiguration(dialog.getDesignConfVLNV()));
    designConf->setDesignRef(dialog.getDesignVLNV());
    designConf->setDesignConfigImplementation(KactusAttribute::KTS_SW);

    QSharedPointer<Design> newDesign(new Design(dialog.getDesignVLNV()));
    newDesign->setDesignImplementation(KactusAttribute::KTS_SW);
    QList<ColumnDesc> columns;

    if (component->getComponentImplementation() == KactusAttribute::KTS_SW)
    {
        columns.append(ColumnDesc("Low-level", COLUMN_CONTENT_COMPONENTS, 0, SystemDesignDiagram::SW_COLUMN_WIDTH));
        columns.append(ColumnDesc("Middle-level", COLUMN_CONTENT_COMPONENTS, 0, SystemDesignDiagram::SW_COLUMN_WIDTH));
        columns.append(ColumnDesc("High-level", COLUMN_CONTENT_COMPONENTS, 0, SystemDesignDiagram::SW_COLUMN_WIDTH));
        columns.append(ColumnDesc("Out", COLUMN_CONTENT_IO, 0, SystemDesignDiagram::IO_COLUMN_WIDTH));
    }
    else
    {
        columns.append(ColumnDesc("Pre-mapped SW", COLUMN_CONTENT_COMPONENTS, 0, SystemDesignDiagram::SW_COLUMN_WIDTH));
        columns.append(ColumnDesc("Pre-mapped SW", COLUMN_CONTENT_COMPONENTS, 0, SystemDesignDiagram::SW_COLUMN_WIDTH));
    }

    newDesign->setColumns(columns);

    libraryHandler_->beginSave();

    bool success = true;

    if (!libraryHandler_->writeModelToFile(dialog.getPath(), newDesign))
    {
        success = false;
    }

    if (!libraryHandler_->writeModelToFile(dialog.getPath(), designConf))
    {
        success = false;
    }

    if (!libraryHandler_->writeModelToFile(component))
    {
        success = false;
    }

    libraryHandler_->endSave();

    if (success)
    {
        // Open the design.
        openSWDesign(vlnv, view->getName(), true);
    }
    else
    {
        emit errorMessage("Error saving files to disk.");
    }
}

//-----------------------------------------------------------------------------
// Function: openSettings()
//-----------------------------------------------------------------------------
void MainWindow::openSettings()
{
	SettingsDialog dialog(*pluginMgr_, this);

	connect(&dialog, SIGNAL(scanLibrary()),
		this, SLOT(onLibrarySearch()), Qt::UniqueConnection);

	if (dialog.exec() == QDialog::Accepted)
	{
		QSettings settings;

		// Apply the settings to the open documents.
		for (int i = 0; i < designTabs_->count(); ++i)
		{
			TabDocument* doc = static_cast<TabDocument*>(designTabs_->widget(i));
			Q_ASSERT(doc != 0);

			doc->applySettings(settings);
		}

        // Update the generator plugin actions.
        updateGeneratorPluginActions();
	}
}

//-----------------------------------------------------------------------------
// Function: createSystem()
//-----------------------------------------------------------------------------
void MainWindow::createSystem(VLNV const& compVLNV, QString const& viewName,
							  VLNV const& sysVLNV, QString const& directory)
{
	Q_ASSERT(sysVLNV.isValid());

	libraryHandler_->beginSave();

	VLNV designVLNV(VLNV::DESIGN, sysVLNV.getVendor(), sysVLNV.getLibrary(),
		sysVLNV.getName().remove(".comp") + ".sysdesign", sysVLNV.getVersion());
	VLNV desConfVLNV(VLNV::DESIGNCONFIGURATION, sysVLNV.getVendor(), sysVLNV.getLibrary(),
		sysVLNV.getName().remove(".comp") + ".sysdesigncfg", sysVLNV.getVersion());

    QSharedPointer<Component> parentComp;

    // Check if mapping information was given.
    if (compVLNV.isValid())
    {
        // Retrieve the component to which the system design will be based on.
        parentComp = libraryHandler_->getModel(compVLNV).staticCast<Component>();
        Q_ASSERT(parentComp != 0);
    }
    else
    {
        // Otherwise create a system component to encapsulate the system design.
        parentComp = QSharedPointer<Component>(new Component(sysVLNV));
        parentComp->setComponentHierarchy(KactusAttribute::KTS_PRODUCT);
        parentComp->setComponentFirmness(KactusAttribute::KTS_FIXED);
        parentComp->setComponentImplementation(KactusAttribute::KTS_SYS);
    }

    // Create the system view to the system design.
    QString sysViewName = "system";
    unsigned int runningNumber = 1;

    while (parentComp->findSystemView(sysViewName) != 0)
    {
        ++runningNumber;
        sysViewName = "system" + QString::number(runningNumber);
    }

    SystemView* systemView = new SystemView(sysViewName);
    systemView->setHierarchyRef(desConfVLNV);
    systemView->setHWViewRef(viewName);

	parentComp->addSystemView(systemView);

	// Flat-out the hierarchy to form the system design.
	QSharedPointer<Design> sysDesign(new Design(designVLNV));
    sysDesign->setDesignImplementation(KactusAttribute::KTS_SYS);

    QList<ColumnDesc> columns;
    columns.append(ColumnDesc("SW Components", COLUMN_CONTENT_COMPONENTS, 0, SystemDesignDiagram::SYSTEM_COLUMN_WIDTH));
    columns.append(ColumnDesc("SW Components", COLUMN_CONTENT_COMPONENTS, 0, SystemDesignDiagram::SYSTEM_COLUMN_WIDTH));
    sysDesign->setColumns(columns);
    
	generateSystemDesignV2(libraryHandler_, parentComp->getHierRef(viewName), *sysDesign);

	// Create the design configuration.
	QSharedPointer<DesignConfiguration> designConf(new DesignConfiguration(desConfVLNV));
	designConf->setDesignRef(designVLNV);
    designConf->setDesignConfigImplementation(KactusAttribute::KTS_SYS);

	// Create the files.
    bool success = true;

	if (!libraryHandler_->writeModelToFile(directory, designConf))
    {
        success = false;
    }

    if (!libraryHandler_->writeModelToFile(directory, sysDesign))
    {
        success = false;
    }

    if (compVLNV.isValid())
    {
        if (!libraryHandler_->writeModelToFile(parentComp))
        {
            success = false;
        }
    }
    else
    {
        if (!libraryHandler_->writeModelToFile(directory, parentComp))
        {
            success = false;
        }
    }

	libraryHandler_->endSave();

    if (success)
    {
	    openSystemDesign(*parentComp->getVlnv(), sysViewName, true);
    }
    else
    {
        emit errorMessage("Error saving files to disk.");
    }
}

//-----------------------------------------------------------------------------
// Function: MainWindow::createSWDesign()
//-----------------------------------------------------------------------------
void MainWindow::createSystemDesign(VLNV const& vlnv)
{
    Q_ASSERT(libraryHandler_->contains(vlnv));
    Q_ASSERT(libraryHandler_->getDocumentType(vlnv) == VLNV::COMPONENT);

    libraryHandler_->beginSave();

    // Retrieve the component to which the system design will be created.
    QSharedPointer<LibraryComponent> libComp = libraryHandler_->getModel(vlnv);
    QSharedPointer<Component> component = libComp.staticCast<Component>();

    // Ask the user the VLNV, target path and view name.
    NewDesignDialog dialog(libraryHandler_, component, KactusAttribute::KTS_SYS, this);
    dialog.setWindowTitle(tr("New System Design"));
    dialog.setVLNV(VLNV(VLNV::DESIGN, component->getVlnv()->getVendor(), component->getVlnv()->getLibrary(), "", ""));

    QSettings settings;
    QStringList suggestions = settings.value("Policies/SysViewNames").toStringList();
    dialog.setViewNameSuggestions(suggestions);

    QString baseViewName = "";
    QString viewName = "";

    if (!suggestions.isEmpty())
    {
        baseViewName = suggestions.first();

        viewName = baseViewName;
        unsigned int runningNumber = 1;

        while (component->findSystemView(viewName) != 0)
        {
            ++runningNumber;
            viewName = baseViewName + QString::number(runningNumber);
        }
    }

    dialog.setViewName(viewName);
    
    if (dialog.exec() == QDialog::Rejected)
    {
        return;
    }

    // Create the view.
    SystemView* view = new SystemView(dialog.getViewName());
    view->setHierarchyRef(dialog.getDesignConfVLNV());   
    view->setHWViewRef(component->getHierViews().first());
    component->addSystemView(view);

    // Create the design and design configuration objects to the same folder as the component.
    QSharedPointer<DesignConfiguration> designConf(new DesignConfiguration(dialog.getDesignConfVLNV()));
    designConf->setDesignRef(dialog.getDesignVLNV());
    designConf->setDesignConfigImplementation(KactusAttribute::KTS_SYS);

    QSharedPointer<Design> newDesign = QSharedPointer<Design>(new Design(dialog.getDesignVLNV()));
    newDesign->setDesignImplementation(KactusAttribute::KTS_SYS);

    QList<ColumnDesc> columns;
    columns.append(ColumnDesc("SW Components", COLUMN_CONTENT_COMPONENTS, 0, SystemDesignDiagram::SYSTEM_COLUMN_WIDTH));
    columns.append(ColumnDesc("SW Components", COLUMN_CONTENT_COMPONENTS, 0, SystemDesignDiagram::SYSTEM_COLUMN_WIDTH));
    newDesign->setColumns(columns);

    generateSystemDesignV2(libraryHandler_, component->getHierRef(), *newDesign);

    bool success = true;

    if (!libraryHandler_->writeModelToFile(dialog.getPath(), newDesign))
    {
        success = false;
    }

    if (!libraryHandler_->writeModelToFile(dialog.getPath(), designConf))
    {
        success = false;
    }

    if (!libraryHandler_->writeModelToFile(component))
    {
        success = false;
    }

    libraryHandler_->endSave();

    if (success)
    {
        // Open the design.
        QString viewName = view->getName();
        openSystemDesign(vlnv, viewName, true);
    }
    else
    {
        emit errorMessage("Error saving files to disk.");
    }
}

//-----------------------------------------------------------------------------
// Function: createBus()
//-----------------------------------------------------------------------------
void MainWindow::createBus( VLNV const& vlnv, QString const& directory ) {

	Q_ASSERT(vlnv.isValid());
	Q_ASSERT(!directory.isEmpty());

	// create the vlnvs that identify the bus definition and abstraction definition
	VLNV busVLNV = vlnv;
	busVLNV.setType(VLNV::BUSDEFINITION);

	Q_ASSERT(!libraryHandler_->contains(busVLNV));

	VLNV absVLNV = busVLNV;

	// remove the possible .busDef from the end of the name field
	QString absDefName = busVLNV.getName();
	absDefName = absDefName.remove(".busDef", Qt::CaseInsensitive);

	absVLNV.setName(absDefName + ".absDef");
	absVLNV.setType(VLNV::ABSTRACTIONDEFINITION);

	// by default the abs def and bus def are saved to same directory
	QString absDirectory = directory;

	if (libraryHandler_->contains(absVLNV)) {
		VLNV newAbsDefVLNV;

		if (!NewObjectDialog::saveAsDialog(this, libraryHandler_, absVLNV, newAbsDefVLNV, absDirectory,
			"Set VLNV for abstraction definition")) {
				// if user canceled
				return;
		}
		// save the created abstraction definition vlnv
		absVLNV = newAbsDefVLNV;
	}

	// Create a bus definition.
	QSharedPointer<BusDefinition> busDef = QSharedPointer<BusDefinition>(new BusDefinition());
	busDef->setVlnv(busVLNV);

	// Create the file for the bus definition.
    bool success = true;

    if (!libraryHandler_->writeModelToFile(directory, busDef))
    {
        success = false;
    }

	// create an abstraction definition
	QSharedPointer<AbstractionDefinition> absDef = QSharedPointer<AbstractionDefinition>(new AbstractionDefinition());
	absDef->setVlnv(absVLNV);

	// set reference from abstraction definition to bus definition
	absDef->setBusType(busVLNV);

	// create the file for the abstraction definition
    if (!libraryHandler_->writeModelToFile(absDirectory, absDef))
    {
        success = false;
    }

    if (success)
    {
	    // Open the bus editor.
	    openBus(busVLNV, absVLNV, false, true);
    }
    else
    {
        emit errorMessage("Error saving files to disk.");
    }
}

void MainWindow::createAbsDef( const VLNV& busDefVLNV, const QString& directory, bool disableBusDef ) {

	Q_ASSERT(busDefVLNV.isValid());
	Q_ASSERT(!directory.isEmpty());

	VLNV absVLNV = busDefVLNV;

	// remove the possible .busDef from the end of the name field
	QString absDefName = busDefVLNV.getName();
	absDefName = absDefName.remove(".busDef", Qt::CaseInsensitive);

	absVLNV.setName(absDefName + ".absDef");
	absVLNV.setType(VLNV::ABSTRACTIONDEFINITION);

	// by default the abs def and bus def are saved to same directory
	QString absDirectory = directory;

	if (libraryHandler_->contains(absVLNV)) {
		VLNV newAbsDefVLNV;

		if (!NewObjectDialog::saveAsDialog(this, libraryHandler_, absVLNV, newAbsDefVLNV, absDirectory,
			"Set VLNV for abstraction definition")) {
				// if user canceled
				return;
		}
		// save the created abstraction definition vlnv
		absVLNV = newAbsDefVLNV;
	}

	// create an abstraction definition
	QSharedPointer<AbstractionDefinition> absDef = QSharedPointer<AbstractionDefinition>(new AbstractionDefinition());
	absDef->setVlnv(absVLNV);

	// set reference from abstraction definition to bus definition
	absDef->setBusType(busDefVLNV);

	// create the file for the abstraction definition
    if (!libraryHandler_->writeModelToFile(absDirectory, absDef))
    {
        emit errorMessage("Error saving files to disk.");
        return;
    }

	// Open the bus editor.
	openBus(busDefVLNV, absVLNV, disableBusDef, true);
}

//-----------------------------------------------------------------------------
// Function: MainWindow::createComDefinition()
//-----------------------------------------------------------------------------
void MainWindow::createComDefinition(VLNV const& vlnv, QString const& directory)
{
    Q_ASSERT(vlnv.isValid());
    Q_ASSERT(!directory.isEmpty());

    // Create an empty COM definition and save it.
    QSharedPointer<ComDefinition> comDef(new ComDefinition(vlnv));

    if (!libraryHandler_->writeModelToFile(directory, comDef))
    {
        emit errorMessage("Error saving files to disk.");
        return;
    }

    // Open the COM definition.
    openComDefinition(vlnv, true);
}

//-----------------------------------------------------------------------------
// Function: MainWindow::createApiDefinition()
//-----------------------------------------------------------------------------
void MainWindow::createApiDefinition(VLNV const& vlnv, QString const& directory)
{
    Q_ASSERT(vlnv.isValid());
    Q_ASSERT(!directory.isEmpty());

    VLNV apiDefVLNV = vlnv;
    //apiDefVLNV.setName(vlnv.getName() + ".apiDef");

    // Create an empty API definition and save it.
    QSharedPointer<ApiDefinition> apiDef(new ApiDefinition(apiDefVLNV));

    if (!libraryHandler_->writeModelToFile(directory, apiDef))
    {
        emit errorMessage("Error saving files to disk.");
        return;
    }

    // Open the API definition.
    openApiDefinition(apiDefVLNV, true);
}

void MainWindow::openBus(const VLNV& busDefVLNV, const VLNV& absDefVLNV, bool disableBusDef, bool forceUnlocked ) {

	if (isOpen(absDefVLNV)) {
		return;
	}
	else if (isOpen(busDefVLNV)) {
		return;
	}

	// Check if the bus editor is already open and activate it.
	if (busDefVLNV.isValid()) {
		for (int i = 0; i < designTabs_->count(); i++) {
			BusEditor* editor = dynamic_cast<BusEditor*>(designTabs_->widget(i));

			// if the abstraction definition matches
			if (editor && absDefVLNV.isValid() && editor->getDocumentVLNV() == absDefVLNV) {
				designTabs_->setCurrentIndex(i);
				return;
			}

			// if bus definition matches
			if (editor && editor->getDocumentVLNV() == busDefVLNV) {
				designTabs_->setCurrentIndex(i);
				return;
			}
		}
	}
	// at least the bus definition vlnv must be valid
	else {
		return;
	}

	// Editor for given vlnv was not yet open so create one for it
	QSharedPointer<BusDefinition> busDef;
	QSharedPointer<AbstractionDefinition> absDef;

	if (libraryHandler_->contains(busDefVLNV) && 
		libraryHandler_->getDocumentType(busDefVLNV) == VLNV::BUSDEFINITION) {

			QSharedPointer<LibraryComponent> libComp = libraryHandler_->getModel(busDefVLNV);
			busDef = libComp.dynamicCast<BusDefinition>();
	}
	else {
		emit errorMessage(tr("Bus definition %1:%2:%3:%4 was not found in the library").arg(
			busDefVLNV.getVendor()).arg(
			busDefVLNV.getLibrary()).arg(
			busDefVLNV.getName()).arg(
			busDefVLNV.getVersion()));
		return;
	}

	// pointer to the bus editor
	BusEditor* editor = 0;

	// if abstraction definition has been specified
	if (absDefVLNV.isValid()) {

		// if library contains the abstraction definition
		if (libraryHandler_->contains(absDefVLNV) && 
			libraryHandler_->getDocumentType(absDefVLNV) == VLNV::ABSTRACTIONDEFINITION) {

				QSharedPointer<LibraryComponent> libComp = libraryHandler_->getModel(absDefVLNV);
				absDef = libComp.staticCast<AbstractionDefinition>();

				editor = new BusEditor(this, libraryHandler_, busDef, absDef, disableBusDef);
		}
		else {
			emit errorMessage(tr("Abstraction definition %1:%2:%3:%4 was not found in the library").arg(
				absDefVLNV.getVendor()).arg(
				absDefVLNV.getLibrary()).arg(
				absDefVLNV.getName()).arg(
				absDefVLNV.getVersion()));
			return;
		}
	}

	// if no abstraction definition has been specified.
	else {
		editor = new BusEditor(this, libraryHandler_, busDef);
	}

    registerDocument(editor, forceUnlocked);
}

//-----------------------------------------------------------------------------
// Function: openDesign()
//-----------------------------------------------------------------------------
void MainWindow::openDesign(const VLNV& vlnv, const QString& viewName, bool forceUnlocked)
{
	// the vlnv must always be for a component
	Q_ASSERT(libraryHandler_->getDocumentType(vlnv) == VLNV::COMPONENT);

	// parse the referenced component
	QSharedPointer<LibraryComponent> libComp = libraryHandler_->getModel(vlnv);
	QSharedPointer<Component> comp = libComp.staticCast<Component>();
	
	// check if the design is already open
	VLNV refVLNV = comp->getHierRef(viewName);
	VLNV designVLNV = libraryHandler_->getDesignVLNV(refVLNV);
	if (isOpen(designVLNV)) {
		return;
	}
	
	QList<VLNV> hierRefs = comp->getHierRefs();

	// make sure that all component's hierarchy refs are valid
	bool hadInvalidRefs = false;
	foreach (VLNV ref, hierRefs) {

		// if the hierarchy referenced object is not found in library
		if (!libraryHandler_->contains(ref)) {
			emit errorMessage(tr("Component %1 has hierarchical reference to object %2,"
				" which is not found in library. Component is not valid and can not "
				"be opened in design view. Edit component with component editor to "
				"remove invalid references.").arg(vlnv.toString(":")).arg(ref.toString(":")));
			hadInvalidRefs = true;
			continue;
		}

		// if the reference is to a wrong object type
		else if (libraryHandler_->getDocumentType(ref) != VLNV::DESIGN &&
			libraryHandler_->getDocumentType(ref) != VLNV::DESIGNCONFIGURATION) {
				emit errorMessage(tr("Component %1 has hierarchical reference to object %2,"
					" which is not design or design configuration. Component is not valid and"
					" can not be opened in design view. Edit component with component editor to"
					" remove invalid references.").arg(vlnv.toString(":")).arg(ref.toString(":")));
				hadInvalidRefs = true;
				continue;
		}

		// if the reference is for a design configuration then check that also
		// the design is found
		else if (libraryHandler_->getDocumentType(ref) == VLNV::DESIGNCONFIGURATION) {
			VLNV designVLNV = libraryHandler_->getDesignVLNV(ref);

			QSharedPointer<LibraryComponent> libComp2 = libraryHandler_->getModel(ref);
			QSharedPointer<DesignConfiguration> desConf = libComp2.staticCast<DesignConfiguration>();
			VLNV refToDesign = desConf->getDesignRef();

			// if the referenced design was not found in the library
			if (!designVLNV.isValid()) {
				emit errorMessage(tr("Component %1 has hierarchical reference to object %2,"
					" which is design configuration and references to design %3. This "
					"design is not found in library so component can not be opened in "
					"design view. Edit component with component editor to remove "
					"invalid references").arg(
					vlnv.toString(":")).arg(
					ref.toString(":")).arg(
					refToDesign.toString(":")));
				hadInvalidRefs = true;
				continue;
			}
		}
	}
	// if there was at least one invalid reference then do not open the design
	if (hadInvalidRefs) {
		return;
	}

	HWDesignWidget* designWidget = new HWDesignWidget(libraryHandler_, this);
	
    // open the design in the designWidget
	designWidget->setDesign(vlnv, viewName);

	// if the design could not be opened
	if (!designWidget->getOpenDocument()) {
		delete designWidget;
		return;
	}

    connect(designWidget, SIGNAL(zoomChanged()), this, SLOT(updateZoomTools()), Qt::UniqueConnection);
    connect(designWidget, SIGNAL(modeChanged(DrawMode)),
        this, SLOT(onDrawModeChanged(DrawMode)), Qt::UniqueConnection);

    connect(designWidget, SIGNAL(destroyed(QObject*)),
        this, SLOT(onClearItemSelection()), Qt::UniqueConnection);

    connect(designWidget, SIGNAL(openDesign(const VLNV&, const QString&)),
        this, SLOT(openDesign(const VLNV&, const QString&)));
    connect(designWidget, SIGNAL(openComponent(const VLNV&)),
        this, SLOT(openComponent(const VLNV&)), Qt::UniqueConnection);
    connect(designWidget, SIGNAL(openBus(VLNV const&, VLNV const&, bool)),
        this, SLOT(openBus(VLNV const&, VLNV const&, bool)), Qt::UniqueConnection);

    connect(designWidget, SIGNAL(clearItemSelection()),
        libraryHandler_, SLOT(onClearSelection()), Qt::UniqueConnection);

    connect(designWidget, SIGNAL(componentSelected(ComponentItem*)),
        this, SLOT(onComponentSelected(ComponentItem*)), Qt::UniqueConnection);
    connect(designWidget, SIGNAL(interfaceSelected(ConnectionEndpoint*)),
        this, SLOT(onInterfaceSelected(ConnectionEndpoint*)), Qt::UniqueConnection);
    connect(designWidget, SIGNAL(connectionSelected(GraphicsConnection*)),
        this, SLOT(onConnectionSelected(GraphicsConnection*)), Qt::UniqueConnection);

    connect(designWidget, SIGNAL(clearItemSelection()),
        this, SLOT(onClearItemSelection()), Qt::UniqueConnection);

    registerDocument(designWidget, forceUnlocked);
}

//-----------------------------------------------------------------------------
// Function: MainWindow::openMemoryDesign()
//-----------------------------------------------------------------------------
void MainWindow::openMemoryDesign(const VLNV& vlnv, const QString& viewName, bool forceUnlocked)
{
    // the vlnv must always be for a component
    Q_ASSERT(libraryHandler_->getDocumentType(vlnv) == VLNV::COMPONENT);

    // parse the referenced component
    QSharedPointer<LibraryComponent> libComp = libraryHandler_->getModel(vlnv);
    QSharedPointer<Component> comp = libComp.staticCast<Component>();

    // check if the design is already open
    VLNV refVLNV = comp->getHierRef(viewName);
    VLNV designVLNV = libraryHandler_->getDesignVLNV(refVLNV);
    if (isOpen(designVLNV)) {
        return;
    }

    QList<VLNV> hierRefs = comp->getHierRefs();

    // make sure that all component's hierarchy refs are valid
    bool hadInvalidRefs = false;
    foreach (VLNV ref, hierRefs) {

        // if the hierarchy referenced object is not found in library
        if (!libraryHandler_->contains(ref)) {
            emit errorMessage(tr("Component %1 has hierarchical reference to object %2,"
                " which is not found in library. Component is not valid and can not "
                "be opened in design view. Edit component with component editor to "
                "remove invalid references.").arg(vlnv.toString(":")).arg(ref.toString(":")));
            hadInvalidRefs = true;
            continue;
        }

        // if the reference is to a wrong object type
        else if (libraryHandler_->getDocumentType(ref) != VLNV::DESIGN &&
            libraryHandler_->getDocumentType(ref) != VLNV::DESIGNCONFIGURATION) {
                emit errorMessage(tr("Component %1 has hierarchical reference to object %2,"
                    " which is not design or design configuration. Component is not valid and"
                    " can not be opened in design view. Edit component with component editor to"
                    " remove invalid references.").arg(vlnv.toString(":")).arg(ref.toString(":")));
                hadInvalidRefs = true;
                continue;
        }

        // if the reference is for a design configuration then check that also
        // the design is found
        else if (libraryHandler_->getDocumentType(ref) == VLNV::DESIGNCONFIGURATION) {
            VLNV designVLNV = libraryHandler_->getDesignVLNV(ref);

            QSharedPointer<LibraryComponent> libComp2 = libraryHandler_->getModel(ref);
            QSharedPointer<DesignConfiguration> desConf = libComp2.staticCast<DesignConfiguration>();
            VLNV refToDesign = desConf->getDesignRef();

            // if the referenced design was not found in the library
            if (!designVLNV.isValid()) {
                emit errorMessage(tr("Component %1 has hierarchical reference to object %2,"
                    " which is design configuration and references to design %3. This "
                    "design is not found in library so component can not be opened in "
                    "design view. Edit component with component editor to remove "
                    "invalid references").arg(
                    vlnv.toString(":")).arg(
                    ref.toString(":")).arg(
                    refToDesign.toString(":")));
                hadInvalidRefs = true;
                continue;
            }
        }
    }
    // if there was at least one invalid reference then do not open the design
    if (hadInvalidRefs) {
        return;
    }

    MemoryDesignWidget* designWidget = new MemoryDesignWidget(libraryHandler_, this);
    
    // open the design in the designWidget
    designWidget->setDesign(vlnv, viewName);

    // if the design could not be opened
    if (!designWidget->getOpenDocument()) {
        delete designWidget;
        return;
    }

    connect(designWidget, SIGNAL(zoomChanged()), this, SLOT(updateZoomTools()), Qt::UniqueConnection);
    connect(designWidget, SIGNAL(modeChanged(DrawMode)),
        this, SLOT(onDrawModeChanged(DrawMode)), Qt::UniqueConnection);

    connect(designWidget, SIGNAL(destroyed(QObject*)),
        this, SLOT(onClearItemSelection()), Qt::UniqueConnection);

    connect(designWidget, SIGNAL(clearItemSelection()),
        libraryHandler_, SLOT(onClearSelection()), Qt::UniqueConnection);

    connect(designWidget, SIGNAL(clearItemSelection()),
        this, SLOT(onClearItemSelection()), Qt::UniqueConnection);

    registerDocument(designWidget, forceUnlocked);
}

//-----------------------------------------------------------------------------
// Function: openSWDesign()
//-----------------------------------------------------------------------------
void MainWindow::openSWDesign(const VLNV& vlnv, QString const& viewName, bool forceUnlocked)
{
	// the vlnv must always be for a component
	Q_ASSERT(libraryHandler_->getDocumentType(vlnv) == VLNV::COMPONENT);

	// parse the referenced component
	QSharedPointer<LibraryComponent> libComp = libraryHandler_->getModel(vlnv);
	QSharedPointer<Component> comp = libComp.staticCast<Component>();

	// check if the design is already open
	VLNV refVLNV = comp->getHierSWRef(viewName);
	VLNV designVLNV = libraryHandler_->getDesignVLNV(refVLNV);
	if (isOpen(designVLNV)) {
		return;
	}

	SystemDesignWidget* designWidget = new SystemDesignWidget(true, libraryHandler_, this);

	if (!designWidget->setDesign(vlnv, viewName))
	{
		delete designWidget;
		return;
	}

	connect(designWidget, SIGNAL(openComponent(const VLNV&)),
		this, SLOT(openComponent(const VLNV&)), Qt::UniqueConnection);
	connect(designWidget, SIGNAL(openSWDesign(const VLNV&, const QString&)),
		this, SLOT(openSWDesign(const VLNV&, const QString&)));
	connect(designWidget, SIGNAL(openCSource(ComponentItem*)),
		this, SLOT(openCSource(ComponentItem*)), Qt::UniqueConnection);

	connect(designWidget, SIGNAL(componentSelected(ComponentItem*)),
		this, SLOT(onComponentSelected(ComponentItem*)), Qt::UniqueConnection);
	connect(designWidget, SIGNAL(interfaceSelected(ConnectionEndpoint*)),
		this, SLOT(onInterfaceSelected(ConnectionEndpoint*)), Qt::UniqueConnection);
	connect(designWidget, SIGNAL(connectionSelected(GraphicsConnection*)),
		this, SLOT(onConnectionSelected(GraphicsConnection*)), Qt::UniqueConnection);
	connect(designWidget, SIGNAL(destroyed(QObject*)),
		this, SLOT(onClearItemSelection()), Qt::UniqueConnection);
	connect(designWidget, SIGNAL(clearItemSelection()),
		libraryHandler_, SLOT(onClearSelection()), Qt::UniqueConnection);
	connect(designWidget, SIGNAL(clearItemSelection()),
		this, SLOT(onClearItemSelection()), Qt::UniqueConnection);

	connect(designWidget, SIGNAL(zoomChanged()), this, SLOT(updateZoomTools()), Qt::UniqueConnection);
	connect(designWidget, SIGNAL(modeChanged(DrawMode)),
		this, SLOT(onDrawModeChanged(DrawMode)), Qt::UniqueConnection);
	connect(designWidget, SIGNAL(modifiedChanged(bool)),
		actSave_, SLOT(setEnabled(bool)), Qt::UniqueConnection);

    registerDocument(designWidget, forceUnlocked);
}

//-----------------------------------------------------------------------------
// Function: openSystemDesign()
//-----------------------------------------------------------------------------
void MainWindow::openSystemDesign(VLNV const& vlnv, QString const& viewName, bool forceUnlocked)
{
	// the vlnv must always be for a component
	Q_ASSERT(libraryHandler_->getDocumentType(vlnv) == VLNV::COMPONENT);

	// parse the referenced component
	QSharedPointer<LibraryComponent> libComp = libraryHandler_->getModel(vlnv);
	QSharedPointer<Component> comp = libComp.staticCast<Component>();

	// check if the design is already open
	VLNV refVLNV = comp->getHierSystemRef(viewName);
	VLNV designVLNV = libraryHandler_->getDesignVLNV(refVLNV);
	if (isOpen(designVLNV)) {
		return;
	}

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	SystemDesignWidget* designWidget = new SystemDesignWidget(false, libraryHandler_, this);
    
	if (!designWidget->setDesign(vlnv, viewName))
	{
		delete designWidget;
		QApplication::restoreOverrideCursor();
		return;
	}

	connect(designWidget, SIGNAL(openComponent(const VLNV&)),
		this, SLOT(openComponent(const VLNV&)), Qt::UniqueConnection);
	connect(designWidget, SIGNAL(openSWDesign(const VLNV&, const QString&)),
		this, SLOT(openSWDesign(const VLNV&, const QString&)));
	connect(designWidget, SIGNAL(openCSource(ComponentItem*)),
		this, SLOT(openCSource(ComponentItem*)), Qt::UniqueConnection);
    
	connect(designWidget, SIGNAL(componentSelected(ComponentItem*)),
		this, SLOT(onComponentSelected(ComponentItem*)), Qt::UniqueConnection);
    connect(designWidget, SIGNAL(interfaceSelected(ConnectionEndpoint*)),
        this, SLOT(onInterfaceSelected(ConnectionEndpoint*)), Qt::UniqueConnection);
    connect(designWidget, SIGNAL(connectionSelected(GraphicsConnection*)),
        this, SLOT(onConnectionSelected(GraphicsConnection*)), Qt::UniqueConnection);
	connect(designWidget, SIGNAL(destroyed(QObject*)),
		this, SLOT(onClearItemSelection()), Qt::UniqueConnection);
	connect(designWidget, SIGNAL(clearItemSelection()),
		libraryHandler_, SLOT(onClearSelection()), Qt::UniqueConnection);
	connect(designWidget, SIGNAL(clearItemSelection()),
		this, SLOT(onClearItemSelection()), Qt::UniqueConnection);

	connect(designWidget, SIGNAL(zoomChanged()), this, SLOT(updateZoomTools()), Qt::UniqueConnection);
	connect(designWidget, SIGNAL(modeChanged(DrawMode)),
		this, SLOT(onDrawModeChanged(DrawMode)), Qt::UniqueConnection);
	connect(designWidget, SIGNAL(modifiedChanged(bool)),
		actSave_, SLOT(setEnabled(bool)), Qt::UniqueConnection);

    registerDocument(designWidget, forceUnlocked);

	QApplication::restoreOverrideCursor();
}

void MainWindow::openComponent( const VLNV& vlnv, bool forceUnlocked ) {

	if (isOpen(vlnv)) {
		return;
	}

	// component editor was not yet open so create it

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	QSharedPointer<Component> component;

	if (libraryHandler_->contains(vlnv)) {
		QSharedPointer<LibraryComponent> libComp = libraryHandler_->getModel(vlnv);
		component = libComp.dynamicCast<Component>();
	}
	else {
		emit errorMessage(tr("VLNV %1:%2:%3:%4 was not found in the library").arg(
			vlnv.getVendor()).arg(
			vlnv.getLibrary()).arg(
			vlnv.getName()).arg(
			vlnv.getVersion()));
		QApplication::restoreOverrideCursor();
		return;
	}

	if (!component) {
		emit errorMessage(tr("Document type did not match Component"));
		QApplication::restoreOverrideCursor();
		return;
	}

	ComponentEditor* editor = new ComponentEditor(libraryHandler_, *pluginMgr_, component, this);
    
	QString styleSheet("*[mandatoryField=\"true\"] { background-color: LemonChiffon; }");
	editor->setStyleSheet(styleSheet);

	connect(editor, SIGNAL(openCSource(QString const&, QSharedPointer<Component>)),
            this , SLOT(openCSource(QString const&, QSharedPointer<Component>)), Qt::UniqueConnection);
	connect(editor, SIGNAL(openDesign(const VLNV&, const QString&)),
		this, SLOT(openDesign(const VLNV&, const QString&)), Qt::UniqueConnection);
	connect(editor, SIGNAL(openBus(const VLNV&, const VLNV&)),
		this, SLOT(openBus(const VLNV&, const VLNV&)), Qt::UniqueConnection);
	connect(editor, SIGNAL(openComDefinition(const VLNV&)),
		this, SLOT(openComDefinition(const VLNV&)), Qt::UniqueConnection);
	connect(editor, SIGNAL(openSWDesign(const VLNV&, const QString&)),
		this, SLOT(openSWDesign(const VLNV&, const QString&)), Qt::UniqueConnection);
	connect(editor, SIGNAL(openSystemDesign(const VLNV&, const QString&)),
		this, SLOT(openSystemDesign(const VLNV&, const QString&)), Qt::UniqueConnection);

    registerDocument(editor, forceUnlocked);

	QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------
// Function: MainWindow::openComDefinition()
//-----------------------------------------------------------------------------
void MainWindow::openComDefinition(VLNV const& vlnv, bool forceUnlocked /*= false*/)
{

	if (isOpen(vlnv)) {
		return;
	}

    // Editor was not yet open so create it.
    QSharedPointer<ComDefinition> comDef;

    if (libraryHandler_->contains(vlnv))
    {
        QSharedPointer<LibraryComponent> libComp = libraryHandler_->getModel(vlnv);
        comDef = libComp.dynamicCast<ComDefinition>();
    }
    else
    {
        emit errorMessage(tr("VLNV %1:%2:%3:%4 was not found in the library").arg(vlnv.getVendor(),
                                                                                  vlnv.getLibrary(),
                                                                                  vlnv.getName(),
                                                                                  vlnv.getVersion()));
        return;
    }

    if (comDef == 0)
    {
        emit errorMessage(tr("Document type did not match COM Definition"));
        return;
    }

    ComDefinitionEditor* editor = new ComDefinitionEditor(this, libraryHandler_, comDef);
    registerDocument(editor, forceUnlocked);
}


//-----------------------------------------------------------------------------
// Function: MainWindow::openApiDefinition()
//-----------------------------------------------------------------------------
void MainWindow::openApiDefinition(VLNV const& vlnv, bool forceUnlocked /*= false*/)
{

	if (isOpen(vlnv)) {
		return;
	}

	// Editor was not yet open so create it.
    QSharedPointer<ApiDefinition> apiDef;

    if (libraryHandler_->contains(vlnv))
    {
        QSharedPointer<LibraryComponent> libComp = libraryHandler_->getModel(vlnv);
        apiDef = libComp.dynamicCast<ApiDefinition>();
    }
    else
    {
        emit errorMessage(tr("VLNV %1:%2:%3:%4 was not found in the library").arg(vlnv.getVendor(),
                                                                                  vlnv.getLibrary(),
                                                                                  vlnv.getName(),
                                                                                  vlnv.getVersion()));
        return;
    }

    if (apiDef == 0)
    {
        emit errorMessage(tr("Document type did not match API Definition"));
        return;
    }

    ApiDefinitionEditor* editor = new ApiDefinitionEditor(this, libraryHandler_, apiDef);
    registerDocument(editor, forceUnlocked);
}

//-----------------------------------------------------------------------------
// Function: MainWindow::isOpen()
//-----------------------------------------------------------------------------
bool MainWindow::isOpen( const VLNV& vlnv ) const {
	if (!vlnv.isValid()) {
		return false;
	}

	for (int i = 0; i < designTabs_->count(); i++) {
		TabDocument* document = dynamic_cast<TabDocument*>(designTabs_->widget(i));

		// if the document is already open is some tab
		if (document && document->getIdentifyingVLNV() == vlnv) {
			designTabs_->setCurrentIndex(i);
			return true;
		}
	}
	// document was not open
	return false;
}

//-----------------------------------------------------------------------------
// Function: MainWindow::setWindowVisibilityForSupportedWindow()
//-----------------------------------------------------------------------------
void MainWindow::setWindowVisibilityForSupportedWindow(TabDocument::SupportedWindows type, bool show)
{
    if (isSupportedWindowType(type)) {
        setWindowVisibility(type, show);
    }
}

//-----------------------------------------------------------------------------
// Function: MainWindow::setWindowVisibility()
//-----------------------------------------------------------------------------
void MainWindow::setWindowVisibility(TabDocument::SupportedWindows windowType, bool show)
{
    visibilities_.insert(windowType, show);
}

//-----------------------------------------------------------------------------
// Function: mainwindow::updateWindows()
//-----------------------------------------------------------------------------
void MainWindow::updateWindows()
{
    updateWindowAndControlVisibility(TabDocument::LIBRARYWINDOW, libraryDock_);
    updateWindowAndControlVisibility(TabDocument::OUTPUTWINDOW, consoleDock_);
    updateWindowAndControlVisibility(TabDocument::CONTEXT_HELP_WINDOW, contextHelpDock_);
    updateWindowAndControlVisibility(TabDocument::PREVIEWWINDOW, previewDock_);
    updateWindowAndControlVisibility(TabDocument::CONFIGURATIONWINDOW, configurationDock_);
    updateWindowAndControlVisibility(TabDocument::SYSTEM_DETAILS_WINDOW, systemDetailsDock_);
    updateWindowAndControlVisibility(TabDocument::CONNECTIONWINDOW, connectionDock_);
    updateWindowAndControlVisibility(TabDocument::INTERFACEWINDOW, interfaceDock_);
    updateWindowAndControlVisibility(TabDocument::INSTANCEWINDOW, instanceDock_);
    updateWindowAndControlVisibility(TabDocument::ADHOC_WINDOW, adHocDock_);
    updateWindowAndControlVisibility(TabDocument::ADDRESS_WINDOW, addressDock_);   
    //updateWindowAndControlVisibility(TabDocument::NOTES_WINDOW, notesDock_);
}

//-----------------------------------------------------------------------------
// Function: MainWindow::updateWindowControlVisibility()
//-----------------------------------------------------------------------------
void MainWindow::updateWindowAndControlVisibility(TabDocument::SupportedWindows windowType, QDockWidget* dock)
{
    QAction* showAction = dock->toggleViewAction(); 
   
    if (isSupportedWindowType(windowType)) {
        showAction->setVisible(true);
        dock->setVisible(visibilities_.value(windowType));
    }
    else {
        showAction->setVisible(false);
        dock->hide();
    }
}

//-----------------------------------------------------------------------------
// Function: MainWindow::isSupportedWindowType()
//-----------------------------------------------------------------------------
bool MainWindow::isSupportedWindowType(TabDocument::SupportedWindows windowType)
{
    return currentlySupportedWindows() & windowType;
}

//-----------------------------------------------------------------------------
// Function: MainWindow::supportedWindows()
//-----------------------------------------------------------------------------
unsigned int MainWindow::currentlySupportedWindows()
{
    unsigned int windows = defaultWindows();

    if (designTabs_->count() != 0)
    {
        TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());
        windows = doc->getSupportedWindows();
    }

    return windows;
}

//-----------------------------------------------------------------------------
// Function: MainWindow::defaultWindows()
//-----------------------------------------------------------------------------
unsigned int MainWindow::defaultWindows()
{
    return TabDocument::OUTPUTWINDOW | TabDocument::LIBRARYWINDOW | 
        TabDocument::PREVIEWWINDOW | TabDocument::CONTEXT_HELP_WINDOW;        
}

//-----------------------------------------------------------------------------
// Function: MainWindow::connectVisibilityControls()
//-----------------------------------------------------------------------------
void MainWindow::connectVisibilityControls()
{
    // Action to show/hide the output window.
    connect(consoleDock_->toggleViewAction(), SIGNAL(toggled(bool)),
        this, SLOT(onOutputAction(bool)), Qt::UniqueConnection);

    // Action to show/hide the context help window.
    connect(contextHelpDock_->toggleViewAction(), SIGNAL(toggled(bool)),
        this, SLOT(onContextHelpAction(bool)), Qt::UniqueConnection);

    // Action to show/hide the preview box.
    connect(previewDock_->toggleViewAction(), SIGNAL(toggled(bool)),
        this, SLOT(onPreviewAction(bool)), Qt::UniqueConnection);

    // Action to show/hide the library window.
    connect(libraryDock_->toggleViewAction(), SIGNAL(toggled(bool)),
        this, SLOT(onLibraryAction(bool)), Qt::UniqueConnection);

    // Action to show/hide the configuration window.
    connect(configurationDock_->toggleViewAction(), SIGNAL(toggled(bool)),
        this, SLOT(onConfigurationAction(bool)), Qt::UniqueConnection);

    // Action to show/hide the system detail window.
    connect(systemDetailsDock_->toggleViewAction(), SIGNAL(toggled(bool)),
        this, SLOT(onSystemDetailsAction(bool)), Qt::UniqueConnection);

    // Action to show/hide the connection editor.
    connect(connectionDock_->toggleViewAction(), SIGNAL(toggled(bool)),
        this, SLOT(onConnectionAction(bool)), Qt::UniqueConnection);

    // Action to show/hide the interface editor.
    connect(interfaceDock_->toggleViewAction(), SIGNAL(toggled(bool)),
        this, SLOT(onInterfaceAction(bool)), Qt::UniqueConnection);

    // Action to show/hide the instance editor.
    connect(instanceDock_->toggleViewAction(), SIGNAL(toggled(bool)),
        this, SLOT(onInstanceAction(bool)), Qt::UniqueConnection);

    // Action to show/hide the ad-hoc visibility editor.
    connect(adHocDock_->toggleViewAction(), SIGNAL(toggled(bool)), 
        this, SLOT(onAdHocAction(bool)), Qt::UniqueConnection);

    // Action to show/hide the address editor.
    connect(addressDock_->toggleViewAction(), SIGNAL(toggled(bool)), 
        this, SLOT(onAddressAction(bool)), Qt::UniqueConnection);

    // Action to show/hide the notes.
    /*connect(notesDock_->toggleViewAction(), SIGNAL(toggled(bool)), 
        this, SLOT(onNotesEditorAction(bool)), Qt::UniqueConnection);*/
}

//-----------------------------------------------------------------------------
// Function: MainWindow::disconnectVisibilityControls()
//-----------------------------------------------------------------------------
void MainWindow::disconnectVisibilityControls()
{
    disconnect(consoleDock_->toggleViewAction(), SIGNAL(toggled(bool)), this, SLOT(onOutputAction(bool)));

    disconnect(contextHelpDock_->toggleViewAction(), SIGNAL(toggled(bool)), this, SLOT(onContextHelpAction(bool)));

    disconnect(previewDock_->toggleViewAction(), SIGNAL(toggled(bool)), this, SLOT(onPreviewAction(bool)));

    disconnect(libraryDock_->toggleViewAction(), SIGNAL(toggled(bool)), this, SLOT(onLibraryAction(bool)));	

    disconnect(configurationDock_->toggleViewAction(), SIGNAL(toggled(bool)),
        this, SLOT(onConfigurationAction(bool)));

    disconnect(systemDetailsDock_->toggleViewAction(), SIGNAL(toggled(bool)), 
        this, SLOT(onSystemDetailsAction(bool)));

    disconnect(connectionDock_->toggleViewAction(), SIGNAL(toggled(bool)), this, SLOT(onConnectionAction(bool)));

    disconnect(interfaceDock_->toggleViewAction(), SIGNAL(toggled(bool)), this, SLOT(onInterfaceAction(bool)));

    disconnect(instanceDock_->toggleViewAction(), SIGNAL(toggled(bool)), this, SLOT(onInstanceAction(bool)));

    disconnect(adHocDock_->toggleViewAction(), SIGNAL(toggled(bool)), this, SLOT(onAdHocAction(bool)));

    disconnect(addressDock_->toggleViewAction(), SIGNAL(toggled(bool)), this, SLOT(onAddressAction(bool)));
   
    //disconnect(notesDock_->toggleViewAction(), SIGNAL(toggled(bool)), this, SLOT(onNotesEditorAction(bool)));
}

//-----------------------------------------------------------------------------
// Function: openCSource()
//-----------------------------------------------------------------------------
void MainWindow::openCSource(ComponentItem* compItem)
{
	Q_ASSERT(compItem != 0);

	QSharedPointer<FileSet> fileSet = compItem->componentModel()->getFileSet("cSources");

	if (fileSet == 0 || fileSet->getFiles().count() == 0)
	{
		return;
	}

	QString filename;

	if (fileSet->getFiles().count() > 1)
	{
		// Show a dialog for selecting what source file to open.
		ListSelectDialog dialog(this);
		dialog.setWindowTitle(tr("Open Source"));
		dialog.setDescription(tr("Select C source file to open:"));

		foreach (QSharedPointer<File> file, fileSet->getFiles())
		{
			dialog.addItem(new QListWidgetItem(file->getName()));
		}

		if (dialog.exec() == QDialog::Rejected)
		{
			return;
		}

		filename = dialog.getSelectedItem()->text();
	}
	else
	{
		// Otherwise there is only one possibility.
		filename = fileSet->getFiles().first()->getName();
	}

	if (compItem->componentModel()->getVlnv()->isValid())
	{
		filename = General::getAbsolutePath(libraryHandler_->getPath(*compItem->componentModel()->getVlnv()),
			filename);
	}

	openCSource(filename, compItem->componentModel());
}

//-----------------------------------------------------------------------------
// Function: MainWindow::openCSource()
//-----------------------------------------------------------------------------
void MainWindow::openCSource(QString const& filename, QSharedPointer<Component> component)
{
	// Check if the source is already open and activate it.
	for (int i = 0; i < designTabs_->count(); i++)
	{
		CSourceWidget* sourceWidget = dynamic_cast<CSourceWidget*>(designTabs_->widget(i));

		if (sourceWidget != 0 && sourceWidget->getSourceFile() == filename)
		{
			designTabs_->setCurrentIndex(i);
			return;
		}
	}

	// Otherwise make sure that the file exists.
	if (!QFile::exists(filename))
	{
		QMessageBox msgBox(QMessageBox::Critical, QCoreApplication::applicationName(),
			"The source file " + filename + " is not found!", QMessageBox::Ok, this);
		msgBox.exec();
		return;
	}

	// Open the source to a view.
	CSourceWidget* sourceWidget = new CSourceWidget(filename, component, libraryHandler_, this, this);
	connect(sourceWidget, SIGNAL(contentChanged()), this, SLOT(updateMenuStrip()));
	connect(sourceWidget->getEditProvider(), SIGNAL(editStateChanged()), this, SLOT(updateMenuStrip()));
	sourceWidget->setTabWidget(designTabs_);
}

//-----------------------------------------------------------------------------
// Function: changeProtection()
//-----------------------------------------------------------------------------
void MainWindow::changeProtection(bool locked)
{
	TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());
		
	// if there is no currently selected tab
	if (!doc) {
		return;
	}

	VLNV docVLNV = doc->getDocumentVLNV();
    Q_ASSERT(docVLNV.isValid());
	
	// if user set the document to be locked and it has been modified
	if (locked)
	{
		if (doc->isModified())
		{

			// ask user if he wants to save the changes made to document
			QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
				tr("The document has been modified. The changes need to be saved before the "
				   "document can be locked. Save changes and continue?"),
				QMessageBox::Yes | QMessageBox::No, this);

			// if user does not want to save or save can't be done
			if (msgBox.exec() == QMessageBox::No || !doc->save())
			{
				actProtect_->setChecked(false);
				return;
			}
		}
		// lock the document
		doc->setProtection(locked);
    }

	// if user wants to unlock the document
    else
    {
        // the list containing documents for the same document
        QList<TabDocument*> otherDocs;
        // If the same document has been edited in another editor
        TabDocument* docToSave = NULL;

		for (int i = 0; i < designTabs_->count(); ++i)
        {
			TabDocument* otherDoc = static_cast<TabDocument*>(designTabs_->widget(i));
			Q_ASSERT(otherDoc);

			// if the other document is for the same component
			if (otherDoc != doc && otherDoc->getDocumentVLNV() == docVLNV) {
				
				otherDocs.append(otherDoc);

				// if there is a document that is not locked
				if (!otherDoc->isProtected()) {

					// if there is a document that has been modified
					if (otherDoc->isModified()) {
						
						// there should only be one doc at a time that can be modified
						Q_ASSERT(!docToSave);

						// this was the document that was modified
						docToSave = otherDoc;
					}
				}
			}
		}

        // if there was document that was unlocked and modified
        if (docToSave)
        {
            // Ask the user if he wants to save and switch locks.
            QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
                tr("The document is being edited in another tab and "
                   "has unsaved changes. Changes need to be saved "
                   "before this tab can be unlocked. "
                   "Save changes and switch locks?"),
                QMessageBox::Yes | QMessageBox::No, this);

            // Restore the lock if the user canceled.
            if (msgBox.exec() == QMessageBox::No) {
                actProtect_->setChecked(true);
                return;
            }

            // if the document could not be saved
            if (!docToSave->save()) {
                return;
            }
        }

        bool previouslyUnlocked = doc->isPreviouslyUnlocked();

        // user wanted to unlock this doc so go through other documents
        foreach (TabDocument* otherDoc, otherDocs) {
            // Lock the other document.
            otherDoc->setProtection(true);

            // if the other document has been previously unlocked
            if (otherDoc->isPreviouslyUnlocked()) {
                previouslyUnlocked = true;
            }
        }

        // if the document has not been previously unlocked
        if (!previouslyUnlocked) {

            QString message = tr("Are you sure you want to unlock the document?\n");
            QString detailMsg = "";

            // if edited document was component 
            if (docVLNV.isValid() && libraryHandler_->getDocumentType(docVLNV) == VLNV::COMPONENT) {

                // if component has been instantiated in other components then print
                // a list of those components
                QList<VLNV> list;
                int refCount = libraryHandler_->getOwners(list, docVLNV);
                if (refCount > 0) {
                    detailMsg += tr("The component has been instantiated in the following %1 component(s):\n").arg(refCount);

                    foreach (VLNV owner, list) {
                        detailMsg += "* " + owner.getVendor() + ":" + owner.getLibrary() + ":" +
                            owner.getName() + ":" + owner.getVersion() + "\n"; 
                    }

                    message += tr("Changes to the document can affect %1 other documents.").arg(refCount);
                }
                else {
                    message += tr("Changes to the document can affect other documents.");
                }
            }
            else {
                message += tr("Changes to the document can affect other documents.");
            }

            message += " If you choose yes, this will not be asked next time for this document.";

            // Ask verification from the user.
            QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
                message, QMessageBox::Yes | QMessageBox::No, this);
            msgBox.setDetailedText(detailMsg);

            // if user did not want to unlock the document
            if (msgBox.exec() == QMessageBox::No) {
                actProtect_->setChecked(true);
                return;
            }
        }

        // mark the other documents also unlocked to keep the unlock history in sync.
        foreach (TabDocument* otherDoc, otherDocs) {
            otherDoc->setPreviouslyUnlocked();
        }

        //doc->refresh(); Refresh is no longer needed.
        // Unlock the document.
        doc->setProtection(locked);
	}

	// if the tab is a design widget
	DesignWidget* designwidget = dynamic_cast<DesignWidget*>(doc);
	if (designwidget) {
		// update the editors to match the locked state
		configurationEditor_->setLocked(locked);

        if (designwidget->getSupportedWindows() & TabDocument::SYSTEM_DETAILS_WINDOW) {
            systemDetailsEditor_->setLocked(locked);
        }
    }
	else {
		configurationEditor_->setLocked(true);
        systemDetailsEditor_->setLocked(true);
    }

	// Clear the item selection since the current instance is no longer valid.
    onClearItemSelection();

	onProtectionChanged(locked);

	updateMenuStrip();
}

//-----------------------------------------------------------------------------
// Function: onProtectionChanged()
//-----------------------------------------------------------------------------
void MainWindow::onProtectionChanged(bool locked)
{
	if (locked)
	{
		actProtect_->setIcon(QIcon(":icons/common/graphics/protection-locked.png"));
		actProtect_->setText(tr("Locked"));
	}
	else
	{
		actProtect_->setIcon(QIcon(":icons/common/graphics/protection-unlocked.png"));
		actProtect_->setText(tr("Unlocked"));
	}
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void MainWindow::undo()
{
	TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());

	if (doc != 0)
	{
		doc->getEditProvider()->undo();
	}
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void MainWindow::redo()
{
	TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());

	if (doc != 0)
	{
		doc->getEditProvider()->redo();
	}
}

//-----------------------------------------------------------------------------
// Function: createSWComponent()
//-----------------------------------------------------------------------------
void MainWindow::createSWComponent(VLNV const& vlnv, QString const& directory)
{
	Q_ASSERT(vlnv.isValid());

	// Create a component.
	QSharedPointer<Component> component = QSharedPointer<Component>(new Component());
	component->setVlnv(vlnv);

	// Set Kactus attributes.
	component->setComponentImplementation(KactusAttribute::KTS_SW);

    // Create the file.
    if (!libraryHandler_->writeModelToFile(directory, component))
    {
        emit errorMessage("Error saving files to disk.");
        return;
    }

    // Open up the component wizard.
    ComponentWizard wizard(component, directory, *pluginMgr_, libraryHandler_, this);
    wizard.exec();

	// Save wizard changes.
    if (!libraryHandler_->writeModelToFile(component))
    {
        emit errorMessage("Error saving files to disk.");
        return;
    }

	openComponent(vlnv, true);
}

//-----------------------------------------------------------------------------
// Function: showAbout()
//-----------------------------------------------------------------------------
void MainWindow::showAbout()
{
	SplashScreen* splash = new SplashScreen(this);
	splash->setAttribute(Qt::WA_DeleteOnClose);
	splash->setWindowFlags(splash->windowFlags() & ~(Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint));
	splash->move(this->mapToGlobal(this->rect().center() - splash->rect().center()));
	splash->setWindowModality(Qt::ApplicationModal);
	splash->show();
	splash->showMessage("");
}

//-----------------------------------------------------------------------------
// Function: showAbout()
//-----------------------------------------------------------------------------
void MainWindow::showHelp()
{
    helpWnd_->show();
}

void MainWindow::selectVisibleDocks() {
	windowsMenu_.exec(QCursor::pos());
}

void MainWindow::hideEvent( QHideEvent* event ) 
{
    disconnectVisibilityControls();
	QMainWindow::hideEvent(event);
}

void MainWindow::showEvent( QShowEvent* event ) 
{
    connectVisibilityControls();
	QMainWindow::showEvent(event);
}

//-----------------------------------------------------------------------------
// Function: openWorkspaceMenu()
//-----------------------------------------------------------------------------
void MainWindow::openWorkspaceMenu()
{
    workspaceMenu_.exec(QCursor::pos());
}

//-----------------------------------------------------------------------------
// Function: MainWindow::onWorkspaceChanged()
//-----------------------------------------------------------------------------
void MainWindow::onWorkspaceChanged(QAction* action)
{
    saveWorkspace(curWorkspaceName_);

    curWorkspaceName_ = action->text();
    loadWorkspace(curWorkspaceName_);
}

//-----------------------------------------------------------------------------
// Function: onNewWorkspace()
//-----------------------------------------------------------------------------
void MainWindow::onNewWorkspace()
{
    NewWorkspaceDialog dialog(this);

    if (dialog.exec() == QDialog::Accepted)
    {
        // Save the new workspace with current settings and set the it as the current one.
        FilterWidget::FilterOptions defaultOptions;
        defaultOptions.type.advanced_ = false;
        dialer_->setFilters(defaultOptions);
        saveWorkspace(dialog.getName());
        curWorkspaceName_ = dialog.getName();
        updateWorkspaceMenu();
    }
}

//-----------------------------------------------------------------------------
// Function: onDeleteWorkspace()
//-----------------------------------------------------------------------------
void MainWindow::onDeleteWorkspace()
{
    DeleteWorkspaceDialog dialog(this);

    // Fill in the dialog with existing workspace names.
    QSettings settings;
    settings.beginGroup("Workspaces");

    QStringList workspaces = settings.childGroups();

    foreach (QString const& workspace, workspaces)
    {
        if (workspace != "Default" && workspace != curWorkspaceName_)
        {        
            dialog.addWorkspaceName(workspace);
        }
    }

    settings.endGroup();

    // Execute the dialog.
    if (dialog.exec() == QDialog::Accepted)
    {
        // Remove the workspace from the settings and update the workspace menu.
        settings.remove("Workspaces/" + dialog.getName());
        updateWorkspaceMenu();
    }
}

//-----------------------------------------------------------------------------
// Function: mainwindow::onOutputAction()
//-----------------------------------------------------------------------------
void MainWindow::onOutputAction( bool show ) {
    setWindowVisibilityForSupportedWindow(TabDocument::OUTPUTWINDOW, show);
}

//-----------------------------------------------------------------------------
// Function: mainwindow::onContextHelpAction()
//-----------------------------------------------------------------------------
void MainWindow::onContextHelpAction( bool show )
{
    setWindowVisibilityForSupportedWindow(TabDocument::CONTEXT_HELP_WINDOW, show);
}

//-----------------------------------------------------------------------------
// Function: mainwindow::onPreviewAction()
//-----------------------------------------------------------------------------
void MainWindow::onPreviewAction( bool show ) {
    setWindowVisibilityForSupportedWindow(TabDocument::PREVIEWWINDOW, show);
}

//-----------------------------------------------------------------------------
// Function: mainwindow::onLibraryAction()
//-----------------------------------------------------------------------------
void MainWindow::onLibraryAction( bool show ) {
    setWindowVisibilityForSupportedWindow(TabDocument::LIBRARYWINDOW, show);
}

//-----------------------------------------------------------------------------
// Function: mainwindow::onConfigurationAction()
//-----------------------------------------------------------------------------
void MainWindow::onConfigurationAction( bool show ) {
    setWindowVisibilityForSupportedWindow(TabDocument::CONFIGURATIONWINDOW, show);
}

//-----------------------------------------------------------------------------
// Function: mainwindow::onSystemDetailsAction()
//-----------------------------------------------------------------------------
void MainWindow::onSystemDetailsAction( bool show )
{
    setWindowVisibilityForSupportedWindow(TabDocument::SYSTEM_DETAILS_WINDOW, show);
}

//-----------------------------------------------------------------------------
// Function: mainwindow::onConnectionAction()
//-----------------------------------------------------------------------------
void MainWindow::onConnectionAction( bool show ) {
    setWindowVisibilityForSupportedWindow(TabDocument::CONNECTIONWINDOW, show);
}

//-----------------------------------------------------------------------------
// Function: mainwindow::onInterfaceAction()
//-----------------------------------------------------------------------------
void MainWindow::onInterfaceAction( bool show ) {
    setWindowVisibilityForSupportedWindow(TabDocument::INTERFACEWINDOW, show);
}

//-----------------------------------------------------------------------------
// Function: mainwindow::onAdHocAction()
//-----------------------------------------------------------------------------
void MainWindow::onAdHocAction( bool show ) {
    setWindowVisibilityForSupportedWindow(TabDocument::ADHOC_WINDOW, show);
}

//-----------------------------------------------------------------------------
// Function: MainWindow::onAddressAction()
//-----------------------------------------------------------------------------
void MainWindow::onAddressAction(bool show)
{
    setWindowVisibilityForSupportedWindow(TabDocument::ADDRESS_WINDOW, show);
}

//-----------------------------------------------------------------------------
// Function: mainwindow::onInstanceAction()
//-----------------------------------------------------------------------------
void MainWindow::onInstanceAction( bool show ) {
    setWindowVisibilityForSupportedWindow(TabDocument::INSTANCEWINDOW, show);
}

//-----------------------------------------------------------------------------
// Function: mainwindow::onNotesEditorAction()
//-----------------------------------------------------------------------------
void MainWindow::onNotesEditorAction(bool show)
{
    setWindowVisibilityForSupportedWindow(TabDocument::NOTES_WINDOW, show);
}

//-----------------------------------------------------------------------------
// Function: openVisibilityControlMenu()
//-----------------------------------------------------------------------------
void MainWindow::openVisibilityControlMenu()
{
    visibilityMenu_.clear();

    // Add all available visibility controls from the current document to the popup menu.
    TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());
    Q_ASSERT(doc != 0);

    QMapIterator<QString, bool> iter(doc->getVisibilityControls());
    QActionGroup* group = new QActionGroup(this);

    while (iter.hasNext())
    {
        iter.next();
        
        QAction* action = new QAction(tr(qPrintable(iter.key())), this);
        action->setCheckable(true);
        action->setChecked(iter.value());

        group->addAction(action);
        visibilityMenu_.addAction(action);
    }

    connect(group, SIGNAL(triggered(QAction*)), this, SLOT(onVisibilityControlToggled(QAction*)));

    // Show the popup menu.
    visibilityMenu_.exec(QCursor::pos());
}

//-----------------------------------------------------------------------------
// Function: onVisibilityControlToggled()
//-----------------------------------------------------------------------------
void MainWindow::onVisibilityControlToggled(QAction* action)
{
    TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());
    Q_ASSERT(doc != 0);

    doc->setVisibilityControlState(action->text(), !doc->getVisibilityControls().value(action->text()));
}

//-----------------------------------------------------------------------------
// Function: MainWindow::refresh()
//-----------------------------------------------------------------------------
void MainWindow::refresh()
{
    TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());
    
    if (doc == 0)
    {
        return;
    }

    if (doc->isModified())
    {
        QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
            tr("The document has been modified. Save changes before refresh?"),
            QMessageBox::Yes | QMessageBox::No, this);

        if (msgBox.exec() == QMessageBox::Yes)
        {
            doc->save();
        }
    }

    doc->refresh();
}

//-----------------------------------------------------------------------------
// Function: MainWindow::isDesignOpen()
//-----------------------------------------------------------------------------
bool MainWindow::isDesignOpen(VLNV const& vlnv, KactusAttribute::Implementation implementation) const
{
    if (vlnv.isValid())
    {
        for (int i = 0; i < designTabs_->count(); i++)
        {
            DesignWidget* designWidget = dynamic_cast<DesignWidget*>(designTabs_->widget(i));

            if (designWidget != 0 && *designWidget->getOpenDocument() == vlnv &&
                designWidget->getImplementation() == implementation)
            {
                designTabs_->setCurrentIndex(i);
                return true;
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: MainWindow::registerDocument()
//-----------------------------------------------------------------------------
void MainWindow::registerDocument(TabDocument* doc, bool forceUnlocked)
{
    connect(doc, SIGNAL(errorMessage(const QString&)),
        console_, SLOT(onErrorMessage(const QString&)), Qt::UniqueConnection);
    connect(doc, SIGNAL(noticeMessage(const QString&)),
        console_, SLOT(onNoticeMessage(const QString&)), Qt::UniqueConnection);

    connect(doc, SIGNAL(helpUrlRequested(QString const&)),
            this, SIGNAL(helpUrlRequested(QString const&)), Qt::UniqueConnection);


    if (doc->getEditProvider() != 0)
    {
        connect(doc->getEditProvider(), SIGNAL(editStateChanged()), this, SLOT(updateMenuStrip()));
    }

    connect(doc, SIGNAL(contentChanged()), this, SLOT(updateMenuStrip()), Qt::UniqueConnection);
    connect(doc, SIGNAL(modifiedChanged(bool)), actSave_, SLOT(setEnabled(bool)), Qt::UniqueConnection);
    connect(doc, SIGNAL(documentSaved(TabDocument*)),
            this, SLOT(onDocumentSaved(TabDocument*)), Qt::UniqueConnection);

    if (forceUnlocked/* || !isDocumentOpen(doc->getDocumentVLNV())*/)
    {
        doc->setProtection(false);
    }
    else
    {
        // Open in unlocked mode by default only if the version is draft.
        doc->setProtection(doc->getDocumentVLNV().getVersion() != "draft");
    }

    doc->setTabWidget(designTabs_);
}

//-----------------------------------------------------------------------------
// Function: MainWindow::onDocumentSaved()
//-----------------------------------------------------------------------------
void MainWindow::onDocumentSaved(TabDocument* doc)
{
    for (int i = 0; i < designTabs_->count(); i++)
    {
        TabDocument* otherDoc = static_cast<TabDocument*>(designTabs_->widget(i));

        if (otherDoc != doc && otherDoc->getRelatedVLNVs().contains(doc->getDocumentVLNV()))
        {
            otherDoc->requestRefresh();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MainWindow::setLibraryLocations()
//-----------------------------------------------------------------------------
void MainWindow::setLibraryLocations()
{
    QSettings settings;
    LibrarySettingsDialog dialog(settings, this);
    connect(&dialog, SIGNAL(scanLibrary()), this, SLOT(onLibrarySearch()), Qt::UniqueConnection);

    dialog.exec();
}

//-----------------------------------------------------------------------------
// Function: MainWindow::createGeneratorPluginActions()
//-----------------------------------------------------------------------------
void MainWindow::createGeneratorPluginActions()
{
    pluginActionGroup_ = new QActionGroup(this);

    foreach (IPlugin* plugin, pluginMgr_->getActivePlugins())
    {
        IGeneratorPlugin* genPlugin = dynamic_cast<IGeneratorPlugin*>(plugin);

        if (genPlugin != 0)
        {
            // Add a small overlay icon to the plugin icon to visualize that this is a plugin.
            QIcon pluginBaseIcon = genPlugin->getIcon();

            QPixmap icon(24, 24);
            icon.fill(Qt::transparent);

            QPainter painter(&icon);
            painter.drawPixmap(0, 0, pluginBaseIcon.pixmap(24, 24));
            painter.drawPixmap(14, 14, QPixmap(":icons/common/graphics/generator_plugin_overlay.png"));

            QAction* action = new QAction(icon, genPlugin->getName(), this);
            action->setData(qVariantFromValue((void*)genPlugin));
            generationGroup_->addAction(action);
            pluginActionGroup_->addAction(action);
        }
    }

    connect(pluginActionGroup_, SIGNAL(triggered(QAction*)),
            this, SLOT(runGeneratorPlugin(QAction*)), Qt::UniqueConnection);
}


//-----------------------------------------------------------------------------
// Function: MainWindow::updateGeneratorPluginActions()
//-----------------------------------------------------------------------------
void MainWindow::updateGeneratorPluginActions()
{
    // Remove all plugin actions.
    QList<QAction*> actions = generationGroup_->actions();

    foreach (QAction* action, actions)
    {
        // Check if the action contains the plugin pointer.
        if (action->data().value<void*>() != 0)
        {
            generationGroup_->removeAction(action);
            delete action;
        }
    }

    delete pluginActionGroup_;
    
    // Recreate the plugin actions.
    createGeneratorPluginActions();
}
