//-----------------------------------------------------------------------------
// Instantiations: InstantiationsValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 23.11.2015
//
// Description:
// Validator for ipxact:Instantiations.
//-----------------------------------------------------------------------------

#include "InstantiationsValidator.h"

#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <library/LibraryManager/libraryinterface.h>

#include <IPXACTmodels/Component/Choice.h>
#include <IPXACTmodels/Component/DesignConfigurationInstantiation.h>
#include <IPXACTmodels/Component/DesignInstantiation.h>
#include <IPXACTmodels/Component/ComponentInstantiation.h>
#include <IPXACTmodels/Component/FileSet.h>

#include <IPXACTmodels/common/Enumeration.h>
#include <IPXACTmodels/common/validators/ParameterValidator2014.h>

#include <QRegularExpression>
#include <QStringList>

//-----------------------------------------------------------------------------
// Function: InstantiationsValidator::InstantiationsValidator()
//-----------------------------------------------------------------------------
InstantiationsValidator::InstantiationsValidator(QSharedPointer<ExpressionParser> expressionParser,
                                                 QSharedPointer<QList<QSharedPointer<FileSet> > > fileSets,
                                                 QSharedPointer<ParameterValidator2014> parameterValidator,
                                                 LibraryInterface* libraryHandler):
expressionParser_(expressionParser),
availableFileSets_(fileSets),
parameterValidator_(parameterValidator),
libraryHandler_(libraryHandler)
{

}

//-----------------------------------------------------------------------------
// Function: InstantiationsValidator::~InstantiationsValidator()
//-----------------------------------------------------------------------------
InstantiationsValidator::~InstantiationsValidator()
{

}

//-----------------------------------------------------------------------------
// Function: InstantiationsValidator::componentChange()
//-----------------------------------------------------------------------------
void InstantiationsValidator::componentChange(QSharedPointer<QList<QSharedPointer<FileSet> > > newFileSets)
{
    availableFileSets_ = newFileSets;
}

//-----------------------------------------------------------------------------
// Function: InstantiationsValidator::validateDesignInstantiation()
//-----------------------------------------------------------------------------
bool InstantiationsValidator::validateDesignInstantiation(QSharedPointer<DesignInstantiation> designInstantiation)
    const
{
    return hasValidName(designInstantiation->name()) && hasValidDesignReference(designInstantiation);
}

