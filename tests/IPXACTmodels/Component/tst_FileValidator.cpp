//-----------------------------------------------------------------------------
// File: tst_FileValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 20.11.2015
//
// Description:
// Unit test for class FileValidator.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/validators/FileValidator.h>
#include <IPXACTmodels/Component/File.h>

#include <KactusAPI/include/SystemVerilogExpressionParser.h>

#include <QtTest>
#include <QSharedPointer>

class tst_FileValidator : public QObject
{
	Q_OBJECT

public:
    tst_FileValidator();

private slots:
	void baseCase();
	void failFileTypes();
	void failName();
	void failDefine();
	void failReplace();

private:
};

//-----------------------------------------------------------------------------
// Function: tst_FileValidator::tst_FileValidator()
//-----------------------------------------------------------------------------
tst_FileValidator::tst_FileValidator()
{

}

//-----------------------------------------------------------------------------
// Function: tst_FileValidator::baseCase()
//-----------------------------------------------------------------------------
void tst_FileValidator::baseCase()
{
	QSharedPointer<File> file(new File());
	FileValidator validator(QSharedPointer<ExpressionParser>(new SystemVerilogExpressionParser()));
	file->setName("ruokalista.txt");

	file->setIsPresent("1");
	file->getFileTypes()->append(FileType("text"));
	file->getDefines()->append(QSharedPointer<NameValuePair>(new NameValuePair("Lihapullia","viis") ));
	
	QSharedPointer<BuildCommand> bc( new BuildCommand );
	bc->setReplaceDefaultFlags("1");
	file->setBuildcommand( bc );

	QVector<QString> errorList;
	validator.findErrorsIn(errorList, file, "test");

	QCOMPARE( errorList.size(), 0 );
	QVERIFY( validator.validate(file) );
}

//-----------------------------------------------------------------------------
// Function: tst_FileValidator::failFileTypes()
//-----------------------------------------------------------------------------
void tst_FileValidator::failFileTypes()
{
	QSharedPointer<File> file( new File );
	FileValidator validator(QSharedPointer<ExpressionParser>(new SystemVerilogExpressionParser()));
	file->setName("ruokalista.txt");

	QVector<QString> errorList;
	validator.findErrorsIn(errorList, file, "test");

	QCOMPARE( errorList.size(), 1 );
	QCOMPARE( errorList.first(), QString("File ruokalista.txt must have at least one file type defined."));
	QVERIFY( !validator.validate(file) );
}

//-----------------------------------------------------------------------------
// Function: tst_FileValidator::failName()
//-----------------------------------------------------------------------------
void tst_FileValidator::failName()
{
	QSharedPointer<File> file( new File );
    FileValidator validator(QSharedPointer<ExpressionParser>(new SystemVerilogExpressionParser()));
	file->setName(" \t\n\r");

	file->getFileTypes()->append(FileType("text"));
	file->getDefines()->append(QSharedPointer<NameValuePair>(new NameValuePair("Lihapullia", "viis") ));

	QVector<QString> errorList;
	validator.findErrorsIn(errorList, file, "test");

	QCOMPARE( errorList.size(), 1 );
	QCOMPARE(errorList.first(), QString("The file name ' \t\n\r' is invalid within test."));
	QVERIFY( !validator.validate(file) );
}

//-----------------------------------------------------------------------------
// Function: tst_FileValidator::failDefine()
//-----------------------------------------------------------------------------
void tst_FileValidator::failDefine()
{
	QSharedPointer<File> file( new File );
	FileValidator validator(QSharedPointer<ExpressionParser>(new SystemVerilogExpressionParser()));
	file->setName("joku.txt");

	file->getFileTypes()->append(FileType("text"));
	file->getDefines()->append(QSharedPointer<NameValuePair>(new NameValuePair(" \t\r\n", "viis") ));

	QVector<QString> errorList;
	validator.findErrorsIn(errorList, file, "test");

	QCOMPARE( errorList.size(), 1 );
	QCOMPARE( errorList.first(), QString("The name ' \t\r\n' of a define is invalid within file joku.txt."));
	QVERIFY( !validator.validate(file) );
}

//-----------------------------------------------------------------------------
// Function: tst_FileValidator::failReplace()
//-----------------------------------------------------------------------------
void tst_FileValidator::failReplace()
{
	QSharedPointer<File> file( new File );
    FileValidator validator(QSharedPointer<ExpressionParser>(new SystemVerilogExpressionParser()));
	file->setName("joku.txt");

	file->getFileTypes()->append(FileType("text"));
	QSharedPointer<BuildCommand> bc( new BuildCommand );
	bc->setReplaceDefaultFlags("rertthyju");
	file->setBuildcommand( bc );

	QVector<QString> errorList;
	validator.findErrorsIn(errorList, file, "test");

	QCOMPARE( errorList.size(), 1 );
	QCOMPARE( errorList.first(), 
		QString("Invalid \"replace default flags\" value set for build command in file joku.txt. Value must evaluate to 0 or 1."));
	QVERIFY( !validator.validate(file) );
}

QTEST_APPLESS_MAIN(tst_FileValidator)

#include "tst_FileValidator.moc"
