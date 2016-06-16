//-----------------------------------------------------------------------------
// File: ModelSimGeneratorPlugin.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 10.06.2016
//
// Description:
// ModelSim generator plugin.
//-----------------------------------------------------------------------------

#include "ModelSimGenerator.h"
#include "ModelSimGeneratorPlugin.h"

#include <QCoreApplication>
#include <QFileInfo>
#include <QMessageBox>
#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>

#include "QFileDialog"

//-----------------------------------------------------------------------------
// Function: ModelSimGeneratorPlugin::ModelSimGeneratorPlugin()
//-----------------------------------------------------------------------------
ModelSimGeneratorPlugin::ModelSimGeneratorPlugin() : QObject(0)
{

}

//-----------------------------------------------------------------------------
// Function: ModelSimGeneratorPlugin::ModelSimGeneratorPlugin()
//-----------------------------------------------------------------------------
ModelSimGeneratorPlugin::~ModelSimGeneratorPlugin()
{

}

//-----------------------------------------------------------------------------
// Function: ModelSimGeneratorPlugin::getName()
//----------------------------------------------------------------------------
QString ModelSimGeneratorPlugin::getName() const
{
    return "ModelSim Generator";
}

//-----------------------------------------------------------------------------
// Function: ModelSimGeneratorPlugin::getVersion()
//-----------------------------------------------------------------------------
QString ModelSimGeneratorPlugin::getVersion() const
{
    return "1.1";
}

//-----------------------------------------------------------------------------
// Function: ModelSimGeneratorPlugin::getDescription()
//-----------------------------------------------------------------------------
QString ModelSimGeneratorPlugin::getDescription() const
{
	return "Generates a ModelSim do file, which adds the files in design to ModelSim library and then starts "\
		 "a simulation for the design.";
}

//-----------------------------------------------------------------------------
// Function: ModelSimGeneratorPlugin::getVendor()
//-----------------------------------------------------------------------------
QString ModelSimGeneratorPlugin::getVendor() const
{
    return tr("TUT");
}

//-----------------------------------------------------------------------------
// Function: ModelSimGeneratorPlugin::getLicence()
//-----------------------------------------------------------------------------
QString ModelSimGeneratorPlugin::getLicence() const
{
    return tr("GPL2");
}

//-----------------------------------------------------------------------------
// Function: ModelSimGeneratorPlugin::getLicenceHolder()
//-----------------------------------------------------------------------------
QString ModelSimGeneratorPlugin::getLicenceHolder() const
{
    return tr("Public");
}

//-----------------------------------------------------------------------------
// Function: ModelSimGeneratorPlugin::getSettingsWidget()
//-----------------------------------------------------------------------------
PluginSettingsWidget* ModelSimGeneratorPlugin::getSettingsWidget()
{
    return new PluginSettingsWidget();
}

//-----------------------------------------------------------------------------
// Function: ModelSimGeneratorPlugin::getIcon()
//-----------------------------------------------------------------------------
QIcon ModelSimGeneratorPlugin::getIcon() const
{
    return QIcon(":icons/ModelSimGenerator.png");
}

//-----------------------------------------------------------------------------
// Function: ModelSimGeneratorPlugin::checkGeneratorSupport()
//-----------------------------------------------------------------------------
bool ModelSimGeneratorPlugin::checkGeneratorSupport( QSharedPointer<Document const> libComp,
    QSharedPointer<Document const> libDesConf,
    QSharedPointer<Document const> libDes ) const
{
	QSharedPointer<const Component> targetComponent = libComp.dynamicCast<const Component>();

	return targetComponent && targetComponent->getImplementation() == KactusAttribute::HW;
}

