//-----------------------------------------------------------------------------
// File: mainwindow.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti Maatta
// Date: 24.2.2011
//
// Description:
// The main window of Kactus2.
//-----------------------------------------------------------------------------

#include "mainwindow.h"

#include "SplashScreen.h"

#include "VersionHelper.h"

#include <mainwindow/DockWidgetHandler.h>
#include <mainwindow/Ribbon/Ribbon.h>
#include <mainwindow/Ribbon/RibbonGroup.h>
#include <mainwindow/NewPages/NewCatalogPage.h>
#include <mainwindow/NewPages/NewComponentPage.h>
#include <mainwindow/NewPages/NewDesignPage.h>
#include <mainwindow/NewPages/NewSWComponentPage.h>
#include <mainwindow/NewPages/NewSWDesignPage.h>
#include <mainwindow/NewPages/NewSystemPage.h>
#include <mainwindow/NewPages/NewComDefinitionPage.h>
#include <mainwindow/NewPages/NewApiDefinitionPage.h>
#include <mainwindow/NewPages/NewBusDefinitionPage.h>
#include <mainwindow/DrawingBoard/DrawingBoard.h>
#include <mainwindow/SaveHierarchy/DocumentTreeBuilder.h>
#include <mainwindow/SaveHierarchy/SaveHierarchyDialog.h>

#include <common/NameGenerationPolicy.h>
#include <common/dialogs/LibrarySettingsDialog/LibrarySettingsDialog.h>
#include <common/dialogs/NewDesignDialog/NewDesignDialog.h>
#include <common/dialogs/newObjectDialog/newobjectdialog.h>
#include <common/dialogs/listSelectDialog/ListSelectDialog.h>
#include <common/dialogs/propertyPageDialog/PropertyPageDialog.h>
#include <common/graphicsItems/ComponentItem.h>
#include <common/graphicsItems/GraphicsColumnConstants.h>
#include <common/widgets/assistedTextEdit/HighlightStyleDesc.h>

#include <KactusAPI/KactusAPI.h>

#include <editors/common/DesignWidgetFactoryImplementation.h>
#include <editors/HWDesign/HWDesignWidget.h>
#include <editors/HWDesign/HWDesignDiagram.h>
#include <editors/SystemDesign/SystemDesignWidget.h>
#include <editors/SystemDesign/SystemDesignDiagram.h>
#include <editors/MemoryDesigner/MemoryDesignDocument.h>

#include <editors/ApiDefinitionEditor/ApiDefinitionEditor.h>
#include <editors/BusDefinitionEditor/BusDefinitionEditor.h>
#include <editors/ComDefinitionEditor/ComDefinitionEditor.h>
#include <editors/CatalogEditor/CatalogEditor.h>
#include <editors/ComponentEditor/componenteditor.h>
#include <editors/CSourceEditor/CSourceWidget.h>
#include <KactusAPI/include/ExpressionFormatterFactoryImplementation.h>
#include <editors/ConfigurationTools/ViewConfigurer.h>

#include <KactusAPI/include/IGeneratorPlugin.h>
#include <KactusAPI/include/PluginUtilityAdapter.h>
// #include <Plugins/PluginSystem/ConsolePluginUtility.h>

#include <settings/SettingsDialog.h>


#include <wizards/ComponentWizard/ComponentWizard.h>
#include <wizards/ImportWizard/ImportWizard.h>

#include <kactusGenerators/DocumentGenerator/documentgenerator.h>

#include <KactusAPI/include/LibraryHandler.h>
#include <library/LibraryUtils.h>

#include <IPXACTmodels/common/VLNV.h>
#include <IPXACTmodels/common/ConfigurableVLNVReference.h>

#include <IPXACTmodels/Catalog/Catalog.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Model.h>
#include <IPXACTmodels/Component/FileSet.h>
#include <IPXACTmodels/Component/File.h>
#include <IPXACTmodels/Component/View.h>

#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>
#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>
#include <IPXACTmodels/BusDefinition/BusDefinition.h>

#include <IPXACTmodels/kactusExtensions/ComDefinition.h>
#include <IPXACTmodels/kactusExtensions/ApiDefinition.h>
#include <IPXACTmodels/kactusExtensions/ApiInterface.h>
#include <IPXACTmodels/kactusExtensions/SystemView.h>

#include <QCoreApplication>
#include <QApplication>
#include <QSettings>
#include <QStringList>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QDockWidget>
#include <QMessageBox>
#include <QFileInfo>
#include <QFile>
#include <QTextStream>
#include <QUrl>
#include <QCursor>
#include <QDesktopServices>
#include <QPainter>
#include <QDateTime>
#include <QStatusBar>

