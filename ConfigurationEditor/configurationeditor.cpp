/* 
 *  	Created on: 19.8.2011
 *      Author: Antti Kamppi
 * 		filename: configurationeditor.cpp
 *		Project: Kactus 2
 */

#include "configurationeditor.h"

#include <designwidget/HWDesignWidget.h>

#include <common/dialogs/createConfigurationDialog/createconfigurationdialog.h>

#include <LibraryManager/libraryinterface.h>

#include <models/component.h>
#include <models/designconfiguration.h>
#include <models/design.h>
#include <models/view.h>
#include <models/SWView.h>
#include <models/SystemView.h>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QStringList>
#include <QMessageBox>
#include <QFileInfo>
#include <QCoreApplication>

#include <QDebug>

ConfigurationEditor::ConfigurationEditor(LibraryInterface* handler, 
										 QWidget *parent):
QWidget(parent),
handler_(handler),
addNewButton_(tr("Add new\nconfiguration"), this),
removeButton_(tr("Remove\nconfiguration"), this),
configurationSelector_(this),
activeViewEditor_(this),
component_(),
designWidget_(NULL) {

	setuplayout();
	setupConnections();
	clear();
}

ConfigurationEditor::~ConfigurationEditor() {
}

void ConfigurationEditor::setuplayout() {
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

void ConfigurationEditor::setupConnections() {

	connect(&addNewButton_, SIGNAL(clicked(bool)),
		this, SLOT(onAdd()), Qt::UniqueConnection);
	connect(&removeButton_, SIGNAL(clicked(bool)),
		this, SLOT(onRemove()), Qt::UniqueConnection);
	connect(&configurationSelector_, SIGNAL(currentIndexChanged(const QString&)),
		this, SLOT(onConfigurationChanged(const QString&)), Qt::UniqueConnection);
}

void ConfigurationEditor::onAdd() {

	askSave();
	
	// create dialog to input the settings for the new configuration
	CreateConfigurationDialog dialog(handler_, component_, this);
	// if user cancels the dialog
	if (dialog.exec() == QDialog::Rejected) {
		return;
	}

	// get the info from the dialog
	QString viewName = dialog.getConfigurationName();
	VLNV configVLNV = dialog.getConfigurationVLNV();
	QString implementationRef = dialog.getImplementationViewName();

	// get the directory path to save the files into
	QString compPath = handler_->getPath(*component_->getVlnv());
	QFileInfo compInfo(compPath);
	QString dirPath = compInfo.absolutePath();

	// create the configuration
	QSharedPointer<DesignConfiguration> desConf(new DesignConfiguration(configVLNV));

	switch (dialog.designSelection()) {
		// if user wanted to use the existing design
		case CreateConfigurationDialog::USE_EXISTING: {
			// set the configuration to reference the same design.
			desConf->setDesignRef(designWidget_->getDiagram()->getDesignConfiguration()->getDesignRef());
			break;
													  }
		// if user wanted to create a new design
		case CreateConfigurationDialog::CREATE_EMPTY: {
			VLNV designVLNV = dialog.getDesignVLNV();
			desConf->setDesignRef(designVLNV);

			QSharedPointer<Design> design = QSharedPointer<Design>(new Design(designVLNV));
			handler_->writeModelToFile(dirPath, design);
			break;
													  }
		// CreateConfigurationDialog::CREATE_COPY
		default: {
			VLNV designVLNV = dialog.getDesignVLNV();
			desConf->setDesignRef(designVLNV);

			QSharedPointer<Design> design = designWidget_->getDiagram()->createDesign(designVLNV);
			handler_->writeModelToFile(dirPath, design);
			break;
				 }
	}

	handler_->writeModelToFile(dirPath, desConf);

    // create new view for the component and set it to reference to the configuration
    switch (designWidget_->getImplementation())
    {
    case KactusAttribute::KTS_HW:
        {
	        View* view = component_->createView();
	        view->setName(viewName);
	        // if user specified an implementation reference
	        if (!implementationRef.isEmpty()) {
		        view->setTopLevelView(implementationRef);
	        }
	        view->setHierarchyRef(configVLNV);
	        view->addEnvIdentifier(QString(""));
            break;
        }

    case KactusAttribute::KTS_SW:
        {
            SWView* view = component_->createSWView();
            view->setName(viewName);
            view->setHierarchyRef(configVLNV);
            break;
        }

    case KactusAttribute::KTS_SYS:
        {
            SystemView* view = component_->createSystemView();
            view->setName(viewName);
            view->setHierarchyRef(configVLNV);
        }
    }

	handler_->writeModelToFile(component_);

	// now that files have been saved tell design widget to open the new configuration.
	designWidget_->setDesign(*component_->getVlnv(), viewName);

	// set the configuration to be editable
	designWidget_->setProtection(false);

	// update this editor
	setConfiguration(designWidget_, false);

	// now theres at least two configurations so one of them can be removed.
	removeButton_.setEnabled(true);
}

void ConfigurationEditor::onRemove() {
	// there has to be at least two views to remove one
	Q_ASSERT(configurationSelector_.count() > 1);

	// get the current view that is to be removed.
	QString viewToRemove = configurationSelector_.currentText();

	QMessageBox::StandardButton answer = QMessageBox::question(this, tr("Remove the configuration"), 
		tr("Are you sure you want to remove the configuration %1?").arg(viewToRemove),
		QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Cancel);

	if (answer == QMessageBox::Cancel)
		return;
	
	// disconnect the signals from selector
	configurationSelector_.disconnect(this);

	// remove the configuration from the selector
	int index = configurationSelector_.currentIndex();
	configurationSelector_.removeItem(index);

	// get the name of the new configuration to display
	QString newView = configurationSelector_.currentText();

	// find the vlnvs of the objects to remove
	VLNV configVLNV;

    switch (designWidget_->getImplementation())
    {
    case KactusAttribute::KTS_HW:
        {
            configVLNV = component_->findView(viewToRemove)->getHierarchyRef();
            break;
        }

    case KactusAttribute::KTS_SW:
        {
            configVLNV = component_->findSWView(viewToRemove)->getHierarchyRef();
            break;
        }

    case KactusAttribute::KTS_SYS:
        {
            configVLNV = component_->findSystemView(viewToRemove)->getHierarchyRef();
            break;
        }
    }

	VLNV designVLNV;
	QSharedPointer<LibraryComponent> libComp = handler_->getModel(configVLNV);
	if (handler_->getDocumentType(configVLNV) == VLNV::DESIGNCONFIGURATION) {
		QSharedPointer<DesignConfiguration> desConf = libComp.staticCast<DesignConfiguration>();
		designVLNV = desConf->getDesignRef();
	}
	else if (handler_->getDocumentType(configVLNV) == VLNV::DESIGN) {
		designVLNV = configVLNV;
		configVLNV.clear();
	}
	libComp.clear();

	// remove the view from the component and retrieve the remaining references.
    QList<VLNV> hierRefs;

    switch (designWidget_->getImplementation())
    {
    case KactusAttribute::KTS_HW:
        {
            component_->removeView(viewToRemove);
            hierRefs = component_->getHierRefs();
            break;
        }

    case KactusAttribute::KTS_SW:
        {
            component_->removeSWView(viewToRemove);
            hierRefs = component_->getHierSWRefs();
            break;
        }

    case KactusAttribute::KTS_SYS:
        {
            component_->removeSystemView(viewToRemove);
            hierRefs = component_->getHierSystemRefs();
            break;
        }
    }
	
	// if config vlnv is still valid then it can be removed 
	bool removeConfig = configVLNV.isValid();
	bool removeDesign = true;

	foreach (VLNV ref, hierRefs) {
		
		// if theres still a reference to the configuration don't remove anything
		if (ref == configVLNV) {
			removeConfig = false;
			removeDesign = false;
			break;
		}

		// if theres reference to design then don't remove the design
		else if (ref == designVLNV) {
			removeDesign = false;
		}

		// if one of the remaining configurations references to the design
		libComp = handler_->getModel(ref);
		QSharedPointer<DesignConfiguration> desConf = libComp.dynamicCast<DesignConfiguration>();
		if (desConf && desConf->getDesignRef() == designVLNV) {
			removeDesign = false;
		}
	}

	if (removeConfig)
		handler_->removeObject(configVLNV);
	if (removeDesign)
		handler_->removeObject(designVLNV);

	// save the component 
	handler_->writeModelToFile(component_);

	designWidget_->setDesign(*component_->getVlnv(), newView);
	designWidget_->setProtection(false);
	setConfiguration(designWidget_, false);
}

void ConfigurationEditor::setConfiguration( DesignWidget* designWidget, bool locked ) {

	// if there was a previous design being displayed.
	if (designWidget_)
		designWidget_->disconnect(this);

	// connect the new design.
	designWidget_ = designWidget;
	connect(designWidget_, SIGNAL(refreshed()),
		this, SLOT(onRefresh()), Qt::UniqueConnection);
	connect(&activeViewEditor_, SIGNAL(contentChanged()),
		designWidget_, SIGNAL(contentChanged()), Qt::UniqueConnection);

	addNewButton_.setDisabled(locked);
	removeButton_.setDisabled(locked);
	configurationSelector_.setEnabled(true);

	// get the component being edited
	component_ = designWidget->getEditedComponent();

	// get the names of the hierarchical views.
	QStringList hierViewNames;
    
    switch (designWidget->getImplementation())
    {
    case KactusAttribute::KTS_HW:
        {
            hierViewNames = component_->getHierViews();
            break;
        }

    case KactusAttribute::KTS_SW:
        {
            hierViewNames = component_->getSWViewNames();
            break;
        }

    case KactusAttribute::KTS_SYS:
        {
            hierViewNames = component_->getSystemViewNames();
        }
    }
	
	// if theres only one configuration then it can't be removed.
	if (hierViewNames.size() < 2)
		removeButton_.setDisabled(true);

	// ask the active view name.
	QString activeView = designWidget->getOpenViewName();

	// find the index of the active view and set it as selected
	int activeIndex = hierViewNames.indexOf(activeView);

	// disconnect signals so the index can be changed
	configurationSelector_.disconnect(this);
	configurationSelector_.clear();
	configurationSelector_.addItems(hierViewNames);
	configurationSelector_.setCurrentIndex(activeIndex);

	// reconnect the signal
	connect(&configurationSelector_, SIGNAL(currentIndexChanged(const QString&)),
		this, SLOT(onConfigurationChanged(const QString&)), Qt::UniqueConnection);

	activeViewEditor_.setDesign(designWidget, locked);

	// display this widget
	parentWidget()->raise();
}

void ConfigurationEditor::clear() {

	if (designWidget_) {
		designWidget_->disconnect(this);
		designWidget_ = 0;
	}

	configurationSelector_.clear();
	configurationSelector_.setDisabled(true);
	
	addNewButton_.setDisabled(true);
	removeButton_.setDisabled(true);
	
	activeViewEditor_.clear();
}

void ConfigurationEditor::onRefresh() {
	setConfiguration(designWidget_, designWidget_->isProtected());
}

void ConfigurationEditor::onConfigurationChanged( const QString& newConfigName ) {

	askSave();

	if (designWidget_) {

		// ask if previous configuration was locked
		bool wasLocked = designWidget_->isProtected();

		designWidget_->setDesign(*component_->getVlnv(), newConfigName);

		// keep the locked/unlocked state from the previous configuration to this configuration.
		designWidget_->setProtection(wasLocked);
		setConfiguration(designWidget_, wasLocked);
	}
}

void ConfigurationEditor::askSave() {
	// if the current configuration is modified
	if (designWidget_ && designWidget_->isModified()) {

		// ask user if he wants to save the changes
		QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
			"Do you want to save changes to " + designWidget_->getDocumentName() + 
			"?",
			QMessageBox::Yes | QMessageBox::No, this);

		int ret = msgBox.exec();
		// if user wanted to save
		if (ret == QMessageBox::Yes)
			designWidget_->save();
	}

}

void ConfigurationEditor::setLocked( bool locked ) {
	addNewButton_.setDisabled(locked);

	// if theres only one configuration it can't be removed
	if (configurationSelector_.count() < 2)
		removeButton_.setDisabled(true);
	else
		removeButton_.setDisabled(locked);

	activeViewEditor_.setLocked(locked);
}
