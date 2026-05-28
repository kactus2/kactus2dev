//-----------------------------------------------------------------------------
// File: tst_RegisterFileValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 19.11.2015
//
// Description:
// Unit test for class RegisterValidator.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/validators/RegisterFileValidator.h>

#include <IPXACTmodels/Component/validators/EnumeratedValueValidator.h>
#include <IPXACTmodels/common/validators/ParameterValidator.h>
#include <IPXACTmodels/Component/validators/FieldValidator.h>
#include <IPXACTmodels/Component/validators/RegisterValidator.h>

#include <IPXACTmodels/Component/RegisterFile.h>
#include <IPXACTmodels/Component/Register.h>
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

class tst_RegisterFileValidator : public QObject
{
    Q_OBJECT

public:
    tst_RegisterFileValidator();

private slots:
    
    void testNameIsValid();
    void testNameIsValid_data();

    void testIsPresentIsValid();
    void testIsPresentIsValid_data();

    void testDimensionIsValid();
    void testDimensionIsValid_data();

    void testAddressOffsetIsValid();
    void testAddressOffsetIsValid_data();

    void testRangeIsValid();
    void testRangeIsValid_data();

    void testMemoryArrayIsValid();
    void testMemoryArrayIsValid_data();

    void testAccessPoliciesAreValid();

    void testRegisterDataIsValid();

    void testRegisterDataUniqueNames();
    void testRegisterDataUniqueNames_data();

    void testBasicOutOfRange();

    void testOverlappingRegisterData();
    void testTightlyPackedRegisters();
    void testChildRegisterStride();

private:

    bool errorIsNotFoundInErrorList(QString const& expectedError, QStringList const& errorList);
};

//-----------------------------------------------------------------------------
// Function: tst_RegisterFileValidator::tst_RegisterFileValidator()
//-----------------------------------------------------------------------------
tst_RegisterFileValidator::tst_RegisterFileValidator()
{

}

