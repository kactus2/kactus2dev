//-----------------------------------------------------------------------------
// File: DockWidgetHandler.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 10.10.2017
//
// Description:
// Manages the dockable widgets for the main window.
//-----------------------------------------------------------------------------

#include "DockWidgetHandler.h"

#include <mainwindow/MessageConsole/messageconsole.h>

#include <Help/HelpSystem/HelpWindow.h>
#include <Help/HelpSystem/ContextHelpBrowser.h>

#include <library/LibraryWidget.h>
#include <KactusAPI/include/LibraryHandler.h>

#include <common/ui/GraphicalMessageMediator.h>

#include <common/widgets/ParameterGroupBox/parametergroupbox.h>
#include <common/graphicsItems/ConnectionEndpoint.h>

#include <editors/ComponentEditor/componenteditor.h>
#include <KactusAPI/include/ListParameterFinder.h>
#include <KactusAPI/include/ExpressionFormatter.h>
#include <KactusAPI/include/MultipleParameterFinder.h>
#include <editors/ComponentEditor/parameterReferenceTree/ParameterReferenceTreeWindow.h>
#include <KactusAPI/include/ParametersInterface.h>

#include <editors/common/DesignParameterReferenceTree/DesignParameterReferenceCounter.h>
#include <editors/common/DesignParameterReferenceTree/DesignParameterReferenceTree.h>
#include <editors/common/ComponentInstanceEditor/componentinstanceeditor.h>
#include <editors/common/ConfigurationEditor/configurationeditor.h>
#include <editors/common/InterfaceEditor/interfaceeditor.h>
#include <editors/common/ConnectionEditor/connectioneditor.h>
#include <editors/common/VendorExtensionEditor/VendorExtensionsEditor.h>

#include <editors/HWDesign/AdHocItem.h>
#include <editors/HWDesign/HWDesignWidget.h>
#include <editors/HWDesign/HWDesignDiagram.h>
#include <editors/HWDesign/AdhocEditor/AdhocEditor.h>
#include <editors/SystemDesign/SystemDesignWidget.h>
#include <editors/SystemDesign/SystemDetailsEditor/SystemDetailsEditor.h>

#include <QActionGroup>
#include <QList>
#include <QString>
#include <QHelpEngine>
#include <QApplication>

namespace 
{
    const QMap<TabDocument::SupportedWindows, QString> DOCK_SETTINGS =
    {
        { TabDocument::CONFIGURATIONWINDOW, "ConfigurationVisibility" },
        { TabDocument::SYSTEM_DETAILS_WINDOW, "SystemDetailsVisibility" },
        { TabDocument::CONNECTIONWINDOW, "ConnectionVisibility" },
        { TabDocument::INSTANCEWINDOW, "InstanceVisibility" },
        { TabDocument::ADHOC_WINDOW, "AdHocEditorVisibility" },
        { TabDocument::INTERFACEWINDOW, "InterfaceVisibility" },
        { TabDocument::LIBRARYWINDOW, "LibraryVisibility" },
        { TabDocument::OUTPUTWINDOW, "OutputVisibility" },
        { TabDocument::CONTEXT_HELP_WINDOW, "ContextHelpVisibility" },
        { TabDocument::DESIGNPARAMETERSWINDOW, "DesignParameterVisibility" },
        { TabDocument::VENDOREXTENSIONWINDOW, "VendorExtensionVisibility" }
    };
};

//-----------------------------------------------------------------------------
// Function: DockWidgetHandler::DockWidgetHandler()
//-----------------------------------------------------------------------------
DockWidgetHandler::DockWidgetHandler(
    LibraryHandler* library,
    MessageMediator* messageChannel, 
    QToolBar* leftToolbar,
    QToolBar* rightToolbar,
    QMainWindow* parent) :
libraryHandler_(library),
    libraryDock_(0),
    libraryWidget_(0),
    console_(0),
    consoleDock_(0),
    contextHelpBrowser_(0),
    contextHelpDock_(0),
    designParameterReferenceCounter_(0),
    designParametersEditor_(0),
    designParameterDock_(0),
    designParameterTree_(0),
    designParameterFinder_(new ListParameterFinder()),
    designAndInstancesParameterFinder_(new MultipleParameterFinder()),
    instanceEditor_(0),
    instanceDock_(0),
    adhocEditor_(0),
    adhocDock_(0),
    configurationEditor_(0),
    configurationDock_(0),
    systemDetailsEditor_(0),
    systemDetailsDock_(0),
    interfaceEditor_(0),
    interfaceDock_(0),
    connectionEditor_(0),
    connectionDock_(0),
    extensionDock_(0),
    extensionEditor_(0),
    helpWnd_(0),
    docks_(),
    visibilityControls_(),
    visibilities_(),
    leftToolbar_(leftToolbar),
    rightToolbar_(rightToolbar),
    leftActions_(new QActionGroup(this)),
    rightActions_(new QActionGroup(this)),
    mainWindow_(parent),
    messageChannel_(messageChannel)
{
     leftActions_->setExclusionPolicy(QActionGroup::ExclusionPolicy::ExclusiveOptional);
     rightActions_->setExclusionPolicy(QActionGroup::ExclusionPolicy::ExclusiveOptional);
}

//-----------------------------------------------------------------------------
// Function: DockWidgetHandler::setupDockWidgets()
//-----------------------------------------------------------------------------
void DockWidgetHandler::setupDockWidgets()
{
    setupMessageConsole();
    setupContextHelp();
    setupLibraryDock();
    setupDesignParametersEditor();
    setupInstanceEditor();
    setupAdHocEditor();
    setupConfigurationEditor();
    setupSystemDetailsEditor();
    setupInterfaceEditor();
    setupConnectionEditor();
    setupVendorExtensionEditor();

    docks_ =
    {
        { TabDocument::CONFIGURATIONWINDOW, configurationDock_ },
        { TabDocument::SYSTEM_DETAILS_WINDOW, systemDetailsDock_ },
        { TabDocument::CONNECTIONWINDOW, connectionDock_ },
        { TabDocument::INSTANCEWINDOW, instanceDock_ },
        { TabDocument::ADHOC_WINDOW, adhocDock_ },
        { TabDocument::INTERFACEWINDOW, interfaceDock_ },
        { TabDocument::LIBRARYWINDOW, libraryDock_ },
        { TabDocument::OUTPUTWINDOW, consoleDock_ },
        { TabDocument::CONTEXT_HELP_WINDOW, contextHelpDock_ },
        { TabDocument::DESIGNPARAMETERSWINDOW, designParameterDock_ },
        { TabDocument::VENDOREXTENSIONWINDOW, extensionDock_ }
    };

    for (auto const& dock : docks_)
    {
        auto viewAction = dock->toggleViewAction();
        viewAction->setIcon(dock->windowIcon());
        viewAction->setCheckable(true);

        connect(viewAction, SIGNAL(toggled(bool)), dock, SLOT(setVisible(bool)), Qt::UniqueConnection);
        connect(dock, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)),
            this, SLOT(onDockLocationChanged(Qt::DockWidgetArea)), Qt::UniqueConnection);

        auto visibilityAction = new QAction(this);
        visibilityAction->setText(dock->windowTitle());
        visibilityAction->setCheckable(true);

        visibilityControls_.insert(docks_.key(dock), visibilityAction);
    }
}

