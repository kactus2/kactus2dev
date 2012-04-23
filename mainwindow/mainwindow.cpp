/* 
 *
 *  Created on: 24.2.2011
 * 		filename: mainwindow.cpp
 */

#include "mainwindow.h"

#include "SplashScreen.h"

#include "NewComponentPage.h"
#include "NewDesignPage.h"
#include "NewSWDesignPage.h"
#include "NewSystemPage.h"
#include "NewComDefinitionPage.h"
#include "NewApiDefinitionPage.h"
#include "newbuspage.h"

#include "NewWorkspaceDialog.h"
#include "DeleteWorkspaceDialog.h"

#include <common/KactusAttribute.h>
#include <settings/SettingsDialog.h>

#include <EndpointDesign/EndpointDesignWidget.h>
#include <EndpointDesign/ProgramEntityItem.h>
#include <EndpointDesign/ApplicationItem.h>

#include <MCAPI/MCAPISourceWidget.h>
#include <MCAPI/MCAPIContentMatcher.h>

#include <DocumentGenerator/documentgenerator.h>

#include <LibraryManager/libraryhandler.h>
#include <LibraryManager/vlnv.h>
#include <LibraryManager/LibraryUtils.h>

#include <common/dialogs/newObjectDialog/newobjectdialog.h>
#include <common/dialogs/listSelectDialog/ListSelectDialog.h>
#include <common/widgets/componentPreviewBox/ComponentPreviewBox.h>
#include <common/dialogs/propertyPageDialog/PropertyPageDialog.h>
#include <common/widgets/tabWidgetEx/TabWidgetEx.h>

#include <models/view.h>
#include <models/component.h>
#include <models/model.h>
#include <models/design.h>
#include <models/designconfiguration.h>
#include <models/abstractiondefinition.h>
#include <models/busdefinition.h>
#include <models/fileset.h>
#include <models/file.h>
#include <models/ComDefinition.h>
#include <models/ApiDefinition.h>

#include <designwidget/designwidget.h>
#include <designwidget/diagramcomponent.h>
#include <designwidget/diagramport.h>
#include <designwidget/diagraminterface.h>

#include <IPXactWrapper/ComDefinitionEditor/ComDefinitionEditor.h>
#include <IPXactWrapper/ApiDefinitionEditor/ApiDefinitionEditor.h>
#include <IPXactWrapper/BusEditor/buseditor.h>
#include <IPXactWrapper/ComponentEditor/ipxactcomponenteditor.h>
#include <IPXactWrapper/SWDesignEditor/SWDesignEditor.h>

#include <PropertyWidget/messageconsole.h>

#include <ComponentInstanceEditor/componentinstanceeditor.h>
#include <ConfigurationEditor/configurationeditor.h>
#include <InterfaceEditor/interfaceeditor.h>
#include <ConnectionEditor/connectioneditor.h>
#include <AdHocEditor/AdHocEditor.h>

#include <QCoreApplication>
#include <QSettings>
#include <QToolBar>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QDockWidget>
#include <QMessageBox>
#include <QDebug>
#include <QCoreApplication>
#include <QFileInfo>
#include <QUrl>
#include <QDesktopServices>
#include <QCursor>
#include <QDateTime>
#include <QElapsedTimer>

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
      instanceEditor_(0),
      instanceDock_(0),
      configurationEditor_(0),
      configurationDock_(0),
      interfaceEditor_(0),
      interfaceDock_(0),
      adHocEditor_(0),
      adHocDock_(0),
      connectionEditor_(0),
      connectionDock_(0),
      actNew_(0),
      actSave_(0),
      actSaveAs_(0),
      actPrint_(0), 
      editGroup_(0),
      actUndo_(0), 
      actRedo_(0),
      actLibrarySearch_(0), 
      actCheckIntegrity_(0),
      hwDesignGroup_(0),
      actAddColumn_(0), 
      actGenVHDL_(0), 
      actGenModelSim_(0),
      actGenQuartus_(0), 
      actGenDocumentation_(0),
      diagramToolsGroup_(0), 
      actToolSelect_(0), 
      actToolConnect_(0),
      actToolInterface_(0),
      actToolDraft_(0),
      actToolToggleOffPage_(0),
      actZoomIn_(0),
      actZoomOut_(0), 
      actZoomOriginal_(0), 
      actFitInView_(0),
      actVisibleDocks_(0),
      actVisibilityControl_(0),
      actWorkspaces_(0),
      actRefresh_(0),
      actProtect_(0), 
      actSettings_(0),
      actAbout_(0), 
      actExit_(0),
      showOutputAction_(0),
      showPreviewAction_(0),
      showLibraryAction_(0),
      showConfigurationAction_(0),
      showConnectionAction_(0),
      showInterfaceAction_(0),
      showInstanceAction_(0),
      showAdHocAction_(0),
      windowsMenu_(this),
      visibilityMenu_(this),
      workspaceMenu_(this),
      curWorkspaceName_("Default"),
      visibilities_()
{
	// set the identification tags for the application
	QCoreApplication::setOrganizationDomain(tr("tut.fi"));
	QCoreApplication::setOrganizationName(tr("TUT"));
	QCoreApplication::setApplicationName(tr("Kactus2"));
	QCoreApplication::setApplicationVersion("1.0");

	// set the top title for the application
	setWindowTitle(QCoreApplication::applicationName());

	// Set the application icon.
	setWindowIcon(QIcon(":icons/graphics/appicon.png"));

	// By default, the window is 1024x768 and set to maximized state.
	resize(1024, 768);
	setWindowState(Qt::WindowMaximized);

	// set up the widgets
	setupMessageConsole();
	setupDrawBoard();
	setupLibraryDock();
	setupInstanceEditor();
    setupAdHocVisibilityEditor();
	setupConfigurationEditor();
	setupInterfaceEditor();
	setupConnectionEditor();

	// some actions need the editors so set them up before the actions
	setupActions();

	// Restore program settings.
	restoreSettings();

	// don't display empty editors
	updateWindows(TabDocument::OUTPUTWINDOW | TabDocument::LIBRARYWINDOW | 
		          TabDocument::PREVIEWWINDOW);

	setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
}

//-----------------------------------------------------------------------------
// Function: ~MainWindow()
//-----------------------------------------------------------------------------
MainWindow::~MainWindow() {

	saveSettings();
}

//-----------------------------------------------------------------------------
// Function: openDesign()
//-----------------------------------------------------------------------------
void MainWindow::openDesign(const VLNV& vlnv, const QString& viewName, bool forceUnlocked)
{

	// the vlnv must always be for a component
	Q_ASSERT(libraryHandler_->getDocumentType(vlnv) == VLNV::COMPONENT);

	// Check if the design is already open and activate it.
	if (vlnv.isValid()) {
		for (int i = 0; i < designTabs_->count(); i++) {
			DesignWidget *designWidget = dynamic_cast<DesignWidget*>(designTabs_->widget(i));

			if (designWidget != 0 && *designWidget->getOpenDocument() == vlnv) {
				designTabs_->setCurrentIndex(i);
				return;
			}
		}
	}

	// parse the referenced component
	QSharedPointer<LibraryComponent> libComp = libraryHandler_->getModel(vlnv);
	QSharedPointer<Component> comp = libComp.staticCast<Component>();
	QList<VLNV> hierRefs = comp->getHierRefs();

	// component must have at least one hierarchical reference
	//Q_ASSERT(!hierRefs.isEmpty());

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

	DesignWidget *designWidget = new DesignWidget(libraryHandler_, this);

	connect(designWidget->getEditProvider(), SIGNAL(editStateChanged()), this, SLOT(updateMenuStrip()));
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
	connect(designWidget, SIGNAL(errorMessage(const QString&)),
		console_, SLOT(onErrorMessage(const QString&)), Qt::UniqueConnection);
	connect(designWidget, SIGNAL(noticeMessage(const QString&)),
		console_, SLOT(onNoticeMessage(const QString&)), Qt::UniqueConnection);

	connect(designWidget, SIGNAL(clearItemSelection()),
		libraryHandler_, SLOT(onClearSelection()), Qt::UniqueConnection);

	connect(designWidget, SIGNAL(componentSelected(ComponentItem*)),
		this, SLOT(onComponentSelected(ComponentItem*)), Qt::UniqueConnection);
	connect(designWidget, SIGNAL(interfaceSelected(DiagramInterface*)),
		this, SLOT(onInterfaceSelected(DiagramInterface*)), Qt::UniqueConnection);
	connect(designWidget, SIGNAL(portSelected(DiagramPort*)),
		this, SLOT(onPortSelected(DiagramPort*)), Qt::UniqueConnection);
    connect(designWidget, SIGNAL(adHocPortSelected(DiagramAdHocPort*)),
        this, SLOT(onAdHocPortSelected(DiagramAdHocPort*)), Qt::UniqueConnection);
    connect(designWidget, SIGNAL(adHocInterfaceSelected(DiagramAdHocInterface*)),
        this, SLOT(onAdHocInterfaceSelected(DiagramAdHocInterface*)), Qt::UniqueConnection);
	connect(designWidget, SIGNAL(connectionSelected(DiagramInterconnection*)),
		this, SLOT(onConnectionSelected(DiagramInterconnection*)), Qt::UniqueConnection);

	connect(designWidget, SIGNAL(clearItemSelection()),
		this, SLOT(onClearItemSelection()), Qt::UniqueConnection);

	connect(designWidget, SIGNAL(modifiedChanged(bool)),
		actSave_, SLOT(setEnabled(bool)), Qt::UniqueConnection);

	// open the design in the designWidget
	designWidget->setDesign(&vlnv, viewName);

	// if the design could not be opened
	if (!designWidget->getOpenDocument()) {
		delete designWidget;
		return;
	}

	if (forceUnlocked)
	{
		designWidget->setProtection(false);
	}
	else 
	{
		// Open in unlocked mode by default only if the version is draft.
		designWidget->setProtection(vlnv.getVersion() != "draft");
	}

	designWidget->setTabWidget(designTabs_);

	// A small hack to center the view correctly.
	//     designWidget->fitInView();
	//     designWidget->setZoomLevel(100);
}


