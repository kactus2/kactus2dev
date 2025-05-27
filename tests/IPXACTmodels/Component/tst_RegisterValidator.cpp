//-----------------------------------------------------------------------------
// File: tst_RegisterValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 19.11.2015
//
// Description:
// Unit test for class RegisterValidator.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/validators/RegisterValidator.h>
#include <IPXACTmodels/Component/validators/FieldValidator.h>
#include <IPXACTmodels/Component/validators/EnumeratedValueValidator.h>
#include <IPXACTmodels/common/validators/ParameterValidator.h>

#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/AlternateRegister.h>
#include <IPXACTmodels/Component/Field.h>
#include <IPXACTmodels/Component/EnumeratedValue.h>
#include <IPXACTmodels/Component/WriteValueConstraint.h>
#include <IPXACTmodels/Component/MemoryArray.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Mode.h>
#include <IPXACTmodels/common/Parameter.h>

#include <IPXACTmodels/generaldeclarations.h>

#include <KactusAPI/include/SystemVerilogExpressionParser.h>
#include <KactusAPI/include/ComponentParameterFinder.h>

#include <QtTest>

class tst_RegisterValidator : public QObject
{
    Q_OBJECT

public:
    tst_RegisterValidator();

private slots:

    void testNameIsValid();
    void testNameIsValid_data();

    void testIsPresentIsValid();
    void testIsPresentIsValid_data();

    void testDimensionIsValid();
    void testDimensionIsValid_data();

    void testAddressOffsetIsValid();
    void testAddressOffsetIsValid_data();

    void testSizeIsValid();
    void testSizeIsValid_data();

    void testRegisterAndFieldVolatileIsValid();
    void testRegisterAndFieldVolatileIsValid_data();

    void testRegisterAndFieldAccessAreValid();
    void testRegisterAndFieldAccessAreValid_data();

    void testFieldsAreValid();
    void testFieldRangesAreValid();
    void testFieldRangesAreValid_data();
    void testFieldsAreWithinRegister();
    void testFieldsAreWithinRegister_data();
    
    void testFieldWithStrideIsWithinRegister();
    void testFieldWithStrideIsWithinRegister_data();
    void testFieldOverlapWithStride();
    void testFieldReplicasOverlapDueToSmallStride();
    void testFieldWithDimAndEmptyStride();
    void testFieldOverlapWithStrideAndNonStride();

    void testFieldsHaveSimilarDefinitionGroups();
    void testFieldsHaveSimilarDefinitionGroups_data();

    void testAlternateRegisterIsValid();
    void testAlternateRegisterIsValid_data();
    void testMultipleAlternateRegistersAreValid();
    void testAlternateRegisterParametersAreValid();

    void testParametersAreValid();

    void testRegisterMemoryArrayIsValid2022();
    void testRegisterMemoryArrayIsValid2022_data();

    void testRegisterAccessPoliciesAreValid2022();


    void testModeRefsOfAlternativeRegisetrsAreValid2022();
    void testAlternateRegisterAccessPoliciesAreValid2022();

private:

    bool errorIsNotFoundInErrorList(QString const& expectedError, QVector<QString> errorList);
};

