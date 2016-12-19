//-----------------------------------------------------------------------------
// File: configurationeditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 19.08.2011
//
// Description:
// Editor to edit the design configurations for component.
//-----------------------------------------------------------------------------

#include "configurationeditor.h"

#include <VersionHelper.h>

#include <designEditors/common/DesignWidget.h>
#include <designEditors/common/DesignDiagram.h>

#include <common/dialogs/createConfigurationDialog/createconfigurationdialog.h>

#include <library/LibraryManager/libraryinterface.h>

#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>
#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/View.h>
#include <IPXACTmodels/Component/DesignConfigurationInstantiation.h>
#include <IPXACTmodels/kactusExtensions/SystemView.h>

#include <QApplication>
#include <QCoreApplication>
#include <QFileInfo>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QStringList>
#include <QList>

//-----------------------------------------------------------------------------
// Function: ConfigurationEditor::ConfigurationEditor()
//-----------------------------------------------------------------------------
ConfigurationEditor::ConfigurationEditor(LibraryInterface* handler, QWidget *parent):
QWidget(parent),
library_(handler),
addNewButton_(tr("Add new\nconfiguration"), this),
removeButton_(tr("Remove\nconfiguration"), this),
configurationSelector_(this),
activeViewEditor_(this),
component_(),
designWidget_(0)
{
	setuplayout();
	setupConnections();
	clear();
}

//-----------------------------------------------------------------------------
// Function: ConfigurationEditor::~ConfigurationEditor()
//-----------------------------------------------------------------------------
ConfigurationEditor::~ConfigurationEditor()
{

}

//-----------------------------------------------------------------------------
// Function: ConfigurationEditor::setConfiguration()
//-----------------------------------------------------------------------------
void ConfigurationEditor::setConfiguration(DesignWidget* designWidget)
{
    // if there was a previous design being displayed.
    if (designWidget_)
    {
        designWidget_->disconnect(this);
    }

    // connect the new design.
    designWidget_ = designWidget;
    connect(designWidget_, SIGNAL(refreshed()),	this, SLOT(onRefresh()), Qt::UniqueConnection);
    connect(&activeViewEditor_, SIGNAL(contentChanged()), 
        designWidget_, SIGNAL(contentChanged()), Qt::UniqueConnection);

    addNewButton_.setDisabled(designWidget_->isProtected());
    removeButton_.setDisabled(designWidget_->isProtected());
    configurationSelector_.setEnabled(true);

    component_ = designWidget->getEditedComponent();

    QStringList hierViewNames;

    const KactusAttribute::Implementation designImplementation = designWidget->getImplementation();
    if (designImplementation == KactusAttribute::HW || designImplementation == KactusAttribute::SW)
    {
        hierViewNames = component_->getHierViews();
    }
    else if (designImplementation == KactusAttribute::SYSTEM)
    {
        hierViewNames = component_->getSystemViewNames();
    }
    else
    {
        Q_ASSERT(false);
    }

    VLNV designVLNV = designWidget_->getIdentifyingVLNV();

    // add the views that use the same design
    QStringList viewsToAdd;
    foreach (QString const& viewName, hierViewNames)
    {
        // the vlnv that the component references
        VLNV ref;
        if (designImplementation == KactusAttribute::HW || designImplementation == KactusAttribute::SW)
        {
            ref = component_->getHierRef(viewName);
        }
        else if (designImplementation ==  KactusAttribute::SYSTEM)
        {
            ref = component_->getHierSystemRef(viewName);
        }
        else
        {
            Q_ASSERT(false);
        }

        // the VLNV for the design used by the view
        VLNV referencedDesign = library_->getDesignVLNV(ref);
        if (referencedDesign == designVLNV)
        {
            viewsToAdd.append(viewName);
        }
    }

    // if theres only one configuration then it can't be removed.
    removeButton_.setDisabled(viewsToAdd.size() < 2);

    // find the index of the active view and set it as selected
    int activeIndex = viewsToAdd.indexOf(designWidget->getOpenViewName());

    // disconnect signals so the index can be changed
    configurationSelector_.disconnect(this);
    configurationSelector_.clear();
    configurationSelector_.addItems(viewsToAdd);
    configurationSelector_.setCurrentIndex(activeIndex);

    // reconnect the signal
    connect(&configurationSelector_, SIGNAL(currentIndexChanged(QString const&)),
        this, SIGNAL(configurationChanged(QString const&)), Qt::UniqueConnection);
    connect(&configurationSelector_, SIGNAL(currentIndexChanged(QString const&)),
        this, SLOT(onConfigurationChanged(QString const&)), Qt::UniqueConnection);

    activeViewEditor_.setDesign(designWidget);

    // display this widget
    parentWidget()->raise();
}

