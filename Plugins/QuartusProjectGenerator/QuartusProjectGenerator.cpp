//-----------------------------------------------------------------------------
// File: QuartusProjectGenerator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 02.06.2015
//
// Description:
// The plugin for generating quartus projects containing quartus project files and quartus settings files.
//-----------------------------------------------------------------------------

#include "QuartusProjectGenerator.h"
#include "QuartusGenerator.h"

#include <library/LibraryManager/libraryinterface.h>

#include <Plugins/PluginSystem/IPluginUtility.h>

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/kactusExtensions/KactusAttribute.h>
#include <IPXACTmodels/designconfiguration.h>
#include <IPXACTmodels/design.h>
#include <IPXACTmodels/vlnv.h>

#include <QtPlugin>
#include <QDebug>
#include <QFileDialog>

//-----------------------------------------------------------------------------
// Function: QuartusProjectGenerator::QuartusProjectGenerator()
//-----------------------------------------------------------------------------
QuartusProjectGenerator::QuartusProjectGenerator():
utility_(NULL)
{

}

//-----------------------------------------------------------------------------
// Function: QuartusProjectGenerator::~QuartusProjectGenerator()
//-----------------------------------------------------------------------------
QuartusProjectGenerator::~QuartusProjectGenerator()
{

}

//-----------------------------------------------------------------------------
// Function: QuartusProjectGenerator::getName()
//-----------------------------------------------------------------------------
QString QuartusProjectGenerator::getName() const
{
	static QString name(tr("Quartus Project Generator"));
	return name;
}

//-----------------------------------------------------------------------------
// Function: QuartusProjectGenerator::getVersion()
//-----------------------------------------------------------------------------
QString QuartusProjectGenerator::getVersion() const
{
    static QString version(tr("1.0"));
    return version;
}

//-----------------------------------------------------------------------------
// Function: QuartusProjectGenerator::getDescription()
//-----------------------------------------------------------------------------
QString QuartusProjectGenerator::getDescription() const
{
    static QString description(tr("Generates quartus project file and quartus settings file for hw design."));
    return description;
}

//-----------------------------------------------------------------------------
// Function: QuartusProjectGenerator::getVendor()
//-----------------------------------------------------------------------------
QString QuartusProjectGenerator::getVendor() const
{
    static QString vendor(tr("TUT"));
    return vendor;
}

//-----------------------------------------------------------------------------
// Function: QuartusProjectGenerator::getLicence()
//-----------------------------------------------------------------------------
QString QuartusProjectGenerator::getLicence() const
{
    static QString licence(tr("GPL2"));
    return licence;
}

