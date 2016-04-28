//-----------------------------------------------------------------------------
// File: ViewConfigurationValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 13.01.2016
//
// Description:
// Validator for the ipxact:viewConfiguration.
//-----------------------------------------------------------------------------

#include "ViewConfigurationValidator.h"

#include <library/LibraryManager/libraryinterface.h>

#include <IPXACTmodels/designConfiguration/ViewConfiguration.h>

#include <IPXACTmodels/Design/ComponentInstance.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/View.h>

#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <QRegularExpression>

//-----------------------------------------------------------------------------
// Function: ViewConfigurationValidator::ViewConfigurationValidator()
//-----------------------------------------------------------------------------
ViewConfigurationValidator::ViewConfigurationValidator(LibraryInterface* library,
    QSharedPointer<ExpressionParser> parser):
libraryHandler_(library),
parser_(parser),
availableViews_(),
availableInstances_()
{

}

//-----------------------------------------------------------------------------
// Function: ViewConfigurationValidator::~ViewConfigurationValidator()
//-----------------------------------------------------------------------------
ViewConfigurationValidator::~ViewConfigurationValidator()
{

}

//-----------------------------------------------------------------------------
// Function: ViewConfigurationValidator::changeComponentInstances()
//-----------------------------------------------------------------------------
void ViewConfigurationValidator::changeComponentInstances(
    QSharedPointer<QList<QSharedPointer<ComponentInstance> > > newInstances)
{
    availableInstances_ = newInstances;
}

//-----------------------------------------------------------------------------
// Function: ViewConfigurationValidator::validate()
//-----------------------------------------------------------------------------
bool ViewConfigurationValidator::validate(QSharedPointer<ViewConfiguration> configuration)
{
    return hasValidName(configuration) && hasValidIsPresent(configuration) && hasValidViewReference(configuration);
}

//-----------------------------------------------------------------------------
// Function: ViewConfigurationValidator::hasValidName()
//-----------------------------------------------------------------------------
bool ViewConfigurationValidator::hasValidName(QSharedPointer<ViewConfiguration> configuration) const
{
    if (!configuration->getInstanceName().isEmpty() && availableInstances_)
    {
        foreach (QSharedPointer<ComponentInstance> instance, *availableInstances_)
        {
            if (instance->getInstanceName() == configuration->getInstanceName())
            {
                QRegularExpression whiteSpaceExpression;
                whiteSpaceExpression.setPattern("^\\s*$");
                QRegularExpressionMatch whiteSpaceMatch =
                    whiteSpaceExpression.match(configuration->getInstanceName());

                if (configuration->getInstanceName().isEmpty() || whiteSpaceMatch.hasMatch())
                {
                    return false;
                }

                return true;
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ViewConfigurationValidator::hasValidIsPresent()
//-----------------------------------------------------------------------------
bool ViewConfigurationValidator::hasValidIsPresent(QSharedPointer<ViewConfiguration> configuration) const
{
    if (!configuration->getIsPresent().isEmpty())
    {
        QString solvedValue = parser_->parseExpression(configuration->getIsPresent());

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
// Function: ViewConfigurationValidator::hasValidViewReference()
//-----------------------------------------------------------------------------
bool ViewConfigurationValidator::hasValidViewReference(QSharedPointer<ViewConfiguration> configuration)
{
    changeAvailableViews(configuration);

    if (!configuration->getViewReference().isEmpty())
    {
        foreach (const QString& currentView, availableViews_)
        {
            if (configuration->getViewReference() == currentView)
            {
                return true;
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ViewConfigurationValidator::changeAvailableViews()
//-----------------------------------------------------------------------------
void ViewConfigurationValidator::changeAvailableViews(QSharedPointer<ViewConfiguration> configuration)
{
    if (!configuration->getInstanceName().isEmpty() && availableInstances_)
    {
        foreach (QSharedPointer<ComponentInstance> instance, *availableInstances_)
        {
            if (instance->getInstanceName() == configuration->getInstanceName())
            {
                if (instance->getComponentRef() && instance->getComponentRef()->isValid())
                {
                    QSharedPointer<Component> component =
                        libraryHandler_->getModel(*instance->getComponentRef().data()).dynamicCast<Component>();

                    if (component)
                    {
						availableViews_ = component->getViewNames();
						availableViews_.append( component->getSWViewNames() );
                        return;
                    }
                }

                break;
            }
        }
    }

    availableViews_.clear();
}

//-----------------------------------------------------------------------------
// Function: ViewConfigurationValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void ViewConfigurationValidator::findErrorsIn(QVector<QString>& errors,
    QSharedPointer<ViewConfiguration> configuration, QString const& context)
{
    findErrorsInName(errors, configuration, context);
    findErrorsInIsPresent(errors, configuration, context);
    findErrorsInViewReference(errors, configuration, context);
}

//-----------------------------------------------------------------------------
// Function: ViewConfigurationValidator::findErrorsInName()
//-----------------------------------------------------------------------------
void ViewConfigurationValidator::findErrorsInName(QVector<QString>& errors,
    QSharedPointer<ViewConfiguration> configuration, QString const& context) const
{
    if (!hasValidName(configuration))
    {
        errors.append(QObject::tr("Invalid name '%1' set for view configuration within %2")
            .arg(configuration->getInstanceName()).arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: ViewConfigurationValidator::findErrorsInIsPresent()
//-----------------------------------------------------------------------------
void ViewConfigurationValidator::findErrorsInIsPresent(QVector<QString>& errors,
    QSharedPointer<ViewConfiguration> configuration, QString const& context) const
{
    if (!hasValidIsPresent(configuration))
    {
        errors.append(QObject::tr("Invalid isPresent set for view configuration %1 within %2")
            .arg(configuration->getInstanceName()).arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: ViewConfigurationValidator::findErrorsInViewReference()
//-----------------------------------------------------------------------------
void ViewConfigurationValidator::findErrorsInViewReference(QVector<QString>& errors,
    QSharedPointer<ViewConfiguration> configuration, QString const& context)
{
    if (!hasValidViewReference(configuration))
    {
        errors.append(QObject::tr("Invalid view reference '%1' set for view configuration %2 within %3")
            .arg(configuration->getViewReference()).arg(configuration->getInstanceName()).arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: ViewConfigurationValidator::getAvailableInstances()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<ComponentInstance> > > ViewConfigurationValidator::getAvailableInstances()
    const
{
    return availableInstances_;
}

//-----------------------------------------------------------------------------
// Function: ViewConfigurationValidator::getLibraryHandler()
//-----------------------------------------------------------------------------
LibraryInterface* ViewConfigurationValidator::getLibraryHandler() const
{
    return libraryHandler_;
}