//-----------------------------------------------------------------------------
// Function: DockWidgetHandler::addVisibilityActions()
//-----------------------------------------------------------------------------
void DockWidgetHandler::setupVisibilityActionMenu(QMenu& visibilityMenu)
{
    for (auto const& control : visibilityControls_)
    {
        visibilityMenu.addAction(control);
    }
}

//-----------------------------------------------------------------------------
// Function: DockWidgetHandler::getLibraryHandler()
//-----------------------------------------------------------------------------
LibraryHandler* DockWidgetHandler::getLibraryHandler() const
{
    return libraryHandler_;
}

//-----------------------------------------------------------------------------
// Function: DockWidgetHandler::loadVisiblities()
//-----------------------------------------------------------------------------
void DockWidgetHandler::loadVisiblities(QSettings const& settings)
{
    for (auto i = DOCK_SETTINGS.cbegin(); i != DOCK_SETTINGS.cend(); ++i)
    {
        const bool VISIBLE = settings.value(i.value(), true).toBool();
        const auto TYPE = i.key();

        visibilities_.insert(TYPE, VISIBLE);
        visibilityControls_[TYPE]->setChecked(VISIBLE);
    }
}

//-----------------------------------------------------------------------------
// Function: DockWidgetHandler::saveVisibilitySettings()
//-----------------------------------------------------------------------------
void DockWidgetHandler::saveVisibilitySettings(QSettings& settings) const
{
    for (auto i = DOCK_SETTINGS.cbegin(); i != DOCK_SETTINGS.cend(); ++i)
    {
        settings.setValue(i.value(), visibilities_.value(i.key()));
    }
}

//-----------------------------------------------------------------------------
// Function: DockWidgetHandler::loadFilterSettings()
//-----------------------------------------------------------------------------
void DockWidgetHandler::loadFilterSettings(QSettings& settings)
{
    libraryWidget_->loadFilterSettings(settings);
}

//-----------------------------------------------------------------------------
// Function: DockWidgetHandler::saveFilterSettings()
//-----------------------------------------------------------------------------
void DockWidgetHandler::saveFilterSettings(QSettings& settings) const
{
    libraryWidget_->saveFilterSettings(settings);
}

//-----------------------------------------------------------------------------
// Function: DockWidgetHandler::applySettings()
//-----------------------------------------------------------------------------
void DockWidgetHandler::applySettings()
{
    // Nothing to do.
}

//-----------------------------------------------------------------------------
// Function: DockWidgetHandler::setupMessageConsole()
//-----------------------------------------------------------------------------
void DockWidgetHandler::setupMessageConsole()
{
    consoleDock_ = new QDockWidget(tr("Output"), mainWindow_);
    consoleDock_->setObjectName(tr("Output"));
    consoleDock_->setWindowIcon(QIcon(":icons/common/graphics/log.png"));
    consoleDock_->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
    consoleDock_->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | 
        QDockWidget::DockWidgetFloatable);

    console_ = new MessageConsole(consoleDock_);
    consoleDock_->setWidget(console_);
    mainWindow_->addDockWidget(Qt::BottomDockWidgetArea, consoleDock_);

    auto guiChannel = dynamic_cast<GraphicalMessageMediator*>(messageChannel_);
    if (guiChannel)
    {
        connect(guiChannel, SIGNAL(noticeMessage(QString const&)),
            console_, SLOT(onNoticeMessage(QString const&)), Qt::UniqueConnection);
        connect(guiChannel, SIGNAL(errorMessage(QString const&)),
            console_, SLOT(onErrorMessage(QString const&)), Qt::UniqueConnection);
    }

    connect(this, SIGNAL(errorMessage(const QString&)),
        console_, SLOT(onErrorMessage(const QString&)), Qt::UniqueConnection);
    connect(this, SIGNAL(noticeMessage(const QString&)),
        console_, SLOT(onNoticeMessage(const QString&)), Qt::UniqueConnection);

    // Force console visible on new messages.
    connect(this, SIGNAL(errorMessage(const QString&)), consoleDock_, SLOT(show()), Qt::UniqueConnection);    
}

//-----------------------------------------------------------------------------
// Function: DockWidgetHandler::setupContextHelp()
//-----------------------------------------------------------------------------
void DockWidgetHandler::setupContextHelp()
{
    // Create the dock widget for the context help.
    contextHelpDock_ = new QDockWidget(tr("Context Help"), mainWindow_);
    contextHelpDock_->setObjectName(tr("Context Help"));
    contextHelpDock_->setWindowIcon(QIcon(":icons/common/graphics/system-help.png"));
    contextHelpDock_->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
    contextHelpDock_->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable |
        QDockWidget::DockWidgetFloatable);

    // Initialize the help engine.
    QSettings settings;
    QString helpPath = settings.value("Platform/HelpPath", "Help").toString();

    QDir helpDirectory(helpPath);
    if (helpDirectory.isRelative())
    {
        helpPath.prepend(QLatin1Char('/'));
        helpPath.prepend(QCoreApplication::applicationDirPath());        
    }  

    QHelpEngine* helpEngine = new QHelpEngine(helpPath + "/Kactus2Help.qhc", mainWindow_);
    helpEngine->setupData();

    // Create the help window.
    helpWnd_ = new HelpWindow(helpEngine, mainWindow_);

    // Create the context help browser.
    contextHelpBrowser_ = new ContextHelpBrowser(helpEngine, "qthelp://com.tut.kactus2.2.0/doc",
        contextHelpDock_);

    QColor bgColor = QApplication::palette().color(QPalette::Window);
    QString style = QString("QTextBrowser { border: 5px solid transparent; "
        "background-color: #%1}").arg(bgColor.rgb() & 0x00FFFFFF, 0, 16);
    contextHelpBrowser_->setStyleSheet(style);

    contextHelpDock_->setWidget(contextHelpBrowser_);
    mainWindow_->addDockWidget(Qt::RightDockWidgetArea, contextHelpDock_);

    connect(this, SIGNAL(helpUrlRequested(QString const&)),
        contextHelpBrowser_, SLOT(onHelpRequested(QString const&)), Qt::UniqueConnection);
    contextHelpBrowser_->onHelpRequested("welcome.html");
}

