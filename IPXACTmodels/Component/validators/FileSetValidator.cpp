//-----------------------------------------------------------------------------
// FileSet: FileSetValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 20.11.2015
//
// Description:
// Validator for ipxact:FileSet.
//-----------------------------------------------------------------------------

#include "FileSetValidator.h"
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
FileSetValidator::FileSetValidator()
{

}

//-----------------------------------------------------------------------------
// Function: SystemVerilogValidator::~SystemVerilogValidator()
//-----------------------------------------------------------------------------
FileSetValidator::~FileSetValidator()
{

}

//-----------------------------------------------------------------------------
// Function: FileSetValidator::validate()
//-----------------------------------------------------------------------------
bool FileSetValidator::validate(QSharedPointer<FileSet> fileSet) const
{
	QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());

	if ( !hasValidName( fileSet->name() ) )
	{
		return false;
	}

	foreach ( QSharedPointer<FileBuilder> currentFileBuilder, *fileSet->getDefaultFileBuilders() )
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

	FileValidator fileValidator;

	foreach( QSharedPointer<File> file, *fileSet->getFiles() )
	{
		if ( !fileValidator.validate( file ) )
		{
			return false;
		}
	}

	return true;
}

//-----------------------------------------------------------------------------
// Function: FileSetValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void FileSetValidator::findErrorsIn(QVector<QString>& errors, QSharedPointer<FileSet> fileSet,
    QString const& context) const
{
	QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());

	if ( !hasValidName( fileSet->name() ) )
	{
		errors.append(QObject::tr("The name is invalid or in-existing: %1").arg(fileSet->name()));
	}

	foreach ( QSharedPointer<FileBuilder> currentFileBuilder, *fileSet->getDefaultFileBuilders() )
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

	FileValidator fileValidator;

	foreach( QSharedPointer<File> file, *fileSet->getFiles() )
	{
		fileValidator.findErrorsIn( errors, file, context );
	}
}

//-----------------------------------------------------------------------------
// Function: EnumeratedValueValidator::hasValidName()
//-----------------------------------------------------------------------------
bool FileSetValidator::hasValidName(QString const& name) const
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
