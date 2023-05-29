//-----------------------------------------------------------------------------
// File: DesignValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 14.01.2016
//
// Description:
// Validator for the ipxact:design.
//-----------------------------------------------------------------------------

#include "DesignValidator.h"

#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/Design/ComponentInstance.h>
#include <IPXACTmodels/Design/AdHocConnection.h>

#include <IPXACTmodels/Design/validator/ComponentInstanceValidator.h>
#include <IPXACTmodels/Design/validator/InterconnectionValidator.h>
#include <IPXACTmodels/Design/validator/AdHocConnectionValidator.h>

#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/common/Assertion.h>

#include <IPXACTmodels/common/validators/ParameterValidator.h>
#include <IPXACTmodels/common/validators/AssertionValidator.h>

#include <KactusAPI/include/ExpressionParser.h>

#include <KactusAPI/include/LibraryInterface.h>

#include <QRegularExpression>

//-----------------------------------------------------------------------------
// Function: DesignValidator::DesignValidator()
//-----------------------------------------------------------------------------
DesignValidator::DesignValidator(QSharedPointer<ExpressionParser> parser, LibraryInterface* library):
componentInstanceValidator_(new ComponentInstanceValidator(parser, library)),
interconnectionValidator_(new InterconnectionValidator(parser, library)),
adHocConnectionValidator_(new AdHocConnectionValidator(parser, library)),
parameterValidator_(new ParameterValidator(parser, QSharedPointer<QList<QSharedPointer<Choice> > >())),
assertionValidator_(new AssertionValidator(parser))
{

}

//-----------------------------------------------------------------------------
// Function: DesignValidator::~DesignValidator()
//-----------------------------------------------------------------------------
DesignValidator::~DesignValidator()
{

}

//-----------------------------------------------------------------------------
// Function: DesignValidator::validate()
//-----------------------------------------------------------------------------
bool DesignValidator::validate(QSharedPointer<Design> design) const
{
    return hasValidVLNV(design) && hasValidComponentInstances(design) && hasValidInterconnections(design) &&
        hasValidMonitorInterconnections(design) && hasValidAdHocConnections(design) &&
        hasValidParameters(design) && hasValidAssertions(design);
}

//-----------------------------------------------------------------------------
// Function: DesignValidator::hasValidVLNV()
//-----------------------------------------------------------------------------
bool DesignValidator::hasValidVLNV(QSharedPointer<Design> design) const
{
    return design->getVlnv().getType() == VLNV::DESIGN && design->getVlnv().isValid();
}

