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

#include <IPXACTmodels/Component/choice.h>
#include <IPXACTmodels/common/Enumeration.h>
#include <IPXACTmodels/common/validators/ParameterValidator2014.h>

#include <editors/ComponentEditor/common/SystemVerilogExpressionParser.h>

#include <QRegularExpression>
#include <QStringList>

//-----------------------------------------------------------------------------
// Function: InstantiationsValidator::InstantiationsValidator()
//-----------------------------------------------------------------------------
InstantiationsValidator::InstantiationsValidator()
{

}

//-----------------------------------------------------------------------------
// Function: InstantiationsValidator::~InstantiationsValidator()
//-----------------------------------------------------------------------------
InstantiationsValidator::~InstantiationsValidator()
{

}

//-----------------------------------------------------------------------------
// Function: InstantiationsValidator::validateDesignInstantiation()
//-----------------------------------------------------------------------------
bool InstantiationsValidator::validateDesignInstantiation(QSharedPointer<DesignInstantiation> designInstantiation) const
{
	if ( !hasValidName( designInstantiation->name() ) )
	{
		return false;
	}

	if ( !designInstantiation->getDesignReference() || !designInstantiation->getDesignReference()->isValid() )
	{
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Function: InstantiationsValidator::findErrorsInDesignInstantiation()
//-----------------------------------------------------------------------------
void InstantiationsValidator::findErrorsInDesignInstantiation(QVector<QString>& errors,
	QSharedPointer<DesignInstantiation> designInstantiation, QString const&) const
{
	if ( !hasValidName( designInstantiation->name() ) )
	{
		errors.append(QObject::tr("The name is invalid or in-existing: %1").arg(designInstantiation->name()));
	}

	if ( !designInstantiation->getDesignReference() )
	{
		errors.append(QObject::tr("The design reference does not exist."));
	}
	else if ( !designInstantiation->getDesignReference()->isValid() )
	{
		errors.append(QObject::tr("The design reference is invalid:").arg(designInstantiation->getDesignReference()->toString()));
	}
}

//-----------------------------------------------------------------------------
// Function: InstantiationsValidator::validateDesignConfigurationInstantiation()
//-----------------------------------------------------------------------------
bool InstantiationsValidator::validateDesignConfigurationInstantiation(
	QSharedPointer<DesignConfigurationInstantiation> designConfigurationInstantiation) const
{
	QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
	ParameterValidator2014 paraValidator(parser, QSharedPointer<QList<QSharedPointer<Choice> > > ());

	if ( !hasValidName( designConfigurationInstantiation->name() ) )
	{
		return false;
	}

	if ( !designConfigurationInstantiation->getDesignConfigurationReference() ||
		!designConfigurationInstantiation->getDesignConfigurationReference()->isValid() )
	{
		return false;
	}

	foreach ( QSharedPointer<Parameter> parameter, *designConfigurationInstantiation->getParameters() )
	{
		if ( !paraValidator.validate(parameter.data()) )
		{
			return false;
		}
	}

	return true;
}

//-----------------------------------------------------------------------------
// Function: InstantiationsValidator::findErrorsInDesignConfigurationInstantiation()
//-----------------------------------------------------------------------------
void InstantiationsValidator::findErrorsInDesignConfigurationInstantiation(QVector<QString>& errors,
	QSharedPointer<DesignConfigurationInstantiation> designConfigurationInstantiation, QString const& context) const
{
	QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
	ParameterValidator2014 paraValidator(parser, QSharedPointer<QList<QSharedPointer<Choice> > > ());

	if ( !hasValidName( designConfigurationInstantiation->name() ) )
	{
		errors.append(QObject::tr("The name is invalid or in-existing: %1").arg(designConfigurationInstantiation->name()));
	}

	if ( !designConfigurationInstantiation->getDesignConfigurationReference() )
	{
		errors.append(QObject::tr("The design reference does not exist."));
	}
	else if ( !designConfigurationInstantiation->getDesignConfigurationReference()->isValid() )
	{
		errors.append(QObject::tr("The design reference is invalid:")
			.arg(designConfigurationInstantiation->getDesignConfigurationReference()->toString()));
	}

	foreach ( QSharedPointer<Parameter> parameter, *designConfigurationInstantiation->getParameters() )
	{
		paraValidator.findErrorsIn(errors,parameter,context);
	}
}

//-----------------------------------------------------------------------------
// Function: InstantiationsValidator::validateComponentInstantiation()
//-----------------------------------------------------------------------------
bool InstantiationsValidator::validateComponentInstantiation(QSharedPointer<ComponentInstantiation> componentInstantiation,
	QSharedPointer<QList<QSharedPointer<FileSet> > > fileSets) const
{
	QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
	ParameterValidator2014 paraValidator(parser, QSharedPointer<QList<QSharedPointer<Choice> > > ());

	if ( inexistingReferenceList(fileSets, componentInstantiation ) )
	{
		return false;
	}

	foreach ( QString fileSetRef, *componentInstantiation->getFileSetReferences() )
	{
		if ( !existingReference(fileSets, fileSetRef) )
		{
			return false;
		}
	}

	foreach ( QSharedPointer<FileBuilder> currentFileBuilder, *componentInstantiation->getDefaultFileBuilders() )
	{
		if ( !hasValidName( currentFileBuilder->getFileType() ) )
		{
			return false;
		}

		if ( !currentFileBuilder->getReplaceDefaultFlags().isEmpty() &&
			!parser->isValidExpression( currentFileBuilder->getReplaceDefaultFlags() ) )
		{
			return false;
		}
	}

	foreach ( QSharedPointer<ModuleParameter> parameter, *componentInstantiation->getModuleParameters() )
	{
		if ( !paraValidator.validate(parameter.data()) )
		{
			return false;
		}

		if ( !isValidPresence(parameter, parser) )
		{
			return false;
		}

		if ( !isValidUsageType(parameter) )
		{
			return false;
		}
	}

	foreach ( QSharedPointer<Parameter> parameter, *componentInstantiation->getParameters() )
	{
		if ( !paraValidator.validate(parameter.data()) )
		{
			return false;
		}
	}

	return true;
}

//-----------------------------------------------------------------------------
// Function: InstantiationsValidator::findErrorsInComponentInstantiation()
//-----------------------------------------------------------------------------
void InstantiationsValidator::findErrorsInComponentInstantiation(QVector<QString>& errors,
	QSharedPointer<ComponentInstantiation> componentInstantiation, QString const& contex,
	QSharedPointer<QList<QSharedPointer<FileSet> > > fileSets) const
{
	QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
	ParameterValidator2014 paraValidator(parser, QSharedPointer<QList<QSharedPointer<Choice> > > ());

	if ( inexistingReferenceList(fileSets, componentInstantiation ) )
	{
		errors.append(QObject::tr("There are file set references, but no file sets."));
	}
	else
	{
		foreach ( QString fileSetRef, *componentInstantiation->getFileSetReferences() )
		{
			if ( !existingReference(fileSets, fileSetRef) )
			{
				errors.append(QObject::tr("Referenced file set %1 does not exist.").arg(fileSetRef));
			}
		}
	}

	foreach ( QSharedPointer<FileBuilder> currentFileBuilder, *componentInstantiation->getDefaultFileBuilders() )
	{
		if ( !hasValidName( currentFileBuilder->getFileType() ) )
		{
			errors.append(QObject::tr("The type of default file builder is empty."));
		}

		if ( !currentFileBuilder->getReplaceDefaultFlags().isEmpty() &&
			!parser->isValidExpression( currentFileBuilder->getReplaceDefaultFlags() ) )
		{
			errors.append(QObject::tr("\"Replace default flags\" of default file builder is an invalid expression: %1"
				).arg(currentFileBuilder->getReplaceDefaultFlags()));
		}
	}

	foreach ( QSharedPointer<ModuleParameter> parameter, *componentInstantiation->getModuleParameters() )
	{
		paraValidator.findErrorsIn(errors,parameter,contex);

		if ( !isValidPresence(parameter, parser) )
		{
			errors.append(QObject::tr("The presence of module parameter %1 is invalid: %2")
				.arg(parameter->name()).arg(parameter->getIsPresent()));
		}

		if ( !isValidUsageType(parameter) )
		{
			errors.append(QObject::tr("The usage of module parameter %1 is invalid: %2")
				.arg(parameter->name()).arg(parameter->getUsageType()));
		}
	}

	foreach ( QSharedPointer<Parameter> parameter, *componentInstantiation->getParameters() )
	{
		paraValidator.findErrorsIn(errors,parameter,contex);
	}
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
// Function: InstantiationsValidator::inexistingReferenceList()
//-----------------------------------------------------------------------------
bool InstantiationsValidator::inexistingReferenceList(QSharedPointer<QList<QSharedPointer<FileSet> > > fileSet,
	QSharedPointer<ComponentInstantiation> componentInstantiation) const
{
	return ( ( !fileSet || fileSet->count() < 1 ) && componentInstantiation->getFileSetReferences()->count() > 0 );
}

//-----------------------------------------------------------------------------
// Function: InstantiationsValidator::existingReference()
//-----------------------------------------------------------------------------
bool InstantiationsValidator::existingReference(QSharedPointer<QList<QSharedPointer<FileSet> > > fileSets, QString fileSetRef) const
{
	foreach ( QSharedPointer<FileSet> fileSet, *fileSets )
	{
		if ( fileSetRef == fileSet->name() )
		{
			return true;
		}
	}

	return false;
}

//-----------------------------------------------------------------------------
// Function: InstantiationsValidator::isValidUsageType()
//-----------------------------------------------------------------------------
bool InstantiationsValidator::isValidUsageType(QSharedPointer<ModuleParameter> parameter) const
{
	if ( !parameter->getUsageType().isEmpty() && parameter->getUsageType() != "nontyped"
		&& parameter->getUsageType() != "typed" )
	{
		return false;
	}
}

//-----------------------------------------------------------------------------
// Function: InstantiationsValidator::isValidPresence()
//-----------------------------------------------------------------------------
bool InstantiationsValidator::isValidPresence(QSharedPointer<ModuleParameter> parameter,
	QSharedPointer<ExpressionParser> parser) const
{
	if ( !parameter->getIsPresent().isEmpty() && !parser->isValidExpression( parameter->getIsPresent() ) )
	{
		return false;
	}
}
