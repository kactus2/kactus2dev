//-----------------------------------------------------------------------------
// File: VHDLGenerator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 13.11.2017
//
// Description:
// Kactus2 plugin for VHDL file generation.
//-----------------------------------------------------------------------------

#include "VHDLGenerator.h"

#include "vhdlgenerator2.h"
#include "VHDLGeneratorDialog.h"

#include <library/LibraryInterface.h>

#include <IPXACTmodels/Component/Component.h>

#include <Plugins/PluginSystem/GeneratorPlugin/ViewSelection.h>
#include <Plugins/PluginSystem/GeneratorPlugin/GenerationControl.h>

#include <editors/ComponentEditor/common/ComponentParameterFinder.h>
#include <editors/ComponentEditor/common/IPXactSystemVerilogParser.h>

#include <QCoreApplication>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>

//-----------------------------------------------------------------------------
// Function: VHDLGeneratorPlugin::VHDLGeneratorPlugin()
//-----------------------------------------------------------------------------
VHDLGeneratorPlugin::VHDLGeneratorPlugin() : QObject(0)
{

}

//-----------------------------------------------------------------------------
// Function: VHDLGeneratorPlugin::VHDLGeneratorPlugin()
//-----------------------------------------------------------------------------
VHDLGeneratorPlugin::~VHDLGeneratorPlugin()
{

}

//-----------------------------------------------------------------------------
// Function: VHDLGeneratorPlugin::getName()
//----------------------------------------------------------------------------
QString VHDLGeneratorPlugin::getName() const
{
    return "VHDL Generator";
}

//-----------------------------------------------------------------------------
// Function: VHDLGeneratorPlugin::getVersion()
//-----------------------------------------------------------------------------
QString VHDLGeneratorPlugin::getVersion() const
{
    return "1.0";
}

//-----------------------------------------------------------------------------
// Function: VHDLGeneratorPlugin::getDescription()
//-----------------------------------------------------------------------------
QString VHDLGeneratorPlugin::getDescription() const
{
    return "Generates a VHDL entity for a component or a structural description of a HW design.";
}

//-----------------------------------------------------------------------------
// Function: VHDLGeneratorPlugin::getVendor()
//-----------------------------------------------------------------------------
QString VHDLGeneratorPlugin::getVendor() const
{
    return tr("tut.fi");
}

//-----------------------------------------------------------------------------
// Function: VHDLGeneratorPlugin::getLicence()
//-----------------------------------------------------------------------------
QString VHDLGeneratorPlugin::getLicence() const
{
    return tr("GPL2");
}

//-----------------------------------------------------------------------------
// Function: VHDLGeneratorPlugin::getLicenceHolder()
//-----------------------------------------------------------------------------
QString VHDLGeneratorPlugin::getLicenceHolder() const
{
    return tr("Public");
}

//-----------------------------------------------------------------------------
// Function: VHDLGeneratorPlugin::getSettingsWidget()
//-----------------------------------------------------------------------------
QWidget* VHDLGeneratorPlugin::getSettingsWidget()
{
    return new QWidget();
}

//-----------------------------------------------------------------------------
// Function: VHDLGeneratorPlugin::getSettingsModel()
//-----------------------------------------------------------------------------
PluginSettingsModel* VHDLGeneratorPlugin::getSettingsModel()
{
    return 0;
}

//-----------------------------------------------------------------------------
// Function: VHDLGeneratorPlugin::getIcon()
//-----------------------------------------------------------------------------
QIcon VHDLGeneratorPlugin::getIcon() const
{
    return QIcon(":icons/VHDLGenerator.png");
}

//-----------------------------------------------------------------------------
// Function: VHDLGeneratorPlugin::checkGeneratorSupport()
//-----------------------------------------------------------------------------
bool VHDLGeneratorPlugin::checkGeneratorSupport(QSharedPointer<Component const> component,
    QSharedPointer<Design const> design,
    QSharedPointer<DesignConfiguration const> designConfiguration) const
{
    return component && component->getImplementation() == KactusAttribute::HW;
}

