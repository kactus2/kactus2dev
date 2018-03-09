//-----------------------------------------------------------------------------
// File: AbstractionTypeValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 19.01.2018
//
// Description:
// Validator for the ipxact:abstractionType.
//-----------------------------------------------------------------------------

#include "AbstractionTypeValidator.h"

#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <IPXACTmodels/common/ConfigurableVLNVReference.h>

#include <IPXACTmodels/Component/AbstractionType.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/View.h>
#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/PortMap.h>
#include <IPXACTmodels/Component/validators/PortMapValidator.h>
#include <IPXACTmodels/Component/validators/MemoryReserve.h>

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>
#include <IPXACTmodels/AbstractionDefinition/PortAbstraction.h>

#include <library/LibraryInterface.h>

//-----------------------------------------------------------------------------
// Function: AbstractionTypeValidator::AbstractionTypeValidator()
//-----------------------------------------------------------------------------
AbstractionTypeValidator::AbstractionTypeValidator(QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<QList<QSharedPointer<View> > > views, QSharedPointer<PortMapValidator> portMapValidator,
    LibraryInterface* library):
library_(library),
availableViews_(views),
portMapValidator_(portMapValidator),
expressionParser_(expressionParser)
{

}

//-----------------------------------------------------------------------------
// Function: AbstractionTypeValidator::~AbstractionTypeValidator()
//-----------------------------------------------------------------------------
AbstractionTypeValidator::~AbstractionTypeValidator()
{

}

