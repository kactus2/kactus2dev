//-----------------------------------------------------------------------------
// File: configurationeditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 19.08.2011
//
// Description:
// Editor to edit the design configurations for component.
//-----------------------------------------------------------------------------

#include "configurationeditor.h"

#include <KactusAPI/include/VersionHelper.h>

#include <common/dialogs/createConfigurationDialog/createconfigurationdialog.h>
#include <common/widgets/vlnvDisplayer/vlnvdisplayer.h>

#include <editors/common/DesignWidget.h>
#include <editors/common/DesignDiagram.h>

#include <KactusAPI/include/LibraryInterface.h>

#include <common/widgets/ParameterGroupBox/parametergroupbox.h>

#include <KactusAPI/include/ListParameterFinder.h>
#include <KactusAPI/include/ExpressionFormatter.h>

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
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QStringList>
#include <QList>
#include <QTabWidget>

//-----------------------------------------------------------------------------
// Function: ConfigurationEditor::ConfigurationEditor()
//-----------------------------------------------------------------------------
ConfigurationEditor::ConfigurationEditor(LibraryInterface* handler, QWidget *parent):
QWidget(parent),
    library_(handler),
    addNewButton_(new QPushButton(QIcon(":icons/common/graphics/add.png"), "Add...", this)),
    configurationSelector_(new QComboBox(this)),
    configurationDisplay_(new VLNVDisplayer(this, VLNV(), true)),    
    activeViewEditor_(new ActiveViewEditor(this)),
    topComponent_(nullptr),
    designWidget_(nullptr)
{
    configurationDisplay_->setFlat(true);
    configurationDisplay_->setTitle(tr("Design Configuration VLNV"));
    configurationDisplay_->setToolTip(tr("Add new Design Configuration"));

    activeViewEditor_->setFlat(true);

	setuplayout();
	setupConnections();
	clear();
}

//-----------------------------------------------------------------------------
// Function: ConfigurationEditor::setConfiguration()
//-----------------------------------------------------------------------------
void ConfigurationEditor::setConfiguration(DesignWidget* designWidget)
{
    if (designWidget_)
    {
        designWidget_->disconnect(this);
    }

    designWidget_ = designWidget;
    topComponent_ = designWidget_->getEditedComponent();

    connect(designWidget_, SIGNAL(refreshed()),	this, SLOT(onRefresh()), Qt::UniqueConnection);
    connect(activeViewEditor_, SIGNAL(contentChanged()), 
        designWidget_, SIGNAL(contentChanged()), Qt::UniqueConnection);

    addNewButton_->setDisabled(designWidget_->isProtected());

    configurationSelector_->disconnect(this);
    configurationSelector_->clear();
    configurationSelector_->addItems(findPossibleViews());
    configurationSelector_->setCurrentText(designWidget_->getOpenViewName());

    connect(configurationSelector_, SIGNAL(currentTextChanged(QString const&)),
        this, SIGNAL(configurationChanged(QString const&)), Qt::UniqueConnection);
    connect(configurationSelector_, SIGNAL(currentTextChanged(QString const&)),
        this, SLOT(onConfigurationChanged(QString const&)), Qt::UniqueConnection);

    activeViewEditor_->setDesign(designWidget_);

    bool hasConfiguration = designWidget_->getDiagram()->getDesignConfiguration().isNull() == false;

    configurationDisplay_->setVisible(hasConfiguration);
    if (hasConfiguration)
    {
        configurationDisplay_->setVLNV(designWidget_->getDiagram()->getDesignConfiguration()->getVlnv());
    }

    // Display this widget.
    //parentWidget()->raise();
}

//-----------------------------------------------------------------------------
// Function: ConfigurationEditor::clear()
//-----------------------------------------------------------------------------
void ConfigurationEditor::clear()
{
    if (designWidget_)
    {
        designWidget_->disconnect(this);
        designWidget_ = nullptr;
    }

    configurationSelector_->clear();

    addNewButton_->setDisabled(true);

    activeViewEditor_->clear();
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
    addNewButton_->setDisabled(locked);

    activeViewEditor_->setLocked(locked);
}

