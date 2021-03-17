//-----------------------------------------------------------------------------
// File: interfacemodestack.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 20.06.2012
//
// Description:
// Container for editor on the general tab of a bus interface editor.
//-----------------------------------------------------------------------------

#include "interfacemodestack.h"

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Component.h>

#include <editors/ComponentEditor/common/ExpressionParser.h>
#include <editors/ComponentEditor/busInterfaces/interfaces/BusInterfaceInterface.h>

//-----------------------------------------------------------------------------
// Function: interfacemodestack::InterfaceModeStack()
//-----------------------------------------------------------------------------
InterfaceModeStack::InterfaceModeStack(BusInterfaceInterface* busInterface, std::string const& busName,
    QSharedPointer<Component> component, QSharedPointer<ParameterFinder> parameterFinder,
    LibraryInterface* handler, QSharedPointer<ExpressionParser> expressionParser, QWidget* parent):
QStackedWidget(parent),
busInterface_(busInterface),
busName_(busName),
master_(General::MASTER, busInterface_, busName_, component, parameterFinder, expressionParser, this),
slave_(busInterface_, busName_, this),
system_(busInterface_, busName_, handler, this),
mirroredMaster_(busInterface_, busName_, this),
mirroredSlave_(busInterface_, busName_, parameterFinder, expressionParser, this),
mirroredSystem_(busInterface_, busName_, handler, this),
monitor_(busInterface_, busName_, handler, this)
{
    Q_ASSERT(component);
    Q_ASSERT(parent);

    insertWidget(General::MASTER, &master_);
	insertWidget(General::SLAVE, &slave_);
	insertWidget(General::SYSTEM, &system_);
	insertWidget(General::MIRROREDMASTER, &mirroredMaster_);
	insertWidget(General::MIRROREDSLAVE, &mirroredSlave_);
	insertWidget(General::MIRROREDSYSTEM, &mirroredSystem_);
	insertWidget(General::MONITOR, &monitor_);
    insertWidget(General::INTERFACE_MODE_COUNT, new QGroupBox(tr("Interface mode specific options"), this));

    connect(&master_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&slave_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&system_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&mirroredMaster_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&mirroredSlave_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&mirroredSystem_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&monitor_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    connect(&mirroredSlave_, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(&mirroredSlave_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    connect(&master_, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(&master_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: interfacemodestack::setMode()
//-----------------------------------------------------------------------------
void InterfaceModeStack::setMode( General::InterfaceMode mode )
{
    General::InterfaceMode currentMode = busInterface_->getMode(busName_);
    if (currentMode == General::MIRROREDSLAVE)
    {
        mirroredSlave_.removeReferencesFromExpressions();
    }
    else if (currentMode == General::MASTER)
    {
        master_.removeReferencesFromExpressions();
    }

    std::string newMode = General::interfaceMode2Str(mode).toStdString();

    busInterface_->setMode(busName_, newMode);

	refresh();

	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: interfacemodestack::refresh()
//-----------------------------------------------------------------------------
void InterfaceModeStack::refresh()
{
    General::InterfaceMode currentMode = busInterface_->getMode(busName_);

	// select the correct editor
	setCurrentIndex(currentMode);

    if (currentMode == General::MASTER)
    {
        master_.refresh();
        master_.saveModeSpecific();
    }
    else if (currentMode == General::SLAVE)
    {
        slave_.refresh();
        slave_.saveModeSpecific();
    }
    else if (currentMode == General::SYSTEM)
    {
        system_.refresh();
        system_.saveModeSpecific();

    }
    else if (currentMode == General::MIRROREDMASTER)
    {
        mirroredMaster_.refresh();
        mirroredMaster_.saveModeSpecific();
    }
    else if (currentMode == General::MIRROREDSLAVE)
    {
        mirroredSlave_.refresh();
        mirroredSlave_.saveModeSpecific();

    }
    else if (currentMode == General::MIRROREDSYSTEM)
    {
        mirroredSystem_.refresh();
        mirroredSystem_.saveModeSpecific();

    }
    else if (currentMode == General::MONITOR)
    {
        monitor_.refresh();
        monitor_.saveModeSpecific();
    }
    else
    {
        setCurrentIndex(General::INTERFACE_MODE_COUNT);
        master_.refresh();
        master_.saveModeSpecific();
    }
}

//-----------------------------------------------------------------------------
// Function: interfacemodestack::nameChanged()
//-----------------------------------------------------------------------------
void InterfaceModeStack::nameChanged(std::string const& newName)
{
    busName_ = newName;
    master_.changeName(newName);
    slave_.changeName(newName);
    system_.changeName(newName);
    mirroredMaster_.changeName(newName);
    mirroredSlave_.changeName(newName);
    mirroredSystem_.changeName(newName);
    monitor_.changeName(newName);
}