//-----------------------------------------------------------------------------
// Function: QuartusProjectGenerator::getLicenceHolder()
//-----------------------------------------------------------------------------
QString QuartusProjectGenerator::getLicenceHolder() const
{
    static QString holder(tr("Public"));
    return holder;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapHeaderGenerator::getSettingsWidget()
//-----------------------------------------------------------------------------
PluginSettingsWidget* QuartusProjectGenerator::getSettingsWidget()
{
    return new PluginSettingsWidget();
}


//-----------------------------------------------------------------------------
// Function: MemoryMapHeaderGenerator::getIcon()
//-----------------------------------------------------------------------------
QIcon QuartusProjectGenerator::getIcon() const
{
    return QIcon(":icons/quartus_generator.png");
}

//-----------------------------------------------------------------------------
// Function: QuartusProjectGenerator::checkGeneratorSupport()
//-----------------------------------------------------------------------------
bool QuartusProjectGenerator::checkGeneratorSupport( QSharedPointer<LibraryComponent const> libComp,
    QSharedPointer<LibraryComponent const> libDesConf /*= QSharedPointer<LibraryComponent const>()*/,
    QSharedPointer<LibraryComponent const> libDes /*= QSharedPointer<LibraryComponent const>()*/ ) const
{
    QSharedPointer<Component const> component = libComp.staticCast<Component const>();

    if (component && component->getComponentImplementation() == KactusAttribute::HW)
    {
        QSharedPointer<DesignConfiguration const> desConf = libDesConf.staticCast<DesignConfiguration const>();
        QSharedPointer<Design const> design = libDes.staticCast<Design const>();

        if ((desConf && desConf->getDesignConfigImplementation() == KactusAttribute::HW) ||
            (design && design->getDesignImplementation() == KactusAttribute::HW))
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: QuartusProjectGenerator::runGenerator()
//-----------------------------------------------------------------------------
void QuartusProjectGenerator::runGenerator(IPluginUtility* utility,
    QSharedPointer<LibraryComponent> libComp,
    QSharedPointer<LibraryComponent> libDesConf,
    QSharedPointer<LibraryComponent> libDes)
{
    utility->printInfo(tr("Running %1 %2.").arg(getName(), getVersion()));

    utility_ = utility;

    QSharedPointer<Component> component = libComp.staticCast<Component>();

    QString path = QFileDialog::getExistingDirectory(utility->getParentWidget(),
        tr("Set the directory where the Quartus project is created to"),
        utility->getLibraryInterface()->getPath(*component->getVlnv()));

    if (!path.isEmpty())
    {
        QString openViewName = getOpenViewName(libDesConf, libDes, component);

        QuartusGenerator quartusGenerator(utility->getLibraryInterface(), utility->getParentWidget());

        connect(&quartusGenerator, SIGNAL(errorMessage(QString const&)),
            this, SLOT(onErrorMessage(QString const&)), Qt::UniqueConnection);
        connect(&quartusGenerator, SIGNAL(noticeMessage(QString const&)),
            this, SLOT(onNoticeMessage(QString const&)), Qt::UniqueConnection);

        QString generatorInformation = getName() + " " + getVersion();

        quartusGenerator.readExistingPinMap(component);
        quartusGenerator.parseFiles(component, openViewName);
        quartusGenerator.generateProject(path, component->getVlnv()->getName(), generatorInformation);

        utility->printInfo(tr("Quartus project generation complete."));
    }
    else
    {
        utility->printInfo(tr("Generation aborted."));
    }
}

//-----------------------------------------------------------------------------
// Function: QuartusProjectGenerator::getProgramRequirements()
//-----------------------------------------------------------------------------
QList<IPlugin::ExternalProgramRequirement> QuartusProjectGenerator::getProgramRequirements()
{
    return QList<IPlugin::ExternalProgramRequirement>();
}

//-----------------------------------------------------------------------------
// Function: QuartusProjectGenerator::onErrorMessage()
//-----------------------------------------------------------------------------
void QuartusProjectGenerator::onErrorMessage(QString const& message)
{
    utility_->printError(message);
}

//-----------------------------------------------------------------------------
// Function: QuartusProjectGenerator::onNoticeMessage()
//-----------------------------------------------------------------------------
void QuartusProjectGenerator::onNoticeMessage(QString const& message)
{
    utility_->printInfo(message);
}

//-----------------------------------------------------------------------------
// Function: QuartusProjectGenerator::getOpenViewName()
//-----------------------------------------------------------------------------
QString QuartusProjectGenerator::getOpenViewName(QSharedPointer<LibraryComponent> libDesConf,
    QSharedPointer<LibraryComponent> libDes, QSharedPointer<Component> component)
{
    QSharedPointer<DesignConfiguration> desConf = libDesConf.staticCast<DesignConfiguration>();
    QSharedPointer<Design> design = libDes.staticCast<Design>();

    foreach (QSharedPointer<View> currentView, component->getViews())
    {
        if ((desConf && currentView->getHierarchyRef() == *desConf->getVlnv()) ||
            (design && currentView->getHierarchyRef() == *design->getVlnv()))
        {
            return currentView->getName();
        }
    }

    return QString("");
}