//-----------------------------------------------------------------------------
// Function: MainWindow::MainWindow()
//-----------------------------------------------------------------------------
MainWindow::MainWindow(LibraryHandler* library, MessageMediator* messageChannel, QWidget *parent):
QMainWindow(parent),
libraryHandler_(library),
designTabs_(0),
dockHandler_(new DockWidgetHandler(library, messageChannel, this)),
ribbon_(0), 
statusBar_(new QStatusBar(this)),
actNew_(0),
actSave_(0),
actSaveAs_(0),
actSaveHierarchy_(0),
actPrint_(0),
actImageExport_(0),
editGroup_(0),
actUndo_(0),
actRedo_(0),
actLibraryLocations_(0),
actLibrarySearch_(0),
actCheckIntegrity_(0),
generationGroup_(0),
pluginActionGroup_(0),
actGenDocumentation_(0),
actRunImport_(0),
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
openMemoryDesignerAction_(0),
actWorkspaces_(0),
protectGroup_(0),
actRefresh_(0),
actProtect_(0),
actSettings_(0),
actAbout_(0),
actHelp_(0),
actExit_(0),
configurationToolsGroup_(0),
actionConfigureViews_(0),
filteringGroup_(0),
actionFilterAddressSpaceChains_(0),
actionCondenseMemoryItems_(0),
actionCondenseFieldItems_(0),
actionExtendFieldItems_(0),
actionFilterSegments_(0),
actionFilterAddressBlocks_(0),
actionFilterRegisters_(0),
actionFilterFields_(0),
actionFilterUnconnectedMemoryItems_(0),
windowsMenu_(this),
visibilityMenu_(this),
workspaceMenu_(this),
workspace_(this, dockHandler_),
messageChannel_(messageChannel)
{    
    setWindowTitle(QCoreApplication::applicationName());
    setWindowIcon(QIcon(":icons/common/graphics/appicon.png"));

    // By default, the window is 1024x768 and set to maximized state.
    resize(1024, 768);
    setWindowState(Qt::WindowMaximized);

    QString defaultStyleSheet(
        "QCheckBox::indicator:unchecked { image: url(:icons/common/graphics/traffic-light_gray.png);}"
        "QCheckBox::indicator:indeterminate { image: url(:icons/common/graphics/traffic-light_green_gray.png);}"
        "QCheckBox::indicator:checked { image: url(:icons/common/graphics/traffic-light_green.png);}"
        "QGroupBox::title { subcontrol-origin: margin; margin: 0 8px; }"
        "QGroupBox::indicator:unchecked {image: url(:icons/common/graphics/traffic-light_gray.png);}"        
        "QGroupBox::indicator:checked {image: url(:icons/common/graphics/traffic-light_green.png);}"
        "QTableView::indicator:checked {image: url(:icons/common/graphics/checkMark.png);}"
        "QTableView::indicator:unchecked {image: none;}"
        "QDockWidget::title {background-color: #89B6E2; font-size: 18pt; padding-left: 2px; padding-top: 2px;}"
        "*[mandatoryField=\"true\"] { background-color: LemonChiffon; }");
    setStyleSheet(defaultStyleSheet);

    // Setup windows.
    setupDrawBoard();
    dockHandler_->setupDockWidgets();
    connectDockHandler();
    setupAndConnectLibraryHandler();

    setStatusBar(statusBar_);

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

//-----------------------------------------------------------------------------
// Function: mainwindow::onLibrarySearch()
//-----------------------------------------------------------------------------
void MainWindow::onLibrarySearch()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    libraryHandler_->searchForIPXactFiles();
    QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------
// Function: restoreSettings()
//-----------------------------------------------------------------------------
void MainWindow::restoreSettings()
{
    workspace_.restoreSettings();
    updateWorkspaceMenu();
    actWorkspaces_->setText(workspace_.getCurrentWorkspace());

    dockHandler_->applySettings();
}

//-----------------------------------------------------------------------------
// Function: saveSettings()
//-----------------------------------------------------------------------------
void MainWindow::saveSettings()
{
    workspace_.saveSettings();
}

//-----------------------------------------------------------------------------
// Function: updateWorkspaceMenu()
//-----------------------------------------------------------------------------
void MainWindow::updateWorkspaceMenu()
{
    workspace_.updateWorkspaceMenu(&workspaceMenu_);
}

//-----------------------------------------------------------------------------
// Function: loadWorkspace()
//-----------------------------------------------------------------------------
void MainWindow::loadWorkspace(QString const& workspaceName)
{
    workspace_.loadWorkspace(workspaceName);

    updateWindows();
}

//-----------------------------------------------------------------------------
// Function: mainwindow::setupActions()
//-----------------------------------------------------------------------------
void MainWindow::setupActions()
{
    // the action to create a new hierarchical component
    actNew_ = new QAction(QIcon(":/icons/common/graphics/file-new.png"), tr("New (Ctrl+N)"), this);
    actNew_->setShortcut(QKeySequence::New);
    connect(actNew_, SIGNAL(triggered()), this, SLOT(createNew()));

    actSave_ = new QAction(QIcon(":/icons/common/graphics/file-save.png"), tr("Save (Ctrl+S)"), this);
    actSave_->setShortcut(QKeySequence::Save);
    actSave_->setEnabled(false);
    connect(actSave_, SIGNAL(triggered()), designTabs_, SLOT(saveCurrentDocument()));
    connect(designTabs_, SIGNAL(documentModifiedChanged(bool)),
        actSave_, SLOT(setEnabled(bool)), Qt::UniqueConnection);

    actSaveAs_ = new QAction(QIcon(":/icons/common/graphics/file-save-as.png"), tr("Save As"), this);
    actSaveAs_->setShortcut(QKeySequence::SaveAs);
    actSaveAs_->setEnabled(false);
    connect(actSaveAs_, SIGNAL(triggered()), designTabs_, SLOT(saveCurrentDocumentAs()));

    actSaveHierarchy_ = new QAction(QIcon(":/icons/common/graphics/file-save-hierarchical.png"),
        tr("Save Hierarchy"), this);
    actSaveHierarchy_->setEnabled(false);
    connect(actSaveHierarchy_, SIGNAL(triggered()), this, SLOT(saveCurrentDocumentHierarchy()));

    actSaveAll_ = new QAction(QIcon(":/icons/common/graphics/file-save_all.png"),
        tr("Save All (Ctrl+Shift+S)"), this);
    actSaveAll_->setShortcut(QKeySequence("Ctrl+Shift+S"));
    connect(actSaveAll_, SIGNAL(triggered()), this, SLOT(saveAll()));

    actPrint_ = new QAction(QIcon(":/icons/common/graphics/file-print.png"), tr("Print (Ctrl+P)"), this);
    actPrint_->setShortcut(QKeySequence::Print);
    actPrint_->setEnabled(false);
    connect(actPrint_, SIGNAL(triggered()), designTabs_, SLOT(printCurrentDocument()));

    actImageExport_ = new QAction(QIcon(":/icons/common/graphics/export.png"), tr("Export image"), this);
    actImageExport_->setEnabled(false);
    connect(actImageExport_, SIGNAL(triggered()), designTabs_, SLOT(exportCurrentDocumentAsImage()));

    actUndo_ = new QAction(QIcon(":/icons/common/graphics/edit-undo.png"), tr("Undo (Ctrl+Z)"), this);
    actUndo_->setShortcut(QKeySequence::Undo);
    connect(actUndo_, SIGNAL(triggered()), this, SLOT(undo()));

    actRedo_ = new QAction(QIcon(":/icons/common/graphics/edit-redo.png"),
        tr("Redo (") + QKeySequence(QKeySequence::Redo).toString() +")", this);
    actRedo_->setShortcut(QKeySequence::Redo);
    connect(actRedo_, SIGNAL(triggered()), this, SLOT(redo()));

    actLibraryLocations_ = new QAction(QIcon(":/icons/common/graphics/library-config.png"),
        tr("Configure Library"), this);
    connect(actLibraryLocations_, SIGNAL(triggered()), this, SLOT(setLibraryLocations()), Qt::UniqueConnection);

    // the action to search for IP-Xact documents in file system
    actLibrarySearch_ = new QAction(QIcon(":/icons/common/graphics/library-refresh.png"),
        tr("Refresh Library"), this);
    connect(actLibrarySearch_, SIGNAL(triggered()),	this, SLOT(onLibrarySearch()), Qt::UniqueConnection);

    // Check the library integrity
    actCheckIntegrity_ = new QAction(QIcon(":/icons/common/graphics/checkIntegrity.png"),
        tr("View Library Integrity Report"), this);
    connect(actCheckIntegrity_, SIGNAL(triggered()),
        dockHandler_, SIGNAL(generateIntegrityReport()), Qt::UniqueConnection);

    // initialize the action to generate documentation for the component/design
    actGenDocumentation_ = new QAction(QIcon(":icons/common/graphics/documentation.png"),
        tr("Generate Documentation"), this);
    connect(actGenDocumentation_, SIGNAL(triggered()), this, SLOT(generateDoc()), Qt::UniqueConnection);

    // initialize the action to run import wizard for component.
    actRunImport_ = new QAction(QIcon(":icons/common/graphics/import.png"), tr("Import File to Component"), this);
    connect(actRunImport_, SIGNAL(triggered()), this, SLOT(onRunImportWizard()), Qt::UniqueConnection);

    // Initialize the action to set draw mode to selection mode.
    actToolSelect_ = new QAction(QIcon(":/icons/common/graphics/tool-select.png"), tr("Select Tool"), this);
    actToolSelect_->setCheckable(true);
    actToolSelect_->setChecked(true);

    // Initialize the action to add a new column.
    actAddColumn_ = new QAction(QIcon(":/icons/common/graphics/diagram-add-column.png"), tr("Add Column"), this);
    connect(actAddColumn_, SIGNAL(triggered()), this, SLOT(addColumn()), Qt::UniqueConnection);

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

    //! Initialize the action to condense memory graphics items.
    actionCondenseMemoryItems_ = new QAction(QIcon(":icons/common/graphics/compressMemoryItems.png"),
        tr("Compress Memory Items"), this);
    actionCondenseMemoryItems_->setCheckable(true);
    connect(actionCondenseMemoryItems_, SIGNAL(triggered(bool)),
        this, SLOT(onCondenseMemoryItems(bool)), Qt::UniqueConnection);

    //! Initialize the action to filter chained address space memory connections.
    actionFilterAddressSpaceChains_ = new QAction(QIcon(":/icons/common/graphics/addressSpaceFilter.png"),
        tr("Address Space Filter"), this);
    actionFilterAddressSpaceChains_->setCheckable(true);
    connect(actionFilterAddressSpaceChains_, SIGNAL(triggered(bool)),
        this, SLOT(onFilterAddressSpaceChains(bool)), Qt::UniqueConnection);

    //! Initialize the action to condense field graphics items.
    actionCondenseFieldItems_ = new QAction(QIcon(":icons/common/graphics/compressFields.png"),
        tr("Compress Field Items"), this);
    connect(actionCondenseFieldItems_, SIGNAL(triggered()),
        this, SLOT(onCondenseFieldItems()), Qt::UniqueConnection);

    //! Initialize the action to extend field graphics items.
    actionExtendFieldItems_ = new QAction(QIcon(":icons/common/graphics/extendFields.png"),
        tr("Extend Field Items"), this);
    connect(actionExtendFieldItems_, SIGNAL(triggered()), this, SLOT(onExtendFieldItems()), Qt::UniqueConnection);

    //! Initialize the action to filter address space segments.
    actionFilterSegments_ = new QAction(QIcon(":icons/common/graphics/filterSegments.png"),
        tr("Filter Segments"), this);
    actionFilterSegments_->setCheckable(true);
    connect(actionFilterSegments_, SIGNAL(triggered(bool)),
        this, SLOT(onFilterSegments(bool)), Qt::UniqueConnection);

    //! Initialize the action to filter memory map address blocks.
    actionFilterAddressBlocks_ = new QAction(QIcon(":icons/common/graphics/filterAddressBlocks.png"),
        tr("Filter Address Blocks"), this);
    actionFilterAddressBlocks_->setCheckable(true);
    connect(actionFilterAddressBlocks_, SIGNAL(triggered(bool)),
        this, SLOT(onFilterAddressBlocks(bool)), Qt::UniqueConnection);

    //! Initialize the action to filter address block registers.
    actionFilterRegisters_ = new QAction(QIcon(":icons/common/graphics/filterRegisters.png"),
        tr("Filter Registers"), this);
    actionFilterRegisters_->setCheckable(true);
    connect(actionFilterRegisters_, SIGNAL(triggered(bool)),
        this, SLOT(onFilterRegisters(bool)), Qt::UniqueConnection);

    //! Initialize the action to filter register fields.
    actionFilterFields_ = new QAction(QIcon(":icons/common/graphics/filterFields.png"), tr("Filter Fields"), this);
    actionFilterFields_->setCheckable(true);
    connect(actionFilterFields_, SIGNAL(triggered(bool)), this, SLOT(onFilterFields(bool)), Qt::UniqueConnection);

    //! Initialize the action to filter unconnected memory items.
    actionFilterUnconnectedMemoryItems_ = new QAction(QIcon(
        ":icons/common/graphics/filterUnconnectedMemoryItems.png"), tr("Filter unconnected items"), this);
    actionFilterUnconnectedMemoryItems_->setCheckable(true);
    connect(actionFilterUnconnectedMemoryItems_, SIGNAL(triggered(bool)),
        this, SLOT(onFilterUnconnectedMemoryItems(bool)), Qt::UniqueConnection);

    // Initialize the action to zoom in.
    actZoomIn_ = new QAction(QIcon(":/icons/common/graphics/view-zoom_in.png"), tr("Zoom In"), this);
    actZoomIn_->setEnabled(false);
    actZoomIn_->setVisible(false);
    connect(actZoomIn_, SIGNAL(triggered()), this, SLOT(zoomIn()));

    // Initialize the action to zoom out.
    actZoomOut_ = new QAction(QIcon(":/icons/common/graphics/view-zoom_out.png"), tr("Zoom Out"), this);
    actZoomOut_->setEnabled(false);
    actZoomOut_->setVisible(false);
    connect(actZoomOut_, SIGNAL(triggered()), this, SLOT(zoomOut()));

    // Initialize the action to reset the zoom to original 1:1 ratio.
    actZoomOriginal_ = new QAction(QIcon(":/icons/common/graphics/view-zoom_original.png"),
        tr("Original 1:1 Zoom"), this);
    actZoomOriginal_->setEnabled(false);
    actZoomOriginal_->setVisible(false);
    connect(actZoomOriginal_, SIGNAL(triggered()), this, SLOT(zoomOriginal()));

    // Initialize the action to fit the document into the view.
    actFitInView_ = new QAction(QIcon(":/icons/common/graphics/view-fit_best.png"),
        tr("Fit Document to View"), this);
    actFitInView_->setEnabled(false);
    actFitInView_->setVisible(false);
    connect(actFitInView_, SIGNAL(triggered()), this, SLOT(fitInView()));

    // the action for user to select the visible docks
    actVisibleDocks_ = new QAction(QIcon(":icons/common/graphics/dockSelect.png"), tr("Visible Windows"), this);
    actVisibleDocks_->setMenu(&windowsMenu_);
    connect(actVisibleDocks_, SIGNAL(triggered()), this, SLOT(selectVisibleDocks()), Qt::UniqueConnection);

    // Initialize the action to manage visibility control.
    actVisibilityControl_ = new QAction(QIcon(":icons/common/graphics/visibility.png"), tr("Visibility Control"), this);
    actVisibilityControl_->setEnabled(false);
    actVisibilityControl_->setVisible(false);
    actVisibilityControl_->setMenu(&visibilityMenu_);
    connect(actVisibilityControl_, SIGNAL(triggered()), this, SLOT(openVisibilityControlMenu()), Qt::UniqueConnection);
    connect(&visibilityMenu_, SIGNAL(triggered(QAction*)), this, SLOT(onVisibilityControlToggled(QAction*)));

    // Initialize the action for opening memory designer.
    openMemoryDesignerAction_ = new QAction(QIcon(":icons/common/graphics/memoryDesigner.png"), tr("Open Memory Designer"), this);
    openMemoryDesignerAction_->setVisible(false);
    connect(openMemoryDesignerAction_, SIGNAL(triggered()), this, SLOT(openMemoryDesign()), Qt::UniqueConnection);

    // Initialize the action to manage workspaces.
    actWorkspaces_ = new QAction(QIcon(":icons/common/graphics/workspace.png"),	tr("Workspaces"), this);
    connect(actWorkspaces_, SIGNAL(triggered()), this, SLOT(openWorkspaceMenu()), Qt::UniqueConnection);
    actWorkspaces_->setMenu(&workspaceMenu_);

    actRefresh_ = new QAction(QIcon(":/icons/common/graphics/refresh.png"), tr("Refresh (F5)"), this);
    actRefresh_->setShortcut(QKeySequence("F5"));
    connect(actRefresh_, SIGNAL(triggered(bool)), designTabs_, SLOT(refreshCurrentDocument()));

    actProtect_ = new QAction(QIcon(":/icons/common/graphics/protection-unlocked.png"), tr("Unlocked (Ctrl+L)"), this);
    actProtect_->setCheckable(true);
    actProtect_->setEnabled(false);
    actProtect_->setShortcut(QKeySequence("Ctrl+L"));
    connect(actProtect_, SIGNAL(triggered(bool)), this, SLOT(changeProtection(bool)));

    // Initialize the action to open Kactus2 settings.
    actSettings_ = new QAction(QIcon(":/icons/common/graphics/system-settings.png"), tr("Settings"), this);
    connect(actSettings_, SIGNAL(triggered()), this, SLOT(openSettings()));

    // Initialize the action to open the about box.
    actAbout_= new QAction(QIcon(":/icons/common/graphics/system-about.png"), tr("About"), this);
    connect(actAbout_, SIGNAL(triggered()), this, SLOT(showAbout()), Qt::UniqueConnection);

    // Initialize the action to open the help window.
    actHelp_= new QAction(QIcon(":/icons/common/graphics/system-help.png"), tr("Help (F1)"), this);
    actHelp_->setShortcut(QKeySequence::HelpContents);
    connect(actHelp_, SIGNAL(triggered()), dockHandler_, SLOT(showHelp()), Qt::UniqueConnection);

    // Initialize the action to exit the program.
    actExit_ = new QAction(QIcon(":/icons/common/graphics/system-exit.png"), tr("Exit"), this);
    connect(actExit_, SIGNAL(triggered()), this, SLOT(close()), Qt::UniqueConnection);

    actionConfigureViews_ = new QAction(QIcon(":/icons/common/graphics/viewConfiguration.png"),
        tr("View Configuration"), this);
    connect(actionConfigureViews_, SIGNAL(triggered()), this, SLOT(onConfigureViews()), Qt::UniqueConnection);

    connectVisibilityControls();

    setupMenus();
}

//-----------------------------------------------------------------------------
// Function: mainwindow::onAdjustVisibilityInWindow()
//-----------------------------------------------------------------------------
void MainWindow::onAdjustVisibilityInWindow(TabDocument::SupportedWindows type, bool show)
{
    int tabCount = designTabs_->count();
    QWidget* currentWidget = 0;
    if (tabCount > 0)
    {
        currentWidget = designTabs_->currentWidget();
    }

    dockHandler_->setWindowVisibilityForSupportedWindow(tabCount, currentWidget, type, show);
}

//-----------------------------------------------------------------------------
// Function: setupMenus()
//-----------------------------------------------------------------------------
void MainWindow::setupMenus()
{
    ribbon_ = new Ribbon(this);
    addToolBar(ribbon_);

    // The "File" group.
    RibbonGroup* fileGroup = new RibbonGroup(tr("File"), ribbon_);
    fileGroup->addAction(actNew_);
    fileGroup->addAction(actSave_);
    fileGroup->addAction(actSaveAs_);
    fileGroup->addAction(actSaveAll_);
    fileGroup->addAction(actSaveHierarchy_);
    fileGroup->addAction(actPrint_);
    fileGroup->addAction(actImageExport_);
    ribbon_->addGroup(fileGroup);

    // The "Library" group.
    RibbonGroup* libGroup = new RibbonGroup(tr("Library"), ribbon_);
    libGroup->addAction(actLibraryLocations_);
    libGroup->addAction(actLibrarySearch_);
    libGroup->addAction(actCheckIntegrity_);
    ribbon_->addGroup(libGroup);

    // The "protection" group
    protectGroup_ = new RibbonGroup(tr("Protection"), ribbon_);
    protectGroup_->addAction(actProtect_);

    protectAction_ = ribbon_->addGroup(protectGroup_); 
    protectAction_->setVisible(false);

    // The "Edit" group.
    editGroup_ = new RibbonGroup(tr("Edit"), ribbon_);
    editGroup_->addAction(actRefresh_);
    editGroup_->addAction(actUndo_);
    editGroup_->addAction(actRedo_);

    editAction_ = ribbon_->addGroup(editGroup_);
    editAction_->setVisible(false);
    editAction_->setEnabled(false);

    // The "Generation" group.
    generationGroup_ = new RibbonGroup(tr("Generation"), ribbon_);
    generationGroup_->addAction(actGenDocumentation_);
    generationGroup_->addAction(actRunImport_);

    generationAction_ = ribbon_->addGroup(generationGroup_);
    generationAction_->setVisible(false);
    generationAction_->setEnabled(false);

    createGeneratorPluginActions();

    //! The "Diagram Tools" group.
    diagramToolsGroup_ = new RibbonGroup(tr("Diagram Tools"), ribbon_);
    diagramToolsGroup_->addAction(actToolSelect_);
    diagramToolsGroup_->addAction(actAddColumn_);
    diagramToolsGroup_->addAction(actToolConnect_);
    diagramToolsGroup_->addAction(actToolInterface_);
    diagramToolsGroup_->addAction(actToolDraft_);
    diagramToolsGroup_->addAction(actToolToggleOffPage_);
    diagramToolsGroup_->addAction(actToolLabel_);

    diagramToolsAction_ = ribbon_->addGroup(diagramToolsGroup_);
    diagramToolsAction_->setVisible(false);

    //! The "Filtering tools" group.
    filteringGroup_ = new RibbonGroup(tr("Filtering Tools"), ribbon_);
    filteringGroup_->addAction(actionFilterSegments_);
    filteringGroup_->addAction(actionFilterAddressBlocks_);
    filteringGroup_->addAction(actionFilterRegisters_);
    filteringGroup_->addAction(actionFilterFields_);
    filteringGroup_->addAction(actionFilterUnconnectedMemoryItems_);
    filteringGroup_->addAction(actionFilterAddressSpaceChains_);
    filteringGroup_->addAction(actionCondenseMemoryItems_);
    filteringGroup_->addAction(actionCondenseFieldItems_);
    filteringGroup_->addAction(actionExtendFieldItems_);

    filteringAction_ = ribbon_->addGroup(filteringGroup_);
    filteringAction_->setVisible(false);
    filteringAction_->setEnabled(true);

    //! The "View" group.
    RibbonGroup* viewGroup = new RibbonGroup(tr("View"), ribbon_);
    viewGroup->addAction(actVisibleDocks_);
    viewGroup->addAction(actZoomIn_);
    viewGroup->addAction(actZoomOut_);
    viewGroup->addAction(actZoomOriginal_);
    viewGroup->addAction(actFitInView_);
    viewGroup->addAction(actVisibilityControl_);

    ribbon_->addGroup(viewGroup);

    //! The "Configuration tools" group.
    configurationToolsGroup_ = new RibbonGroup(tr("Configuration Tools"), ribbon_);
    configurationToolsGroup_->addAction(actionConfigureViews_);
    configurationToolsGroup_->addAction(openMemoryDesignerAction_);

    configurationToolsAction_ = ribbon_->addGroup(configurationToolsGroup_);
    configurationToolsAction_->setVisible(false);
    configurationToolsAction_->setEnabled(false);

    //! The "Workspace" group.
    RibbonGroup* workspacesGroup = new RibbonGroup(tr("Workspace"), ribbon_);
    workspacesGroup->addAction(actWorkspaces_);
    workspacesGroup->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    ribbon_->addGroup(workspacesGroup);

    connect(&workspace_, SIGNAL(requestMenuUpdate()), this, SLOT(updateWorkspaceMenu()), Qt::UniqueConnection);
    connect(&workspace_, SIGNAL(workspaceChanged(QString const&)), 
        this, SLOT(onWorkspaceChanged(QString const&)), Qt::UniqueConnection);

    //! The "System" group.
    RibbonGroup* sysGroup = new RibbonGroup(tr("System"), ribbon_);
    sysGroup->addAction(actSettings_);
    sysGroup->addAction(actHelp_);
    sysGroup->addAction(actAbout_);
    sysGroup->addAction(actExit_);

    ribbon_->addGroup(sysGroup);

    // the menu to display the dock widgets
    dockHandler_->setupVisibilityActionMenu(windowsMenu_);
}

//-----------------------------------------------------------------------------
// Function: MainWindow::setupDrawBoard()
//-----------------------------------------------------------------------------
void MainWindow::setupDrawBoard()
{
    designTabs_ = new DrawingBoard();
    designTabs_->setMovable(true);
    designTabs_->setTabsClosable(true);

    setCentralWidget(designTabs_);

    connect(designTabs_, SIGNAL(lastDocumentClosed()), this, SLOT(onLastDocumentClosed()), Qt::UniqueConnection);
    connect(designTabs_, SIGNAL(currentChanged(int)), this, SLOT(onDocumentChanged(int)), Qt::UniqueConnection);

    connect(designTabs_, SIGNAL(documentContentChanged()), this, SLOT(updateMenuStrip()), Qt::UniqueConnection);
    connect(designTabs_, SIGNAL(documentEditStateChanged()), this, SLOT(updateMenuStrip()));

    connect(designTabs_, SIGNAL(helpUrlRequested(QString const&)),
        this, SIGNAL(helpUrlRequested(QString const&)), Qt::UniqueConnection);

    connect(designTabs_, SIGNAL(errorMessage(const QString&)),
        dockHandler_, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
    connect(designTabs_, SIGNAL(noticeMessage(const QString&)),
        dockHandler_, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: mainwindow::connectDockHandler()
//-----------------------------------------------------------------------------
void MainWindow::connectDockHandler()
{
    connect(this, SIGNAL(errorMessage(QString const&)),
        dockHandler_, SIGNAL(errorMessage(QString const&)), Qt::UniqueConnection);
    connect(this, SIGNAL(noticeMessage(QString const&)),
        dockHandler_, SIGNAL(noticeMessage(QString const&)), Qt::UniqueConnection);

    connect(this, SIGNAL(helpUrlRequested(QString const&)),
        dockHandler_, SIGNAL(helpUrlRequested(QString const&)), Qt::UniqueConnection);

    connect(dockHandler_, SIGNAL(designChanged()), this, SLOT(onDesignChanged()), Qt::UniqueConnection);

    connect(dockHandler_, SIGNAL(statusMessage(QString const&)),
        statusBar_, SLOT(showMessage(QString const&)));
}

//-----------------------------------------------------------------------------
// Function: mainwindow::setupAndConnectLibraryHandler()
//-----------------------------------------------------------------------------
void MainWindow::setupAndConnectLibraryHandler()
{
    connect(libraryHandler_, SIGNAL(openDesign(const VLNV&, const QString&)),
        this, SLOT(openDesign(const VLNV&, const QString&)));
    connect(libraryHandler_, SIGNAL(openMemoryDesign(const VLNV&, const QString&)),
        this, SLOT(openMemoryDesign(const VLNV&, const QString&)));

    connect(libraryHandler_, SIGNAL(openComponent(const VLNV&)),
        this, SLOT(openComponent(const VLNV&)), Qt::UniqueConnection);
    connect(libraryHandler_, SIGNAL(openCatalog(const VLNV&)),
        this, SLOT(openCatalog(const VLNV&)), Qt::UniqueConnection);
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

    connect(libraryHandler_, SIGNAL(openApiDefinition(const VLNV&)),
        this, SLOT(openApiDefinition(const VLNV&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: mainwindow::onDesignChanged()
//-----------------------------------------------------------------------------
void MainWindow::onDesignChanged()
{
    // find the currently open editor
    QWidget* widget = designTabs_->currentWidget();

    // if editor was found
    if (widget)
    {
        DesignWidget* editor = dynamic_cast<DesignWidget*>(widget);

        // if editor is design widget then set it to be modified.
        if (editor)
        {
            editor->setModified(true);
            updateMenuStrip();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MainWindow::saveCurrentDocumentHierarchy()
//-----------------------------------------------------------------------------
void MainWindow::saveCurrentDocumentHierarchy()
{
    TabDocument* currentDocument = dynamic_cast<TabDocument*>(designTabs_->currentWidget());

    if (currentDocument)
    {
        DocumentTreeBuilder documentTreeBuilder(libraryHandler_);
        QObject* documentRoot = documentTreeBuilder.createFrom(currentDocument->getDocumentVLNV());

        SaveHierarchyDialog saveDialog(documentRoot, libraryHandler_, this);
        saveDialog.exec();

        delete documentRoot;
    }
}

//-----------------------------------------------------------------------------
// Function: mainwindow::onClearItemSelection()
//-----------------------------------------------------------------------------
void MainWindow::onClearItemSelection()
{
    HWDesignWidget* designWidget = dynamic_cast<HWDesignWidget*>(designTabs_->currentWidget());

    dockHandler_->clearItemSelection(designWidget);
}

//-----------------------------------------------------------------------------
// Function: mainwindow::onComponentSelected()
//-----------------------------------------------------------------------------
void MainWindow::onComponentSelected(ComponentItem* component)
{
    Q_ASSERT(component);

    dockHandler_->selectComponent(designTabs_->currentWidget(), component);
}

//-----------------------------------------------------------------------------
// Function: mainwindow::onInterfaceSelected()
//-----------------------------------------------------------------------------
void MainWindow::onInterfaceSelected(ConnectionEndpoint* interface)
{
    Q_ASSERT(interface);

    dockHandler_->selectConnectionInterface(designTabs_->currentWidget(), interface);
}

//-----------------------------------------------------------------------------
// Function: onConnectionSelected()
//-----------------------------------------------------------------------------
void MainWindow::onConnectionSelected(GraphicsConnection* connection)
{
    dockHandler_->selectGraphicsConnection(designTabs_->currentWidget(), connection);
}

//-----------------------------------------------------------------------------
// Function: updateMenuStrip()
//-----------------------------------------------------------------------------
void MainWindow::updateMenuStrip()
{
    TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());
    bool unlocked = doc != 0 && (!(doc->getFlags() & TabDocument::DOC_PROTECTION_SUPPORT) || !doc->isProtected());

    bool isHWComp = false;
    ComponentEditor* componentEditor = dynamic_cast<ComponentEditor*>(doc);
    if (componentEditor)
    {
        isHWComp = componentEditor->isHWImplementation();
    }

    bool isHWDesign = dynamic_cast<HWDesignWidget*>(doc) != 0;
    bool isSystemDesign = dynamic_cast<SystemDesignWidget*>(doc) != 0;

    MemoryDesignDocument* memoryDocument = dynamic_cast<MemoryDesignDocument*>(doc);
    bool isMemoryDesign = memoryDocument != 0;

    actSave_->setEnabled(doc != 0 && doc->isModified());
    actSaveAs_->setEnabled(doc != 0);
    actSaveHierarchy_->setEnabled(componentEditor || dynamic_cast<DesignWidget*>(doc));
    actPrint_->setEnabled(doc != 0 && (doc->getFlags() & TabDocument::DOC_PRINT_SUPPORT));
    actImageExport_->setEnabled(doc != 0 && doc->getFlags() & TabDocument::DOC_PRINT_SUPPORT);

    generationAction_->setEnabled(unlocked);
    generationAction_->setVisible(doc != 0 && (componentEditor != 0 || isHWDesign || isSystemDesign));

    actGenDocumentation_->setEnabled((isHWDesign|| isHWComp) && unlocked);
    actGenDocumentation_->setVisible((isHWDesign|| isHWComp));

    actRunImport_->setEnabled(isHWComp && unlocked);
    actRunImport_->setVisible(isHWComp);

    openMemoryDesignerAction_->setVisible(isHWDesign);

    configurationToolsAction_->setEnabled(unlocked);
    configurationToolsAction_->setVisible(doc != 0 && (isHWComp || isHWDesign));
    actionConfigureViews_->setEnabled(unlocked);
    actionConfigureViews_->setVisible(isHWDesign || isHWComp);

    editAction_->setVisible(doc != 0);
    editAction_->setEnabled(doc != 0 && unlocked);
    actUndo_->setVisible(doc != 0 && doc->getEditProvider() != 0);
    actRedo_->setVisible(doc != 0 && doc->getEditProvider() != 0);
    actUndo_->setEnabled(doc != 0 && doc->getEditProvider() != 0 && doc->getEditProvider()->canUndo());
    actRedo_->setEnabled(doc != 0 && doc->getEditProvider() != 0 && doc->getEditProvider()->canRedo());

    diagramToolsAction_->setVisible(doc != 0 && (doc->getFlags() & TabDocument::DOC_DRAW_MODE_SUPPORT));
    diagramToolsAction_->setEnabled(doc != 0 && (doc->getFlags() & TabDocument::DOC_DRAW_MODE_SUPPORT) &&
        !doc->isProtected());
    actToolSelect_->setEnabled(doc != 0 && (doc->getSupportedDrawModes() & MODE_SELECT));
    actToolConnect_->setEnabled(doc != 0 && (doc->getSupportedDrawModes() & MODE_CONNECT));
    actToolInterface_->setEnabled(doc != 0 && (doc->getSupportedDrawModes() & MODE_INTERFACE));
    actToolDraft_->setEnabled(doc != 0 && (doc->getSupportedDrawModes() & MODE_DRAFT));
    actToolToggleOffPage_->setEnabled(doc != 0 && (doc->getSupportedDrawModes() & MODE_TOGGLE_OFFPAGE));
    actToolLabel_->setEnabled(doc != 0 && (doc->getSupportedDrawModes() & MODE_LABEL));

    bool oldProtectionState = actProtect_->isChecked();

    protectAction_->setVisible(doc != 0 && (doc->getFlags() & TabDocument::DOC_PROTECTION_SUPPORT));
    actProtect_->setEnabled(doc != 0 && (doc->getFlags() & TabDocument::DOC_PROTECTION_SUPPORT));
    actProtect_->setChecked(doc != 0 && (doc->getFlags() & TabDocument::DOC_PROTECTION_SUPPORT) &&
        doc->isProtected());

    if (oldProtectionState != actProtect_->isChecked())
        onProtectionChanged(actProtect_->isChecked());

    actVisibilityControl_->setEnabled(doc != 0 && (doc->getFlags() & TabDocument::DOC_VISIBILITY_CONTROL_SUPPORT));
    actVisibilityControl_->setVisible(doc != 0 && (doc->getFlags() & TabDocument::DOC_VISIBILITY_CONTROL_SUPPORT));

    setPluginVisibilities();

    updateZoomTools();

    if (isMemoryDesign)
    {
        generationAction_->setVisible(false);

        actionFilterAddressSpaceChains_->setChecked(memoryDocument->addressSpaceChainsAreFiltered());
        actionCondenseMemoryItems_->setChecked(memoryDocument->memoryItemsAreCondensed());
        actionFilterSegments_->setChecked(memoryDocument->addressSpaceSegmentsAreFilterted());
        actionFilterAddressBlocks_->setChecked(memoryDocument->addressBlocksAreFiltered());
        actionFilterRegisters_->setChecked(memoryDocument->addressBlockRegistersAreFiltered());
        actionFilterFields_->setChecked(memoryDocument->fieldsAreFiltered());
        actionFilterUnconnectedMemoryItems_->setChecked(memoryDocument->unconnectedMemoryItemsAreFiltered());
    }

    filteringAction_->setVisible(isMemoryDesign);
}

//-----------------------------------------------------------------------------
// Function: saveAll()
//-----------------------------------------------------------------------------
void MainWindow::saveAll()
{
    designTabs_->saveAll();

    TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());
    actSave_->setEnabled(doc && doc->isModified());
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
    QString viewName;

    // if the design is supported by the document type
    DesignWidget* desWidget = qobject_cast<DesignWidget*>(doc);
    if (desWidget)
    {
        // the vlnvs must be for different objects
        Q_ASSERT(compVLNV != desVLNV);

        // find the design config is one exists
        viewName = desWidget->getOpenViewName();
    }

    // Retrieve the plugin pointer from the action.
    IGeneratorPlugin* plugin = reinterpret_cast<IGeneratorPlugin*>(action->data().value<void*>());
    Q_ASSERT(plugin != 0);

    KactusAPI::runGenerator(plugin, compVLNV, viewName, QString(), this);

    // Refresh the document.
    doc->refresh();
}

//-----------------------------------------------------------------------------
// Function: MainWindow::generateDoc()
//-----------------------------------------------------------------------------
void MainWindow::generateDoc()
{
    emit noticeMessage(QString("Document generation started at %1.").arg(
        QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss")));

    // get the vlnv of the current component
    TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());
    Q_ASSERT(doc);
    VLNV vlnv = doc->getDocumentVLNV();
    Q_ASSERT(vlnv.isValid());

    // if changes have been made to the component
    if (doc->isModified())
    {
        QMessageBox::information(this, QCoreApplication::applicationName(),
            tr("Changes have been made to the component. Save the changes before generating documentation."));
        return;
    }

    const QString XMLPath = libraryHandler_->getPath(vlnv);
    QFileInfo xmlInfo(XMLPath);

    QString targetPath = QFileDialog::getSaveFileName(NULL, tr("Save the documentation to..."),
        xmlInfo.absolutePath(), tr("web pages (*.html)"));

    if (targetPath.isEmpty())
    {
        emit noticeMessage(tr("Generation aborted."));
        return;
    }

    QFile targetFile(targetPath);
    if (!targetFile.open(QFile::WriteOnly))
    {
        emit errorMessage(tr("Could not open file %1 for writing.").arg(targetPath));
        emit noticeMessage(tr("Generation aborted."));
        return;
    }

    QTextStream stream(&targetFile);

    DesignWidgetFactoryImplementation designWidgetFactory(libraryHandler_,
        dockHandler_->getDesignAndInstanceParameterFinder(), dockHandler_->getDesignParameterFinder());

    ExpressionFormatterFactoryImplementation expressionFormatterFactory;

    DocumentGenerator generator(libraryHandler_, vlnv, &designWidgetFactory, &expressionFormatterFactory, this);
    connect(&generator, SIGNAL(errorMessage(const QString&)),
        dockHandler_, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
    connect(&generator, SIGNAL(noticeMessage(const QString&)),
        dockHandler_, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

    generator.writeDocumentation(stream, targetPath);
    targetFile.close();

    emit noticeMessage(QString("Generation complete."));

    // open the generated document in user's default browser
    QDesktopServices::openUrl(QUrl::fromLocalFile(targetPath));

    // if the editor was component editor then it must be refreshed to make the changes to metadata visible
    ComponentEditor* compEditor = dynamic_cast<ComponentEditor*>(doc);

    if (compEditor)
    {
        designTabs_->refreshCurrentDocument();
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

//-----------------------------------------------------------------------------
// Function: MainWindow::onLastDocumentClosed()
//-----------------------------------------------------------------------------
void MainWindow::onLastDocumentClosed()
{
    updateWindows();
}

//-----------------------------------------------------------------------------
// Function: MainWindow::onDocumentChanged()
//-----------------------------------------------------------------------------
void MainWindow::onDocumentChanged(int index)
{
    TabDocument* doc = static_cast<TabDocument*>(designTabs_->widget(index));

    // update the menu
    if (doc)
    {
        updateWindows();
        updateVisibilityControlMenu(doc);
    }

    dockHandler_->documentChanged(doc);

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
    // disconnect the signals that would otherwise change the window states to be saved.
    disconnectVisibilityControls();

    // Go through all tab documents and ask the user what to do if they are not saved.
    designTabs_->closeAll();

    if (designTabs_->count() != 0)
    {
        event->ignore();
        return;
    }
}

//-----------------------------------------------------------------------------
// Function: updateZoomTools()
//-----------------------------------------------------------------------------
void MainWindow::updateZoomTools()
{
    TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());
    bool canZoom = doc != 0 && (doc->getFlags() & TabDocument::DOC_ZOOM_SUPPORT);

    actZoomIn_->setVisible(canZoom);
    actZoomOut_->setVisible(canZoom);
    actZoomOriginal_->setVisible(canZoom);
    actFitInView_->setVisible(canZoom);

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
    dialog.setWindowTitle(tr("New"));

    NewBusDefinitionPage* busPage = new NewBusDefinitionPage(libraryHandler_, &dialog);
    connect(busPage, SIGNAL(createBus(VLNV const&, QString const&)),
        this, SLOT(createBus(VLNV const&, QString const&)), Qt::UniqueConnection);
    dialog.addPage(QIcon(":icons/common/graphics/bus-def.png"), tr("Bus Definition"), busPage);

    NewCatalogPage* catalogPage = new NewCatalogPage(libraryHandler_, &dialog);
    connect(catalogPage, SIGNAL(createCatalog(VLNV const&, QString const&)),
        this, SLOT(createCatalog(VLNV const&, QString const&)), Qt::UniqueConnection);
    dialog.addPage(QIcon(":icons/common/graphics/catalog.png"), tr("Catalog"), catalogPage);

    // Add pages to the dialog.
    NewComponentPage* compPage = new NewComponentPage(libraryHandler_, &dialog);
    connect(compPage, SIGNAL(createComponent(KactusAttribute::ProductHierarchy, KactusAttribute::Firmness,
            QVector<TagData>, VLNV const&, QString const&)),
        this, SLOT(createComponent(KactusAttribute::ProductHierarchy, KactusAttribute::Firmness,
            QVector<TagData>, VLNV const&, QString const&)));
    dialog.addPage(QIcon(":icons/common/graphics/hw-component.png"), tr("HW Component"), compPage);

    NewDesignPage* designPage = new NewDesignPage(libraryHandler_, &dialog);
    connect(designPage, SIGNAL(createDesign(KactusAttribute::ProductHierarchy, KactusAttribute::Firmness,
            QVector<TagData>, VLNV const&, QString const&)),
        this, SLOT(createDesign(KactusAttribute::ProductHierarchy, KactusAttribute::Firmness,
            QVector<TagData>, VLNV const&, QString const&)));
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

    NewApiDefinitionPage* apiDefPage = new NewApiDefinitionPage(libraryHandler_, &dialog);
    connect(apiDefPage, SIGNAL(createApiDefinition(VLNV const&, QString const&)),
        this, SLOT(createApiDefinition(VLNV const&, QString const&)), Qt::UniqueConnection);
    dialog.addPage(QIcon(":icons/common/graphics/new-api_definition.png"), tr("API Definition"), apiDefPage);

    NewComDefinitionPage* comDefPage = new NewComDefinitionPage(libraryHandler_, &dialog);
    connect(comDefPage, SIGNAL(createComDefinition(VLNV const&, QString const&)),
            this, SLOT(createComDefinition(VLNV const&, QString const&)), Qt::UniqueConnection);
    dialog.addPage(QIcon(":icons/common/graphics/new-com_definition.png"), tr("COM Definition"), comDefPage);

    dialog.finalizePages();

    dialog.exec();
}

//-----------------------------------------------------------------------------
// Function: mainwindow::createComponent()
//-----------------------------------------------------------------------------
void MainWindow::createComponent(KactusAttribute::ProductHierarchy prodHier, KactusAttribute::Firmness firmness,
    QVector<TagData> tags, VLNV const& vlnv, QString const& directory)
{
    Q_ASSERT(vlnv.isValid());

    // Create a component.
    QSharedPointer<Component> component = QSharedPointer<Component>(new Component(vlnv));

    // Set Kactus attributes.
    component->setHierarchy(prodHier);
    component->setFirmness(firmness);
    component->setTags(tags);
    component->setImplementation(KactusAttribute::HW);

    component->setVersion(VersionHelper::versionFileStr());

    // Create the file.
    if (!libraryHandler_->writeModelToFile(directory, component))
    {
        emit errorMessage("Error saving file to disk.");
        return;
    }

    runComponentWizard(component, directory);

    // Open the component editor.
    openComponent(vlnv);

    unlockNewlyCreatedDocument(vlnv);
}

//-----------------------------------------------------------------------------
// Function: mainwindow::createDesign()
//-----------------------------------------------------------------------------
void MainWindow::createDesign(KactusAttribute::ProductHierarchy prodHier, KactusAttribute::Firmness firmness,
    QVector<TagData> tags, VLNV const& vlnv, QString const& directory)
{
    Q_ASSERT(vlnv.isValid());

    VLNV designVLNV(VLNV::DESIGN, vlnv.getVendor(), vlnv.getLibrary(),
        vlnv.getName().remove(".comp") + ".design", vlnv.getVersion());
    VLNV desConfVLNV(VLNV::DESIGNCONFIGURATION, vlnv.getVendor(), vlnv.getLibrary(),
        vlnv.getName().remove(".comp") + ".designcfg", vlnv.getVersion());

    // Create a component and a hierarchical view .
    QSharedPointer<Component> component(new Component(vlnv));

    // Set Kactus2 attributes.
    component->setHierarchy(prodHier);
    component->setFirmness(firmness);
    component->setTags(tags);

    component->setImplementation(KactusAttribute::HW);
    component->setVersion(VersionHelper::versionFileStr());

    QSharedPointer<View> newHierarchicalView(new View());
    newHierarchicalView->setName(NameGenerationPolicy::hierarchicalViewName());

    QSharedPointer<ConfigurableVLNVReference> tempReference (new ConfigurableVLNVReference(desConfVLNV));

    QSharedPointer<DesignConfigurationInstantiation> hierarchicalInstantiation
        (new DesignConfigurationInstantiation(desConfVLNV.getName() + "_" + desConfVLNV.getVersion()));
    hierarchicalInstantiation->setDesignConfigurationReference(tempReference);

    newHierarchicalView->setDesignConfigurationInstantiationRef(hierarchicalInstantiation->name());

    component->getDesignConfigurationInstantiations()->append(hierarchicalInstantiation);
    component->getViews()->append(newHierarchicalView);

    QStringList viewNames = component->getHierViews();
    Q_ASSERT(!viewNames.isEmpty());

    // Create the design and design configuration.
    QSharedPointer<Design> design(new Design(designVLNV));
    design->setDesignImplementation(KactusAttribute::HW);
    design->setVersion(VersionHelper::versionFileStr());

    QSharedPointer<DesignConfiguration> designConf(new DesignConfiguration(desConfVLNV));
    designConf->setDesignRef(designVLNV);
    designConf->setDesignConfigImplementation(KactusAttribute::HW);
    designConf->setVersion(VersionHelper::versionFileStr());

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
        openDesign(vlnv, viewNames.first());

        unlockNewlyCreatedDocument(vlnv);
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
    QSharedPointer<Document> libComp = libraryHandler_->getModel(vlnv);
    QSharedPointer<Component> component = libComp.staticCast<Component>();

    // Ask the user the VLNV, target path and view name.
    NewDesignDialog dialog(libraryHandler_, component, KactusAttribute::HW, this);
    dialog.setVLNV(VLNV(VLNV::DESIGN, component->getVlnv().getVendor(), component->getVlnv().getLibrary(), "", ""));

    QSettings settings;
    QStringList suggestions = settings.value("Policies/HWViewNames").toStringList();
    dialog.setViewNameSuggestions(suggestions);
    component->setVersion(VersionHelper::versionFileStr());

    QString baseViewName = "";
    QString viewName = "";

    if (!suggestions.isEmpty())
    {
        baseViewName = suggestions.first();

        viewName = baseViewName;
        unsigned int runningNumber = 1;

        for (QSharedPointer<View> view : *component->getViews())
        {
            if (view->name() == viewName)
            {
                ++runningNumber;
                viewName = baseViewName + QString::number(runningNumber);
            }
        }
    }

    dialog.setViewName(viewName);

    if (dialog.exec() == QDialog::Rejected)
    {
        return;
    }

    // Create the view.
    QSharedPointer<View> view (new View(dialog.getViewName()));

    VLNV designConfigVLNV = dialog.getDesignConfVLNV();
    QSharedPointer<DesignConfigurationInstantiation> hierarchyInstantiation
        (new DesignConfigurationInstantiation(NameGenerationPolicy::designConfigurationInstantiationName(dialog.getQualifierName())));
    hierarchyInstantiation->setDesignConfigurationReference(
        QSharedPointer<ConfigurableVLNVReference>( new ConfigurableVLNVReference( designConfigVLNV ) ) );

    view->setDesignConfigurationInstantiationRef(hierarchyInstantiation->name());

    component->getViews()->append(view);
    component->getDesignConfigurationInstantiations()->append(hierarchyInstantiation);

    // Create the design and design configuration objects.
    QSharedPointer<DesignConfiguration> designConf(new DesignConfiguration(dialog.getDesignConfVLNV()));
    designConf->setDesignRef(dialog.getDesignVLNV());
    designConf->setDesignConfigImplementation(KactusAttribute::HW);
    designConf->setVersion(VersionHelper::versionFileStr());

    QSharedPointer<Design> newDesign = QSharedPointer<Design>(new Design(dialog.getDesignVLNV()));
    newDesign->setDesignImplementation(KactusAttribute::HW);
    newDesign->setVersion(VersionHelper::versionFileStr());

    newDesign->addColumn(QSharedPointer<ColumnDesc>(new ColumnDesc(
        "IO", ColumnTypes::IO, 0, GraphicsColumnConstants::IO_COLUMN_WIDTH)));
    newDesign->addColumn(QSharedPointer<ColumnDesc>(new ColumnDesc(
        "Buses", ColumnTypes::BUSES, 0, GraphicsColumnConstants::COMPONENT_COLUMN_WIDTH)));
    newDesign->addColumn(QSharedPointer<ColumnDesc>(new ColumnDesc(
        "Components", ColumnTypes::COMPONENTS, 0, GraphicsColumnConstants::COMPONENT_COLUMN_WIDTH)));
    newDesign->addColumn(QSharedPointer<ColumnDesc>(new ColumnDesc(
        "IO", ColumnTypes::IO, 0, GraphicsColumnConstants::IO_COLUMN_WIDTH)));

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
        openDesign(vlnv, view->name());

        unlockNewlyCreatedDocument(vlnv);

    }
    else
    {
        emit errorMessage("Error saving files to disk.");
    }
}

//-----------------------------------------------------------------------------
// Function: MainWindow::unlockNewlyCreatedDocument()
//-----------------------------------------------------------------------------
void MainWindow::unlockNewlyCreatedDocument(VLNV const& vlnv)
{
    TabDocument* document = dynamic_cast<TabDocument*>(designTabs_->currentWidget());
    if (document && document->getIdentifyingVLNV() == vlnv)
    {
        document->setProtection(false);
    }
}

//-----------------------------------------------------------------------------
// Function: mainwindow::createCatalog()
//-----------------------------------------------------------------------------
void MainWindow::createCatalog(VLNV const& catalogVLNV, QString const& directory)
{
    Q_ASSERT(catalogVLNV.isValid());

    // Create the catalog.
    QSharedPointer<Catalog> catalog = QSharedPointer<Catalog>(new Catalog());
    catalog->setVlnv(catalogVLNV);
    catalog->setVersion(VersionHelper::versionFileStr());

    // create the file for the abstraction definition
    if (!libraryHandler_->writeModelToFile(directory, catalog))
    {
        emit errorMessage("Error saving files to disk.");
        return;
    }

    // Open the catalog editor.
    openCatalog(catalogVLNV);

    unlockNewlyCreatedDocument(catalogVLNV);
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
    component->setImplementation(KactusAttribute::SW);
    component->setVersion(VersionHelper::versionFileStr());

    // Create the view.
    QSharedPointer<View> view (new View("software"));

    QSharedPointer<ConfigurableVLNVReference> tempReference (new ConfigurableVLNVReference(desConfVLNV));

    QSharedPointer<DesignConfigurationInstantiation> hierarchicalInstantiation
        (new DesignConfigurationInstantiation(desConfVLNV.getName() + "_" + desConfVLNV.getVersion()));
    hierarchicalInstantiation->setDesignConfigurationReference(tempReference);

    view->setDesignConfigurationInstantiationRef(hierarchicalInstantiation->name());

    component->getDesignConfigurationInstantiations()->append(hierarchicalInstantiation);
    component->getViews()->append(view);

    // Create the design and design configuration.
    QSharedPointer<Design> design(new Design(designVLNV));
    design->setDesignImplementation(KactusAttribute::SW);
    design->setVersion(VersionHelper::versionFileStr());

    QSharedPointer<DesignConfiguration> designConf(new DesignConfiguration(desConfVLNV));
    designConf->setDesignRef(designVLNV);
    designConf->setDesignConfigImplementation(KactusAttribute::SW);
    designConf->setVersion(VersionHelper::versionFileStr());

    design->addColumn(QSharedPointer<ColumnDesc>(new ColumnDesc("Low-level", ColumnTypes::COMPONENTS, 0,
        GraphicsColumnConstants::COMPONENT_COLUMN_WIDTH)));
    design->addColumn(QSharedPointer<ColumnDesc>(new ColumnDesc("Middle-level", ColumnTypes::COMPONENTS, 0,
        GraphicsColumnConstants::COMPONENT_COLUMN_WIDTH)));
    design->addColumn(QSharedPointer<ColumnDesc>(new ColumnDesc("High-level", ColumnTypes::COMPONENTS, 0,
        GraphicsColumnConstants::COMPONENT_COLUMN_WIDTH)));
    design->addColumn(QSharedPointer<ColumnDesc>(new ColumnDesc("Out", ColumnTypes::IO, 0,
        GraphicsColumnConstants::IO_COLUMN_WIDTH)));

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
        openSWDesign(vlnv, view->name());
        unlockNewlyCreatedDocument(vlnv);
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
    QSharedPointer<Document> libComp = libraryHandler_->getModel(vlnv);
    QSharedPointer<Component> component = libComp.staticCast<Component>();

    // Ask the user the VLNV, target path and view name.
    NewDesignDialog dialog(libraryHandler_, component, KactusAttribute::SW, this);
    dialog.setVLNV(VLNV(VLNV::DESIGN, component->getVlnv().getVendor(), component->getVlnv().getLibrary(), "", ""));

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

        for (QSharedPointer<View> swView : *component->getViews())
        {
            if (swView->name() == viewName)
            {
                ++runningNumber;
                viewName = baseViewName + QString::number(runningNumber);
            }
        }
    }

    dialog.setViewName(viewName);

    if (dialog.exec() == QDialog::Rejected)
    {
        return;
    }

    // Create the view.
    QSharedPointer<View> view (new View(dialog.getViewName()));

    QSharedPointer<ConfigurableVLNVReference> tempReference (new ConfigurableVLNVReference(dialog.getDesignConfVLNV()));

    QSharedPointer<DesignConfigurationInstantiation> hierarchicalInstantiation
        (new DesignConfigurationInstantiation(dialog.getDesignConfVLNV().getName() + "_" + dialog.getDesignConfVLNV().getVersion()));
    hierarchicalInstantiation->setDesignConfigurationReference(tempReference);

    view->setDesignConfigurationInstantiationRef(hierarchicalInstantiation->name());

    component->getDesignConfigurationInstantiations()->append(hierarchicalInstantiation);
    component->getViews()->append(view);

    component->setVersion(VersionHelper::versionFileStr());

    // Create the design and design configuration objects.
    QSharedPointer<DesignConfiguration> designConf(new DesignConfiguration(dialog.getDesignConfVLNV()));
    designConf->setDesignRef(dialog.getDesignVLNV());
    designConf->setDesignConfigImplementation(KactusAttribute::SW);
    designConf->setVersion(VersionHelper::versionFileStr());

    QSharedPointer<Design> newDesign(new Design(dialog.getDesignVLNV()));
    newDesign->setDesignImplementation(KactusAttribute::SW);
    newDesign->setVersion(VersionHelper::versionFileStr());

    if (component->getImplementation() == KactusAttribute::SW)
    {
        newDesign->addColumn(QSharedPointer<ColumnDesc>(new ColumnDesc(
            "Low-level", ColumnTypes::COMPONENTS, 0, GraphicsColumnConstants::COMPONENT_COLUMN_WIDTH)));
        newDesign->addColumn(QSharedPointer<ColumnDesc>(new ColumnDesc(
            "Middle-level", ColumnTypes::COMPONENTS, 0,GraphicsColumnConstants::COMPONENT_COLUMN_WIDTH)));
        newDesign->addColumn(QSharedPointer<ColumnDesc>(new ColumnDesc(
            "High-level", ColumnTypes::COMPONENTS, 0, GraphicsColumnConstants::COMPONENT_COLUMN_WIDTH)));
        newDesign->addColumn(QSharedPointer<ColumnDesc>(new ColumnDesc(
            "Out", ColumnTypes::IO, 0, GraphicsColumnConstants::IO_COLUMN_WIDTH)));
    }
    else
    {
        newDesign->addColumn(QSharedPointer<ColumnDesc>(new ColumnDesc(
            "Pre-mapped SW", ColumnTypes::COMPONENTS, 0, GraphicsColumnConstants::COMPONENT_COLUMN_WIDTH)));
        newDesign->addColumn(QSharedPointer<ColumnDesc>(new ColumnDesc(
            "Pre-mapped SW", ColumnTypes::COMPONENTS, 0, GraphicsColumnConstants::COMPONENT_COLUMN_WIDTH)));
    }

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
        openSWDesign(vlnv, view->name());

        unlockNewlyCreatedDocument(vlnv);
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
    SettingsDialog dialog(this);

    if (dialog.exec() == QDialog::Accepted)
    {
        designTabs_->applySettings();
        updateGeneratorPluginActions();
        dockHandler_->applySettings();
    }
}

//-----------------------------------------------------------------------------
// Function: mainwindow::createSystem()
//-----------------------------------------------------------------------------
void MainWindow::createSystem(VLNV const& compVLNV, QString const& viewName, VLNV const& sysVLNV,
                              QString const& directory)
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
        parentComp->setHierarchy(KactusAttribute::PRODUCT);
        parentComp->setFirmness(KactusAttribute::FIXED);
        parentComp->setImplementation(KactusAttribute::SYSTEM);
        parentComp->setVersion(VersionHelper::versionFileStr());
    }

    // Create the system view to the system design.
    QString sysViewName = "system";
    unsigned int runningNumber = 1;

    while (parentComp->findSystemView(sysViewName) != 0)
    {
        ++runningNumber;
        sysViewName = "system" + QString::number(runningNumber);
    }

    QSharedPointer<SystemView> systemView (new SystemView(sysViewName));
    systemView->setHierarchyRef(desConfVLNV);
    systemView->setHWViewRef(viewName);

    QList<QSharedPointer<SystemView> > systemViews = parentComp->getSystemViews();
    systemViews.append(systemView);
    parentComp->setSystemViews(systemViews);

    // Flat-out the hierarchy to form the system design.
    QSharedPointer<Design> sysDesign(new Design(designVLNV));
    sysDesign->setDesignImplementation(KactusAttribute::SYSTEM);
    sysDesign->setVersion(VersionHelper::versionFileStr());

    sysDesign->addColumn(QSharedPointer<ColumnDesc>(new ColumnDesc("SW Components", ColumnTypes::COMPONENTS, 0,
        GraphicsColumnConstants::SYSTEM_COLUMN_WIDTH)));
    sysDesign->addColumn(QSharedPointer<ColumnDesc>(new ColumnDesc("SW Components", ColumnTypes::COMPONENTS, 0,
        GraphicsColumnConstants::SYSTEM_COLUMN_WIDTH)));

    generateSystemDesignV2(libraryHandler_, parentComp->getHierRef(viewName), *sysDesign);

    // Create the design configuration.
    QSharedPointer<DesignConfiguration> designConf(new DesignConfiguration(desConfVLNV));
    designConf->setDesignRef(designVLNV);
    designConf->setDesignConfigImplementation(KactusAttribute::SYSTEM);
    designConf->setVersion(VersionHelper::versionFileStr());

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
        openSystemDesign(parentComp->getVlnv(), sysViewName);

        unlockNewlyCreatedDocument(parentComp->getVlnv());
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
    QSharedPointer<Document> libComp = libraryHandler_->getModel(vlnv);
    QSharedPointer<Component> component = libComp.staticCast<Component>();
    component->setVersion(VersionHelper::versionFileStr());

    // Ask the user the VLNV, target path and view name.
    NewDesignDialog dialog(libraryHandler_, component, KactusAttribute::SYSTEM, this);
    dialog.setVLNV(VLNV(VLNV::DESIGN, component->getVlnv().getVendor(), component->getVlnv().getLibrary(), "", ""));

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
    QSharedPointer<SystemView> view (new SystemView(dialog.getViewName()));
    view->setHierarchyRef(dialog.getDesignConfVLNV());
    view->setHWViewRef(component->getHierViews().first());

    QList<QSharedPointer<SystemView> > systemViews = component->getSystemViews();
    systemViews.append(view);
    component->setSystemViews(systemViews);

    // Create the design and design configuration objects to the same folder as the component.
    QSharedPointer<DesignConfiguration> designConf(new DesignConfiguration(dialog.getDesignConfVLNV()));
    designConf->setDesignRef(dialog.getDesignVLNV());
    designConf->setDesignConfigImplementation(KactusAttribute::SYSTEM);
    designConf->setVersion(VersionHelper::versionFileStr());

    QSharedPointer<Design> newDesign = QSharedPointer<Design>(new Design(dialog.getDesignVLNV()));
    newDesign->setDesignImplementation(KactusAttribute::SYSTEM);
    newDesign->setVersion(VersionHelper::versionFileStr());

    newDesign->addColumn(QSharedPointer<ColumnDesc>(new ColumnDesc("SW Components", ColumnTypes::COMPONENTS, 0,
        GraphicsColumnConstants::SYSTEM_COLUMN_WIDTH)));
    newDesign->addColumn(QSharedPointer<ColumnDesc>(new ColumnDesc("SW Components", ColumnTypes::COMPONENTS, 0,
        GraphicsColumnConstants::SYSTEM_COLUMN_WIDTH)));

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
        QString viewName = view->name();
        openSystemDesign(vlnv, viewName);

        unlockNewlyCreatedDocument(vlnv);
    }
    else
    {
        emit errorMessage("Error saving files to disk.");
    }
}

//-----------------------------------------------------------------------------
// Function: mainwindow::createBus()
//-----------------------------------------------------------------------------
void MainWindow::createBus(VLNV const& vlnv, QString const& directory)
{
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

    if (libraryHandler_->contains(absVLNV))
    {
        VLNV newAbsDefVLNV;

        if (!NewObjectDialog::saveAsDialog(this, libraryHandler_, absVLNV, newAbsDefVLNV, absDirectory,
            "Set VLNV for abstraction definition"))
        {
            // if user canceled
            return;
        }
        // save the created abstraction definition vlnv
        absVLNV = newAbsDefVLNV;
    }

    // Create a bus definition.
    QSharedPointer<BusDefinition> busDef = QSharedPointer<BusDefinition>(new BusDefinition());
    busDef->setVlnv(busVLNV);
    busDef->setVersion(VersionHelper::versionFileStr());

    // Create the file for the bus definition.
    bool success = true;

    if (!libraryHandler_->writeModelToFile(directory, busDef))
    {
        success = false;
    }

    // create an abstraction definition
    QSharedPointer<AbstractionDefinition> absDef = QSharedPointer<AbstractionDefinition>(new AbstractionDefinition());
    absDef->setVlnv(absVLNV);
    absDef->setVersion(VersionHelper::versionFileStr());

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
        openBus(busVLNV, absVLNV, false);

        unlockNewlyCreatedDocument(absVLNV);
    }
    else
    {
        emit errorMessage("Error saving files to disk.");
    }
}

//-----------------------------------------------------------------------------
// Function: mainwindow::createAbsDef()
//-----------------------------------------------------------------------------
void MainWindow::createAbsDef( const VLNV& busDefVLNV, const QString& directory, bool disableBusDef )
{
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

    if (libraryHandler_->contains(absVLNV))
    {
        VLNV newAbsDefVLNV;

        if (!NewObjectDialog::saveAsDialog(this, libraryHandler_, absVLNV, newAbsDefVLNV, absDirectory,
            "Set VLNV for abstraction definition"))
        {
            // if user canceled
            return;
        }
        // save the created abstraction definition vlnv
        absVLNV = newAbsDefVLNV;
    }

    // create an abstraction definition
    QSharedPointer<AbstractionDefinition> absDef = QSharedPointer<AbstractionDefinition>(new AbstractionDefinition());
    absDef->setVlnv(absVLNV);
    absDef->setVersion(VersionHelper::versionFileStr());

    // set reference from abstraction definition to bus definition
    absDef->setBusType(busDefVLNV);

    // create the file for the abstraction definition
    if (!libraryHandler_->writeModelToFile(absDirectory, absDef))
    {
        emit errorMessage("Error saving files to disk.");
        return;
    }

    // Open the bus editor.
    openBus(busDefVLNV, absVLNV, disableBusDef);

    unlockNewlyCreatedDocument(absVLNV);
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
    comDef->setVersion(VersionHelper::versionFileStr());

    if (!libraryHandler_->writeModelToFile(directory, comDef))
    {
        emit errorMessage("Error saving files to disk.");
        return;
    }

    // Open the COM definition.
    openComDefinition(vlnv);
    unlockNewlyCreatedDocument(vlnv);
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
    apiDef->setVersion(VersionHelper::versionFileStr());

    if (!libraryHandler_->writeModelToFile(directory, apiDef))
    {
        emit errorMessage("Error saving files to disk.");
        return;
    }

    // Open the API definition.
    openApiDefinition(apiDefVLNV);
    unlockNewlyCreatedDocument(vlnv);
}

//-----------------------------------------------------------------------------
// Function: mainwindow::openBus()
//-----------------------------------------------------------------------------
void MainWindow::openBus(const VLNV& busDefVLNV, const VLNV& absDefVLNV, bool disableBusDef)
{
    if (isOpen(absDefVLNV) || isOpen(busDefVLNV) || !busDefVLNV.isValid())
    {
        return;
    }

    // Check if the bus editor is already open and activate it.
    for (int i = 0; i < designTabs_->count(); i++)
    {
        TabDocument* editor = dynamic_cast<TabDocument*>(designTabs_->widget(i));

        if (editor &&
            ((absDefVLNV.isValid() && editor->getDocumentVLNV() == absDefVLNV) ||
            editor->getDocumentVLNV() == busDefVLNV))
        {
            designTabs_->setCurrentIndex(i);
            return;
        }
    }

    // Editor for given vlnv was not yet open so create one for it
    QSharedPointer<BusDefinition> busDef;
    QSharedPointer<AbstractionDefinition> absDef;

    if (libraryHandler_->contains(busDefVLNV) &&
        libraryHandler_->getDocumentType(busDefVLNV) == VLNV::BUSDEFINITION)
    {
        busDef = libraryHandler_->getModel(busDefVLNV).dynamicCast<BusDefinition>();
    }
    else
    {
        emit errorMessage(tr("Bus definition %1 was not found in the library").arg(busDefVLNV.toString()));
        return;
    }

    if (absDefVLNV.isValid())
    {
        if (libraryHandler_->contains(absDefVLNV) &&
            libraryHandler_->getDocumentType(absDefVLNV) == VLNV::ABSTRACTIONDEFINITION)
        {
            absDef = libraryHandler_->getModel(absDefVLNV).staticCast<AbstractionDefinition>();
        }
        else
        {
            emit errorMessage(tr("Abstraction definition %1 was not found in the library").arg(
                absDefVLNV.toString()));
            return;
        }
    }

    BusDefinitionEditor* editor = new BusDefinitionEditor(this, libraryHandler_, busDef, absDef, absDef && disableBusDef);

    designTabs_->addAndOpenDocument(editor);
}

//-----------------------------------------------------------------------------
// Function: MainWindow::openCatalog()
//-----------------------------------------------------------------------------
void MainWindow::openCatalog(const VLNV& vlnv)
{
    if (isOpen(vlnv))
    {
        return;
    }
 
    if (!libraryHandler_->contains(vlnv))
    {
        emit errorMessage(tr("VLNV %1 was not found in the library").arg(vlnv.toString()));
        return;            
    }

    QSharedPointer<Catalog> catalog = libraryHandler_->getModel(vlnv).dynamicCast<Catalog>();
    if (catalog == 0)
    {
        emit errorMessage(tr("Document could not be opened for Catalog"));
        return;
    }

    CatalogEditor* editor = new CatalogEditor(libraryHandler_, catalog, this);

    connect(editor, SIGNAL(openCatalog(const VLNV&)),
        this, SLOT(openCatalog(const VLNV&)), Qt::UniqueConnection);

    connect(editor, SIGNAL(openBus(const VLNV&, VLNV const&)),
        this, SLOT(openBus(const VLNV&, VLNV const&)), Qt::UniqueConnection);

    connect(editor, SIGNAL(openComponent(const VLNV&)),
        this, SLOT(openComponent(const VLNV&)), Qt::UniqueConnection);

    designTabs_->addAndOpenDocument(editor);
}

//-----------------------------------------------------------------------------
// Function: openDesign()
//-----------------------------------------------------------------------------
void MainWindow::openDesign(VLNV const& vlnv, QString const& viewName)
{
    // The vlnv must always be for a component.
    Q_ASSERT(libraryHandler_->getDocumentType(vlnv) == VLNV::COMPONENT);

    QSharedPointer<Component> comp = libraryHandler_->getModel(vlnv).staticCast<Component>();

    // Check if the design is already open.
    VLNV refVLNV = comp->getHierRef(viewName);
    VLNV designVLNV = libraryHandler_->getDesignVLNV(refVLNV);
    if (isOpen(designVLNV) || hasInvalidReferences(comp->getHierRefs(), vlnv))
    {
        return;
    }

    QSharedPointer<Design> newDesign = libraryHandler_->getDesign(designVLNV);
    dockHandler_->setupDesignParameterFinder(newDesign);

    DesignWidgetFactoryImplementation factory(libraryHandler_,
        dockHandler_->getDesignAndInstanceParameterFinder(), dockHandler_->getDesignParameterFinder());
    DesignWidget* designWidget = factory.makeHWDesignWidget(this);

    // open the design in the designWidget
    designWidget->setDesign(vlnv, viewName);

    // if the design could not be opened
    if (designWidget->getOpenDocument().isEmpty())
    {
        delete designWidget;
        return;
    }

    connect(designWidget, SIGNAL(zoomChanged()), this, SLOT(updateZoomTools()), Qt::UniqueConnection);
    connect(designWidget, SIGNAL(modeChanged(DrawMode)),
        this, SLOT(onDrawModeChanged(DrawMode)), Qt::UniqueConnection);

    connect(designWidget, SIGNAL(destroyed(QObject*)), this, SLOT(onClearItemSelection()), Qt::UniqueConnection);

    connect(designWidget, SIGNAL(openDesign(const VLNV&, const QString&)),
        this, SLOT(openDesign(const VLNV&, const QString&)));
    connect(designWidget, SIGNAL(openComponent(const VLNV&)),
        this, SLOT(openComponent(const VLNV&)), Qt::UniqueConnection);
    connect(designWidget, SIGNAL(openBus(VLNV const&, VLNV const&, bool)),
        this, SLOT(openBus(VLNV const&, VLNV const&, bool)), Qt::UniqueConnection);

    connect(designWidget, SIGNAL(componentSelected(ComponentItem*)),
        this, SLOT(onComponentSelected(ComponentItem*)), Qt::UniqueConnection);
    connect(designWidget, SIGNAL(interfaceSelected(ConnectionEndpoint*)),
        this, SLOT(onInterfaceSelected(ConnectionEndpoint*)), Qt::UniqueConnection);
    connect(designWidget, SIGNAL(connectionSelected(GraphicsConnection*)),
        this, SLOT(onConnectionSelected(GraphicsConnection*)), Qt::UniqueConnection);

    connect(designWidget, SIGNAL(clearItemSelection()),
        this, SLOT(onClearItemSelection()), Qt::UniqueConnection);
    connect(designWidget, SIGNAL(refreshed()), this, SLOT(onDesignDocumentRefreshed()), Qt::UniqueConnection);

    designTabs_->addAndOpenDocument(designWidget);
}

//-----------------------------------------------------------------------------
// Function: MainWindow::openMemoryDesign()
//-----------------------------------------------------------------------------
void MainWindow::openMemoryDesign()
{
    TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());

    if (doc)
    {
        DesignWidget* currentDesignWidget = static_cast<DesignWidget*>(doc);
        if (currentDesignWidget)
        {
            QString activeView = currentDesignWidget->getOpenViewName();
            libraryHandler_->onOpenMemoryDesign(doc->getDocumentVLNV(), activeView);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MainWindow::openMemoryDesign()
//-----------------------------------------------------------------------------
void MainWindow::openMemoryDesign(VLNV const& vlnv, QString const& viewName)
{
    // the vlnv must always be for a component
    Q_ASSERT(libraryHandler_->getDocumentType(vlnv) == VLNV::COMPONENT);

    QSharedPointer<Component> comp = libraryHandler_->getModel(vlnv).staticCast<Component>();

    // check if the design is already open
    VLNV refVLNV = comp->getHierRef(viewName);
    VLNV designVLNV = libraryHandler_->getDesignVLNV(refVLNV);

    QString memoryDesignName = designVLNV.getName();

    int extensionBegin = memoryDesignName.size() - 7;
    memoryDesignName.replace(extensionBegin, 7, ".memoryDesign");
    designVLNV.setName(memoryDesignName);

    if (isOpen(designVLNV))
    {
        return;
    }

    // make sure that all component's hierarchy refs are valid
    // if there was at least one invalid reference then do not open the design
    if (hasInvalidReferences(comp->getHierRefs(), vlnv))
    {
        return;
    }

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    MemoryDesignDocument* memoryDesignWidget = new MemoryDesignDocument(libraryHandler_, this);

    connect(memoryDesignWidget, SIGNAL(errorMessage(const QString&)),
        dockHandler_, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);

    if (!memoryDesignWidget->setDesign(vlnv, viewName))
    {
        QApplication::restoreOverrideCursor();
        delete memoryDesignWidget;
        return;
    }

    connect(memoryDesignWidget, SIGNAL(zoomChanged()), this, SLOT(updateZoomTools()), Qt::UniqueConnection);

    connect(memoryDesignWidget, SIGNAL(destroyed(QObject*)), this, SLOT(onClearItemSelection()), Qt::UniqueConnection);

//     connect(memoryDesignWidget, SIGNAL(clearItemSelection()),
//         libraryHandler_, SLOT(onClearSelection()), Qt::UniqueConnection);

    connect(memoryDesignWidget, SIGNAL(clearItemSelection()), this, SLOT(onClearItemSelection()), Qt::UniqueConnection);

    connect(memoryDesignWidget, SIGNAL(openComponentDocument(VLNV const&, QVector<QString>)),
        this, SLOT(onOpenComponentItem(const VLNV&, QVector<QString>)), Qt::UniqueConnection);

    designTabs_->addAndOpenDocument(memoryDesignWidget);

    QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------
// Function: mainwindow::onOpenComponentItem()
//-----------------------------------------------------------------------------
void MainWindow::onOpenComponentItem(const VLNV& componentVLNV, QVector<QString> identifierChain)
{
    openComponent(componentVLNV);

    if (!identifierChain.isEmpty())
    {
        TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());
        if (doc)
        {
            ComponentEditor* componentEditor = dynamic_cast<ComponentEditor*>(doc);
            if (componentEditor)
            {
                componentEditor->openItemEditor(identifierChain);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: openSWDesign()
//-----------------------------------------------------------------------------
void MainWindow::openSWDesign(const VLNV& vlnv, QString const& viewName)
{
    // the vlnv must always be for a component
    Q_ASSERT(libraryHandler_->getDocumentType(vlnv) == VLNV::COMPONENT);

    QSharedPointer<Component> comp = libraryHandler_->getModel(vlnv).staticCast<Component>();

    // check if the design is already open
    VLNV refVLNV = comp->getHierRef(viewName);
    VLNV designVLNV = libraryHandler_->getDesignVLNV(refVLNV);
    if (isOpen(designVLNV))
    {
        return;
    }

    SystemDesignWidget* designWidget = new SystemDesignWidget(true, libraryHandler_, this);

    connect(designWidget, SIGNAL(errorMessage(const QString&)),
        dockHandler_, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
    connect(designWidget, SIGNAL(noticeMessage(const QString&)),
        dockHandler_, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

    if (!designWidget->setDesign(vlnv, viewName))
    {
        delete designWidget;
        return;
    }

    disconnect(designWidget, SIGNAL(errorMessage(const QString&)),
        dockHandler_, SIGNAL(errorMessage(const QString&)));
    disconnect(designWidget, SIGNAL(noticeMessage(const QString&)),
        dockHandler_, SIGNAL(noticeMessage(const QString&)));

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
    connect(designWidget, SIGNAL(destroyed(QObject*)), this, SLOT(onClearItemSelection()), Qt::UniqueConnection);
    connect(designWidget, SIGNAL(clearItemSelection()),
        libraryHandler_, SLOT(onClearSelection()), Qt::UniqueConnection);
    connect(designWidget, SIGNAL(clearItemSelection()), this, SLOT(onClearItemSelection()), Qt::UniqueConnection);

    connect(designWidget, SIGNAL(refreshed()), this, SLOT(onDesignDocumentRefreshed()), Qt::UniqueConnection);
    connect(designWidget, SIGNAL(zoomChanged()), this, SLOT(updateZoomTools()), Qt::UniqueConnection);
    connect(designWidget, SIGNAL(modeChanged(DrawMode)),
        this, SLOT(onDrawModeChanged(DrawMode)), Qt::UniqueConnection);
    connect(designWidget, SIGNAL(modifiedChanged(bool)), actSave_, SLOT(setEnabled(bool)), Qt::UniqueConnection);

    designTabs_->addAndOpenDocument(designWidget);
}

//-----------------------------------------------------------------------------
// Function: openSystemDesign()
//-----------------------------------------------------------------------------
void MainWindow::openSystemDesign(VLNV const& vlnv, QString const& viewName)
{
    // the vlnv must always be for a component
    Q_ASSERT(libraryHandler_->getDocumentType(vlnv) == VLNV::COMPONENT);

    QSharedPointer<Component> comp = libraryHandler_->getModel(vlnv).staticCast<Component>();

    // check if the design is already open
    VLNV refVLNV = comp->getHierSystemRef(viewName);
    VLNV designVLNV = libraryHandler_->getDesignVLNV(refVLNV);
    if (isOpen(designVLNV))
    {
        return;
    }

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    SystemDesignWidget* designWidget = new SystemDesignWidget(false, libraryHandler_, this);

    connect(designWidget, SIGNAL(errorMessage(const QString&)),
        dockHandler_, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
    connect(designWidget, SIGNAL(noticeMessage(const QString&)),
        dockHandler_, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

    if (!designWidget->setDesign(vlnv, viewName))
    {
        delete designWidget;
        QApplication::restoreOverrideCursor();
        return;
    }

    disconnect(designWidget, SIGNAL(errorMessage(const QString&)),
        dockHandler_, SIGNAL(errorMessage(const QString&)));
    disconnect(designWidget, SIGNAL(noticeMessage(const QString&)),
        dockHandler_, SIGNAL(noticeMessage(const QString&)));

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

    connect(designWidget, SIGNAL(refreshed()), this, SLOT(onDesignDocumentRefreshed()), Qt::UniqueConnection);
    connect(designWidget, SIGNAL(zoomChanged()), this, SLOT(updateZoomTools()), Qt::UniqueConnection);
    connect(designWidget, SIGNAL(modeChanged(DrawMode)),
        this, SLOT(onDrawModeChanged(DrawMode)), Qt::UniqueConnection);
    connect(designWidget, SIGNAL(modifiedChanged(bool)),
        actSave_, SLOT(setEnabled(bool)), Qt::UniqueConnection);

    designTabs_->addAndOpenDocument(designWidget);

    QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------
// Function: mainwindow::openComponent()
//-----------------------------------------------------------------------------
void MainWindow::openComponent(VLNV const& vlnv)
{
    if (isOpen(vlnv))
    {
        return;
    }

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    QSharedPointer<Component> component;

    if (libraryHandler_->contains(vlnv))
    {
        component = libraryHandler_->getModel(vlnv).dynamicCast<Component>();
    }
    else
    {
        emit errorMessage(tr("VLNV %1 was not found in the library").arg(vlnv.toString()));
        QApplication::restoreOverrideCursor();
        return;
    }

    if (!component)
    {
        emit errorMessage(tr("Document could not be opened for Component"));
        QApplication::restoreOverrideCursor();
        return;
    }

    ComponentEditor* editor = new ComponentEditor(libraryHandler_, component, this);

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

    designTabs_->addAndOpenDocument(editor);

    QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------
// Function: MainWindow::openComDefinition()
//-----------------------------------------------------------------------------
void MainWindow::openComDefinition(VLNV const& vlnv)
{
    if (isOpen(vlnv))
    {
        return;
    }

    // Editor was not yet open so create it.
    QSharedPointer<ComDefinition> comDef;

    if (libraryHandler_->contains(vlnv))
    {
        comDef = libraryHandler_->getModel(vlnv).dynamicCast<ComDefinition>();
    }
    else
    {
        emit errorMessage(tr("VLNV %1 was not found in the library").arg(vlnv.toString()));
        return;
    }

    if (comDef == 0)
    {
        emit errorMessage(tr("Document could not be opened for COM Definition"));
        return;
    }

    ComDefinitionEditor* editor = new ComDefinitionEditor(this, libraryHandler_, comDef);

    designTabs_->addAndOpenDocument(editor);
}


//-----------------------------------------------------------------------------
// Function: MainWindow::openApiDefinition()
//-----------------------------------------------------------------------------
void MainWindow::openApiDefinition(VLNV const& vlnv)
{
    if (isOpen(vlnv))
    {
        return;
    }

    // Editor was not yet open so create it.
    QSharedPointer<ApiDefinition> apiDef;

    if (libraryHandler_->contains(vlnv))
    {
        apiDef = libraryHandler_->getModel(vlnv).dynamicCast<ApiDefinition>();
    }
    else
    {
        emit errorMessage(tr("VLNV %1 was not found in the library").arg(vlnv.toString()));
        return;
    }

    if (apiDef == 0)
    {
        emit errorMessage(tr("Document could not be opened for API Definition"));
        return;
    }

    ApiDefinitionEditor* editor = new ApiDefinitionEditor(this, libraryHandler_, apiDef);

    designTabs_->addAndOpenDocument(editor);
}

//-----------------------------------------------------------------------------
// Function: MainWindow::isOpen()
//-----------------------------------------------------------------------------
bool MainWindow::isOpen(VLNV const& vlnv) const
{
    if (!vlnv.isValid())
    {
        return false;
    }

    for (int i = 0; i < designTabs_->count(); i++)
    {
        TabDocument* document = dynamic_cast<TabDocument*>(designTabs_->widget(i));

        // if the document is already open is some tab
        if (document && document->getIdentifyingVLNV() == vlnv)
        {
            designTabs_->setCurrentIndex(i);
            return true;
        }
    }
    // document was not open
    return false;
}

//-----------------------------------------------------------------------------
// Function: mainwindow::updateWindows()
//-----------------------------------------------------------------------------
void MainWindow::updateWindows()
{
    dockHandler_->updateWindows(designTabs_->count(), designTabs_->currentWidget());
}

//-----------------------------------------------------------------------------
// Function: MainWindow::updateVisibilityControlMenu()
//-----------------------------------------------------------------------------
void MainWindow::updateVisibilityControlMenu(TabDocument* document)
{
    visibilityMenu_.clear();

    QActionGroup* group = new QActionGroup(this);
    group->setExclusive(false);

    QMapIterator<QString, bool> iter(document->getVisibilityControls());
    while (iter.hasNext())
    {
        iter.next();

        QAction* action = new QAction(iter.key(), this);
        action->setCheckable(true);
        action->setChecked(iter.value());

        group->addAction(action);
        visibilityMenu_.addAction(action);
    }
}

//-----------------------------------------------------------------------------
// Function: MainWindow::connectVisibilityControls()
//-----------------------------------------------------------------------------
void MainWindow::connectVisibilityControls()
{
    connect(dockHandler_, SIGNAL(adjustVisibilityInWindow(TabDocument::SupportedWindows, bool)),
        this, SLOT(onAdjustVisibilityInWindow(TabDocument::SupportedWindows, bool)), Qt::UniqueConnection);
    dockHandler_->connectVisibilityControls();
}

//-----------------------------------------------------------------------------
// Function: MainWindow::disconnectVisibilityControls()
//-----------------------------------------------------------------------------
void MainWindow::disconnectVisibilityControls()
{
    disconnect(dockHandler_, SIGNAL(adjustVisibilityInWindow(TabDocument::SupportedWindows, bool)),
        this, SLOT(onAdjustVisibilityInWindow(TabDocument::SupportedWindows, bool)));
    dockHandler_->disconnectVisibilityControls();
}

//-----------------------------------------------------------------------------
// Function: openCSource()
//-----------------------------------------------------------------------------
void MainWindow::openCSource(ComponentItem* compItem)
{
    Q_ASSERT(compItem != 0);

    QSharedPointer<FileSet> fileSet = compItem->componentModel()->getFileSet("cSources");

    if (fileSet == 0 || fileSet->getFiles()->count() == 0)
    {
        return;
    }

    QString filename;

    if (fileSet->getFiles()->count() > 1)
    {
        // Show a dialog for selecting what source file to open.
        ListSelectDialog saveDialog(this);
        saveDialog.setWindowTitle(tr("Open Source"));
        saveDialog.setDescription(tr("Select C source file to open:"));

        for (QSharedPointer<File> file : *fileSet->getFiles())
        {
            saveDialog.addItem(new QListWidgetItem(file->name()));
        }

        if (saveDialog.exec() == QDialog::Rejected)
        {
            return;
        }

        filename = saveDialog.getSelectedItem()->text();
    }
    else
    {
        // Otherwise there is only one possibility.
        filename = fileSet->getFiles()->first()->name();
    }

    if (compItem->componentModel()->getVlnv().isValid())
    {
        filename = General::getAbsolutePath(libraryHandler_->getPath(compItem->componentModel()->getVlnv()),
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
    designTabs_->addAndOpenDocument(sourceWidget);

    dynamic_cast<TabDocument*>(designTabs_->currentWidget())->setProtection(false);
}

//-----------------------------------------------------------------------------
// Function: changeProtection()
//-----------------------------------------------------------------------------
void MainWindow::changeProtection(bool locked)
{
    TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());

    // if there is no currently selected tab
    if (!doc)
    {
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
                "document can be locked. Save changes and continue?"), QMessageBox::Yes | QMessageBox::No, this);

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

    //If user wants to unlock the document.
    else
    {
        // The list containing tabs for the same document.
        QVector<TabDocument*> otherDocs;

        // If the same document has been edited in another editor.
        TabDocument* docToSave = 0;

        for (int i = 0; i < designTabs_->count(); i++)
        {
            TabDocument* otherDoc = static_cast<TabDocument*>(designTabs_->widget(i));
            Q_ASSERT(otherDoc);

            // if the other document is for the same component
            if (otherDoc != doc && otherDoc->getDocumentVLNV() == docVLNV)
            {
                otherDocs.append(otherDoc);

                // If there is a document that is not locked and has been modified.
                if (!otherDoc->isProtected() && otherDoc->isModified())
                {
                    // there should only be one doc at a time that can be modified
                    Q_ASSERT(!docToSave);

                    // this was the document that was modified
                    docToSave = otherDoc;
                }
            }
        }

        // if there was document that was unlocked and modified
        if (docToSave)
        {
            // Ask the user if he wants to save and switch locks.
            QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
                tr("The document is being edited in another tab and has unsaved changes. Changes need to be saved "
                   "before this tab can be unlocked. Save changes and switch locks?"),
                QMessageBox::Yes | QMessageBox::No, this);

            // Restore the lock if the user canceled.
            if (msgBox.exec() == QMessageBox::No)
            {
                actProtect_->setChecked(true);
                return;
            }

            // if the document could not be saved
            if (!docToSave->save())
            {
                return;
            }
        }

        bool previouslyUnlocked = doc->isPreviouslyUnlocked();

        // User wanted to unlock this doc so go through other documents to see if they were unlocked.
        for (TabDocument* otherDoc : otherDocs)
        {
            otherDoc->setProtection(true);

            if (otherDoc->isPreviouslyUnlocked())
            {
                previouslyUnlocked = true;
            }
        }

        if (!previouslyUnlocked)
        {
            QString message = tr("Are you sure you want to unlock the document?\n");
            QString detailMsg = "";

            // If edited document was component and has been instantiated in other components, print them.
            if (docVLNV.isValid() && libraryHandler_->getDocumentType(docVLNV) == VLNV::COMPONENT)
            {
                QList<VLNV> list;
                int refCount = libraryHandler_->getOwners(list, docVLNV);
                if (refCount > 0)
                {
                    message += tr("Changes to the document can affect %1 other documents.").arg(refCount);

                    detailMsg += tr("The component has been instantiated in the following %1 component(s):\n").arg(
                        refCount);
                    for (VLNV const& owner : list)
                    {
                        detailMsg += "* " + owner.toString() + "\n";
                    }
                }
                else
                {
                    message += tr("Changes to the document can affect other documents. ");
                }
            }
            else
            {
                message += tr("Changes to the document can affect other documents. ");
            }

            message += "If you choose yes, this will not be asked next time for this document.";

            // Ask verification from the user.
            QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(), message,
                QMessageBox::Yes | QMessageBox::No, this);

            msgBox.setDetailedText(detailMsg);

            // if user did not want to unlock the document
            if (msgBox.exec() == QMessageBox::No)
            {
                actProtect_->setChecked(true);
                return;
            }
        }

        // mark the other documents also unlocked to keep the unlock history in sync.
        for (TabDocument* otherDoc : otherDocs)
        {
            otherDoc->setPreviouslyUnlocked();
        }

        // Unlock the document.
        doc->setProtection(locked);
    }

    dockHandler_->changeProtection(doc, locked);

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
        actProtect_->setText(tr("Locked (Ctrl+L)"));
    }
    else
    {
        actProtect_->setIcon(QIcon(":icons/common/graphics/protection-unlocked.png"));
        actProtect_->setText(tr("Unlocked (Ctrl+L)"));
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
// Function: mainwindow::createSWComponent()
//-----------------------------------------------------------------------------
void MainWindow::createSWComponent(VLNV const& vlnv, QString const& directory)
{
    Q_ASSERT(vlnv.isValid());

    // Create a component.
    QSharedPointer<Component> component = QSharedPointer<Component>(new Component());
    component->setVlnv(vlnv);
    component->setVersion(VersionHelper::versionFileStr());

    // Set Kactus attributes.
    component->setImplementation(KactusAttribute::SW);

    // Create the file.
    if (!libraryHandler_->writeModelToFile(directory, component))
    {
        emit errorMessage("Error saving files to disk.");
        return;
    }

    runComponentWizard(component, directory);

    openComponent(vlnv);

    unlockNewlyCreatedDocument(vlnv);
}

//-----------------------------------------------------------------------------
// Function: showAbout()
//-----------------------------------------------------------------------------
void MainWindow::showAbout()
{
    SplashScreen* splash = new SplashScreen(VersionHelper::createVersionString(), this);
    splash->setAttribute(Qt::WA_DeleteOnClose);
    splash->setWindowFlags(splash->windowFlags() & ~(Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint));
    splash->move(this->mapToGlobal(this->rect().center() - splash->rect().center()));
    splash->setWindowModality(Qt::ApplicationModal);
    splash->show();
    splash->showMessage("");
}

//-----------------------------------------------------------------------------
// Function: mainwindow::selectVisibleDocks()
//-----------------------------------------------------------------------------
void MainWindow::selectVisibleDocks()
{
    windowsMenu_.exec(QCursor::pos());
}

//-----------------------------------------------------------------------------
// Function: mainwindow::hideEvent()
//-----------------------------------------------------------------------------
void MainWindow::hideEvent( QHideEvent* event )
{
    disconnectVisibilityControls();
    QMainWindow::hideEvent(event);
}

//-----------------------------------------------------------------------------
// Function: mainwindow::showEvent()
//-----------------------------------------------------------------------------
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
void MainWindow::onWorkspaceChanged(QString const& workspace)
{
    actWorkspaces_->setText(workspace);

    designTabs_->applySettings();
}

//-----------------------------------------------------------------------------
// Function: openVisibilityControlMenu()
//-----------------------------------------------------------------------------
void MainWindow::openVisibilityControlMenu()
{
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
// Function: MainWindow::isDesignOpen()
//-----------------------------------------------------------------------------
bool MainWindow::isDesignOpen(VLNV const& vlnv, KactusAttribute::Implementation implementation) const
{
    if (vlnv.isValid())
    {
        for (int i = 0; i < designTabs_->count(); i++)
        {
            DesignWidget* designWidget = dynamic_cast<DesignWidget*>(designTabs_->widget(i));

            if (designWidget != 0 && designWidget->getOpenDocument() == vlnv &&
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
// Function: MainWindow::hasInvalidReferences()
//-----------------------------------------------------------------------------
bool MainWindow::hasInvalidReferences(QList<VLNV> hierRefs, VLNV const& referencingVlnv)
{
    bool invalidReferences = false;
    for (VLNV const& ref : hierRefs)
    {
        // if the hierarchy referenced object is not found in library
        if (!libraryHandler_->contains(ref))
        {
            emit errorMessage(tr("Component %1 has hierarchical reference to object %2,"
                " which is not found in library. Component is not valid and can not "
                "be opened in design view. Edit component with component editor to "
                "remove invalid references.").arg(referencingVlnv.toString(":")).arg(ref.toString(":")));
            invalidReferences = true;
        }

        // if the reference is to a wrong object type
        else if (libraryHandler_->getDocumentType(ref) != VLNV::DESIGN &&
            libraryHandler_->getDocumentType(ref) != VLNV::DESIGNCONFIGURATION)
        {
            emit errorMessage(tr("Component %1 has hierarchical reference to object %2,"
                " which is not design or design configuration. Component is not valid and"
                " can not be opened in design view. Edit component with component editor to"
                " remove invalid references.").arg(referencingVlnv.toString(":")).arg(ref.toString(":")));
            invalidReferences = true;
        }

        // if the reference is for a design configuration then check that also
        // the design is found
        else if (libraryHandler_->getDocumentType(ref) == VLNV::DESIGNCONFIGURATION)
        {
            VLNV designVLNV = libraryHandler_->getDesignVLNV(ref);

            QSharedPointer<Document> libComp2 = libraryHandler_->getModel(ref);
            QSharedPointer<DesignConfiguration> desConf = libComp2.staticCast<DesignConfiguration>();
            VLNV refToDesign = desConf->getDesignRef();

            // if the referenced design was not found in the library
            if (!designVLNV.isValid())
            {
                emit errorMessage(tr("Component %1 has hierarchical reference to object %2, "
                    "which is design configuration and references to design %3. This "
                    "design is not found in library so component can not be opened in "
                    "design view. Edit component with component editor to remove invalid references").arg(
                    referencingVlnv.toString(":"), ref.toString(":"), refToDesign.toString(":")));
                invalidReferences = true;
            }
        }
    }

    return invalidReferences;
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
// Function: MainWindow::onRunImportWizard()
//-----------------------------------------------------------------------------
void MainWindow::onRunImportWizard()
{
    TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());

    if (!doc || doc->isProtected())
    {
        return;
    }

    if (doc->isModified())
    {
        QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
            tr("The document has been modified. The changes need to be saved before the "
            "import wizard can be run. Save changes and continue?"),
            QMessageBox::Yes | QMessageBox::No, this);

        // if user does not want to save or save can't be done
        if (msgBox.exec() == QMessageBox::No || !doc->save())
        {
            return;
        }
    }

    VLNV vlnv = doc->getIdentifyingVLNV();
    QSharedPointer<Component> component = libraryHandler_->getModel(vlnv).dynamicCast<Component>();

    if (component)
    {
        ImportWizard wizard(component, libraryHandler_, this);

        if (wizard.exec() == QDialog::Accepted)
        {
            if (libraryHandler_->writeModelToFile(wizard.getComponent()))
            {
                doc->refresh();
            }
            else
            {
                emit errorMessage("Error saving file to disk.");
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MainWindow::createGeneratorPluginActions()
//-----------------------------------------------------------------------------
void MainWindow::createGeneratorPluginActions()
{
    pluginActionGroup_ = new QActionGroup(this);

    for (IPlugin* plugin : PluginManager::getInstance().getActivePlugins())
    {
        IGeneratorPlugin* genPlugin = dynamic_cast<IGeneratorPlugin*>(plugin);

        if (genPlugin != 0)
        {
            // Add a small overlay icon to the plugin icon to visualize that this is a plugin.
            QIcon pluginBaseIcon = genPlugin->getIcon();

            QPixmap icon(32, 32);
            icon.fill(Qt::transparent);

            QPainter painter(&icon);
            painter.drawPixmap(0, 0, pluginBaseIcon.pixmap(32, 32));

            QAction* action = new QAction(icon, genPlugin->getName(), this);
            action->setData(QVariant::fromValue((void*)genPlugin));

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

    for (QAction* action : actions)
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

    setPluginVisibilities();
}

//-----------------------------------------------------------------------------
// Function: MainWindow::runComponentWizard()
//-----------------------------------------------------------------------------
void MainWindow::runComponentWizard(QSharedPointer<Component> component, QString const& directory)
{
    // Open the component wizard.
    ComponentWizard wizard(component, directory, libraryHandler_, this);

    QString styleSheet("*[mandatoryField=\"true\"] { background-color: LemonChiffon; }");
    wizard.setStyleSheet(styleSheet);

    if (wizard.exec() == QDialog::Accepted)
    {
        // Save wizard changes.
        if (!libraryHandler_->writeModelToFile(wizard.getComponent()))
        {
            emit errorMessage("Error saving file to disk.");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: mainwindow::setPluginVisibilities()
//-----------------------------------------------------------------------------
void MainWindow::setPluginVisibilities()
{
    TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());

    // Enable/disable the plugin generator actions based on the component being edited in the document.
    VLNV compVLNV;
    VLNV desVLNV;
    QSharedPointer<Component const> component;
    QSharedPointer<Design const> design;
    QSharedPointer<DesignConfiguration const> designConfiguration;

    if (doc)
    {
        compVLNV = doc->getDocumentVLNV();
        desVLNV = doc->getIdentifyingVLNV();
        QSharedPointer<Document const> libComp = libraryHandler_->getModelReadOnly(compVLNV);
        component = libComp.dynamicCast<Component const>();

        if (component)
        {
            // if the design is supported by the document type
            DesignWidget* desWidget = qobject_cast<DesignWidget*>(doc);
            if (desWidget)
            {
                // design is the object that identifies the editor
                QSharedPointer<Document const> libDes = libraryHandler_->getModelReadOnly(desVLNV);
                design = libDes.dynamicCast<Design const>();

                // find the design config is one exists
                QString viewName = desWidget->getOpenViewName();

                // The name must exist!
                Q_ASSERT(!viewName.isEmpty());

                VLNV desConfVLNV;

                // the implementation defines where to search for the hierarchy ref
                switch (desWidget->getImplementation())
                {
                case KactusAttribute::HW:
                case KactusAttribute::SW:
                    {
                        desConfVLNV = component->getHierRef(viewName);
                        break;
                    }
                case KactusAttribute::SYSTEM:
                    {
                        desConfVLNV = component->getHierSystemRef(viewName);
                        break;
                    }
                default:
                    {
                        Q_ASSERT(false);
                        return;
                    }
                }

                // if the hierarchy ref is not directly to the design but design config is in between
                if (desConfVLNV.isValid() && desConfVLNV != desVLNV)
                {
                     QSharedPointer<Document const> libDesConf = libraryHandler_->getModelReadOnly(desConfVLNV);
                     designConfiguration = libDesConf.dynamicCast<DesignConfiguration const>();
                }
            }
        }
    }

    for (QAction* action : pluginActionGroup_->actions())
    {
        IGeneratorPlugin* plugin = reinterpret_cast<IGeneratorPlugin*>(action->data().value<void*>());
        Q_ASSERT(plugin != 0);

        action->setVisible(component != 0 && plugin->checkGeneratorSupport(component, design, designConfiguration));
    }


    bool isGenerationGroupVisible = false;
    for (QAction* action : generationGroup_->actions())
    {
        if (action->isVisible())
        {
            isGenerationGroupVisible = true;
            break;
        }
    }

    generationAction_->setVisible(isGenerationGroupVisible && doc);
}

//-----------------------------------------------------------------------------
// Function: mainwindow::onDesignDocumentRefreshed()
//-----------------------------------------------------------------------------
void MainWindow::onDesignDocumentRefreshed()
{
    dockHandler_->refreshDesignDocument(designTabs_->currentWidget());
}

//-----------------------------------------------------------------------------
// Function: mainwindow::onConfigureViews()
//-----------------------------------------------------------------------------
void MainWindow::onConfigureViews()
{
    TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());

    if (!doc || doc->isProtected())
    {
        return;
    }

    if (doc->isModified())
    {
        QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(), tr("The document has been "
            "modified. The changes need to be saved before the import wizard can be run. Save changes and "
            "continue?"), QMessageBox::Yes | QMessageBox::No, this);

        if (msgBox.exec() == QMessageBox::No || !doc->save())
        {
            return;
        }
    }

    VLNV componentVLNV = doc->getDocumentVLNV();
    VLNV designVLNV = doc->getIdentifyingVLNV();

    QSharedPointer<Component> currentComponent = libraryHandler_->getModel(componentVLNV).dynamicCast<Component>();
    QSharedPointer<Design> libraryDesign;
    QSharedPointer<DesignConfiguration> libraryDesignConfiguration;

    DesignWidget* desWidget = qobject_cast<DesignWidget*>(doc);
    QString viewName ("");

    if (desWidget && componentVLNV != designVLNV)
    {
        libraryDesign = libraryHandler_->getModel(designVLNV).dynamicCast<Design>();

        viewName = desWidget->getOpenViewName();

        VLNV designConfigurationVLNV;

        if (desWidget->getImplementation() == KactusAttribute::HW)
        {
            designConfigurationVLNV = currentComponent->getHierRef(viewName);
        }
        else
        {
            return;
        }

        if (designConfigurationVLNV.isValid() && designConfigurationVLNV != designVLNV)
        {
            libraryDesignConfiguration = libraryHandler_->getModel(designConfigurationVLNV).
                dynamicCast<DesignConfiguration>();
        }
    }

    if (currentComponent)
    {
        ViewConfigurer configurer(currentComponent, libraryDesign, libraryDesignConfiguration, libraryHandler_,
            viewName);

        if (configurer.exec() == QDialog::Accepted)
        {
            doc->refresh();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: mainwindow::onCondenseMemoryItems()
//-----------------------------------------------------------------------------
void MainWindow::onCondenseMemoryItems(bool condenseMemoryItems)
{
    MemoryDesignDocument* memoryDocument = dynamic_cast<MemoryDesignDocument*>(designTabs_->currentWidget());
    if (memoryDocument && !memoryDocument->isProtected())
    {
        memoryDocument->setCondenseMemoryItems(condenseMemoryItems);
    }
}

//-----------------------------------------------------------------------------
// Function: mainwindow::onFilterAddressSpaceChains()
//-----------------------------------------------------------------------------
void MainWindow::onFilterAddressSpaceChains(bool filterChains)
{
    MemoryDesignDocument* memoryDocument = dynamic_cast<MemoryDesignDocument*>(designTabs_->currentWidget());
    if (memoryDocument && !memoryDocument->isProtected())
    {
        memoryDocument->filterAddressSpaceChains(filterChains);
    }
}

//-----------------------------------------------------------------------------
// Function: mainwindow::onCondenseFieldItems()
//-----------------------------------------------------------------------------
void MainWindow::onCondenseFieldItems()
{
    MemoryDesignDocument* memoryDocument = dynamic_cast<MemoryDesignDocument*>(designTabs_->currentWidget());
    if (memoryDocument && !memoryDocument->isProtected())
    {
        memoryDocument->condenseFieldItems();
    }
}

//-----------------------------------------------------------------------------
// Function: mainwindow::onExtendFieldItems()
//-----------------------------------------------------------------------------
void MainWindow::onExtendFieldItems()
{
    MemoryDesignDocument* memoryDocument = dynamic_cast<MemoryDesignDocument*>(designTabs_->currentWidget());
    if (memoryDocument && !memoryDocument->isProtected())
    {
        memoryDocument->extendFieldItems();
    }
}

//-----------------------------------------------------------------------------
// Function: mainwindow::onFilterSegments()
//-----------------------------------------------------------------------------
void MainWindow::onFilterSegments(bool filterSegments)
{
    MemoryDesignDocument* memoryDocument = dynamic_cast<MemoryDesignDocument*>(designTabs_->currentWidget());
    if (memoryDocument && !memoryDocument->isProtected())
    {
        memoryDocument->filterAddressSpaceSegments(filterSegments);
    }
}

//-----------------------------------------------------------------------------
// Function: mainwindow::onFilterAddressBlocks()
//-----------------------------------------------------------------------------
void MainWindow::onFilterAddressBlocks(bool filterBlocks)
{
    MemoryDesignDocument* memoryDocument = dynamic_cast<MemoryDesignDocument*>(designTabs_->currentWidget());
    if (memoryDocument && !memoryDocument->isProtected())
    {
        memoryDocument->filterAddressBlocks(filterBlocks);
    }
}

//-----------------------------------------------------------------------------
// Function: mainwindow::onFilterRegisters()
//-----------------------------------------------------------------------------
void MainWindow::onFilterRegisters(bool filterRegisters)
{
    MemoryDesignDocument* memoryDocument = dynamic_cast<MemoryDesignDocument*>(designTabs_->currentWidget());
    if (memoryDocument && !memoryDocument->isProtected())
    {
        memoryDocument->filterAddressBlockRegisters(filterRegisters);
    }
}

//-----------------------------------------------------------------------------
// Function: mainwindow::onFilterFields()
//-----------------------------------------------------------------------------
void MainWindow::onFilterFields(bool filterFields)
{
    MemoryDesignDocument* memoryDocument = dynamic_cast<MemoryDesignDocument*>(designTabs_->currentWidget());
    if (memoryDocument && !memoryDocument->isProtected())
    {
        memoryDocument->filterFields(filterFields);
    }
}

//-----------------------------------------------------------------------------
// Function: mainwindow::onFilterUnconnectedMemoryItems()
//-----------------------------------------------------------------------------
void MainWindow::onFilterUnconnectedMemoryItems(bool filterUnconnected)
{
    MemoryDesignDocument* memoryDocument = dynamic_cast<MemoryDesignDocument*>(designTabs_->currentWidget());
    if (memoryDocument && !memoryDocument->isProtected())
    {
        memoryDocument->filterUnconnectedMemoryItems(filterUnconnected);
    }
}
