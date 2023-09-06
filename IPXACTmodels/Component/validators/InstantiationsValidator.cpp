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

#include <KactusAPI/include/ExpressionParser.h>

#include <KactusAPI/include/LibraryInterface.h>

#include <IPXACTmodels/Component/Choice.h>
#include <IPXACTmodels/Component/DesignConfigurationInstantiation.h>
#include <IPXACTmodels/Component/DesignInstantiation.h>
#include <IPXACTmodels/Component/ComponentInstantiation.h>
#include <IPXACTmodels/Component/FileSet.h>
#include <IPXACTmodels/Component/FileSetRef.h>

#include <IPXACTmodels/common/Enumeration.h>
#include <IPXACTmodels/common/validators/ParameterValidator.h>

#include <QRegularExpression>
#include <QStringList>

//-----------------------------------------------------------------------------
// Function: InstantiationsValidator::InstantiationsValidator()
//-----------------------------------------------------------------------------
InstantiationsValidator::InstantiationsValidator(QSharedPointer<ExpressionParser> expressionParser,
                                                 QSharedPointer<QList<QSharedPointer<FileSet> > > fileSets,
                                                 QSharedPointer<ParameterValidator> parameterValidator,
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
    whiteSpaceExpression.setPattern(QStringLiteral("^\\s*$"));
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
    if (designInstantiation->getDesignReference() && designInstantiation->getDesignReference()->isValid())
    {
        return libraryHandler_->getDocumentType(*designInstantiation->getDesignReference().data()) == VLNV::DESIGN;
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
    else if (!libraryHandler_->contains(*designInstantiation->getDesignReference()))
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
	QSharedPointer<DesignConfigurationInstantiation> instantiation, Document::Revision docRevision) const
{
    return hasValidName(instantiation->name()) && hasValidDesignConfigurationReference(instantiation) &&
        hasValidParameters(instantiation->getParameters(), docRevision);
}

//-----------------------------------------------------------------------------
// Function: InstantiationsValidator::hasValidDesignConfigurationReference()
//-----------------------------------------------------------------------------
bool InstantiationsValidator::hasValidDesignConfigurationReference(
    QSharedPointer<DesignConfigurationInstantiation> instantiation) const
{
    if (instantiation->getDesignConfigurationReference() &&
        instantiation->getDesignConfigurationReference()->isValid())
    {
        return libraryHandler_->getDocumentType(*instantiation->getDesignConfigurationReference()) == 
            VLNV::DESIGNCONFIGURATION;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: InstantiationsValidator::hasValidParameters()
//-----------------------------------------------------------------------------
bool InstantiationsValidator::hasValidParameters(
    QSharedPointer<QList<QSharedPointer<Parameter> > > availableParameters, Document::Revision docRevision) const
{
    if (!availableParameters->isEmpty())
    {
        QStringList parameterNames;
        for( QSharedPointer<Parameter> parameter : *availableParameters )
        {
            if ( parameterNames.contains(parameter->name()) || !parameterValidator_->validate(parameter, docRevision) )
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
    else if (!libraryHandler_->contains(*instantiation->getDesignConfigurationReference()))
    {
        errors.append(QObject::tr("Design configuration %1 referenced by design configuration instantiation in "
            "%2 was not found in the library.")
            .arg(instantiation->getDesignConfigurationReference()->toString()).arg(contex));
    }

    QStringList parameterNames;
    QStringList foundNames;
	for ( QSharedPointer<Parameter> parameter : *instantiation->getParameters() )
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
bool InstantiationsValidator::validateComponentInstantiation (QSharedPointer<ComponentInstantiation> instantiation,
    Document::Revision docRevision) const
{
    return hasValidName(instantiation->name()) &&
        componentInstantiationFileBuildersAreValid(instantiation) &&
        componentInstantiationFileSetReferencesAreValid(instantiation) &&
        hasValidModuleParameters(instantiation, docRevision) &&
        hasValidParameters(instantiation->getParameters(), docRevision);
}

//-----------------------------------------------------------------------------
// Function: InstantiationsValidator::componentInstantiationFileBuildersAreValid()
//-----------------------------------------------------------------------------
bool InstantiationsValidator::componentInstantiationFileBuildersAreValid(
    QSharedPointer<ComponentInstantiation> instantiation) const
{
    for ( QSharedPointer<FileBuilder> currentFileBuilder : *instantiation->getDefaultFileBuilders() )
    {
        if ( !hasValidName( currentFileBuilder->getFileType().type_ ) ||
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
        bool replaceFlagsOk = false;
        int replaceFlagsInt = expressionParser_->parseExpression(replaceDefaultFlags, &replaceFlagsOk).toInt();

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
    for ( auto const& fileSetRef : *instantiation->getFileSetReferences() )
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
bool InstantiationsValidator::fileSetReferenceIsValid(QSharedPointer<FileSetRef> fileSetRef) const
{
    for ( auto const& fileSet : *availableFileSets_ )
    {
        if ( fileSetRef->getReference() == fileSet->name() )
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: InstantiationsValidator::hasValidModuleParameters()
//-----------------------------------------------------------------------------
bool InstantiationsValidator::hasValidModuleParameters(QSharedPointer<ComponentInstantiation> instantiation,
    Document::Revision docRevision) const
{
    if (!instantiation->getModuleParameters()->isEmpty())
    {
        QStringList moduleParameterNames;
        for ( QSharedPointer<ModuleParameter> parameter : *instantiation->getModuleParameters() )
        {
            if ( moduleParameterNames.contains(parameter->name()) || !parameterValidator_->validate(parameter, docRevision) ||
                !moduleParameterHasValidPresence(parameter) || !moduleParameterHasValidUsageType(parameter, docRevision))
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
    QSharedPointer<ComponentInstantiation> instantiation, QString const& context,
    Document::Revision docRevision) const
{
    if (!hasValidName(instantiation->name()))
    {
        errors.append(QObject::tr("Invalid name set for component instantiation %1 within %2")
            .arg(instantiation->name()).arg(context));
    }

    for ( auto const& fileSetRef : *instantiation->getFileSetReferences() )
    {
        if ( !fileSetReferenceIsValid(fileSetRef) )
        {
            errors.append(QObject::tr("Referenced file set %1 does not exist.").arg(fileSetRef->getReference()));
        }
    }

	for ( QSharedPointer<FileBuilder> currentFileBuilder : *instantiation->getDefaultFileBuilders() )
	{
		if ( !hasValidName( currentFileBuilder->getFileType().type_ ) )
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
	for ( QSharedPointer<ModuleParameter> parameter : *instantiation->getModuleParameters() )
	{
		parameterValidator_->findErrorsIn(errors,parameter,context, docRevision);

		if ( !moduleParameterHasValidPresence(parameter) )
		{
			errors.append(QObject::tr("The presence of module parameter %1 is invalid: %2")
				.arg(parameter->name()).arg(parameter->getIsPresent()));
		}

		if (!moduleParameterHasValidUsageType(parameter, docRevision))
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
	for ( QSharedPointer<Parameter> parameter : *instantiation->getParameters() )
	{
        parameterValidator_->findErrorsIn(errors,parameter,context, docRevision);
        
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
bool InstantiationsValidator::moduleParameterHasValidUsageType(QSharedPointer<ModuleParameter> parameter,
    Document::Revision docRevision) const
{
    if (parameter->getUsageType().isEmpty())
    {
        return true;
    }

    QStringList validUsageTypes = { QStringLiteral("nontyped"), QStringLiteral("typed") };
    if (docRevision == Document::Revision::Std22)
    {
        validUsageTypes << QStringLiteral("runtime");
    }

    if (!validUsageTypes.contains(parameter->getUsageType()))
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
    bool isValidPresence = false;
    expressionParser_->parseExpression(parameter->getIsPresent(), &isValidPresence);
    
	if (isValidPresence == false)
	{
		return false;
	}

    return true;
}

//-----------------------------------------------------------------------------
// Function: InstantiationsValidator::getParameterValidator()
//-----------------------------------------------------------------------------
QSharedPointer<ParameterValidator> InstantiationsValidator::getParameterValidator() const
{
    return parameterValidator_;
}
