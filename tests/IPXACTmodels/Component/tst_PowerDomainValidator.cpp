//-----------------------------------------------------------------------------
// File: tst_PowerDomainvalidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 6.10.2023
//
// Description:
// Unit test for class PowerDomainValidator
//-----------------------------------------------------------------------------

#include <KactusAPI/include/SystemVerilogExpressionParser.h>

#include <IPXACTmodels/Component/validators/PowerDomainValidator.h>
#include <IPXACTmodels/common/validators/ParameterValidator.h>

#include <IPXACTmodels/Component/PowerDomain.h>
#include <IPXACTmodels/common/Parameter.h>

#include <QtTest>

class tst_PowerDomainValidator : public QObject
{
    Q_OBJECT

public:
    tst_PowerDomainValidator();

private slots:
  
    void testHasValidName();
    void testHasValidName_data();

    void testHasValidAlwaysOn();
    void testHasValidAlwaysOn_data();

    void testHasValidSubDomain();
    void testHasValidSubDomain_data();

    void testHasValidParameters();

private:

    bool errorIsNotFoundInErrorList(QString const& expectedError, QVector<QString> errorList);

    QSharedPointer<PowerDomainValidator> createValidator();
};

//-----------------------------------------------------------------------------
// Function: tst_PowerDomainValidator::tst_PowerDomainValidator()
//-----------------------------------------------------------------------------
tst_PowerDomainValidator::tst_PowerDomainValidator()
{

}

