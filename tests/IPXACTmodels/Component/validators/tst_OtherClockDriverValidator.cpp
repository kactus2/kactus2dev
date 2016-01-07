//-----------------------------------------------------------------------------
// File: tst_OtherClockDriverValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 14.12.2015
//
// Description:
// Unit test for class OtherClockDriverValidator.
//-----------------------------------------------------------------------------

#include <editors/ComponentEditor/common/SystemVerilogExpressionParser.h>

#include <IPXACTmodels/Component/validators/OtherClockDriverValidator.h>

#include <IPXACTmodels/Component/OtherClockDriver.h>
#include <IPXACTmodels/common/ClockUnit.h>

#include <QtTest>

class tst_OtherClockDriverValidator : public QObject
{
    Q_OBJECT

public:
    tst_OtherClockDriverValidator();

private slots:

    void testHasValidName();
    void testHasValidName_data();

    void testHasValidClockPeriod();
    void testHasValidClockPeriod_data();

    void testHasValidClockPulseOffset();
    void testHasValidClockPulseOffset_data();

    void testHasValidClockPulseValue();
    void testHasValidClockPulseValue_data();

    void testHasValidClockPulseDuration();
    void testHasValidClockPulseDuration_data();

private:
    
    bool errorIsNotFoundInErrorList(QString const& expectedError, QVector<QString> errorList);
};

//-----------------------------------------------------------------------------
// Function: tst_OtherClockDriverValidator::tst_OtherClockDriverValidator()
//-----------------------------------------------------------------------------
tst_OtherClockDriverValidator::tst_OtherClockDriverValidator()
{
}

