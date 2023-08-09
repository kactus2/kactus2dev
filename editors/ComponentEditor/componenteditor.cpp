//-----------------------------------------------------------------------------
// File: componenteditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 16.5.2012
//
// Description:
// The editor to edit/packet IP-Xact components.
//-----------------------------------------------------------------------------

#include "componenteditor.h"

#include <KactusAPI/include/LibraryInterface.h>

#include <editors/ComponentEditor/treeStructure/componenteditorrootitem.h>
#include <editors/ComponentEditor/treeStructure/componenteditorgeneralitem.h>
#include <editors/ComponentEditor/treeStructure/componenteditorfilesetsitem.h>
#include <editors/ComponentEditor/treeStructure/componenteditorparametersitem.h>
#include <editors/ComponentEditor/treeStructure/componenteditormemmapsitem.h>
#include <editors/ComponentEditor/treeStructure/componenteditoraddrspacesitem.h>
#include <editors/ComponentEditor/treeStructure/componenteditorviewsitem.h>
#include <editors/ComponentEditor/treeStructure/componenteditorportsitem.h>
#include <editors/ComponentEditor/treeStructure/componenteditorbusinterfacesitem.h>
#include <editors/ComponentEditor/treeStructure/componenteditorchannelsitem.h>
#include <editors/ComponentEditor/treeStructure/ComponentEditorChoicesItem.h>
#include <editors/ComponentEditor/treeStructure/componenteditorcpusitem.h>
#include <editors/ComponentEditor/treeStructure/componenteditorotherclocksitem.h>
#include <editors/ComponentEditor/treeStructure/ResetTypesItem.h>
#include <editors/ComponentEditor/treeStructure/componenteditorcominterfacesitem.h>
#include <editors/ComponentEditor/treeStructure/componenteditorapiinterfacesitem.h>
#include <editors/ComponentEditor/treeStructure/componenteditorswpropertiesitem.h>
#include <editors/ComponentEditor/treeStructure/ComponentEditorSystemViewsItem.h>
#include <editors/ComponentEditor/treeStructure/RemapStatesItem.h>
#include <editors/ComponentEditor/treeStructure/ModesItem.h>
#include <editors/ComponentEditor/treeStructure/componenteditortreemodel.h>
#include <editors/ComponentEditor/treeStructure/InstantiationsItem.h>
#include <editors/ComponentEditor/treeStructure/ComponentEditorIndirectInterfacesItem.h>
#include <editors/ComponentEditor/treeStructure/componenteditoritem.h>
#include <editors/ComponentEditor/treeStructure/ComponentEditorTreeSortProxyModel.h>
#include <editors/ComponentEditor/treeStructure/ComponentEditorTreeDelegate.h>

#include <editors/ComponentEditor/parameterReferenceTree/ComponentParameterReferenceTree.h>
#include <editors/ComponentEditor/parameterReferenceTree/ParameterReferenceTreeWindow.h>

#include <editors/ComponentEditor/general/generaleditor.h>

#include <editors/common/ExpressionSet.h>

#include <KactusAPI/include/ParameterCache.h>
#include <KactusAPI/include/IPXactSystemVerilogParser.h>
#include <KactusAPI/include/ComponentAndInstantiationsParameterFinder.h>

#include <common/dialogs/newObjectDialog/newobjectdialog.h>
#include <common/dialogs/comboSelector/comboselector.h>

#include <editors/ComponentEditor/itemeditor.h>
#include <editors/ComponentEditor/itemvisualizer.h>

#include <IPXACTmodels/common/TagData.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/FileSet.h>
#include <IPXACTmodels/Component/validators/ComponentValidator.h>

#include <KactusAPI/include/BusInterfaceInterface.h>
#include <KactusAPI/include/BusInterfaceInterfaceFactory.h>
#include <KactusAPI/include/AbstractionTypeInterface.h>
#include <KactusAPI/include/PortMapInterface.h>

#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QApplication>

//-----------------------------------------------------------------------------
// Function: ComponentEditor::ComponentEditor()
//-----------------------------------------------------------------------------
ComponentEditor::ComponentEditor(LibraryInterface* libHandler,
								 QSharedPointer<Component> component,
								 QWidget *parent):