//-----------------------------------------------------------------------------
// Function: tst_RegisterValidator::tst_RegisterValidator()
//-----------------------------------------------------------------------------
tst_RegisterValidator::tst_RegisterValidator()
{
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterValidator::testNameIsValid()
//-----------------------------------------------------------------------------
void tst_RegisterValidator::testNameIsValid()
{
    QFETCH(QString, name);
    QFETCH(bool, isValid);

    QSharedPointer<Register> testRegister (new Register(name));

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator> parameterValidator(new ParameterValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > > (), Document::Revision::Std14));
    QSharedPointer<EnumeratedValueValidator> enumValidator (new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator (new FieldValidator(parser, enumValidator, parameterValidator));
    RegisterValidator validator(parser, fieldValidator, parameterValidator);
    QCOMPARE(validator.hasValidName(testRegister), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testRegister, "test");

        QString expectedError = QObject::tr("Invalid name specified for register '%1' within %2").
            arg(testRegister->name(), "test");
        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterValidator::testNameIsValid_data()
//-----------------------------------------------------------------------------
void tst_RegisterValidator::testNameIsValid_data()
{
    QTest::addColumn<QString>("name");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Name test is valid") << "test" << true;
    QTest::newRow("Empty name is invalid") << "" << false;
    QTest::newRow("Name consisting of only white spaces is invalid") << "    " << false;
    QTest::newRow("Name consisting of characters and white spaces is valid") << "  test  " << true;
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterValidator::testIsPresentIsValid()
//-----------------------------------------------------------------------------
void tst_RegisterValidator::testIsPresentIsValid()
{
    QFETCH(QString, isPresent);
    QFETCH(bool, isValid);

    QSharedPointer<Register> testRegister (new Register("TestRegister"));
    testRegister->setIsPresent(isPresent);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator> parameterValidator(new ParameterValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > > (), Document::Revision::Std14));
    QSharedPointer<EnumeratedValueValidator> enumValidator (new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator (new FieldValidator(parser, enumValidator, parameterValidator));
    RegisterValidator validator(parser, fieldValidator, parameterValidator);
    QCOMPARE(validator.hasValidIsPresent(testRegister), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testRegister, "test");

        QString expectedError = QObject::tr("Invalid isPresent set for register '%1' within %2").
            arg(testRegister->name(), "test");
        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterValidator::testIsPresentIsValid_data()
//-----------------------------------------------------------------------------
void tst_RegisterValidator::testIsPresentIsValid_data()
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
// Function: tst_RegisterValidator::testDimensionIsValid()
//-----------------------------------------------------------------------------
void tst_RegisterValidator::testDimensionIsValid()
{
    QFETCH(QString, dimension);
    QFETCH(bool, isValid);

    QSharedPointer<Register> testRegister (new Register("TestRegister"));
    testRegister->setDimension(dimension);

    QSharedPointer<ExpressionParser> parser (new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator> parameterValidator(new ParameterValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > > (), Document::Revision::Std14));
    QSharedPointer<EnumeratedValueValidator> enumValidator (new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator (new FieldValidator(parser, enumValidator, parameterValidator));
    RegisterValidator validator(parser, fieldValidator, parameterValidator);

    QCOMPARE(validator.hasValidDimensions(testRegister), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testRegister, "test");

        QString expectedError = QObject::tr("Invalid dimensions set for register '%1' within %2").
            arg(testRegister->name(), "test");

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterValidator::testDimensionIsValid_data()
//-----------------------------------------------------------------------------
void tst_RegisterValidator::testDimensionIsValid_data()
{
    QTest::addColumn<QString>("dimension");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Dimension 3 is valid") << "3" << true;
    QTest::newRow("Dimension -8 is invalid") << "-8" << false;
    QTest::newRow("Dimension 4*4/2-8 is invalid") << "0" << false;
    QTest::newRow("Text value is invalid for dimension") << "text" << false;
    QTest::newRow("String value is invalid for dimension") << "\"text\"" << false;

    QTest::newRow("Long dimension is valid") << "40000000000" << true;
    //QTest::newRow("Really long dimension is not valid") << "40000000000000000000000000000000000000000000" << false;
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterValidator::testAddressOffsetIsValid()
//-----------------------------------------------------------------------------
void tst_RegisterValidator::testAddressOffsetIsValid()
{
    QFETCH(QString, addressOffset);
    QFETCH(bool, isValid);

    QSharedPointer<Register> testRegister (new Register("testRegister", addressOffset));

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator> parameterValidator(new ParameterValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > > (), Document::Revision::Std14));
    QSharedPointer<EnumeratedValueValidator> enumValidator (new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator (new FieldValidator(parser, enumValidator, parameterValidator));
    RegisterValidator validator(parser, fieldValidator, parameterValidator);
    QCOMPARE(validator.hasValidAddressOffset(testRegister), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testRegister, "test");

        QString expectedError = QObject::tr("Invalid address offset set for register '%1' within %2").
            arg(testRegister->name()).arg("test");

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterValidator::testAddressOffsetIsValid_data()
//-----------------------------------------------------------------------------
void tst_RegisterValidator::testAddressOffsetIsValid_data()
{
    QTest::addColumn<QString>("addressOffset");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Address offset of 10 is valid") << "10" << true;
    QTest::newRow("Empty address offset is invalid") << "" << false;
    QTest::newRow("Address offset of -2 is invalid") << "-2" << false;
    QTest::newRow("Address offset of 20*80 is valid") << "20*80" << true;
    QTest::newRow("Text is invalid for address offset") << "text" << false;
    QTest::newRow("String is invalid for address offset") << "\"text\"" << false;

    QTest::newRow("Long address offset is valid") << "40000000000" <<  true;
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterValidator::testSizeIsValid()
//-----------------------------------------------------------------------------
void tst_RegisterValidator::testSizeIsValid()
{
    QFETCH(QString, size);
    QFETCH(bool, isValid);

    QSharedPointer<Register> testRegister (new Register("testRegister", "8", size));

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator> parameterValidator(new ParameterValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > > (), Document::Revision::Std14));
    QSharedPointer<EnumeratedValueValidator> enumValidator (new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator (new FieldValidator(parser, enumValidator, parameterValidator));
    RegisterValidator validator(parser, fieldValidator, parameterValidator);
    QCOMPARE(validator.hasValidSize(testRegister), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testRegister, "test");

        QString expectedError = QObject::tr("Invalid size specified for register '%1' within %2").
            arg(testRegister->name()).arg("test");

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterValidator::testSizeIsValid_data()
//-----------------------------------------------------------------------------
void tst_RegisterValidator::testSizeIsValid_data()
{
    QTest::addColumn<QString>("size");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Positive size is valid") << "10" << true;
    QTest::newRow("Empty size is invalid") << "" << false;
    QTest::newRow("Negative size is invalid") << "-2" << false;
    QTest::newRow("Size with expression is valid") << "20*80" << true;
    QTest::newRow("Size 0 is invalid") << "18/2-9" << false;
    QTest::newRow("Text is invalid for size") << "text" << false;
    QTest::newRow("String is invalid for size") << "\"text\"" << false;

    QTest::newRow("Long size is valid") << "40000000000" << true;
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterValidator::testRegisterAndFieldVolatileIsValid()
//-----------------------------------------------------------------------------
void tst_RegisterValidator::testRegisterAndFieldVolatileIsValid()
{
    QFETCH(bool, registerVolatile);
    QFETCH(bool, fieldVolatile);
    QFETCH(bool, isValid);

    QSharedPointer<Field> testField(new Field("testField"));
    testField->setVolatile(fieldVolatile);
    testField->setBitOffset("2");
    testField->setBitWidth("5");

    QSharedPointer<Register> testRegister(new Register("testRegister"));
    testRegister->setSize("100");
    testRegister->setVolatile(registerVolatile);
    testRegister->getFields()->append(testField);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator> parameterValidator(new ParameterValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > > (), Document::Revision::Std14));
    QSharedPointer<EnumeratedValueValidator> enumValidator (new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator (new FieldValidator(parser, enumValidator, parameterValidator));
    RegisterValidator validator(parser, fieldValidator, parameterValidator);
    QCOMPARE(validator.hasValidFields(testRegister, testRegister->getSize()), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testRegister, "test");

        QString expectedError = QObject::tr("Volatile cannot be set to false in register %1, where contained "
            "field %2 has volatile true")
            .arg(testRegister->name()).arg(testField->name());

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterValidator::testRegisterAndFieldVolatileIsValid_data()
//-----------------------------------------------------------------------------
void tst_RegisterValidator::testRegisterAndFieldVolatileIsValid_data()
{
    QTest::addColumn<bool>("registerVolatile");
    QTest::addColumn<bool>("fieldVolatile");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Field: volatile = true in Register: volatile = true is valid") << true << true << true;
    QTest::newRow("Field: volatile = true in Register: volatile = false is invalid") << false << true << false;
    QTest::newRow("Field: volatile = false, register volatile is always valid") << false << false << true;
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterValidator::testRegisterAndFieldAccessAreValid()
//-----------------------------------------------------------------------------
void tst_RegisterValidator::testRegisterAndFieldAccessAreValid()
{
    QFETCH(QString, fieldAccess);
    QFETCH(QString, registerAccess);
    QFETCH(bool, isValid);

    QSharedPointer<Field> testField(new Field("testField"));
    testField->setAccess(AccessTypes::str2Access(fieldAccess, AccessTypes::ACCESS_COUNT));
    testField->setBitOffset("2");
    testField->setBitWidth("5");

    QSharedPointer<Register> testRegister(new Register("testRegister"));
    testRegister->setSize("100");
    testRegister->setAccess(AccessTypes::str2Access(registerAccess, AccessTypes::ACCESS_COUNT));
    testRegister->getFields()->append(testField);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator> parameterValidator(new ParameterValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > > (), Document::Revision::Std14));
    QSharedPointer<EnumeratedValueValidator> enumValidator (new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator (new FieldValidator(parser, enumValidator, parameterValidator));
    RegisterValidator validator(parser, fieldValidator, parameterValidator);
    QCOMPARE(validator.hasValidFields(testRegister, testRegister->getSize()), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testRegister, "test");

        QString expectedError = QObject::tr("Access cannot be set to %1 in field %2, where containing register %3 "
            "has access %4").arg(AccessTypes::access2Str(testField->getAccess())).arg(testField->name())
            .arg(testRegister->name()).arg(AccessTypes::access2Str(testRegister->getAccess()));

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterValidator::testRegisterAndFieldAccessAreValid_data()
//-----------------------------------------------------------------------------
void tst_RegisterValidator::testRegisterAndFieldAccessAreValid_data()
{
    QTest::addColumn<QString>("fieldAccess");
    QTest::addColumn<QString>("registerAccess");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Field: access = read-write, Register: access = read-only is invalid") << "read-write" <<
        "read-only" << false;
    QTest::newRow("Field: access = read-only, Register: access = read-only is valid") << "read-only" <<
        "read-only" << true;
    QTest::newRow("Field: access = write-only, Register: access = read-only is invalid") << "write-only" <<
        "read-only" << false;
    QTest::newRow("Field: access = read-writeOnce, Register: access = read-only is invalid") << "read-writeOnce" <<
        "read-only" << false;
    QTest::newRow("Field: access = writeOnce, Register: access = read-only is invalid") << "writeOnce" <<
        "read-only" << false;

    QTest::newRow("Field: access = read-write, Register: access = write-only is invalid") << "read-write" <<
        "write-only" << false;
    QTest::newRow("Field: access = read-only, Register: access = write-only is invalid") << "read-only" <<
        "write-only" << false;
    QTest::newRow("Field: access = write-only, Register: access = write-only is valid") << "write-only" <<
        "write-only" << true;
    QTest::newRow("Field: access = read-writeOnce, Register: access = write-only is invalid") <<
        "read-writeOnce" << "write-only" << false;
    QTest::newRow("Field: access = writeOnce, Register: access = write-only is valid") << "writeOnce" <<
        "write-only" << true;

    QTest::newRow("Field: access = read-write, Register: access = read-writeOnce is invalid") << "read-write" <<
        "read-writeOnce" << false;
    QTest::newRow("Field: access = read-only, Register: access = read-writeOnce is valid") << "read-only" <<
        "read-writeOnce" << true;
    QTest::newRow("Field: access = write-only, Register: access = read-writeOnce is invalid") << "write-only" <<
        "read-writeOnce" << false;
    QTest::newRow("Field: access = read-writeOnce, Register: access = read-writeOnce is valid") <<
        "read-writeOnce" << "read-writeOnce" << true;
    QTest::newRow("Field: access = writeOnce, Register: access = read-writeOnce is valid") << "writeOnce" <<
        "read-writeOnce" << true;

    QTest::newRow("Field: access = read-write, Register: access = writeOnce is invalid") << "read-write" <<
        "writeOnce" << false;
    QTest::newRow("Field: access = read-only, Register: access = writeOnce is invalid") << "read-only" <<
        "writeOnce" << false;
    QTest::newRow("Field: access = write-only, Register: access = writeOnce is invalid") << "write-only" <<
        "writeOnce" << false;
    QTest::newRow("Field: access = read-writeOnce, Register: access = writeOnce is invalid") << "read-writeOnce" <<
        "writeOnce" << false;
    QTest::newRow("Field: access = writeOnce, Register: access = writeOnce is valid") << "writeOnce" <<
        "writeOnce" << true;
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterValidator::testFieldsAreValid()
//-----------------------------------------------------------------------------
void tst_RegisterValidator::testFieldsAreValid()
{
    QSharedPointer<ExpressionParser> parser (new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator> parameterValidator(new ParameterValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > > (), Document::Revision::Std14));
    QSharedPointer<EnumeratedValueValidator> enumValidator (new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator (new FieldValidator(parser, enumValidator, parameterValidator));
    RegisterValidator validator(parser, fieldValidator, parameterValidator);

    QSharedPointer<Field> testField (new Field("Lagann"));
    testField->setBitWidth("1");

    QSharedPointer<Register> testRegister (new Register("Gurren"));
    testRegister->setSize("100");
    testRegister->getFields()->append(testField);

    QCOMPARE(validator.hasValidFields(testRegister, testRegister->getSize()), false);

    QVector<QString> foundErrors;
    validator.findErrorsIn(foundErrors, testRegister, "test");

    QString expectedError = QObject::tr("Invalid bit offset set for field %1 within %2").
        arg(testField->name()).arg("register " + testRegister->name());

    if (errorIsNotFoundInErrorList(expectedError, foundErrors))
    {
        QFAIL("No error message found");
    }

    testField->setBitOffset("4+1");
    QCOMPARE(validator.hasValidFields(testRegister, testRegister->getSize()), true);

    QSharedPointer<Field> otherField (new Field(*testField.data()));
    testRegister->getFields()->append(otherField);
    QCOMPARE(validator.hasValidFields(testRegister, testRegister->getSize()), false);

    foundErrors.clear();
    validator.findErrorsIn(foundErrors, testRegister, "test");

    expectedError = QObject::tr("Name %1 of fields in %2 is not unique.").
        arg(testField->name()).arg("register " + testRegister->name());

    if (errorIsNotFoundInErrorList(expectedError, foundErrors))
    {
        QFAIL("No error message found");
    }
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterValidator::testFieldRangesAreValid()
//-----------------------------------------------------------------------------
void tst_RegisterValidator::testFieldRangesAreValid()
{
    QFETCH(QString, firstOffset);
    QFETCH(QString, firstWidth);
    QFETCH(QString, firstAccess);
    QFETCH(QString, secondOffset);
    QFETCH(QString, secondWidth);
    QFETCH(QString, secondAccess);
    QFETCH(bool, isValid);

    QSharedPointer<Field> fieldOne (new Field("FieldOne"));
    fieldOne->setBitOffset(firstOffset);
    fieldOne->setBitWidth(firstWidth);
    fieldOne->setAccess(AccessTypes::str2Access(firstAccess, AccessTypes::ACCESS_COUNT));

    QSharedPointer<Field> fieldTwo (new Field("FieldTwo"));
    fieldTwo->setBitOffset(secondOffset);
    fieldTwo->setBitWidth(secondWidth);
    fieldTwo->setAccess(AccessTypes::str2Access(secondAccess, AccessTypes::ACCESS_COUNT));

    QSharedPointer<Register> testRegister(new Register("TestRegister"));
    testRegister->setSize("100");
    testRegister->getFields()->append(fieldOne);
    testRegister->getFields()->append(fieldTwo);
    
    QSharedPointer<ExpressionParser> parser (new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator> parameterValidator(new ParameterValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > > (), Document::Revision::Std14));
    QSharedPointer<EnumeratedValueValidator> enumValidator (new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator (new FieldValidator(parser, enumValidator, parameterValidator));
    RegisterValidator validator(parser, fieldValidator, parameterValidator);

    QCOMPARE(validator.hasValidFields(testRegister, testRegister->getSize()), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testRegister, "test");

        QString expectedError = QObject::tr("Fields %1 and %2 overlap within register %3")
            .arg(fieldOne->name()).arg(fieldTwo->name()).arg(testRegister->name());

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterValidator::testFieldRangesAreValid_data()
//-----------------------------------------------------------------------------
void tst_RegisterValidator::testFieldRangesAreValid_data()
{
    QTest::addColumn<QString>("firstOffset");
    QTest::addColumn<QString>("firstWidth");
    QTest::addColumn<QString>("firstAccess");
    QTest::addColumn<QString>("secondOffset");
    QTest::addColumn<QString>("secondWidth");
    QTest::addColumn<QString>("secondAccess");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Field1: offset 0, width 2 and field2: offset 2, width 2 are valid") << "0" << "2" << "" <<
        "2" << "2" << "" << true;
    QTest::newRow("Field1: offset 0, width 2 and field2: offset 10, width 2 are valid") << "0" << "2" << "" <<
        "10" << "2" << "" << true;
    QTest::newRow("Field1: offset 0, width 2 and field2: offset 1, width 2 are invalid") << "0" << "2" << "" <<
        "1" << "2" << "" << false;
    QTest::newRow("Field1: offset 1, width 2 and field2: offset 1, width 2 are invalid") << "1" << "2" << "" <<
        "1" << "2" << "" << false;
    QTest::newRow("Field1: offset 0, width 11 and field2: offset 4, width 3 are invalid") << "0" << "11" << "" <<
        "4" << "3" << "" << false;
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterValidator::testFieldsAreWithinRegister()
//-----------------------------------------------------------------------------
void tst_RegisterValidator::testFieldsAreWithinRegister()
{
    QFETCH(QString, bitOffset);
    QFETCH(QString, bitWidth);
    QFETCH(QString, registerSize);
    QFETCH(bool, isValid);

    QSharedPointer<Field> fieldOne (new Field("FieldOne"));
    fieldOne->setBitOffset(bitOffset);
    fieldOne->setBitWidth(bitWidth);

    QSharedPointer<Register> testRegister(new Register("TestRegister"));
    testRegister->setSize(registerSize);
    testRegister->getFields()->append(fieldOne);

    QSharedPointer<ExpressionParser> parser (new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator> parameterValidator(new ParameterValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > > (), Document::Revision::Std14));
    QSharedPointer<EnumeratedValueValidator> enumValidator (new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator (new FieldValidator(parser, enumValidator, parameterValidator));
    RegisterValidator validator(parser, fieldValidator, parameterValidator);

    QCOMPARE(validator.hasValidFields(testRegister, testRegister->getSize()), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testRegister, "test");

        QString expectedError = QObject::tr("Field %1 is not contained within %2").arg(fieldOne->name()).
            arg(testRegister->name());

        bool offsetValid = false;
        bool bitwidthValid = false;
        parser->parseExpression(bitOffset, &offsetValid);
        parser->parseExpression(bitWidth, &bitwidthValid);
        if (offsetValid == false)
        {
            expectedError = QObject::tr("Invalid bit offset set for field %1 within register %2").
                arg(fieldOne->name()).arg(testRegister->name());
        }
        else if (bitwidthValid == false)
        {
            expectedError = QObject::tr("Invalid bit width set for field %1 within register %2").
                arg(fieldOne->name()).arg(testRegister->name());
        }

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterValidator::testFieldsAreWithinRegister_data()
//-----------------------------------------------------------------------------
void tst_RegisterValidator::testFieldsAreWithinRegister_data()
{
    QTest::addColumn<QString>("bitOffset");
    QTest::addColumn<QString>("bitWidth");
    QTest::addColumn<QString>("registerSize");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Field: bitOffset 0, bitWidth 4 in register with size 10 is valid") << "0" << "4" << "10" <<
        true;
    QTest::newRow("Field: bitOffset 8, bitWidth 2 in register with size 2 is invalid") << "8" << "2" << "2" << false;
    QTest::newRow("Field: bitOffset 2, bitWidth 8 in register with size 4 is invalid") << "2" << "8" << "4" << false;

    QTest::newRow("Field with long bit offset is not within a small register") <<
        "4000000000000000000000000000000000" << "0" << "10" << false;
    QTest::newRow("Field with long bit width is not within a small register") <<
        "0" << "4000000000000000000000000000000000" << "10" << false;
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterValidator::testFieldWithStrideIsWithinRegister()
//-----------------------------------------------------------------------------
void tst_RegisterValidator::testFieldWithStrideIsWithinRegister()
{
    QFETCH(QString, bitOffset);
    QFETCH(QString, bitWidth);
    QFETCH(QString, registerSize);
    QFETCH(QString, dimension);
    QFETCH(QString, stride);
    QFETCH(QString, invalidIndex);
    QFETCH(int, numErrors);
    QFETCH(bool, isValid);


    QSharedPointer<Field> fieldOne(new Field("FieldOne"));
    fieldOne->setBitOffset(bitOffset);
    fieldOne->setBitWidth(bitWidth);
    fieldOne->setDimension(dimension);
    fieldOne->setStride(stride);

    QSharedPointer<Register> testRegister(new Register("TestRegister"));
    testRegister->setAddressOffset("0");
    testRegister->setSize(registerSize);
    testRegister->getFields()->append(fieldOne);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator> parameterValidator(new ParameterValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice>>>(), Document::Revision::Std22));
    QSharedPointer<EnumeratedValueValidator> enumValidator(new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator(new FieldValidator(parser, enumValidator, parameterValidator));
    RegisterValidator validator(parser, fieldValidator, parameterValidator, Document::Revision::Std22);

    QCOMPARE(validator.hasValidFields(testRegister, testRegister->getSize()), isValid);

    if (!isValid)
    {
        QList<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testRegister, "test");
        
        QCOMPARE(foundErrors.size(), numErrors);

        QString expectedError = QObject::tr("Field %1 (%2) is not contained within %3").arg(fieldOne->name())
            .arg(invalidIndex).arg(testRegister->name());

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterValidator::testFieldWithStrideIsWithinRegister_data()
//-----------------------------------------------------------------------------
void tst_RegisterValidator::testFieldWithStrideIsWithinRegister_data()
{
    QTest::addColumn<QString>("bitOffset");
    QTest::addColumn<QString>("bitWidth");
    QTest::addColumn<QString>("registerSize");
    QTest::addColumn<QString>("dimension");
    QTest::addColumn<QString>("stride");
    QTest::addColumn<QString>("invalidIndex");
    QTest::addColumn<int>("numErrors");
    QTest::addColumn<bool>("isValid");

    // Reg size 32
    // Valid: 3 replicas of 4-bit fields starting at offset 2, stride 6, total bits used = 2 + (3-1)*6 + 4 = 16 < 32
    QTest::newRow("Valid replicated field") << "2" << "4" << "32" << "3" << "6" << "" << 0 << true;

    // Reg size 16
    // Invalid: 4 replicas of 4-bit fields starting at offset 2, stride 6, total bits used = 2 + (3-1)*6 + 4 = 21 > 16
    // => Two replicas (2,3) out of bounds
    QTest::newRow("Invalid: last replica exceeds register") << "2" << "4" << "16" << "4" << "6" << "3" << 2 << false;
}


//-----------------------------------------------------------------------------
// Function: tst_RegisterValidator::testFieldOverlapWithStride()
//-----------------------------------------------------------------------------

void tst_RegisterValidator::testFieldOverlapWithStride()
{
    QSharedPointer<Field> fieldOne(new Field("FieldOne"));
    fieldOne->setBitOffset("0");
    fieldOne->setBitWidth("4");
    fieldOne->setDimension("3"); // 3 instances
    fieldOne->setStride("8");   // Each instance starts 8 bits apart

    QSharedPointer<Field> fieldTwo(new Field("FieldTwo"));
    fieldTwo->setBitOffset("6"); // Between first (0-3) and second (8-11) instance of FieldOne
    fieldTwo->setBitWidth("2");

    QSharedPointer<Field> fieldThree(new Field("FieldThree"));
    fieldThree->setBitOffset("9"); // Overlaps with second instance of FieldOne (8-11)
    fieldThree->setBitWidth("4");

    QSharedPointer<Register> testRegister(new Register("TestRegister"));
    testRegister->setAddressOffset("0");
    testRegister->setSize("32");
    testRegister->getFields()->append(fieldOne);
    testRegister->getFields()->append(fieldTwo);
    testRegister->getFields()->append(fieldThree);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator> parameterValidator(new ParameterValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > >(), Document::Revision::Std14));
    QSharedPointer<EnumeratedValueValidator> enumValidator(new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator(new FieldValidator(parser, enumValidator, parameterValidator));
    RegisterValidator validator(parser, fieldValidator, parameterValidator);

    // This should be invalid due to overlap between FieldOne[1] and FieldThree
    QCOMPARE(validator.hasValidFields(testRegister, testRegister->getSize()), false);

    QVector<QString> foundErrors;
    validator.findErrorsIn(foundErrors, testRegister, "test");

    QString expectedError = QObject::tr("Fields %1 (1) and %2 overlap within register %3")
        .arg(fieldOne->name()).arg(fieldThree->name()).arg(testRegister->name());

    if (errorIsNotFoundInErrorList(expectedError, foundErrors))
    {
        QFAIL("Expected overlap error not found");
    }
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterValidator::testFieldReplicasOverlapDueToSmallStride()
//-----------------------------------------------------------------------------
void tst_RegisterValidator::testFieldReplicasOverlapDueToSmallStride()
{
    QSharedPointer<Field> fieldOne(new Field("FieldOne"));
    fieldOne->setBitOffset("0");
    fieldOne->setBitWidth("8");
    fieldOne->setDimension("3"); // 3 instances
    fieldOne->setStride("4");    // Too small: replicas will overlap

    QSharedPointer<Register> testRegister(new Register("TestRegister"));
    testRegister->setSize("32");
    testRegister->setAddressOffset("0");
    testRegister->getFields()->append(fieldOne);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator> parameterValidator(new ParameterValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice>>>(), Document::Revision::Std14));
    QSharedPointer<EnumeratedValueValidator> enumValidator(new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator(new FieldValidator(parser, enumValidator, parameterValidator));
    RegisterValidator validator(parser, fieldValidator, parameterValidator);

    // This should be invalid due to overlapping replicas of FieldOne
    QCOMPARE(validator.hasValidFields(testRegister, testRegister->getSize()), false);

    QVector<QString> foundErrors;
    validator.findErrorsIn(foundErrors, testRegister, "test");

    QString expectedError = QObject::tr("Fields %1 (0) and %2 (1) overlap within register %3")
        .arg(fieldOne->name()).arg(fieldOne->name()).arg(testRegister->name());

    if (errorIsNotFoundInErrorList(expectedError, foundErrors))
    {
        QFAIL("Expected overlap error due to stride was not found");
    }
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterValidator::testFieldWithDimAndEmptyStride()
//-----------------------------------------------------------------------------
void tst_RegisterValidator::testFieldWithDimAndEmptyStride()
{
    QSharedPointer<Field> fieldOne(new Field("FieldOne"));
    fieldOne->setBitOffset("0");
    fieldOne->setBitWidth("4");
    fieldOne->setDimension("2"); // Two instances
    // No stride set � should default to tightly packed (i.e., stride = width)

    QSharedPointer<Register> testRegister(new Register("TestRegister"));
    testRegister->setAddressOffset("0");
    testRegister->setSize("16");
    testRegister->getFields()->append(fieldOne);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator> parameterValidator(new ParameterValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice>>>(), Document::Revision::Std14));
    QSharedPointer<EnumeratedValueValidator> enumValidator(new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator(new FieldValidator(parser, enumValidator, parameterValidator));
    RegisterValidator validator(parser, fieldValidator, parameterValidator);

    // This should be valid: replicas are at bit offsets 0 and 4 (no overlap)
    QCOMPARE(validator.hasValidFields(testRegister, testRegister->getSize()), true);

    QVector<QString> foundErrors;
    validator.findErrorsIn(foundErrors, testRegister, "test");

    if (!foundErrors.isEmpty())
    {
        QFAIL("Unexpected error(s) found for field with empty stride");
    }
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterValidator::testFieldOverlapWithStrideAndNonStride()
//-----------------------------------------------------------------------------
void tst_RegisterValidator::testFieldOverlapWithStrideAndNonStride()
{
    QSharedPointer<Field> fieldOne(new Field("FieldOne"));
    fieldOne->setBitOffset("0");
    fieldOne->setBitWidth("4");
    fieldOne->setDimension("3"); // 3 replicas
    fieldOne->setStride("8");    // Replicas at 0, 8, 16

    QSharedPointer<Field> fieldTwo(new Field("FieldTwo"));
    fieldTwo->setBitOffset("9"); // Overlaps with second replica of FieldOne (8�11)
    fieldTwo->setBitWidth("4");

    QSharedPointer<Register> testRegister(new Register("TestRegister"));
    testRegister->setAddressOffset("0");
    testRegister->setSize("32");
    testRegister->getFields()->append(fieldOne);
    testRegister->getFields()->append(fieldTwo);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator> parameterValidator(new ParameterValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice>>>(), Document::Revision::Std14));
    QSharedPointer<EnumeratedValueValidator> enumValidator(new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator(new FieldValidator(parser, enumValidator, parameterValidator));
    RegisterValidator validator(parser, fieldValidator, parameterValidator);

    //should be invalid due to overlap between FieldOne[1] and FieldTwo
    QCOMPARE(validator.hasValidFields(testRegister, testRegister->getSize()), false);

    QVector<QString> foundErrors;
    validator.findErrorsIn(foundErrors, testRegister, "test");

    QString expectedError = QObject::tr("Fields %1 (1) and %2 overlap within register %3")
        .arg(fieldOne->name()).arg(fieldTwo->name()).arg(testRegister->name());

    if (errorIsNotFoundInErrorList(expectedError, foundErrors))
    {
        QFAIL("Expected overlap error between strided and non-strided field not found");
    }
}



//-----------------------------------------------------------------------------
// Function: tst_RegisterValidator::testFieldsHaveSimilarDefinitionGroups()
//-----------------------------------------------------------------------------
void tst_RegisterValidator::testFieldsHaveSimilarDefinitionGroups()
{
    QFETCH(QString, typeIdentifierOne);
    QFETCH(QString, bitWidthOne);
    QFETCH(bool, volatileOne);
    QFETCH(QString, accessOne);
    QFETCH(QString, enumeratedNameOne);
    QFETCH(QString, enumeratedValueOne);
    QFETCH(QString, writeConstraintMinimumOne);
    QFETCH(QString, modifiedWriteValueOne);
    QFETCH(QString, readActionOne);
    QFETCH(bool, testableOne);
    QFETCH(QString, reservedOne);

    QFETCH(QString, typeIdentifierTwo);
    QFETCH(QString, bitWidthTwo);
    QFETCH(bool, volatileTwo);
    QFETCH(QString, accessTwo);
    QFETCH(QString, enumeratedNameTwo);
    QFETCH(QString, enumeratedValueTwo);
    QFETCH(QString, writeConstraintMinimumTwo);
    QFETCH(QString, modifiedWriteValueTwo);
    QFETCH(QString, readActionTwo);
    QFETCH(bool, testableTwo);
    QFETCH(QString, reservedTwo);

    QFETCH(bool, isValid);

    QSharedPointer<EnumeratedValue> enumOne (new EnumeratedValue(enumeratedNameOne, enumeratedValueOne));

    QSharedPointer<WriteValueConstraint> constraintOne (new WriteValueConstraint());
    constraintOne->setMinimum(writeConstraintMinimumOne);
    constraintOne->setMaximum("20");

    QSharedPointer<Field> fieldOne (new Field("fieldOne"));
    fieldOne->setBitOffset("0");
    fieldOne->setBitOffset("2");
    fieldOne->setTypeIdentifier(typeIdentifierOne);
    fieldOne->setBitWidth(bitWidthOne);
    fieldOne->setVolatile(volatileOne);
    fieldOne->setAccess(AccessTypes::str2Access(accessOne, AccessTypes::ACCESS_COUNT));
    fieldOne->setModifiedWrite(General::str2ModifiedWrite(modifiedWriteValueOne));
    fieldOne->setReadAction(General::str2ReadAction(readActionOne));
    fieldOne->setTestable(testableOne);
    fieldOne->setReserved(reservedOne);
    fieldOne->setWriteConstraint(constraintOne);
    fieldOne->getEnumeratedValues()->append(enumOne);

    QSharedPointer<EnumeratedValue> enumTwo (new EnumeratedValue(enumeratedNameTwo, enumeratedValueTwo));

    QSharedPointer<WriteValueConstraint> constraintTwo (new WriteValueConstraint());
    constraintTwo->setMinimum(writeConstraintMinimumTwo);
    constraintTwo->setMaximum("20");

    QSharedPointer<Field> fieldTwo (new Field("fieldTwo"));
    fieldTwo->setBitOffset("3");
    fieldTwo->setBitWidth("2");
    fieldTwo->setTypeIdentifier(typeIdentifierTwo);
    fieldTwo->setBitWidth(bitWidthTwo);
    fieldTwo->setVolatile(volatileTwo);
    fieldTwo->setAccess(AccessTypes::str2Access(accessTwo, AccessTypes::ACCESS_COUNT));
    fieldTwo->setModifiedWrite(General::str2ModifiedWrite(modifiedWriteValueTwo));
    fieldTwo->setReadAction(General::str2ReadAction(readActionTwo));
    fieldTwo->setTestable(testableTwo);
    fieldTwo->setReserved(reservedTwo);
    fieldTwo->setWriteConstraint(constraintTwo);
    fieldTwo->getEnumeratedValues()->append(enumTwo);

    QSharedPointer<Register> testRegister(new Register("TestRegister"));
    testRegister->setSize("100");
    testRegister->getFields()->append(fieldOne);
    testRegister->getFields()->append(fieldTwo);

    QSharedPointer<ExpressionParser> parser (new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator> parameterValidator(new ParameterValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > > (), Document::Revision::Std14));
    QSharedPointer<EnumeratedValueValidator> enumValidator (new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator (new FieldValidator(parser, enumValidator, parameterValidator));
    RegisterValidator validator(parser, fieldValidator, parameterValidator);

    QCOMPARE(validator.hasValidFields(testRegister, testRegister->getSize()), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testRegister, "test");

        QString expectedError = QObject::tr("Fields %1 and %2 have type identifier %3, but different field "
            "definitions within %4").arg(fieldOne->name()).arg(fieldTwo->name())
            .arg(fieldOne->getTypeIdentifier()).arg("register " + testRegister->name());

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterValidator::testFieldsHaveSimilarDefinitionGroups_data()
//-----------------------------------------------------------------------------
void tst_RegisterValidator::testFieldsHaveSimilarDefinitionGroups_data()
{
    QTest::addColumn<QString>("typeIdentifierOne");
    QTest::addColumn<QString>("bitWidthOne");
    QTest::addColumn<bool>("volatileOne");
    QTest::addColumn<QString>("accessOne");
    QTest::addColumn<QString>("enumeratedNameOne");
    QTest::addColumn<QString>("enumeratedValueOne");
    QTest::addColumn<QString>("modifiedWriteValueOne");
    QTest::addColumn<QString>("writeConstraintMinimumOne");
    QTest::addColumn<QString>("readActionOne");
    QTest::addColumn<bool>("testableOne");
    QTest::addColumn<QString>("reservedOne");

    QTest::addColumn<QString>("typeIdentifierTwo");
    QTest::addColumn<QString>("bitWidthTwo");
    QTest::addColumn<bool>("volatileTwo");
    QTest::addColumn<QString>("accessTwo");
    QTest::addColumn<QString>("enumeratedNameTwo");
    QTest::addColumn<QString>("enumeratedValueTwo");
    QTest::addColumn<QString>("modifiedWriteValueTwo");
    QTest::addColumn<QString>("writeConstraintMinimumTwo");
    QTest::addColumn<QString>("readActionTwo");
    QTest::addColumn<bool>("testableTwo");
    QTest::addColumn<QString>("reservedTwo");

    QTest::addColumn<bool>("isValid");

    QTest::newRow("Similar field definitions with same typeIdentifier are valid") <<
        "identifier" << "1" << false << "" << "enum" << "2" << "" << "1" << "set" << true << "1" <<
        "identifier" << "1" << false << "" << "enum" << "2" << "" << "1" << "set" << true << "1" <<
        true;
    QTest::newRow("Similar field definitions with different typeIdentifier are valid") <<
        "identifier" << "1" << false << "" << "enum" << "2" << "" << "1" << "set" << true << "1" <<
        "identifier2" << "1" << false << "" << "enum" << "2" << "" << "1" << "set" << true << "1" <<
        true;
    QTest::newRow("Different field definitions with empty typeIdentifier are valid") <<
        "" << "1" << false << "" << "enum" << "2" << "" << "1" << "set" << true << "1" <<
        "" << "2" << false << "" << "enum2" << "3" << "" << "2" << "set" << true << "0" <<
        true;
    QTest::newRow("Different field definitions with different typeIdentifiers are valid") <<
        "identifier1" << "1" << false << "" << "enum" << "2" << "" << "1" << "set" << true << "1" <<
        "identifier2" << "2" << false << "" << "enum2" << "3" << "" << "2" << "set" << true << "0" <<
        true;

    QTest::newRow("Different bitWidth is invalid") <<
        "identifier" << "1" << false << "" << "enum" << "2" << "" << "1" << "set" << true << "1" <<
        "identifier" << "2" << false << "" << "enum" << "2" << "" << "1" << "set" << true << "1" <<
        false;
    QTest::newRow("Different volatile is invalid") <<
        "identifier" << "1" << false << "" << "enum" << "2" << "" << "1" << "set" << true << "1" <<
        "identifier" << "1" << true << "" << "enum" << "2" << "" << "1" << "set" << true << "1" <<
        false;
    QTest::newRow("Different access is invalid") <<
        "identifier" << "1" << false << "" << "enum" << "2" << "" << "1" << "set" << true << "1" <<
        "identifier" << "1" << false << "read-write" << "enum" << "2" << "" << "1" << "set" << true << "1" <<
        false;
    QTest::newRow("Different enumerated name is invalid") <<
        "identifier" << "1" << false << "" << "enum" << "2" << "" << "1" << "set" << true << "1" <<
        "identifier" << "1" << false << "" << "enum2" << "2" << "" << "1" << "set" << true << "1" <<
        false;
    QTest::newRow("Different enumerated value is invalid") <<
        "identifier" << "1" << false << "" << "enum" << "2" << "" << "1" << "set" << true << "1" <<
        "identifier" << "1" << false << "" << "enum" << "3" << "" << "1" << "set" << true << "1" <<
        false;
    QTest::newRow("Different modified write value is invalid") <<
        "identifier" << "1" << false << "" << "enum" << "2" << "" << "1" << "set" << true << "1" <<
        "identifier" << "1" << false << "" << "enum" << "2" << "oneToClear" << "1" << "set" << true << "1" <<
        false;
    QTest::newRow("Different write constraint minimum is invalid") <<
        "identifier" << "1" << false << "" << "enum" << "2" << "" << "1" << "set" << true << "1" <<
        "identifier" << "1" << false << "" << "enum" << "2" << "" << "2" << "set" << true << "1" <<
        false;
    QTest::newRow("Different read action is invalid") <<
        "identifier" << "1" << false << "" << "enum" << "2" << "" << "1" << "set" << true << "1" <<
        "identifier" << "1" << false << "" << "enum" << "2" << "" << "1" << "" << true << "1" <<
        false;
    QTest::newRow("Different testable value is invalid") <<
        "identifier" << "1" << false << "" << "enum" << "2" << "" << "1" << "" << true << "1" <<
        "identifier" << "1" << false << "" << "enum" << "2" << "" << "1" << "" << false << "1" <<
        false;
    QTest::newRow("Different reserved value is invalid") <<
        "identifier" << "1" << false << "" << "enum" << "2" << "" << "1" << "" << true << "1" <<
        "identifier" << "1" << false << "" << "enum" << "2" << "" << "1" << "" << true << "0+1" <<
        false;
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterValidator::testAlternateRegisterIsValid()
//-----------------------------------------------------------------------------
void tst_RegisterValidator::testAlternateRegisterIsValid()
{
    QFETCH(QString, name);
    QFETCH(QString, isPresent);
    QFETCH(QString, alternateGroup1);
    QFETCH(QString, alternateGroup2);
    QFETCH(bool, isValid);

    QSharedPointer<Field> field (new Field("TestField"));
    field->setBitWidth("2");
    field->setBitOffset("4");

    QSharedPointer<AlternateRegister> testAlternateRegister(new AlternateRegister(name, alternateGroup1));
    testAlternateRegister->setIsPresent(isPresent);
    testAlternateRegister->getAlternateGroups()->append(alternateGroup2);
    testAlternateRegister->getFields()->append(field);

    QSharedPointer<Register> testRegister (new Register("testRegister"));
    testRegister->setSize("100");
    testRegister->getAlternateRegisters()->append(testAlternateRegister);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    //     QSharedPointer<ParameterFinder> finder (new ComponentParameterFinder(QSharedPointer<Component> ()));
    QSharedPointer<ParameterValidator> parameterValidator(new ParameterValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > > (), Document::Revision::Std14));
    QSharedPointer<EnumeratedValueValidator> enumValidator (new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator (new FieldValidator(parser, enumValidator, parameterValidator));
    RegisterValidator validator(parser, fieldValidator, parameterValidator);
    QCOMPARE(validator.hasValidAlternateRegisters(testRegister), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testRegister, "test");

        QString expectedError;
        if (!validator.hasValidName(testAlternateRegister))
        {
            expectedError = QObject::tr("Invalid name specified for alternate register %1 within %2").
                arg(testAlternateRegister->name()).arg("register " + testRegister->name());

            if (errorIsNotFoundInErrorList(expectedError, foundErrors))
            {
                QFAIL("No error message found");
            }
        }

        if (!validator.hasValidIsPresent(testAlternateRegister))
        {
            expectedError = QObject::tr("Invalid isPresent set for alternate register %1 within %2").
                arg(testAlternateRegister->name()).arg("register " + testRegister->name());

            if (errorIsNotFoundInErrorList(expectedError, foundErrors))
            {
                QFAIL("No error message found");
            }
        }

        if (!validator.hasValidAlternateGroups(testAlternateRegister))
        {
            expectedError = QObject::tr("Alternate groups are not unique or not empty in alternate register %1 within %2").
                arg(testAlternateRegister->name()).arg("register " + testRegister->name());

            if (errorIsNotFoundInErrorList(expectedError, foundErrors))
            {
                QFAIL("No error message found");
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterValidator::testAlternateRegisterIsValid_data()
//-----------------------------------------------------------------------------
void tst_RegisterValidator::testAlternateRegisterIsValid_data()
{
    QTest::addColumn<QString>("name");
    QTest::addColumn<QString>("isPresent");
    QTest::addColumn<QString>("alternateGroup1");
    QTest::addColumn<QString>("alternateGroup2");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Alternate register without a name is invalid") << "" << "" << "alternate" << "beast" << false;
    QTest::newRow("Alternate register with name and alternate group is valid") << "Golden" << "" << "Axe" << "I" <<
        true;
    QTest::newRow("Alternate register without alternate group is invalid") << "Vendetta" << "" << "" << "" <<
        false;
    QTest::newRow("Alternate register with non-unique alternate groups is invalid") << "Baseball" << "" <<
        "Stars" << "Stars" << false;

    QTest::newRow("IsPresent 1 is valid") << "Metal" << "1" << "Slug" << "SuperVehicle" << true;
    QTest::newRow("IsPresent 1*3-3 is valid") << "Metal" << "1*3-3" << "Slug" << "II" << true;
    QTest::newRow("IsPresent 2*100 is invalid") << "Metal" << "2*100" << "Slug" << "X" << false;
    QTest::newRow("IsPresent -14 is invalid") << "Metal" << "-14" << "Slug" << "3" << false;
    QTest::newRow("Real number isPresent  0.12 is invalid") << "Metal" << "0.12" << "Slug" << "4" << false;
    QTest::newRow("Text as isPresent is invalid") << "Metal" << "test" << "Slug" << "5" << false;
    QTest::newRow("String as isPresent is invalid") << "Metal" << "\"test\"" << "Slug" << "6" << false;
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterValidator::testMultipleAlternateRegistersAreValid()
//-----------------------------------------------------------------------------
void tst_RegisterValidator::testMultipleAlternateRegistersAreValid()
{
    QSharedPointer<Field> field (new Field("TestField"));
    field->setBitOffset("4");
    field->setBitWidth("2");

    QSharedPointer<AlternateRegister> testAlternateRegister(new AlternateRegister("register1", "group"));
    testAlternateRegister->getFields()->append(field);
    QSharedPointer<AlternateRegister> otherAlternateRegister(new AlternateRegister("register2", "group2"));
    otherAlternateRegister->getFields()->append(field);

    QSharedPointer<Register> testRegister (new Register("testRegister"));
    testRegister->setSize("100");
    testRegister->getAlternateRegisters()->append(testAlternateRegister);
    testRegister->getAlternateRegisters()->append(otherAlternateRegister);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator> parameterValidator(new ParameterValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > > (), Document::Revision::Std14));
    QSharedPointer<EnumeratedValueValidator> enumValidator (new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator (new FieldValidator(parser, enumValidator, parameterValidator));
    RegisterValidator validator(parser, fieldValidator, parameterValidator);
    QCOMPARE(validator.hasValidAlternateRegisters(testRegister), true);

    QSharedPointer<AlternateRegister> thirdAlternateRegister(new AlternateRegister(*testAlternateRegister.data()));
    testRegister->getAlternateRegisters()->append(thirdAlternateRegister);
    QCOMPARE(validator.hasValidAlternateRegisters(testRegister), false);

    QVector<QString> foundErrors;
    validator.findErrorsIn(foundErrors, testRegister, "test");

    QString expectedError = QObject::tr("Name %1 of alternate registers in %2 is not unique.").
        arg(thirdAlternateRegister->name()).arg("register " + testRegister->name());

    if (errorIsNotFoundInErrorList(expectedError, foundErrors))
    {
        QFAIL("No error message found");
    }
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterValidator::testAlternateRegisterParametersAreValid()
//-----------------------------------------------------------------------------
void tst_RegisterValidator::testAlternateRegisterParametersAreValid()
{
    QSharedPointer<Parameter> testParameter (new Parameter());
    testParameter->setValueId("Sanger");
    testParameter->setName("Daizengar");
    testParameter->setValue("20");
    testParameter->setType("int");

    QSharedPointer<Field> field (new Field("testField"));
    field->setBitOffset("2");
    field->setBitWidth("4");

    QSharedPointer<AlternateRegister> testAlternateRegister (new AlternateRegister("Alternate"));
    testAlternateRegister->getParameters()->append(testParameter);
    testAlternateRegister->getFields()->append(field);

    QSharedPointer<Register> testRegister (new Register("TestRegister"));
    testRegister->getAlternateRegisters()->append(testAlternateRegister);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator> parameterValidator(new ParameterValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > > (), Document::Revision::Std14));
    QSharedPointer<EnumeratedValueValidator> enumValidator (new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator (new FieldValidator(parser, enumValidator, parameterValidator));
    RegisterValidator validator(parser, fieldValidator, parameterValidator);

    QCOMPARE(validator.hasValidParameters(testAlternateRegister), true);

    testParameter->setValue("");
    QCOMPARE(validator.hasValidParameters(testAlternateRegister), false);

    QVector<QString> errorsFound;
    validator.findErrorsIn(errorsFound, testRegister, "test");

    QString expectedError = QObject::tr("No value specified for %1 %2 within alternate register %3 within register %4").
        arg(testParameter->elementName()).arg(testParameter->name()).arg(testAlternateRegister->name()).
        arg(testRegister->name());

    if (errorIsNotFoundInErrorList(expectedError, errorsFound))
    {
        QFAIL("Error was not found");
    }

    QSharedPointer<Parameter> otherParameter (new Parameter(*testParameter.data()));
    testParameter->setValue("1");
    otherParameter->setValue("2");
    testAlternateRegister->getParameters()->append(otherParameter);

    QCOMPARE(validator.hasValidParameters(testAlternateRegister), false);

    errorsFound.clear();
    validator.findErrorsIn(errorsFound, testRegister, "test");
    expectedError = QObject::tr("Name %1 of parameters in alternate register %2 within register %3 is not unique.").
        arg(otherParameter->name()).arg(testAlternateRegister->name()).arg(testRegister->name());
    if (errorIsNotFoundInErrorList(expectedError, errorsFound))
    {
        QFAIL("Error was not found");
    }
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterValidator::testParametersAreValid()
//-----------------------------------------------------------------------------
void tst_RegisterValidator::testParametersAreValid()
{
    QSharedPointer<Parameter> testParameter (new Parameter());
    testParameter->setValueId("Sanger");
    testParameter->setName("Daizengar");
    testParameter->setValue("20");
    testParameter->setType("int");

    QSharedPointer<Register> testRegister (new Register("TestRegister"));
    testRegister->getParameters()->append(testParameter);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator> parameterValidator(new ParameterValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > > (), Document::Revision::Std14));
    QSharedPointer<EnumeratedValueValidator> enumValidator (new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator (new FieldValidator(parser, enumValidator, parameterValidator));
    RegisterValidator validator(parser, fieldValidator, parameterValidator);

    QCOMPARE(validator.hasValidParameters(testRegister), true);

    testParameter->setValue("");
    QCOMPARE(validator.hasValidParameters(testRegister), false);

    QVector<QString> errorsFound;
    validator.findErrorsIn(errorsFound, testRegister, "test");

    QString expectedError = QObject::tr("No value specified for %1 %2 within register %3").
        arg(testParameter->elementName()).arg(testParameter->name()).arg(testRegister->name());

    if (errorIsNotFoundInErrorList(expectedError, errorsFound))
    {
        QFAIL("Error was not found");
    }

    QSharedPointer<Parameter> otherParameter (new Parameter(*testParameter.data()));
    testParameter->setValue("1");
    otherParameter->setValue("2");
    testRegister->getParameters()->append(otherParameter);

    QCOMPARE(validator.hasValidParameters(testRegister), false);

    errorsFound.clear();
    validator.findErrorsIn(errorsFound, testRegister, "test");
    expectedError = QObject::tr("Name %1 of parameters in register %2 is not unique.").arg(otherParameter->name()).
        arg(testRegister->name());
    if (errorIsNotFoundInErrorList(expectedError, errorsFound))
    {
        QFAIL("Error was not found");
    }
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterValidator::testRegisterMemoryArrayIsValid2022()
//-----------------------------------------------------------------------------
void tst_RegisterValidator::testRegisterMemoryArrayIsValid2022()
{
    QFETCH(QString, dimValue);
    QFETCH(QString, indexVar);
    QFETCH(QString, stride);
    QFETCH(bool, validDim);
    QFETCH(bool, validStride);

    QSharedPointer<Register> testRegister(new Register("test", "8", "8"));
    
    QSharedPointer<MemoryArray> testMemArray(new MemoryArray());

    QSharedPointer<MemoryArray::Dimension> testDim(new MemoryArray::Dimension);
    testDim->value_ = dimValue;
    testDim->indexVar_ = indexVar;
    
    if (dimValue.isEmpty() == false)
    {
        testMemArray->getDimensions()->append(testDim);
    }

    testMemArray->setStride(stride);

    testRegister->setMemoryArray(testMemArray);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator> parameterValidator(new ParameterValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > >(), Document::Revision::Std22));
    QSharedPointer<EnumeratedValueValidator> enumValidator(new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator(new FieldValidator(parser, enumValidator, parameterValidator));
    RegisterValidator validator(parser, fieldValidator, parameterValidator, Document::Revision::Std22);

    QCOMPARE(validator.hasValidMemoryArray(testRegister), validDim && validStride);

    if (!(validDim && validStride))
    {
        QStringList foundErrors;
        validator.findErrorsIn(foundErrors, testRegister, "test");

        QString expectedError;

        if (testMemArray->getDimensions()->isEmpty())
        {
            expectedError = "No dimensions defined for memory array in register 'test' within test";
        }

        else if (!validDim)
        {
            expectedError = "One or more dimensions of the memory array in register 'test' within test have an invalid value.";
        }

        else if (!validStride)
        {
            expectedError = "Memory array in register 'test' within test has an invalid stride value.";
        }

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterValidator::testRegisterMemoryArrayIsValid2022_data()
//-----------------------------------------------------------------------------
void tst_RegisterValidator::testRegisterMemoryArrayIsValid2022_data()
{
    QTest::addColumn<QString>("dimValue");
    QTest::addColumn<QString>("indexVar");
    QTest::addColumn<QString>("stride");
    QTest::addColumn<bool>("validDim");
    QTest::addColumn<bool>("validStride");

    QTest::newRow("Dimension with valid values is valid") << "8" << "i" << "8" << true << true;
    QTest::newRow("Dimension without stride is valid") << "8" << "i" << "" << true << true;
    QTest::newRow("Valid expression in dim is valid") << "1+1" << "index" << "" << true << true;
    QTest::newRow("Invalid expression in dim is not valid") << "a+a" << "index" << "" << false << true;
    QTest::newRow("Invalid value in dim is not valid") << "-1" << "index" << "" << false << true;
    QTest::newRow("Valid expression in stride is valid") << "1" << "index" << "1+1" << true << true;
    QTest::newRow("Invalid expression in stride is not valid") << "1" << "index" << "a+a" << true << false;
    QTest::newRow("Invalid value in stride is not valid") << "1" << "index" << "-1" << true << false;
    QTest::newRow("Dimension doesn't exist is invalid") << "" << "" << "" << false << false;
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterValidator::testRegisterAccessPolicies2022()
//-----------------------------------------------------------------------------
void tst_RegisterValidator::testRegisterAccessPoliciesAreValid2022()
{
    QSharedPointer<Register> testRegister(new Register("test", "8", "8"));

    QSharedPointer<ModeReference> modeRef1(new ModeReference());
    modeRef1->setPriority(0);
    modeRef1->setReference("ref");

    QSharedPointer<ModeReference> modeRef2(new ModeReference());
    modeRef2->setPriority(0);
    modeRef2->setReference("ref1");

    QSharedPointer<AccessPolicy> accessPolicy1(new AccessPolicy());
    QSharedPointer<AccessPolicy> accessPolicy2(new AccessPolicy());
    accessPolicy1->getModeReferences()->append(modeRef1);
    accessPolicy2->getModeReferences()->append(modeRef2);

    testRegister->getAccessPolicies()->append(accessPolicy1);
    testRegister->getAccessPolicies()->append(accessPolicy2);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator> parameterValidator(new ParameterValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > >(), Document::Revision::Std22));
    QSharedPointer<EnumeratedValueValidator> enumValidator(new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator(new FieldValidator(parser, enumValidator, parameterValidator));
    RegisterValidator validator(parser, fieldValidator, parameterValidator, Document::Revision::Std22);
    
    QSharedPointer<Component> dummyComponent(new Component(VLNV(VLNV::COMPONENT, "vendor", "library", "name", "version"), Document::Revision::Std22));
    QSharedPointer<Mode> mode1(new Mode("ref"));
    QSharedPointer<Mode> mode2(new Mode("ref1"));
    dummyComponent->getModes()->append(mode1);
    dummyComponent->getModes()->append(mode2);

    validator.componentChange(dummyComponent);

    QStringList errors;

    QStringList possibleErrors(QStringList()
        << "One or more mode references in access policies of register 'test' within test contain duplicate priority values."
        << "One or more mode references in access policies of register 'test' within test contain duplicate mode reference values."
        << "Mode reference in access policies of register 'test' within test has invalid or empty reference value 'ref2'."
        << "In register test in test, multiple access policies are not allowed if one of them lacks a mode reference."
    );

    // Test duplicate mode reference priority.
    validator.findErrorsIn(errors, testRegister, "test");
    QVERIFY(errors.contains(possibleErrors.first()));
    QVERIFY(validator.hasValidAccessPolicies(testRegister) == false);

    // Test duplicate reference.
    errors.clear();

    modeRef2->setPriority(1);
    modeRef2->setReference("ref");

    validator.findErrorsIn(errors, testRegister, "test");
    QVERIFY(errors.contains(possibleErrors.at(1)));
    QVERIFY(validator.hasValidAccessPolicies(testRegister) == false);

    // Test invalid mode ref.
    errors.clear();

    modeRef2->setReference("ref2");

    validator.findErrorsIn(errors, testRegister, "test");
    QVERIFY(errors.contains(possibleErrors.at(2)));
    
    
    // Test valid mode refs.
    errors.clear();
    modeRef2->setReference("ref1");
    
    validator.findErrorsIn(errors, testRegister, "test");
    QVERIFY(std::none_of(possibleErrors.cbegin(), possibleErrors.cend(), [&errors](QString const& str)
        {
            return errors.contains(str);
        }));

    QVERIFY(validator.hasValidAccessPolicies(testRegister));
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterValidator::testModeRefsOfAlternativeRegisetrsAreValid2022()
//-----------------------------------------------------------------------------
void tst_RegisterValidator::testModeRefsOfAlternativeRegisetrsAreValid2022()
{
    QSharedPointer<Field> registerField(new Field("regField"));
    registerField->setBitOffset("8");
    registerField->setTypeDefinitionsRef("testTypeDefinitions");
    registerField->setFieldDefinitionRef("testField");

    QSharedPointer<Field> altRegisterField(new Field("altRegField"));
    altRegisterField->setBitOffset("8");
    altRegisterField->setTypeDefinitionsRef("testTypeDefinitions2");
    altRegisterField->setFieldDefinitionRef("testField2");

    QSharedPointer<Register> testRegister(new Register("testRegister", "10", "32"));
    QSharedPointer<AlternateRegister> alternateRegister(new AlternateRegister("testAlternate"));

    testRegister->getFields()->append(registerField);
    alternateRegister->getFields()->append(altRegisterField);

    testRegister->getAlternateRegisters()->append(alternateRegister);

    QSharedPointer<AlternateRegister> alternateRegister2(new AlternateRegister(*alternateRegister));
    alternateRegister2->setName("testAlternate2");

    testRegister->getAlternateRegisters()->append(alternateRegister2);

    QSharedPointer<ModeReference> modeRef1(new ModeReference());
    modeRef1->setPriority(0);
    modeRef1->setReference("testMode");
    
    QSharedPointer<ModeReference> modeRef2(new ModeReference());
    modeRef2->setPriority(1);
    modeRef2->setReference("testMode2");

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator> parameterValidator(new ParameterValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > >(), Document::Revision::Std22));
    QSharedPointer<EnumeratedValueValidator> enumValidator(new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator(new FieldValidator(parser, enumValidator, parameterValidator));
    RegisterValidator validator(parser, fieldValidator, parameterValidator, Document::Revision::Std22);

    QSharedPointer<Component> dummyComponent(new Component(VLNV(VLNV::COMPONENT, "vendor", "library", "name", "version"), Document::Revision::Std22));
    QSharedPointer<Mode> mode1(new Mode("testMode"));
    QSharedPointer<Mode> mode2(new Mode("testMode2"));
    dummyComponent->getModes()->append(mode1);
    dummyComponent->getModes()->append(mode2);

    validator.componentChange(dummyComponent);

    QStringList errors;

    // Test no mode refs = error
    validator.findErrorsIn(errors, testRegister, "test");
    QCOMPARE(errors.size(), 2);
    QVERIFY(validator.hasValidAlternateRegisters(testRegister) == false);
    QVERIFY(errors.contains(QString("Alternate register testAlternate within register testRegister must have at least one mode reference defined.")));

    errors.clear();

    // test two valid mode refs
    alternateRegister->getModeReferences()->append(modeRef1);
    alternateRegister2->getModeReferences()->append(modeRef2);

    validator.findErrorsIn(errors, testRegister, "test");
    QVERIFY(errors.isEmpty());
    QVERIFY(validator.hasValidAlternateRegisters(testRegister));

    errors.clear();

    // Test duplicate priority
    modeRef1->setPriority(1);

    validator.findErrorsIn(errors, testRegister, "test");
    QCOMPARE(errors.size(), 1);
    QVERIFY(validator.hasValidAlternateRegisters(testRegister) == false);
    
    errors.clear();
    modeRef1->setPriority(0);

    // Test duplicate reference
    modeRef1->setReference("testMode2");

    validator.findErrorsIn(errors, testRegister, "test");
    QCOMPARE(errors.size(), 1);
    QVERIFY(validator.hasValidAlternateRegisters(testRegister) == false);

    errors.clear();

    // Test invalid reference value.
    modeRef1->setReference("value with whitespace");
    validator.findErrorsIn(errors, testRegister, "test");
    QCOMPARE(errors.size(), 1);
    QVERIFY(errors.contains("Mode reference in alternate register testAlternate within register testRegister has invalid or empty reference value 'value with whitespace'."));
    QVERIFY(validator.hasValidAlternateRegisters(testRegister) == false);
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterValidator::testAlternateRegisterAccessPolicies2022()
//-----------------------------------------------------------------------------
void tst_RegisterValidator::testAlternateRegisterAccessPoliciesAreValid2022()
{
    QSharedPointer<Register> testRegister(new Register("testRegister", "10", "32"));
    QSharedPointer<AlternateRegister> alternateRegister(new AlternateRegister("testAlternate"));

    QSharedPointer<Field> registerField(new Field("regField"));
    registerField->setBitOffset("8");
    registerField->setTypeDefinitionsRef("testTypeDefinitions");
    registerField->setFieldDefinitionRef("testField");

    QSharedPointer<Field> altRegisterField(new Field("altRegField"));
    altRegisterField->setBitOffset("8");
    altRegisterField->setTypeDefinitionsRef("testTypeDefinitions2");
    altRegisterField->setFieldDefinitionRef("testField2");

    testRegister->getFields()->append(registerField);
    alternateRegister->getFields()->append(altRegisterField);

    testRegister->getAlternateRegisters()->append(alternateRegister);

    QSharedPointer<ModeReference> modeRef1(new ModeReference());
    modeRef1->setPriority(0);
    modeRef1->setReference("ref");

    QSharedPointer<ModeReference> modeRef2(new ModeReference());
    modeRef2->setPriority(0);
    modeRef2->setReference("ref1");
    
    QSharedPointer<ModeReference> altRegisterModeRef(new ModeReference());
    altRegisterModeRef->setPriority(1000);
    altRegisterModeRef->setReference("superUnique");

    QSharedPointer<AccessPolicy> accessPolicy1(new AccessPolicy());
    QSharedPointer<AccessPolicy> accessPolicy2(new AccessPolicy());
    accessPolicy1->getModeReferences()->append(modeRef1);
    accessPolicy2->getModeReferences()->append(modeRef2);

    alternateRegister->getAccessPolicies()->append(accessPolicy1);
    alternateRegister->getAccessPolicies()->append(accessPolicy2);
    alternateRegister->getModeReferences()->append(altRegisterModeRef);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator> parameterValidator(new ParameterValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > >(), Document::Revision::Std22));
    QSharedPointer<EnumeratedValueValidator> enumValidator(new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator(new FieldValidator(parser, enumValidator, parameterValidator));
    RegisterValidator validator(parser, fieldValidator, parameterValidator, Document::Revision::Std22);

    QSharedPointer<Component> dummyComponent(new Component(VLNV(VLNV::COMPONENT, "vendor", "library", "name", "version"), Document::Revision::Std22));
    QSharedPointer<Mode> mode1(new Mode("ref"));
    QSharedPointer<Mode> mode2(new Mode("ref1"));
    QSharedPointer<Mode> mode3(new Mode("superUnique"));
    dummyComponent->getModes()->append(mode1);
    dummyComponent->getModes()->append(mode2);
    dummyComponent->getModes()->append(mode3);

    validator.componentChange(dummyComponent);

    QStringList errors;

    QStringList possibleErrors(QStringList()
        << "One or more mode references in access policies of alternate register testAlternate within register testRegister contain duplicate priority values."
        << "One or more mode references in access policies of alternate register testAlternate within register testRegister contain duplicate mode reference values."
        << "In register 'test' in test, multiple access policies are not allowed if one of them lacks a mode reference."
    );

    // Test duplicate mode reference priority.
    validator.findErrorsIn(errors, testRegister, "test");
    QVERIFY(errors.contains(possibleErrors.first()));
    QVERIFY(validator.hasValidAlternateRegisters(testRegister) == false);

    // Test duplicate reference.
    errors.clear();

    modeRef2->setPriority(1);
    modeRef2->setReference("ref");

    validator.findErrorsIn(errors, testRegister, "test");
    QVERIFY(errors.contains(possibleErrors.at(1)));
    QVERIFY(validator.hasValidAlternateRegisters(testRegister) == false);

    // Test valid mode ref.
    errors.clear();

    modeRef2->setReference("ref1");

    validator.findErrorsIn(errors, testRegister, "test");
    QVERIFY(std::none_of(possibleErrors.cbegin(), possibleErrors.cend(), [&errors](QString const& str)
        {
            return errors.contains(str);
        }));

    QVERIFY(validator.hasValidAlternateRegisters(testRegister));
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterValidator::errorIsNotFoundInErrorList()
//-----------------------------------------------------------------------------
bool tst_RegisterValidator::errorIsNotFoundInErrorList(QString const& expectedError, QVector<QString> errorList)
{
    if (!errorList.contains(expectedError))
    {
        qDebug() << "The following error:" << Qt::endl << expectedError << Qt::endl << "was not found in error list:";
        for (auto& error : errorList)
        {
            qDebug() << error;
        }
        return true;
    }

    return false;
}

QTEST_APPLESS_MAIN(tst_RegisterValidator)

#include "tst_RegisterValidator.moc"
