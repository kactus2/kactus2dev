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

#include <editors/ComponentEditor/common/SystemVerilogExpressionParser.h>

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
	file->getFileTypes()->append("text");
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

	file->getFileTypes()->append("text");
	file->getDefines()->append(QSharedPointer<NameValuePair>(new NameValuePair("Lihapullia", "viis") ));

	QVector<QString> errorList;
	validator.findErrorsIn(errorList, file, "test");

	QCOMPARE( errorList.size(), 1 );
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

	file->getFileTypes()->append("text");
	file->getDefines()->append(QSharedPointer<NameValuePair>(new NameValuePair(" \t\r\n", "viis") ));

	QVector<QString> errorList;
	validator.findErrorsIn(errorList, file, "test");

	QCOMPARE( errorList.size(), 1 );
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

	file->getFileTypes()->append("text");
	QSharedPointer<BuildCommand> bc( new BuildCommand );
	bc->setReplaceDefaultFlags("rertthyju");
	file->setBuildcommand( bc );

	QVector<QString> errorList;
	validator.findErrorsIn(errorList, file, "test");

	QCOMPARE( errorList.size(), 1 );
	QVERIFY( !validator.validate(file) );
}

QTEST_APPLESS_MAIN(tst_FileValidator)

#include "tst_FileValidator.moc"