//-----------------------------------------------------------------------------
// Function: openSWDesign()
//-----------------------------------------------------------------------------
void MainWindow::openSWDesign(const VLNV& vlnv, bool forceUnlocked)
{
	// Check if the SW design editor is already open and activate it.
	if (vlnv.isValid())
	{
		for (int i = 0; i < designTabs_->count(); i++)
		{
			SWDesignEditor* editor = dynamic_cast<SWDesignEditor*>(designTabs_->widget(i));

			if (editor && editor->getComponentVLNV() == vlnv)
			{
				designTabs_->setCurrentIndex(i);
				return;
			}
		}
	}

	// Editor was not yet open so create it.
	QSharedPointer<Component> component;

	if (libraryHandler_->contains(vlnv))
	{
		QSharedPointer<LibraryComponent> libComp = libraryHandler_->getModel(vlnv);
		component = libComp.dynamicCast<Component>();
	}
	else
	{
		emit errorMessage(tr("VLNV %1:%2:%3:%4 was not found in the library").arg(
			vlnv.getVendor()).arg(
			vlnv.getLibrary()).arg(
			vlnv.getName()).arg(
			vlnv.getVersion()));
		return;
	}

	if (!component) {
		emit errorMessage(tr("Document type did not match Component"));
		return;
	}

	QString filePath = libraryHandler_->getPath(vlnv);
	QFileInfo info(filePath);

	SWDesignEditor* editor = new SWDesignEditor(this, this, libraryHandler_, component);

	if (forceUnlocked)
	{
		editor->setProtection(false);
	}

	editor->setTabWidget(designTabs_);

	connect(editor, SIGNAL(errorMessage(const QString&)),
		console_, SLOT(onErrorMessage(const QString&)), Qt::UniqueConnection);
	connect(editor, SIGNAL(noticeMessage(const QString&)),
		console_, SLOT(onNoticeMessage(const QString&)), Qt::UniqueConnection);
	connect(editor, SIGNAL(contentChanged()),
		this, SLOT(updateMenuStrip()), Qt::UniqueConnection);
	connect(editor, SIGNAL(modifiedChanged(bool)),
		actSave_, SLOT(setEnabled(bool)), Qt::UniqueConnection);
}

void MainWindow::onLibrarySearch() {

	if (libraryHandler_) {
		libraryHandler_->searchForIPXactFiles();
	}
}

