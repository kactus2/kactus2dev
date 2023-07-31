//-----------------------------------------------------------------------------
// File: tst_CatalogValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 01.02.2017
//
// Description:
// Unit test for class CatalogValidator
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Catalog/Catalog.h>
#include <IPXACTmodels/Catalog/IpxactFile.h>
#include <IPXACTmodels/Catalog/validators/CatalogValidator.h>

#include <QtTest>

class tst_CatalogValidator : public QObject
{
	Q_OBJECT

public:
    tst_CatalogValidator();

private slots:

	void testMandatoryFields();

    void testMandatoryFieldsInFile();

private:

};

//-----------------------------------------------------------------------------
// Function: tst_CatalogValidator::tst_CatalogValidator()
//-----------------------------------------------------------------------------
tst_CatalogValidator::tst_CatalogValidator()
{

}

//-----------------------------------------------------------------------------
// Function: tst_CatalogValidator::testMandatoryFields()
//-----------------------------------------------------------------------------
void tst_CatalogValidator::testMandatoryFields()
{
    QSharedPointer<Catalog> testCatalog(new Catalog(VLNV(), Document::Revision::Std14));

    CatalogValidator testValidator;
    QCOMPARE(testValidator.validate(testCatalog), false);

    QVector<QString> errorList;
    testValidator.findErrorsIn(errorList, testCatalog);

    QCOMPARE(errorList.size(), 1);
    QCOMPARE(errorList.first(), QString("The VLNV ::: is invalid."));

    testCatalog->setVlnv(VLNV(VLNV::CATALOG, "tut.fi", "TestLibrary", "TestCatalog", "1.0"));
    QCOMPARE(testValidator.validate(testCatalog), true);
}

//-----------------------------------------------------------------------------
// Function: tst_CatalogValidator::testMandatoryFieldsInFile()
//-----------------------------------------------------------------------------
void tst_CatalogValidator::testMandatoryFieldsInFile()
{
    QSharedPointer<Catalog> testCatalog(new Catalog(VLNV(VLNV::CATALOG, "tut.fi", "TestLibrary", "TestCatalog", "1.0"), Document::Revision::Std14));

    QSharedPointer<IpxactFile> testFile(new IpxactFile());

    testCatalog->getAbstractors()->append(testFile);

    CatalogValidator testValidator;
    QCOMPARE(testValidator.validate(testCatalog), false);

    QVector<QString> errorList;
    testValidator.findErrorsIn(errorList, testCatalog);

    QCOMPARE(errorList.size(), 6);
    //QCOMPARE(errorList.first(), QString("The VLNV ::: is invalid for a file within catalog tut.fi:TestLibrary:TestCatalog:1.0."));
    QCOMPARE(errorList.last(), QString("File name must not be empty for abstractor within catalog tut.fi:TestLibrary:TestCatalog:1.0."));

    testFile->setVlnv(VLNV(VLNV::ABSTRACTOR, "tut.fi", "TestLibrary", "TestAbstractor", "1.0"));
  
    errorList.clear();
    testValidator.findErrorsIn(errorList, testCatalog);

    QCOMPARE(testValidator.validate(testCatalog), false);
    QCOMPARE(errorList.size(), 1);
    QCOMPARE(errorList.first(), QString("File name must not be empty for abstractor within catalog tut.fi:TestLibrary:TestCatalog:1.0."));

    testFile->setName("./test.xml");
   
    errorList.clear();
    testValidator.findErrorsIn(errorList, testCatalog);

    QCOMPARE(testValidator.validate(testCatalog), true);
    QCOMPARE(errorList.size(), 0);
}


QTEST_APPLESS_MAIN(tst_CatalogValidator)


#include "tst_CatalogValidator.moc"