//-----------------------------------------------------------------------------
// Function: DockWidgetHandler::setupLibraryDock()
//-----------------------------------------------------------------------------
void DockWidgetHandler::setupLibraryDock()
{
    // set up the dock widget for the library
    libraryDock_ = new QDockWidget(tr("IP-XACT Library"), mainWindow_);
    libraryDock_->setObjectName(tr("Library"));
    libraryDock_->setWindowIcon(QIcon(":icons/common/graphics/library.png"));
    libraryDock_->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    libraryDock_->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable |
        QDockWidget::DockWidgetFloatable);

    // create a container widget for dialer and library display
    libraryWidget_ = new LibraryWidget(libraryHandler_, messageChannel_, mainWindow_);

    libraryDock_->setWidget(libraryWidget_);
    mainWindow_->addDockWidget(Qt::LeftDockWidgetArea, libraryDock_);

    connect(this, SIGNAL(generateIntegrityReport()), libraryWidget_,
        SLOT(onGenerateIntegrityReport()), Qt::UniqueConnection);


    connect(libraryWidget_, SIGNAL(createBus(const VLNV&, const QString&)),
        mainWindow_, SLOT(createBus(const VLNV&, const QString&)), Qt::UniqueConnection);
    connect(libraryWidget_, SIGNAL(createComponent(KactusAttribute::ProductHierarchy, KactusAttribute::Firmness,
        QVector<TagData>, const VLNV&, const QString&)),
        mainWindow_, SLOT(createComponent(KactusAttribute::ProductHierarchy, KactusAttribute::Firmness, QVector<TagData>,
            const VLNV&, const QString&)), Qt::UniqueConnection);
    
    connect(libraryWidget_, SIGNAL(createAbsDef(const VLNV&, const QString&, bool)),
        mainWindow_, SLOT(createAbsDef(const VLNV&, const QString&, bool)), Qt::UniqueConnection);

    connect(libraryWidget_, SIGNAL(createDesignForExistingComponent(const VLNV&)),
        mainWindow_, SLOT(createDesignForExistingComponent(const VLNV&)), Qt::UniqueConnection);


    connect(libraryWidget_, SIGNAL(createComDef(const VLNV&, const QString&)),
        mainWindow_, SLOT(createComDefinition(const VLNV&, const QString&)), Qt::UniqueConnection);
    connect(libraryWidget_, SIGNAL(createApiDef(const VLNV&, const QString&)),
        mainWindow_, SLOT(createApiDefinition(const VLNV&, const QString&)), Qt::UniqueConnection);

    connect(libraryWidget_, SIGNAL(createSWDesign(const VLNV&)),
        mainWindow_, SLOT(createSWDesign(const VLNV&)), Qt::UniqueConnection);
    connect(libraryWidget_, SIGNAL(createSystemDesign(const VLNV&)),
        mainWindow_, SLOT(createSystemDesign(const VLNV&)), Qt::UniqueConnection);


    connect(libraryWidget_, SIGNAL(statusMessage(QString const&)),
        this, SIGNAL(statusMessage(QString const&)));
}

