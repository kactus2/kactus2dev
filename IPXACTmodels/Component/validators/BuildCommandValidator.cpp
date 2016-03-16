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
bool FileValidator::validate(QSharedPointer<File> File) const
{
	QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());

	if ( !hasValidName( File->name() ) )
	{
		return false;
	}
	
	if ( !File->getIsPresent().isEmpty() &&
		!parser->isValidExpression( File->getIsPresent() ) )
	{
		return false;
	}
	
	if ( File->getFileTypes()->count() > 0 )
	{
		return false;
	}

	foreach ( QSharedPointer<NameValuePair> currentPair, File->getDefines() )
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
void FileValidator::findErrorsIn(QVector<QString>& errors, QSharedPointer<File> File,
    QString const&) const
{
	QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());

	if ( !hasValidName( File->name() ) )
	{
		errors.append(QObject::tr("The name is invalid or in-existing: %1").arg(File->name()));
	}

	if ( !File->getIsPresent().isEmpty() && !parser->isValidExpression( File->getIsPresent() ) )
	{
		errors.append(QObject::tr("The presence is invalid: %1").arg(File->getIsPresent()));
	}
	
	if ( File->getInterfaces().count() < 2 )
	{
		errors.append(QObject::tr("Must have at least two bus interface references."));
	}

	foreach ( QString currentRef, File->getInterfaces() )
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