//-----------------------------------------------------------------------------
// Function: ConfigurationEditor::onAdd()
//-----------------------------------------------------------------------------
void ConfigurationEditor::onAdd()
{
	askSave();
	
    const KactusAttribute::Implementation designImplementation = designWidget_->getImplementation();

	// Create dialog to input the settings for the new configuration.
	CreateConfigurationDialog dialog(library_, topComponent_, designImplementation, this);
	if (dialog.exec() == QDialog::Rejected)
    {
		return;
	}

	QApplication::setOverrideCursor(Qt::WaitCursor);

    createAndSaveConfiguration(dialog);

    createTopLevelReferenceToConfiguration(dialog.getConfigurationVLNV(), dialog.getConfigurationName(), 
        designImplementation);

	designWidget_->setDesign(topComponent_->getVlnv(), dialog.getConfigurationName());
	designWidget_->setProtection(false);

	setConfiguration(designWidget_);

    emit(designConfigurationChanged(designWidget_->getDiagram()->getDesignConfiguration()));

    QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------
// Function: ConfigurationEditor::createAndSaveConfiguration()
//-----------------------------------------------------------------------------
void ConfigurationEditor::createAndSaveConfiguration(CreateConfigurationDialog const& dialog)
{
    QString directoryPath = QFileInfo(library_->getPath(topComponent_->getVlnv())).absolutePath();

    VLNV designVLNV = dialog.getDesignVLNV();
    VLNV const& configurationVLNV = dialog.getConfigurationVLNV();

    // Create the configuration.
    QSharedPointer<DesignConfiguration> designConfiguration(new DesignConfiguration(configurationVLNV));
    designConfiguration->setDesignConfigImplementation(KactusAttribute::HW);
    designConfiguration->setVersion(VersionHelper::versionFileStr());

    // Set the configuration to reference the same design.
    if (dialog.designSelection() == CreateConfigurationDialog::USE_EXISTING)
    {        
        designConfiguration->setDesignRef(designWidget_->getDiagram()->getDesign()->getVlnv());
    }

    // Create a new design.
    else if (dialog.designSelection() == CreateConfigurationDialog::CREATE_EMPTY)
    {
        QSharedPointer<Design> design = QSharedPointer<Design>(new Design(designVLNV));
        library_->writeModelToFile(directoryPath, design);

        designConfiguration->setDesignRef(designVLNV);
    }

    // Copy the design.
    else // CreateConfigurationDialog::CREATE_COPY
    {
        QSharedPointer<Design> design = designWidget_->getDiagram()->getDesign();
        design->setVlnv(designVLNV);
        library_->writeModelToFile(directoryPath, design);

        designConfiguration->setDesignRef(designVLNV);
    }

    library_->writeModelToFile(directoryPath, designConfiguration);
}

//-----------------------------------------------------------------------------
// Function: ConfigurationEditor::createTopLevelReferenceToConfiguration()
//-----------------------------------------------------------------------------
void ConfigurationEditor::createTopLevelReferenceToConfiguration(VLNV const& configurationVLNV, 
    QString const& viewName, const KactusAttribute::Implementation designImplementation)
{    
    // create new view for the component and set it to reference to the configuration
    if (designImplementation == KactusAttribute::HW || designImplementation == KactusAttribute::SW)
    {
        QSharedPointer<View> view (new View(viewName));
        view->setDesignConfigurationInstantiationRef(viewName);
        topComponent_->getViews()->append(view);

        QSharedPointer<DesignConfigurationInstantiation> instantiation(
            new DesignConfigurationInstantiation(viewName));
        QSharedPointer<ConfigurableVLNVReference> configurationReference(
            new ConfigurableVLNVReference(configurationVLNV));
        instantiation->setDesignConfigurationReference(configurationReference);
        topComponent_->getDesignConfigurationInstantiations()->append(instantiation);
    }
    else if (designImplementation == KactusAttribute::SYSTEM)
    {
        QSharedPointer<SystemView> view (new SystemView(viewName));
        view->setHierarchyRef(configurationVLNV);

        QList<QSharedPointer<SystemView> > componentSystemViews = topComponent_->getSystemViews();
        componentSystemViews.append(view);
        topComponent_->setSystemViews(componentSystemViews);
    }

    library_->writeModelToFile(topComponent_);
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

		designWidget_->setDesign(topComponent_->getVlnv(), newConfigName);
		designWidget_->setProtection(wasLocked);

		setConfiguration(designWidget_);
    
        emit(designConfigurationChanged(designWidget_->getDiagram()->getDesignConfiguration()));
    }
}

//-----------------------------------------------------------------------------
// Function: ConfigurationEditor::findPossibleViews()
//-----------------------------------------------------------------------------
QStringList ConfigurationEditor::findPossibleViews()
{
    QStringList hierViewNames;

    const KactusAttribute::Implementation designImplementation = designWidget_->getImplementation();
    if (designImplementation == KactusAttribute::HW || designImplementation == KactusAttribute::SW)
    {
        hierViewNames = topComponent_->getHierViews();
    }
    else if (designImplementation == KactusAttribute::SYSTEM)
    {
        hierViewNames = topComponent_->getSystemViewNames();
    } 

    VLNV currentDesign = designWidget_->getIdentifyingVLNV();

    QStringList possibleViews;
    for (QString const& viewName : hierViewNames)
    {
        // the vlnv that the component references
        VLNV viewDesign;
        if (designImplementation == KactusAttribute::HW || designImplementation == KactusAttribute::SW)
        {
            viewDesign = topComponent_->getHierRef(viewName);
        }
        else if (designImplementation ==  KactusAttribute::SYSTEM)
        {
            viewDesign = topComponent_->getHierSystemRef(viewName);
        }

        if (library_->getDesignVLNV(viewDesign) == currentDesign)
        {
            possibleViews.append(viewName);
        }
    }

    return possibleViews;
}

//-----------------------------------------------------------------------------
// Function: ConfigurationEditor::setuplayout()
//-----------------------------------------------------------------------------
void ConfigurationEditor::setuplayout()
{
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(new QLabel(tr("Select configuration:")));
    buttonLayout->addWidget(configurationSelector_, 1);
    buttonLayout->addWidget(addNewButton_);

    QVBoxLayout* topLayout = new QVBoxLayout(this);    
    topLayout->addLayout(buttonLayout);
    topLayout->addWidget(configurationDisplay_);
    topLayout->addWidget(activeViewEditor_);
}

//-----------------------------------------------------------------------------
// Function: ConfigurationEditor::setupConnections()
//-----------------------------------------------------------------------------
void ConfigurationEditor::setupConnections()
{
    connect(addNewButton_, SIGNAL(clicked(bool)), this, SLOT(onAdd()), Qt::UniqueConnection);

    connect(configurationSelector_, SIGNAL(currentTextChanged(QString const&)),
        this, SIGNAL(configurationChanged(QString const&)), Qt::UniqueConnection);
    connect(configurationSelector_, SIGNAL(currentTextChanged(QString const&)),
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
			tr("Do you want to save changes to %1?").arg(designWidget_->getDocumentName()),
			QMessageBox::Yes | QMessageBox::No, this);

		if (msgBox.exec() == QMessageBox::Yes)
        {
			designWidget_->save();
        }
	}
}
