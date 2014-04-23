//-----------------------------------------------------------------------------
// File: QuartusPinImportPlugin.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 19.11.2013
//
// Description:
// Kactus2 plugin for Quartus II pin import.
//-----------------------------------------------------------------------------


#include "QuartusPinImportPlugin.h"
#include "QuartusPinImportDialog.h"

#include "QuartusPinSyntax.h"

#include <common/KactusAttribute.h>
#include <models/component.h>
#include <models/generaldeclarations.h>

#include <library/LibraryManager/libraryinterface.h>

#include <Plugins/PluginSystem/IPluginUtility.h>

#include <QApplication>
#include <QFile>
#include <QFileInfo>
#include <QDate>
#include <QList>
#include <QRegExp>
#include <QString>
#include <QTextStream>
#include <QTime>
#include <QtPlugin>

//-----------------------------------------------------------------------------
// Function: QuartusPinImportPlugin::QuartusPinImportPlugin()
//-----------------------------------------------------------------------------
QuartusPinImportPlugin::QuartusPinImportPlugin()
  : messageList_()
{
}

//-----------------------------------------------------------------------------
// Function: QuartusPinImportPlugin::~QuartusPinImportPlugin()
//-----------------------------------------------------------------------------
QuartusPinImportPlugin::~QuartusPinImportPlugin()
{
}

//-----------------------------------------------------------------------------
// Function: QuartusPinImportPlugin::getName()
//-----------------------------------------------------------------------------
QString const& QuartusPinImportPlugin::getName() const
{
    static QString name(tr("Quartus II Pin Import Plugin"));
    return name;
}

//-----------------------------------------------------------------------------
// Function: QuartusPinImportPlugin::getVersion()
//-----------------------------------------------------------------------------
QString const& QuartusPinImportPlugin::getVersion() const
{
    static QString version(tr("1.0"));
    return version;
}

//-----------------------------------------------------------------------------
// Function: QuartusPinImportPlugin::getDescription()
//-----------------------------------------------------------------------------
QString const& QuartusPinImportPlugin::getDescription() const
{
    static QString description(tr("Imports ports from Quartus pin file."));
    return description;
}

//-----------------------------------------------------------------------------
// Function: QuartusPinImportPlugin::getVendor()
//-----------------------------------------------------------------------------
QString const& QuartusPinImportPlugin::getVendor() const {
    static QString vendor(tr("TUT"));
    return vendor;
}

//-----------------------------------------------------------------------------
// Function: QuartusPinImportPlugin::getLicence()
//-----------------------------------------------------------------------------
QString const& QuartusPinImportPlugin::getLicence() const {
    static QString licence(tr("GPL2"));
    return licence;
}

//-----------------------------------------------------------------------------
// Function: QuartusPinImportPlugin::getLicenceHolder()
//-----------------------------------------------------------------------------
QString const& QuartusPinImportPlugin::getLicenceHolder() const {
    static QString holder(tr("Public"));
    return holder;
}

//-----------------------------------------------------------------------------
// Function: QuartusPinImportPlugin::getSettingsWidget()
//-----------------------------------------------------------------------------
PluginSettingsWidget* QuartusPinImportPlugin::getSettingsWidget()
{
    return new PluginSettingsWidget();
}

//-----------------------------------------------------------------------------
// Function: QuartusPinImportPlugin::getProgramRequirements()
//-----------------------------------------------------------------------------
QList<IPlugin::ExternalProgramRequirement> QuartusPinImportPlugin::getProgramRequirements()
{
    return QList<IPlugin::ExternalProgramRequirement>();
}

//-----------------------------------------------------------------------------
// Function: QuartusPinImportPlugin::getIcon()
//-----------------------------------------------------------------------------
QIcon QuartusPinImportPlugin::getIcon() const
{
    return QIcon(":icons/quartuspinimport.png");
}

//-----------------------------------------------------------------------------
// Function: QuartusPinImportPlugin::checkGeneratorSupport()
//-----------------------------------------------------------------------------
bool QuartusPinImportPlugin::checkGeneratorSupport(QSharedPointer<LibraryComponent const> libComp, 
    QSharedPointer<LibraryComponent const> libDesConf /*= QSharedPointer<LibraryComponent const>()*/, 
    QSharedPointer<LibraryComponent const> libDes /*= QSharedPointer<LibraryComponent const>()*/) const
{
    // Pin import can only be run on component editor.
    if (libDesConf || libDes) {
        return false;
    }

    // Pin import can only run on HW components.
    QSharedPointer<const Component> comp = libComp.dynamicCast<const Component>();
    if (comp == 0 || comp->getComponentImplementation() != KactusAttribute::KTS_HW)
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: QuartusPinImportPlugin::runGenerator()
//-----------------------------------------------------------------------------
void QuartusPinImportPlugin::runGenerator(IPluginUtility* utility, QSharedPointer<LibraryComponent> libComp, 
    QSharedPointer<LibraryComponent> libDesConf /*= QSharedPointer<LibraryComponent>()*/, 
    QSharedPointer<LibraryComponent> libDes /*= QSharedPointer<LibraryComponent>()*/)
{
    QSharedPointer<Component> comp = libComp.dynamicCast<Component>();
    if (comp == 0)
    {
        return;
    }

    QuartusPinImportDialog dialog(comp, utility->getLibraryInterface(), utility->getParentWidget());
    connect(&dialog, SIGNAL(printMessage(QString const&)), 
        this, SLOT(logMessage(QString const&)), Qt::UniqueConnection);

    // Save changes to file, if parsing was accepted.
    if (dialog.exec() == QDialog::Accepted)
    {

        comp->getPorts().clear();

        QApplication::setOverrideCursor(Qt::WaitCursor);

        // Replace with newly generated ports.
        foreach(QSharedPointer<Port> port, dialog.getPorts())
        {
            comp->addPort(port);
        }

        utility->getLibraryInterface()->writeModelToFile(comp);
        QApplication::restoreOverrideCursor();

        printGeneratorSummary(utility);

    }
}

//-----------------------------------------------------------------------------
// Function: QuartusPinImportPlugin::logError()
//-----------------------------------------------------------------------------
void QuartusPinImportPlugin::logMessage(QString const& message)
{
    messageList_ << message;
}

//-----------------------------------------------------------------------------
// Function: QuartusPinImportPlugin::printGeneratorSummary()
//-----------------------------------------------------------------------------
void QuartusPinImportPlugin::printGeneratorSummary(IPluginUtility* utility)
{
    utility->printInfo(QString("---------- " + getName() + " " + getVersion() + " ----------"));    
    utility->printInfo(QString(tr("Generation finished ") + QDate::currentDate().toString("dd.MM.yyyy") + 
        " " + QTime::currentTime().toString()));

    foreach(QString message, messageList_)
    {
        utility->printInfo(message);
    }
}