//-----------------------------------------------------------------------------
// Function: ModelSimGeneratorPlugin::runGenerator()
//-----------------------------------------------------------------------------
void ModelSimGeneratorPlugin::runGenerator( IPluginUtility* utility, 
    QSharedPointer<Document> libComp,
    QSharedPointer<Document> libDesConf,
    QSharedPointer<Document> libDes)
{
	// Dynamic cast to component, as it is needed that way.
	QSharedPointer<Component> topComponent = libComp.dynamicCast<Component>();
	// Dynamic cast to design, as it is needed that way.
	QSharedPointer<const Design> design = libDes.dynamicCast<Design const>();
	// Dynamic cast to design configuration, as it is needed that way.
	QSharedPointer<DesignConfiguration const> desgConf = libDesConf.dynamicCast<DesignConfiguration const>();

	// In principle, these could be null.
	if ( !topComponent || (design && !desgConf) || (!design && desgConf) )
	{
		utility->printError( "ModelSim generator received null top component or design!" );
		return;
	}

	// Must know if generating for a design.
	bool targetIsDesign = design && desgConf;

	// Inform when done.
	utility->printInfo( "ModelSim generation complete." );

	// Top component may have been affected by changes -> save.
	utility->getLibraryInterface()->writeModelToFile(libComp);

	// select a view to generate the ModelSim script for
	QSharedPointer<View> view = topComponent->getViews()->first();//ComboSelector::selectView(topComponent, utility->getParentWidget(), QString(), tr("Select a view to generate the modelsim script for"));
	if (!view)
	{
		return;
	}

	QString fileName = utility->getLibraryInterface()->getPath(topComponent->getVlnv());
	QFileInfo targetInfo(fileName);
	fileName = targetInfo.absolutePath();
	fileName += QString("/%1.%2.create_makefile").arg(topComponent->getVlnv().getName()).arg(view->name());

	// ask user to select a location to save the makefile
	fileName = QFileDialog::getSaveFileName(utility->getParentWidget(), 
		tr("Set the file name for the modelsim script."), fileName,
		tr("Modelsim scripts (*.do);;Shell Scripts (*.sh)"));

	// if user clicked cancel
	if (fileName.isEmpty())
	{
		return;
	}

	// Parse the matching file sets.
	ModelSimParser sparser(utility, utility->getLibraryInterface());
	sparser.parseFiles(topComponent, view);

	// construct the generator
	ModelSimGenerator generator(sparser,utility,utility->getLibraryInterface());
	/*connect(&generator, SIGNAL(noticeMessage(const QString&)),
		this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);
	connect(&generator, SIGNAL(errorMessage(const QString&)),
		this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);*/

	// create the script file
	generator.generateMakefile(fileName);

	// check if the file already exists in the metadata
	QString basePath = utility->getLibraryInterface()->getPath(topComponent->getVlnv());
	QString relativePath = General::getRelativePath(basePath, fileName);
	if (!topComponent->hasFile(relativePath))
	{
		// ask user if he wants to save the generated ModelSim script into 
		// object metadata
		QMessageBox::StandardButton button = QMessageBox::question(utility->getParentWidget(), 
			tr("Save generated ModelSim script to metadata?"),
			tr("Would you like to save the generated ModelSim script to IP-XACT"
			" metadata?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

		// if the generated file is saved
		if (button == QMessageBox::Yes)
		{
			QString xmlPath = utility->getLibraryInterface()->getPath(topComponent->getVlnv());

			// if the file was successfully added to the library
			if (generator.addMakefile2IPXact(topComponent, fileName, xmlPath))
			{
				utility->getLibraryInterface()->writeModelToFile(topComponent);
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Function: ModelSimGeneratorPlugin::getProgramRequirements()
//-----------------------------------------------------------------------------
QList<IPlugin::ExternalProgramRequirement> ModelSimGeneratorPlugin::getProgramRequirements()
{
    return QList<IPlugin::ExternalProgramRequirement>();
}

//-----------------------------------------------------------------------------
// Function: ModelSimGeneratorPlugin::componentModelsimGenerate()
//-----------------------------------------------------------------------------
/*bool ModelSimGeneratorPlugin::componentModelsimGenerate()
{
	if (isModified() && askSaveFile())
	{
		save();
	}

	// select a view to generate the modelsim script for
	QString viewName = ComboSelector::selectView(component_, this, QString(),
		tr("Select a view to generate the modelsim script for"));
	if (viewName.isEmpty()) {
		return false;
	}

	QString fileName = libHandler_->getPath(component_->getVlnv());
	QFileInfo targetInfo(fileName);
	fileName = targetInfo.absolutePath();
	fileName += QString("/%1.%2.create_makefile").arg(component_->getVlnv().getName()).arg(viewName);

	// ask user to select a location to save the makefile
	fileName = QFileDialog::getSaveFileName(this, 
		tr("Set the file name for the modelsim script."), fileName,
		tr("Modelsim scripts (*.do);;Shell cripts (*.sh)"));

	// if user clicked cancel
	if (fileName.isEmpty())
	{
		return false;
	}

	// construct the generator
	ModelsimGenerator generator(libHandler_, this);
	connect(&generator, SIGNAL(noticeMessage(const QString&)),
		this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);
	connect(&generator, SIGNAL(errorMessage(const QString&)),
		this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);

	// parse the component and view / sub-designs
	generator.parseFiles(component_, viewName);

	// create the script file
	generator.generateMakefile(fileName);

	// check if the file already exists in the metadata
	QString basePath = libHandler_->getPath(component_->getVlnv());
	QString relativePath = General::getRelativePath(basePath, fileName);
	if (!component_->hasFile(relativePath))
	{
		// ask user if he wants to save the generated modelsim script into 
		// object metadata
		QMessageBox::StandardButton button = QMessageBox::question(this, 
			tr("Save generated modelsim script to metadata?"),
			tr("Would you like to save the generated modelsim script to IP-Xact"
			" metadata?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

		// if the generated file is saved
		if (button == QMessageBox::Yes)
		{
			QString xmlPath = libHandler_->getPath(component_->getVlnv());

			// if the file was successfully added to the library
			if (generator.addMakefile2IPXact(component_, fileName, xmlPath))
			{
				libHandler_->writeModelToFile(component_);
				return true;
			}
		}
	}
	return false;
}

//-----------------------------------------------------------------------------
// Function: ModelSimGeneratorPlugin::designModelsimGenerate()
//-----------------------------------------------------------------------------
void ModelSimGeneratorPlugin::designModelsimGenerate()
{
	if (isModified() && askSaveFile())
	{
		save();
	}

	QString fileName = getLibraryInterface()->getPath(getEditedComponent()->getVlnv());
	QFileInfo targetInfo(fileName);
	fileName = targetInfo.absolutePath();
	fileName += QString("/%1.%2.compile.do").arg(
		getEditedComponent()->getVlnv().getName()).arg(getOpenViewName());

	// ask user to select a location to save the makefile
	fileName = QFileDialog::getSaveFileName(this, 
		tr("Set the file name for the modelsim script."), fileName,
		tr("Modelsim scripts (*.do);;Shell cripts (*.sh)"));

	// if user clicked cancel
	if (fileName.isEmpty())
	{
		return;
	}

	// construct the generator
	ModelsimGenerator generator(getLibraryInterface(), this);
	connect(&generator, SIGNAL(noticeMessage(const QString&)),
		this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);
	connect(&generator, SIGNAL(errorMessage(const QString&)),
		this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);

	// parse the getEditedComponent() and view / sub-designs
	generator.parseFiles(getEditedComponent(), getOpenViewName());

	// create the script file
	generator.generateMakefile(fileName);

	// check if the file already exists in the metadata
	QString basePath = getLibraryInterface()->getPath(getEditedComponent()->getVlnv());
	QString relativePath = General::getRelativePath(basePath, fileName);
	if (!getEditedComponent()->hasFile(relativePath))
	{
		// Ask user if he wants to save the generated modelsim script into object metadata.
		QMessageBox::StandardButton button = QMessageBox::question(this, 
			tr("Save generated modelsim script to metadata?"),
			tr("Would you like to save the generated modelsim script to IP-Xact"
			" metadata?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

		if (button == QMessageBox::Yes)
		{
			QString xmlPath = getLibraryInterface()->getPath(getEditedComponent()->getVlnv());

			// if the file was successfully added to the library
			if (generator.addMakefile2IPXact(getEditedComponent(), fileName, xmlPath))
			{
				getLibraryInterface()->writeModelToFile(getEditedComponent());
			}
		}
	}
}*/