//-----------------------------------------------------------------------------
// Function: VHDLGeneratorPlugin::runGenerator()
//-----------------------------------------------------------------------------
void VHDLGeneratorPlugin::runGenerator(IPluginUtility* utility, 
    QSharedPointer<Component> component,
    QSharedPointer<Design> design,
    QSharedPointer<DesignConfiguration> designConfiguration)
{
    utility_ = utility;

    QSharedPointer<ViewSelection> viewSettings(new ViewSelection(QStringLiteral("vhdl"), true,
        QString(), new GenerationSettings(), component->getViews(), component->getComponentInstantiations(), component->getFileSets()));

    QFileInfo targetInfo(utility_->getLibraryInterface()->getPath(component->getVlnv()));

    QString fileName = targetInfo.absolutePath()+ "/" + findEntityName(component, viewSettings->getViewName()) +".vhd";


    VHDLGeneratorDialog dialog(viewSettings, fileName, utility->getParentWidget());

    // if user clicks cancel then nothing is created
    if (dialog.exec() != QDialog::Accepted)
    {
        return;
    }


    QSharedPointer<ParameterFinder> finder(new ComponentParameterFinder(component));
    QSharedPointer<ExpressionParser> expressionParser(new IPXactSystemVerilogParser(finder));

    VhdlGenerator2 vhdlGen(expressionParser, utility_->getLibraryInterface(), this);

    connect(&vhdlGen, SIGNAL(errorMessage(const QString&)),
        this, SLOT(errorMessage(const QString&)), Qt::UniqueConnection);
    connect(&vhdlGen, SIGNAL(noticeMessage(const QString&)),
        this, SLOT(noticeMessage(const QString&)), Qt::UniqueConnection);

    // if errors are detected during parsing
    if (!vhdlGen.parse(component, viewSettings->getViewName()))
    {
        return;
    }

    QString path = dialog.getOutputPath();

    // generate the vhdl code
    vhdlGen.generate(path);

    // check if the file already exists in the metadata
    QString basePath = utility_->getLibraryInterface()->getPath(component->getVlnv());
    QString relativePath = General::getRelativePath(basePath, path);
    if (viewSettings->getSaveToFileset())
    {
        vhdlGen.addRTLView(viewSettings->getFileSetName(), path);
        utility_->getLibraryInterface()->writeModelToFile(component);
    }

    // Inform when done.
    utility->printInfo( "VHDL generation complete." );
}

//-----------------------------------------------------------------------------
// Function: VHDLGeneratorPlugin::getProgramRequirements()
//-----------------------------------------------------------------------------
QList<IPlugin::ExternalProgramRequirement> VHDLGeneratorPlugin::getProgramRequirements()
{
    return QList<IPlugin::ExternalProgramRequirement>();
}

//-----------------------------------------------------------------------------
// Function: VHDLGeneratorPlugin::noticeMessage()
//-----------------------------------------------------------------------------
void VHDLGeneratorPlugin::noticeMessage(QString const& message)
{
    utility_->printInfo(message);
}

//-----------------------------------------------------------------------------
// Function: VHDLGeneratorPlugin::errorMessage()
//-----------------------------------------------------------------------------
void VHDLGeneratorPlugin::errorMessage(QString const& message)
{
    utility_->printError(message);
}

//-----------------------------------------------------------------------------
// Function: VHDLGeneratorPlugin::findEntityName()
//-----------------------------------------------------------------------------
QString VHDLGeneratorPlugin::findEntityName(QSharedPointer<Component> component, QString const& viewName) const
{
    foreach (QSharedPointer<View> componentView, *component->getViews())
    {
        if (componentView->name() == viewName)
        {
            QString componentInstantiation = componentView->getComponentInstantiationRef();

            foreach (QSharedPointer<ComponentInstantiation> instantiation, 
                *component->getComponentInstantiations())
            {
                if (instantiation->name() == componentInstantiation)
                {
                    return instantiation->getModuleName();
                }
            }

        }
    }

    return component->getVlnv().getName();
}