//-----------------------------------------------------------------------------
// Function: tst_OtherClockDriverValidator::testHasValidName()
//-----------------------------------------------------------------------------
void tst_OtherClockDriverValidator::testHasValidName()
{
    QFETCH(QString, name);
    QFETCH(bool, isValid);

    QSharedPointer<OtherClockDriver> testDriver (new OtherClockDriver(name));

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    OtherClockDriverValidator validator(parser);

    QCOMPARE(validator.hasValidName(testDriver->getClockName()), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testDriver, "test");

        QString expectedError = QObject::tr("Invalid name '%1' set for other clock driver within %2")
            .arg(testDriver->getClockName()).arg("test");
        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_OtherClockDriverValidator::testHasValidName_data()
//-----------------------------------------------------------------------------
void tst_OtherClockDriverValidator::testHasValidName_data()
{
    QTest::addColumn<QString>("name");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Name test is valid") << "test" << true;
    QTest::newRow("Empty name is invalid") << "" << false;
    QTest::newRow("Name consisting of only white spaces is invalid") << "    " << false;
    QTest::newRow("Name consisting of characters and white spaces is valid") << "  test  " << true;
}

//-----------------------------------------------------------------------------
// Function: tst_OtherClockDriverValidator::testHasValidClockPeriod()
//-----------------------------------------------------------------------------
void tst_OtherClockDriverValidator::testHasValidClockPeriod()
{
    QFETCH(QString, clockValue);
    QFETCH(bool, isValid);

    QSharedPointer<ClockUnit> testUnit (new ClockUnit(clockValue));
    
    QSharedPointer<OtherClockDriver> testDriver (new OtherClockDriver("MassDriver"));
    testDriver->setClockPeriod(testUnit);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    OtherClockDriverValidator validator(parser);

    QCOMPARE(validator.hasValidClockValue(testDriver->getClockPeriod()), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testDriver, "test");

        QString expectedError = QObject::tr("Invalid clock period %1 set for other clock driver %2 within %3")
            .arg(testUnit->getValue()).arg(testDriver->getClockName()).arg("test");
        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_OtherClockDriverValidator::testHasValidClockPeriod_data()
//-----------------------------------------------------------------------------
void tst_OtherClockDriverValidator::testHasValidClockPeriod_data()
{
    QTest::addColumn<QString>("clockValue");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Clock value 1+1.12 is valid") << "1+1.12" << true;
    QTest::newRow("Clock value 0 is valid") << "0" << true;
    QTest::newRow("Clock value -4.12 is valid") << "-4.12" << true;
    QTest::newRow("Empty clock value is not valid") << "" << false;
    QTest::newRow("Text is not valid for clock value") << "text" << false;
    QTest::newRow("String is not valid for clock value") << "\"text\"" << false;
}

//-----------------------------------------------------------------------------
// Function: tst_OtherClockDriverValidator::testHasValidClockPulseOffset()
//-----------------------------------------------------------------------------
void tst_OtherClockDriverValidator::testHasValidClockPulseOffset()
{
    QFETCH(QString, clockValue);
    QFETCH(bool, isValid);

    QSharedPointer<ClockUnit> testUnit (new ClockUnit(clockValue));

    QSharedPointer<OtherClockDriver> testDriver (new OtherClockDriver("MassDriver"));
    testDriver->setClockPulseOffset(testUnit);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    OtherClockDriverValidator validator(parser);

    QCOMPARE(validator.hasValidClockValue(testDriver->getClockPulseOffset()), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testDriver, "test");

        QString expectedError = QObject::tr("Invalid clock pulse offset %1 set for other clock driver %2 within %3")
            .arg(clockValue).arg(testDriver->getClockName()).arg("test");
        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_OtherClockDriverValidator::testHasValidClockPulseOffset_data()
//-----------------------------------------------------------------------------
void tst_OtherClockDriverValidator::testHasValidClockPulseOffset_data()
{
    QTest::addColumn<QString>("clockValue");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Clock value 1+1.12 is valid") << "1+1.12" << true;
    QTest::newRow("Clock value 0 is valid") << "0" << true;
    QTest::newRow("Clock value -4.12 is valid") << "-4.12" << true;
    QTest::newRow("Empty clock value is not valid") << "" << false;
    QTest::newRow("Text is not valid for clock value") << "text" << false;
    QTest::newRow("String is not valid for clock value") << "\"text\"" << false;
}

//-----------------------------------------------------------------------------
// Function: tst_OtherClockDriverValidator::testHasValidClockPulseValue()
//-----------------------------------------------------------------------------
void tst_OtherClockDriverValidator::testHasValidClockPulseValue()
{
    QFETCH(QString, pulseValue);
    QFETCH(bool, isValid);

    QSharedPointer<OtherClockDriver> testDriver (new OtherClockDriver("MassDriver"));
    testDriver->setClockPulseValue(pulseValue);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    OtherClockDriverValidator validator(parser);

    QCOMPARE(validator.hasValidClockPulseValue(testDriver), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testDriver, "test");

        QString expectedError = QObject::tr("Invalid clock pulse value %1 set for other clock driver %2 within %3")
            .arg(pulseValue).arg(testDriver->getClockName()).arg("test");
        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_OtherClockDriverValidator::testHasValidClockPulseValue_data()
//-----------------------------------------------------------------------------
void tst_OtherClockDriverValidator::testHasValidClockPulseValue_data()
{
    QTest::addColumn<QString>("pulseValue");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Clock pulse of 1 is valid") << "1" << true;
    QTest::newRow("Clock pulse of 0 is valid") << "0" << true;
    QTest::newRow("Clock pulse value that can be converted to bit vector is valid") << "24" << true;
    QTest::newRow("Clock pulse of 'b1001 is valid") << "'b1001" << true;
    QTest::newRow("Clock pulse of 0.24 is not valid") << "0.24" << false;
    QTest::newRow("Empty clock pulse is not valid") << "" << false;
    QTest::newRow("Text is not valid for clock pulse") << "text" << false;
    QTest::newRow("String is not valid for clock pulse") << "\"text\"" << false;
}

//-----------------------------------------------------------------------------
// Function: tst_OtherClockDriverValidator::testHasValidClockPulseDuration()
//-----------------------------------------------------------------------------
void tst_OtherClockDriverValidator::testHasValidClockPulseDuration()
{
    QFETCH(QString, clockValue);
    QFETCH(bool, isValid);

    QSharedPointer<ClockUnit> testUnit (new ClockUnit(clockValue));

    QSharedPointer<OtherClockDriver> testDriver (new OtherClockDriver("MassDriver"));
    testDriver->setClockPulseDuration(testUnit);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    OtherClockDriverValidator validator(parser);

    QCOMPARE(validator.hasValidClockValue(testDriver->getClockPulseDuration()), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testDriver, "test");

        QString expectedError = QObject::tr("Invalid clock pulse duration %1 set for other clock driver %2 within %3")
            .arg(clockValue).arg(testDriver->getClockName()).arg("test");
        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_OtherClockDriverValidator::testHasValidClockPulseDuration_data()
//-----------------------------------------------------------------------------
void tst_OtherClockDriverValidator::testHasValidClockPulseDuration_data()
{
    QTest::addColumn<QString>("clockValue");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Clock value 1+1.12 is valid") << "1+1.12" << true;
    QTest::newRow("Clock value 0 is valid") << "0" << true;
    QTest::newRow("Clock value -4.12 is valid") << "-4.12" << true;
    QTest::newRow("Empty clock value is not valid") << "" << false;
    QTest::newRow("Text is not valid for clock value") << "text" << false;
    QTest::newRow("String is not valid for clock value") << "\"text\"" << false;
}

//-----------------------------------------------------------------------------
// Function: tst_OtherClockDriverValidator::errorIsNotFoundInErrorList()
//-----------------------------------------------------------------------------
bool tst_OtherClockDriverValidator::errorIsNotFoundInErrorList(QString const& expectedError,
    QVector<QString> errorList)
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

QTEST_APPLESS_MAIN(tst_OtherClockDriverValidator)

#include "tst_OtherClockDriverValidator.moc"
