//-----------------------------------------------------------------------------
// File: tst_FieldValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 17.11.2015
//
// Description:
// Unit test for class FieldValidator.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/validators/FieldValidator.h>
#include <IPXACTmodels/Component/validators/EnumeratedValueValidator.h>
#include <IPXACTmodels/common/validators/ParameterValidator2014.h>
#include <IPXACTmodels/Component/Field.h>
#include <IPXACTmodels/Component/EnumeratedValue.h>
#include <IPXACTmodels/Component/WriteValueConstraint.h>

#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/Component/Choice.h>

#include <editors/ComponentEditor/common/SystemVerilogExpressionParser.h>

#include <QtTest>

class tst_FieldValidator : public QObject
{
    Q_OBJECT

public:
    tst_FieldValidator();

private slots:

    void testNameIsValid();
    void testNameIsValid_data();

    void testIsPresentIsValid();
    void testIsPresentIsValid_data();

    void testBitOffsetIsValid();
    void testBitOffsetIsValid_data();

    void testResetsAreValid();
    void testResetsAreValid_data();

    void testWriteValueConstraintIsValid();
    void testWriteValueConstraintIsValid_data();

    void testReservedIsValid();
    void testReservedIsValid_data();

    void testBitWidthIsValid();
    void testBitWidthIsValid_data();

    void testEnumeratedValuesAreValid();
    void testParametersAreValid();

    void testAccessIsValid();
    void testAccessIsValid_data();

private:

    bool errorIsNotFoundInErrorList(QString const& expectedError, QVector<QString> errorList);
};

//-----------------------------------------------------------------------------
// Function: tst_FieldValidator::tst_FieldValidator()
//-----------------------------------------------------------------------------
tst_FieldValidator::tst_FieldValidator()
{
}

