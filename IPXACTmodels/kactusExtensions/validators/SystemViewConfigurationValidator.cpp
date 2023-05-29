//-----------------------------------------------------------------------------
// File: SystemViewConfigurationValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 09.02.2016
//
// Description:
// Validator for the kactus2:SystemViewConfiguration.
//-----------------------------------------------------------------------------

#include "SystemViewConfigurationValidator.h"

#include <KactusAPI/include/LibraryInterface.h>

#include <IPXACTmodels/Design/ComponentInstance.h>

#include <IPXACTmodels/DesignConfiguration/ViewConfiguration.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/kactusExtensions/SystemView.h>

#include <QRegularExpression>

//-----------------------------------------------------------------------------
// Function: SystemViewConfigurationValidator::SystemViewConfigurationValidator()
//-----------------------------------------------------------------------------
SystemViewConfigurationValidator::SystemViewConfigurationValidator(LibraryInterface* library,
    QSharedPointer<ExpressionParser> parser):
ViewConfigurationValidator(library, parser),
availableViews_()
{

}

//-----------------------------------------------------------------------------
// Function: SystemViewConfigurationValidator::~SystemViewConfigurationValidator()
//-----------------------------------------------------------------------------
SystemViewConfigurationValidator::~SystemViewConfigurationValidator()
{

}

//-----------------------------------------------------------------------------
// Function: SystemViewConfigurationValidator::hasValidViewReference()
//-----------------------------------------------------------------------------
bool SystemViewConfigurationValidator::hasValidViewReference(QSharedPointer<ViewConfiguration> configuration)
{
    changeAvailableSystemViews(configuration);

    if (auto reference = configuration->getViewReference(); reference.empty() == false && !availableViews_.isEmpty())
    {
        return std::any_of(availableViews_.cbegin(), availableViews_.cend(),
            [&reference](auto const& view) { return reference == view.toStdString();  });
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: SystemViewConfigurationValidator::changeAvailableSystemViews()
//-----------------------------------------------------------------------------
void SystemViewConfigurationValidator::changeAvailableSystemViews(QSharedPointer<ViewConfiguration> configuration)
{
    if (configuration->getInstanceName().empty() == false && getAvailableInstances())
    {
        for (QSharedPointer<ComponentInstance> instance : *getAvailableInstances())
        {
            if (instance->getInstanceName() == configuration->getInstanceName())
            {
                if (instance->getComponentRef() && instance->getComponentRef()->isValid())
                {
                    QSharedPointer<Component> component = getLibraryHandler()->getModel(
                        *instance->getComponentRef().data()).dynamicCast<Component>();

                    if (component)
                    {
                        availableViews_ = component->getViewNames();
						availableViews_.append( component->getSystemViewNames() );
                        return;
                    }
                }

                break;
            }
        }
    }
    
    if (!availableViews_.isEmpty())
    {
        availableViews_.clear();
    }
}