//-----------------------------------------------------------------------------
// Function: tst_RegisterFileValidator::testNameIsValid()
//-----------------------------------------------------------------------------
void tst_RegisterFileValidator::testNameIsValid()
{
    QFETCH(QString, name);
    QFETCH(bool, isValid);

    QSharedPointer<RegisterFile> testRegisterFile(new RegisterFile(name));

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator> parameterValidator(new ParameterValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > >(), Document::Revision::Std14));
    QSharedPointer<EnumeratedValueValidator> enumValidator(new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator(new FieldValidator(parser, enumValidator, parameterValidator));
    QSharedPointer<RegisterValidator> registerValidator(new RegisterValidator(parser, fieldValidator, parameterValidator));

    RegisterFileValidator validator(parser, registerValidator, parameterValidator, Document::Revision::Std14);

    QCOMPARE(validator.hasValidName(testRegisterFile), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testRegisterFile, "test", "8", "32");

        QString expectedError = QObject::tr("Invalid name specified for register file '%1' within %2").
            arg(testRegisterFile->name(), "test");
        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterFileValidator::testNameIsValid_data()
//-----------------------------------------------------------------------------
void tst_RegisterFileValidator::testNameIsValid_data()
{
    QTest::addColumn<QString>("name");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Name test is valid") << "test" << true;
    QTest::newRow("Empty name is invalid") << "" << false;
    QTest::newRow("Name consisting of only white spaces is invalid") << "    " << false;
    QTest::newRow("Name consisting of characters and white spaces is valid") << "  test  " << true;
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterFileValidator::testIsPresentIsValid()
//-----------------------------------------------------------------------------
void tst_RegisterFileValidator::testIsPresentIsValid()
{
    QFETCH(QString, isPresent);
    QFETCH(bool, isValid);

    QSharedPointer<RegisterFile> testRegisterFile(new RegisterFile("name"));
    testRegisterFile->setIsPresent(isPresent);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator> parameterValidator(new ParameterValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > >(), Document::Revision::Std14));
    QSharedPointer<EnumeratedValueValidator> enumValidator(new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator(new FieldValidator(parser, enumValidator, parameterValidator));
    QSharedPointer<RegisterValidator> registerValidator(new RegisterValidator(parser, fieldValidator, parameterValidator));

    RegisterFileValidator validator(parser, registerValidator, parameterValidator, Document::Revision::Std14);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testRegisterFile, "test", "8", "32");

        QString expectedError = QObject::tr("Invalid isPresent set for register file '%1' within %2").
            arg(testRegisterFile->name(), "test");
        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterFileValidator::testIsPresentIsValid_data()
//-----------------------------------------------------------------------------
void tst_RegisterFileValidator::testIsPresentIsValid_data()
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
// Function: tst_RegisterFileValidator::testDimensionIsValid()
//-----------------------------------------------------------------------------
void tst_RegisterFileValidator::testDimensionIsValid()
{
    QFETCH(QString, dimension);
    QFETCH(bool, isValid);

    QSharedPointer<RegisterFile> testRegisterFile(new RegisterFile("name"));
    testRegisterFile->setDimension(dimension);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator> parameterValidator(new ParameterValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > >(), Document::Revision::Std14));
    QSharedPointer<EnumeratedValueValidator> enumValidator(new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator(new FieldValidator(parser, enumValidator, parameterValidator));
    QSharedPointer<RegisterValidator> registerValidator(new RegisterValidator(parser, fieldValidator, parameterValidator));

    RegisterFileValidator validator(parser, registerValidator, parameterValidator, Document::Revision::Std14);

    QCOMPARE(validator.hasValidDimensions(testRegisterFile), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testRegisterFile, "test", "8", "32");

        QString expectedError = QObject::tr("Invalid dimensions set for register file '%1' within %2").
            arg(testRegisterFile->name(), "test");

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterFileValidator::testDimensionIsValid_data()
//-----------------------------------------------------------------------------
void tst_RegisterFileValidator::testDimensionIsValid_data()
{
    QTest::addColumn<QString>("dimension");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Dimension 3 is valid") << "3" << true;
    QTest::newRow("Dimension -8 is invalid") << "-8" << false;
    QTest::newRow("Dimension 4*4/2-8 is invalid") << "0" << false;
    QTest::newRow("Text value is invalid for dimension") << "text" << false;
    QTest::newRow("String value is invalid for dimension") << "\"text\"" << false;

    QTest::newRow("Long dimension is valid") << "40000000000" << true;
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterFileValidator::testAddressOffsetIsValid()
//-----------------------------------------------------------------------------
void tst_RegisterFileValidator::testAddressOffsetIsValid()
{
    QFETCH(QString, addressOffset);
    QFETCH(bool, isValid);

    QSharedPointer<RegisterFile> testRegisterFile(new RegisterFile("name", addressOffset));

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator> parameterValidator(new ParameterValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > >(), Document::Revision::Std22));
    QSharedPointer<EnumeratedValueValidator> enumValidator(new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator(new FieldValidator(parser, enumValidator, parameterValidator));
    QSharedPointer<RegisterValidator> registerValidator(new RegisterValidator(parser, fieldValidator, parameterValidator));

    RegisterFileValidator validator(parser, registerValidator, parameterValidator, Document::Revision::Std22);

    QCOMPARE(validator.hasValidAddressOffset(testRegisterFile), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testRegisterFile, "test", "8", "32");

        QString expectedError = QObject::tr("Invalid address offset set for register file '%1' within %2").
            arg(testRegisterFile->name()).arg("test");

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterFileValidator::testAddressOffsetIsValid_data()
//-----------------------------------------------------------------------------
void tst_RegisterFileValidator::testAddressOffsetIsValid_data()
{
    QTest::addColumn<QString>("addressOffset");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Address offset of 10 is valid") << "10" << true;
    QTest::newRow("Empty address offset is invalid") << "" << false;
    QTest::newRow("Address offset of -2 is invalid") << "-2" << false;
    QTest::newRow("Address offset of 20*80 is valid") << "20*80" << true;
    QTest::newRow("Text is invalid for address offset") << "text" << false;
    QTest::newRow("String is invalid for address offset") << "\"text\"" << false;

    QTest::newRow("Long address offset is valid") << "40000000000" << true;
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterFileValidator::testRangeIsValid()
//-----------------------------------------------------------------------------
void tst_RegisterFileValidator::testRangeIsValid()
{
    QFETCH(QString, range);
    QFETCH(bool, isValid);

    QSharedPointer<RegisterFile> testRegisterFile(new RegisterFile("name", "0", range));

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator> parameterValidator(new ParameterValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > >(), Document::Revision::Std22));
    QSharedPointer<EnumeratedValueValidator> enumValidator(new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator(new FieldValidator(parser, enumValidator, parameterValidator));
    QSharedPointer<RegisterValidator> registerValidator(new RegisterValidator(parser, fieldValidator, parameterValidator));

    RegisterFileValidator validator(parser, registerValidator, parameterValidator, Document::Revision::Std22);

    QCOMPARE(validator.hasValidRange(testRegisterFile), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testRegisterFile, "test", "8", "32");

        QString expectedError = QObject::tr("Invalid range specified for register file %1 within %2").
            arg(testRegisterFile->name()).arg("test");

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterFileValidator::testRangeIsValid_data()
//-----------------------------------------------------------------------------
void tst_RegisterFileValidator::testRangeIsValid_data()
{
    QTest::addColumn<QString>("range");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Range of 10 is valid") << "10" << true;
    QTest::newRow("Empty range is invalid") << "" << false;
    QTest::newRow("Range of -2 is invalid") << "-2" << false;
    QTest::newRow("of 20*80 is valid") << "20*80" << true;
    QTest::newRow("Text is invalid for address offset") << "text" << false;
    QTest::newRow("String is invalid for address offset") << "\"text\"" << false;
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterFileValidator::testMemoryArrayIsValid()
//-----------------------------------------------------------------------------
void tst_RegisterFileValidator::testMemoryArrayIsValid()
{
    QFETCH(QString, dimValue);
    QFETCH(QString, indexVar);
    QFETCH(QString, stride);
    QFETCH(bool, validDim);
    QFETCH(bool, validStride);

    QSharedPointer<RegisterFile> testRegisterFile(new RegisterFile("test", "8", "8"));

    QSharedPointer<MemoryArray> testMemArray(new MemoryArray());

    QSharedPointer<MemoryArray::Dimension> testDim(new MemoryArray::Dimension);
    testDim->value_ = dimValue;
    testDim->indexVar_ = indexVar;

    if (dimValue.isEmpty() == false)
    {
        testMemArray->getDimensions()->append(testDim);
    }

    testMemArray->setStride(stride);

    testRegisterFile->setMemoryArray(testMemArray);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator> parameterValidator(new ParameterValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > >(), Document::Revision::Std22));
    QSharedPointer<EnumeratedValueValidator> enumValidator(new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator(new FieldValidator(parser, enumValidator, parameterValidator));
    QSharedPointer<RegisterValidator> registerValidator(new RegisterValidator(parser, fieldValidator, parameterValidator));

    RegisterFileValidator validator(parser, registerValidator, parameterValidator, Document::Revision::Std22);

    QCOMPARE(validator.hasValidMemoryArray(testRegisterFile), validDim && validStride);

    if (!(validDim && validStride))
    {
        QStringList foundErrors;
        validator.findErrorsIn(foundErrors, testRegisterFile, "test", "8", "32");

        QString expectedError;

        if (testMemArray->getDimensions()->isEmpty())
        {
            expectedError = QObject::tr("No dimensions defined for memory array in register file '%1' within %2")
                .arg(testRegisterFile->name()).arg("test");
        }

        else if (!validDim)
        {
            expectedError = QObject::tr("One or more dimensions of the memory array in register file '%1' within %2 have an invalid value.")
                .arg(testRegisterFile->name()).arg("test");
        }

        else if (!validStride)
        {
            expectedError = QObject::tr("Memory array in register file '%1' within %2 has an invalid stride value.")
                .arg(testRegisterFile->name()).arg("test");
        }

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterFileValidator::testMemoryArrayIsValid_data()
//-----------------------------------------------------------------------------
void tst_RegisterFileValidator::testMemoryArrayIsValid_data()
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
// Function: tst_RegisterFileValidator::testAccessPoliciesAreValid()
//-----------------------------------------------------------------------------
void tst_RegisterFileValidator::testAccessPoliciesAreValid()
{
    QSharedPointer<RegisterFile> testRegisterFile(new RegisterFile("test", "8", "8"));

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

    testRegisterFile->getAccessPolicies()->append(accessPolicy1);
    testRegisterFile->getAccessPolicies()->append(accessPolicy2);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator> parameterValidator(new ParameterValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > >(), Document::Revision::Std22));
    QSharedPointer<EnumeratedValueValidator> enumValidator(new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator(new FieldValidator(parser, enumValidator, parameterValidator));
    QSharedPointer<RegisterValidator> registerValidator(new RegisterValidator(parser, fieldValidator, parameterValidator));

    RegisterFileValidator validator(parser, registerValidator, parameterValidator, Document::Revision::Std22);

    QSharedPointer<Component> dummyComponent(new Component(VLNV(VLNV::COMPONENT, "vendor", "libray", "name", "1.0"), Document::Revision::Std22));
    QSharedPointer<Mode> mode1(new Mode("ref"));
    QSharedPointer<Mode> mode2(new Mode("ref1"));
    dummyComponent->getModes()->append(mode1);
    dummyComponent->getModes()->append(mode2);

    validator.componentChange(dummyComponent);

    QStringList errors;

    QStringList possibleErrors(QStringList()
        << "One or more mode references in access policies of register file test within test contain duplicate priority values."
        << "One or more mode references in access policies of register file test within test contain duplicate mode reference values."
        << "In register file test in test, multiple access policies are not allowed if one of them lacks a mode reference."
    );

    // Test duplicate mode reference priority.
    validator.findErrorsIn(errors, testRegisterFile, "test", "8", "32");
    QVERIFY(errors.contains(possibleErrors.first()));
    QVERIFY(validator.hasValidAccessPolicies(testRegisterFile) == false);

    // Test duplicate reference.
    errors.clear();

    modeRef2->setPriority(1);
    modeRef2->setReference("ref");

    validator.findErrorsIn(errors, testRegisterFile, "test", "8", "32");
    QVERIFY(errors.contains(possibleErrors.at(1)));
    QVERIFY(validator.hasValidAccessPolicies(testRegisterFile) == false);

    // Test invalid.
    errors.clear();

    modeRef2->setReference("ref2");

    validator.findErrorsIn(errors, testRegisterFile, "test", "8", "32");
    QVERIFY(std::none_of(possibleErrors.cbegin(), possibleErrors.cend(), [&errors](QString const& str)
        {
            return errors.contains(str);
        }));

    QVERIFY(validator.hasValidAccessPolicies(testRegisterFile) == false);

    // Test valid.
    errors.clear();

    modeRef2->setReference("ref1");

    validator.findErrorsIn(errors, testRegisterFile, "test", "8", "32");
    QVERIFY(std::none_of(possibleErrors.cbegin(), possibleErrors.cend(), [&errors](QString const& str)
        {
            return errors.contains(str);
        }));

    QVERIFY(validator.hasValidAccessPolicies(testRegisterFile));
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterFileValidator::testRegisterDataIsValid()
//-----------------------------------------------------------------------------
void tst_RegisterFileValidator::testRegisterDataIsValid()
{
    QSharedPointer<RegisterFile> parentRegisterFile(new RegisterFile("parent", "0", "32"));

    QSharedPointer<Field> childField(new Field("childField"));
    childField->setBitOffset("4");
    childField->setBitWidth("4");

    QSharedPointer<Register> childRegister(new Register("childRegister", "8", "8"));

    parentRegisterFile->getRegisterData()->append(childRegister);

    // Test no field error in child register.
    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator> parameterValidator(new ParameterValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > >(), Document::Revision::Std22));
    QSharedPointer<EnumeratedValueValidator> enumValidator(new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator(new FieldValidator(parser, enumValidator, parameterValidator));
    QSharedPointer<RegisterValidator> registerValidator(new RegisterValidator(parser, fieldValidator, parameterValidator));

    RegisterFileValidator validator(parser, registerValidator, parameterValidator, Document::Revision::Std22);

    QStringList errors;

    validator.findErrorsIn(errors, parentRegisterFile, "test", "8", "32");
    QVERIFY(errors.contains(QString("Register childRegister must contain at least one field")));
    QVERIFY(validator.hasValidRegisterData(parentRegisterFile, "8", "32") == false);

    // Test erroneous child register file.
    QSharedPointer<RegisterFile> childRegisterFile(new RegisterFile("name with whitespace", "0", "8"));
    parentRegisterFile->getRegisterData()->clear();

    parentRegisterFile->getRegisterData()->append(childRegisterFile);

    errors.clear();
    validator.findErrorsIn(errors, parentRegisterFile, "test", "8", "32");
    QVERIFY(errors.contains(QString("Invalid name specified for register file 'name with whitespace' within register file 'parent' within test")));
    QVERIFY(validator.hasValidRegisterData(parentRegisterFile, "8", "32") == false);

    // Test valid child register and register file.
    errors.clear();
    childRegister->getFields()->append(childField);
    childRegisterFile->setName("validName");

    parentRegisterFile->getRegisterData()->append(childRegister);

    validator.findErrorsIn(errors, parentRegisterFile, "test", "8", "32");
    QCOMPARE(errors.size(), 0);
    QVERIFY(validator.hasValidRegisterData(parentRegisterFile, "8", "32"));
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterFileValidator::testRegisterDataUniqueNames()
//-----------------------------------------------------------------------------
void tst_RegisterFileValidator::testRegisterDataUniqueNames()
{
    // Register and register file names must be unique in an address block or register file.
    QFETCH(QString, registerOneName);
    QFETCH(QString, registerTwoName);
    QFETCH(QString, registerFileOneName);
    QFETCH(QString, registerFileTwoName);
    QFETCH(bool, isValid);

    QString addressUnitBits("8");

    // Test no field error in child register.
    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator> parameterValidator(new ParameterValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > >(), Document::Revision::Std22));
    QSharedPointer<EnumeratedValueValidator> enumValidator(new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator(new FieldValidator(parser, enumValidator, parameterValidator));
    QSharedPointer<RegisterValidator> registerValidator(new RegisterValidator(parser, fieldValidator, parameterValidator));

    RegisterFileValidator validator(parser, registerValidator, parameterValidator, Document::Revision::Std22);

    QSharedPointer<Field> testfield(new Field("testField"));
    testfield->setBitOffset("0");
    testfield->setBitWidth("8");

    QSharedPointer<Register> registerOne(new Register(registerOneName, "0", "8"));
    QSharedPointer<Register> registerTwo(new Register(registerTwoName, "1", "8"));

    registerOne->getFields()->append(testfield);
    registerTwo->getFields()->append(testfield);

    QSharedPointer<RegisterFile> registerFileOne(new RegisterFile(registerFileOneName, "2", "1"));
    QSharedPointer<RegisterFile> registerFileTwo(new RegisterFile(registerFileTwoName, "3", "1"));

    QSharedPointer<RegisterFile> parentRegisterFile(new RegisterFile("parent", "0", "50"));

    parentRegisterFile->getRegisterData()->append(registerOne);
    parentRegisterFile->getRegisterData()->append(registerTwo);
    parentRegisterFile->getRegisterData()->append(registerFileOne);
    parentRegisterFile->getRegisterData()->append(registerFileTwo);

    QCOMPARE(validator.hasValidRegisterData(parentRegisterFile, "8", "32"), isValid);

    if (!isValid)
    {
        QStringList foundErrors;
        validator.findErrorsIn(foundErrors, parentRegisterFile, "test", addressUnitBits, "32");
        QStringList expectedErrors;

        if (registerOneName == registerTwoName)
        {
            expectedErrors.append(QObject::tr("Name %1 of registers in register file '%2' within test is not unique.")
                .arg(registerOneName).arg(parentRegisterFile->name()));
        }

        if (registerFileOneName == registerFileTwoName)
        {
            expectedErrors.append(QObject::tr("Name %1 of register files in register file '%2' within test is not unique.")
                .arg(registerFileOneName).arg(parentRegisterFile->name()));
        }

        for (auto const& error : expectedErrors)
        {
            if (errorIsNotFoundInErrorList(error, foundErrors))
            {
                QFAIL("No error message found");
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterFileValidator::testRegisterDataUniqueNames_data()
//-----------------------------------------------------------------------------
void tst_RegisterFileValidator::testRegisterDataUniqueNames_data()
{
    QTest::addColumn<QString>("registerOneName");
    QTest::addColumn<QString>("registerTwoName");
    QTest::addColumn<QString>("registerFileOneName");
    QTest::addColumn<QString>("registerFileTwoName");
    QTest::addColumn<bool>("isValid");

    QTest::addRow("Unique register and register file names is valid") << "reg1" << "reg2" << "regFile1" << "regFile2" << true;
    QTest::addRow("Non-unique register and unique register file names is invalid") << "reg" << "reg" << "regFile1" << "regFile2" << false;
    QTest::addRow("Unique register and non-unique register file names is invalid") << "reg1" << "reg2" << "regFile" << "regFile" << false;
    QTest::addRow("Non-unique register and non-unique register file names is invalid") << "reg" << "reg" << "regFile" << "regFile" << false;

}

//-----------------------------------------------------------------------------
// Function: tst_RegisterFileValidator::testBasicOutOfRange()
//-----------------------------------------------------------------------------
void tst_RegisterFileValidator::testBasicOutOfRange()
{
    const QString addressUnitBits("8");

    QSharedPointer<RegisterFile> nestedFile(new RegisterFile("NestedFile"));
    nestedFile->setAddressOffset("0");
    nestedFile->setRange("2");

    QSharedPointer<Field> testField(new Field("testField"));
    testField->setBitOffset("0");
    testField->setBitWidth("8");

    QSharedPointer<Register> registerA(new Register("RegisterA"));
    registerA->setAddressOffset("5");
    registerA->setSize("32");
    registerA->getFields()->append(testField);
    // should be out of range

    // top-level register file
    QSharedPointer<RegisterFile> topLevelFile(new RegisterFile("TopLevelFile"));
    topLevelFile->setAddressOffset("0");
    topLevelFile->setRange("8");
    topLevelFile->getRegisterData()->append(nestedFile);
    topLevelFile->getRegisterData()->append(registerA);

    // Set up validators
    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator> parameterValidator(new ParameterValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice>>>(), Document::Revision::Std14));
    QSharedPointer<EnumeratedValueValidator> enumValidator(new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator(new FieldValidator(parser, enumValidator, parameterValidator));
    QSharedPointer<RegisterValidator> registerValidator(new RegisterValidator(parser, fieldValidator, parameterValidator));

    RegisterFileValidator validator(parser, registerValidator, parameterValidator, Document::Revision::Std14);

    QList<QString> foundErrors;

    QCOMPARE(validator.hasValidRegisterData(topLevelFile, addressUnitBits, "32"), false);
    validator.findErrorsIn(foundErrors, topLevelFile, "test", addressUnitBits, "32");

    QString expectedError = tr("Register %1 is not contained within register file '%2' within test")
        .arg(registerA->name()).arg(topLevelFile->name());

    if (errorIsNotFoundInErrorList(expectedError, foundErrors))
    {
        QFAIL("No error message found");
    }
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterFileValidator::testOneDimensionOverlap()
//-----------------------------------------------------------------------------
void tst_RegisterFileValidator::testOverlappingRegisterData()
{
    const QString addressUnitBits = "8"; // 1 address unit = 8 bits

    // Sample register with field
    QSharedPointer<Field> testField(new Field("testField"));
    testField->setBitOffset("0");
    testField->setBitWidth("8");

    QSharedPointer<Register> registerA(new Register("RegisterA"));
    registerA->setAddressOffset("0");
    registerA->setSize("32");
    registerA->getFields()->append(testField);

    // empty nested register file
    QSharedPointer<RegisterFile> nestedFile(new RegisterFile("NestedFile"));
    nestedFile->setAddressOffset("2");
    nestedFile->setRange("2");
    // should overlap with register

    // top-level register file
    QSharedPointer<RegisterFile> topLevelFile(new RegisterFile("TopLevelFile"));
    topLevelFile->setAddressOffset("0");
    topLevelFile->setRange("8"); // Enough space
    topLevelFile->getRegisterData()->append(registerA);
    topLevelFile->getRegisterData()->append(nestedFile);

    // Set up validators
    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator> parameterValidator(new ParameterValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice>>>(), Document::Revision::Std14));
    QSharedPointer<EnumeratedValueValidator> enumValidator(new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator(new FieldValidator(parser, enumValidator, parameterValidator));
    QSharedPointer<RegisterValidator> registerValidator(new RegisterValidator(parser, fieldValidator, parameterValidator));

    RegisterFileValidator validator(parser, registerValidator, parameterValidator, Document::Revision::Std14);

    QCOMPARE(validator.hasValidRegisterData(topLevelFile, addressUnitBits, "32"), false);

    QVector<QString> foundErrors;
    validator.findErrorsIn(foundErrors, topLevelFile, "test", addressUnitBits, "32");

    QString expectedError = tr("Register data %1 and %2 overlap within register file '%3' within test")
        .arg(registerA->name()).arg(nestedFile->name()).arg(topLevelFile->name());

    if (errorIsNotFoundInErrorList(expectedError, foundErrors))
    {
        QFAIL("No error message found");
    }
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterFileValidator::testTightlyPackedRegisters()
//-----------------------------------------------------------------------------
void tst_RegisterFileValidator::testTightlyPackedRegisters()
{
    const QString addressUnitBits = "8";

    // Sample register with field
    QSharedPointer<Field> testField(new Field("testField"));
    testField->setBitOffset("0");
    testField->setBitWidth("8");

    QSharedPointer<Register> registerA(new Register("RegisterA"));
    registerA->setAddressOffset("0");
    registerA->setSize("32");       // 32 bits = 4 address units
    registerA->setDimension("4");   // 4 tightly packed instances
    registerA->getFields()->append(testField);

    // Top-level register file
    QSharedPointer<RegisterFile> topLevelFile(new RegisterFile("TopLevelFile"));
    topLevelFile->setAddressOffset("0");
    topLevelFile->setRange("16"); // 16 address units = 128 bits, enough for 4 * 32-bit registers
    topLevelFile->getRegisterData()->append(registerA);

    // Set up validators
    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator> parameterValidator(new ParameterValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice>>>(), Document::Revision::Std14));
    QSharedPointer<EnumeratedValueValidator> enumValidator(new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator(new FieldValidator(parser, enumValidator, parameterValidator));
    QSharedPointer<RegisterValidator> registerValidator(new RegisterValidator(parser, fieldValidator, parameterValidator));

    RegisterFileValidator validator(parser, registerValidator, parameterValidator, Document::Revision::Std14);

    // This should be valid: tightly packed register array fits within the register file
    QCOMPARE(validator.hasValidRegisterData(topLevelFile, addressUnitBits, "32"), true);

    QVector<QString> foundErrors;
    validator.findErrorsIn(foundErrors, topLevelFile, "test", addressUnitBits, "32");

    if (!foundErrors.isEmpty())
    {
        QFAIL("Unexpected error(s) found for tightly packed register array in register file");
    }
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterFileValidator::testChildRegisterStride()
//-----------------------------------------------------------------------------
void tst_RegisterFileValidator::testChildRegisterStride()
{
    const QString addressUnitBits = "8";

    // Create a register with a size of 32 bits (4 address units)
    QSharedPointer<Field> testField(new Field("testField"));
    testField->setBitOffset("0");
    testField->setBitWidth("8");

    // Valid stride: 4 address units
    QSharedPointer<Register> validStrideRegister(new Register("ValidStrideRegister"));
    validStrideRegister->setAddressOffset("0");
    validStrideRegister->setSize("32");
    validStrideRegister->setDimension("4");
    validStrideRegister->setStride("4");
    validStrideRegister->getFields()->append(testField);

    // Too-small stride: 2 address units (overlap)
    QSharedPointer<Register> overlappingRegister(new Register("OverlappingRegister"));
    overlappingRegister->setAddressOffset("0");
    overlappingRegister->setSize("32");
    overlappingRegister->setDimension("4");
    overlappingRegister->setStride("2"); // Too small: causes overlap
    overlappingRegister->getFields()->append(testField);

    // Register file with enough space for valid stride
    QSharedPointer<RegisterFile> validFile(new RegisterFile("ValidStrideFile"));
    validFile->setAddressOffset("0");
    validFile->setRange("16"); // 4 * 4 = 16 address units
    validFile->getRegisterData()->append(validStrideRegister);

    // Register file with insufficient range
    QSharedPointer<RegisterFile> smallRangeFile(new RegisterFile("SmallRangeFile"));
    smallRangeFile->setAddressOffset("0");
    smallRangeFile->setRange("12"); // Not enough for 4 * 4 stride
    smallRangeFile->getRegisterData()->append(validStrideRegister);

    // Register file with registers with overlapping stride
    QSharedPointer<RegisterFile> overlappingFile(new RegisterFile("OverlappingStrideFile"));
    overlappingFile->setAddressOffset("0");
    overlappingFile->setRange("16"); // Enough range, but stride is too small
    overlappingFile->getRegisterData()->append(overlappingRegister);

    // Set up validators
    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator> parameterValidator(new ParameterValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice>>>(), Document::Revision::Std22));
    QSharedPointer<EnumeratedValueValidator> enumValidator(new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator(new FieldValidator(parser, enumValidator, parameterValidator));
    QSharedPointer<RegisterValidator> registerValidator(new RegisterValidator(parser, fieldValidator, parameterValidator));

    RegisterFileValidator validator(parser, registerValidator, parameterValidator, Document::Revision::Std22);

    // Valid case
    QList<QString> foundErrors;
    QCOMPARE(validator.hasValidRegisterData(validFile, addressUnitBits, "32"), true);
    
    validator.findErrorsIn(foundErrors, validFile, "test", addressUnitBits, "32");
    QVERIFY(foundErrors.isEmpty());

    foundErrors.clear();

    // Small range case
    QCOMPARE(validator.hasValidRegisterData(smallRangeFile, addressUnitBits, "32"), false);
    validator.findErrorsIn(foundErrors, smallRangeFile, "test", addressUnitBits, "32");
    
    // Fourth replica is outside range
    QString expectedError = tr("Register %1 (3) is not contained within register file '%2' within test")
        .arg(validStrideRegister->name()).arg(smallRangeFile->name());

    if (errorIsNotFoundInErrorList(expectedError, foundErrors))
    {
        QFAIL("No error message found");
    }

    foundErrors.clear();
    
    // Overlapping stride case
    QCOMPARE(validator.hasValidRegisterData(overlappingFile, addressUnitBits, "32"), false);
    validator.findErrorsIn(foundErrors, overlappingFile, "test", addressUnitBits, "32");
    
    QVERIFY(foundErrors.size() == 3);

    // Overlapping register replicas are (0,1), (1,2) and (2,3)
    QStringList overlapErrors = {
        tr("Register data %1 (0) and %1 (1) overlap within register file '%2' within test")
            .arg(overlappingRegister->name()).arg(overlappingFile->name()),

        tr("Register data %1 (1) and %1 (2) overlap within register file '%2' within test")
            .arg(overlappingRegister->name()).arg(overlappingFile->name()),

        tr("Register data %1 (2) and %1 (3) overlap within register file '%2' within test")
            .arg(overlappingRegister->name()).arg(overlappingFile->name()),
    };

    for (auto const& error : overlapErrors)
    {
        if (errorIsNotFoundInErrorList(error, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterFileValidator::errorIsNotFoundInErrorList()
//-----------------------------------------------------------------------------
bool tst_RegisterFileValidator::errorIsNotFoundInErrorList(QString const& expectedError, QStringList const& errorList)
{
    if (!errorList.contains(expectedError))
    {
        qDebug() << "The following error:" << Qt::endl << expectedError << Qt::endl << "was not found in error list:";
        for (auto const& error : errorList)
        {
            qDebug() << error;
        }
        return true;
    }

    return false;
}

QTEST_APPLESS_MAIN(tst_RegisterFileValidator)

#include "tst_RegisterFileValidator.moc"
