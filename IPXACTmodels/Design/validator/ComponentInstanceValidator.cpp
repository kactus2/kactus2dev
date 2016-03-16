//-----------------------------------------------------------------------------
// File: ComponentInstanceValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 15.01.2016
//
// Description:
// Validator for the ipxact:componentInstance.
//-----------------------------------------------------------------------------

#include "ComponentInstanceValidator.h"

#include <IPXACTmodels/Component/Component.h>

#include <IPXACTmodels/Design/ComponentInstance.h>

#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <library/LibraryManager/libraryinterface.h>

#include <QRegularExpression>

//-----------------------------------------------------------------------------
// Function: ComponentInstanceValidator::ComponentInstanceValidator()
//-----------------------------------------------------------------------------
ComponentInstanceValidator::ComponentInstanceValidator(QSharedPointer<ExpressionParser> parser,
    LibraryInterface* library):
parser_(parser),
libraryHandler_(library)
{

}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceValidator::~ComponentInstanceValidator()
//-----------------------------------------------------------------------------
ComponentInstanceValidator::~ComponentInstanceValidator()
{

}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceValidator::validate()
//-----------------------------------------------------------------------------
bool ComponentInstanceValidator::validate(QSharedPointer<ComponentInstance> instance) const
{
    return hasValidName(instance) && hasValidIsPresent(instance) && hasValidComponentReference(instance);
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceValidator::hasValidName()
//-----------------------------------------------------------------------------
bool ComponentInstanceValidator::hasValidName(QSharedPointer<ComponentInstance> instance) const
{
    QRegularExpression whiteSpaceExpression;
    whiteSpaceExpression.setPattern("^\\s*$");
    QRegularExpressionMatch whiteSpaceMatch = whiteSpaceExpression.match(instance->getInstanceName());

    if (instance->getInstanceName().isEmpty() || whiteSpaceMatch.hasMatch())
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceValidator::hasValidIsPresent()
//-----------------------------------------------------------------------------
bool ComponentInstanceValidator::hasValidIsPresent(QSharedPointer<ComponentInstance> instance) const
{
    if (!instance->getIsPresent().isEmpty())
    {
        QString solvedValue = parser_->parseExpression(instance->getIsPresent());

        bool toIntOk = true;
        int intValue = solvedValue.toInt(&toIntOk);

        if (!toIntOk || intValue < 0 || intValue > 1)
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceValidator::hasValidComponentReference()
//-----------------------------------------------------------------------------
bool ComponentInstanceValidator::hasValidComponentReference(QSharedPointer<ComponentInstance> instance) const
{
    if (instance->getComponentRef() && instance->getComponentRef()->isValid())
    {
        QSharedPointer<Component> referencedComponent =
            libraryHandler_->getModel(*instance->getComponentRef().data()).dynamicCast<Component>();
        if (referencedComponent)
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void ComponentInstanceValidator::findErrorsIn(QVector<QString>& errors, QSharedPointer<ComponentInstance> instance,
    QString const& context) const
{
    findErrorsInName(errors, instance, context);
    findErrorsInIsPresent(errors, instance, context);
    findErrorsInComponentReference(errors, instance, context);
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceValidator::findErrorsInName()
//-----------------------------------------------------------------------------
void ComponentInstanceValidator::findErrorsInName(QVector<QString>& errors,
    QSharedPointer<ComponentInstance> instance, QString const& context) const
{
    if (!hasValidName(instance))
    {
        errors.append(QObject::tr("Invalid instance name '%1' set for component instance within %2")
            .arg(instance->getInstanceName()).arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceValidator::findErrorsInIsPresent()
//-----------------------------------------------------------------------------
void ComponentInstanceValidator::findErrorsInIsPresent(QVector<QString>& errors,
    QSharedPointer<ComponentInstance> instance, QString const& context) const
{
    if (!hasValidIsPresent(instance))
    {
        errors.append(QObject::tr("Invalid isPresent set for component instance %1 within %2")
            .arg(instance->getInstanceName()).arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceValidator::findErrorsInComponentReference()
//-----------------------------------------------------------------------------
void ComponentInstanceValidator::findErrorsInComponentReference(QVector<QString>& errors,
    QSharedPointer<ComponentInstance> instance, QString const& context) const
{
    if (!instance->getComponentRef()->isEmpty())
    {
        if (!libraryHandler_->contains(*instance->getComponentRef().data()))
        {
            errors.append(QObject::tr("Component reference %1 in component instance %2 within %3 was not found "
                "in the library")
                .arg(instance->getComponentRef()->toString()).arg(instance->getInstanceName()).arg(context));
        }

        QString instanceContext =
            QObject::tr("component reference in component instance %1").arg(instance->getInstanceName());

        instance->getComponentRef()->isValid(errors, instanceContext);
    }
    else
    {
        errors.append(QObject::tr("No component reference given in component instance %1 within %2")
            .arg(instance->getInstanceName()).arg(context));
    }
}