//-----------------------------------------------------------------------------
// Function: tst_FieldValidator::testNameIsValid()
//-----------------------------------------------------------------------------
void tst_FieldValidator::testNameIsValid()
{
    QFETCH(QString, name);
    QFETCH(bool, isValid);

    QSharedPointer<Field> testField (new Field(name));

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<EnumeratedValueValidator> enumeratedValueValidator (new EnumeratedValueValidator(parser));
    QSharedPointer<ParameterValidator2014> parameterValidator (
        new ParameterValidator2014(parser, QSharedPointer<QList<QSharedPointer<Choice> > > ()));
    FieldValidator validator(parser, enumeratedValueValidator, parameterValidator);
    QCOMPARE(validator.hasValidName(testField), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testField, "test");

        QString expectedError = QObject::tr("Invalid name specified for %1 within %2").
            arg(testField->name(), "test");
        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_FieldValidator::testNameIsValid_data()
//-----------------------------------------------------------------------------
void tst_FieldValidator::testNameIsValid_data()
{
    QTest::addColumn<QString>("name");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Name test is valid") << "test" << true;
    QTest::newRow("Empty name is invalid") << "" << false;
    QTest::newRow("Name consisting of only white spaces is invalid") << "    " << false;
    QTest::newRow("Name consisting of characters and white spaces is valid") << "  test  " << true;
}

//-----------------------------------------------------------------------------
// Function: tst_FieldValidator::testIsPresentIsValid()
//-----------------------------------------------------------------------------
void tst_FieldValidator::testIsPresentIsValid()
{
    QFETCH(QString, isPresent);
    QFETCH(bool, isValid);

    QSharedPointer<Field> testField (new Field());
    testField->setName("testField");
    testField->setIsPresent(isPresent);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<EnumeratedValueValidator> enumeratedValueValidator (new EnumeratedValueValidator(parser));
    QSharedPointer<ParameterValidator2014> parameterValidator (
        new ParameterValidator2014(parser, QSharedPointer<QList<QSharedPointer<Choice> > > ()));
    FieldValidator validator(parser, enumeratedValueValidator, parameterValidator);
    QCOMPARE(validator.hasValidIsPresent(testField), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testField, "test");

        QString expectedError = QObject::tr("Invalid isPresent value specified for testField within test. "
            "Value should evaluate to 0 or 1.");
        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_FieldValidator::testIsPresentIsValid_data()
//-----------------------------------------------------------------------------
void tst_FieldValidator::testIsPresentIsValid_data()
{
    QTest::addColumn<QString>("isPresent");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("IsPresent 1 is valid") << "1" << true;
    QTest::newRow("IsPresent 1*3-3 is valid") << "1*3-3" << true;
    QTest::newRow("IsPresent 2*100 is invalid") << "2*100" << false;
    QTest::newRow("IsPresent -14 is invalid") << "-14" << false;
    QTest::newRow("Real number isPresent  0.12 is invalid") << "0.12" << false;
    QTest::newRow("Text as isPresent is invalid") << "test" << false;
    QTest::newRow("String as isPresent is invalid") << "\"test\"" << false;
}

//-----------------------------------------------------------------------------
// Function: tst_FieldValidator::testBitOffsetIsValid()
//-----------------------------------------------------------------------------
void tst_FieldValidator::testBitOffsetIsValid()
{
    QFETCH(QString, bitOffset);
    QFETCH(bool, isValid);

    QSharedPointer<Field> testField (new Field());
    testField->setBitOffset(bitOffset);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<EnumeratedValueValidator> enumeratedValueValidator (new EnumeratedValueValidator(parser));
    QSharedPointer<ParameterValidator2014> parameterValidator (
        new ParameterValidator2014(parser, QSharedPointer<QList<QSharedPointer<Choice> > > ()));
    FieldValidator validator(parser, enumeratedValueValidator, parameterValidator);
    QCOMPARE(validator.hasValidBitOffset(testField), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testField, "test");

        QString expectedError = QObject::tr("Invalid bit offset set for field %1 within %2").
            arg(testField->name()).arg("test");
        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_FieldValidator::testBitOffsetIsValid_data()
//-----------------------------------------------------------------------------
void tst_FieldValidator::testBitOffsetIsValid_data()
{
    QTest::addColumn<QString>("bitOffset");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Value 1 is valid for bit offset") << "1" << true;
    QTest::newRow("Value 0 is valid for bit offset") << "0" << true;
    QTest::newRow("Expression 1+1 is valid for bit offset") << "1+1" << true;
    QTest::newRow("Negative offset is invalid") << "-1" << false;
    QTest::newRow("Negative expression is invalid for bit offset") << "2-4" << false;
    QTest::newRow("Empty bit offset is invalid") << "" << false;
    QTest::newRow("Text is invalid for bit offset") << "test" << false;
    QTest::newRow("String is invalid for bit offset") << "\"test\"" << false;

    QTest::newRow("Long bit offset is valid") << "40000000000" << true;
}

//-----------------------------------------------------------------------------
// Function: tst_FieldValidator::testResetValueIsValid()
//-----------------------------------------------------------------------------
void tst_FieldValidator::testResetsAreValid()
{
    QFETCH(QString, resetValue);
    QFETCH(QString, resetMask);
    QFETCH(bool, resetValueIsValid);
    QFETCH(bool, resetMaskIsValid);

    QSharedPointer<Field> testField (new Field());
    testField->setResetValue(resetValue);
    testField->setResetMask(resetMask);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<EnumeratedValueValidator> enumeratedValueValidator (new EnumeratedValueValidator(parser));
    QSharedPointer<ParameterValidator2014> parameterValidator (
        new ParameterValidator2014(parser, QSharedPointer<QList<QSharedPointer<Choice> > > ()));
    FieldValidator validator(parser, enumeratedValueValidator, parameterValidator);
    QCOMPARE(validator.hasValidResetValue(testField), resetValueIsValid);

    if (!resetValueIsValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testField, "test");

        QString expectedError = QObject::tr("Invalid reset value set for field %1 within %2").
            arg(testField->name()).arg("test");
        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }

    QCOMPARE(validator.hasValidResetMask(testField), resetMaskIsValid);
    if (!resetMaskIsValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testField, "test");

        QString expectedError = QObject::tr("Invalid reset mask set for field %1 within %2").
            arg(testField->name()).arg("test");
        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_FieldValidator::testResetValueIsValid_data()
//-----------------------------------------------------------------------------
void tst_FieldValidator::testResetsAreValid_data()
{
    QTest::addColumn<QString>("resetValue");
    QTest::addColumn<QString>("resetMask");
    QTest::addColumn<bool>("resetValueIsValid");
    QTest::addColumn<bool>("resetMaskIsValid");

    QTest::newRow("Bit vector 1 is valid for reset value") << "1" << "" << true << true;
    QTest::newRow("Empty reset value is valid") << "" << "" << true << true;
    QTest::newRow("8'b1001_0110 is valid for value and mask") << "8'b1001_0110" << "8'b1001_0110" << true << true;
    QTest::newRow("Text is invalid") << "test" << "test" << false << false;
    QTest::newRow("String is invalid") << "\"test\"" << "\"test\"" << false << false;
    QTest::newRow("Reset mask without a reset value is invalid") << "" << "2'b10" << false << true;
    QTest::newRow("Reset value 14 and mask 2'h2 is valid") << "14" << "2'h2" << true << true;
    QTest::newRow("White space is invalid for reset value and mask") << "  " << "   " << false << false;
}

//-----------------------------------------------------------------------------
// Function: tst_FieldValidator::testWriteValueConstraintIsValid()
//-----------------------------------------------------------------------------
void tst_FieldValidator::testWriteValueConstraintIsValid()
{
    QFETCH(QString, minimum);
    QFETCH(QString, maximum);
    QFETCH(QString, constraintType);
    QFETCH(bool, isValid);

    QSharedPointer<WriteValueConstraint> testConstraint(new WriteValueConstraint());
    if (constraintType == QLatin1String("writeAsRead"))
    {
        testConstraint->setType(WriteValueConstraint::WRITE_AS_READ);
    }
    else if (constraintType == QLatin1String("useEnumeratedValues"))
    {
        testConstraint->setType(WriteValueConstraint::USE_ENUM);
    }
    else if (constraintType == QLatin1String("minMax"))
    {
        testConstraint->setMinimum(minimum);
        testConstraint->setMaximum(maximum);
    }

    QSharedPointer<Field> testField (new Field());
    testField->setWriteConstraint(testConstraint);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<EnumeratedValueValidator> enumeratedValueValidator (new EnumeratedValueValidator(parser));
    QSharedPointer<ParameterValidator2014> parameterValidator (
        new ParameterValidator2014(parser, QSharedPointer<QList<QSharedPointer<Choice> > > ()));
    FieldValidator validator(parser, enumeratedValueValidator, parameterValidator);
    QCOMPARE(validator.hasValidWriteValueConstraint(testField), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testField, "test");

        if (testConstraint->getType() == WriteValueConstraint::TYPE_COUNT)
        {
            QString expectedError = QObject::tr("Invalid write value constraint set for field %1 within %2").
                arg(testField->name()).arg("test");
            if (errorIsNotFoundInErrorList(expectedError, foundErrors))
            {
                QFAIL("No error message found");
            }
        }
        else if (testConstraint->getType() == WriteValueConstraint::MIN_MAX)
        {
            int minimumValue = parser->parseExpression(testConstraint->getMinimum()).toInt();
            int maximumValue = parser->parseExpression(testConstraint->getMaximum()).toInt();

            QString expectedError;
            if (minimumValue > maximumValue)
            {
                expectedError = (QObject::tr("Maximum value must be greater than or equal to the minimum value in "
                    "write value constraint set for field %1 within %2").arg(testField->name()).arg("test"));
                if (errorIsNotFoundInErrorList(expectedError, foundErrors))
                {
                    QFAIL("No error message found");
                }
            }

            QRegularExpression bitExpression("^([0-9]+|[1-9]+[0-9]*'([bB][01_]+|[hH][0-9a-fA-F_]+))$");
            if (!bitExpression.match(testConstraint->getMinimum()).hasMatch() ||
                !bitExpression.match(parser->parseExpression(testConstraint->getMinimum())).hasMatch())
            {
                expectedError = QObject::tr(
                    "Invalid minimum value set for write value constraint in field %1 within %2").
                    arg(testField->name()).arg("test");
                if (errorIsNotFoundInErrorList(expectedError, foundErrors))
                {
                    QFAIL("No error message found");
                }
            }

            if (!bitExpression.match(testConstraint->getMaximum()).hasMatch() ||
                !bitExpression.match(parser->parseExpression(testConstraint->getMaximum())).hasMatch())
            {
                expectedError = QObject::tr(
                    "Invalid maximum value set for write value constraint in field %1 within %2").
                    arg(testField->name()).arg("test");
                if (errorIsNotFoundInErrorList(expectedError, foundErrors))
                {
                    QFAIL("No error message found");
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_FieldValidator::testWriteValueConstraintIsValid_data()
//-----------------------------------------------------------------------------
void tst_FieldValidator::testWriteValueConstraintIsValid_data()
{
    QTest::addColumn<QString>("minimum");
    QTest::addColumn<QString>("maximum");
    QTest::addColumn<QString>("constraintType");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("No type for write value constraint is invalid") << "" << "" << "" << false;
    QTest::newRow("Write value constraint writeAsRead is valid") << "" << "" << "writeAsRead" << true;
    QTest::newRow("Write value constraint useEnumerated values is valid") << "" << "" << "useEnumeratedValues" << true;
    QTest::newRow("Write value constraint minMax is invalid for empty min and max") << "" << "" << "minMax" << false;
    QTest::newRow("8'b1001_0110 is valid for min and max") << "8'b1001_0110" << "8'b1001_0110" << "minMax" << true;
    QTest::newRow("Text is invalid for min and max") << "text" << "text" << "minMax" << false;
    QTest::newRow("String is invalid for min and max") << "\"test\"" << "\"test\"" << "minMax" << false;
    QTest::newRow("Values 14 and 2'h2 are invalid for min and max") << "14" << "2'h2" << "minMax" << false;
}

//-----------------------------------------------------------------------------
// Function: tst_FieldValidator::testReservedIsValid()
//-----------------------------------------------------------------------------
void tst_FieldValidator::testReservedIsValid()
{
    QFETCH(QString, reserved);
    QFETCH(bool, isValid);

    QSharedPointer<Field> testField (new Field());
    testField->setReserved(reserved);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<EnumeratedValueValidator> enumeratedValueValidator (new EnumeratedValueValidator(parser));
    QSharedPointer<ParameterValidator2014> parameterValidator (
        new ParameterValidator2014(parser, QSharedPointer<QList<QSharedPointer<Choice> > > ()));
    FieldValidator validator(parser, enumeratedValueValidator, parameterValidator);
    QCOMPARE(validator.hasValidReserved(testField), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testField, "test");

        QString expectedError = QObject::tr("Invalid reserved set for field %1 within %2").arg(testField->name()).
            arg("test");
        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_FieldValidator::testReservedIsValid_data()
//-----------------------------------------------------------------------------
void tst_FieldValidator::testReservedIsValid_data()
{
    QTest::addColumn<QString>("reserved");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Empty reserved value is valid") << "" << true;
    QTest::newRow("White space is invalid reserved value") << "   " << false;
    QTest::newRow("10 is invalid reserved value") << "10" << false;
    QTest::newRow("4+5*3-20/2-8 is valid reserved value") << "4+5*3-20/2-8" << true;
}

//-----------------------------------------------------------------------------
// Function: tst_FieldValidator::testBitWidthIsValid()
//-----------------------------------------------------------------------------
void tst_FieldValidator::testBitWidthIsValid()
{
    QFETCH(QString, bitWidth);
    QFETCH(bool, isValid);

    QSharedPointer<Field> testField (new Field());
    testField->setBitWidth(bitWidth);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<EnumeratedValueValidator> enumeratedValueValidator (new EnumeratedValueValidator(parser));
    QSharedPointer<ParameterValidator2014> parameterValidator (
        new ParameterValidator2014(parser, QSharedPointer<QList<QSharedPointer<Choice> > > ()));
    FieldValidator validator(parser, enumeratedValueValidator, parameterValidator);
    QCOMPARE(validator.hasValidBitWidth(testField), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testField, "test");

        QString expectedError = QObject::tr("Invalid bit width set for field %1 within %2").
            arg(testField->name()).arg("test");
        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_FieldValidator::testBitWidthIsValid_data()
//-----------------------------------------------------------------------------
void tst_FieldValidator::testBitWidthIsValid_data()
{
    QTest::addColumn<QString>("bitWidth");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Value 1 is valid for bit width") << "1" << true;
    QTest::newRow("Value 0 is invalid for bit width") << "0" << false;
    QTest::newRow("Expression 1+1-1 is valid for bit width") << "1+1-1" << true;
    QTest::newRow("Negative width is invalid for bit width") << "-1" << false;
    QTest::newRow("Negative expression is invalid for bit width") << "2-3" << false;
    QTest::newRow("Empty bit width is invalid for bit width") << "" << false;
    QTest::newRow("Text is invalid for bit width") << "test" << false;
    QTest::newRow("String is invalid for bit width") << "\"test\"" << false;

    QTest::newRow("Long bit width is valid") << "40000000000" << true;
}

//-----------------------------------------------------------------------------
// Function: tst_FieldValidator::testEnumeratedValuesAreValid()
//-----------------------------------------------------------------------------
void tst_FieldValidator::testEnumeratedValuesAreValid()
{
    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<EnumeratedValueValidator> enumeratedValueValidator (new EnumeratedValueValidator(parser));
    QSharedPointer<ParameterValidator2014> parameterValidator (
        new ParameterValidator2014(parser, QSharedPointer<QList<QSharedPointer<Choice> > > ()));
    FieldValidator validator(parser, enumeratedValueValidator, parameterValidator);

    QSharedPointer<EnumeratedValue> enumeratedValue(new EnumeratedValue("test", "4"));

    QSharedPointer<Field> testField (new Field());
    testField->getEnumeratedValues()->append(enumeratedValue);

    QCOMPARE(validator.hasValidEnumeratedValues(testField), true);

    QSharedPointer<EnumeratedValue> enumeratedValue2 (new EnumeratedValue("", "2"));
    testField->getEnumeratedValues()->append(enumeratedValue2);

    QCOMPARE(validator.hasValidEnumeratedValues(testField), false);
    
    QVector<QString> errorsFound;
    validator.findErrorsIn(errorsFound, testField, "test");

    QString expectedError = QObject::tr("Invalid name specified for %1 within field %2").
        arg(enumeratedValue2->name()).arg(testField->name());

    if (errorIsNotFoundInErrorList(expectedError, errorsFound))
    {
        QFAIL("Error was not found");
    }

    enumeratedValue2->setName("other");

    QCOMPARE(validator.hasValidEnumeratedValues(testField), true);

    QSharedPointer<EnumeratedValue> enumeratedValue3(new EnumeratedValue("test", "1+1"));
    testField->getEnumeratedValues()->append(enumeratedValue3);

    QCOMPARE(validator.hasValidEnumeratedValues(testField), false);

    errorsFound.clear();
    validator.findErrorsIn(errorsFound, testField, "test");
    expectedError = QObject::tr("Name %1 of enumerated values in field %2 is not unique.").
        arg(enumeratedValue->name()).arg(testField->name());

    if (errorIsNotFoundInErrorList(expectedError, errorsFound))
    {
        QFAIL("Error was not found");
    }

    QSharedPointer<WriteValueConstraint> writeConstaint (new WriteValueConstraint());
    writeConstaint->setType(WriteValueConstraint::USE_ENUM);
    testField->setWriteConstraint(writeConstaint);

    QCOMPARE(validator.hasValidEnumeratedValues(testField), false);
    errorsFound.clear();
    validator.findErrorsIn(errorsFound, testField, "test");
    expectedError = QObject::tr("Write value constraint of Use enumerated values needs an enumerated value with "
        "usage of write or read-write. Such an enumerated value was not found in field %1")
        .arg(testField->name());
    if (errorIsNotFoundInErrorList(expectedError, errorsFound))
    {
        QFAIL("Error was not found");
    }

    enumeratedValue3->setUsage(EnumeratedValue::WRITE);
    enumeratedValue3->setName("writeEnumeratedValue");
    QCOMPARE(validator.hasValidEnumeratedValues(testField), true);
}

//-----------------------------------------------------------------------------
// Function: tst_FieldValidator::testParametersAreValid()
//-----------------------------------------------------------------------------
void tst_FieldValidator::testParametersAreValid()
{
    QSharedPointer<Parameter> testParameter (new Parameter());
    testParameter->setValueId("Sanger");
    testParameter->setName("Daizengar");
    testParameter->setValue("20");
    testParameter->setType("int");

    QSharedPointer<Field> testField (new Field());
    testField->getParameters()->append(testParameter);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<EnumeratedValueValidator> enumeratedValueValidator (new EnumeratedValueValidator(parser));
    QSharedPointer<ParameterValidator2014> parameterValidator (
        new ParameterValidator2014(parser, QSharedPointer<QList<QSharedPointer<Choice> > > ()));
    FieldValidator validator(parser, enumeratedValueValidator, parameterValidator);
    
    QCOMPARE(validator.hasValidParameters(testField), true);

    testParameter->setValue("");
    QCOMPARE(validator.hasValidParameters(testField), false);

    QVector<QString> errorsFound;
    validator.findErrorsIn(errorsFound, testField, "test");

    QString expectedError = QObject::tr("No value specified for %1 %2 within field %3").
        arg(testParameter->elementName()).arg(testParameter->name()).arg(testField->name());

    if (errorIsNotFoundInErrorList(expectedError, errorsFound))
    {
        QFAIL("Error was not found");
    }

    QSharedPointer<Parameter> otherParameter (new Parameter(*testParameter.data()));
    testParameter->setValue("1");
    otherParameter->setValue("2");
    testField->getParameters()->append(otherParameter);

    QCOMPARE(validator.hasValidParameters(testField), false);

    errorsFound.clear();
    validator.findErrorsIn(errorsFound, testField, "test");
    expectedError = QObject::tr("Name %1 of parameters in field %2 is not unique.").arg(otherParameter->name()).
        arg(testField->name());
    if (errorIsNotFoundInErrorList(expectedError, errorsFound))
    {
        QFAIL("Error was not found");
    }
}

//-----------------------------------------------------------------------------
// Function: tst_FieldValidator::testModifiedWriteValue()
//-----------------------------------------------------------------------------
void tst_FieldValidator::testAccessIsValid()
{
    QFETCH(QString, accessValue);
    QFETCH(QString, modifiedWriteValue);
    QFETCH(QString, readAction);
    QFETCH(bool, isValid);

    QSharedPointer<Field> testField (new Field());
    testField->setAccess(AccessTypes::str2Access(accessValue, AccessTypes::ACCESS_COUNT));
    testField->setModifiedWrite(General::str2ModifiedWrite(modifiedWriteValue));
    testField->setReadAction(General::str2ReadAction(readAction));

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<EnumeratedValueValidator> enumeratedValueValidator (new EnumeratedValueValidator(parser));
    QSharedPointer<ParameterValidator2014> parameterValidator (
        new ParameterValidator2014(parser, QSharedPointer<QList<QSharedPointer<Choice> > > ()));
    FieldValidator validator(parser, enumeratedValueValidator, parameterValidator);
    QCOMPARE(validator.hasValidAccess(testField), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testField, "test");

        if (testField->getAccess() == AccessTypes::READ_ONLY &&
            testField->getModifiedWrite() != General::MODIFIED_WRITE_COUNT)
        {
            QString expectedError = QObject::tr("In field %1 within %2, access type readOnly does not allow a field "
                "to include a modified write value.").arg(testField->name()).arg("test");
            if (errorIsNotFoundInErrorList(expectedError, foundErrors))
            {
                QFAIL("No error message found");
            }
        }
        else if ((testField->getAccess() == AccessTypes::WRITE_ONLY ||
            testField->getAccess() == AccessTypes::WRITEONCE) &&
            testField->getReadAction() != General::READ_ACTION_COUNT)
        {
            QString expectedError = QObject::tr("In field %1 within %2, access type write only and write once "
                "do not allow a field to include a read action value.").arg(testField->name()).arg("test");
            if (errorIsNotFoundInErrorList(expectedError, foundErrors))
            {
                QFAIL("No error message found");
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_FieldValidator::testModifiedWriteValue_data()
//-----------------------------------------------------------------------------
void tst_FieldValidator::testAccessIsValid_data()
{
    QTest::addColumn<QString>("accessValue");
    QTest::addColumn<QString>("modifiedWriteValue");
    QTest::addColumn<QString>("readAction");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Empty access, modified write value and read action are valid") << "" << "" << "" << true;
    QTest::newRow("Access read-write with a modified write value is valid") << "read-write" << "oneToClear" <<
        "set" << true;
    QTest::newRow("Access read-only with a modified write value is invalid") << "read-only" << "oneToClear" <<
        "set" << false;
    QTest::newRow("Access read-only without modified write value is valid") << "read-only" << "" << "set" << true;
    QTest::newRow("Access write-only with a read action is invalid") << "write-only" << "" << "set" << false;
    QTest::newRow("Access writeOnce with a read action is invalid") << "writeOnce" << "" << "clear" << false;
    QTest::newRow("Access write-only without a read action is valid") << "write-only" << "oneToClear" << "" << true;
    QTest::newRow("Access writeOnce without a read action is valid") << "writeOnce" << "oneToClear" << "" << true;
}

//-----------------------------------------------------------------------------
// Function: tst_FieldValidator::errorIsNotFoundInErrorList()
//-----------------------------------------------------------------------------
bool tst_FieldValidator::errorIsNotFoundInErrorList(QString const& expectedError, QVector<QString> errorList)
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

QTEST_APPLESS_MAIN(tst_FieldValidator)

#include "tst_FieldValidator.moc"