//-----------------------------------------------------------------------------
// Function: AbstractionTypeValidator::changeComponent()
//-----------------------------------------------------------------------------
void AbstractionTypeValidator::changeComponent(QSharedPointer<QList<QSharedPointer<View> > > newViews,
    QSharedPointer<QList<QSharedPointer<Port> > > newPorts)
{
    availableViews_ = newViews;

    portMapValidator_->componentChanged(newPorts);
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypeValidator::getPortMapValidator()
//-----------------------------------------------------------------------------
QSharedPointer<PortMapValidator> AbstractionTypeValidator::getPortMapValidator() const
{
    return portMapValidator_;
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypeValidator::validate()
//-----------------------------------------------------------------------------
bool AbstractionTypeValidator::validate(QSharedPointer<AbstractionType> abstraction,
    QSharedPointer<QList<QSharedPointer<AbstractionType> > > abstractionList,
    QSharedPointer<BusInterface> containingBus) const
{
    return hasValidAbstractionReference(abstraction) &&
        hasValidViewReferences(abstraction, abstractionList) &&
        hasValidPortMaps(abstraction, containingBus);
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypeValidator::hasValidAbstractionReference()
//-----------------------------------------------------------------------------
bool AbstractionTypeValidator::hasValidAbstractionReference(QSharedPointer<AbstractionType> abstraction) const
{
    if (abstraction->getAbstractionRef())
    {
        return library_->contains(*abstraction->getAbstractionRef().data()) && 
            library_->getDocumentType(*abstraction->getAbstractionRef().data()) == VLNV::ABSTRACTIONDEFINITION;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypeValidator::hasValidViewReferences()
//-----------------------------------------------------------------------------
bool AbstractionTypeValidator::hasValidViewReferences(QSharedPointer<AbstractionType> abstraction,
    QSharedPointer<QList<QSharedPointer<AbstractionType> > > abstractionList) const
{
    if (abstraction->getViewReferences()->isEmpty() && abstractionList->size() > 1)
    {
        return false;
    }

    else
    {
        for (int i = 0; i < abstraction->getViewReferences()->size(); ++i)
        {
            QString view = abstraction->getViewReferences()->at(i);
            if (view.isEmpty() == abstraction->getViewReferences()->size() == 1)
            {
                return true;
            }
            if (referencedViewIsValid(view, abstractionList) == false)
            {
                return false;
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypeValidator::referencedViewIsValid()
//-----------------------------------------------------------------------------
bool AbstractionTypeValidator::referencedViewIsValid(QString const& viewReference,
    QSharedPointer<QList<QSharedPointer<AbstractionType> > > abstractionList) const
{
    return !viewReference.isEmpty() && referencedViewExists(viewReference) &&
        viewIsReferencedOnce(viewReference, abstractionList);
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypeValidator::referencedViewExists()
//-----------------------------------------------------------------------------
bool AbstractionTypeValidator::referencedViewExists(QString const& viewReference) const
{
    foreach (QSharedPointer<View> view, *availableViews_)
    {
        if (view->name().compare(viewReference) == 0)
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypeValidator::viewIsReferencedOnce()
//-----------------------------------------------------------------------------
bool AbstractionTypeValidator::viewIsReferencedOnce(QString const& viewReference,
    QSharedPointer<QList<QSharedPointer<AbstractionType> > > abstractionList) const
{
    foreach (QSharedPointer<AbstractionType> abstraction, *abstractionList)
    {
        for (int i = 0; i < abstraction->getViewReferences()->size(); ++i)
        {
            QString abstractionView = abstraction->getViewReferences()->at(i);
            if (abstractionView != viewReference && QString::compare(abstractionView, viewReference) == 0)
            {
                return false;
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypeValidator::hasValidPortMaps()
//-----------------------------------------------------------------------------
bool AbstractionTypeValidator::hasValidPortMaps(QSharedPointer<AbstractionType> abstraction,
    QSharedPointer<BusInterface> containingBus) const
{
    if (abstraction->getAbstractionRef())
    {
        QSharedPointer<AbstractionDefinition const> abstractionDefinition =
            getAbstractionDefinition(abstraction->getAbstractionRef());

        if (!requiredPortAbstractionsAreMapped(containingBus, abstractionDefinition, abstraction->getPortMaps()))
        {
            return false;
        }
        else if (!abstraction->getPortMaps()->isEmpty())
        {
            portMapValidator_->busInterfaceChanged(
                abstraction->getAbstractionRef(), containingBus->getInterfaceMode(), containingBus->getSystem());

            MemoryReserve reservedArea;
            foreach (QSharedPointer<PortMap> portMap, *abstraction->getPortMaps())
            {
                if (abstractionDefinition && portMap->getLogicalPort() &&
                    !logicalPortHasValidPresence(abstractionDefinition, portMap->getLogicalPort()->name_,
                    containingBus->getInterfaceMode(), containingBus->getSystem()))
                {
                    return false;
                }

                if (!portMapValidator_->validate(portMap))
                {
                    return false;
                }

                qint64 logicalAreaBegin = 0;
                qint64 LogicalAreaEnd = 0;

                if (portMap->getLogicalPort()->range_)
                {
                    logicalAreaBegin = expressionParser_->parseExpression(
                        portMap->getLogicalPort()->range_->getLeft()).toLongLong();
                    LogicalAreaEnd = expressionParser_->parseExpression(
                        portMap->getLogicalPort()->range_->getRight()).toLongLong();
                }

                if (LogicalAreaEnd < logicalAreaBegin)
                {
                    int temporary = logicalAreaBegin;
                    logicalAreaBegin = LogicalAreaEnd;
                    LogicalAreaEnd = temporary;
                }

                reservedArea.addArea(portMap->getLogicalPort()->name_, logicalAreaBegin, LogicalAreaEnd);
            }

            return !reservedArea.hasIdDependantOverlap();
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypeValidator::getAbstractionDefinition()
//-----------------------------------------------------------------------------
QSharedPointer<AbstractionDefinition const> AbstractionTypeValidator::getAbstractionDefinition(
    QSharedPointer<ConfigurableVLNVReference> abstractionReference) const
{
    QSharedPointer<AbstractionDefinition const> abstractionDefinition;

    if (abstractionReference)
    {
        QSharedPointer<Document const> abstractionDocument =
            library_->getModelReadOnly(*abstractionReference.data());
        abstractionDefinition = abstractionDocument.dynamicCast<AbstractionDefinition const>();
    }

    return abstractionDefinition;
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypeValidator::requiredPortAbstractionsAreMapped()
//-----------------------------------------------------------------------------
bool AbstractionTypeValidator::requiredPortAbstractionsAreMapped(QSharedPointer<BusInterface> containingBus,
    QSharedPointer<AbstractionDefinition const> abstractionDefinition,
    QSharedPointer<QList<QSharedPointer<PortMap> > > portMaps) const
{
    if (abstractionDefinition && abstractionDefinition->getLogicalPorts())
    {
        foreach (QSharedPointer<PortAbstraction> logicalPort, *abstractionDefinition->getLogicalPorts())
        {
            if (logicalPort->hasMode(containingBus->getInterfaceMode(), containingBus->getSystem()) &&
                logicalPort->getPresence(
                containingBus->getInterfaceMode(), containingBus->getSystem()) == PresenceTypes::REQUIRED &&
                !logicalPortHasReferencingPortMaps(logicalPort->name(), portMaps))
            {
                return false;
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypesValidator::logicalPortHasReferencingPortMaps()
//-----------------------------------------------------------------------------
bool AbstractionTypeValidator::logicalPortHasReferencingPortMaps(QString const& portName,
    QSharedPointer<QList<QSharedPointer<PortMap> > > portMaps) const
{
    foreach (QSharedPointer<PortMap> currentMap, *portMaps)
    {
        if (currentMap->getLogicalPort() && currentMap->getLogicalPort()->name_ == portName)
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypesValidator::logicalPortHasValidPresence()
//-----------------------------------------------------------------------------
bool AbstractionTypeValidator::logicalPortHasValidPresence(
    QSharedPointer<AbstractionDefinition const> abstractionDefinition, QString const& logicalPortName,
    General::InterfaceMode mode, QString const& systemGroup) const
{
    QSharedPointer<PortAbstraction> logicalPort = getLogicalPort(logicalPortName, abstractionDefinition);

    if (logicalPort && logicalPort->getPresence(mode, systemGroup) == PresenceTypes::ILLEGAL)
    {
        return false;
    }

    return true;
}


//-----------------------------------------------------------------------------
// Function: AbstractionTypesValidator::getLogicalPort()
//-----------------------------------------------------------------------------
QSharedPointer<PortAbstraction> AbstractionTypeValidator::getLogicalPort(QString const& portName,
    QSharedPointer<AbstractionDefinition const> abstractionDefinition) const
{
    if (abstractionDefinition)
    {
        foreach (QSharedPointer<PortAbstraction> definitionPort, *abstractionDefinition->getLogicalPorts())
        {
            if (portName == definitionPort->getLogicalName())
            {
                return definitionPort;
            }
        }
    }

    return QSharedPointer<PortAbstraction>();
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypeValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void AbstractionTypeValidator::findErrorsIn(QVector<QString>& errors, QSharedPointer<AbstractionType> abstraction,
    QSharedPointer<QList<QSharedPointer<AbstractionType> > > abstractionList,
    QSharedPointer<BusInterface> containingBus, QString const& context) const
{
    findErrorsInReferencedViews(errors, abstraction, abstractionList, context);
    findErrorsInAbstractionReference(errors, abstraction, context);

    QString portMapContext = QObject::tr("%1 %2")
        .arg(General::interfaceMode2Str(containingBus->getInterfaceMode())).arg(context);

    findErrorsInPortMaps(errors, abstraction, containingBus, portMapContext);
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypeValidator::findErrorsInReferencedViews()
//-----------------------------------------------------------------------------
void AbstractionTypeValidator::findErrorsInReferencedViews(QVector<QString>& errors,
    QSharedPointer<AbstractionType> abstraction,
    QSharedPointer<QList<QSharedPointer<AbstractionType> > > abstractionList, QString const& context) const
{
    if (abstraction->getViewReferences()->isEmpty() && abstractionList->size() > 1)
    {
        QString newError = QObject::tr(
            "Abstraction with no view references should be the only abstraction definition in %1").arg(context);
        if (!errors.contains(newError))
        {
            errors.append(newError);
        }
    }
    else
    {
        for (int i = 0; i < abstraction->getViewReferences()->size(); ++i)
        {
            QString abstractionView = abstraction->getViewReferences()->at(i);
            if (!referencedViewExists(abstractionView))
            {
                QString newError = QObject::tr("Invalid view reference %1 set for abstraction type in %2")
                    .arg(abstractionView).arg(context);
                if (!errors.contains(newError))
                {
                    errors.append(newError);
                }
            }
            if (!viewIsReferencedOnce(abstractionView, abstractionList))
            {
                QString newError =
                    QObject::tr("View %1 has been referenced multiple times in abstraction types in %2")
                    .arg(abstractionView).arg(context);
                if (!errors.contains(newError))
                {
                    errors.append(newError);
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypeValidator::findErrorsInAbstractionReference()
//-----------------------------------------------------------------------------
void AbstractionTypeValidator::findErrorsInAbstractionReference(QVector<QString>& errors,
    QSharedPointer<AbstractionType> abstraction, QString const& context) const
{
    if (!abstraction->getAbstractionRef())
    {
        errors.append(QObject::tr("Abstraction reference must be set for each abstraction type in %1")
            .arg(context));
    }
    else
    {
        if (!library_->contains(*abstraction->getAbstractionRef().data()))
        {
            errors.append(QObject::tr("Abstraction reference %1 set for %2 could not be found in the library")
                .arg(abstraction->getAbstractionRef()->toString()).arg(context));
        }
        if (library_->getDocumentType(*abstraction->getAbstractionRef()) != VLNV::ABSTRACTIONDEFINITION)
        {
            errors.append(QObject::tr("Invalid abstraction reference set within %1").arg(context));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypeValidator::findErrorsInPortMaps()
//-----------------------------------------------------------------------------
void AbstractionTypeValidator::findErrorsInPortMaps(QVector<QString>& errors,
    QSharedPointer<AbstractionType> abstraction, QSharedPointer<BusInterface> containingBus,
    QString const& context) const
{
    if (abstraction && abstraction->getAbstractionRef())
    {
        QSharedPointer<AbstractionDefinition const> abstractionDefinition =
            getAbstractionDefinition(abstraction->getAbstractionRef());

        if (!requiredPortAbstractionsAreMapped(containingBus, abstractionDefinition, abstraction->getPortMaps()))
        {
            findErrorsInRequiredPortAbstractions(
                errors, context, containingBus, abstractionDefinition, abstraction->getPortMaps());
        }

        if (!abstraction->getPortMaps()->isEmpty())
        {
            portMapValidator_->busInterfaceChanged(
                abstraction->getAbstractionRef(), containingBus->getInterfaceMode(), containingBus->getSystem());

            QStringList portNames;

            MemoryReserve logicalReservedArea;

            foreach (QSharedPointer<PortMap> portMap, *abstraction->getPortMaps())
            {
                if (abstractionDefinition && portMap->getLogicalPort() &&
                    !logicalPortHasValidPresence(abstractionDefinition, portMap->getLogicalPort()->name_,
                    containingBus->getInterfaceMode(), containingBus->getSystem()) &&
                    !portNames.contains(portMap->getLogicalPort()->name_))
                {
                    errors.append(QObject::tr("Logical port %1 with presence 'ILLEGAL' mapped within %2.")
                        .arg(portMap->getLogicalPort()->name_)
                        .arg(context));

                    portNames.append(portMap->getLogicalPort()->name_);
                }

                portMapValidator_->findErrorsIn(errors, portMap, context);

                if (portMap->getLogicalPort())
                {
                    qint64 logicalAreaBegin = 0;
                    qint64 LogicalAreaEnd = 0;

                    if (portMap->getLogicalPort()->range_)
                    {
                        logicalAreaBegin = expressionParser_->parseExpression(
                            portMap->getLogicalPort()->range_->getLeft()).toLongLong();
                        LogicalAreaEnd = expressionParser_->parseExpression(
                            portMap->getLogicalPort()->range_->getRight()).toLongLong();
                    }
                    if (LogicalAreaEnd < logicalAreaBegin)
                    {
                        int temporary = logicalAreaBegin;
                        logicalAreaBegin = LogicalAreaEnd;
                        LogicalAreaEnd = temporary;
                    }

                    logicalReservedArea.addArea(portMap->getLogicalPort()->name_, logicalAreaBegin, LogicalAreaEnd);
                }
            }
            logicalReservedArea.findErrorsInIdDependantOverlap(errors, QLatin1String("logical port"), context);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypeValidator::findErrorsInRequiredPortAbstractions()
//-----------------------------------------------------------------------------
void AbstractionTypeValidator::findErrorsInRequiredPortAbstractions(QVector<QString>& errors,
    QString const& context, QSharedPointer<BusInterface> busInterface,
    QSharedPointer<AbstractionDefinition const> abstractionDefinition,
    QSharedPointer<QList<QSharedPointer<PortMap> > > portMaps) const
{
    if (!requiredPortAbstractionsAreMapped(busInterface, abstractionDefinition, portMaps))
    {
        foreach (QSharedPointer<PortAbstraction> logicalPort, *abstractionDefinition->getLogicalPorts())
        {
            if (logicalPort->hasMode(busInterface->getInterfaceMode(), busInterface->getSystem()) &&
                logicalPort->getPresence(busInterface->getInterfaceMode(), busInterface->getSystem())
                == PresenceTypes::REQUIRED && !logicalPortHasReferencingPortMaps(logicalPort->name(), portMaps))
            {
                errors.append(QObject::tr("Logical port %1 with presence 'REQUIRED' is not mapped within %2.")
                    .arg(logicalPort->name())
                    .arg(context));
            }
        }
    }
}