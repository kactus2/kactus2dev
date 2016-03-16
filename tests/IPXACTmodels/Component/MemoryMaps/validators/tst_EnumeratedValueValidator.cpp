//-----------------------------------------------------------------------------
// File: tst_EnumeratedValueValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 17.11.2015
//
// Description:
// Unit test for class EnumeratedValueValidator.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/validators/EnumeratedValueValidator.h>
#include <IPXACTmodels/Component/EnumeratedValue.h>

#include <editors/ComponentEditor/common/SystemVerilogExpressionParser.h>

#include <QtTest>

class tst_EnumeratedValueValidator : public QObject
{
    Q_OBJECT

public:
    tst_EnumeratedValueValidator();

private slots:

    void testNameIsValid();
    void testNameIsValid_data();

    void testValueIsValid();
    void testValueIsValid_data();

private:

    bool errorIsNotFoundInErrorList(QString const& expectedError, QVector<QString> errorList);
};

//-----------------------------------------------------------------------------
// Function: tst_EnumeratedValueValidator::tst_EnumeratedValueValidator()
//-----------------------------------------------------------------------------
tst_EnumeratedValueValidator::tst_EnumeratedValueValidator()
{
}

//-----------------------------------------------------------------------------
// Function: tst_EnumeratedValueValidator::()
//-----------------------------------------------------------------------------
void tst_EnumeratedValueValidator::testNameIsValid()
{
    QFETCH(QString, name);
    QFETCH(QString, value);
    QFETCH(bool, isValid);

    QSharedPointer<EnumeratedValue> enumeratedValue (new EnumeratedValue(name, value));

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());

    EnumeratedValueValidator validator(parser);
    QCOMPARE(validator.validate(enumeratedValue), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, enumeratedValue, "test");

        QString expectedError =
            QObject::tr("Invalid name specified for %1 within %2").arg(enumeratedValue->name(), "test");
        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_EnumeratedValueValidator::testUsageIsValid_data()
//-----------------------------------------------------------------------------
void tst_EnumeratedValueValidator::testNameIsValid_data()
{
    QTest::addColumn<QString>("name");
    QTest::addColumn<QString>("value");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Name test is valid for enumerated value") << "test" << "1" << true;
    QTest::newRow("Empty name is invalid for enumerated value") << "" << "1" << false;
    QTest::newRow("Name consisting of only white spaces is invalid for enumerated value") << "    " << "1" << false;
    QTest::newRow("Name consisting of characters and white spaces is valid.") << "  test  " << "1" << true;
}

//-----------------------------------------------------------------------------
// Function: tst_EnumeratedValueValidator::testValueIsValid()
//-----------------------------------------------------------------------------
void tst_EnumeratedValueValidator::testValueIsValid()
{
    QFETCH(QString, value);
    QFETCH(bool, isValid);

    QSharedPointer<EnumeratedValue> enumeratedValue (new EnumeratedValue("test", value));

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());

    EnumeratedValueValidator validator(parser);
    QCOMPARE(validator.hasValidValue(enumeratedValue), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, enumeratedValue, "test");

        QString expectedError = QObject::tr("Invalid value %1 set for enumerated value %2 within test").
            arg(enumeratedValue->getValue()).arg(enumeratedValue->name());

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_EnumeratedValueValidator::testValueIsValid_data()
//-----------------------------------------------------------------------------
void tst_EnumeratedValueValidator::testValueIsValid_data()
{
    QTest::addColumn<QString>("value");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Empty value is invalid.") << "" << false;
    QTest::newRow("Value 1 is valid.") << "1" << true;
}

//-----------------------------------------------------------------------------
// Function: tst_EnumeratedValueValidator::errorIsNotFoundInErrorList()
//-----------------------------------------------------------------------------
bool tst_EnumeratedValueValidator::errorIsNotFoundInErrorList(QString const& expectedError,
    QVector<QString> errorList)
{
    if (!errorList.contains(expectedError))
    {
        qDebug() << "The following error:" << endl << expectedError << endl << "was not found in errorlist:";
        foreach(QString error, errorList)
        {
            qDebug() << error; 
        }
        return true;
    }

    return false;
}

QTEST_APPLESS_MAIN(tst_EnumeratedValueValidator)

#include "tst_EnumeratedValueValidator.moc"
