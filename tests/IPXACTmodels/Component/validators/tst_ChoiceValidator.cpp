//-----------------------------------------------------------------------------
// File: tst_ChoiceValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 07.12.2015
//
// Description:
// Unit test for class ChoiceValidator.
//-----------------------------------------------------------------------------

#include <editors/ComponentEditor/common/SystemVerilogExpressionParser.h>

#include <IPXACTmodels/Component/validators/ChoiceValidator.h>

#include <IPXACTmodels/Component/Choice.h>
#include <IPXACTmodels/common/Enumeration.h>

#include <QtTest>

class tst_ChoiceValidator : public QObject
{
    Q_OBJECT

public:
    tst_ChoiceValidator();

private slots:

    void testHasValidName();
    void testHasValidName_data();

    void testHasValidEnumerations();

private:

    bool errorIsNotFoundInErrorList(QString const& expectedError, QVector<QString> errorList);
};

//-----------------------------------------------------------------------------
// Function: tst_ChoiceValidator::tst_ChoiceValidator()
//-----------------------------------------------------------------------------
tst_ChoiceValidator::tst_ChoiceValidator()
{
}

//-----------------------------------------------------------------------------
// Function: tst_ChoiceValidator::testHasValidName()
//-----------------------------------------------------------------------------
void tst_ChoiceValidator::testHasValidName()
{
    QFETCH(QString, name);
    QFETCH(bool, isValid);

    QSharedPointer<Choice> testChoice (new Choice());
    testChoice->setName(name);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    ChoiceValidator validator(parser);
    QCOMPARE(validator.hasValidName(testChoice), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testChoice, "test");

        QString expectedError = QObject::tr("Invalid name set for choice %1 within %2")
            .arg(testChoice->name()).arg("test");
        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_ChoiceValidator::testHasValidName_data()
//-----------------------------------------------------------------------------
void tst_ChoiceValidator::testHasValidName_data()
{
    QTest::addColumn<QString>("name");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Name test is valid") << "test" << true;
    QTest::newRow("Empty name is invalid") << "" << false;
    QTest::newRow("Name consisting of only white spaces is invalid") << "    " << false;
    QTest::newRow("Name consisting of characters and white spaces is valid") << "  test  " << true;
}

//-----------------------------------------------------------------------------
// Function: tst_ChoiceValidator::testHasValidEnumerations()
//-----------------------------------------------------------------------------
void tst_ChoiceValidator::testHasValidEnumerations()
{
    QSharedPointer<Choice> testChoice (new Choice());
    testChoice->setName("testChoice");

    QSharedPointer<ExpressionParser> parser (new SystemVerilogExpressionParser());
    ChoiceValidator validator(parser);

    QCOMPARE(validator.hasValidEnumerations(testChoice), false);

    QVector<QString> foundErrors;
    validator.findErrorsIn(foundErrors, testChoice, "test");

    QString expectedError = QObject::tr("No enumerations found in choice '%1' within %2")
        .arg(testChoice->name()).arg("test");
    if (errorIsNotFoundInErrorList(expectedError, foundErrors))
    {
        QFAIL("No error message found");
    }

    QSharedPointer<Enumeration> testEnumeration (new Enumeration());
    testChoice->enumerations()->append(testEnumeration);

    QCOMPARE(validator.hasValidEnumerations(testChoice), false);

    foundErrors.clear();
    validator.findErrorsIn(foundErrors, testChoice, "test");

    expectedError = QObject::tr("Invalid value '%1' set for enumeration in choice '%2' within %3")
        .arg(testEnumeration->getValue()).arg(testChoice->name()).arg("test");
    if (errorIsNotFoundInErrorList(expectedError, foundErrors))
    {
        QFAIL("No error message found");
    }

    testEnumeration->setValue("4+4");

    QCOMPARE(validator.hasValidEnumerations(testChoice), true);

    QSharedPointer<Enumeration> otherEnumeration (new Enumeration());
    otherEnumeration->setValue("10");
    testChoice->enumerations()->append(otherEnumeration);

    QCOMPARE(validator.hasValidEnumerations(testChoice), true);
}

//-----------------------------------------------------------------------------
// Function: tst_ChoiceValidator::errorIsNotFoundInErrorList()
//-----------------------------------------------------------------------------
bool tst_ChoiceValidator::errorIsNotFoundInErrorList(QString const& expectedError, QVector<QString> errorList)
{
    if (!errorList.contains(expectedError))
    {
        qDebug() << "The following error:" << endl << expectedError << endl << "was not found in error list:";
        foreach(QString error, errorList)
        {
            qDebug() << error;
        }
        return true;
    }

    return false;
}

QTEST_APPLESS_MAIN(tst_ChoiceValidator)

#include "tst_ChoiceValidator.moc"