TabDocument(parent, DOC_PROTECTION_SUPPORT),
libHandler_(libHandler),
component_(component),
navigationSplitter_(Qt::Horizontal, this),
editorVisualizerSplitter_(Qt::Horizontal, &navigationSplitter_), 
navigationModel_(this),
navigationView_(libHandler, component->getVlnv(), &navigationSplitter_),
proxy_(this),
editorSlot_(&editorVisualizerSplitter_),
visualizerSlot_(&editorVisualizerSplitter_),
parameterFinder_(new ParameterCache(component_)),
fullParameterFinder_(),
referenceCounter_(0),
expressionFormatter_(new ExpressionFormatter(parameterFinder_)),
expressionParser_(new IPXactSystemVerilogParser(parameterFinder_)),
validator_(expressionParser_, libHandler_),
parameterReferenceTree_(0),
parameterReferenceWindow_()
{
    fullParameterFinder_ = QSharedPointer<ComponentAndInstantiationsParameterFinder>(
        new ComponentAndInstantiationsParameterFinder(component));

    referenceCounter_ = QSharedPointer<ComponentParameterReferenceCounter>(
        new ComponentParameterReferenceCounter(fullParameterFinder_, component_));

    QSharedPointer<ExpressionFormatter> fullFormatter(new ExpressionFormatter(fullParameterFinder_));
    parameterReferenceTree_ =
        new ComponentParameterReferenceTree(component_, fullFormatter, referenceCounter_);
    parameterReferenceWindow_ = new ParameterReferenceTreeWindow(parameterReferenceTree_, this);

    supportedWindows_ |= TabDocument::VENDOREXTENSIONWINDOW;

    // these can be used when debugging to identify the objects
	setObjectName(tr("ComponentEditor"));
	navigationSplitter_.setObjectName(tr("NavigationSplitter"));
	editorVisualizerSplitter_.setObjectName(tr("EditorVisualizerSplitter"));
	editorSlot_.setObjectName(tr("EditorSlot"));
	visualizerSlot_.setObjectName(tr("VisualizerSlot"));

	Q_ASSERT(component_);
	Q_ASSERT(libHandler_);

	// set the name and type for the tab
    setDocumentName(QString("%1 (%2)").arg(component_->getVlnv().getName()).arg(
        component_->getVlnv().getVersion()));

    if (component_->getImplementation() == KactusAttribute::HW)
    {
	    setDocumentType(tr("HW Component"));
    }
    else if (component_->getImplementation() == KactusAttribute::SW)
    {
        setDocumentType(tr("SW Component"));
    }
    else if (component_->getImplementation() == KactusAttribute::SYSTEM)
    {
        setDocumentType(tr("Unmapped System"));
    }

    addRelatedVLNV(component_->getVlnv());

    setupLayout();

	// set the component to be displayed in the navigation model
    navigationModel_.setRootItem(createNavigationRootForComponent(component_));

	QSettings settings;
	setRowVisibility(settings);
	
    // Set source model for the proxy.
    proxy_.setSourceModel(&navigationModel_);

    navigationView_.setItemDelegate(new ComponentEditorTreeDelegate(this));

	// connect the view with the model (proxy) and sort.
	navigationView_.setModel(&proxy_);
    navigationView_.sortByColumn(0, Qt::AscendingOrder);

	// when starting the component editor open the general editor.
	onItemActivated(proxy_.index(0, 0, QModelIndex()));

	// navigation model may request an item to be expanded
	connect(&navigationModel_, SIGNAL(expandItem(const QModelIndex&)),
		this, SLOT(onExpand(const QModelIndex&)), Qt::UniqueConnection);

	connect(&navigationView_, SIGNAL(activated(const QModelIndex&)),
		this, SLOT(onItemActivated(const QModelIndex&)), Qt::UniqueConnection);

	connect(&navigationModel_, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
		    this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&navigationModel_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&navigationModel_, SIGNAL(helpUrlRequested(QString const&)),
            this, SIGNAL(helpUrlRequested(QString const&)), Qt::UniqueConnection);
	connect(&navigationModel_, SIGNAL(errorMessage(const QString&)),
		this, SLOT(onErrorDialog(const QString&)), Qt::UniqueConnection);

    connect(&navigationModel_, SIGNAL(openCSource(QString const&, QSharedPointer<Component>)),
            this, SIGNAL(openCSource(QString const&, QSharedPointer<Component>)), Qt::UniqueConnection);

	connect(&navigationModel_, SIGNAL(selectItem(const QModelIndex&)),
		this, SLOT(onNavigationTreeSelection(const QModelIndex&)), Qt::UniqueConnection);
	connect(&navigationModel_, SIGNAL(openDesign(const VLNV&, const QString&)),
		this, SIGNAL(openDesign(const VLNV&, const QString&)), Qt::UniqueConnection);
	connect(&navigationModel_, SIGNAL(openBus(const VLNV&)),
		this, SIGNAL(openBus(const VLNV&)), Qt::UniqueConnection);
	connect(&navigationModel_, SIGNAL(openComDefinition(const VLNV&)),
		this, SIGNAL(openComDefinition(const VLNV&)), Qt::UniqueConnection);
	connect(&navigationModel_, SIGNAL(openSWDesign(const VLNV&, const QString&)),
		this, SIGNAL(openSWDesign(const VLNV&, const QString&)), Qt::UniqueConnection);
	connect(&navigationModel_, SIGNAL(openSystemDesign(const VLNV&, const QString&)),
		this, SIGNAL(openSystemDesign(const VLNV&, const QString&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ComponentEditor::~ComponentEditor()
//-----------------------------------------------------------------------------
ComponentEditor::~ComponentEditor()
{

}

//-----------------------------------------------------------------------------
// Function: ComponentEditor::getIdentifyingVLNV()
//-----------------------------------------------------------------------------
VLNV ComponentEditor::getIdentifyingVLNV() const
{
    return getDocumentVLNV();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditor::getDocumentVLNV()
//-----------------------------------------------------------------------------
VLNV ComponentEditor::getDocumentVLNV() const
{
	return component_->getVlnv();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditor::isHWImplementation()
//-----------------------------------------------------------------------------
bool ComponentEditor::isHWImplementation() const
{
    return component_->getImplementation() == KactusAttribute::HW;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditor::refresh()
//-----------------------------------------------------------------------------
void ComponentEditor::refresh()
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	// remember the locked state
	bool locked = isProtected();
	
	// clear the slots 
	editorSlot_.setWidget(NULL);
	visualizerSlot_.setWidget(NULL);

	// get the VLNV of the component
	VLNV compVLNV = component_->getVlnv();

	// get the original model of the component
	QSharedPointer<Component> comp = libHandler_->getModel<Component>(compVLNV);
	Q_ASSERT(comp.isNull() == false);

	// rebuild the navigation tree
	navigationModel_.setRootItem(createNavigationRootForComponent(comp));
	component_.clear();
	component_ = comp;

    referenceCounter_->setComponent(component_);
    parameterReferenceTree_->setComponent(component_);

    parameterFinder_->setComponent(comp);
    fullParameterFinder_->setComponent(comp);

	// open the general editor.
	onItemActivated(proxy_.index(0, 0, QModelIndex()));

	// the document is no longer modified
	setModified(false);
	TabDocument::refresh();

	// set the protection state to same as before refreshing
	setProtection(locked);

	QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditor::applySettings()
//-----------------------------------------------------------------------------
void ComponentEditor::applySettings(QSettings& settings)
{
	setRowVisibility(settings);
}

//-----------------------------------------------------------------------------
// Function: componenteditor::getHwItemNames()
//-----------------------------------------------------------------------------
QStringList ComponentEditor::getHwItemNames()
{
	QStringList itemNames;

	itemNames << "File_sets" << "Choices" << "Parameters" << "Memory_maps" << 
		"Address_spaces" << "Instantiations" << "Views" << "Software_views" << "System_views" << "Ports" <<
        "Bus_interfaces" << "Indirect_interfaces" << "Channels" << "Remap_states" << "Cpus" <<
        "Other_clock_drivers" << "Reset_types" << "COM_interfaces" << "Software_properties";

	return itemNames;
}

//-----------------------------------------------------------------------------
// Function: componenteditor::getSwItemNames()
//-----------------------------------------------------------------------------
QStringList ComponentEditor::getSwItemNames()
{
	QStringList itemNames;

	itemNames << "File_sets" << "Choices" << "Parameters" << "Software_views" << "COM_interfaces" << 
		"API_interfaces" << "Software_properties";

	return itemNames;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditor::validate()
//-----------------------------------------------------------------------------
bool ComponentEditor::validate(QVector<QString>& errorList)
{
    if (!validator_.validate(component_))
    {
        validator_.findErrorsIn(errorList, component_);
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditor::save()
//-----------------------------------------------------------------------------
bool ComponentEditor::save()
{
	if (libHandler_->writeModelToFile(component_))
    {
        navigationModel_.clearItemsModified();
		return TabDocument::save();
	}
	else
    {
		emit errorMessage(tr("Component was not saved to disk."));
		return false;
	}
}

//-----------------------------------------------------------------------------
// Function: ComponentEditor::saveAs()
//-----------------------------------------------------------------------------
bool ComponentEditor::saveAs()
{
	// Ask the user for a new VLNV along with attributes and directory.
    KactusAttribute::ProductHierarchy prodHier = component_->getHierarchy();
    KactusAttribute::Firmness firmness = component_->getFirmness();
    QVector<TagData> tags = component_->getTags();

	VLNV vlnv;
	QString directory;

    if (component_->getImplementation() == KactusAttribute::HW)
    {
	    if (!NewObjectDialog::saveAsDialog(
            parentWidget(), libHandler_, component_->getVlnv(), prodHier, firmness, tags, vlnv, directory))
        {
		    return false;
	    }
    }
    else
    {
        if (!NewObjectDialog::saveAsDialog(parentWidget(), libHandler_, component_->getVlnv(), vlnv, directory))
        {
            return false;
        }
    }

	// save pointer to the old component
	QSharedPointer<Component> oldComponent = component_;

	// create copies of the objects so saving is not done to the original component
	component_ = QSharedPointer<Component>(new Component(*component_));

	// make sure the vlnv type is correct
	VLNV compVLNV = vlnv;
	compVLNV.setType(VLNV::COMPONENT);

	// update the vlnv
	component_->setVlnv(vlnv);
    component_->setHierarchy(prodHier);
    component_->setFirmness(firmness);

	// get the paths to the original xml file
	QFileInfo sourceInfo(libHandler_->getPath(oldComponent->getVlnv()));
	QString sourcePath = sourceInfo.absolutePath();

	// update the file paths and copy necessary files
    updateComponentFiles(component_, oldComponent, sourcePath, directory);

	// Write the component to a file.
	if (libHandler_->writeModelToFile(directory, component_))
    {
		setDocumentName(compVLNV.getName() + " (" + compVLNV.getVersion() + ")");
        navigationModel_.clearItemsModified();
		return TabDocument::saveAs();
	}
	else
    {
		emit errorMessage(tr("Component was not saved to disk."));
		return false;
	}

    clearRelatedVLNVs();
    addRelatedVLNV(compVLNV);

    return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditor::onNavigationTreeSelection()
//-----------------------------------------------------------------------------
void ComponentEditor::onNavigationTreeSelection( const QModelIndex& index )
{
	navigationView_.setCurrentIndex(index);
	onItemActivated(index);
}

//-----------------------------------------------------------------------------
// Function: ComponentEditor::onExpand()
//-----------------------------------------------------------------------------
void ComponentEditor::onExpand(const QModelIndex& index)
{    
    // Expand parents until root is hit.            
    QModelIndex parentIndex = index;
    while(parentIndex.isValid())
    {
        navigationView_.expand(proxy_.mapFromSource(parentIndex));
        parentIndex = parentIndex.parent();
    }    
}

//-----------------------------------------------------------------------------
// Function: ComponentEditor::onErrorDialog()
//-----------------------------------------------------------------------------
void ComponentEditor::onErrorDialog(const QString& message)
{
    QMessageBox::warning(this, tr("Component editor"), message);
}

//-----------------------------------------------------------------------------
// Function: ComponentEditor::onItemActivated()
//-----------------------------------------------------------------------------
void ComponentEditor::onItemActivated( const QModelIndex& index )
{
    // If tree proxy model index is used, the item must be retrieved from the source model.
    const ComponentEditorTreeProxyModel* indexModel = 
        dynamic_cast<const ComponentEditorTreeProxyModel*>(index.model());
	ComponentEditorItem* item = 0;
    if ( indexModel == 0 )
    {
        item = static_cast<ComponentEditorItem*>(index.internalPointer());
    }
    else
    {
        item = static_cast<ComponentEditorItem*>(proxy_.mapToSource(index).internalPointer());
    }

    Q_ASSERT(item);

	QList<int> editorVisualizerSizes;

	ItemEditor* editor = item->editor();
	if (editor)
    {
		// the width of the previous editor
		QWidget* prevEditor = editorSlot_.getWidget();
		if (prevEditor)
        {
			int prevWidth = prevEditor->size().width();
			editorVisualizerSizes.append(prevWidth);
		}
		// if there was no previous editor then use the size hint
		else
        {
			editorVisualizerSizes.append(editor->sizeHint().width());
		}
		editor->refresh();
	}
	// if there is no editor then hide the editor slot
	else
    {
		editorVisualizerSizes.append(0);
	}
	editorSlot_.setWidget(editor);

	ItemVisualizer* visualizer = item->visualizer();
	if (visualizer)
    {
		// the width of the previous visualizer
		QWidget* prevVisualizer = visualizerSlot_.getWidget();
		if (prevVisualizer) {
			int prevWidth = prevVisualizer->size().width();
			editorVisualizerSizes.append(prevWidth);
		}
		// if there was no previous visualizer then use the size hint
		else
        {
			editorVisualizerSizes.append(visualizer->sizeHint().width());
		}
	}
	// if there is no visualizer then hide the visualizer slot
	else
    {
		editorVisualizerSizes.append(0);
	}
	visualizerSlot_.setWidget(visualizer);

	editorVisualizerSplitter_.setSizes(editorVisualizerSizes);
}

//-----------------------------------------------------------------------------
// Function: ComponentEditor::setProtection()
//-----------------------------------------------------------------------------
void ComponentEditor::setProtection( bool locked )
{
    TabDocument::setProtection(locked);

    // tell tree items to change the state of the editors
    navigationModel_.setLocked(locked);
}

//-----------------------------------------------------------------------------
// Function: ComponentEditor::setRowVisibility()
//-----------------------------------------------------------------------------
void ComponentEditor::setRowVisibility(QSettings& settings)
{
    QString workSpace = settings.value("Workspaces/CurrentWorkspace").toString();

    settings.beginGroup("Workspaces/" + workSpace + "/ComponentEditorFilters");

    if (isHWImplementation())
    {
        settings.beginGroup("HW");
        settings.beginGroup(KactusAttribute::hierarchyToString(component_->getHierarchy()));
    }
    else
    {
        settings.beginGroup("SW");
    }

    // List of the hidden rows in component editor.
    QStringList hiddenRows;
    for (QString name : settings.childKeys()) 
    {
        if (settings.value(name, true).toBool() == false)
        {
            QString wordReplaced = name.replace("_"," ");
            hiddenRows.append(wordReplaced);
        }
    }

    if (isHWImplementation())
    {
        // End hierarchy group.
        settings.endGroup();
    }

    // End hardware or software group.
    settings.endGroup();

    // End Workspace/CurrentWorkspace/ComponentEditorFilters group.
    settings.endGroup();

    proxy_.setRowVisibility( hiddenRows );
}

//-----------------------------------------------------------------------------
// Function: ComponentEditor::createNavigationRootForComponent()
//-----------------------------------------------------------------------------
QSharedPointer<ComponentEditorRootItem> ComponentEditor::createNavigationRootForComponent(
    QSharedPointer<Component> component)
{
    ExpressionSet expressionsSupport({ parameterFinder_, expressionParser_, expressionFormatter_});

    auto docRevision = component->getRevision();

    ComponentEditorRootItem* root = new ComponentEditorRootItem(libHandler_, component, &navigationModel_);

    QSharedPointer<ComponentEditorGeneralItem> generalItem(
        new ComponentEditorGeneralItem(&navigationModel_, libHandler_, component, root));

    connect(generalItem.data(), SIGNAL(changeVendorExtensions(QString const&, QSharedPointer<Extendable>)),
        this, SIGNAL(changeVendorExtensions(QString const&, QSharedPointer<Extendable>)), Qt::UniqueConnection);

    root->addChildItem(generalItem);

    GeneralEditor* genEditor = static_cast<GeneralEditor*>(root->child(0)->editor());

    connect(genEditor, SIGNAL(hierarchyChanged(QSettings&)), this, SLOT(setRowVisibility(QSettings&)));

    root->addChildItem(QSharedPointer<ComponentEditorFileSetsItem>( new ComponentEditorFileSetsItem(
        &navigationModel_, libHandler_, component, referenceCounter_, parameterFinder_,
        expressionParser_, expressionFormatter_, root)));

    if (component->getImplementation() == KactusAttribute::HW)
    {
        root->addChildItem(QSharedPointer<ComponentEditorChoicesItem>(
            new ComponentEditorChoicesItem(&navigationModel_, libHandler_, component, expressionParser_, root)));

        QSharedPointer<ComponentEditorParametersItem> parametersItem(new ComponentEditorParametersItem(
            &navigationModel_, libHandler_, component, referenceCounter_, parameterFinder_, expressionParser_,
            expressionFormatter_, root));

        root->addChildItem(parametersItem);

        connect(parametersItem.data(), SIGNAL(openReferenceTree(QString const&, QString const&)),
            parameterReferenceWindow_, SLOT(openReferenceTree(QString const&, QString const)),
            Qt::UniqueConnection);

        root->addChildItem(QSharedPointer<ComponentEditorMemMapsItem>(new ComponentEditorMemMapsItem(
            &navigationModel_, libHandler_, component, referenceCounter_, parameterFinder_, expressionFormatter_,
            expressionParser_, root)));

        root->addChildItem(QSharedPointer<ComponentEditorAddrSpacesItem>(new ComponentEditorAddrSpacesItem(
            &navigationModel_, libHandler_, component, referenceCounter_, parameterFinder_, expressionFormatter_,
            expressionParser_, root)));
    }

    QSharedPointer<InstantiationsItem> instantiationsItem (new InstantiationsItem(&navigationModel_, libHandler_,
        component, referenceCounter_, parameterFinder_, expressionFormatter_, expressionParser_, root));

    root->addChildItem(instantiationsItem);

    connect(instantiationsItem.data(), SIGNAL(openReferenceTree(QString const&, QString const&)),
        parameterReferenceWindow_, SLOT(openReferenceTree(QString const&, QString const&)), Qt::UniqueConnection);

    QSharedPointer<ComponentEditorViewsItem> viewsItem(new ComponentEditorViewsItem(&navigationModel_, libHandler_,
        component, referenceCounter_, parameterFinder_, expressionFormatter_, expressionParser_, root));

    root->addChildItem(viewsItem);

    connect(viewsItem.data(), SIGNAL(openReferenceTree(QString const&, QString const&)),
        parameterReferenceWindow_, SLOT(openReferenceTree(QString const&, QString const&)), Qt::UniqueConnection);

    if (component->getImplementation() != KactusAttribute::SW)
    {
        root->addChildItem(QSharedPointer<ComponentEditorSystemViewsItem>(new ComponentEditorSystemViewsItem(
            &navigationModel_, libHandler_, component, parameterFinder_, expressionParser_, expressionFormatter_,
            root)));
    }

    if (component->getImplementation() == KactusAttribute::HW)
    {
        BusInterfaceInterface* busInterface = BusInterfaceInterfaceFactory::createBusInterface(
            parameterFinder_, expressionFormatter_, expressionParser_, component, libHandler_);

        AbstractionTypeInterface* absTypeInterface = busInterface->getAbstractionTypeInterface();
        if (absTypeInterface)
        {
            PortMapInterface* portMapInterface =
                dynamic_cast<PortMapInterface*>(absTypeInterface->getPortMapInterface());
            if (portMapInterface)
            {
                QSharedPointer<ComponentEditorPortsItem> portsItem(new ComponentEditorPortsItem(&navigationModel_,
                    libHandler_, component, referenceCounter_, parameterFinder_, expressionFormatter_,
                    expressionParser_, busInterface, root));

                root->addChildItem(portsItem);
                connect(portsItem.data(), SIGNAL(createInterface()),
                    root, SLOT(onInterfaceAdded()), Qt::UniqueConnection);

                connect(portsItem.data(),
                    SIGNAL(changeVendorExtensions(QString const&, QSharedPointer<Extendable>)),
                    this, SIGNAL(changeVendorExtensions(QString const&, QSharedPointer<Extendable>)),
                    Qt::UniqueConnection);

                QSharedPointer<ComponentEditorBusInterfacesItem> busInterfaceItem(
                    new ComponentEditorBusInterfacesItem(busInterface, portMapInterface, &navigationModel_,
                        libHandler_, component, referenceCounter_, expressionsSupport, root, parentWidget()));

                root->addChildItem(busInterfaceItem);

                connect(busInterfaceItem.data(), SIGNAL(openReferenceTree(QString const&, QString const&)),
                    parameterReferenceWindow_, SLOT(openReferenceTree(QString const&, QString const&)),
                    Qt::UniqueConnection);

                QSharedPointer<ComponentEditorIndirectInterfacesItem> indirectInterfacesItem(
                    QSharedPointer<ComponentEditorIndirectInterfacesItem>(
                        new ComponentEditorIndirectInterfacesItem(&navigationModel_, libHandler_, component,
                            referenceCounter_, parameterFinder_, expressionFormatter_, expressionParser_,
                            busInterface, root, parentWidget())));

                root->addChildItem(indirectInterfacesItem);

                connect(indirectInterfacesItem.data(), SIGNAL(openReferenceTree(QString const&, QString const&)),
                    parameterReferenceWindow_, SLOT(openReferenceTree(QString const&, QString const&)),
                    Qt::UniqueConnection);
            }
        }

        root->addChildItem(QSharedPointer<ComponentEditorChannelsItem>(
            new ComponentEditorChannelsItem(&navigationModel_, libHandler_, component, expressionParser_, root)));

        if (docRevision == Document::Revision::Std14)
        {
            root->addChildItem(QSharedPointer<RemapStatesItem>(
                new RemapStatesItem(&navigationModel_, libHandler_, component, referenceCounter_, parameterFinder_,
                    expressionFormatter_, expressionParser_, root)));
        }
        else if (docRevision == Document::Revision::Std22)
        {
            root->addChildItem(QSharedPointer<ModesItem>(
                new ModesItem(&navigationModel_, libHandler_, component, referenceCounter_, expressionsSupport, root)));
        }

        root->addChildItem(QSharedPointer<ComponentEditorCpusItem>(
            new ComponentEditorCpusItem(&navigationModel_, libHandler_, component, referenceCounter_,
                expressionsSupport, root)));

        root->addChildItem(QSharedPointer<ComponentEditorOtherClocksItem>(
            new ComponentEditorOtherClocksItem(&navigationModel_, libHandler_, component, expressionParser_, root)));

        root->addChildItem(QSharedPointer<ResetTypesItem>(
            new ResetTypesItem(&navigationModel_, libHandler_, component, validator_, root)));
    }
    else if (component->getImplementation() == KactusAttribute::SW)
    {
        root->addChildItem(QSharedPointer<ComponentEditorAPIInterfacesItem>(
            new ComponentEditorAPIInterfacesItem(&navigationModel_, libHandler_, component, root)));
    }  

    root->addChildItem(QSharedPointer<ComponentEditorComInterfacesItem>(
        new ComponentEditorComInterfacesItem(&navigationModel_, libHandler_, component, root)));

    root->addChildItem(QSharedPointer<ComponentEditorSWPropertiesItem>(
        new ComponentEditorSWPropertiesItem(&navigationModel_, libHandler_, component, root)));

    return QSharedPointer<ComponentEditorRootItem>(root);
}

//-----------------------------------------------------------------------------
// Function: ComponentEditor::setupLayout()
//-----------------------------------------------------------------------------
void ComponentEditor::setupLayout()
{
    // the top layout contains only the navigation splitter
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(&navigationSplitter_);
    layout->setContentsMargins(5, 5, 5, 5);

    // editor visualizer splitter contains the editor- and visualizer slots
    editorVisualizerSplitter_.addWidget(&editorSlot_);
    editorVisualizerSplitter_.addWidget(&visualizerSlot_);

    // navigation splitter contains the navigation tree and the other splitter
    navigationSplitter_.addWidget(&navigationView_);
    navigationSplitter_.addWidget(&editorVisualizerSplitter_);
    navigationSplitter_.setStretchFactor(1, 1);

    // The navigation tree takes 1/5 of the space available and editor and 
    // visualizer take the rest
    QList<int> navigationSize;
    navigationSize.append(ComponentTreeView::DEFAULT_WIDTH);
    navigationSize.append(4 * ComponentTreeView::DEFAULT_WIDTH);
    navigationSplitter_.setSizes(navigationSize);
}

//-----------------------------------------------------------------------------
// Function: componenteditor::updateComponentFiles()
//-----------------------------------------------------------------------------
void ComponentEditor::updateComponentFiles(QSharedPointer<Component> targetComponent,
    QSharedPointer<Component> otherComponent, QString const& sourcePath, QString const& targetPath)
{
    // can't operate on the same component
    if (targetComponent == otherComponent)
    {
        return;
    }

    // get the files from the other component
    for (QString const& file : getComponentFileNames(otherComponent))
    {
        // get the absolute path to the file
        QDir source(sourcePath);
        QString absoluteSource = source.absoluteFilePath(file);

        // if file is located under the source directory
        if (!file.contains(QString("../")))
        {

            QDir target(targetPath);
            QString absoluteTarget = target.absoluteFilePath(file);

            QFileInfo targetInfo(absoluteTarget);

            target.mkpath(targetInfo.absolutePath());
            QFile::copy(absoluteSource, absoluteTarget);

        }
        // if file is higher in directory hierarchy than the source directory
        else
        {
            // update the file name
            changeFileName(file, absoluteSource, targetComponent);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: componenteditor::getComponentFileNames()
//-----------------------------------------------------------------------------
QStringList ComponentEditor::getComponentFileNames(QSharedPointer<Component> component) const
{
    QStringList fileNames;
    for (QSharedPointer<FileSet> fileSet : *component->getFileSets())
    {
        fileNames.append(fileSet->getFileNames());
    }

    return fileNames;
}

//-----------------------------------------------------------------------------
// Function: componenteditor::changeFileName()
//-----------------------------------------------------------------------------
void ComponentEditor::changeFileName(QString const& from, QString const& to, QSharedPointer<Component> component)
    const
{
    for (QSharedPointer<FileSet> fileSet : *component->getFileSets())
    {
        if (fileSet->contains(from))
        {
            fileSet->changeFileName(from, to);
            return;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: componenteditor::openItemEditor()
//-----------------------------------------------------------------------------
void ComponentEditor::openItemEditor(QVector<QString> itemIdentifierChain)
{
    QModelIndex itemIndex = navigationModel_.getIndexOfItem(itemIdentifierChain);
    if (itemIndex.isValid())
    {
        onExpand(itemIndex);
        onItemActivated(itemIndex);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentEditor::getComponent()
//-----------------------------------------------------------------------------
QSharedPointer<Component> ComponentEditor::getComponent() const
{
    return component_;
}