//-----------------------------------------------------------------------------
// Function: ConfigurationEditor::clear()
//-----------------------------------------------------------------------------
void ConfigurationEditor::clear()
{
    if (designWidget_)
    {
        designWidget_->disconnect(this);
        designWidget_ = 0;
    }

    configurationSelector_.clear();
    configurationSelector_.setDisabled(true);

    addNewButton_.setDisabled(true);
    removeButton_.setDisabled(true);

    activeViewEditor_.clear();
}

//-----------------------------------------------------------------------------
// Function: ConfigurationEditor::onRefresh()
//-----------------------------------------------------------------------------
void ConfigurationEditor::onRefresh()
{
    setConfiguration(designWidget_);
}

//-----------------------------------------------------------------------------
// Function: ConfigurationEditor::setLocked()
//-----------------------------------------------------------------------------
void ConfigurationEditor::setLocked(bool locked)
{
    addNewButton_.setDisabled(locked);

    // if theres only one configuration it can't be removed
    removeButton_.setDisabled(configurationSelector_.count() < 2);

    activeViewEditor_.setLocked(locked);
}

//-----------------------------------------------------------------------------
// Function: ConfigurationEditor::onAdd()
//-----------------------------------------------------------------------------
void ConfigurationEditor::onAdd()
{
	askSave();
	
    const KactusAttribute::Implementation designImplementation = designWidget_->getImplementation();

	// create dialog to input the settings for the new configuration
	CreateConfigurationDialog dialog(library_, component_, designImplementation, this);
	
	if (dialog.exec() == QDialog::Rejected)
    {
		return;
	}

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	// get the info from the dialog
	QString viewName = dialog.getConfigurationName();
	VLNV configVLNV = dialog.getConfigurationVLNV();

	// get the directory path to save the files into
	QFileInfo componentFileInfo(library_->getPath(component_->getVlnv()));
	QString directoryPath = componentFileInfo.absolutePath();

	// create the configuration
	QSharedPointer<DesignConfiguration> desConf(new DesignConfiguration(configVLNV));
    desConf->setDesignConfigImplementation(KactusAttribute::HW);
    desConf->setVersion(VersionHelper::versionFileStr());
    
    if (dialog.designSelection() == CreateConfigurationDialog::USE_EXISTING)
    {
        // set the configuration to reference the same design.
        desConf->setDesignRef(designWidget_->getDiagram()->getDesignConfiguration()->getDesignRef());
    }

    // if user wanted to create a new design
    else if (dialog.designSelection() == CreateConfigurationDialog::CREATE_EMPTY)
    {
        VLNV designVLNV = dialog.getDesignVLNV();
        desConf->setDesignRef(designVLNV);

        QSharedPointer<Design> design = QSharedPointer<Design>(new Design(designVLNV));
        library_->writeModelToFile(directoryPath, design);
    }
		
    else // CreateConfigurationDialog::CREATE_COPY
    {
        VLNV designVLNV = dialog.getDesignVLNV();
        desConf->setDesignRef(designVLNV);

        QSharedPointer<Design> design = designWidget_->getDiagram()->getDesign();
        design->setVlnv(designVLNV);
        library_->writeModelToFile(directoryPath, design);
    }

    library_->writeModelToFile(directoryPath, desConf);

    // create new view for the component and set it to reference to the configuration
    if (designImplementation == KactusAttribute::HW || designImplementation == KactusAttribute::SW)
    {
        QSharedPointer<View> view (new View(viewName));

        view->setDesignConfigurationInstantiationRef(viewName);

        QSharedPointer<ConfigurableVLNVReference> configurationReference (new ConfigurableVLNVReference(configVLNV));
        
        QSharedPointer<DesignConfigurationInstantiation> instantiation 
            (new DesignConfigurationInstantiation(viewName));
        instantiation->setDesignConfigurationReference(configurationReference);

		component_->getDesignConfigurationInstantiations()->append(instantiation);
        component_->getViews()->append(view);
    }
    else if (designImplementation == KactusAttribute::SYSTEM)
    {
        QSharedPointer<SystemView> view (new SystemView(viewName));
        view->setHierarchyRef(configVLNV);

        QList<QSharedPointer<SystemView> > componentSystemViews = component_->getSystemViews();
        componentSystemViews.append(view);
        component_->setSystemViews(componentSystemViews);
    }
    else
    {
        Q_ASSERT(false);
    }

	library_->writeModelToFile(component_);

	// now that files have been saved tell design widget to open the new configuration.
	designWidget_->setDesign(component_->getVlnv(), viewName);
	designWidget_->setProtection(false);

	// update this editor
	setConfiguration(designWidget_);

    emit(designConfigurationChanged(designWidget_->getDiagram()->getDesignConfiguration()));

	// now theres at least two configurations so one of them can be removed.
	removeButton_.setEnabled(true);

    QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------
// Function: ConfigurationEditor::onRemove()
//-----------------------------------------------------------------------------
void ConfigurationEditor::onRemove()
{
	// there has to be at least two views to remove one
	Q_ASSERT(configurationSelector_.count() > 1);

	QString viewToRemove = configurationSelector_.currentText();

	QMessageBox::StandardButton answer = QMessageBox::question(this, tr("Remove the configuration"), 
		tr("Are you sure you want to remove the configuration %1?").arg(viewToRemove),
		QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Cancel);

	if (answer == QMessageBox::Cancel)
    {
		return;
    }

	configurationSelector_.disconnect(this);
	configurationSelector_.removeItem(configurationSelector_.currentIndex());

	QString newView = configurationSelector_.currentText();

	// find the vlnvs of the objects to remove
	VLNV configVLNV;

    const KactusAttribute::Implementation designImplementation = designWidget_->getImplementation();
    if (designImplementation == KactusAttribute::HW || designImplementation == KactusAttribute::SW)
    {
        removeViewHierarchicalInstantiationRefs(viewToRemove);
    }
    else if (designImplementation ==  KactusAttribute::SYSTEM)
    {
        configVLNV = component_->findSystemView(viewToRemove)->getHierarchyRef();
    }
    else
    {
        Q_ASSERT(false);
    }

	VLNV designVLNV;
	QSharedPointer<Document> libComp = library_->getModel(configVLNV);
	if (library_->getDocumentType(configVLNV) == VLNV::DESIGNCONFIGURATION)
    {
		QSharedPointer<DesignConfiguration> desConf = libComp.staticCast<DesignConfiguration>();
		designVLNV = desConf->getDesignRef();
	}
	else if (library_->getDocumentType(configVLNV) == VLNV::DESIGN)
    {
		designVLNV = configVLNV;
		configVLNV.clear();
	}
	libComp.clear();

	// remove the view from the component and retrieve the remaining references.
    QList<VLNV> hierRefs;

    if (designImplementation == KactusAttribute::HW || designImplementation == KactusAttribute::SW)
    {
        removeViewFromComponent(viewToRemove);
        hierRefs = component_->getHierRefs();
    }
    else if (designImplementation == KactusAttribute::SYSTEM)
    {
        removeSystemViewFromComponent(viewToRemove);
        hierRefs = component_->getHierSystemRefs();
    }
    else
    {
        Q_ASSERT(false);
    }

	// if config vlnv is still valid then it can be removed 
	bool removeConfig = configVLNV.isValid();
	bool removeDesign = true;

	foreach (VLNV ref, hierRefs)
    {
		// if theres still a reference to the configuration don't remove anything
		if (ref == configVLNV)
        {
			removeConfig = false;
			removeDesign = false;
			break;
		}

		// if theres reference to design then don't remove the design
		else if (ref == designVLNV)
        {
			removeDesign = false;
		}

		// if one of the remaining configurations references to the design
		libComp = library_->getModel(ref);
		QSharedPointer<DesignConfiguration> desConf = libComp.dynamicCast<DesignConfiguration>();
		if (desConf && desConf->getDesignRef() == designVLNV)
        {
			removeDesign = false;
		}
	}

	if (removeConfig)
    {
		library_->removeObject(configVLNV);
    }

    if (removeDesign)
	{
        library_->removeObject(designVLNV);
    }

	// save the component 
	library_->writeModelToFile(component_);

    bool designWidgetIsProtected = designWidget_->isProtected();

	designWidget_->setDesign(component_->getVlnv(), newView);
	designWidget_->setProtection(false);
	setConfiguration(designWidget_);
    
    emit(designConfigurationChanged(designWidget_->getDiagram()->getDesignConfiguration()));

    if (designWidgetIsProtected)
    {
        designWidget_->setProtection(designWidgetIsProtected);
        setLocked(designWidgetIsProtected);
    }
}

//-----------------------------------------------------------------------------
// Function: ConfigurationEditor::onConfigurationChanged()
//-----------------------------------------------------------------------------
void ConfigurationEditor::onConfigurationChanged(QString const& newConfigName)
{
	askSave();

	if (designWidget_)
    {
		bool wasLocked = designWidget_->isProtected();

		designWidget_->setDesign(component_->getVlnv(), newConfigName);

		// keep the locked/unlocked state from the previous configuration to this configuration.
		designWidget_->setProtection(wasLocked);
		setConfiguration(designWidget_);

        emit(designConfigurationChanged(designWidget_->getDiagram()->getDesignConfiguration()));
    }
}

//-----------------------------------------------------------------------------
// Function: ConfigurationEditor::setuplayout()
//-----------------------------------------------------------------------------
void ConfigurationEditor::setuplayout()
{
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(&addNewButton_);
    buttonLayout->addWidget(&removeButton_);

    QFormLayout* selectorLayout = new QFormLayout();
    selectorLayout->addRow(tr("Current configuration:"), &configurationSelector_);

    QVBoxLayout* topLayout = new QVBoxLayout(this);
    topLayout->addLayout(buttonLayout);
    topLayout->addLayout(selectorLayout);
    topLayout->addWidget(&activeViewEditor_);
}

//-----------------------------------------------------------------------------
// Function: ConfigurationEditor::setupConnections()
//-----------------------------------------------------------------------------
void ConfigurationEditor::setupConnections()
{
    connect(&addNewButton_, SIGNAL(clicked(bool)), this, SLOT(onAdd()), Qt::UniqueConnection);
    connect(&removeButton_, SIGNAL(clicked(bool)), this, SLOT(onRemove()), Qt::UniqueConnection);

    connect(&configurationSelector_, SIGNAL(currentIndexChanged(QString const&)),
        this, SIGNAL(configurationChanged(QString const&)), Qt::UniqueConnection);
    connect(&configurationSelector_, SIGNAL(currentIndexChanged(QString const&)),
        this, SLOT(onConfigurationChanged(QString const&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ConfigurationEditor::askSave()
//-----------------------------------------------------------------------------
void ConfigurationEditor::askSave()
{
	if (designWidget_ && designWidget_->isModified())
    {
		QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
			"Do you want to save changes to " + designWidget_->getDocumentName() + "?",
			QMessageBox::Yes | QMessageBox::No, this);

		if (msgBox.exec() == QMessageBox::Yes)
        {
			designWidget_->save();
        }
	}
}

//-----------------------------------------------------------------------------
// Function: configurationeditor::removeComponentViewDesignInstantiationRefs()
//-----------------------------------------------------------------------------
void ConfigurationEditor::removeViewHierarchicalInstantiationRefs(QString const& viewName) const
{
    foreach (QSharedPointer<View> view, *component_->getViews())
    {
        if (view->name() == viewName)
        {
            view->setDesignInstantiationRef("");
            view->setDesignConfigurationInstantiationRef("");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: configurationeditor::removeViewFromComponent()
//-----------------------------------------------------------------------------
void ConfigurationEditor::removeViewFromComponent(QString const& viewName) const
{
    foreach (QSharedPointer<View> view, *component_->getViews())
    {
        if (view->name() == viewName)
        {
            component_->getViews()->removeOne(view);
            return;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: configurationeditor::removeSystemViewFromComponent()
//-----------------------------------------------------------------------------
void ConfigurationEditor::removeSystemViewFromComponent(QString const& viewName) const
{
    QList<QSharedPointer<SystemView> > systemViews = component_->getSystemViews();
    foreach(QSharedPointer<SystemView> view, systemViews)
    {
        if (view->name() == viewName)
        {
            systemViews.removeOne(view);
            break;
        }
    }

    if (!systemViews.isEmpty())
    {
        component_->setSystemViews(systemViews);
    }
}