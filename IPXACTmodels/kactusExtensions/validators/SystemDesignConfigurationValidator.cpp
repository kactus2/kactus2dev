//-----------------------------------------------------------------------------
// File: SystemDesignConfigurationValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 09.02.2016
//
// Description:
// Validator for the kactus2:systemDesignConfiguration.
//-----------------------------------------------------------------------------

#include "SystemDesignConfigurationValidator.h"

#include <IPXACTmodels/kactusExtensions/validators/SystemViewConfigurationValidator.h>

#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>

#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/Design/ComponentInstance.h>

#include <IPXACTmodels/designConfiguration/validators/ViewConfigurationValidator.h>

#include <IPXACTmodels/kactusExtensions/SWInstance.h>

#include <library/LibraryManager/libraryinterface.h>

//-----------------------------------------------------------------------------
// Function: SystemDesignConfigurationValidator::SystemDesignConfigurationValidator()
//-----------------------------------------------------------------------------
SystemDesignConfigurationValidator::SystemDesignConfigurationValidator(QSharedPointer<ExpressionParser> parser,
    LibraryInterface* library):
DesignConfigurationValidator(parser, library),
systemViewConfigurationValidator_()
{
    systemViewConfigurationValidator_ =
        QSharedPointer<SystemViewConfigurationValidator>(new SystemViewConfigurationValidator(library, parser));
}

//-----------------------------------------------------------------------------
// Function: SystemDesignConfigurationValidator::~DesignConfigurationValidator()
//-----------------------------------------------------------------------------
SystemDesignConfigurationValidator::~SystemDesignConfigurationValidator()
{

}

//-----------------------------------------------------------------------------
// Function: SystemDesignConfigurationValidator::hasValidViewConfigurations()
//-----------------------------------------------------------------------------
bool SystemDesignConfigurationValidator::hasValidViewConfigurations(
    QSharedPointer<DesignConfiguration> designConfiguration) const
{
    if (!designConfiguration->getViewConfigurations()->isEmpty())
    {
        if (designConfiguration->getDesignRef().isValid())
        {
            QSharedPointer<Design> referencedDesign =
                getLibraryHandler()->getDesign(designConfiguration->getDesignRef());

            QSharedPointer<QList<QSharedPointer<ComponentInstance> > > hwInstances =
                referencedDesign->getComponentInstances();

            getViewConfigurationValidator()->changeComponentInstances(hwInstances);
            systemViewConfigurationValidator_->changeComponentInstances(referencedDesign->getSWInstances());

            QVector<QString> instanceNames;
            foreach (QSharedPointer<ViewConfiguration> viewConfiguration,
                *designConfiguration->getViewConfigurations())
            {
				if ( instanceNames.contains(viewConfiguration->getInstanceName() ) )
				{
					return false;
				}
					
				if ( viewConfigurationReferencesHWInstance(viewConfiguration, hwInstances) )
				{
					if ( !getViewConfigurationValidator()->validate(viewConfiguration) )
					{
						return false;
					}
				}
				else if ( !systemViewConfigurationValidator_->validate(viewConfiguration) )
                {
                    return false;
                }

                instanceNames.append(viewConfiguration->getInstanceName());
            }
        }
        else
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: SystemDesignConfigurationValidator::viewConfigurationReferencesHWInstance()
//-----------------------------------------------------------------------------
bool SystemDesignConfigurationValidator::viewConfigurationReferencesHWInstance(
    QSharedPointer<ViewConfiguration> viewConfiguration,
    QSharedPointer<QList<QSharedPointer<ComponentInstance> > > hwInstances) const
{
    foreach (QSharedPointer<ComponentInstance> instance, *hwInstances)
    {
        if (viewConfiguration->getInstanceName() == instance->getInstanceName())
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: SystemDesignConfigurationValidator::findErrorsInViewConfigurations()
//-----------------------------------------------------------------------------
void SystemDesignConfigurationValidator::findErrorsInViewConfigurations(QVector<QString>& errors,
    QSharedPointer<DesignConfiguration> designConfiguration, QString const& context) const
{
    if (!designConfiguration->getViewConfigurations()->isEmpty())
    {
        if (designConfiguration->getDesignRef().isValid())
        {
            QSharedPointer<Design> referencedDesign =
                getLibraryHandler()->getDesign(designConfiguration->getDesignRef());

            QSharedPointer<QList<QSharedPointer<ComponentInstance> > > hwInstances =
                referencedDesign->getComponentInstances();

            if (referencedDesign)
            {
                getViewConfigurationValidator()->changeComponentInstances(hwInstances);
                systemViewConfigurationValidator_->changeComponentInstances(referencedDesign->getSWInstances());
            }

            QVector<QString> instanceNames;
            QVector<QString> duplicateNames;
            foreach (QSharedPointer<ViewConfiguration> viewConfiguration,
                *designConfiguration->getViewConfigurations())
            {
                if (instanceNames.contains(viewConfiguration->getInstanceName()) &&
                    !duplicateNames.contains(viewConfiguration->getInstanceName()))
                {
                    errors.append(QObject::tr("View configuration name '%1' within %2 is not unique.")
                        .arg(viewConfiguration->getInstanceName()).arg(context));
                    duplicateNames.append(viewConfiguration->getInstanceName());
                }
                else
                {
                    instanceNames.append(viewConfiguration->getInstanceName());
                }

                if (viewConfigurationReferencesHWInstance(viewConfiguration, hwInstances))
                {
                    getViewConfigurationValidator()->findErrorsIn(errors, viewConfiguration, context);
                }
                else
                {
                    systemViewConfigurationValidator_->findErrorsIn(errors, viewConfiguration, context);
                }
            }
        }
    }
}