//-----------------------------------------------------------------------------
// Function: DockWidgetHandler::setupDesignParametersEditor()
//-----------------------------------------------------------------------------
void DockWidgetHandler::setupDesignParametersEditor()
{
    designParameterDock_ = new QDockWidget(tr("Design Parameters"), mainWindow_);
    designParameterDock_->setObjectName(tr("Design Parameters dock"));
    designParameterDock_->setAllowedAreas(Qt::BottomDockWidgetArea);
    designParameterDock_->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable |
        QDockWidget::DockWidgetFloatable);

    QSharedPointer<ExpressionFormatter> formatter(new ExpressionFormatter(designParameterFinder_));
    QSharedPointer<ExpressionFormatter> referenceTreeFormatter(
        new ExpressionFormatter(designAndInstancesParameterFinder_));
    designAndInstancesParameterFinder_->addFinder(designParameterFinder_);

    designParameterReferenceCounter_ = QSharedPointer<DesignParameterReferenceCounter>(
        new DesignParameterReferenceCounter(designParameterFinder_));

    designParameterTree_ =
        new DesignParameterReferenceTree(referenceTreeFormatter, designParameterReferenceCounter_, mainWindow_);

    designParametersEditor_ = new ParameterGroupBox(QSharedPointer<QList<QSharedPointer<Parameter> > >(),
        QSharedPointer<QList<QSharedPointer<Choice> > >(), designParameterFinder_, formatter, mainWindow_);

    ParameterReferenceTreeWindow* designParameterReferenceWindow =
        new ParameterReferenceTreeWindow(designParameterTree_, designParametersEditor_);

    designParameterDock_->setWidget(designParametersEditor_);
    mainWindow_->addDockWidget(Qt::BottomDockWidgetArea, designParameterDock_);

    connect(designParametersEditor_, SIGNAL(contentChanged()),
        this, SIGNAL(designChanged()), Qt::UniqueConnection);

    connect(designParametersEditor_, SIGNAL(increaseReferences(QString)), 
        designParameterReferenceCounter_.data(), SLOT(increaseReferenceCount(QString)), Qt::UniqueConnection);
    connect(designParametersEditor_, SIGNAL(decreaseReferences(QString)),
        designParameterReferenceCounter_.data(), SLOT(decreaseReferenceCount(QString)), Qt::UniqueConnection);
    connect(designParametersEditor_, SIGNAL(openReferenceTree(QString const&, QString const&)),
        designParameterReferenceWindow, SLOT(openReferenceTree(QString const&, QString const&)),
        Qt::UniqueConnection);

    connect(designParametersEditor_,
        SIGNAL(recalculateReferencesToParameters(QVector<QString> const&, AbstractParameterInterface*)),
        designParameterReferenceCounter_.data(),
        SLOT(recalculateReferencesToParameters(QVector<QString> const&, AbstractParameterInterface*)),
        Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: DockWidgetHandler::setupInstanceEditor()
//-----------------------------------------------------------------------------
void DockWidgetHandler::setupInstanceEditor()
{
    instanceDock_ = new QDockWidget(tr("Component Instance Details"), mainWindow_);
    instanceDock_->setObjectName(tr("Instance Editor"));
    instanceDock_->setWindowIcon(QIcon(":icons/common/graphics/hw-component-edit.png"));
    instanceDock_->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    instanceDock_->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable |
        QDockWidget::DockWidgetFloatable);

    instanceEditor_ = new ComponentInstanceEditor(instanceDock_);
    instanceDock_->setWidget(instanceEditor_);
    mainWindow_->addDockWidget(Qt::RightDockWidgetArea, instanceDock_);

    connect(instanceEditor_, SIGNAL(contentChanged()), this, SIGNAL(designChanged()), Qt::UniqueConnection);
    connect(instanceEditor_, SIGNAL(increaseReferences(QString)),
        designParameterReferenceCounter_.data(), SLOT(increaseReferenceCount(QString)), Qt::UniqueConnection);
    connect(instanceEditor_, SIGNAL(decreaseReferences(QString)),
        designParameterReferenceCounter_.data(), SLOT(decreaseReferenceCount(QString)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: DockWidgetHandler::setupAdHocEditor()
//-----------------------------------------------------------------------------
void DockWidgetHandler::setupAdHocEditor()
{
    adhocDock_ = new QDockWidget(tr("Ad-hoc Port Editor"), mainWindow_);
    adhocDock_->setObjectName(tr("Ad-hoc Port Editor"));
    adhocDock_->setWindowIcon(QIcon(":icons/common/graphics/adhoc.png"));
    adhocDock_->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    adhocDock_->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable |
        QDockWidget::DockWidgetFloatable);

    adhocEditor_ = new AdhocEditor(designParameterFinder_, adhocDock_);
    adhocDock_->setWidget(adhocEditor_);
    mainWindow_->addDockWidget(Qt::RightDockWidgetArea, adhocDock_);

    connect(adhocEditor_, SIGNAL(increaseReferences(QString const&)), 
        designParameterReferenceCounter_.data(), SLOT(increaseReferenceCount(QString const&)),
        Qt::UniqueConnection);
    connect(adhocEditor_, SIGNAL(decreaseReferences(QString const&)),
        designParameterReferenceCounter_.data(), SLOT(decreaseReferenceCount(QString const&)),
        Qt::UniqueConnection);

    connect(adhocEditor_, SIGNAL(contentChanged()), this, SIGNAL(designChanged()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: DockWidgetHandler::setupConfigurationEditor()
//-----------------------------------------------------------------------------
void DockWidgetHandler::setupConfigurationEditor()
{
    configurationDock_ = new QDockWidget(tr("Design Configuration Details"), mainWindow_);
    configurationDock_->setObjectName(tr("Configuration editor"));
    configurationDock_->setWindowIcon(QIcon(":icons/common/graphics/settings-general.png"));
    configurationDock_->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    configurationDock_->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable |
        QDockWidget::DockWidgetFloatable);

    configurationEditor_ = new ConfigurationEditor(libraryHandler_, configurationDock_);
    configurationDock_->setWidget(configurationEditor_);
    mainWindow_->addDockWidget(Qt::RightDockWidgetArea, configurationDock_);

    connect(configurationEditor_, SIGNAL(contentChanged()), this, SIGNAL(designChanged()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: DockWidgetHandler::setupSystemDetailsEditor()
//-----------------------------------------------------------------------------
void DockWidgetHandler::setupSystemDetailsEditor()
{
    systemDetailsDock_ = new QDockWidget(tr("HW Mapping Details"), mainWindow_);
    systemDetailsDock_->setObjectName(tr("HW Mapping Details Editor"));
    systemDetailsDock_->setWindowIcon(QIcon(":icons/common/graphics/mapping.png"));
    systemDetailsDock_->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    systemDetailsDock_->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable |
        QDockWidget::DockWidgetFloatable);

    systemDetailsEditor_ = new SystemDetailsEditor(libraryHandler_, systemDetailsDock_);
    systemDetailsDock_->setWidget(systemDetailsEditor_);
    mainWindow_->addDockWidget(Qt::RightDockWidgetArea, systemDetailsDock_);

    connect(systemDetailsEditor_, SIGNAL(contentChanged()), this, SIGNAL(designChanged()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: DockWidgetHandler::setupInterfaceEditor()
//-----------------------------------------------------------------------------
void DockWidgetHandler::setupInterfaceEditor()
{
    interfaceDock_ = new QDockWidget(tr("Interface Editor"), mainWindow_);
    interfaceDock_->setObjectName(tr("Interface Editor"));
    interfaceDock_->setWindowIcon(QIcon(":icons/common/graphics/tool-interface.png"));
    interfaceDock_->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    interfaceDock_->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable |
        QDockWidget::DockWidgetFloatable);

    interfaceEditor_ = new InterfaceEditor(interfaceDock_, libraryHandler_);
    interfaceDock_->setWidget(interfaceEditor_);
    mainWindow_->addDockWidget(Qt::RightDockWidgetArea, interfaceDock_);

    connect(interfaceEditor_, SIGNAL(contentChanged()), this, SIGNAL(designChanged()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: DockWidgetHandler::setupConnectionEditor()
//-----------------------------------------------------------------------------
void DockWidgetHandler::setupConnectionEditor()
{
    connectionDock_ = new QDockWidget(tr("Connection Editor"), mainWindow_);
    connectionDock_->setObjectName(tr("Connection Editor"));
    connectionDock_->setWindowIcon(QIcon(":icons/common/graphics/tool-interconnection.png"));
    connectionDock_->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    connectionDock_->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable |
        QDockWidget::DockWidgetFloatable);

    connectionEditor_ = new ConnectionEditor(libraryHandler_, connectionDock_);
    connectionDock_->setWidget(connectionEditor_);
    mainWindow_->addDockWidget(Qt::RightDockWidgetArea, connectionDock_);
}

//-----------------------------------------------------------------------------
// Function: DockWidgetHandler::setupVendorExtensionEditor()
//-----------------------------------------------------------------------------
void DockWidgetHandler::setupVendorExtensionEditor()
{
    extensionDock_ = new QDockWidget(tr("Vendor Extensions (experimental)"), mainWindow_);
    extensionDock_->setObjectName(tr("Vendor Extension Editor"));
    extensionDock_->setWindowIcon(QIcon(":icons/common/graphics/puzzle.png"));
    extensionDock_->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
    extensionDock_->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable |
        QDockWidget::DockWidgetFloatable);

    extensionEditor_ = new VendorExtensionsEditor(connectionDock_);
    extensionDock_->setWidget(extensionEditor_);
    mainWindow_->addDockWidget(Qt::RightDockWidgetArea, extensionDock_);
}


//-----------------------------------------------------------------------------
// Function: DockWidgetHandler::clearItemSelection()
//-----------------------------------------------------------------------------
void DockWidgetHandler::clearItemSelection(HWDesignWidget* designWidget)
{
    if (designWidget != 0)
    {
        QSharedPointer<Design> containedDesign = designWidget->getDiagram()->getDesign();

        adhocEditor_->setVisibilityData(designWidget->getDiagram(), containedDesign,
            designWidget->getEditProvider(), designWidget->isProtected());
    }
    else
    {
        adhocEditor_->clearVisibilityData();
    }

    instanceEditor_->clear();
    interfaceEditor_->clear();
    connectionEditor_->clear();
    adhocEditor_->clearPortData();
}

//-----------------------------------------------------------------------------
// Function: DockWidgetHandler::selectComponent()
//-----------------------------------------------------------------------------
void DockWidgetHandler::selectComponent(QWidget* currentTabWidget, ComponentItem* component)
{
    connectionEditor_->clear();
    interfaceEditor_->clear();
    adhocEditor_->clearPortData();

    DesignWidget* designWidget(0);

    HWDesignWidget* hwDesignWidget = dynamic_cast<HWDesignWidget*>(currentTabWidget);
    if (hwDesignWidget)
    {
        designWidget = hwDesignWidget;
    }
    else
    {
        SystemDesignWidget* systemDesignWidget = dynamic_cast<SystemDesignWidget*>(currentTabWidget);
        designWidget = systemDesignWidget;
    }

    QSharedPointer<Design> containingDesign = designWidget->getDiagram()->getDesign();

    // Update the instance and ad-hoc visibility editors.
    instanceEditor_->setComponentInstance(component, designWidget->getEditProvider(), containingDesign,
        designWidget->getDiagram()->getDesignConfiguration(), designWidget->getOpenViewName());

    HWComponentItem* hwComponent = dynamic_cast<HWComponentItem*>(component);
    if (hwComponent != 0)
    {
        adhocEditor_->setVisibilityData(hwComponent, containingDesign, designWidget->getEditProvider(),
            designWidget->isProtected());
    }
    else
    {
        adhocEditor_->clearVisibilityData();
    }

    VLNV componentVLNV = component->componentModel()->getVlnv();
    libraryWidget_->selectComponent(componentVLNV);
}

//-----------------------------------------------------------------------------
// Function: DockWidgetHandler::selectConnectionInterface()
//-----------------------------------------------------------------------------
void DockWidgetHandler::selectConnectionInterface(QWidget* currentTabWidget, ConnectionEndpoint* interface)
{
    adhocEditor_->clearVisibilityData();
    connectionEditor_->clear();
    instanceEditor_->clear();

    if (!interface->isAdHoc())
    {
        DesignWidget* designWidget = dynamic_cast<DesignWidget*>(currentTabWidget);
        if (designWidget)
        {
            interfaceEditor_->setInterface(interface, designWidget->getDiagram()->getDesign(),
                designWidget->getDiagram()->getDesignConfiguration(), designWidget->getOpenViewName(),
                designWidget->getEditProvider(), designWidget->isProtected());
        }

        adhocEditor_->clearPortData();
    }
    else
    {
        AdHocItem* adhocEndPoint = dynamic_cast<AdHocItem*>(interface);
        if (adhocEndPoint)
        {
            HWDesignWidget* hwDesignWidget = dynamic_cast<HWDesignWidget*>(currentTabWidget);
            if (hwDesignWidget)
            {
                HWDesignDiagram* hwDiagram = dynamic_cast<HWDesignDiagram*>(hwDesignWidget->getDiagram());
                if (hwDiagram)
                {
                    adhocEditor_->setPortData(adhocEndPoint, hwDiagram, hwDesignWidget->getEditProvider());

                    auto dataSource = dynamic_cast<AdHocEnabled*>(adhocEndPoint->encompassingComp());
                    if (dataSource == nullptr)
                    {
                        dataSource = hwDiagram;
                    }

                    adhocEditor_->setVisibilityData(dataSource, hwDiagram->getDesign(), hwDesignWidget->getEditProvider(),
                        hwDesignWidget->isProtected());
                }
            }
        }
        else
        {
            adhocEditor_->clearPortData();
        }

        interfaceEditor_->clear();
    }
}

//-----------------------------------------------------------------------------
// Function: DockWidgetHandler::selectGraphicsConnection()
//-----------------------------------------------------------------------------
void DockWidgetHandler::selectGraphicsConnection(QWidget* currentTabWidget, GraphicsConnection* connection)
{
    DesignWidget* designWidget = dynamic_cast<DesignWidget*>(currentTabWidget);

    Q_ASSERT(connection);
    adhocEditor_->clearVisibilityData();
    adhocEditor_->clearPortData();
    instanceEditor_->clear();
    interfaceEditor_->clear();
    connectionEditor_->setConnection(connection, designWidget->getDiagram());
}

//-----------------------------------------------------------------------------
// Function: DockWidgetHandler::updateWindows()
//-----------------------------------------------------------------------------
void DockWidgetHandler::updateWindows(QWidget* currentTabWidget)
{
    for (auto i = docks_.cbegin(); i != docks_.cend(); ++i)
    {
        updateWindowAndControlVisibility(currentTabWidget, i.key(), i.value());
    }
}

//-----------------------------------------------------------------------------
// Function: DockWidgetHandler::updateWindowAndControlVisibility()
//-----------------------------------------------------------------------------
void DockWidgetHandler::updateWindowAndControlVisibility(QWidget* currentTabWidget,
    TabDocument::SupportedWindows windowType, QDockWidget* dock)
{
    QAction* showAction = dock->toggleViewAction();

    bool allowedToShow = isSupportedWindowType(currentTabWidget, windowType);
    bool wantedToShow = visibilities_.value(windowType);
    bool selected = showAction->isChecked();

    visibilityControls_[windowType]->setVisible(allowedToShow);
    showAction->setVisible(allowedToShow && wantedToShow);
    showAction->setChecked(selected && allowedToShow && wantedToShow);
}

//-----------------------------------------------------------------------------
// Function: DockWidgetHandler::isSupportedWindowType()
//-----------------------------------------------------------------------------
bool DockWidgetHandler::isSupportedWindowType(QWidget* currentTabWidget,
    TabDocument::SupportedWindows windowType) const
{
    return currentlySupportedWindows(currentTabWidget) & windowType;
}

//-----------------------------------------------------------------------------
// Function: DockWidgetHandler::currentlySupportedWindows()
//-----------------------------------------------------------------------------
unsigned int DockWidgetHandler::currentlySupportedWindows(QWidget* currentTabWidget) const
{
    unsigned int windows = defaultWindows();

    TabDocument* doc = dynamic_cast<TabDocument*>(currentTabWidget);
    if (doc != nullptr)
    {
        windows = doc->getSupportedWindows();
    }

    return windows;
}

//-----------------------------------------------------------------------------
// Function: DockWidgetHandler::defaultWindows()
//-----------------------------------------------------------------------------
unsigned int DockWidgetHandler::defaultWindows() const
{
    return TabDocument::OUTPUTWINDOW | TabDocument::LIBRARYWINDOW | TabDocument::CONTEXT_HELP_WINDOW;
}

//-----------------------------------------------------------------------------
// Function: DockWidgetHandler::documentChanged()
//-----------------------------------------------------------------------------
void DockWidgetHandler::documentChanged(TabDocument* doc)
{    
    ComponentEditor* componentEditor = dynamic_cast<ComponentEditor*>(doc);
    DesignWidget* designwidget = dynamic_cast<DesignWidget*>(doc);

    disconnect(extensionEditor_, SLOT(changeVendorExtensions(QString const&, QSharedPointer<Extendable>)));

    if (doc)
    {
        connect(doc, SIGNAL(changeVendorExtensions(QString const&, QSharedPointer<Extendable>)),
            extensionEditor_, SLOT(changeVendorExtensions(QString const&, QSharedPointer<Extendable>)));
    }

    if (componentEditor)
    {
        extensionEditor_->setContext(doc);

        QSharedPointer<Component> editedComponent = componentEditor->getComponent();
        QString extensionsID = QLatin1String("Component: ") + editedComponent->getVlnv().toString();

        extensionEditor_->changeVendorExtensions(extensionsID, editedComponent);
    }
    else if(designwidget)
    {
        extensionEditor_->setContext(doc);

        QSharedPointer<Design> editedDesign = designwidget->getDiagram()->getDesign();
        QString extensionsID = QLatin1String("Design: ") + editedDesign->getVlnv().toString();

        extensionEditor_->changeVendorExtensions(extensionsID, editedDesign);
    }
    else
    {
        extensionEditor_->clear();
    }

    // set the configuration manager to edit the active design
    if (designwidget)
    {
        configurationEditor_->setConfiguration(designwidget);

        QSharedPointer<Document> topItem = libraryHandler_->getModel(doc->getDocumentVLNV());
        QSharedPointer<Component> topComponent = topItem.dynamicCast<Component>();
        if (topComponent)
        {
            instanceEditor_->setContext(topComponent, designwidget->getDiagram()->getEditProvider());
            instanceEditor_->setTopComponentActiveView(
                topComponent->getModel()->findView(designwidget->getOpenViewName()));
            instanceEditor_->setProtection(designwidget->isProtected());

            QSharedPointer<Design> design = designwidget->getDiagram()->getDesign();
            designParametersEditor_->setDisabled(designwidget->isProtected());
            designParametersEditor_->setNewParameters(design->getParameters());
            designParameterTree_->setDesign(design);
            designParameterReferenceCounter_->setDesign(design);

            setupDesignParameterFinder(design);
        }

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
    else
    {
        configurationEditor_->clear();
        systemDetailsEditor_->clear();
        instanceEditor_->clear();
        adhocEditor_->clearVisibilityData();
        adhocEditor_->clearPortData();
        interfaceEditor_->clear();
        connectionEditor_->clear();
    }
}

//-----------------------------------------------------------------------------
// Function: DockWidgetHandler::setupDesignParameterFinder()
//-----------------------------------------------------------------------------
void DockWidgetHandler::setupDesignParameterFinder(QSharedPointer<Design> newDesign)
{
    designParameterFinder_->setParameterList(newDesign->getParameters());

    designAndInstancesParameterFinder_->removeAllFinders();
    designAndInstancesParameterFinder_->addFinder(designParameterFinder_);

    if (!newDesign->getComponentInstances()->isEmpty())
    {
        QList<QSharedPointer<const Component> > componentsWithFinders;

        for (QSharedPointer<ComponentInstance> instance : *newDesign->getComponentInstances())
        {
            QSharedPointer<const Document> referencedDocument =
                libraryHandler_->getModelReadOnly(*instance->getComponentRef().data());
            if (referencedDocument)
            {
                QSharedPointer<const Component> instancedComponent =
                    referencedDocument.dynamicCast<const Component>();
                if (instancedComponent && !componentsWithFinders.contains(instancedComponent))
                {
                    QSharedPointer<ComponentParameterFinder> instanceFinder(
                        new ComponentParameterFinder(instancedComponent));
                    designAndInstancesParameterFinder_->addFinder(instanceFinder);

                    componentsWithFinders.append(instancedComponent);
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: DockWidgetHandler::connectVisibilityControls()
//-----------------------------------------------------------------------------
void DockWidgetHandler::connectVisibilityControls()
{
    connect(visibilityControls_[TabDocument::OUTPUTWINDOW], SIGNAL(toggled(bool)),
        this, SLOT(onOutputAction(bool)), Qt::UniqueConnection);
    connect(visibilityControls_[TabDocument::CONTEXT_HELP_WINDOW], SIGNAL(toggled(bool)),
        this, SLOT(onContextHelpAction(bool)), Qt::UniqueConnection);
    connect(visibilityControls_[TabDocument::LIBRARYWINDOW], SIGNAL(toggled(bool)),
        this, SLOT(onLibraryAction(bool)), Qt::UniqueConnection);
    connect(visibilityControls_[TabDocument::CONFIGURATIONWINDOW], SIGNAL(toggled(bool)),
        this, SLOT(onConfigurationAction(bool)), Qt::UniqueConnection);
    connect(visibilityControls_[TabDocument::SYSTEM_DETAILS_WINDOW], SIGNAL(toggled(bool)),
        this, SLOT(onSystemDetailsAction(bool)), Qt::UniqueConnection);
    connect(visibilityControls_[TabDocument::CONNECTIONWINDOW], SIGNAL(toggled(bool)),
        this, SLOT(onConnectionAction(bool)), Qt::UniqueConnection);
    connect(visibilityControls_[TabDocument::INTERFACEWINDOW], SIGNAL(toggled(bool)),
        this, SLOT(onInterfaceAction(bool)), Qt::UniqueConnection);
    connect(visibilityControls_[TabDocument::INSTANCEWINDOW], SIGNAL(toggled(bool)),
        this, SLOT(onInstanceAction(bool)), Qt::UniqueConnection);
    connect(visibilityControls_[TabDocument::DESIGNPARAMETERSWINDOW], SIGNAL(toggled(bool)),
        this, SLOT(onDesignParametersAction(bool)), Qt::UniqueConnection);
    connect(visibilityControls_[TabDocument::ADHOC_WINDOW], SIGNAL(toggled(bool)),
        this, SLOT(onAdHocEditorAction(bool)), Qt::UniqueConnection);
    connect(visibilityControls_[TabDocument::VENDOREXTENSIONWINDOW], SIGNAL(toggled(bool)),
        this, SLOT(onVendorExtensionVisibilityAction(bool)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: DockWidgetHandler::disconnectVisibilityControls()
//-----------------------------------------------------------------------------
void DockWidgetHandler::disconnectVisibilityControls()
{
    disconnect(visibilityControls_[TabDocument::OUTPUTWINDOW], SIGNAL(toggled(bool)),
        this, SLOT(onOutputAction(bool)));
    disconnect(visibilityControls_[TabDocument::CONTEXT_HELP_WINDOW], SIGNAL(toggled(bool)),
        this, SLOT(onContextHelpAction(bool)));
    disconnect(visibilityControls_[TabDocument::LIBRARYWINDOW], SIGNAL(toggled(bool)),
        this, SLOT(onLibraryAction(bool)));
    disconnect(visibilityControls_[TabDocument::CONFIGURATIONWINDOW], SIGNAL(toggled(bool)),
        this, SLOT(onConfigurationAction(bool)));
    disconnect(visibilityControls_[TabDocument::SYSTEM_DETAILS_WINDOW], SIGNAL(toggled(bool)),
        this, SLOT(onSystemDetailsAction(bool)));
    disconnect(visibilityControls_[TabDocument::CONNECTIONWINDOW], SIGNAL(toggled(bool)),
        this, SLOT(onConnectionAction(bool)));
    disconnect(visibilityControls_[TabDocument::INTERFACEWINDOW], SIGNAL(toggled(bool)),
        this, SLOT(onInterfaceAction(bool)));
    disconnect(visibilityControls_[TabDocument::INSTANCEWINDOW], SIGNAL(toggled(bool)),
        this, SLOT(onInstanceAction(bool)));
    disconnect(visibilityControls_[TabDocument::DESIGNPARAMETERSWINDOW], SIGNAL(toggled(bool)),
        this, SLOT(onDesignParametersAction(bool)));
    disconnect(visibilityControls_[TabDocument::ADHOC_WINDOW], SIGNAL(toggled(bool)),
        this, SLOT(onAdHocEditorAction(bool)));
    disconnect(visibilityControls_[TabDocument::VENDOREXTENSIONWINDOW], SIGNAL(toggled(bool)),
        this, SLOT(onVendorExtensionVisibilityAction(bool)));
}

//-----------------------------------------------------------------------------
// Function: DockWidgetHandler::onOutputAction()
//-----------------------------------------------------------------------------
void DockWidgetHandler::onOutputAction( bool show )
{
    emit adjustVisibilityInWindow(TabDocument::OUTPUTWINDOW, show);
}

//-----------------------------------------------------------------------------
// Function: DockWidgetHandler::onContextHelpAction()
//-----------------------------------------------------------------------------
void DockWidgetHandler::onContextHelpAction( bool show )
{
    emit adjustVisibilityInWindow(TabDocument::CONTEXT_HELP_WINDOW, show);
}

//-----------------------------------------------------------------------------
// Function: DockWidgetHandler::onLibraryAction()
//-----------------------------------------------------------------------------
void DockWidgetHandler::onLibraryAction( bool show )
{
    emit adjustVisibilityInWindow(TabDocument::LIBRARYWINDOW, show);
}

//-----------------------------------------------------------------------------
// Function: DockWidgetHandler::onConfigurationAction()
//-----------------------------------------------------------------------------
void DockWidgetHandler::onConfigurationAction( bool show )
{
    emit adjustVisibilityInWindow(TabDocument::CONFIGURATIONWINDOW, show);
}

//-----------------------------------------------------------------------------
// Function: DockWidgetHandler::onSystemDetailsAction()
//-----------------------------------------------------------------------------
void DockWidgetHandler::onSystemDetailsAction( bool show )
{
    emit adjustVisibilityInWindow(TabDocument::SYSTEM_DETAILS_WINDOW, show);
}

//-----------------------------------------------------------------------------
// Function: DockWidgetHandler::onConnectionAction()
//-----------------------------------------------------------------------------
void DockWidgetHandler::onConnectionAction( bool show )
{
    emit adjustVisibilityInWindow(TabDocument::CONNECTIONWINDOW, show);
}

//-----------------------------------------------------------------------------
// Function: DockWidgetHandler::onVendorExtensionVisibilityAction()
//-----------------------------------------------------------------------------
void DockWidgetHandler::onVendorExtensionVisibilityAction(bool show)
{
    emit adjustVisibilityInWindow(TabDocument::VENDOREXTENSIONWINDOW, show);
}

//-----------------------------------------------------------------------------
// Function: DockWidgetHandler::onInterfaceAction()
//-----------------------------------------------------------------------------
void DockWidgetHandler::onInterfaceAction( bool show )
{
    emit adjustVisibilityInWindow(TabDocument::INTERFACEWINDOW, show);
}

//-----------------------------------------------------------------------------
// Function: DockWidgetHandler::onAdHocEditorAction()
//-----------------------------------------------------------------------------
void DockWidgetHandler::onAdHocEditorAction(bool show)
{
    emit adjustVisibilityInWindow(TabDocument::ADHOC_WINDOW, show);
}

//-----------------------------------------------------------------------------
// Function: DockWidgetHandler::onInstanceAction()
//-----------------------------------------------------------------------------
void DockWidgetHandler::onInstanceAction( bool show )
{
    emit adjustVisibilityInWindow(TabDocument::INSTANCEWINDOW, show);
}

//-----------------------------------------------------------------------------
// Function: DockWidgetHandler::onDesignParametersAction()
//-----------------------------------------------------------------------------
void DockWidgetHandler::onDesignParametersAction(bool show)
{
    emit adjustVisibilityInWindow(TabDocument::DESIGNPARAMETERSWINDOW, show);
}

//-----------------------------------------------------------------------------
// Function: DockWidgetHandler::onDockLocationChanged()
//-----------------------------------------------------------------------------
void DockWidgetHandler::onDockLocationChanged(Qt::DockWidgetArea area)
{
    auto const& dock = qobject_cast<QDockWidget*>(sender());
    auto action = dock->toggleViewAction();

    placeActionInToolbar(action, area);
}

//-----------------------------------------------------------------------------
// Function: DockWidgetHandler::setWindowVisibilityForSupportedWindow()
//-----------------------------------------------------------------------------
void DockWidgetHandler::setWindowVisibilityForSupportedWindow(QWidget* currentTabWidget,
    TabDocument::SupportedWindows type, bool show)
{
    if (isSupportedWindowType(currentTabWidget, type))
    {
        setWindowVisibility(type, show);
    }
}

//-----------------------------------------------------------------------------
// Function: DockWidgetHandler::setWindowVisibility()
//-----------------------------------------------------------------------------
void DockWidgetHandler::setWindowVisibility(TabDocument::SupportedWindows windowType, bool show)
{
    visibilities_.insert(windowType, show);

    visibilityControls_[windowType]->setChecked(show);
    docks_[windowType]->toggleViewAction()->setVisible(show);
    docks_[windowType]->setVisible(show);
}

//-----------------------------------------------------------------------------
// Function: DockWidgetHandler::placeActionInToolbar()
//-----------------------------------------------------------------------------
void DockWidgetHandler::placeActionInToolbar(QAction* action, Qt::DockWidgetArea area)
{
    if ((area == Qt::LeftDockWidgetArea && leftActions_->actions().contains(action)) ||
        (area == Qt::RightDockWidgetArea && rightActions_->actions().contains(action)))
    {
        return;
    }

    leftActions_->removeAction(action);
    leftToolbar_->removeAction(action);

    rightActions_->removeAction(action);
    rightToolbar_->removeAction(action);

    if (area == Qt::LeftDockWidgetArea)
    {
        leftActions_->addAction(action);
        leftToolbar_->addAction(action);
    }
    else if (area == Qt::RightDockWidgetArea)
    {
        rightActions_->addAction(action);
        rightToolbar_->addAction(action);
    }
}

//-----------------------------------------------------------------------------
// Function: DockWidgetHandler::changeProtection()
//-----------------------------------------------------------------------------
void DockWidgetHandler::changeProtection(TabDocument* doc, bool locked)
{
    extensionEditor_->setLocked(locked);

    DesignWidget* designwidget = dynamic_cast<DesignWidget*>(doc);
    if (designwidget)
    {
        // update the editors to match the locked state
        configurationEditor_->setLocked(locked);
        instanceEditor_->setProtection(locked);
        designParametersEditor_->setDisabled(locked);

        if (designwidget->getSupportedWindows() & TabDocument::SYSTEM_DETAILS_WINDOW)
        {
            systemDetailsEditor_->setLocked(locked);
        }
    }
    else
    {        
        configurationEditor_->setLocked(true);
        systemDetailsEditor_->setLocked(true);
    }
}

//-----------------------------------------------------------------------------
// Function: DockWidgetHandler::getDesignAndInstanceParameterFinder()
//-----------------------------------------------------------------------------
QSharedPointer<MultipleParameterFinder> DockWidgetHandler::getDesignAndInstanceParameterFinder() const
{
    return designAndInstancesParameterFinder_;
}

//-----------------------------------------------------------------------------
// Function: DockWidgetHandler::getDesignParameterFinder()
//-----------------------------------------------------------------------------
QSharedPointer<ListParameterFinder> DockWidgetHandler::getDesignParameterFinder() const
{
    return designParameterFinder_;
}

//-----------------------------------------------------------------------------
// Function: DockWidgetHandler::refreshDesignDocument()
//-----------------------------------------------------------------------------
void DockWidgetHandler::refreshDesignDocument(QWidget* currentTabWidget)
{
    TabDocument* doc = static_cast<TabDocument*>(currentTabWidget);
   
    DesignWidget* designWidget = dynamic_cast<DesignWidget*>(doc);
    if (designWidget)
    {
        QSharedPointer<Document> topItem = libraryHandler_->getModel(doc->getDocumentVLNV());
        QSharedPointer<Component> topComponent = topItem.dynamicCast<Component>();

        instanceEditor_->setContext(topComponent, designWidget->getDiagram()->getEditProvider());

        QSharedPointer<Design> design = designWidget->getDiagram()->getDesign();
        designParametersEditor_->setNewParameters(design->getParameters());
        designParameterTree_->setDesign(design);
        designParameterReferenceCounter_->setDesign(design);

        setupDesignParameterFinder(design);
    }
}

//-----------------------------------------------------------------------------
// Function: DockWidgetHandler::showHelp()
//-----------------------------------------------------------------------------
void DockWidgetHandler::showHelp()
{
    helpWnd_->show();
}
