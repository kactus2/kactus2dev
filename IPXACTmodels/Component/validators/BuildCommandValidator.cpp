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
// Function: SystemVerilogValidator::SystemVerilogValidator()
//-----------------------------------------------------------------------------
FileValidator::FileValidator()
{

}

//-----------------------------------------------------------------------------
// Function: SystemVerilogValidator::~SystemVerilogValidator()
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

	if ( !hasValidName(file->name() ) )
	{
		return false;
	}
	
	if ( !file->getIsPresent().isEmpty() &&
		!parser->isValidExpression(file->getIsPresent() ) )
	{
		return false;
	}
	
	if (file->getFileTypes()->count() > 0 )
	{
		return false;
	}

	foreach ( QSharedPointer<NameValuePair> currentPair, file->getDefines() )
	{
		if ( !hasValidName( currentPair->name() ) )
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
    QString const&) const
{
	QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());

	if ( !hasValidName(file->name() ) )
	{
		errors.append(QObject::tr("The name is invalid or in-existing: %1").arg(file->name()));
	}

	if ( !File->getIsPresent().isEmpty() && !parser->isValidExpression(file->getIsPresent() ) )
	{
		errors.append(QObject::tr("The presence is invalid: %1").arg(file->getIsPresent()));
	}
	
	if (file->getInterfaces().count() < 2 )
	{
		errors.append(QObject::tr("Must have at least two bus interface references."));
	}

	foreach ( QString currentRef, file->getInterfaces() )
	{
		if ( !hasValidName( currentRef ) )
		{
			errors.append(QObject::tr("A bus interface reference has an invalid name: %1"
				).arg(currentRef));
		}
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