void MainWindow::setupActions() {

	// the action to create a new hierarchical component
	actNew_ = new QAction(QIcon(":/icons/graphics/file-new.png"), tr("New"), this);
	actNew_->setShortcut(QKeySequence::New);
	actNew_->setProperty("rowSpan", 2);
	actNew_->setProperty("colSpan", 2);
	connect(actNew_, SIGNAL(triggered()), this, SLOT(createNew()));

	actSave_ = new QAction(QIcon(":/icons/graphics/file-save.png"), tr("Save"), this);
	actSave_->setShortcut(QKeySequence::Save);
	actSave_->setProperty("rowSpan", 2);
	actSave_->setProperty("colSpan", 2);
	actSave_->setEnabled(false);
	connect(actSave_, SIGNAL(triggered()), this, SLOT(saveCurrent()));

	actSaveAs_ = new QAction(QIcon(":/icons/graphics/file-save.png"), tr("Save As"), this);
	actSaveAs_->setShortcut(QKeySequence::SaveAs);
	actSaveAs_->setProperty("rowSpan", 2);
	actSaveAs_->setProperty("colSpan", 2);
	actSaveAs_->setEnabled(false);
	connect(actSaveAs_, SIGNAL(triggered()), this, SLOT(saveCurrentAs()));

	actSaveAll_ = new QAction(QIcon(":/icons/graphics/file-save_all.png"), tr("Save All"), this);
	actSaveAll_->setShortcut(QKeySequence("Ctrl+Shift+S"));
	actSaveAll_->setProperty("rowSpan", 2);
	actSaveAll_->setProperty("colSpan", 2);
	connect(actSaveAll_, SIGNAL(triggered()), this, SLOT(saveAll()));

	actPrint_ = new QAction(QIcon(":/icons/graphics/file-print.png"), tr("Print"), this);
	actPrint_->setShortcut(QKeySequence::Print);
	actPrint_->setProperty("rowSpan", 2);
	actPrint_->setProperty("colSpan", 2);
	actPrint_->setEnabled(false);
	connect(actPrint_, SIGNAL(triggered()), this, SLOT(printCurrent()));

	actUndo_ = new QAction(QIcon(":/icons/graphics/edit-undo.png"), tr("Undo"), this);
	actUndo_->setShortcut(QKeySequence::Undo);
	actUndo_->setProperty("rowSpan", 2);
	actUndo_->setProperty("colSpan", 2);
	connect(actUndo_, SIGNAL(triggered()), this, SLOT(undo()));

	actRedo_ = new QAction(QIcon(":/icons/graphics/edit-redo.png"), tr("Redo"), this);
	actRedo_->setShortcut(QKeySequence::Redo);
	actRedo_->setProperty("rowSpan", 2);
	actRedo_->setProperty("colSpan", 2);
	connect(actRedo_, SIGNAL(triggered()), this, SLOT(redo()));

	// the action to search for IP-Xact documents in file system
	actLibrarySearch_ = new QAction(QIcon(":/icons/graphics/library-refresh.png"),
		tr("Refresh Library"), this);
	connect(actLibrarySearch_, SIGNAL(triggered()),
		this, SLOT(onLibrarySearch()), Qt::UniqueConnection);

	// Check the library integrity
	actCheckIntegrity_ = new QAction(QIcon(":/icons/graphics/checkIntegrity.png"),
		tr("Check Integrity"), this);
	connect(actCheckIntegrity_, SIGNAL(triggered()),
		libraryHandler_, SLOT(onCheckLibraryIntegrity()), Qt::UniqueConnection);

	// Initialize the action to generate VHDL.
	actGenVHDL_ = new QAction(QIcon(":/icons/graphics/vhdl_gen.png"),
		tr("Generate Top-VHDL"), this);
	connect(actGenVHDL_, SIGNAL(triggered()), 
		this, SLOT(generateVHDL()), Qt::UniqueConnection);

	// Initialize the action to generate a ModelSim makefile.
	actGenModelSim_ = new QAction(QIcon(":/icons/graphics/modelsim_generator.png"),
		tr("Generate ModelSim Makefile"), this);
	connect(actGenModelSim_, SIGNAL(triggered()), 
		this, SLOT(generateModelSim()), Qt::UniqueConnection);

	// Initialize the action to generate a Quartus project.
	actGenQuartus_ = new QAction(QIcon(":/icons/graphics/quartus_generator.png"),
		tr("Generate Quartus Project"), this);
	connect(actGenQuartus_, SIGNAL(triggered()), 
		this, SLOT(generateQuartus()), Qt::UniqueConnection);

	// initialize the action to generate documentation for the component/design
	actGenDocumentation_ = new QAction(QIcon(":icons/graphics/documentation.png"),
		tr("Generate documentation"), this);
	connect(actGenDocumentation_, SIGNAL(triggered()),
		this, SLOT(generateDoc()), Qt::UniqueConnection);

	// Initialize the action to add a new column.
	actAddColumn_ = new QAction(QIcon(":/icons/graphics/diagram-add-column.png"), tr("Add Column"), this);
	actAddColumn_->setProperty("rowSpan", 2);
	actAddColumn_->setProperty("colSpan", 2);
	connect(actAddColumn_, SIGNAL(triggered()), 
		this, SLOT(addColumn()), Qt::UniqueConnection);

	// Initialize the action to set draw mode to selection mode.
	actToolSelect_ = new QAction(QIcon(":/icons/graphics/tool-select.png"), tr("Select Tool"), this);
	actToolSelect_->setCheckable(true);
	actToolSelect_->setChecked(true);

	// Initialize the action to set draw mode to connection mode.
	actToolConnect_ = new QAction(QIcon(":/icons/graphics/tool-interconnection.png"),
		tr("Interconnection Tool"), this);
	actToolConnect_->setCheckable(true);

	// Initialize the action to set draw mode to interface mode.
	actToolInterface_ = new QAction(QIcon(":/icons/graphics/tool-interface.png"), tr("Interface Tool"), this);
	actToolInterface_->setCheckable(true);

	actToolDraft_ = new QAction(QIcon(":/icons/graphics/tool-drafting.png"), tr("Drafting Tool"), this);
	actToolDraft_->setCheckable(true);

    actToolToggleOffPage_ = new QAction(QIcon(":/icons/graphics/tool-toggle_offpage.png"),
                                        tr("Toggle Off-Page Tool"), this);
    actToolToggleOffPage_->setCheckable(true);

	modeActionGroup_ = new QActionGroup(this);
	modeActionGroup_->setExclusive(true);
	modeActionGroup_->addAction(actToolSelect_);
	modeActionGroup_->addAction(actToolConnect_);
	modeActionGroup_->addAction(actToolInterface_);
	modeActionGroup_->addAction(actToolDraft_);
    modeActionGroup_->addAction(actToolToggleOffPage_);
	connect(modeActionGroup_, SIGNAL(triggered(QAction *)), this, SLOT(drawModeChange(QAction *)));

	// Initialize the action to zoom in.
	actZoomIn_ = new QAction(QIcon(":/icons/graphics/view-zoom_in.png"), tr("Zoom In"), this);
	actZoomIn_->setEnabled(false);
	connect(actZoomIn_, SIGNAL(triggered()), 
		this, SLOT(zoomIn()));

	// Initialize the action to zoom out.
	actZoomOut_ = new QAction(QIcon(":/icons/graphics/view-zoom_out.png"), tr("Zoom Out"), this);
	actZoomOut_->setEnabled(false);
	connect(actZoomOut_, SIGNAL(triggered()), this, SLOT(zoomOut()));

	// Initialize the action to reset the zoom to original 1:1 ratio.
	actZoomOriginal_ = new QAction(QIcon(":/icons/graphics/view-zoom_original.png"),
		tr("Original 1:1 Zoom"), this);
	actZoomOriginal_->setEnabled(false);
	connect(actZoomOriginal_, SIGNAL(triggered()), this, SLOT(zoomOriginal()));

	// Initialize the action to fit the document into the view.
	actFitInView_ = new QAction(QIcon(":/icons/graphics/view-fit_best.png"),
		tr("Fit Document to View"), this);
	actFitInView_->setEnabled(false);
	connect(actFitInView_, SIGNAL(triggered()), this, SLOT(fitInView()));

	// the action for user to select the visible docks
	actVisibleDocks_ = new QAction(QIcon(":icons/graphics/dockSelect.png"),
		tr("Visible Windows"), this);
	connect(actVisibleDocks_, SIGNAL(triggered()), this, SLOT(selectVisibleDocks()), Qt::UniqueConnection);

    // Initialize the action to manage visibility control.
    actVisibilityControl_ = new QAction(QIcon(":icons/graphics/visibility.png"), tr("Visibility Control"), this);
    actVisibilityControl_->setEnabled(false);
    connect(actVisibilityControl_, SIGNAL(triggered()),
            this, SLOT(openVisibilityControlMenu()), Qt::UniqueConnection);

    // Initialize the action to manage workspaces.
    actWorkspaces_ = new QAction(QIcon(":icons/graphics/workspace.png"),
                                 tr("Workspaces"), this);
    connect(actWorkspaces_, SIGNAL(triggered()), this, SLOT(openWorkspaceMenu()), Qt::UniqueConnection);

    actRefresh_ = new QAction(QIcon(":/icons/graphics/refresh.png"), tr("Refresh"), this);
    actRefresh_->setProperty("rowSpan", 2);
    actRefresh_->setProperty("colSpan", 2);
    actRefresh_->setShortcut(QKeySequence("F5"));
    connect(actRefresh_, SIGNAL(triggered(bool)), this, SLOT(refresh()));

	actProtect_ = new QAction(QIcon(":/icons/graphics/protection-unlocked.png"), tr("Unlocked"), this);
	actProtect_->setProperty("rowSpan", 2);
	actProtect_->setProperty("colSpan", 2);
	actProtect_->setCheckable(true);
	actProtect_->setEnabled(false);
    actProtect_->setShortcut(QKeySequence("Ctrl+Space"));
	connect(actProtect_, SIGNAL(triggered(bool)), this, SLOT(changeProtection(bool)));

	// Initialize the action to open Kactus2 settings.
	actSettings_ = new QAction(QIcon(":/icons/graphics/system-settings.png"), tr("Settings"), this);
	actSettings_->setProperty("rowSpan", 2);
	actSettings_->setProperty("colSpan", 2);
	connect(actSettings_, SIGNAL(triggered()), this, SLOT(openSettings()));

	// Initialize the action to open the about box.
	actAbout_= new QAction(QIcon(":/icons/graphics/system-about.png"), tr("About"), this);
	actAbout_->setProperty("rowSpan", 2);
	actAbout_->setProperty("colSpan", 2);
	connect(actAbout_, SIGNAL(triggered()), 
		this, SLOT(showAbout()), Qt::UniqueConnection);

	// Initialize the action to exit the program.
	actExit_ = new QAction(QIcon(":/icons/graphics/system-exit.png"), tr("Exit"), this);
	actExit_->setProperty("rowSpan", 2);
	actExit_->setProperty("colSpan", 2);
	connect(actExit_, SIGNAL(triggered()),
		this, SLOT(close()), Qt::UniqueConnection);

	// the actions that select which windows to display

	// Action to show/hide the output window.
	showOutputAction_ = new QAction(tr("Output window"), this);
	showOutputAction_->setCheckable(true);
	showOutputAction_->setChecked(true);
	connect(showOutputAction_, SIGNAL(toggled(bool)),
		this, SLOT(onOutputAction(bool)), Qt::UniqueConnection);
	connect(consoleDock_->toggleViewAction(), SIGNAL(toggled(bool)),
		showOutputAction_, SLOT(setChecked(bool)), Qt::UniqueConnection);

	// Action to show/hide the preview box.
	showPreviewAction_ = new QAction(tr("Preview box"), this);
	showPreviewAction_->setCheckable(true);
	showPreviewAction_->setChecked(true);
	connect(showPreviewAction_, SIGNAL(toggled(bool)),
		this, SLOT(onPreviewAction(bool)), Qt::UniqueConnection);
	connect(previewDock_->toggleViewAction(), SIGNAL(toggled(bool)),
		showPreviewAction_, SLOT(setChecked(bool)), Qt::UniqueConnection);

	// Action to show/hide the library window.
	showLibraryAction_ = new QAction(tr("Library window"), this);
	showLibraryAction_->setCheckable(true);
	showLibraryAction_->setChecked(true);
	connect(showLibraryAction_, SIGNAL(toggled(bool)),
		this, SLOT(onLibraryAction(bool)), Qt::UniqueConnection);
	connect(libraryDock_->toggleViewAction(), SIGNAL(toggled(bool)),
		showLibraryAction_, SLOT(setChecked(bool)), Qt::UniqueConnection); 		
	

	// Action to show/hide the configuration window.
	showConfigurationAction_ = new QAction(tr("Configuration window"), this);
	showConfigurationAction_->setCheckable(true);
	showConfigurationAction_->setChecked(true);
	connect(showConfigurationAction_, SIGNAL(toggled(bool)),
		this, SLOT(onConfigurationAction(bool)), Qt::UniqueConnection);
	connect(configurationDock_->toggleViewAction(), SIGNAL(toggled(bool)),
		showConfigurationAction_, SLOT(setChecked(bool)), Qt::UniqueConnection);

	// Action to show/hide the connection editor.
	showConnectionAction_ = new QAction(tr("Connection editor"), this);
	showConnectionAction_->setCheckable(true);
	showConnectionAction_->setChecked(true);
	connect(showConnectionAction_, SIGNAL(toggled(bool)),
		this, SLOT(onConnectionAction(bool)), Qt::UniqueConnection);
	connect(connectionDock_->toggleViewAction(), SIGNAL(toggled(bool)),
		showConnectionAction_, SLOT(setChecked(bool)), Qt::UniqueConnection);

	// Action to show/hide the interface editor.
	showInterfaceAction_ = new QAction(tr("Interface editor"), this);
	showInterfaceAction_->setCheckable(true);
	showInterfaceAction_->setChecked(true);
	connect(showInterfaceAction_, SIGNAL(toggled(bool)),
		this, SLOT(onInterfaceAction(bool)), Qt::UniqueConnection);
	connect(interfaceDock_->toggleViewAction(), SIGNAL(toggled(bool)),
		showInterfaceAction_, SLOT(setChecked(bool)), Qt::UniqueConnection);

	// Action to show/hide the instance editor.
	showInstanceAction_ = new QAction(tr("Instance editor"), this);
	showInstanceAction_->setCheckable(true);
	showInstanceAction_->setChecked(true);
	connect(showInstanceAction_, SIGNAL(toggled(bool)),
		this, SLOT(onInstanceAction(bool)), Qt::UniqueConnection);
	connect(instanceDock_->toggleViewAction(), SIGNAL(toggled(bool)),
		showInstanceAction_, SLOT(setChecked(bool)), Qt::UniqueConnection);

    // Action to show/hide the ad-hoc visibility editor.
    showAdHocAction_ = new QAction(tr("Ad-hoc Visibility Editor"), this);
    showAdHocAction_->setCheckable(true);
    showAdHocAction_->setChecked(true);
    connect(showAdHocAction_, SIGNAL(toggled(bool)), this, SLOT(onAdHocAction(bool)), Qt::UniqueConnection);
    connect(adHocDock_->toggleViewAction(), SIGNAL(toggled(bool)),
            showAdHocAction_, SLOT(setChecked(bool)), Qt::UniqueConnection);
	
	setupMenus();
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

    // If geometry is saved then restore it.
    restoreGeometry(settings.value("Geometry").toByteArray());

    // If state of widgets is saved then restore it.
    if (settings.contains("WindowState")) {
        restoreState(settings.value("WindowState").toByteArray());
    }

    if (settings.contains("WindowPosition"))
    {
        move(settings.value("WindowPosition").toPoint());
    }

    const bool configurationVisible = settings.value("ConfigurationVisibility", true).toBool();
    visibilities_.showConfiguration_ = configurationVisible;
    showConfigurationAction_->setChecked(configurationVisible);

    const bool connectionVisible = settings.value("ConnectionVisibility", true).toBool();
    visibilities_.showConnection_ = connectionVisible;
    showConnectionAction_->setChecked(connectionVisible);

    const bool instanceVisible = settings.value("InstanceVisibility", true).toBool();
    visibilities_.showInstance_ = instanceVisible;
    showInstanceAction_->setChecked(instanceVisible);

    const bool adHocVisible = settings.value("AdHocVisibility", true).toBool();
    visibilities_.showAdHocVisibility = adHocVisible;
    showAdHocAction_->setChecked(adHocVisible);

    const bool interfaceVisible = settings.value("InterfaceVisibility", true).toBool();
    visibilities_.showInterface_ = interfaceVisible;
    showInterfaceAction_->setChecked(interfaceVisible);

    const bool libraryVisible = settings.value("LibraryVisibility", true).toBool();
    visibilities_.showLibrary_ = libraryVisible;
    showLibraryAction_->setChecked(libraryVisible);

    const bool outputVisible = settings.value("OutputVisibility", true).toBool();
    visibilities_.showOutput_ = outputVisible;
    showOutputAction_->setChecked(outputVisible);

    const bool previewVisible = settings.value("PreviewVisibility", true).toBool();
    visibilities_.showPreview_ = previewVisible;
    showPreviewAction_->setChecked(previewVisible);

    if (designTabs_->count() == 0)
    {
        updateWindows(TabDocument::OUTPUTWINDOW | TabDocument::LIBRARYWINDOW | 
                      TabDocument::PREVIEWWINDOW);
    }
    else
    {
        TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());
        updateWindows(doc->getSupportedWindows());
    }
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

    // Save the geometry and state of windows.
    settings.beginGroup(keyName);

    settings.setValue("WindowState", saveState());
    settings.setValue("Geometry", saveGeometry());
    settings.setValue("WindowPosition", pos());
    settings.setValue("ConfigurationVisibility", visibilities_.showConfiguration_);
    settings.setValue("ConnectionVisibility", visibilities_.showConnection_);
    settings.setValue("InstanceVisibility", visibilities_.showInstance_);
    settings.setValue("InterfaceVisibility", visibilities_.showInterface_);
    settings.setValue("LibraryVisibility", visibilities_.showLibrary_);
    settings.setValue("OutputVisibility", visibilities_.showOutput_);
    settings.setValue("PreviewVisibility", visibilities_.showPreview_);

    settings.endGroup();
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

	menuStrip_ = new GCF::MenuStrip(menuDock);
	menuStrip_->setFixedHeight(100);

	menuDock->setWidget(menuStrip_);
	addDockWidget(Qt::TopDockWidgetArea, menuDock);

	// The "File" group.
	GCF::MenuStripGroup* fileGroup = menuStrip_->addGroup(tr("File"));
	fileGroup->addAction(actNew_);
	fileGroup->addAction(actSave_);
	fileGroup->addAction(actSaveAs_);
	fileGroup->addAction(actSaveAll_);
	fileGroup->addAction(actPrint_);

	// The "Edit" group.
	editGroup_ = menuStrip_->addGroup(tr("Edit"));
	editGroup_->addAction(actUndo_);
	editGroup_->addAction(actRedo_);
	editGroup_->setVisible(false);
	editGroup_->setEnabled(false);

	//! The "Library" group.
	GCF::MenuStripGroup* libGroup = menuStrip_->addGroup(tr("Library"));
	libGroup->addAction(actLibrarySearch_);
	libGroup->addAction(actCheckIntegrity_);

	//! The "HW Design" group.
	hwDesignGroup_ = menuStrip_->addGroup(tr("HW Design"));
	hwDesignGroup_->addAction(actGenVHDL_);
	hwDesignGroup_->addAction(actGenDocumentation_);
	hwDesignGroup_->addAction(actGenModelSim_);
	hwDesignGroup_->addAction(actGenQuartus_);
	hwDesignGroup_->setVisible(false);
	hwDesignGroup_->setEnabled(false);

	//! The "Diagram Tools" group.
	diagramToolsGroup_ = menuStrip_->addGroup(tr("           Diagram Tools           "));
	diagramToolsGroup_->addAction(actAddColumn_);
	diagramToolsGroup_->addAction(actToolSelect_);
	diagramToolsGroup_->addAction(actToolConnect_);
	diagramToolsGroup_->addAction(actToolInterface_);
	diagramToolsGroup_->addAction(actToolDraft_);
    diagramToolsGroup_->addAction(actToolToggleOffPage_);
	diagramToolsGroup_->setVisible(false);

	//! The "View" group.
	GCF::MenuStripGroup* viewGroup = menuStrip_->addGroup(tr("View"));
	viewGroup->addAction(actZoomIn_);
	viewGroup->addAction(actZoomOut_);
	viewGroup->addAction(actZoomOriginal_);
	viewGroup->addAction(actFitInView_);
	viewGroup->addAction(actVisibleDocks_);
    viewGroup->addAction(actVisibilityControl_);
    viewGroup->addAction(actWorkspaces_);

	//! The Protection group.
	protectGroup_ = menuStrip_->addGroup(tr("Protection"));
	protectGroup_->addAction(actProtect_);
    protectGroup_->addAction(actRefresh_);
	protectGroup_->setVisible(false);

	//! The "System" group.
	GCF::MenuStripGroup* sysGroup = menuStrip_->addGroup(tr("System"));
	sysGroup->addAction(actSettings_);
	sysGroup->addAction(actAbout_);
	sysGroup->addAction(actExit_);

	// the menu to display the dock widgets
	windowsMenu_.addAction(showOutputAction_);
	windowsMenu_.addAction(showPreviewAction_);
	windowsMenu_.addAction(showLibraryAction_);
}

