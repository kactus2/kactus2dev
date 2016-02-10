//-----------------------------------------------------------------------------
// File: DesignConfigurationValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 11.01.2016
//
// Description:
// Validator for the ipxact:designConfiguration.
//-----------------------------------------------------------------------------

#include "DesignConfigurationValidator.h"

#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>

#include <IPXACTmodels/Design/Design.h>

#include <IPXACTmodels/common/Parameter.h>

#include <IPXACTmodels/common/validators/ParameterValidator2014.h>
#include <IPXACTmodels/common/validators/AssertionValidator.h>

#include <IPXACTmodels/designConfiguration/validators/InterconnectionConfigurationValidator.h>
#include <IPXACTmodels/designConfiguration/validators/ViewConfigurationValidator.h>

#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <library/LibraryManager/libraryinterface.h>

#include <QRegularExpression>

//-----------------------------------------------------------------------------
// Function: DesignConfigurationValidator::DesignConfigurationValidator()
//-----------------------------------------------------------------------------
DesignConfigurationValidator::DesignConfigurationValidator(QSharedPointer<ExpressionParser> parser,
    LibraryInterface* library):
libraryHandler_(library),
parameterValidator_(),
assertionValidator_(),
interconnectionValidator_(),
viewConfigurationValidator_()
{
    parameterValidator_ = QSharedPointer<ParameterValidator2014>(
        new ParameterValidator2014(parser, QSharedPointer<QList<QSharedPointer<Choice> > > ()));

    assertionValidator_ = QSharedPointer<AssertionValidator>(new AssertionValidator(parser));

    interconnectionValidator_ = QSharedPointer<InterconnectionConfigurationValidator>(
        new InterconnectionConfigurationValidator(parser, libraryHandler_));

    viewConfigurationValidator_ =
        QSharedPointer<ViewConfigurationValidator>(new ViewConfigurationValidator(libraryHandler_, parser));
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationValidator::~DesignConfigurationValidator()
//-----------------------------------------------------------------------------
DesignConfigurationValidator::~DesignConfigurationValidator()
{

}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationValidator::validate()
//-----------------------------------------------------------------------------
bool DesignConfigurationValidator::validate(QSharedPointer<DesignConfiguration> designConfiguration)
{
    return hasValidVLNV(designConfiguration) && hasValidDesignReference(designConfiguration) &&
        hasValidGeneratorChainConfigurations(designConfiguration) &&
        hasValidInterconnectionConfigurations(designConfiguration) &&
        hasValidViewConfigurations(designConfiguration) &&
        hasValidParameters(designConfiguration) &&
        hasValidAssertions(designConfiguration);
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationValidator::hasValidVLNV()
//-----------------------------------------------------------------------------
bool DesignConfigurationValidator::hasValidVLNV(QSharedPointer<DesignConfiguration> designConfiguration) const
{
    return designConfiguration->getVlnv().getType() == VLNV::DESIGNCONFIGURATION &&
        designConfiguration->getVlnv().isValid();
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationValidator::hasValidDesignReference()
//-----------------------------------------------------------------------------
bool DesignConfigurationValidator::hasValidDesignReference(QSharedPointer<DesignConfiguration> designConfiguration)
    const
{
    if (!designConfiguration->getDesignRef().isEmpty())
    {
        return designConfiguration->getDesignRef().getType() == VLNV::DESIGN &&
            libraryHandler_->contains(designConfiguration->getDesignRef()) &&
            designConfiguration->getDesignRef().isValid();
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationValidator::hasValidGeneratorChainConfigurations()
//-----------------------------------------------------------------------------
bool DesignConfigurationValidator::hasValidGeneratorChainConfigurations(
    QSharedPointer<DesignConfiguration> designConfiguration) const
{
    if (!designConfiguration->getGeneratorChainConfs()->isEmpty())
    {
        for (int chainIndex = 0; chainIndex < designConfiguration->getGeneratorChainConfs()->size(); ++chainIndex)
        {
            QSharedPointer<ConfigurableVLNVReference> chainReference =
                designConfiguration->getGeneratorChainConfs()->at(chainIndex);

            if (chainReference->getType() != VLNV::GENERATORCHAIN || !chainReference->isValid())
            {
                return false;
            }

            for (int comparisonIndex = chainIndex + 1;
                comparisonIndex < designConfiguration->getGeneratorChainConfs()->size(); ++comparisonIndex)
            {
                QSharedPointer<ConfigurableVLNVReference> comparedReference =
                    designConfiguration->getGeneratorChainConfs()->at(comparisonIndex);

                if (chainReference->getVendor() == comparedReference->getVendor() &&
                    chainReference->getLibrary() == comparedReference->getLibrary() &&
                    chainReference->getName() == comparedReference->getName() &&
                    chainReference->getVersion() == comparedReference->getVersion())
                {
                    return false;
                }
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationValidator::hasValidInterconnectionConfigurations()
//-----------------------------------------------------------------------------
bool DesignConfigurationValidator::hasValidInterconnectionConfigurations(
    QSharedPointer<DesignConfiguration> designConfiguration) const
{
    if (!designConfiguration->getInterconnectionConfs()->isEmpty())
    {
        if (designConfiguration->getDesignRef().isValid())
        {
            QSharedPointer<Design> referencedDesign =
                libraryHandler_->getDesign(designConfiguration->getDesignRef());
            interconnectionValidator_->designChanged(referencedDesign);

            QVector<QString> interconnectionReferences;
            foreach (QSharedPointer<InterconnectionConfiguration> configuration,
                *designConfiguration->getInterconnectionConfs())
            {
                if (interconnectionReferences.contains(configuration->getInterconnectionReference()) ||
                    !interconnectionValidator_->validate(configuration))
                {
                    return false;
                }

                interconnectionReferences.append(configuration->getInterconnectionReference());
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
// Function: DesignConfigurationValidator::hasValidViewConfigurations()
//-----------------------------------------------------------------------------
bool DesignConfigurationValidator::hasValidViewConfigurations(
    QSharedPointer<DesignConfiguration> designConfiguration) const
{
    if (!designConfiguration->getViewConfigurations()->isEmpty())
    {
        if (designConfiguration->getDesignRef().isValid())
        {
            QSharedPointer<Design> referencedDesign =
                libraryHandler_->getDesign(designConfiguration->getDesignRef());
            viewConfigurationValidator_->changeComponentInstances(referencedDesign->getComponentInstances());

            QVector<QString> instanceNames;
            foreach (QSharedPointer<ViewConfiguration> viewConfiguration,
                *designConfiguration->getViewConfigurations())
            {
                if (instanceNames.contains(viewConfiguration->getInstanceName()) ||
                    !viewConfigurationValidator_->validate(viewConfiguration))
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
// Function: DesignConfigurationValidator::hasValidParameters()
//-----------------------------------------------------------------------------
bool DesignConfigurationValidator::hasValidParameters(QSharedPointer<DesignConfiguration> designConfiguration)
    const
{
    if (!designConfiguration->getParameters()->isEmpty())
    {
        QVector<QString> parameterNames;
        foreach (QSharedPointer<Parameter> parameter, *designConfiguration->getParameters())
        {
            if (parameterNames.contains(parameter->name()) || !parameterValidator_->validate(parameter))
            {
                return false;
            }

            parameterNames.append(parameter->name());
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationValidator::hasValidAssertions()
//-----------------------------------------------------------------------------
bool DesignConfigurationValidator::hasValidAssertions(QSharedPointer<DesignConfiguration> designConfiguration)
    const
{
    if (!designConfiguration->getAssertions()->isEmpty())
    {
        QVector<QString> assertionNames;
        foreach (QSharedPointer<Assertion> assertion, *designConfiguration->getAssertions())
        {
            if (assertionNames.contains(assertion->name()) || !assertionValidator_->validate(assertion))
            {
                return false;
            }

            assertionNames.append(assertion->name());
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void DesignConfigurationValidator::findErrorsIn(QVector<QString>& errors,
    QSharedPointer<DesignConfiguration> designConfiguration)
{
    QString context = QObject::tr("design configuration %1").arg(designConfiguration->getVlnv().toString());

    findErrorsInVLNV(errors, designConfiguration);
    findErrorsInDesignReference(errors, designConfiguration, context);
    findErrorsInGeneratorChainConfigurations(errors, designConfiguration, context);
    findErrorsInInterconnectionConfigurations(errors, designConfiguration, context);
    findErrorsInViewConfigurations(errors, designConfiguration, context);
    findErrorsInParameters(errors, designConfiguration, context);
    findErrorsInAssertions(errors, designConfiguration, context);
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationValidator::findErrorsInVLNV()
//-----------------------------------------------------------------------------
void DesignConfigurationValidator::findErrorsInVLNV(QVector<QString>& errors,
    QSharedPointer<DesignConfiguration> designConfiguration) const
{
    designConfiguration->getVlnv().isValid(errors, QLatin1String("design configuration"));
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationValidator::findErrorsInDesignReference()
//-----------------------------------------------------------------------------
void DesignConfigurationValidator::findErrorsInDesignReference(QVector<QString>& errors,
    QSharedPointer<DesignConfiguration> designConfiguration, QString const& context) const
{
    if (!designConfiguration->getDesignRef().isEmpty())
    {
        if (!libraryHandler_->contains(designConfiguration->getDesignRef()))
        {
            errors.append(QObject::tr("Design reference %1 within %2 was not found in the "
                "library")
                .arg(designConfiguration->getDesignRef().toString()).arg(context));
        }

        designConfiguration->getDesignRef().isValid(errors, QLatin1String("design reference"));
    }
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationValidator::findErrorsInGeneratorChainConfigurations()
//-----------------------------------------------------------------------------
void DesignConfigurationValidator::findErrorsInGeneratorChainConfigurations(QVector<QString>& errors,
    QSharedPointer<DesignConfiguration> designConfiguration, QString const& context) const
{
    if (!designConfiguration->getGeneratorChainConfs()->isEmpty())
    {
        for (int chainIndex = 0; chainIndex < designConfiguration->getGeneratorChainConfs()->size(); ++chainIndex)
        {
            QSharedPointer<ConfigurableVLNVReference> chainReference =
                designConfiguration->getGeneratorChainConfs()->at(chainIndex);

            if (chainReference->getType() != VLNV::GENERATORCHAIN)
            {
                errors.append(QObject::tr("The type of the generator chain configuration is invalid within %1.")
                    .arg(context));
            }

            chainReference->isValid(errors, "generator chain configuration");

            for (int comparisonIndex = chainIndex + 1;
                comparisonIndex < designConfiguration->getGeneratorChainConfs()->size(); ++comparisonIndex)
            {
                QSharedPointer<ConfigurableVLNVReference> comparedReference =
                    designConfiguration->getGeneratorChainConfs()->at(comparisonIndex);

                if (chainReference->getVendor() == comparedReference->getVendor() &&
                    chainReference->getLibrary() == comparedReference->getLibrary() &&
                    chainReference->getName() == comparedReference->getName() &&
                    chainReference->getVersion() == comparedReference->getVersion())
                {
                    errors.append(QObject::tr("Generator chain configuration %1 is not unique within %2.")
                        .arg(chainReference->toString()).arg(context));
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationValidator::findErrorsInInterconnectionConfigurations()
//-----------------------------------------------------------------------------
void DesignConfigurationValidator::findErrorsInInterconnectionConfigurations(QVector<QString>& errors,
    QSharedPointer<DesignConfiguration> designConfiguration, QString const& context) const
{
    if (!designConfiguration->getInterconnectionConfs()->isEmpty() &&
        designConfiguration->getDesignRef().isValid())
    {
        QSharedPointer<Design> referencedDesign =
            libraryHandler_->getDesign(designConfiguration->getDesignRef());
        interconnectionValidator_->designChanged(referencedDesign);

        QVector<QString> connectionReferences;
        QVector<QString> duplicateNames;
        foreach (QSharedPointer<InterconnectionConfiguration> connectionConfiguration,
            *designConfiguration->getInterconnectionConfs())
        {
            if (connectionReferences.contains(connectionConfiguration->getInterconnectionReference()) &&
                !duplicateNames.contains(connectionConfiguration->getInterconnectionReference()))
            {
                errors.append(QObject::tr("Interconnection reference '%1' set for interconnection configuration "
                    "within %2 is not unique.")
                    .arg(connectionConfiguration->getInterconnectionReference()).arg(context));
                duplicateNames.append(connectionConfiguration->getInterconnectionReference());
            }

            interconnectionValidator_->findErrorsIn(errors, connectionConfiguration, context);
            connectionReferences.append(connectionConfiguration->getInterconnectionReference());
        }
    }
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationValidator::findErrorsInViewConfigurations()
//-----------------------------------------------------------------------------
void DesignConfigurationValidator::findErrorsInViewConfigurations(QVector<QString>& errors,
    QSharedPointer<DesignConfiguration> designConfiguration, QString const& context) const
{
    if (!designConfiguration->getViewConfigurations()->isEmpty())
    {
        if (designConfiguration->getDesignRef().isValid())
        {
            QSharedPointer<Design> referencedDesign =
                libraryHandler_->getDesign(designConfiguration->getDesignRef());

            if (referencedDesign)
            {
                viewConfigurationValidator_->changeComponentInstances(referencedDesign->getComponentInstances());
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

                viewConfigurationValidator_->findErrorsIn(errors, viewConfiguration, context);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationValidator::findErrorsInParameters()
//-----------------------------------------------------------------------------
void DesignConfigurationValidator::findErrorsInParameters(QVector<QString>& errors,
    QSharedPointer<DesignConfiguration> designConfiguration, QString const& context) const
{
    if (!designConfiguration->getParameters()->isEmpty())
    {
        QVector<QString> parameterNames;
        QVector<QString> duplicateNames;
        foreach (QSharedPointer<Parameter> parameter, *designConfiguration->getParameters())
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
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationValidator::findErrorsInAssertions()
//-----------------------------------------------------------------------------
void DesignConfigurationValidator::findErrorsInAssertions(QVector<QString>& errors,
    QSharedPointer<DesignConfiguration> designConfiguration, QString const& context) const
{
    if (!designConfiguration->getAssertions()->isEmpty())
    {
        QVector<QString> assertionNames;
        QVector<QString> duplicateNames;
        foreach (QSharedPointer<Assertion> assertion, *designConfiguration->getAssertions())
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
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationValidator::getViewConfigurationValidator()
//-----------------------------------------------------------------------------
QSharedPointer<ViewConfigurationValidator> DesignConfigurationValidator::getViewConfigurationValidator() const
{
    return viewConfigurationValidator_;
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationValidator::getLibraryHandler()
//-----------------------------------------------------------------------------
LibraryInterface* DesignConfigurationValidator::getLibraryHandler() const
{
    return libraryHandler_;
}