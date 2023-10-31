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

#include <KactusAPI/include/LibraryInterface.h>

#include <IPXACTmodels/DesignConfiguration/ViewConfiguration.h>

#include <IPXACTmodels/Design/ComponentInstance.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/View.h>

#include <IPXACTmodels/common/validators/CommonItemsValidator.h>

#include <KactusAPI/include/ExpressionParser.h>

#include <QRegularExpression>

//-----------------------------------------------------------------------------
// Function: ViewConfigurationValidator::ViewConfigurationValidator()
//-----------------------------------------------------------------------------
ViewConfigurationValidator::ViewConfigurationValidator(LibraryInterface* library,
    QSharedPointer<ExpressionParser> parser):
libraryHandler_(library),
parser_(parser)
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
    if (auto nameInConfiguration = configuration->getInstanceName(); 
        nameInConfiguration.isEmpty() == false && availableInstances_)
    {
        for (auto const& instance : *availableInstances_)
        {
            if (auto instanceName = instance->getInstanceName(); instanceName == nameInConfiguration)
            {
                return CommonItemsValidator::hasValidName(instanceName);
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
    return CommonItemsValidator::hasValidIsPresent(configuration->getIsPresent(), parser_);
}

//-----------------------------------------------------------------------------
// Function: ViewConfigurationValidator::hasValidViewReference()
//-----------------------------------------------------------------------------
bool ViewConfigurationValidator::hasValidViewReference(QSharedPointer<ViewConfiguration> configuration)
{
    changeAvailableViews(configuration);

    if (auto viewReference = configuration->getViewReference(); viewReference.isEmpty() == false)
    {
        return std::any_of(availableViews_.cbegin(), availableViews_.cend(), 
            [&viewReference](auto const& view){ return view == viewReference; });
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ViewConfigurationValidator::changeAvailableViews()
//-----------------------------------------------------------------------------
void ViewConfigurationValidator::changeAvailableViews(QSharedPointer<ViewConfiguration> configuration)
{
    if (configuration->getInstanceName().isEmpty() == false && availableInstances_)
    {
        for (auto const& instance : *availableInstances_)
        {
            if (instance->getInstanceName() == configuration->getInstanceName())
            {
                if (instance->getComponentRef() && instance->getComponentRef()->isValid())
                {
                    auto component =
                        libraryHandler_->getModelReadOnly<Component>(*instance->getComponentRef());

                    if (component)
                    {
                        availableViews_.clear();
                        for (auto const& view : component->getViewNames())
                        {
                            availableViews_.append(view);
                        }

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
            .arg(configuration->getInstanceName(), context));
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
        errors.append(QObject::tr("Invalid view reference '%1' set for view configuration %2 within %3").arg(
            configuration->getViewReference(),
            configuration->getInstanceName(),
            context));
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