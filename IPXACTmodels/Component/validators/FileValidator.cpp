//-----------------------------------------------------------------------------
// File: FileValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 20.11.2015
//
// Description:
// Validator for ipxact:File.
//-----------------------------------------------------------------------------

#include "FileValidator.h"

#include <IPXACTmodels/Component/choice.h>
#include <IPXACTmodels/common/Enumeration.h>

#include <editors/ComponentEditor/common/ExpressionParser.h>
#include <editors/ComponentEditor/common/SystemVerilogExpressionParser.h>

#include <QRegularExpression>
#include <QStringList>

//-----------------------------------------------------------------------------
// Function: FileValidator::FileValidator()
//-----------------------------------------------------------------------------
FileValidator::FileValidator()
{

}

//-----------------------------------------------------------------------------
// Function: FileValidator::~FileValidator()
//-----------------------------------------------------------------------------
FileValidator::~FileValidator()
{

}

//-----------------------------------------------------------------------------
// Function: FileValidator::validate()
//-----------------------------------------------------------------------------
bool FileValidator::validate(QSharedPointer<File> file) const
{
	QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());

	if ( !hasValidName( file->name() ) )
	{
		return false;
	}
	
	if ( !file->getIsPresent().isEmpty() &&
		!parser->isValidExpression( file->getIsPresent() ) )
	{
		return false;
	}
	
	if ( file->getFileTypes()->count() < 1 )
	{
		return false;
	}

	foreach ( QSharedPointer<NameValuePair> currentPair, *file->getDefines() )
	{
		if ( !hasValidName( currentPair->name() ) )
		{
			return false;
		}
	}

	if ( file->getBuildCommand() && !file->getBuildCommand()->getReplaceDefaultFlags().isEmpty() &&
		!parser->isValidExpression( file->getBuildCommand()->getReplaceDefaultFlags() ) )
	{
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Function: FileValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void FileValidator::findErrorsIn(QVector<QString>& errors, QSharedPointer<File> file,
    QString const&) const
{
	QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());

	if ( !hasValidName( file->name() ) )
	{
		errors.append(QObject::tr("The name is invalid or in-existing: %1").arg(file->name()));
	}

	if ( !file->getIsPresent().isEmpty() &&
		!parser->isValidExpression( file->getIsPresent() ) )
	{
		errors.append(QObject::tr("The presence is invalid: %1").arg(file->getIsPresent()));
	}

	if ( file->getFileTypes()->count() < 1 )
	{
		errors.append(QObject::tr("Must have at least one file type."));
	}

	foreach ( QSharedPointer<NameValuePair> currentPair, *file->getDefines() )
	{
		if ( !hasValidName( currentPair->name() ) )
		{
			errors.append(QObject::tr("The name of a name-value pair is invalid or in-existing: %1"
				).arg(file->name()));
		}
	}

	if ( file->getBuildCommand() && !file->getBuildCommand()->getReplaceDefaultFlags().isEmpty() &&
		!parser->isValidExpression( file->getBuildCommand()->getReplaceDefaultFlags() ) )
	{
		errors.append(QObject::tr("\"Replace default flags\" of build command is an invalid expression: %1"
			).arg(file->getBuildCommand()->getReplaceDefaultFlags()));
	}
}

//-----------------------------------------------------------------------------
// Function: EnumeratedValueValidator::hasValidName()
//-----------------------------------------------------------------------------
bool FileValidator::hasValidName(QString const& name) const
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
