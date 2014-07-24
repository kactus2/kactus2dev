//-----------------------------------------------------------------------------
// File: HDLComponentInstance.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 14.7.2014
//
// Description:
// Base class for HDL component instances.
//-----------------------------------------------------------------------------

#include "HDLComponentInstance.h"

#include <IPXACTmodels/component.h>

#include <QSharedPointer>

//-----------------------------------------------------------------------------
// Function: HDLComponentInstance::HDLComponentInstance()
//-----------------------------------------------------------------------------
HDLComponentInstance::HDLComponentInstance(QObject* parent,
                                             QSharedPointer<Component> component,							
											 const QString& instanceName,
                                             QString const& viewName,
											 const QString& description):
HDLObject(parent, instanceName, component->getVlnv()->getName(), QString(), description), 
component_(component),
defaultPortConnections_(),
modelParameterValues_()
{
    Q_ASSERT(component_);

    // set the entity name that is used
    setType(component->getEntityName(viewName));

    // get the architecture name for this instance
    setArchitecture(component->getArchitectureName(viewName));
}

//-----------------------------------------------------------------------------
// Function: HDLComponentInstance::~HDLComponentInstance()
//-----------------------------------------------------------------------------
HDLComponentInstance::~HDLComponentInstance()
{

}

//-----------------------------------------------------------------------------
// Function: HDLComponentInstance::assignModelParameter()
//-----------------------------------------------------------------------------
void HDLComponentInstance::assignModelParameter(const QString& modelParameterName, 
    const QString& modelParameterValue)
{

    // if the generics already contains a mapping for the generic
    if (modelParameterValues_.contains(modelParameterName)) {
        QString oldValue = modelParameterValues_.value(modelParameterName);

        // print a notification to user that the previous value for the generic
        // is overwritten with new value
        emit noticeMessage(tr("The instance %1:%2 already contained generic mapping"
            " \"%3 => %4\" but \"%3 => %5\" replaced it.").arg(
            type()).arg(
            name()).arg(
            modelParameterName).arg(
            oldValue).arg(
            modelParameterValue));
    }

    modelParameterValues_.insert(modelParameterName, modelParameterValue);
}

//-----------------------------------------------------------------------------
// Function: HDLComponentInstance::interface()
//-----------------------------------------------------------------------------
QSharedPointer<BusInterface> HDLComponentInstance::interface( const QString& interfaceName ) const
{
	return component_->getBusInterface(interfaceName);
}

//-----------------------------------------------------------------------------
// Function: HDLComponentInstance::vlnv()
//-----------------------------------------------------------------------------
VLNV HDLComponentInstance::vlnv() const
{
	return *component_->getVlnv();
}

//-----------------------------------------------------------------------------
// Function: HDLComponentInstance::componentModel()
//-----------------------------------------------------------------------------
QSharedPointer<Component> HDLComponentInstance::componentModel() const
{
	return component_;
}

//-----------------------------------------------------------------------------
// Function: HDLComponentInstance::hasPort()
//-----------------------------------------------------------------------------
bool HDLComponentInstance::hasPort( const QString& portName ) const
{
	return component_->hasPort(portName);
}

//-----------------------------------------------------------------------------
// Function: HDLComponentInstance::portDirection()
//-----------------------------------------------------------------------------
General::Direction HDLComponentInstance::portDirection( const QString& portName ) const
{
	return component_->getPortDirection(portName);
}

//-----------------------------------------------------------------------------
// Function: HDLComponentInstance::getPortPhysLeftBound()
//-----------------------------------------------------------------------------
int HDLComponentInstance::getPortPhysLeftBound( const QString& portName ) const
{
	return component_->getPortLeftBound(portName);
}

//-----------------------------------------------------------------------------
// Function: HDLComponentInstance::getPortPhysRightBound()
//-----------------------------------------------------------------------------
int HDLComponentInstance::getPortPhysRightBound( const QString& portName ) const
{
	return component_->getPortRightBound(portName);
}

//-----------------------------------------------------------------------------
// Function: HDLComponentInstance::setArchitecture()
//-----------------------------------------------------------------------------
void HDLComponentInstance::setArchitecture(QString const& architecture)
{
    architecture_ = architecture;
}

//-----------------------------------------------------------------------------
// Function: HDLComponentInstance::getArchitecture()
//-----------------------------------------------------------------------------
QString HDLComponentInstance::getArchitecture() const
{
    return architecture_;
}

//-----------------------------------------------------------------------------
// Function: HDLComponentInstance::getAssignedModelParameters()
//-----------------------------------------------------------------------------
QMap<QString, QString> HDLComponentInstance::getAssignedModelParameters() const
{
    return modelParameterValues_;
}
