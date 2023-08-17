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

#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/MemoryArray.h>
#include <IPXACTmodels/Component/Choice.h>
#include <IPXACTmodels/Component/Field.h>
#include <IPXACTmodels/Component/EnumeratedValue.h>
#include <IPXACTmodels/Component/WriteValueConstraint.h>
#include <IPXACTmodels/Component/ResetType.h>
#include <IPXACTmodels/Component/FieldReset.h>

#include <IPXACTmodels/common/validators/ParameterValidator.h>
#include <IPXACTmodels/Component/validators/FieldValidator.h>
#include <IPXACTmodels/Component/validators/EnumeratedValueValidator.h>

#include <KactusAPI/include/SystemVerilogExpressionParser.h>

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

    void testMemoryArrayIsValid();
    void testMemoryArrayIsValid_data();

    void testBitOffsetIsValid();
    void testBitOffsetIsValid_data();

    void testfieldDefinitionRefIsValid();
    void testfieldDefinitionRefIsValid_data();

    void testResetsAreValid();
    void testResetsAreValid_data();

    void testResetTypeRefIsValid();
    void testResetTypeRefIsValid_data();

    void testMultipleResetTypeRefIsValid();
    void testMultipleResetTypeRefIsValid_data();

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

    void testAccessPolicyModeRefs();

    // Test field structure validity for new std.
    void testStructureValidity();

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
    QSharedPointer<ParameterValidator> parameterValidator (
        new ParameterValidator(parser, QSharedPointer<QList<QSharedPointer<Choice> > > ()));
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
    QSharedPointer<ParameterValidator> parameterValidator (
        new ParameterValidator(parser, QSharedPointer<QList<QSharedPointer<Choice> > > ()));
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
// Function: tst_FieldValidator::testMemoryArrayIsValid()
//-----------------------------------------------------------------------------
void tst_FieldValidator::testMemoryArrayIsValid()
{
    QFETCH(QString, dimValue);
    QFETCH(QString, indexVar);
    QFETCH(QString, bitStride);
    QFETCH(bool, isValid);

    QSharedPointer<Field> testField(new Field());
    testField->setName("testField");
    
    QSharedPointer<MemoryArray> memArray(new MemoryArray());
    QSharedPointer<MemoryArray::Dimension> dim(new MemoryArray::Dimension({ dimValue, indexVar }));
    if (dimValue.isEmpty() == false)
    {
        memArray->getDimensions()->append(dim);
    }
    memArray->setStride(bitStride);

    testField->setMemoryArray(memArray);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<EnumeratedValueValidator> enumeratedValueValidator(new EnumeratedValueValidator(parser));
    QSharedPointer<ParameterValidator> parameterValidator(
        new ParameterValidator(parser, QSharedPointer<QList<QSharedPointer<Choice> > >()));
    FieldValidator validator(parser, enumeratedValueValidator, parameterValidator, Document::Revision::Std22);

    QCOMPARE(validator.hasValidMemoryArray(testField), isValid);

    if (!isValid)
    {
        QList<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testField, "test");

        QString expectedError = QObject::tr("No dimensions defined for memory array in test");
        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_FieldValidator::testMemoryArrayIsValid_data()
//-----------------------------------------------------------------------------
void tst_FieldValidator::testMemoryArrayIsValid_data()
{
    QTest::addColumn<QString>("dimValue");
    QTest::addColumn<QString>("indexVar");
    QTest::addColumn<QString>("bitStride");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Dimension exists is valid") << "8" << "i" << "8" << true;
    QTest::newRow("Dimension doesn't exist is invalid") << "" << "" << "" << false;
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
    QSharedPointer<ParameterValidator> parameterValidator (
        new ParameterValidator(parser, QSharedPointer<QList<QSharedPointer<Choice> > > ()));
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
// Function: tst_FieldValidator::testfieldDefinitionRefIsValid()
//-----------------------------------------------------------------------------
void tst_FieldValidator::testfieldDefinitionRefIsValid()
{
    QFETCH(QString, defRef);
    QFETCH(QString, typeDefRef);
    QFETCH(bool, isValid);

    QSharedPointer<Field> testField(new Field());
    testField->setName("testField");
    testField->setTypeDefinitionsRef(typeDefRef);
    testField->setFieldDefinitionRef(defRef);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<EnumeratedValueValidator> enumeratedValueValidator(new EnumeratedValueValidator(parser));
    QSharedPointer<ParameterValidator> parameterValidator(
        new ParameterValidator(parser, QSharedPointer<QList<QSharedPointer<Choice> > >()));
    FieldValidator validator(parser, enumeratedValueValidator, parameterValidator, Document::Revision::Std22);
    QCOMPARE(validator.hasValidFieldDefinitionRef(testField), isValid);

    if (!isValid)
    {
        QStringList errors;
        validator.findErrorsIn(errors, testField, "test");

        QString expectedError("Field testField within test has no typeDefinitions reference "
            "defined for field definition reference.");

        if (errorIsNotFoundInErrorList(expectedError, errors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_FieldValidator::testfieldDefinitionRefIsValid_data()
//-----------------------------------------------------------------------------
void tst_FieldValidator::testfieldDefinitionRefIsValid_data()
{
    QTest::addColumn<QString>("defRef");
    QTest::addColumn<QString>("typeDefRef");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Missing type definitions ref") << "testFieldDef" << "" << false;
    QTest::newRow("Both present") << "testFieldDef" << "testTypeDefinitions" << true;
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
    
    QSharedPointer<FieldReset> testReset(new FieldReset());
    testReset->setResetValue(resetValue);
    testReset->setResetMask(resetMask);
    testField->getResets()->append(testReset);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<EnumeratedValueValidator> enumeratedValueValidator (new EnumeratedValueValidator(parser));
    QSharedPointer<ParameterValidator> parameterValidator (
        new ParameterValidator(parser, QSharedPointer<QList<QSharedPointer<Choice> > > ()));
    FieldValidator validator(parser, enumeratedValueValidator, parameterValidator);
    QCOMPARE(validator.hasValidResetValue(testField->getResets()->first()), resetValueIsValid);

    if (!resetValueIsValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testField, "test");

        QString expectedError = QObject::tr("Invalid reset value set within field %1 in %2").
            arg(testField->name()).arg("test");
        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }

    QCOMPARE(validator.hasValidResetMask(testField->getResets()->first()), resetMaskIsValid);
    if (!resetMaskIsValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testField, "test");

        QString expectedError = QObject::tr("Invalid reset mask set within field %1 in %2").
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
// Function: tst_FieldValidator::testResetTypeRefIsValid()
//-----------------------------------------------------------------------------
void tst_FieldValidator::testResetTypeRefIsValid()
{
    QFETCH(QString, resetTypeReference);
    QFETCH(bool, buildResetType);
    QFETCH(bool, createResetValue);
    QFETCH(bool, isValid);

    QSharedPointer<Field> testField(new Field());
    testField->setName("TestField");

    QSharedPointer<FieldReset> testReset(new FieldReset());
    testReset->setResetTypeReference(resetTypeReference);

    if (createResetValue)
    {
        testReset->setResetValue("1");
    }

    testField->getResets()->append(testReset);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<EnumeratedValueValidator> enumeratedValueValidator(new EnumeratedValueValidator(parser));
    QSharedPointer<ParameterValidator> parameterValidator(
        new ParameterValidator(parser, QSharedPointer<QList<QSharedPointer<Choice> > >()));
    FieldValidator validator(parser, enumeratedValueValidator, parameterValidator);

    if (buildResetType)
    {
        QSharedPointer<ResetType> testReset(new ResetType());
        testReset->setName(resetTypeReference);

        QSharedPointer<QList<QSharedPointer<ResetType> > > componentResets(new QList<QSharedPointer<ResetType> >());
        componentResets->append(testReset);

        QSharedPointer<Component> newComponent(new Component(VLNV(), Document::Revision::Std14));
        newComponent->setResetTypes(componentResets);

        validator.componentChange(newComponent);
    }

    bool resetValueIsValid = validator.hasValidResetValue(testField->getResets()->first());
    bool resetTypeReferenceIsValid = validator.hasValidResetTypeReference(testField->getResets()->first());
    QCOMPARE(resetValueIsValid && resetTypeReferenceIsValid, isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testField, "test");

        QString expectedError = QObject::tr("Invalid reset value set within field %1 in %2").
            arg(testField->name()).arg("test");

        if (!buildResetType)
        {
            expectedError = QObject::tr("Reset type '%1' referenced in field %2 in test does not exist.")
                .arg(resetTypeReference).arg(testField->name());
        }

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_FieldValidator::testResetTypeRefIsValid_data()
//-----------------------------------------------------------------------------
void tst_FieldValidator::testResetTypeRefIsValid_data()
{
    QTest::addColumn<QString>("resetTypeReference");
    QTest::addColumn<bool>("buildResetType");
    QTest::addColumn<bool>("createResetValue");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Empty reset type reference is valid") << "" << false << true << true;
    QTest::newRow("Referencing an existing reset type is valid") << "SOFT" << true << true << true;
    QTest::newRow("Referencing a non-existing reset type is not valid") << "WRONG" << false << true << false;
    QTest::newRow("Reset type without reset value is not valid") << "SOFT" << true << false << false;
}

//-----------------------------------------------------------------------------
// Function: tst_FieldValidator::testMultipleResetTypeRefIsValid()
//-----------------------------------------------------------------------------
void tst_FieldValidator::testMultipleResetTypeRefIsValid()
{
    QFETCH(QString, firstResetReference);
    QFETCH(QString, secondResetReference);
    QFETCH(bool, isValid);

    QSharedPointer<Field> testField(new Field());
    testField->setName("field");

    QSharedPointer<FieldReset> firstReset(new FieldReset());
    firstReset->setResetTypeReference(firstResetReference);
    firstReset->setResetValue("1");

    QSharedPointer<FieldReset> secondReset(new FieldReset());
    secondReset->setResetTypeReference(secondResetReference);
    secondReset->setResetValue("0");

    testField->getResets()->append(firstReset);
    testField->getResets()->append(secondReset);

    QSharedPointer<QList<QSharedPointer<ResetType> > > availableResetTypes(
        new QList<QSharedPointer<ResetType> >());
    if (!firstResetReference.isEmpty())
    {
        QSharedPointer<ResetType> firstResetType(new ResetType());
        firstResetType->setName(firstResetReference);
        availableResetTypes->append(firstResetType);
    }
    if (!secondResetReference.isEmpty() && firstResetReference != secondResetReference)
    {
        QSharedPointer<ResetType> secondResetType(new ResetType());
        secondResetType->setName(secondResetReference);
        availableResetTypes->append(secondResetType);
    }

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<EnumeratedValueValidator> enumeratedValueValidator(new EnumeratedValueValidator(parser));
    QSharedPointer<ParameterValidator> parameterValidator(
        new ParameterValidator(parser, QSharedPointer<QList<QSharedPointer<Choice> > >()));
    FieldValidator validator(parser, enumeratedValueValidator, parameterValidator);
    
    QSharedPointer<Component> dummyComponent(new Component(VLNV(), Document::Revision::Std14));
    dummyComponent->setResetTypes(availableResetTypes);

    validator.componentChange(dummyComponent);

    QCOMPARE(validator.hasValidResets(testField), isValid);
    if (!isValid)
    {
        QString context = "test";

        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testField, context);

        QString expectedError = QObject::tr("Multiple references to default reset type in field %1 in %2.")
            .arg(testField->name()).arg(context);

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_FieldValidator::()
//-----------------------------------------------------------------------------
void tst_FieldValidator::testMultipleResetTypeRefIsValid_data()
{
    QTest::addColumn<QString>("firstResetReference");
    QTest::addColumn<QString>("secondResetReference");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Separate reset type references are valid") << "SOFT_1" << "SOFT_2" << true;
    QTest::newRow("Same reset type references are valid") << "SOFT" << "SOFT" << true;
    QTest::newRow("Single empty reset type reference is valid") << "" << "SOFT" << true;
    QTest::newRow("Multiple empty reset type references is not valid") << "" << "" << false;
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
    QSharedPointer<ParameterValidator> parameterValidator (
        new ParameterValidator(parser, QSharedPointer<QList<QSharedPointer<Choice> > > ()));
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
    QSharedPointer<ParameterValidator> parameterValidator (
        new ParameterValidator(parser, QSharedPointer<QList<QSharedPointer<Choice> > > ()));
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
    QSharedPointer<ParameterValidator> parameterValidator (
        new ParameterValidator(parser, QSharedPointer<QList<QSharedPointer<Choice> > > ()));
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
    QSharedPointer<ParameterValidator> parameterValidator (
        new ParameterValidator(parser, QSharedPointer<QList<QSharedPointer<Choice> > > ()));
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
    QSharedPointer<ParameterValidator> parameterValidator (
        new ParameterValidator(parser, QSharedPointer<QList<QSharedPointer<Choice> > > ()));
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
    QSharedPointer<ParameterValidator> parameterValidator (
        new ParameterValidator(parser, QSharedPointer<QList<QSharedPointer<Choice> > > ()));
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
// Function: tst_FieldValidator::testAccessPolicyModeRefs()
//-----------------------------------------------------------------------------
void tst_FieldValidator::testAccessPolicyModeRefs()
{
    QSharedPointer<Field> testField(new Field());
    testField->setName("testField");
    testField->setBitOffset("8");
    testField->setBitWidth("8");

    QSharedPointer<ModeReference> modeRef1(new ModeReference());
    modeRef1->setPriority("0");
    modeRef1->setReference("ref");
    QSharedPointer<ModeReference> modeRef2(new ModeReference());
    modeRef2->setPriority("0");
    modeRef2->setReference("ref1");

    QSharedPointer<FieldAccessPolicy> testFieldAccessPolicy(new FieldAccessPolicy());
    QSharedPointer<FieldAccessPolicy> testFieldAccessPolicy2(new FieldAccessPolicy());
    testFieldAccessPolicy->getModeReferences()->append(modeRef1);
    testFieldAccessPolicy2->getModeReferences()->append(modeRef2);

    testField->getFieldAccessPolicies()->append(testFieldAccessPolicy);
    testField->getFieldAccessPolicies()->append(testFieldAccessPolicy2);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<EnumeratedValueValidator> enumeratedValueValidator(new EnumeratedValueValidator(parser));
    QSharedPointer<ParameterValidator> parameterValidator(
        new ParameterValidator(parser, QSharedPointer<QList<QSharedPointer<Choice> > >()));
    FieldValidator validator(parser, enumeratedValueValidator, parameterValidator, Document::Revision::Std22);

    QStringList errors;
    
    // Test duplicate priority.
    validator.findErrorsIn(errors, testField, "test");
    QCOMPARE(errors.size(), 1);
    QVERIFY(errors.first().contains("duplicate priority"));
    QVERIFY(validator.hasValidAccessPolicyModeRefs(testField) == false);

    // Test duplicate reference.
    errors.clear();

    modeRef2->setPriority("1");
    modeRef2->setReference("ref");

    validator.findErrorsIn(errors, testField, "test");
    QCOMPARE(errors.size(), 1);
    QVERIFY(errors.first().contains("duplicate mode reference"));
    QVERIFY(validator.hasValidAccessPolicyModeRefs(testField) == false);

    // Test valid.
    errors.clear();

    modeRef2->setReference("ref2");

    validator.findErrorsIn(errors, testField, "test");
    QCOMPARE(errors.size(), 0);
    QVERIFY(validator.hasValidAccessPolicyModeRefs(testField));
}

//-----------------------------------------------------------------------------
// Function: tst_FieldValidator::testStructureValidity()
//-----------------------------------------------------------------------------
void tst_FieldValidator::testStructureValidity()
{
    QSharedPointer<Field> testField(new Field());
    testField->setName("testField");
    testField->setBitOffset("8");

    QSharedPointer<FieldAccessPolicy> testFieldAccessPolicy(new FieldAccessPolicy());
    testFieldAccessPolicy->setAccess(AccessTypes::READ_ONLY);

    QSharedPointer<FieldReference> testFieldReference(new FieldReference());
    QSharedPointer<FieldReference::IndexedReference> fieldRef(new FieldReference::IndexedReference(QString("fieldref"), QStringList()));
    testFieldReference->setReference(fieldRef, FieldReference::FIELD);

    QSharedPointer<FieldReset> testReset(new FieldReset());
    testReset->setResetValue("'b11");

    QSharedPointer<EnumeratedValue> testEnumeratedValue(new EnumeratedValue("testEnumeratedValue", "1"));

    // Test with no field definition reference, no bit width or field reference.
    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<EnumeratedValueValidator> enumeratedValueValidator(new EnumeratedValueValidator(parser));
    QSharedPointer<ParameterValidator> parameterValidator(
        new ParameterValidator(parser, QSharedPointer<QList<QSharedPointer<Choice> > >()));
    FieldValidator validator(parser, enumeratedValueValidator, parameterValidator, Document::Revision::Std22);

    QStringList errors;

    validator.findErrorsIn(errors, testField, "test");
    QVERIFY(validator.hasValidStructure(testField) == false);

    errors.clear();

    // Should be valid with bit width defined
    testField->setBitWidth("8");

    validator.findErrorsIn(errors, testField, "test");
    QCOMPARE(errors.size(), 0);
    QVERIFY(validator.hasValidStructure(testField));
    errors.clear();

    // Invalid, when both field reference and bit width is defined
    testField->setFieldReference(testFieldReference);

    validator.findErrorsIn(errors, testField, "test");
    QCOMPARE(errors.size(), 1);
    QVERIFY(validator.hasValidStructure(testField) == false);
    errors.clear();
    testField->setBitWidth("");

    // Invalid, when reference and volatile is defined.
    testField->setVolatile(true);

    validator.findErrorsIn(errors, testField, "test");
    QCOMPARE(errors.size(), 1);
    QVERIFY(validator.hasValidStructure(testField) == false);
    errors.clear();
    testField->clearVolatile();

    // Invalid, when reference and resets are defined.
    testField->getResets()->append(testReset);

    validator.findErrorsIn(errors, testField, "test");
    QCOMPARE(errors.size(), 1);
    QVERIFY(validator.hasValidStructure(testField) == false);
    errors.clear();
    testField->getResets()->clear();

    // Valid, when just field reference is defined.
    validator.findErrorsIn(errors, testField, "test");
    QCOMPARE(errors.size(), 0);
    QVERIFY(validator.hasValidStructure(testField));
    errors.clear();
    testField->setFieldReference(nullptr);

    // Valid, when just definition ref is defined.
    testField->setFieldDefinitionRef("fieldDefinition");
    testField->setTypeDefinitionsRef("testTypeDefinitions");
    validator.findErrorsIn(errors, testField, "test");
    QCOMPARE(errors.size(), 0);
    QVERIFY(validator.hasValidStructure(testField));
    errors.clear();

    // Invalid, when both definition reference and type identifier are defined
    testField->setTypeIdentifier("testIdentifier");

    validator.findErrorsIn(errors, testField, "test");
    QCOMPARE(errors.size(), 1);
    QVERIFY(validator.hasValidStructure(testField) == false);
    errors.clear();
    testField->setTypeIdentifier("");

    // Invalid, when the field has a definition reference and field access policies defined.
    testField->getFieldAccessPolicies()->append(testFieldAccessPolicy);

    validator.findErrorsIn(errors, testField, "test");
    QCOMPARE(errors.size(), 1);
    QVERIFY(validator.hasValidStructure(testField) == false);
    errors.clear();
    testField->getFieldAccessPolicies()->clear();

    // Invalid, when the field has a definition reference and enumerated values defined.
    testField->getEnumeratedValues()->append(testEnumeratedValue);

    validator.findErrorsIn(errors, testField, "test");
    QCOMPARE(errors.size(), 1);
    QVERIFY(validator.hasValidStructure(testField) == false);
    errors.clear();
    testField->getEnumeratedValues()->clear();

    // Invalid, when the field has a definition reference and bit width.
    testField->setBitWidth("8");

    validator.findErrorsIn(errors, testField, "test");
    QCOMPARE(errors.size(), 1);
    QVERIFY(validator.hasValidStructure(testField) == false);
    errors.clear();
    testField->setBitWidth("");

    // Invalid, when the field has a definition reference and field reference.
    testField->setFieldReference(testFieldReference);

    validator.findErrorsIn(errors, testField, "test");
    QCOMPARE(errors.size(), 1);
    QVERIFY(validator.hasValidStructure(testField) == false);
    errors.clear();
    testField->setFieldDefinitionRef("");
    testField->setTypeDefinitionsRef("");

    // Valid configuration.
    testField->getEnumeratedValues()->append(testEnumeratedValue);
    testField->getFieldAccessPolicies()->append(testFieldAccessPolicy);

    validator.findErrorsIn(errors, testField, "test");
    QCOMPARE(errors.size(), 0);
    QVERIFY(validator.hasValidStructure(testField));
}

//-----------------------------------------------------------------------------
// Function: tst_FieldValidator::errorIsNotFoundInErrorList()
//-----------------------------------------------------------------------------
bool tst_FieldValidator::errorIsNotFoundInErrorList(QString const& expectedError, QVector<QString> errorList)
{
    if (!errorList.contains(expectedError))
    {
        qDebug() << "The following error:" << Qt::endl << expectedError << Qt::endl << "was not found in errorlist:";
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