void MainWindow::setupDrawBoard() {

	designTabs_ = new TabWidgetEx();
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

	connect(libraryHandler_, SIGNAL(openDesign(const VLNV&, const QString&)),
		this, SLOT(openDesign(const VLNV&, const QString&)));
	connect(libraryHandler_, SIGNAL(createBus(const VLNV&, const QString&)),
		this, SLOT(createBus(const VLNV&, const QString&)), Qt::UniqueConnection);
	connect(libraryHandler_, SIGNAL(createAbsDef(const VLNV&, const QString&, bool)),
		this, SLOT(createAbsDef(const VLNV&, const QString&, bool)), Qt::UniqueConnection);
    connect(libraryHandler_, SIGNAL(createComDef(const VLNV&, const QString&)),
        this, SLOT(createComDefinition(const VLNV&, const QString&)), Qt::UniqueConnection);
    connect(libraryHandler_, SIGNAL(createApiDef(const VLNV&, const QString&)),
        this, SLOT(createApiDefinition(const VLNV&, const QString&)), Qt::UniqueConnection);
	connect(libraryHandler_, SIGNAL(openComponent(const VLNV&)),
		this, SLOT(openComponent(const VLNV&)), Qt::UniqueConnection);
	connect(libraryHandler_, SIGNAL(openSWDesign(const VLNV&)),
		this, SLOT(openSWDesign(const VLNV&)), Qt::UniqueConnection);
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
	connect(libraryHandler_, SIGNAL(openSystem(const VLNV&)),
		this, SLOT(openSystem(const VLNV&)), Qt::UniqueConnection);

	connect(libraryHandler_, SIGNAL(createComponent(KactusAttribute::ProductHierarchy,
		KactusAttribute::Firmness,
		const VLNV&, const QString&)),
		this, SLOT(createComponent(KactusAttribute::ProductHierarchy,
		KactusAttribute::Firmness,
		const VLNV&, const QString&)), Qt::UniqueConnection);
}

void MainWindow::setupMessageConsole() {

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
}

void MainWindow::setupConfigurationEditor() {

	configurationDock_ = new QDockWidget(tr("Configuration Details"), this);
	configurationDock_->setObjectName(tr("Configuration Editor"));
	configurationDock_->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	configurationDock_->setFeatures(QDockWidget::AllDockWidgetFeatures);

	configurationEditor_ = new ConfigurationEditor(libraryHandler_, configurationDock_);
	configurationDock_->setWidget(configurationEditor_);
	addDockWidget(Qt::RightDockWidgetArea, configurationDock_);

	connect(configurationEditor_, SIGNAL(contentChanged()),
		this, SLOT(onDesignChanged()), Qt::UniqueConnection);
}