//-----------------------------------------------------------------------------
// Function: EnumeratedValueValidator::hasValidName()
//-----------------------------------------------------------------------------
bool InstantiationsValidator::hasValidName(QString const& name) const
{
    QRegularExpression whiteSpaceExpression;
    whiteSpaceExpression.setPattern("^\\s*$");
    QRegularExpressionMatch whiteSpaceMatch = whiteSpaceExpression.match(name);

    if (name.isEmpty() || whiteSpaceMatch.hasMatch())
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: InstantiationsValidator::hasValidDesignReference()
//-----------------------------------------------------------------------------
bool InstantiationsValidator::hasValidDesignReference(QSharedPointer<DesignInstantiation> designInstantiation) const
{
    if ( libraryHandler_ && designInstantiation->getDesignReference() &&
        designInstantiation->getDesignReference()->isValid() &&
        libraryHandler_->getModel(*designInstantiation->getDesignReference().data()))
    {
        return libraryHandler_->contains(*designInstantiation->getDesignReference().data()) &&
            libraryHandler_->getDocumentType(*designInstantiation->getDesignReference().data()) == VLNV::DESIGN;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: InstantiationsValidator::findErrorsInDesignInstantiation()
//-----------------------------------------------------------------------------
void InstantiationsValidator::findErrorsInDesignInstantiation(QVector<QString>& errors,
    QSharedPointer<DesignInstantiation> designInstantiation, QString const& context) const
{
	if ( !hasValidName( designInstantiation->name() ) )
	{
        errors.append(QObject::tr("Invalid name set for design instantiation %1 within %2")
            .arg(designInstantiation->name()).arg(context));
	}

	if ( !designInstantiation->getDesignReference() )
	{
		errors.append(QObject::tr("The design reference does not exist for design instantiation %2.")
            .arg(designInstantiation->name()));
	}
    else if (!designInstantiation->getDesignReference()->isValid())
	{
		errors.append(QObject::tr("Invalid design reference %1 set for design instantiation %2")
            .arg(designInstantiation->getDesignReference()->toString()).arg(designInstantiation->name()));
	}
    else if (!libraryHandler_->getModel(*designInstantiation->getDesignReference().data()))
    {
        errors.append(QObject::tr("Design %1 referenced by design instantiation in %2 was not found in the "
            "library.")
            .arg(designInstantiation->getDesignReference()->toString()).arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: InstantiationsValidator::validateDesignConfigurationInstantiation()
//-----------------------------------------------------------------------------
bool InstantiationsValidator::validateDesignConfigurationInstantiation(
	QSharedPointer<DesignConfigurationInstantiation> instantiation) const
{
    return hasValidName(instantiation->name()) && hasValidDesignConfigurationReference(instantiation) &&
        hasValidParameters(instantiation->getParameters());
}

//-----------------------------------------------------------------------------
// Function: InstantiationsValidator::hasValidDesignConfigurationReference()
//-----------------------------------------------------------------------------
bool InstantiationsValidator::hasValidDesignConfigurationReference(
    QSharedPointer<DesignConfigurationInstantiation> instantiation) const
{
    if ( libraryHandler_ && instantiation->getDesignConfigurationReference() &&
        instantiation->getDesignConfigurationReference()->isValid() &&
        libraryHandler_->getModel(*instantiation->getDesignConfigurationReference().data()))
    {
        return libraryHandler_->contains(*instantiation->getDesignConfigurationReference().data()) &&
            libraryHandler_->getDocumentType(
            *instantiation->getDesignConfigurationReference().data()) == VLNV::DESIGNCONFIGURATION;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: InstantiationsValidator::hasValidParameters()
//-----------------------------------------------------------------------------
bool InstantiationsValidator::hasValidParameters(
    QSharedPointer<QList<QSharedPointer<Parameter> > > availableParameters) const
{
    if (!availableParameters->isEmpty())
    {
        QStringList parameterNames;
        foreach ( QSharedPointer<Parameter> parameter, *availableParameters )
        {
            if ( parameterNames.contains(parameter->name()) || !parameterValidator_->validate(parameter) )
            {
                return false;
            }
            else
            {
                parameterNames.append(parameter->name());
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: InstantiationsValidator::findErrorsInDesignConfigurationInstantiation()
//-----------------------------------------------------------------------------
void InstantiationsValidator::findErrorsInDesignConfigurationInstantiation(QVector<QString>& errors,
	QSharedPointer<DesignConfigurationInstantiation> instantiation, QString const& contex) const
{
	if ( !hasValidName( instantiation->name() ) )
	{
        errors.append(QObject::tr("Invalid name set for design configuration instantiation %1 within %2")
            .arg(instantiation->name()).arg(contex));
	}

	if ( !instantiation->getDesignConfigurationReference() )
	{
		errors.append(QObject::tr("The design configuration reference does not exist for design configuration "
            "instantiation %1.").arg(instantiation->name()));
	}
	else if ( !instantiation->getDesignConfigurationReference()->isValid() )
	{
        errors.append(QObject::tr("Invalid design configuration reference %1 set for design configuration "
            "instantiation %2")
            .arg(instantiation->getDesignConfigurationReference()->toString()).arg(instantiation->name()));
	}
    else if (!libraryHandler_->getModel(*instantiation->getDesignConfigurationReference().data()))
    {
        errors.append(QObject::tr("Design configuration %1 referenced by design configuration instantiation in "
            "%2 was not found in the library.")
            .arg(instantiation->getDesignConfigurationReference()->toString()).arg(contex));
    }

    QStringList parameterNames;
    QStringList foundNames;
	foreach ( QSharedPointer<Parameter> parameter, *instantiation->getParameters() )
	{
        if (parameterNames.contains(parameter->name()) && !foundNames.contains(parameter->name()))
        {
            errors.append(QObject::tr("Parameter name %1 is not unique within design configuration "
                "instantiation %2").arg(parameter->name()).arg(instantiation->name()));
            foundNames.append(parameter->name());
        }
        else
        {
            parameterNames.append(parameter->name());
        }
		parameterValidator_->findErrorsIn(errors,parameter,contex);
	}
}

//-----------------------------------------------------------------------------
// Function: InstantiationsValidator::validateComponentInstantiation()
//-----------------------------------------------------------------------------
bool InstantiationsValidator::validateComponentInstantiation (QSharedPointer<ComponentInstantiation> instantiation)
    const
{
    return hasValidName(instantiation->name()) &&
        componentInstantiationFileBuildersAreValid(instantiation) &&
        componentInstantiationFileSetReferencesAreValid(instantiation) &&
        hasValidModuleParameters(instantiation) &&
        hasValidParameters(instantiation->getParameters());
}

//-----------------------------------------------------------------------------
// Function: InstantiationsValidator::componentInstantiationFileBuildersAreValid()
//-----------------------------------------------------------------------------
bool InstantiationsValidator::componentInstantiationFileBuildersAreValid(
    QSharedPointer<ComponentInstantiation> instantiation) const
{
    foreach ( QSharedPointer<FileBuilder> currentFileBuilder, *instantiation->getDefaultFileBuilders() )
    {
        if ( !hasValidName( currentFileBuilder->getFileType() ) ||
            !fileBuilderReplaceDefaultFlagsIsValid(currentFileBuilder) )
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: InstantiationsValidator::fileBuilderReplaceDefaultFlagsIsValid()
//-----------------------------------------------------------------------------
bool InstantiationsValidator::fileBuilderReplaceDefaultFlagsIsValid(QSharedPointer<FileBuilder> fileBuilder) const
{
    QString replaceDefaultFlags = fileBuilder->getReplaceDefaultFlags();
    if (!replaceDefaultFlags.isEmpty())
    {
        bool replaceFlagsOk = expressionParser_->isValidExpression(replaceDefaultFlags);
        int replaceFlagsInt = expressionParser_->parseExpression(replaceDefaultFlags).toInt();

        return replaceFlagsOk && (replaceFlagsInt == 0 || replaceFlagsInt == 1);
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: InstantiationsValidator::componentInstantiationFileSetReferencesAreValid()
//-----------------------------------------------------------------------------
bool InstantiationsValidator::componentInstantiationFileSetReferencesAreValid(
    QSharedPointer<ComponentInstantiation> instantiation) const
{
    foreach ( QString fileSetRef, *instantiation->getFileSetReferences() )
    {
        if ( !fileSetReferenceIsValid(fileSetRef) )
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: InstantiationsValidator::existingReference()
//-----------------------------------------------------------------------------
bool InstantiationsValidator::fileSetReferenceIsValid(QString const& fileSetRef) const
{
    foreach ( QSharedPointer<FileSet> fileSet, *availableFileSets_ )
    {
        if ( fileSetRef == fileSet->name() )
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: InstantiationsValidator::hasValidModuleParameters()
//-----------------------------------------------------------------------------
bool InstantiationsValidator::hasValidModuleParameters(QSharedPointer<ComponentInstantiation> instantiation) const
{
    if (!instantiation->getModuleParameters()->isEmpty())
    {
        QStringList moduleParameterNames;
        foreach ( QSharedPointer<ModuleParameter> parameter, *instantiation->getModuleParameters() )
        {
            if ( moduleParameterNames.contains(parameter->name()) || !parameterValidator_->validate(parameter) ||
                !moduleParameterHasValidPresence(parameter) || !moduleParameterHasValidUsageType(parameter))
            {
                return false;
            }

            moduleParameterNames.append(parameter->name());
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: InstantiationsValidator::findErrorsInComponentInstantiation()
//-----------------------------------------------------------------------------
void InstantiationsValidator::findErrorsInComponentInstantiation(QVector<QString>& errors,
    QSharedPointer<ComponentInstantiation> instantiation, QString const& context) const
{
    if (!hasValidName(instantiation->name()))
    {
        errors.append(QObject::tr("Invalid name set for component instantiation %1 within %2")
            .arg(instantiation->name()).arg(context));
    }

    foreach ( QString fileSetRef, *instantiation->getFileSetReferences() )
    {
        if ( !fileSetReferenceIsValid(fileSetRef) )
        {
            errors.append(QObject::tr("Referenced file set %1 does not exist.").arg(fileSetRef));
        }
    }

	foreach ( QSharedPointer<FileBuilder> currentFileBuilder, *instantiation->getDefaultFileBuilders() )
	{
		if ( !hasValidName( currentFileBuilder->getFileType() ) )
		{
			errors.append(QObject::tr("The type of default file builder is empty."));
		}
        if (!fileBuilderReplaceDefaultFlagsIsValid(currentFileBuilder))
		{
            errors.append(QObject::tr("Invalid \"replace default flags\" value set for component instantiation %1."
                " Value must evaluate to 0 or 1.").
                arg(instantiation->name()));
		}
	}

    QStringList parameterNames;
    QStringList duplicateParameterNames;
	foreach ( QSharedPointer<ModuleParameter> parameter, *instantiation->getModuleParameters() )
	{
		parameterValidator_->findErrorsIn(errors,parameter,context);

		if ( !moduleParameterHasValidPresence(parameter) )
		{
			errors.append(QObject::tr("The presence of module parameter %1 is invalid: %2")
				.arg(parameter->name()).arg(parameter->getIsPresent()));
		}

		if ( !moduleParameterHasValidUsageType(parameter) )
		{
			errors.append(QObject::tr("The usage of module parameter %1 is invalid: %2")
				.arg(parameter->name()).arg(parameter->getUsageType()));
		}

        if (parameterNames.contains(parameter->name()) && !duplicateParameterNames.contains(parameter->name()))
        {
            errors.append(QObject::tr("Module parameter name %1 within %2 is not unique.")
                .arg(parameter->name()).arg(context));
            duplicateParameterNames.append(parameter->name());
        }
        else
        {
            parameterNames.append(parameter->name());
        }
	}

    parameterNames.clear();
    duplicateParameterNames.clear();
	foreach ( QSharedPointer<Parameter> parameter, *instantiation->getParameters() )
	{
        parameterValidator_->findErrorsIn(errors,parameter,context);
        
        if (parameterNames.contains(parameter->name()) && !duplicateParameterNames.contains(parameter->name()))
        {
            errors.append(QObject::tr("Parameter name %1 within %2 is not unique.")
                .arg(parameter->name()).arg(context));
            duplicateParameterNames.append(parameter->name());
        }
        else
        {
            parameterNames.append(parameter->name());
        }
	}
}

//-----------------------------------------------------------------------------
// Function: InstantiationsValidator::isValidUsageType()
//-----------------------------------------------------------------------------
bool InstantiationsValidator::moduleParameterHasValidUsageType(QSharedPointer<ModuleParameter> parameter) const
{
	if ( !parameter->getUsageType().isEmpty() && parameter->getUsageType() != "nontyped"
		&& parameter->getUsageType() != "typed" )
	{
		return false;
	}

    return true;
}

//-----------------------------------------------------------------------------
// Function: InstantiationsValidator::isValidPresence()
//-----------------------------------------------------------------------------
bool InstantiationsValidator::moduleParameterHasValidPresence(QSharedPointer<ModuleParameter> parameter) const
{
	if ( !parameter->getIsPresent().isEmpty() &&
        !expressionParser_->isValidExpression( parameter->getIsPresent() ) )
	{
		return false;
	}

    return true;
}
