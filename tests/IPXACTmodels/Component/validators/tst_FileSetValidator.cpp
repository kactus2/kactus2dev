//-----------------------------------------------------------------------------
// FileSet: tst_FileSetValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 20.11.2015
//
// Description:
// Unit test for class FileSetValidator.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/validators/FileValidator.h>
#include <IPXACTmodels/Component/validators/FileSetValidator.h>
#include <IPXACTmodels/Component/FileSet.h>

#include <editors/ComponentEditor/common/SystemVerilogExpressionParser.h>

#include <QtTest>

class tst_FileSetValidator : public QObject
{
	Q_OBJECT

public:
    tst_FileSetValidator();

private slots:
	void baseCase();
	void nameFail();
	void fileSetNoType();
	void fileSetBogusReplace();
	void hasValidFile();
	void hasInValidFile();

private:
    QSharedPointer<ExpressionParser> expressionParser_;
};

//-----------------------------------------------------------------------------
// Function: tst_FileSetValidator::tst_FileSetValidator()
//-----------------------------------------------------------------------------
tst_FileSetValidator::tst_FileSetValidator(): expressionParser_(new SystemVerilogExpressionParser())
{
    
}

//-----------------------------------------------------------------------------
// Function: tst_FileSetValidator::baseCase()
//-----------------------------------------------------------------------------
void tst_FileSetValidator::baseCase()
{
	QSharedPointer<FileSet> fileSet( new FileSet );
	FileSetValidator validator(QSharedPointer<FileValidator>(new FileValidator(expressionParser_)),
        expressionParser_);
	fileSet->setName("tekstit");
	
	QSharedPointer<FileBuilder> builder( new FileBuilder );
	builder->setFileType("txt");
	builder->setReplaceDefaultFlags("1");
	fileSet->getDefaultFileBuilders()->append( builder );

	QVector<QString> errorList;
	validator.findErrorsIn(errorList, fileSet, "test");

	QCOMPARE( errorList.size(), 0 );
	QVERIFY( validator.validate(fileSet) );
}

//-----------------------------------------------------------------------------
// Function: tst_FileSetValidator::nameFail()
//-----------------------------------------------------------------------------
void tst_FileSetValidator::nameFail()
{
	QSharedPointer<FileSet> fileSet( new FileSet );
    FileSetValidator validator(QSharedPointer<FileValidator>(new FileValidator(expressionParser_)),
        expressionParser_);
	fileSet->setName(" \t \r \n");

	QSharedPointer<FileBuilder> builder( new FileBuilder );
	builder->setFileType("txt");
	builder->setReplaceDefaultFlags("1");
	fileSet->getDefaultFileBuilders()->append( builder );

	QVector<QString> errorList;
	validator.findErrorsIn(errorList, fileSet, "test");

	QCOMPARE( errorList.size(), 1 );
	QVERIFY( !validator.validate(fileSet) );
}

//-----------------------------------------------------------------------------
// Function: tst_FileSetValidator::fileSetNoType()
//-----------------------------------------------------------------------------
void tst_FileSetValidator::fileSetNoType()
{
	QSharedPointer<FileSet> fileSet( new FileSet );
    FileSetValidator validator(QSharedPointer<FileValidator>(new FileValidator(expressionParser_)),
        expressionParser_);
	fileSet->setName("tekstit");

	QSharedPointer<FileBuilder> builder( new FileBuilder );
	builder->setFileType("txt");
	builder->setReplaceDefaultFlags("1");
	fileSet->getDefaultFileBuilders()->append( builder );
	QSharedPointer<FileBuilder> builder2( new FileBuilder );
	builder2->setFileType("");
	builder2->setReplaceDefaultFlags("1");
	fileSet->getDefaultFileBuilders()->append( builder2 );

	QVector<QString> errorList;
	validator.findErrorsIn(errorList, fileSet, "test");

	QCOMPARE( errorList.size(), 1 );
	QVERIFY( !validator.validate(fileSet) );
}

//-----------------------------------------------------------------------------
// Function: tst_FileSetValidator::fileSetBogusReplace()
//-----------------------------------------------------------------------------
void tst_FileSetValidator::fileSetBogusReplace()
{
	QSharedPointer<FileSet> fileSet( new FileSet );
    FileSetValidator validator(QSharedPointer<FileValidator>(new FileValidator(expressionParser_)),
        expressionParser_);
	fileSet->setName("tekstit");

	QSharedPointer<FileBuilder> builder( new FileBuilder );
	builder->setFileType("txt");
	builder->setReplaceDefaultFlags("joq");
	fileSet->getDefaultFileBuilders()->append( builder );

	QVector<QString> errorList;
	validator.findErrorsIn(errorList, fileSet, "test");

	QCOMPARE( errorList.size(), 1 );
	QVERIFY( !validator.validate(fileSet) );
}

//-----------------------------------------------------------------------------
// Function: tst_FileSetValidator::hasValidFile()
//-----------------------------------------------------------------------------
void tst_FileSetValidator::hasValidFile()
{
	QSharedPointer<FileSet> fileSet( new FileSet );
    FileSetValidator validator(QSharedPointer<FileValidator>(new FileValidator(expressionParser_)),
        expressionParser_);
	fileSet->setName("jutskat");

	QSharedPointer<File> file( new File );
	file->setName("osat.txt");

	file->setIsPresent("1");
	file->getFileTypes()->append("part");
	file->getDefines()->append(QSharedPointer<NameValuePair>(new NameValuePair("halkasija","kolme") ));

	QSharedPointer<BuildCommand> bc( new BuildCommand );
	bc->setReplaceDefaultFlags("1");
	file->setBuildcommand( bc );

	fileSet->getFiles()->append(file);

	QVector<QString> errorList;
	validator.findErrorsIn(errorList, fileSet, "test");

	QCOMPARE( errorList.size(), 0 );
	QVERIFY( validator.validate(fileSet) );
}

//-----------------------------------------------------------------------------
// Function: tst_FileSetValidator::hasInValidFile()
//-----------------------------------------------------------------------------
void tst_FileSetValidator::hasInValidFile()
{
	QSharedPointer<FileSet> fileSet( new FileSet );
    FileSetValidator validator(QSharedPointer<FileValidator>(new FileValidator(expressionParser_)),
        expressionParser_);
	fileSet->setName("jutskat");

	QSharedPointer<File> file( new File );
	file->setName("osat.txt");

	fileSet->getFiles()->append(file);

	QVector<QString> errorList;
	validator.findErrorsIn(errorList, fileSet, "test");

	QCOMPARE( errorList.size(), 1 );
	QVERIFY( !validator.validate(fileSet) );
}

QTEST_APPLESS_MAIN(tst_FileSetValidator)

#include "tst_FileSetValidator.moc"
