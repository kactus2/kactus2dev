/* 
 *  	Created on: 20.6.2012
 *      Author: Antti Kamppi
 * 		filename: interfacemodestack.cpp
 *		Project: Kactus 2
 */

#include "interfacemodestack.h"
#include "busifgeneraltab.h"

#include <editors/ComponentEditor/common/ExpressionParser.h>

//-----------------------------------------------------------------------------
// Function: interfacemodestack::InterfaceModeStack()
//-----------------------------------------------------------------------------
InterfaceModeStack::InterfaceModeStack(QSharedPointer<BusInterface> busif, QSharedPointer<Component> component,
                                       QSharedPointer<ParameterFinder> parameterFinder, LibraryInterface* handler,
                                       QSharedPointer<ExpressionParser> expressionParser, BusIfGeneralTab* parent):
QStackedWidget(parent),
busif_(busif),
parent_(parent),
mode_(busif->getInterfaceMode()),
master_(General::MASTER, busif_, component, parameterFinder, expressionParser, this),
slave_(busif, component, this),
system_(General::SYSTEM, parent, handler, busif, component, this),
mirroredMaster_(General::MIRROREDMASTER, busif, component, parameterFinder, expressionParser, this),
mirroredSlave_(busif, component, parameterFinder, expressionParser, this),
mirroredSystem_(General::MIRROREDSYSTEM, parent, handler, busif, component, this),
monitor_(busif, component, parent, handler, this)
{
	Q_ASSERT(busif_);
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

    connect(&mirroredMaster_, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(&mirroredMaster_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: interfacemodestack::~InterfaceModeStack()
//-----------------------------------------------------------------------------
InterfaceModeStack::~InterfaceModeStack()
{

}

//-----------------------------------------------------------------------------
// Function: interfacemodestack::setMode()
//-----------------------------------------------------------------------------
void InterfaceModeStack::setMode( General::InterfaceMode mode )
{
    if (mode_ == General::MIRROREDSLAVE)
    {
        mirroredSlave_.removeReferencesFromExpressions();
    }
    else if (mode_ == General::MASTER)
    {
        master_.removeReferencesFromExpressions();
    }
    else if (mode_ == General::MIRROREDMASTER)
    {
        mirroredMaster_.removeReferencesFromExpressions();
    }

	// update the current mode
	mode_ = mode;

	// update the mode also for bus interface
	busif_->setInterfaceMode(mode);

	// refresh the editor
	refresh();

	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: interfacemodestack::refresh()
//-----------------------------------------------------------------------------
void InterfaceModeStack::refresh()
{
	mode_ = busif_->getInterfaceMode();

	// select the correct editor
	setCurrentIndex(mode_);

    if (mode_ == General::MASTER)
    {
        master_.refresh();
        master_.saveModeSpecific();
    }
    else if (mode_ == General::SLAVE)
    {
        slave_.refresh();
        slave_.saveModeSpecific();

    }
    else if (mode_ == General::SYSTEM)
    {
        system_.refresh();
        system_.saveModeSpecific();

    }
    else if (mode_ == General::MIRROREDMASTER)
    {
        mirroredMaster_.refresh();
        mirroredMaster_.saveModeSpecific();

    }
    else if (mode_ == General::MIRROREDSLAVE)
    {
        mirroredSlave_.refresh();
        mirroredSlave_.saveModeSpecific();

    }
    else if (mode_ == General::MIRROREDSYSTEM)
    {
        mirroredSystem_.refresh();
        mirroredSystem_.saveModeSpecific();

    }
    else if (mode_ == General::MONITOR)
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