//-----------------------------------------------------------------------------
// Function: tst_PowerDomainValidator::testHasValidName()
//-----------------------------------------------------------------------------
void tst_PowerDomainValidator::testHasValidName()
{
    QFETCH(QString, name);
    QFETCH(bool, isValid);

    QSharedPointer<PowerDomain> testDomain(new PowerDomain());
    testDomain->setName(name);

    QSharedPointer<PowerDomainValidator> validator = createValidator();

    QCOMPARE(validator->hasValidName(testDomain->name()), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testDomain, "test");

        QString expectedError = QObject::tr("Invalid name specified for power domain %1 within %2")
            .arg(testDomain->name(), "test");
        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_PowerDomainValidator::testHasValidName_data()
//-----------------------------------------------------------------------------
void tst_PowerDomainValidator::testHasValidName_data()
{
    QTest::addColumn<QString>("name");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Name test is valid") << "test" << true;
    QTest::newRow("Empty name is invalid") << "" << false;
    QTest::newRow("Name consisting of only white spaces is invalid") << "    " << false;
    QTest::newRow("Name consisting of characters and white spaces is valid") << "  test  " << true;
}

//-----------------------------------------------------------------------------
// Function: tst_PowerDomainValidator::testHasValidAlwaysOn()
//-----------------------------------------------------------------------------
void tst_PowerDomainValidator::testHasValidAlwaysOn()
{
    QFETCH(QString, alwaysOn);
    QFETCH(bool, isValid);

    QSharedPointer<PowerDomain> testDomain (new PowerDomain());
    testDomain->setName("testDomain");
    testDomain->setAlwaysOn(alwaysOn);

    QSharedPointer<PowerDomainValidator> validator = createValidator();

    QCOMPARE(validator->hasValidAlwaysOn(testDomain), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testDomain, "test");

        QString expectedError = 
            QObject::tr("Invalid expression set for always on in power domain testDomain within test");
        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_PowerDomainValidator::testHasValidAlwaysOn_data()
//-----------------------------------------------------------------------------
void tst_PowerDomainValidator::testHasValidAlwaysOn_data()
{
    QTest::addColumn<QString>("alwaysOn");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("1 is valid for always on") << "1" << true;
    QTest::newRow("0 is valid for always on") << "0" << true;
    QTest::newRow("Empty value is valid for always on") << "" << true;
    QTest::newRow("12*2+13-5 is not valid for always on") << "12*2+13-5" << false;
    QTest::newRow("-20 is not valid for always on") << "-20" << false;
    QTest::newRow("Text is not valid for always on") << "text" << false;
    QTest::newRow("String is not valid for always on") << "\"text\"" << false;
}

//-----------------------------------------------------------------------------
// Function: tst_PowerDomainValidator::testHasValidSubDomain()
//-----------------------------------------------------------------------------
void tst_PowerDomainValidator::testHasValidSubDomain()
{
    QFETCH(QString, subDomain);
    QFETCH(bool, isValid);

    QSharedPointer<PowerDomain> testDomain(new PowerDomain());
    testDomain->setName("testDomain");
    testDomain->setSubDomainOf(subDomain);

    QSharedPointer<PowerDomainValidator> validator = createValidator();

    QCOMPARE(validator->hasValidSubDomainOf(testDomain), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testDomain, "test");

        QString expectedError =
            QObject::tr("Invalid power domain '%1' referenced in power domain testDomain within test").arg(
                subDomain);
        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_PowerDomainValidator::testHasValidSubDomain_data()
//-----------------------------------------------------------------------------
void tst_PowerDomainValidator::testHasValidSubDomain_data()
{
    QTest::addColumn<QString>("subDomain");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Empty domain is valid") << "" << true;
    QTest::newRow("Non-existent domain is not valid") << "None" << false;
    QTest::newRow("Existing domain is valid") << "main" << true;
}

//-----------------------------------------------------------------------------
// Function: tst_PowerDomainValidator::testHasValidParameters()
//-----------------------------------------------------------------------------
void tst_PowerDomainValidator::testHasValidParameters()
{
    QSharedPointer<Parameter> testParameter (new Parameter());
    testParameter->setValueId("Sanger");
    testParameter->setName("Daizengar");
    testParameter->setValue("");
    testParameter->setType("int");

    QSharedPointer<PowerDomain> testDomain (new PowerDomain());
    testDomain->setName("testDomain");
    testDomain->getParameters()->append(testParameter);

    QSharedPointer<PowerDomainValidator> validator = createValidator();
    QCOMPARE(validator->hasValidParameters(testDomain), false);

    QVector<QString> errorsFound;
    validator->findErrorsIn(errorsFound, testDomain, "test");

    QString expectedError = QObject::tr("No value specified for %1 %2 within power domain %3").
        arg(testParameter->elementName()).arg(testParameter->name(), testDomain->name());

    if (errorIsNotFoundInErrorList(expectedError, errorsFound))
    {
        QFAIL("Error was not found");
    }

    testParameter->setValue("20");
    QCOMPARE(validator->hasValidParameters(testDomain), true);

    QSharedPointer<Parameter> otherParameter (new Parameter(*testParameter));
    otherParameter->setValue("2");
    testDomain->getParameters()->append(otherParameter);

    QCOMPARE(validator->hasValidParameters(testDomain), false);

    errorsFound.clear();
    validator->findErrorsIn(errorsFound, testDomain, "test");
    expectedError = QObject::tr("Name %1 of parameters in power domain %2 is not unique.")
        .arg(otherParameter->name(), testDomain->name());
    if (errorIsNotFoundInErrorList(expectedError, errorsFound))
    {
        QFAIL("Error was not found");
    }
}

//-----------------------------------------------------------------------------
// Function: tst_PowerDomainValidator::errorIsNotFoundInErrorList()
//-----------------------------------------------------------------------------
bool tst_PowerDomainValidator::errorIsNotFoundInErrorList(QString const& expectedError,
    QVector<QString> errorList)
{
    if (!errorList.contains(expectedError))
    {
        qDebug() << "The following error:" << Qt::endl << expectedError << Qt::endl << "was not found in error list:";
        for (QString const& error : errorList)
        {
            qDebug() << error;
        }
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: tst_PowerDomainValidator::createValidator()
//-----------------------------------------------------------------------------
QSharedPointer<PowerDomainValidator> tst_PowerDomainValidator::createValidator()
{
    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator> parameterValidator(
        new ParameterValidator(parser, QSharedPointer<QList<QSharedPointer<Choice> > >(), Document::Revision::Std22));

    QSharedPointer<QList<QSharedPointer<PowerDomain> > > componentDomains(new QList<QSharedPointer<PowerDomain> >());
    QSharedPointer<PowerDomain> referenceDomain(new PowerDomain());
    referenceDomain->setName("main");
    componentDomains->append(referenceDomain);

    QSharedPointer<PowerDomainValidator> validator(
        new PowerDomainValidator(componentDomains, parser, parameterValidator));

    return validator;
}


QTEST_APPLESS_MAIN(tst_PowerDomainValidator)

#include "tst_PowerDomainValidator.moc"