void MainWindow::setupInstanceEditor() {

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

void MainWindow::onDesignChanged() {

	// find the currently open editor
	QWidget* widget = designTabs_->currentWidget();

	// if editor was found
	if (widget) {
		DesignWidget* editor = qobject_cast<DesignWidget*>(widget);

		// if editor is design widget then set it to be modified.
		if (editor) {
			editor->setModified(true);
			updateMenuStrip();
		}
	}
}

void MainWindow::onClearItemSelection()
{
    DesignWidget* designWidget = dynamic_cast<DesignWidget*>(designTabs_->currentWidget());

    if (designWidget != 0)
    {
        adHocEditor_->setDataSource(designWidget->scene());
    }
    else
    {
        adHocEditor_->clear();
    }

	instanceEditor_->clear();
	interfaceEditor_->clear();
	connectionEditor_->clear();
}

void MainWindow::onComponentSelected( ComponentItem* component ) {
	Q_ASSERT(component);

	connectionEditor_->clear();
	interfaceEditor_->clear();

	// Update the instance and ad-hoc visibility editors.
	instanceEditor_->setComponent(component);

    if (dynamic_cast<DiagramComponent*>(component) != 0)
    {
        adHocEditor_->setDataSource(dynamic_cast<DiagramComponent*>(component));
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

void MainWindow::onPortSelected( DiagramPort* port ) {
	Q_ASSERT(port);

	// if the port has an encompassing component then it is selected
	DiagramComponent* component = port->encompassingComp();

	if (component->componentModel()->getVlnv()->isValid()) {

		libraryHandler_->onSelectionChanged(*component->componentModel()->getVlnv());
		previewBox_->setComponent(*component->componentModel()->getVlnv());
	}
	// if no component can be identified
	else
		libraryHandler_->onClearSelection();

	connectionEditor_->clear();
	instanceEditor_->clear();
    adHocEditor_->clear();

    interfaceEditor_->setInterface(port);
}

void MainWindow::onInterfaceSelected( DiagramInterface* interface ) {
	Q_ASSERT(interface);

    adHocEditor_->clear();
	connectionEditor_->clear();
	instanceEditor_->clear();
	interfaceEditor_->setInterface(interface);
}

//-----------------------------------------------------------------------------
// Function: MainWindow::onAdHocPortSelected()
//-----------------------------------------------------------------------------
void MainWindow::onAdHocPortSelected(DiagramAdHocPort* port)
{
    adHocEditor_->clear();
    connectionEditor_->clear();
    instanceEditor_->clear();
    interfaceEditor_->clear();
}

//-----------------------------------------------------------------------------
// Function: MainWindow::onAdHocInterfaceSelected()
//-----------------------------------------------------------------------------
void MainWindow::onAdHocInterfaceSelected(DiagramAdHocInterface* interface)
{
    adHocEditor_->clear();
    connectionEditor_->clear();
    instanceEditor_->clear();
    interfaceEditor_->clear();
}

void MainWindow::onConnectionSelected( DiagramInterconnection* connection ) {
	Q_ASSERT(connection);
    adHocEditor_->clear();
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
	IPXactComponentEditor* ipEditor = dynamic_cast<IPXactComponentEditor*>(doc);
	if (ipEditor) {
		isHWComp = ipEditor->isHWImplementation();
	}

	bool isHWDesign = false;
	DesignWidget* designWidget = dynamic_cast<DesignWidget*>(doc);
	if (designWidget) {
		isHWDesign = designWidget->isHWImplementation();
	}

	actSave_->setEnabled(doc != 0 && doc->isModified());
	actSaveAs_->setEnabled(doc != 0);
	actPrint_->setEnabled(doc != 0 && (doc->getFlags() & TabDocument::DOC_PRINT_SUPPORT));

	// if is hardware design then set all actions enabled
	if (isHWDesign) {
		hwDesignGroup_->setVisible(true);
		hwDesignGroup_->setEnabled(unlocked);
		actGenVHDL_->setEnabled(unlocked);
		actGenDocumentation_->setEnabled(unlocked);
		actGenModelSim_->setEnabled(unlocked);
		actGenQuartus_->setEnabled(unlocked);
	}
	// if is hardware component then set only documentation, modelsim and vhdl enabled
	else if (isHWComp) {
		hwDesignGroup_->setVisible(true);
		hwDesignGroup_->setEnabled(unlocked);
		actGenVHDL_->setEnabled(unlocked);
		actGenDocumentation_->setEnabled(unlocked);
		actGenModelSim_->setEnabled(unlocked);
		actGenQuartus_->setDisabled(true);
	}
	else {
		hwDesignGroup_->setVisible(false);
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

	bool oldProtectionState = actProtect_->isChecked();

	protectGroup_->setVisible(doc != 0 && (doc->getFlags() & TabDocument::DOC_PROTECTION_SUPPORT));
	actProtect_->setEnabled(doc != 0 && (doc->getFlags() & TabDocument::DOC_PROTECTION_SUPPORT));
	actProtect_->setChecked(doc != 0 && (doc->getFlags() & TabDocument::DOC_PROTECTION_SUPPORT) &&
		doc->isProtected());

	if (oldProtectionState != actProtect_->isChecked())
		onProtectionChanged(actProtect_->isChecked());

    actVisibilityControl_->setEnabled(doc != 0 && (doc->getFlags() & TabDocument::DOC_VISIBILITY_CONTROL_SUPPORT));

	updateZoomTools();
}

//-----------------------------------------------------------------------------
// Function: saveCurrentAs()
//-----------------------------------------------------------------------------
void MainWindow::saveCurrentAs() {
	TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());

	if (doc != 0)
	{
		doc->saveAs();
		onTabChanged(designTabs_->currentIndex());
		doc->refresh();
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
	EndpointDesignWidget* endpointWidget = dynamic_cast<EndpointDesignWidget*>(curWidget);

	if (designWidget != 0)
	{
		designWidget->addColumn();
	}
	else if (endpointWidget != 0)
	{
		endpointWidget->addColumn();
	}
}

//-----------------------------------------------------------------------------
// Function: generateVHDL()
//-----------------------------------------------------------------------------
void MainWindow::generateVHDL()
{
	DesignWidget* designWidget = dynamic_cast<DesignWidget*>(designTabs_->currentWidget());
	IPXactComponentEditor* compEditor = dynamic_cast<IPXactComponentEditor*>(designTabs_->currentWidget());

	if (designWidget != 0)
	{
		designWidget->onVhdlGenerate();
	}
	else if (compEditor) {

		// if user changed the metadata then the editor must be reopened
		if (compEditor->onVhdlGenerate()) {

			int index = designTabs_->currentIndex();
			VLNV compVLNV = compEditor->getComponentVLNV();

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
	DesignWidget* designWidget = dynamic_cast<DesignWidget*>(designTabs_->currentWidget());
	IPXactComponentEditor* compEditor = dynamic_cast<IPXactComponentEditor*>(designTabs_->currentWidget());

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
	DesignWidget* designWidget = dynamic_cast<DesignWidget*>(designTabs_->currentWidget());

	if (designWidget != 0)
	{
		designWidget->onQuartusGenerate();
	}
}

void MainWindow::generateDoc() {
	// get the vlnv of the current component
	TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());
	Q_ASSERT(doc);
	Q_ASSERT(doc->isHWImplementation());
	VLNV vlnv = doc->getComponentVLNV();
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
	IPXactComponentEditor* compEditor = dynamic_cast<IPXactComponentEditor*>(doc);
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
}

//-----------------------------------------------------------------------------
// Function: onDrawModeChanged()
//-----------------------------------------------------------------------------
void MainWindow::onDrawModeChanged(DrawMode mode)
{
	TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());

	if (doc != 0 && doc->getFlags() & TabDocument::DOC_DRAW_MODE_SUPPORT)
	{
		switch (mode)
		{
		case MODE_CONNECT:
		case MODE_INTERFACE:
		case MODE_DRAFT:
        case MODE_TOGGLE_OFFPAGE:
			{
				doc->setCursor(Qt::CrossCursor);
				break;
			}

		default:
			{
				// If mode is set to select.
				doc->setCursor(Qt::ArrowCursor);
				break;
			}
		}
	}

	actToolSelect_->setChecked(mode == MODE_SELECT);
	actToolConnect_->setChecked(mode == MODE_CONNECT);
	actToolInterface_->setChecked(mode == MODE_INTERFACE);
	actToolDraft_->setChecked(mode == MODE_DRAFT);
    actToolToggleOffPage_->setChecked(mode == MODE_TOGGLE_OFFPAGE);
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

	// remove the widget from the tabs
	designTabs_->removeTab(index);

	// delete the document.
	delete doc;

	// if there are no more tabs in the tab widget
	if (designTabs_->count() == 0) {
		// don't display empty editors
		updateWindows(TabDocument::OUTPUTWINDOW | TabDocument::LIBRARYWINDOW | 
			TabDocument::PREVIEWWINDOW);
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
		updateWindows(doc->getSupportedWindows());
	}

	// if the new tab is designWidget
	DesignWidget* designwidget = dynamic_cast<DesignWidget*>(doc);
	// set the configuration manager to edit the active design
	if (designwidget && designwidget->getHierComponent()->getComponentImplementation() == KactusAttribute::KTS_HW) {
		configurationEditor_->setConfiguration(designwidget, designwidget->isProtected());
	}

	else if (designwidget) {
		configurationEditor_->setConfiguration(designwidget, true);
	}
	// active tab is not design widget so clear the editors associated with design widget
	else {
		configurationEditor_->clear();
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
	// disconnect the signals that would otherwise change the window states to 
	// be saved

	// Action to show/hide the output window.
	disconnect(showOutputAction_, SIGNAL(toggled(bool)),
		this, SLOT(onOutputAction(bool)));

	// Action to show/hide the preview box.
	disconnect(showPreviewAction_, SIGNAL(toggled(bool)),
		this, SLOT(onPreviewAction(bool)));

	// Action to show/hide the library window.
	disconnect(showLibraryAction_, SIGNAL(toggled(bool)),
		this, SLOT(onLibraryAction(bool)));

	// Action to show/hide the configuration window.
	disconnect(showConfigurationAction_, SIGNAL(toggled(bool)),
		this, SLOT(onConfigurationAction(bool)));

	// Action to show/hide the connection editor.
	disconnect(showConnectionAction_, SIGNAL(toggled(bool)),
		this, SLOT(onConnectionAction(bool)));

	// Action to show/hide the interface editor.
	disconnect(showInterfaceAction_, SIGNAL(toggled(bool)),
		this, SLOT(onInterfaceAction(bool)));

	// Action to show/hide the instance editor.
	disconnect(showInstanceAction_, SIGNAL(toggled(bool)),
		this, SLOT(onInstanceAction(bool)));

    // Disconnect the action to show/hide the ad-hoc visibility editor.
    disconnect(showAdHocAction_, SIGNAL(toggled(bool)), this, SLOT(onAdHocAction(bool)));

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
// Function: openSource()
//-----------------------------------------------------------------------------
void MainWindow::openSource(ProgramEntityItem* progEntity)
{
	Q_ASSERT(progEntity != 0);

	ApplicationItem* appItem = progEntity->getApplication();
	Q_ASSERT(appItem != 0);

	FileSet* fileSet = appItem->componentModel()->getFileSet("cSources");

	if (fileSet == 0 || fileSet->getFiles().count() == 0)
	{
		return;
	}

	QString filename;

	if (fileSet->getFiles().count() > 1)
	{
		// Show a dialog for selecting what source file to open.
		ListSelectDialog dlg(this);
		dlg.setWindowTitle(tr("Open Source"));
		dlg.setDescription(tr("Select C source file to open:"));

		foreach (QSharedPointer<File> file, fileSet->getFiles())
		{
			dlg.addItem(new QListWidgetItem(file->getName()));
		}

		if (dlg.exec() == QDialog::Rejected)
		{
			return;
		}

		filename = dlg.getSelectedItem()->text();
	}
	else
	{
		// Otherwise there is only one possibility.
		filename = fileSet->getFiles().first()->getName();
	}

	if (appItem->componentModel()->getVlnv()->isValid())
	{
		filename = General::getAbsolutePath(libraryHandler_->getPath(*appItem->componentModel()->getVlnv()),
			filename);
	}

	// Check if the source is already open and activate it.
	for (int i = 0; i < designTabs_->count(); i++)
	{
		MCAPISourceWidget* sourceWidget = dynamic_cast<MCAPISourceWidget*>(designTabs_->widget(i));

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
			               "The source file " + filename + " is not found!",
			               QMessageBox::Ok, this);
		msgBox.exec();
		return;
	}

	// And open the source to a view.
	MCAPISourceWidget* sourceWidget = new MCAPISourceWidget(filename,
		progEntity->getContentMatcher(), this, this);

	connect(sourceWidget, SIGNAL(contentChanged()), this, SLOT(updateMenuStrip()));
	connect(sourceWidget->getEditProvider(), SIGNAL(editStateChanged()), this, SLOT(updateMenuStrip()));
	sourceWidget->setTabWidget(designTabs_);
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
	PropertyPageDialog dialog(QSize(48, 48), PropertyPageDialog::APPLY_CURRENT, this);
	dialog.setFixedWidth(620);
	dialog.resize(620, 680);
	dialog.setWindowTitle(tr("New"));

    // Add pages to the dialog.
	NewComponentPage* compPage = new NewComponentPage(libraryHandler_, &dialog);
	connect(compPage, SIGNAL(createComponent(KactusAttribute::ProductHierarchy,
		KactusAttribute::Firmness,
		VLNV const&, QString const&)),
		this, SLOT(createComponent(KactusAttribute::ProductHierarchy,
		KactusAttribute::Firmness,
		VLNV const&, QString const&)));
	dialog.addPage(QIcon(":icons/graphics/new-component.png"), tr("Component"), compPage);

	NewDesignPage* designPage = new NewDesignPage(libraryHandler_, &dialog);
	connect(designPage, SIGNAL(createDesign(KactusAttribute::ProductHierarchy,
		KactusAttribute::Firmness,
		VLNV const&, QString const&)),
		this, SLOT(createDesign(KactusAttribute::ProductHierarchy,
		KactusAttribute::Firmness,
		VLNV const&, QString const&)));
	dialog.addPage(QIcon(":icons/graphics/new-design.png"), tr("Design"), designPage);

	NewSWComponentPage* swCompPage = new NewSWComponentPage(libraryHandler_, &dialog);
	connect(swCompPage, SIGNAL(createSWComponent(SWCreateType, VLNV const&, QString const&)),
		this, SLOT(createSWComponent(SWCreateType, VLNV const&, QString const&)));
	dialog.addPage(QIcon(":icons/graphics/new-sw_component.png"), tr("SW Component"), swCompPage);

	NewSWDesignPage* swDesignPage = new NewSWDesignPage(libraryHandler_, &dialog);
	connect(swDesignPage, SIGNAL(createSWDesign(VLNV const&, QString const&)),
		this, SLOT(createSWDesign(VLNV const&, QString const&)), Qt::UniqueConnection);
	dialog.addPage(QIcon(":icons/graphics/new-sw_design.png"), tr("SW Design"), swDesignPage);

	NewSystemPage* sysPage = new NewSystemPage(libraryHandler_, &dialog);
	connect(sysPage, SIGNAL(createSystem(VLNV const&, QString const&, VLNV const&, QString const&)),
		this, SLOT(createSystem(VLNV const&, QString const&, VLNV const&, QString const&)));
	dialog.addPage(QIcon(":icons/graphics/new-system.png"), tr("System"), sysPage);

	NewBusPage* busPage = new NewBusPage(libraryHandler_, &dialog);
	connect(busPage, SIGNAL(createBus(VLNV const&, QString const&)),
		this, SLOT(createBus(VLNV const&, QString const&)), Qt::UniqueConnection);
	dialog.addPage(QIcon(":icons/graphics/new-bus.png"), tr("Bus"), busPage);

    NewComDefinitionPage* comDefPage = new NewComDefinitionPage(libraryHandler_, &dialog);
    connect(comDefPage, SIGNAL(createComDefinition(VLNV const&, QString const&)),
            this, SLOT(createComDefinition(VLNV const&, QString const&)), Qt::UniqueConnection);
    dialog.addPage(QIcon(":icons/graphics/new-com_definition.png"), tr("COM Definition"), comDefPage);

    NewApiDefinitionPage* apiDefPage = new NewApiDefinitionPage(libraryHandler_, &dialog);
    connect(apiDefPage, SIGNAL(createApiDefinition(VLNV const&, QString const&)),
        this, SLOT(createApiDefinition(VLNV const&, QString const&)), Qt::UniqueConnection);
    dialog.addPage(QIcon(":icons/graphics/new-api_definition.png"), tr("API Definition"), apiDefPage);

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
	libraryHandler_->writeModelToFile(directory, component);

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
	QSharedPointer<DesignConfiguration> designConf(new DesignConfiguration(desConfVLNV));
	designConf->setDesignRef(designVLNV);

	// Create the files.
	libraryHandler_->beginSave();
	libraryHandler_->writeModelToFile(directory, designConf);
	libraryHandler_->writeModelToFile(directory, design);
	libraryHandler_->writeModelToFile(directory, component);
	libraryHandler_->endSave();

	// Open the design.
	openDesign(vlnv, viewNames.first(), true);
}

//-----------------------------------------------------------------------------
// Function: createSWDesign()
//-----------------------------------------------------------------------------
void MainWindow::createSWDesign(VLNV const& vlnv, QString const& directory)
{
	Q_ASSERT(vlnv.isValid());

	VLNV designVLNV(VLNV::DESIGN, vlnv.getVendor(), vlnv.getLibrary(),
		vlnv.getName().remove(".comp") + ".design", vlnv.getVersion());

	// Create a component and a hierarchical view.
	QSharedPointer<Component> component(new Component(vlnv));
	component->setComponentImplementation(KactusAttribute::KTS_SW);
	component->setComponentSWType(KactusAttribute::KTS_SW_MAPPING);

	View* hierView = new View(tr("kts_sw_ref"));
	hierView->setHierarchyRef(designVLNV);
	hierView->addEnvIdentifier("");

	Model* model = new Model;
	model->addView(hierView);
	component->setModel(model);

	// Create the design.
	QSharedPointer<Design> design(new Design(designVLNV));

	// Create the files.
	libraryHandler_->writeModelToFile(directory, design);
	libraryHandler_->writeModelToFile(directory, component);

	// Open the design.
	openSWDesign(vlnv, true);
}

//-----------------------------------------------------------------------------
// Function: openSettings()
//-----------------------------------------------------------------------------
void MainWindow::openSettings()
{
	SettingsDialog dialog(this);

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
	}
}

//-----------------------------------------------------------------------------
// Function: createSystem()
//-----------------------------------------------------------------------------
void MainWindow::createSystem(VLNV const& compVLNV, QString const& viewName,
							  VLNV const& sysVLNV, QString const& directory)
{
	Q_ASSERT(compVLNV.isValid());
	Q_ASSERT(sysVLNV.isValid());

	libraryHandler_->beginSave();

	VLNV designVLNV(VLNV::DESIGN, sysVLNV.getVendor(), sysVLNV.getLibrary(),
		sysVLNV.getName().remove(".comp") + ".design", sysVLNV.getVersion());
	VLNV desConfVLNV(VLNV::DESIGNCONFIGURATION, sysVLNV.getVendor(), sysVLNV.getLibrary(),
		sysVLNV.getName().remove(".comp") + ".designcfg", sysVLNV.getVersion());

	// Retrieve the component to which the system design will be based on.
	QSharedPointer<Component> component = libraryHandler_->getModel(compVLNV).staticCast<Component>();
	Q_ASSERT(component != 0);

	// Create a system component to encapsulate the system design.
	QSharedPointer<Component> sysComp(new Component(sysVLNV));
	sysComp->setComponentHierarchy(KactusAttribute::KTS_PRODUCT);
	sysComp->setComponentFirmness(KactusAttribute::KTS_FIXED);
	sysComp->setComponentImplementation(KactusAttribute::KTS_SYS);

	// Create the view to the system design.
	View* hierView = new View("kts_sys_ref");
	hierView->setHierarchyRef(desConfVLNV);
	hierView->addEnvIdentifier("");

	// Create the view to the HW design.
	View* hwView = new View("kts_hw_ref");
	hwView->setHierarchyRef(component->getHierRef(viewName));
	hwView->addEnvIdentifier("");

	Model* model = new Model;
	model->addView(hierView);
	model->addView(hwView);

	sysComp->setModel(model);

	// Flat-out the hierarchy to form the system design.
	QSharedPointer<Design> sysDesign(new Design(designVLNV));
	generateSystemDesign(libraryHandler_, directory, component->getHierRef(viewName), *sysDesign);

	// Create the design configuration.
	QSharedPointer<DesignConfiguration> designConf(new DesignConfiguration(desConfVLNV));
	designConf->setDesignRef(designVLNV);

	// Create the files.
	libraryHandler_->writeModelToFile(directory, designConf);
	libraryHandler_->writeModelToFile(directory, sysDesign);
	libraryHandler_->writeModelToFile(directory, sysComp);

	libraryHandler_->endSave();
	openSystem(sysVLNV, true);
}

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
	libraryHandler_->writeModelToFile(directory, busDef);

	// create an abstraction definition
	QSharedPointer<AbstractionDefinition> absDef = QSharedPointer<AbstractionDefinition>(new AbstractionDefinition());
	absDef->setVlnv(absVLNV);

	// set reference from abstraction definition to bus definition
	absDef->setBusType(busVLNV);

	// create the file for the abstraction definition
	libraryHandler_->writeModelToFile(absDirectory, absDef);

	// Open the bus editor.
	openBus(busVLNV, absVLNV, false, true);
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
	libraryHandler_->writeModelToFile(absDirectory, absDef);

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
    libraryHandler_->writeModelToFile(directory, comDef);

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

    // Create an empty API definition and save it.
    QSharedPointer<ApiDefinition> apiDef(new ApiDefinition(vlnv));
    libraryHandler_->writeModelToFile(directory, apiDef);

    // Open the API definition.
    openApiDefinition(vlnv, true);
}

void MainWindow::openBus(const VLNV& busDefVLNV, const VLNV& absDefVLNV, bool disableBusDef, bool forceUnlocked ) {

	// Check if the bus editor is already open and activate it.
	if (busDefVLNV.isValid()) {
		for (int i = 0; i < designTabs_->count(); i++) {
			BusEditor* editor = dynamic_cast<BusEditor*>(designTabs_->widget(i));

			// if the abstraction definition matches
			if (editor && absDefVLNV.isValid() && editor->getComponentVLNV() == absDefVLNV) {
				designTabs_->setCurrentIndex(i);
				return;
			}

			// if bus definition matches
			if (editor && editor->getComponentVLNV() == busDefVLNV) {
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

	if (forceUnlocked)
	{
		editor->setProtection(false);
	}

	editor->setTabWidget(designTabs_);

	connect(editor, SIGNAL(errorMessage(const QString&)),
		console_, SLOT(onErrorMessage(const QString&)), Qt::UniqueConnection);
	connect(editor, SIGNAL(noticeMessage(const QString&)),
		console_, SLOT(onNoticeMessage(const QString&)), Qt::UniqueConnection);
	connect(editor, SIGNAL(contentChanged()),
		this, SLOT(updateMenuStrip()), Qt::UniqueConnection);
	connect(editor, SIGNAL(modifiedChanged(bool)),
		actSave_, SLOT(setEnabled(bool)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: openSystem()
//-----------------------------------------------------------------------------
void MainWindow::openSystem(VLNV const& vlnv, bool forceUnlocked)
{
	libraryHandler_->beginSave();

	// Check if the system is already open and activate it.
	if (vlnv.isValid()) {
		for (int i = 0; i < designTabs_->count(); i++) {
			EndpointDesignWidget *designWidget = dynamic_cast<EndpointDesignWidget*>(designTabs_->widget(i));

			if (designWidget != 0 && *designWidget->getOpenDocument() == vlnv) {
				designTabs_->setCurrentIndex(i);
				return;
			}
		}
	}

	EndpointDesignWidget* designWidget = new EndpointDesignWidget(libraryHandler_, this, this);

	if (!designWidget->setDesign(vlnv))
	{
		delete designWidget;
		return;
	}

	if (forceUnlocked)
	{
		designWidget->setProtection(false);
	}

	designWidget->setTabWidget(designTabs_);

	connect(designWidget->getEditProvider(), SIGNAL(editStateChanged()), this, SLOT(updateMenuStrip()));
	connect(designWidget, SIGNAL(contentChanged()), this, SLOT(updateMenuStrip()), Qt::UniqueConnection);
	connect(designWidget, SIGNAL(openComponent(const VLNV&)),
		this, SLOT(openComponent(const VLNV&)), Qt::UniqueConnection);
	connect(designWidget, SIGNAL(openDesign(const VLNV&, const QString&)),
		this, SLOT(openDesign(const VLNV&, const QString&)));
	connect(designWidget, SIGNAL(openSource(ProgramEntityItem*)),
		this, SLOT(openSource(ProgramEntityItem*)), Qt::UniqueConnection);

	connect(designWidget, SIGNAL(componentSelected(ComponentItem*)),
		this, SLOT(onComponentSelected(ComponentItem*)), Qt::UniqueConnection);
	connect(designWidget, SIGNAL(destroyed(QObject*)),
		this, SLOT(onClearItemSelection()), Qt::UniqueConnection);
	connect(designWidget, SIGNAL(clearItemSelection()),
		libraryHandler_, SLOT(onClearSelection()), Qt::UniqueConnection);
	connect(designWidget, SIGNAL(clearItemSelection()),
		this, SLOT(onClearItemSelection()), Qt::UniqueConnection);

	//connect(designWidget->getEditProvider(), SIGNAL(editStateChanged()), this, SLOT(updateMenuStrip()));
	connect(designWidget, SIGNAL(zoomChanged()), this, SLOT(updateZoomTools()), Qt::UniqueConnection);
	connect(designWidget, SIGNAL(modeChanged(DrawMode)),
		this, SLOT(onDrawModeChanged(DrawMode)), Qt::UniqueConnection);
	connect(designWidget, SIGNAL(modifiedChanged(bool)),
		actSave_, SLOT(setEnabled(bool)), Qt::UniqueConnection);

	libraryHandler_->endSave();
}

void MainWindow::openComponent( const VLNV& vlnv, bool forceUnlocked ) {

	// Check if the component editor is already open and activate it.
	if (vlnv.isValid()) {
		for (int i = 0; i < designTabs_->count(); i++) {
			IPXactComponentEditor* editor = dynamic_cast<IPXactComponentEditor*>(designTabs_->widget(i));

			if (editor && editor->getComponentVLNV() == vlnv) {
				designTabs_->setCurrentIndex(i);
				return;
			}
		}
	}

	// component editor was not yet open so create it

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
		return;
	}

	if (!component) {
		emit errorMessage(tr("Document type did not match Component"));
		return;
	}

	QString filePath = libraryHandler_->getPath(vlnv);
	QFileInfo info(filePath);

	IPXactComponentEditor* editor = new IPXactComponentEditor(libraryHandler_, info, component, this);
	QString styleSheet("*[mandatoryField=\"true\"] { background-color: LemonChiffon; }");
	editor->setStyleSheet(styleSheet);

	if (forceUnlocked)
	{
		editor->setProtection(false);
	}

	editor->setTabWidget(designTabs_);

	connect(editor, SIGNAL(errorMessage(const QString&)),
		console_, SLOT(onErrorMessage(const QString&)), Qt::UniqueConnection);
	connect(editor, SIGNAL(noticeMessage(const QString&)),
		console_, SLOT(onNoticeMessage(const QString&)), Qt::UniqueConnection);
	connect(editor, SIGNAL(contentChanged()),
		this, SLOT(updateMenuStrip()), Qt::UniqueConnection);
	connect(editor, SIGNAL(modifiedChanged(bool)),
		actSave_, SLOT(setEnabled(bool)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: MainWindow::openComDefinition()
//-----------------------------------------------------------------------------
void MainWindow::openComDefinition(VLNV const& vlnv, bool forceUnlocked /*= false*/)
{
    // Check if the COM definition is already open and activate it.
    if (vlnv.isValid())
    {
        for (int i = 0; i < designTabs_->count(); i++)
        {
            ComDefinitionEditor* editor = dynamic_cast<ComDefinitionEditor*>(designTabs_->widget(i));

            if (editor && editor->getComponentVLNV() == vlnv)
            {
                designTabs_->setCurrentIndex(i);
                return;
            }
        }
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

    if (forceUnlocked)
    {
        editor->setProtection(false);
    }

    editor->setTabWidget(designTabs_);

    connect(editor, SIGNAL(errorMessage(const QString&)),
            console_, SLOT(onErrorMessage(const QString&)), Qt::UniqueConnection);
    connect(editor, SIGNAL(noticeMessage(const QString&)),
            console_, SLOT(onNoticeMessage(const QString&)), Qt::UniqueConnection);
    connect(editor, SIGNAL(contentChanged()),
            this, SLOT(updateMenuStrip()), Qt::UniqueConnection);
    connect(editor, SIGNAL(modifiedChanged(bool)),
            actSave_, SLOT(setEnabled(bool)), Qt::UniqueConnection);
}


//-----------------------------------------------------------------------------
// Function: MainWindow::openApiDefinition()
//-----------------------------------------------------------------------------
void MainWindow::openApiDefinition(VLNV const& vlnv, bool forceUnlocked /*= false*/)
{
    // Check if the API definition is already open and activate it.
    if (vlnv.isValid())
    {
        for (int i = 0; i < designTabs_->count(); i++)
        {
            ApiDefinitionEditor* editor = dynamic_cast<ApiDefinitionEditor*>(designTabs_->widget(i));

            if (editor && editor->getComponentVLNV() == vlnv)
            {
                designTabs_->setCurrentIndex(i);
                return;
            }
        }
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

    ApiDefinitionEditor* editor = new ApiDefinitionEditor(this, this, libraryHandler_, apiDef);

    if (forceUnlocked)
    {
        editor->setProtection(false);
    }

    editor->setTabWidget(designTabs_);

    connect(editor, SIGNAL(errorMessage(const QString&)),
            console_, SLOT(onErrorMessage(const QString&)), Qt::UniqueConnection);
    connect(editor, SIGNAL(noticeMessage(const QString&)),
            console_, SLOT(onNoticeMessage(const QString&)), Qt::UniqueConnection);
    connect(editor, SIGNAL(contentChanged()),
            this, SLOT(updateMenuStrip()), Qt::UniqueConnection);
    connect(editor, SIGNAL(modifiedChanged(bool)),
            actSave_, SLOT(setEnabled(bool)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: changeProtection()
//-----------------------------------------------------------------------------
void MainWindow::changeProtection(bool locked)
{
	TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());
	Q_ASSERT(doc != 0);
	Q_ASSERT(doc->getComponentVLNV().isValid());

	if (doc == 0)
	{
		return;
	}

	if (locked)
	{
		if (doc->isModified())
		{
			QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
				tr("The document has been modified. The changes need to be saved before the "
				"document can be locked. Save changes and continue?"),
				QMessageBox::Yes | QMessageBox::No, this);

			if (msgBox.exec() == QMessageBox::No || !doc->save())
			{
				actProtect_->setChecked(false);
				return;
			}
		}
		// Lock/unlock the document.
		doc->setProtection(locked);
    }
    else
    {
        TabDocument* relatedDoc = NULL;

        // Check if there is a document with the same VLNV being edited (i.e. not unlocked).
        for (int i = 0; i < designTabs_->count(); i++)
        {
            TabDocument* otherDoc = static_cast<TabDocument*>(designTabs_->widget(i));
            Q_ASSERT(otherDoc != 0);

            if (otherDoc != doc && otherDoc->getComponentVLNV() == doc->getComponentVLNV())
            {
                if (!otherDoc->isProtected())
                {
                    // Check if the other document has unsaved changes.
                    if (otherDoc->isModified())
                    {
                        // Ask the user if he wants to save and switch locks.
                        QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
                                           tr("The document is being edited in another view and "
                                              "has unsaved changes. Changes need to be saved "
                                              "before this view can be unlocked. "
                                              "Save changes and switch locks?"),
                                           QMessageBox::Yes | QMessageBox::Cancel, this);

                        // Restore the lock if the user canceled.
                        if (msgBox.exec() == QMessageBox::Cancel)
                        {
                            actProtect_->setChecked(true);
                            return;
                        }

                        // Otherwise save the other document.
                        if (!otherDoc->save())
                        {
                            return;
                        }
                    }
                    
                    // Lock the other document.
                    otherDoc->setProtection(true);
                }

                relatedDoc = otherDoc;
                break;
            }
        }

        if (!doc->isPreviouslyUnlocked() && (relatedDoc == NULL || !relatedDoc->isPreviouslyUnlocked()))
        {
		    QString message = tr("Are you sure you want to unlock the document?\n");
            QString detailMsg = "";

		    // if edited document was component 
		    VLNV vlnv = doc->getComponentVLNV();
		    if (vlnv.isValid() && libraryHandler_->getDocumentType(vlnv) == VLNV::COMPONENT) {

			    // if component has been instantiated in other components then print
			    // a list of those components
			    QList<VLNV> list;
			    int refCount = libraryHandler_->getOwners(list, vlnv);
			    if (refCount > 0) {
				    detailMsg += tr("The component has been instantiated in the following %1 component(s):\n").arg(refCount);

				    foreach (VLNV owner, list) {
					    detailMsg += "* " + owner.getVendor() + ":" + owner.getLibrary() + ":" +
						             owner.getName() + ":" + owner.getVersion() + "\n"; 
				    }

                    message += tr("Changes to the document can affect %1 other documents.").arg(refCount);
			    }
                else
                {
                    message += tr("Changes to the document can affect other documents.");
                }
		    }
			else
			{
				message += tr("Changes to the document can affect other documents.");
			}

            message += " If you choose yes, this will not be asked next time for this document.";

            // Ask verification from the user.
            QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
                               message, QMessageBox::Yes | QMessageBox::No, this);
            msgBox.setDetailedText(detailMsg);

            if (msgBox.exec() == QMessageBox::No)
            {
                actProtect_->setChecked(true);
                return;
            }
        }

        // Mark the related document also unlocked to keep the unlock history in sync.
        if (relatedDoc != NULL)
        {
            relatedDoc->setPreviouslyUnlocked();
        }

		// Refresh and unlock the document.
		doc->refresh();
		doc->setProtection(locked);
	}

	// if the tab is designWidget
	DesignWidget* designwidget = dynamic_cast<DesignWidget*>(doc);
	if (designwidget && designwidget->getHierComponent()->getComponentImplementation() == KactusAttribute::KTS_HW)
		// update the editors to match the locked state
		configurationEditor_->setLocked(locked);
	else
		configurationEditor_->setLocked(true);

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
		actProtect_->setIcon(QIcon(":icons/graphics/protection-locked.png"));
		actProtect_->setText(tr("Locked"));
	}
	else
	{
		actProtect_->setIcon(QIcon(":icons/graphics/protection-unlocked.png"));
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
void MainWindow::createSWComponent(SWCreateType createType, VLNV const& vlnv,
								   QString const& directory)
{
	Q_ASSERT(vlnv.isValid());

	// Create a component.
	QSharedPointer<Component> component = QSharedPointer<Component>(new Component());
	component->setVlnv(vlnv);

	// Set Kactus attributes.
	component->setComponentImplementation(KactusAttribute::KTS_SW);

	if (createType == SW_CREATE_APPLICATION)
	{
		component->setComponentSWType(KactusAttribute::KTS_SW_APPLICATION);

		// Add the fixed bus interface to the component.
		QSharedPointer<BusInterface> busIf(new BusInterface());
		busIf->setName("app_link");
		busIf->setInterfaceMode(General::MASTER);
		busIf->setBusType(VLNV(VLNV::BUSDEFINITION, "Kactus", "internal", "app_link", "1.0"));

		component->addBusInterface(busIf);
	}
	else if (createType == SW_CREATE_ENDPOINTS)
	{
		component->setComponentSWType(KactusAttribute::KTS_SW_ENDPOINTS);

		// Add the fixed bus interface to the component.
		QSharedPointer<BusInterface> busIf(new BusInterface());
		busIf->setName("app_link");
		busIf->setInterfaceMode(General::SLAVE);
		busIf->setBusType(VLNV(VLNV::BUSDEFINITION, "Kactus", "internal", "app_link", "1.0"));

		component->addBusInterface(busIf);
	}
	else
	{
		component->setComponentSWType(KactusAttribute::KTS_SW_PLATFORM);
	}

	// Create a design for the component if it is a platform stack.
	if (createType == SW_CREATE_PLATFORM_STACK)
	{
		VLNV designVLNV(VLNV::DESIGN, vlnv.getVendor(), vlnv.getLibrary(),
			vlnv.getName().remove(".comp") + ".design", vlnv.getVersion());

		View* hierView = new View("kts_sw_ref");
		hierView->setHierarchyRef(designVLNV);
		hierView->addEnvIdentifier("");

		Model* model = new Model;
		model->addView(hierView);
		component->setModel(model);

		QSharedPointer<Design> design(new Design(designVLNV));
		libraryHandler_->writeModelToFile(directory, design);
	}

	// Create the file.
	libraryHandler_->writeModelToFile(directory, component);

	switch (createType)
	{
	case SW_CREATE_PLATFORM_COMPONENT:
	case SW_CREATE_APPLICATION:
	case SW_CREATE_ENDPOINTS:
		{
			openComponent(vlnv, true);
			break;
		}

	case SW_CREATE_PLATFORM_STACK:
		{
			openDesign(vlnv, "kts_sw_ref", true);
			break;
		}
	}
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

void MainWindow::selectVisibleDocks() {
	windowsMenu_.exec(QCursor::pos());
}

void MainWindow::onOutputAction( bool show ) {
	consoleDock_->setVisible(show);

	// if the output window is supported in the current window
	// or if there is no window open
	TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());
	if (!doc || (doc && doc->getSupportedWindows() & TabDocument::OUTPUTWINDOW)) {
		visibilities_.showOutput_ = show;
	}
}

void MainWindow::onPreviewAction( bool show ) {
	previewDock_->setVisible(show);

	// if the preview window is supported in the current window
	// or if there is no window open
	TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());
	if (!doc || (doc && doc->getSupportedWindows() & TabDocument::PREVIEWWINDOW)) {
		visibilities_.showPreview_ = show;
	}
}

void MainWindow::onLibraryAction( bool show ) {
	libraryDock_->setVisible(show);

	// if the library window is supported in the current window
	// or if there is no window open
	TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());
	if (!doc || (doc && doc->getSupportedWindows() & TabDocument::LIBRARYWINDOW)) {
		visibilities_.showLibrary_ = show;
	}
}

void MainWindow::onConfigurationAction( bool show ) {
	configurationDock_->setVisible(show);

	// if the configuration window is supported in the current window
	TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());
	if (doc && doc->getSupportedWindows() & TabDocument::CONFIGURATIONWINDOW) {
		visibilities_.showConfiguration_ = show;
	}
}

void MainWindow::onConnectionAction( bool show ) {
	connectionDock_->setVisible(show);

	// if the connection window is supported in the current window
	TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());

	if (doc && doc->getSupportedWindows() & TabDocument::CONNECTIONWINDOW) {
		visibilities_.showConnection_ = show;
	}
}

void MainWindow::onInterfaceAction( bool show ) {
	interfaceDock_->setVisible(show);

	// if the interface window is supported in the current window
	TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());

	if (doc && doc->getSupportedWindows() & TabDocument::INTERFACEWINDOW) {
		visibilities_.showInterface_ = show;
	}
}

void MainWindow::onAdHocAction( bool show ) {
    adHocDock_->setVisible(show);

    // if the instance window is supported in the current window
    TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());

    if (doc && doc->getSupportedWindows() & TabDocument::ADHOC_WINDOW) {
        visibilities_.showAdHocVisibility = show;
    }
}

void MainWindow::onInstanceAction( bool show ) {
	instanceDock_->setVisible(show);

	// if the instance window is supported in the current window
	TabDocument* doc = static_cast<TabDocument*>(designTabs_->currentWidget());

	if (doc && doc->getSupportedWindows() & TabDocument::INSTANCEWINDOW) {
		visibilities_.showInstance_ = show;
	}
}

void MainWindow::updateWindows( unsigned int supportedWindows ) {

	if (supportedWindows & TabDocument::LIBRARYWINDOW) {
		windowsMenu_.addAction(showLibraryAction_);
		libraryDock_->setVisible(visibilities_.showLibrary_);
	}
	else {
		windowsMenu_.removeAction(showLibraryAction_);
		libraryDock_->hide();
	}

	if (supportedWindows & TabDocument::OUTPUTWINDOW) {
		windowsMenu_.addAction(showOutputAction_);
		consoleDock_->setVisible(visibilities_.showOutput_);
	}
	else {
		windowsMenu_.removeAction(showOutputAction_);
		consoleDock_->hide();
	}

	if (supportedWindows & TabDocument::PREVIEWWINDOW) {
		windowsMenu_.addAction(showPreviewAction_);
		previewDock_->setVisible(visibilities_.showPreview_);
	}
	else {
		windowsMenu_.removeAction(showPreviewAction_);
		previewDock_->hide();
	}

	if (supportedWindows & TabDocument::CONFIGURATIONWINDOW) {
		windowsMenu_.addAction(showConfigurationAction_);
		configurationDock_->setVisible(visibilities_.showConfiguration_);
	}
	else {
		windowsMenu_.removeAction(showConfigurationAction_);
		configurationDock_->hide();
	}

	if (supportedWindows & TabDocument::CONNECTIONWINDOW) {
		windowsMenu_.addAction(showConnectionAction_);
		connectionDock_->setVisible(visibilities_.showConnection_);
	}
	else {
		windowsMenu_.removeAction(showConnectionAction_);
		connectionDock_->hide();
	}

	if (supportedWindows & TabDocument::INTERFACEWINDOW) {
		windowsMenu_.addAction(showInterfaceAction_);
		interfaceDock_->setVisible(visibilities_.showInterface_);
	}
	else {
		windowsMenu_.removeAction(showInterfaceAction_);
		interfaceDock_->hide();
	}

	if (supportedWindows & TabDocument::INSTANCEWINDOW) {
		windowsMenu_.addAction(showInstanceAction_);
		instanceDock_->setVisible(visibilities_.showInstance_);
	}
	else {
		windowsMenu_.removeAction(showInstanceAction_);
		instanceDock_->hide();
	}

    if (supportedWindows & TabDocument::ADHOC_WINDOW) {
        windowsMenu_.addAction(showAdHocAction_);
        adHocDock_->setVisible(visibilities_.showAdHocVisibility);
    }
    else {
        windowsMenu_.removeAction(showAdHocAction_);
        adHocDock_->hide();
    }
}

void MainWindow::hideEvent( QHideEvent* event ) {
	// Action to show/hide the output window.
	disconnect(showOutputAction_, SIGNAL(toggled(bool)),
		this, SLOT(onOutputAction(bool)));
	disconnect(consoleDock_->toggleViewAction(), SIGNAL(toggled(bool)),
		showOutputAction_, SLOT(setChecked(bool)));

	// Action to show/hide the preview box.
	disconnect(showPreviewAction_, SIGNAL(toggled(bool)),
		this, SLOT(onPreviewAction(bool)));
	disconnect(previewDock_->toggleViewAction(), SIGNAL(toggled(bool)),
		showPreviewAction_, SLOT(setChecked(bool)));

	// Action to show/hide the library window.
	disconnect(showLibraryAction_, SIGNAL(toggled(bool)),
		this, SLOT(onLibraryAction(bool)));
	disconnect(libraryDock_->toggleViewAction(), SIGNAL(toggled(bool)),
		showLibraryAction_, SLOT(setChecked(bool))); 		

	// Action to show/hide the configuration window.
	disconnect(showConfigurationAction_, SIGNAL(toggled(bool)),
		this, SLOT(onConfigurationAction(bool)));
	disconnect(configurationDock_->toggleViewAction(), SIGNAL(toggled(bool)),
		showConfigurationAction_, SLOT(setChecked(bool)));

	// Action to show/hide the connection editor.
	disconnect(showConnectionAction_, SIGNAL(toggled(bool)),
		this, SLOT(onConnectionAction(bool)));
	disconnect(connectionDock_->toggleViewAction(), SIGNAL(toggled(bool)),
		showConnectionAction_, SLOT(setChecked(bool)));

	// Action to show/hide the interface editor.
	disconnect(showInterfaceAction_, SIGNAL(toggled(bool)),
		this, SLOT(onInterfaceAction(bool)));
	disconnect(interfaceDock_->toggleViewAction(), SIGNAL(toggled(bool)),
		showInterfaceAction_, SLOT(setChecked(bool)));

	// Action to show/hide the instance editor.
	disconnect(showInstanceAction_, SIGNAL(toggled(bool)),
		       this, SLOT(onInstanceAction(bool)));
	disconnect(instanceDock_->toggleViewAction(), SIGNAL(toggled(bool)),
		       showInstanceAction_, SLOT(setChecked(bool)));

    // Action to show/hide the instance editor.
    disconnect(showAdHocAction_, SIGNAL(toggled(bool)), this, SLOT(onAdHocAction(bool)));
    disconnect(adHocDock_->toggleViewAction(), SIGNAL(toggled(bool)), showAdHocAction_, SLOT(setChecked(bool)));

	QMainWindow::hideEvent(event);
}

void MainWindow::showEvent( QShowEvent* event ) {

	// Action to show/hide the output window.
	connect(showOutputAction_, SIGNAL(toggled(bool)),
		this, SLOT(onOutputAction(bool)), Qt::UniqueConnection);
	connect(consoleDock_->toggleViewAction(), SIGNAL(toggled(bool)),
		showOutputAction_, SLOT(setChecked(bool)), Qt::UniqueConnection);

	// Action to show/hide the preview box.
	connect(showPreviewAction_, SIGNAL(toggled(bool)),
		this, SLOT(onPreviewAction(bool)), Qt::UniqueConnection);
	connect(previewDock_->toggleViewAction(), SIGNAL(toggled(bool)),
		showPreviewAction_, SLOT(setChecked(bool)), Qt::UniqueConnection);

	// Action to show/hide the library window.
	connect(showLibraryAction_, SIGNAL(toggled(bool)),
		this, SLOT(onLibraryAction(bool)), Qt::UniqueConnection);
	connect(libraryDock_->toggleViewAction(), SIGNAL(toggled(bool)),
		showLibraryAction_, SLOT(setChecked(bool)), Qt::UniqueConnection); 		

	// Action to show/hide the configuration window.
	connect(showConfigurationAction_, SIGNAL(toggled(bool)),
		this, SLOT(onConfigurationAction(bool)), Qt::UniqueConnection);
	connect(configurationDock_->toggleViewAction(), SIGNAL(toggled(bool)),
		showConfigurationAction_, SLOT(setChecked(bool)), Qt::UniqueConnection);

	// Action to show/hide the connection editor.
	connect(showConnectionAction_, SIGNAL(toggled(bool)),
		this, SLOT(onConnectionAction(bool)), Qt::UniqueConnection);
	connect(connectionDock_->toggleViewAction(), SIGNAL(toggled(bool)),
		showConnectionAction_, SLOT(setChecked(bool)), Qt::UniqueConnection);

	// Action to show/hide the interface editor.
	connect(showInterfaceAction_, SIGNAL(toggled(bool)),
		this, SLOT(onInterfaceAction(bool)), Qt::UniqueConnection);
	connect(interfaceDock_->toggleViewAction(), SIGNAL(toggled(bool)),
		showInterfaceAction_, SLOT(setChecked(bool)), Qt::UniqueConnection);

	// Action to show/hide the instance editor.
	connect(showInstanceAction_, SIGNAL(toggled(bool)),
		this, SLOT(onInstanceAction(bool)), Qt::UniqueConnection);
	connect(instanceDock_->toggleViewAction(), SIGNAL(toggled(bool)),
		showInstanceAction_, SLOT(setChecked(bool)), Qt::UniqueConnection);

    // Action to show/hide the ad-hoc visibility editor.
    connect(showAdHocAction_, SIGNAL(toggled(bool)), this, SLOT(onAdHocAction(bool)), Qt::UniqueConnection);
    connect(adHocDock_->toggleViewAction(), SIGNAL(toggled(bool)),
            showAdHocAction_, SLOT(setChecked(bool)), Qt::UniqueConnection);

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
// Function: onWorkspaceChanged()
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

MainWindow::WindowVisibility::WindowVisibility():
showOutput_(true),
showPreview_(true),
showLibrary_(true),
showConfiguration_(true),
showConnection_(true),
showInterface_(true),
showInstance_(true) {
}