//-----------------------------------------------------------------------------
// Function: DesignValidator::hasValidComponentInstances()
//-----------------------------------------------------------------------------
bool DesignValidator::hasValidComponentInstances(QSharedPointer<Design> design) const
{
    if (design->getComponentInstances()->isEmpty())
    {
        return true;
    }

    QVector<std::string> instanceNames;
    for (QSharedPointer<ComponentInstance> instance : *design->getComponentInstances())
    {
        if (instanceNames.contains(instance->getInstanceName()) ||
            !componentInstanceValidator_->validate(instance))
        {
            return false;
        }

        instanceNames.append(instance->getInstanceName());
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: DesignValidator::hasValidInterconnections()
//-----------------------------------------------------------------------------
bool DesignValidator::hasValidInterconnections(QSharedPointer<Design> design) const
{
    if (design->getInterconnections()->isEmpty())
    {
        return true;
    }

    interconnectionValidator_->changeComponentInstances(design->getComponentInstances());
    QVector<QString> connectionNames;

    for (QSharedPointer<Interconnection> connection : *design->getInterconnections())
    {
        if (connectionNames.contains(connection->name()) ||
            !interconnectionValidator_->validateInterconnection(connection))
        {
            return false;
        }

        connectionNames.append(connection->name());
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: DesignValidator::hasValidMonitorInterconnections()
//-----------------------------------------------------------------------------
bool DesignValidator::hasValidMonitorInterconnections(QSharedPointer<Design> design) const
{
    if (design->getMonitorInterconnecions()->isEmpty())
    {
        return true;
    }

    interconnectionValidator_->changeComponentInstances(design->getComponentInstances());
    QVector<QString> connectionNames;

    for (QSharedPointer<MonitorInterconnection> connection : *design->getMonitorInterconnecions())
    {
        if (connectionNames.contains(connection->name()) ||
            !interconnectionValidator_->validateMonitorInterconnection(connection))
        {
            return false;
        }

        connectionNames.append(connection->name());
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: DesignValidator::hasValidAdHocConenctions()
//-----------------------------------------------------------------------------
bool DesignValidator::hasValidAdHocConnections(QSharedPointer<Design> design) const
{
    if (design->getAdHocConnections()->isEmpty())
    {
        return true;
    }

    adHocConnectionValidator_->changeComponentInstances(design->getComponentInstances());
    QVector<QString> connectionNames;
    for (QSharedPointer<AdHocConnection> connection: *design->getAdHocConnections())
    {
        if (connectionNames.contains(connection->name()) || !adHocConnectionValidator_->validate(connection))
        {
            return false;
        }

        connectionNames.append(connection->name());
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: DesignValidator::hasValidParameters()
//-----------------------------------------------------------------------------
bool DesignValidator::hasValidParameters(QSharedPointer<Design> design) const
{
    if (design->getParameters()->isEmpty())
    {
        return true;
    }

    QVector<QString> parameterNames;
    for (QSharedPointer<Parameter> parameter : *design->getParameters())
    {
        if (parameterNames.contains(parameter->name()) || !parameterValidator_->validate(parameter))
        {
            return false;
        }

        parameterNames.append(parameter->name());
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: DesignValidator::hasValidAssertions()
//-----------------------------------------------------------------------------
bool DesignValidator::hasValidAssertions(QSharedPointer<Design> design) const
{
    if (design->getAssertions()->isEmpty())
    {
        return true;
    }

    QVector<QString> assertionNames;
    for (QSharedPointer<Assertion> assertion : *design->getAssertions())
    {
        if (assertionNames.contains(assertion->name()) || !assertionValidator_->validate(assertion))
        {
            return false;
        }

        assertionNames.append(assertion->name());
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: DesignValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void DesignValidator::findErrorsIn(QVector<QString>& errors, QSharedPointer<Design> design) const
{
    QString context = QObject::tr("design %1").arg(design->getVlnv().toString());

    findErrorsInVLNV(errors, design);
    findErrorsInComponentInstances(errors, design, context);
    findErrorsInInterconnections(errors, design, context);
    findErrorsInMonitorInterconnections(errors, design, context);
    findErrorsInAdHocConnections(errors, design, context);
    findErrorsInParameters(errors, design, context);
    findErrorsInAssertions(errors, design, context);
}

//-----------------------------------------------------------------------------
// Function: DesignValidator::findErrorsInVLNV()
//-----------------------------------------------------------------------------
void DesignValidator::findErrorsInVLNV(QVector<QString>& errors, QSharedPointer<Design> design) const
{
    design->getVlnv().isValid(errors, QLatin1String("design"));
}

//-----------------------------------------------------------------------------
// Function: DesignValidator::findErrorsInComponentInstances()
//-----------------------------------------------------------------------------
void DesignValidator::findErrorsInComponentInstances(QVector<QString>& errors, QSharedPointer<Design> design,
    QString const& context) const
{
    if (design->getComponentInstances()->isEmpty())
    {
        return;
    }

    QVector<std::string> instanceNames;
    QVector<std::string> duplicateNames;
    for (QSharedPointer<ComponentInstance> instance : *design->getComponentInstances())
    {
        if (instanceNames.contains(instance->getInstanceName()) &&
            !duplicateNames.contains(instance->getInstanceName()))
        {
            errors.append(QObject::tr("Component instance name '%1' within %2 is not unique.").arg(
                QString::fromStdString(instance->getInstanceName()), context));
            duplicateNames.append(instance->getInstanceName());
        }

        instanceNames.append(instance->getInstanceName());
        componentInstanceValidator_->findErrorsIn(errors, instance, context);
    }
}

//-----------------------------------------------------------------------------
// Function: DesignValidator::findErrorsInInterconnections()
//-----------------------------------------------------------------------------
void DesignValidator::findErrorsInInterconnections(QVector<QString>& errors, QSharedPointer<Design> design,
    QString const& context) const
{
    if (design->getInterconnections()->isEmpty())
    {
        return;
    }

    interconnectionValidator_->changeComponentInstances(design->getComponentInstances());
    QVector<QString> connectionNames;
    QVector<QString> duplicateNames;
    for (QSharedPointer<Interconnection> connection : *design->getInterconnections())
    {
        if (connectionNames.contains(connection->name()) && !duplicateNames.contains(connection->name()))
        {
            errors.append(QObject::tr("Interconnection name %1 within %2 is not unique")
                .arg(connection->name()).arg(context));
            duplicateNames.append(connection->name());
        }

        connectionNames.append(connection->name());
        interconnectionValidator_->findErrorsInInterconnection(errors, connection, context);
    }
}

//-----------------------------------------------------------------------------
// Function: DesignValidator::findErrorsInMonitorInterconnections()
//-----------------------------------------------------------------------------
void DesignValidator::findErrorsInMonitorInterconnections(QVector<QString>& errors, QSharedPointer<Design> design,
    QString const& context) const
{
    if (design->getMonitorInterconnecions()->isEmpty())
    {
        return;
    }

    interconnectionValidator_->changeComponentInstances(design->getComponentInstances());
    QVector<QString> connectionNames;
    QVector<QString> duplicateNames;
    for (QSharedPointer<MonitorInterconnection> connection: *design->getMonitorInterconnecions())
    {
        if (connectionNames.contains(connection->name()) && !duplicateNames.contains(connection->name()))
        {
            errors.append(QObject::tr("Monitor interconnection name %1 within %2 is not unique")
                .arg(connection->name()).arg(context));
            duplicateNames.append(connection->name());
        }

        connectionNames.append(connection->name());
        interconnectionValidator_->findErrorsInMonitorInterconnection(errors, connection, context);
    }
}

//-----------------------------------------------------------------------------
// Function: DesignValidator::findErrorsInAdHocConnections()
//-----------------------------------------------------------------------------
void DesignValidator::findErrorsInAdHocConnections(QVector<QString>& errors, QSharedPointer<Design> design,
    QString const& context) const
{
    if (design->getAdHocConnections()->isEmpty())
    {
        return;
    }

    adHocConnectionValidator_->changeComponentInstances(design->getComponentInstances());
    QVector<QString> connectionNames;
    QVector<QString> duplicateNames;
    for (QSharedPointer<AdHocConnection> connection: *design->getAdHocConnections())
    {
        if (connectionNames.contains(connection->name()) && !duplicateNames.contains(connection->name()))
        {
            errors.append(QObject::tr("Ad hoc connection name %1 within %2 is not unique")
                .arg(connection->name()).arg(context));
            duplicateNames.append(connection->name());
        }

        connectionNames.append(connection->name());
        adHocConnectionValidator_->findErrorsIn(errors, connection, context);
    }
}

//-----------------------------------------------------------------------------
// Function: DesignValidator::findErrorsInParameters()
//-----------------------------------------------------------------------------
void DesignValidator::findErrorsInParameters(QVector<QString>& errors, QSharedPointer<Design> design,
    QString const& context) const
{
    if (design->getParameters()->isEmpty())
    {
        return;
    }

    QVector<QString> parameterNames;
    QVector<QString> duplicateNames;
    for (QSharedPointer<Parameter> parameter : *design->getParameters())
    {
        if (parameterNames.contains(parameter->name()) && !duplicateNames.contains(parameter->name()))
        {
            errors.append(QObject::tr("Parameter name %1 within %2 is not unique.")
                .arg(parameter->name()).arg(context));
            duplicateNames.append(parameter->name());
        }

        parameterNames.append(parameter->name());
        parameterValidator_->findErrorsIn(errors, parameter, context);
    }
}

//-----------------------------------------------------------------------------
// Function: DesignValidator::findErrorsInAssertions()
//-----------------------------------------------------------------------------
void DesignValidator::findErrorsInAssertions(QVector<QString>& errors, QSharedPointer<Design> design,
    QString const& context) const
{
    if (design->getAssertions()->isEmpty())
    {
        return;
    }

    QVector<QString> assertionNames;
    QVector<QString> duplicateNames;
    for (QSharedPointer<Assertion> assertion : *design->getAssertions())
    {
        if (assertionNames.contains(assertion->name()) && !duplicateNames.contains(assertion->name()))
        {
            errors.append(QObject::tr("Assertion name %1 within %2 is not unique.")
                .arg(assertion->name()).arg(context));
            duplicateNames.append(assertion->name());
        }

        assertionNames.append(assertion->name());
        assertionValidator_->findErrorsIn(errors, assertion, context);
    }
}
