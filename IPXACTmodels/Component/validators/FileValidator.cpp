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

#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <IPXACTmodels/Component/File.h>

#include <IPXACTmodels/Component/BuildCommand.h>

#include <QRegularExpression>
#include <QStringList>

//-----------------------------------------------------------------------------
// Function: FileValidator::FileValidator()
//-----------------------------------------------------------------------------
FileValidator::FileValidator(QSharedPointer<ExpressionParser> expressionParser):
expressionParser_(expressionParser)
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
	if (!hasValidName( file->name()))
	{
		return false;
	}
	
	if (!file->getIsPresent().isEmpty() && !expressionParser_->isValidExpression(file->getIsPresent()))
	{
		return false;
	}
	
	if (file->getFileTypes()->count() < 1)
	{
		return false;
	}

	foreach (QSharedPointer<NameValuePair> currentPair, *file->getDefines())
	{
		if (!hasValidName(currentPair->name()))
		{
			return false;
		}
	}

    if (file->getBuildCommand() && !buildCommandHasValidReplaceDefaultFlags(file->getBuildCommand()))
    {
        return false;
    }

	return true;
}

//-----------------------------------------------------------------------------
// Function: FileValidator::buildCommandHasValidReplaceDefaultFlags()
//-----------------------------------------------------------------------------
bool FileValidator::buildCommandHasValidReplaceDefaultFlags(QSharedPointer<BuildCommand> buildCommand) const
{
    if (!buildCommand->getReplaceDefaultFlags().isEmpty())
    {
        bool canConvert = true;
        int replaceFlags =
            expressionParser_->parseExpression(buildCommand->getReplaceDefaultFlags()).toInt(&canConvert);

        if (!canConvert || replaceFlags < 0 || replaceFlags > 1)
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: FileValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void FileValidator::findErrorsIn(QVector<QString>& errors, QSharedPointer<File> file,
    QString const& context) const
{
	if (!hasValidName(file->name()))
	{
		errors.append(QObject::tr("The file name '%1' is invalid within %2.").arg(file->name(), context));
	}

	if (!file->getIsPresent().isEmpty() && !expressionParser_->isValidExpression(file->getIsPresent()))
	{
		errors.append(QObject::tr("The presence '%1' is invalid in file %2.").arg(file->getIsPresent(), 
            file->name()));
	}

	if (file->getFileTypes()->count() < 1)
	{
		errors.append(QObject::tr("File %1 must have at least one file type defined.").arg(file->name()));
	}

	foreach (QSharedPointer<NameValuePair> currentPair, *file->getDefines())
	{
		if (!hasValidName(currentPair->name()))
		{
			errors.append(QObject::tr("The name '%1' of a define is invalid within file %2."
				).arg(currentPair->name(), file->name()));
		}
	}

    if (file->getBuildCommand() && !buildCommandHasValidReplaceDefaultFlags(file->getBuildCommand()))
	{
        errors.append(QObject::tr("Invalid \"replace default flags\" value set for build command in file %1."
            " Value must evaluate to 0 or 1.").
            arg(file->name()));
	}
}

//-----------------------------------------------------------------------------
// Function: EnumeratedValueValidator::hasValidName()
//-----------------------------------------------------------------------------
bool FileValidator::hasValidName(QString const& name) const
{
	QRegularExpression whiteSpaceExpression("^\\s*$");

	if (name.isEmpty() || whiteSpaceExpression.match(name).hasMatch())
	{
		return false;
	}

	return true;
}
