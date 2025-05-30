//-----------------------------------------------------------------------------
// File: tst_AddressBlockValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: <Name>
// Date: <Date in the format dd.mm.yyyy>
//
// Description:
// Unit test for class AddressBlockValidator.
//-----------------------------------------------------------------------------

#include <KactusAPI/include/SystemVerilogExpressionParser.h>

#include <IPXACTmodels/Component/validators/AddressBlockValidator.h>
#include <IPXACTmodels/Component/validators/RegisterValidator.h>
#include <IPXACTmodels/Component/validators/RegisterFileValidator.h>
#include <IPXACTmodels/Component/validators/FieldValidator.h>
#include <IPXACTmodels/Component/validators/EnumeratedValueValidator.h>
#include <IPXACTmodels/common/validators/ParameterValidator.h>
#include <IPXACTmodels/common/Document.h>

#include <IPXACTmodels/Component/AddressBlock.h>
#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/RegisterFile.h>
#include <IPXACTmodels/Component/Field.h>
#include <IPXACTmodels/Component/MemoryArray.h>
#include <IPXACTmodels/Component/Mode.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/common/Parameter.h>

#include <QtTest>

class tst_AddressBlockValidator : public QObject
{
    Q_OBJECT

public:
    tst_AddressBlockValidator();

private slots:

    void testNameIsValid();
    void testNameIsValid_data();

    void testIsPresentIsValid();
    void testIsPresentIsValid_data();

    void testBaseAddressIsValid();
    void testBaseAddressIsValid_data();

    void testRangeIsValid();
    void testRangeIsValid_data();

    void testWidthIsValid();
    void testWidthIsValid_data();

    void testHasValidUsage();
    void testHasValidUsage_data();

    void testParametersAreValid();

    void testRegisterDataIsValid();
    void testRegisterDataIsValid_data();
    void testAccessIsValidWithRegister();
    void testAccessIsValidWithRegister_data();
    void testRegisterPositioning();
    void testRegisterPositioning_data();
    void testRegisterOverlapping();
    void testRegisterOverlapping_data();
    void testRegisterIsOverlappingTwoOtherRegisters();
    void testRegistersWithStride();

    void testHasValidMemoryarray2022();
    void testHasValidMemoryarray2022_data();
    void testAccessPolicies2022();
    void testRegisterAlignment2022();
    void testRegisterAlignment2022_data();
    void testStructureIsValid2022();

    void testRegisterDataHaveUniqueNames();
    void testRegisterDataHaveUniqueNames_data();

private:
    
    bool errorIsNotFoundInErrorList(QString const& expectedError, QVector<QString> errorList);

    QSharedPointer<AddressBlockValidator> createValidator(Document::Revision revision);
};

//-----------------------------------------------------------------------------
// Function: tst_AddressBlockValidator::tst_AddressBlockValidator()
//-----------------------------------------------------------------------------
tst_AddressBlockValidator::tst_AddressBlockValidator()
{
}

//-----------------------------------------------------------------------------
// Function: tst_AddressBlockValidator::testNameIsValid()
//-----------------------------------------------------------------------------
void tst_AddressBlockValidator::testNameIsValid()
{
    QFETCH(QString, name);
    QFETCH(bool, isValid);

    QSharedPointer<AddressBlock> testBlock (new AddressBlock(name));

    QSharedPointer<AddressBlockValidator> validator = createValidator(Document::Revision::Std14);

//     AddressBlockValidator validator(parser, QSharedPointer<QList<QSharedPointer<Choice> > > ());
    QCOMPARE(validator->hasValidName(testBlock), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testBlock, "", "test");

        QString expectedError = QObject::tr("Invalid name specified for address block %1 within %2").
            arg(testBlock->name(), "test");
        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_AddressBlockValidator::testNameIsValid_data()
//-----------------------------------------------------------------------------
void tst_AddressBlockValidator::testNameIsValid_data()
{
    QTest::addColumn<QString>("name");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Name test is valid") << "test" << true;
    QTest::newRow("Empty name is invalid") << "" << false;
    QTest::newRow("Name consisting of only white spaces is invalid") << "    " << false;
    QTest::newRow("Name consisting of characters and white spaces is valid") << "  test  " << true;
}

//-----------------------------------------------------------------------------
// Function: tst_AddressBlockValidator::testIsPresentIsValid()
//-----------------------------------------------------------------------------
void tst_AddressBlockValidator::testIsPresentIsValid()
{
    QFETCH(QString, isPresent);
    QFETCH(bool, isValid);

    QSharedPointer<AddressBlock> testBlock (new AddressBlock("TestAddressBlock"));
    testBlock->setIsPresent(isPresent);

    QSharedPointer<AddressBlockValidator> validator = createValidator(Document::Revision::Std14);
    QCOMPARE(validator->hasValidIsPresent(testBlock), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testBlock, "", "test");

        QString expectedError = QObject::tr("Invalid isPresent set for address block %1 within %2").
            arg(testBlock->name(), "test");
        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_AddressBlockValidator::testIsPresentIsValid_data()
//-----------------------------------------------------------------------------
void tst_AddressBlockValidator::testIsPresentIsValid_data()
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
// Function: tst_AddressBlockValidator::testBaseAddressIsValid()
//-----------------------------------------------------------------------------
void tst_AddressBlockValidator::testBaseAddressIsValid()
{
    QFETCH(QString, baseAddress);
    QFETCH(bool, isValid);

    QSharedPointer<AddressBlock> testBlock (new AddressBlock("TestAddressBlock"));
    testBlock->setBaseAddress(baseAddress);

    QSharedPointer<AddressBlockValidator> validator = createValidator(Document::Revision::Std14);
    QCOMPARE(validator->hasValidBaseAddress(testBlock), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testBlock, "", "test");

        QString expectedError = QObject::tr("Invalid baseAddress set for address block %1 within %2").
            arg(testBlock->name(), "test");
        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_AddressBlockValidator::testBaseAddressIsValid_data()
//-----------------------------------------------------------------------------
void tst_AddressBlockValidator::testBaseAddressIsValid_data()
{
    QTest::addColumn<QString>("baseAddress");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("BaseAddress 1 is valid") << "1" << true;
    QTest::newRow("BaseAddress 2*100 is valid") << "2*100" << true;
    QTest::newRow("BaseAddress -14 is invalid") << "-14" << false;
    QTest::newRow("Real number baseAddress 0.12 is invalid") << "0.12" << false;
    QTest::newRow("Text as baseAddress is invalid") << "test" << false;
    QTest::newRow("String as baseAddress is invalid") << "\"test\"" << false;

    QTest::newRow("Long base address is valid") << "40000000000" << true;
    //QTest::newRow("Really long base address is invalid") << "40000000000000000000000000000000000000000*2" << false;
}

//-----------------------------------------------------------------------------
// Function: tst_AddressBlockValidator::testRangeIsValid()
//-----------------------------------------------------------------------------
void tst_AddressBlockValidator::testRangeIsValid()
{
    QFETCH(QString, range);
    QFETCH(bool, isValid);

    QSharedPointer<AddressBlock> testBlock (new AddressBlock("TestAddressBlock"));
    testBlock->setRange(range);

    QSharedPointer<AddressBlockValidator> validator = createValidator(Document::Revision::Std14);
    QCOMPARE(validator->hasValidRange(testBlock), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testBlock, "", "test");

        QString expectedError = QObject::tr("Invalid range set for address block %1 within %2").
            arg(testBlock->name(), "test");
        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_AddressBlockValidator::testRangeIsValid_data()
//-----------------------------------------------------------------------------
void tst_AddressBlockValidator::testRangeIsValid_data()
{
    QTest::addColumn<QString>("range");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Range 1 is valid") << "1" << true;
    QTest::newRow("Range 5*5+15 is valid") << "5*5+15" << true;
    QTest::newRow("Range 2*100-200 is invalid") << "2*100-200" << false;
    QTest::newRow("Range -14 is invalid") << "-14" << false;
    QTest::newRow("Real number as range 0.12 is invalid") << "0.12" << false;
    QTest::newRow("Text as range is invalid") << "test" << false;
    QTest::newRow("String as range is invalid") << "\"test\"" << false;

    QTest::newRow("Long range is valid") << "40000000000" << true;
}

//-----------------------------------------------------------------------------
// Function: tst_AddressBlockValidator::testWidthIsValid()
//-----------------------------------------------------------------------------
void tst_AddressBlockValidator::testWidthIsValid()
{
    QFETCH(QString, width);
    QFETCH(bool, isValid);

    QSharedPointer<AddressBlock> testBlock (new AddressBlock("TestAddressBlock"));
    testBlock->setWidth(width);

    QSharedPointer<AddressBlockValidator> validator = createValidator(Document::Revision::Std14);
    QCOMPARE(validator->hasValidWidth(testBlock), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testBlock, "", "test");

        QString expectedError = QObject::tr("Invalid width set for address block %1 within %2").
            arg(testBlock->name()).arg("test");
        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_AddressBlockValidator::testWidthIsValid_data()
//-----------------------------------------------------------------------------
void tst_AddressBlockValidator::testWidthIsValid_data()
{
    QTest::addColumn<QString>("width");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Width 5*5+15 is valid") << "5*5+15" << true;
    QTest::newRow("Width 2*100-200 is valid") << "2*100-200" << true;
    QTest::newRow("Width -14 is invalid") << "-14" << false;
    QTest::newRow("Real number as width 0.12 is invalid") << "0.12" << false;
    QTest::newRow("Text as width is invalid") << "test" << false;
    QTest::newRow("String as width is invalid") << "\"test\"" << false;

    QTest::newRow("Long width is valid") << "40000000000" << true;
}

//-----------------------------------------------------------------------------
// Function: tst_AddressBlockValidator::testHasValidUsage()
//-----------------------------------------------------------------------------
void tst_AddressBlockValidator::testHasValidUsage()
{
    QFETCH(QString, usage);
    QFETCH(int, registerAmount);
    QFETCH(QString, registerVolatile);
    QFETCH(QString, registerAccess);
    QFETCH(bool, isValid);

    QSharedPointer<Field> testField (new Field("testField"));
    testField->setBitOffset("0");
    testField->setBitWidth("4");

    QSharedPointer<AddressBlock> testBlock (new AddressBlock("testAddressBlock"));
    testBlock->setUsage(General::str2Usage(usage, General::USAGE_COUNT));
    testBlock->setWidth("20");

    QSharedPointer<Register> testRegister;
    for (int i = 0; i < registerAmount; ++i)
    {
        QSharedPointer<Register> newRegister (new Register("testRegister " + i, "0", "10"));
        newRegister->getFields()->append(testField);

        if (usage == QLatin1String("memory") && i == 0)
        {
            testRegister = newRegister;
            if (registerVolatile == QLatin1String("true"))
            {
                newRegister->setVolatile(true);
            }
            else if (registerVolatile == QLatin1String("false"))
            {
                newRegister->setVolatile(false);
            }

            newRegister->setAccess(AccessTypes::str2Access(registerAccess, AccessTypes::ACCESS_COUNT));
        }

        testBlock->getRegisterData()->append(newRegister);
    }

    QSharedPointer<AddressBlockValidator> validator = createValidator(Document::Revision::Std14);
    QCOMPARE(validator->hasValidUsage(testBlock), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testBlock, "", "test");

        if (General::str2Usage(usage, General::USAGE_COUNT) == General::RESERVED)
        {
            QString expectedError = QObject::tr("Registers cannot be contained in address block %1 with usage "
                "%2 within %3").arg(testBlock->name()).arg(usage).arg("test");
            if (errorIsNotFoundInErrorList(expectedError, foundErrors))
            {
                QFAIL("No error message found");
            }
        }
        else if (General::str2Usage(usage, General::USAGE_COUNT) == General::MEMORY)
        {
            QString expectedError = QObject::tr("Access and volatile values must be empty for register %1 in "
                "address block %2 with usage %3 within %4")
                .arg(testRegister->name()).arg(testBlock->name()).arg(usage).arg("test");
            if (errorIsNotFoundInErrorList(expectedError, foundErrors))
            {
                QFAIL("No error message found");
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_AddressBlockValidator::testHasValidUsage_data()
//-----------------------------------------------------------------------------
void tst_AddressBlockValidator::testHasValidUsage_data()
{
    QTest::addColumn<QString>("usage");
    QTest::addColumn<int>("registerAmount");
    QTest::addColumn<QString>("registerVolatile");
    QTest::addColumn<QString>("registerAccess");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Reserved usage is valid without registers") << "reserved" << 0 << "" << "" << true;
    QTest::newRow("Reserved usage is invalid with registers") << "reserved" << 3 << "" << "" << false;

    QTest::newRow("Register usage is valid without registers") << "register" << 0 << "" << "" << true;
    QTest::newRow("Register usage is valid with registers") << "register" << 3 << "" << "" << true;

    QTest::newRow("Memory usage is valid without registers") << "memory" << 0 << "" << "" << true;
    QTest::newRow("Memory usage is valid with virtual registers") << "memory" << 3 << "" << "" << true;
    QTest::newRow("Memory usage is invalid with registers containing volatile") << "memory" << 3 << "true" <<
        "" << false;
    QTest::newRow("Memory usage is invalid with registers containing access") << "memory" << 3 << "" <<
        "read-write" << false;
}

//-----------------------------------------------------------------------------
// Function: tst_AddressBlockValidator::testParametersAreValid()
//-----------------------------------------------------------------------------
void tst_AddressBlockValidator::testParametersAreValid()
{
    QSharedPointer<Parameter> testParameter (new Parameter());
    testParameter->setValueId("Sanger");
    testParameter->setName("Daizengar");
    testParameter->setValue("20");
    testParameter->setType("int");

    QSharedPointer<AddressBlock> testBlock (new AddressBlock("TestAddressBlock"));
    testBlock->getParameters()->append(testParameter);

    QSharedPointer<AddressBlockValidator> validator = createValidator(Document::Revision::Std14);
    QCOMPARE(validator->hasValidParameters(testBlock), true);

    testParameter->setValue("");
    QCOMPARE(validator->hasValidParameters(testBlock), false);

    QVector<QString> errorsFound;
    validator->findErrorsIn(errorsFound, testBlock, "", "test");

    QString expectedError = QObject::tr("No value specified for %1 %2 within address block %3").
        arg(testParameter->elementName()).arg(testParameter->name()).arg(testBlock->name());

    if (errorIsNotFoundInErrorList(expectedError, errorsFound))
    {
        QFAIL("Error was not found");
    }

    QSharedPointer<Parameter> otherParameter (new Parameter(*testParameter.data()));
    testParameter->setValue("1");
    otherParameter->setValue("2");
    testBlock->getParameters()->append(otherParameter);

    QCOMPARE(validator->hasValidParameters(testBlock), false);

    errorsFound.clear();
    validator->findErrorsIn(errorsFound, testBlock, "", "test");
    expectedError = QObject::tr("Name %1 of parameters in address block %2 is not unique.").arg(otherParameter->name()).
        arg(testBlock->name());
    if (errorIsNotFoundInErrorList(expectedError, errorsFound))
    {
        QFAIL("Error was not found");
    }
}

//-----------------------------------------------------------------------------
// Function: tst_AddressBlockValidator::testRegisterDataIsValid()
//-----------------------------------------------------------------------------
void tst_AddressBlockValidator::testRegisterDataIsValid()
{
    QFETCH(QString, registerName1);
    QFETCH(QString, registerOffset1);
    QFETCH(QString, registerSize1);
    QFETCH(QString, typeIdentifier1);
    QFETCH(bool, registerVolatile);

    QFETCH(QString, registerName2);
    QFETCH(QString, registerOffset2);
    QFETCH(QString, registerSize2);
    QFETCH(QString, typeIdentifier2);

    QFETCH(bool, blockVolatile);
    QFETCH(bool, isValid);

    QSharedPointer<Field> testFieldOne (new Field("fieldOne"));
    testFieldOne->setBitOffset("0");
    testFieldOne->setBitWidth("2");
    testFieldOne->setVolatile(registerVolatile);

    QSharedPointer<Field> testFieldTwo (new Field("fieldTwo"));
    testFieldTwo->setBitOffset("0");
    testFieldTwo->setBitWidth("4");
    testFieldTwo->setVolatile(registerVolatile);

    QSharedPointer<Register> registerOne (new Register(registerName1, registerOffset1, registerSize1));
    registerOne->setVolatile(registerVolatile);
    registerOne->setTypeIdentifier(typeIdentifier1);
    registerOne->getFields()->append(testFieldOne);

    QSharedPointer<Register> registerTwo (new Register(registerName2, registerOffset2, registerSize2));
    registerTwo->setVolatile(registerVolatile);
    registerTwo->setTypeIdentifier(typeIdentifier2);
    registerTwo->getFields()->append(testFieldTwo);

    QSharedPointer<AddressBlock> testBlock (new AddressBlock("testAddressBlock"));
    testBlock->setWidth("20");
    testBlock->setVolatile(blockVolatile);
    testBlock->getRegisterData()->append(registerOne);
    testBlock->getRegisterData()->append(registerTwo);

    QSharedPointer<AddressBlockValidator> validator = createValidator(Document::Revision::Std14);
    QCOMPARE(validator->hasValidRegisterData(testBlock, "8"), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testBlock, "8", "test");

        if (registerOffset1.isEmpty())
        {
            QString expectedError = QObject::tr("Invalid address offset set for register '%1' within %2").
                arg(registerName1).arg("address block " + testBlock->name());

            if (errorIsNotFoundInErrorList(expectedError, foundErrors))
            {
                QFAIL("No error message found");
            }
        }

        if (registerName1 == registerName2)
        {
            QString expectedError = QObject::tr("Name %1 of registers in addressBlock %2 is not unique.")
                .arg(registerName1).arg(testBlock->name());

            if (errorIsNotFoundInErrorList(expectedError, foundErrors))
            {
                QFAIL("No error message found");
            }
        }

        if (validator->registerSizeIsNotWithinBlockWidth(registerOne, testBlock))
        {
            QString expectedError = QObject::tr("Register %1 size must not be greater than the containing "
                "addressBlock %2 width.").arg(registerOne->name()).arg(testBlock->name());

            if (errorIsNotFoundInErrorList(expectedError, foundErrors))
            {
                QFAIL("No error message found");
            }
        }

        if (!validator->hasValidVolatileForRegister(testBlock, registerOne))
        {
            QString expectedError = QObject::tr("Volatile value cannot be set to false for addressBlock %1 "
                "containing a register or register field with volatile true").arg(testBlock->name());

            if (errorIsNotFoundInErrorList(expectedError, foundErrors))
            {
                QFAIL("No error message found");
            }
        }

        if (!typeIdentifier1.isEmpty() && typeIdentifier1 == typeIdentifier2)
        {
            QString expectedError = QObject::tr("Registers containing the same type identifiers must contain "
                "similar register definitions within address block %1").arg(testBlock->name());

            if (errorIsNotFoundInErrorList(expectedError, foundErrors))
            {
                QFAIL("No error message found");
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_AddressBlockValidator::testRegisterDataIsValid_data()
//-----------------------------------------------------------------------------
void tst_AddressBlockValidator::testRegisterDataIsValid_data()
{
    QTest::addColumn<QString>("registerName1");
    QTest::addColumn<QString>("registerOffset1");
    QTest::addColumn<QString>("registerSize1");
    QTest::addColumn<QString>("typeIdentifier1");
    QTest::addColumn<bool>("registerVolatile");

    QTest::addColumn<QString>("registerName2");
    QTest::addColumn<QString>("registerOffset2");
    QTest::addColumn<QString>("registerSize2");
    QTest::addColumn<QString>("typeIdentifier2");

    QTest::addColumn<bool>("blockVolatile");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Registers with same type identifier must contain same definition") << "register1" << "0" <<
        "12" << "identifier" << false << "register2" << "12" << "4" << "identifier" << false << false;

    QTest::newRow("Registers with different names are valid") << "register1" << "0" << "8" << "" << false <<
        "register2" << "8" << "8" << "" << false << true;
    QTest::newRow("Register without address offset is invalid") << "register1" << "" << "10" << "" << false <<
        "register2" << "0" << "10" << "" << false << false;
    QTest::newRow("Registers with same names are invalid") << "register" << "0" << "10" << "" << false <<
        "register" << "10" << "10" << "" << false << false;
    QTest::newRow("Register with size > address block width is invalid") << "register" << "0" << "30" << "" <<
        false << "register2" << "0" << "4" << "" << false << false;

    QTest::newRow("Register: volatile = true, address block: volatile = true is valid")  << "register1" << "0" <<
        "8" << "" << true << "register2" << "8" << "8" << "" << true << true;
    QTest::newRow("Register: volatile = true, address block: volatile = false is invalid")  << "register1" <<
        "0" << "10" << "" << true << "register2" << "0" << "10" << "" << false << false;

    QTest::newRow("Registers with same type identifier must contain same definition") << "register1" << "0" <<
        "8" << "identifier" << false << "register2" << "8" << "8" << "identifier" << false << true;
}

//-----------------------------------------------------------------------------
// Function: tst_AddressBlockValidator::testAccessIsValidWithRegister()
//-----------------------------------------------------------------------------
void tst_AddressBlockValidator::testAccessIsValidWithRegister()
{
    QFETCH(QString, blockAccess);
    QFETCH(QString, registerAccess);
    QFETCH(bool, isValid);

    QSharedPointer<Field> testField (new Field("testField"));
    testField->setBitOffset("0");
    testField->setBitWidth("4");
    testField->setAccess(AccessTypes::str2Access(registerAccess, AccessTypes::ACCESS_COUNT));

    QSharedPointer<Register> testRegister (new Register("Mugen", "0", "20"));
    testRegister->setAccess(AccessTypes::str2Access(registerAccess, AccessTypes::ACCESS_COUNT));
    testRegister->getFields()->append(testField);

    QSharedPointer<AddressBlock> testBlock (new AddressBlock("block"));
    testBlock->setWidth("100");
    testBlock->setAccess(AccessTypes::str2Access(blockAccess, AccessTypes::ACCESS_COUNT));
    testBlock->getRegisterData()->append(testRegister);

    QSharedPointer<AddressBlockValidator> validator = createValidator(Document::Revision::Std14);
    QCOMPARE(validator->hasValidRegisterData(testBlock, ""), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testBlock, "", "test");

        QString expectedError = QObject::tr("Access cannot be set to %1 in register %2, where containing address "
            "block %3 has access %4").arg(registerAccess).arg(testRegister->name())
            .arg(testBlock->name()).arg(blockAccess);

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_AddressBlockValidator::testAccessIsValidWithRegister_data()
//-----------------------------------------------------------------------------
void tst_AddressBlockValidator::testAccessIsValidWithRegister_data()
{
    QTest::addColumn<QString>("blockAccess");
    QTest::addColumn<QString>("registerAccess");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("AddressBlock: access = read-only, register: access = read-write is invalid") <<
        "read-only" << "read-write" << false;
    QTest::newRow("AddressBlock: access = read-only, register: access = read-only is valid") <<
        "read-only" << "read-only" << true;
    QTest::newRow("AddressBlock: access = read-only, register: access = write-only is invalid") <<
        "read-only" << "write-only" << false;
    QTest::newRow("AddressBlock: access = read-only, register: access = read-writeOnce is invalid") <<
        "read-only" << "read-writeOnce" << false;
    QTest::newRow("AddressBlock: access = read-only, register: access = writeOnce is invalid") <<
        "read-only" << "writeOnce" << false;

    QTest::newRow("AddressBlock: access = write-only, register: access = read-write is invalid") <<
        "write-only" << "read-write" << false;
    QTest::newRow("AddressBlock: access = write-only, register: access = read-only is invalid") <<
        "write-only" << "read-only" << false;
    QTest::newRow("AddressBlock: access = write-only, register: access = write-only is valid") <<
        "write-only" << "write-only" << true;
    QTest::newRow("AddressBlock: access = write-only, register: access = read-writeOnce is invalid") <<
        "write-only" << "read-writeOnce" << false;
    QTest::newRow("AddressBlock: access = write-only, register: access = writeOnce is valid") <<
        "write-only" << "writeOnce" << true;

    QTest::newRow("AddressBlock: access = read-writeOnce , register: access = read-write is invalid") <<
        "read-writeOnce" << "read-write" << false;
    QTest::newRow("AddressBlock: access = read-writeOnce , register: access = read-only is valid") <<
        "read-writeOnce" << "read-only" << true;
    QTest::newRow("AddressBlock: access = read-writeOnce , register: access = write-only is invalid") <<
        "read-writeOnce" << "write-only" << false;
    QTest::newRow("AddressBlock: access = read-writeOnce , register: access = read-writeOnce is valid") <<
        "read-writeOnce" << "read-writeOnce" << true;
    QTest::newRow("AddressBlock: access = read-writeOnce , register: access = writeOnce is valid") <<
        "read-writeOnce" << "writeOnce" << true;

    QTest::newRow("AddressBlock: access = writeOnce , register: access = read-write is invalid") <<
        "writeOnce" << "read-write" << false;
    QTest::newRow("AddressBlock: access = writeOnce , register: access = read-only is valid") <<
        "writeOnce" << "read-only" << false;
    QTest::newRow("AddressBlock: access = writeOnce , register: access = write-only is invalid") <<
        "writeOnce" << "write-only" << false;
    QTest::newRow("AddressBlock: access = writeOnce , register: access = read-writeOnce is valid") <<
        "writeOnce" << "read-writeOnce" << false;
    QTest::newRow("AddressBlock: access = writeOnce , register: access = writeOnce is valid") <<
        "writeOnce" << "writeOnce" << true;
}

//-----------------------------------------------------------------------------
// Function: tst_AddressBlockValidator::testRegistersAreWithinAddressBlock()
//-----------------------------------------------------------------------------
void tst_AddressBlockValidator::testRegisterPositioning()
{
    QFETCH(QString, registerOffset1);
    QFETCH(QString, registerSize1);
    QFETCH(QString, registerDimension1);
    QFETCH(QString, addressBlockRange);
    QFETCH(QString, addressUnitBits);
    QFETCH(QString, addressBlockWidth);
    QFETCH(QString, invalidIndex);
    QFETCH(bool, isValid);

    QSharedPointer<Field> testField (new Field("testField"));
    testField->setBitOffset("0");
    testField->setBitWidth("1");

    QSharedPointer<Register> registerOne (new Register("Mugen", registerOffset1, registerSize1));
    registerOne->setDimension(registerDimension1);
    registerOne->getFields()->append(testField);

    QSharedPointer<AddressBlock> testBlock (new AddressBlock("testBlock", "0"));
    testBlock->setWidth(addressBlockWidth);
    testBlock->setRange(addressBlockRange);
    testBlock->getRegisterData()->append(registerOne);

    QSharedPointer<AddressBlockValidator> validator = createValidator(Document::Revision::Std14);
    QCOMPARE(validator->hasValidRegisterData(testBlock, addressUnitBits), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testBlock, addressUnitBits, "test");

        QString registerId = registerOne->name();

        if (invalidIndex.isEmpty() == false)
        {
            registerId = QObject::tr("%1 (%2)").arg(registerId).arg(invalidIndex);
        }

        QString expectedError = QObject::tr("Register %1 is not contained within address block %2")
            .arg(registerId).arg(testBlock->name());

        if (registerDimension1 == "0")
        {
            expectedError = QObject::tr("Invalid dimensions set for register '%1' within address block %2")
                .arg(registerOne->name()).arg(testBlock->name());
        }

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_AddressBlockValidator::testRegistersAreWithinAddressBlock_data()
//-----------------------------------------------------------------------------
void tst_AddressBlockValidator::testRegisterPositioning_data()
{
    QTest::addColumn<QString>("registerOffset1");
    QTest::addColumn<QString>("registerSize1");
    QTest::addColumn<QString>("registerDimension1");
    QTest::addColumn<QString>("addressBlockRange");
    QTest::addColumn<QString>("addressUnitBits");
    QTest::addColumn<QString>("addressBlockWidth");
    QTest::addColumn<QString>("invalidIndex"); // index of possible uncontained register replica (if reg has dim)
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Register: offset='h0, size=32, dimension=0; addressBlock range=4, AUB=8, width=32 is not valid")
        << "'h0" << "32" << "0" << "4" << "8" << "32" << "" << false;

    QTest::newRow("Register: offset=4, size=5, dimension=1; addressBlock range = 10; addressUnitBits = 4 is valid")
        << "4" << "5" << "1" << "10" << "4" << "100" << "" << true;
    QTest::newRow("Register: offset=4, size=25, dimension=1; addressBlock range = 10; addressUnitBits = 4 is not valid")
        << "4" << "25" << "1" << "10" << "4" << "100" << "" << false;

    // Register size 5 rounded up to multiple of AUB -> each register occupies 2 addressable units (8 bits)
    QTest::newRow("Register: offset=4, size=5, dimension=4; addressBlock range = 10; addressUnitBits = 4 is not valid")
        << "4" << "5" << "4" << "10" << "4" << "100" << "3" << false;
    QTest::newRow("Register: offset=4, size=5, dimension=1; addressBlock range = 5; addressUnitBits = 4 is not valid")
        << "4" << "5" << "1" << "5" << "4" << "100" << "" << false;

    QTest::newRow("Register: offset='h10, size=32, dimension=; addressBlock range=24, AUB=8, width = 32 is valid")
        << "'h10" << "32" << "" << "24" << "8" << "32" << "" << true;

    /*QTest::newRow("Register with long offset is not valid within a small address block")
        << "4000000000000000000000" << "32" << "0" << "4" << "8" << "32" << false;
    QTest::newRow("Register with long size is not valid within a small address block")
        << "'h0" << "4000000000000000000000" << "0" << "4" << "8" << "32" << false;
    QTest::newRow("Register with long dimension is not valid within a small address block")
        << "'h0" << "32" << "4000000000000000000000" << "4" << "8" << "32" << false;*/
}

//-----------------------------------------------------------------------------
// Function: tst_AddressBlockValidator::testRegisterOverlapping()
//-----------------------------------------------------------------------------
void tst_AddressBlockValidator::testRegisterOverlapping()
{
    QFETCH(QString, registerOffset1);
    QFETCH(QString, registerSize1);
    QFETCH(QString, registerDimension1);
    QFETCH(QString, registerOffset2);
    QFETCH(QString, registerSize2);
    QFETCH(QString, registerDimension2);
    QFETCH(QString, addressUnitBits);
    QFETCH(QString, invalidIndex1);
    QFETCH(QString, invalidIndex2);
    QFETCH(bool, regOneFirst);
    QFETCH(bool, isValid);

    QSharedPointer<Field> testField (new Field("testField"));
    testField->setBitOffset("0");
    testField->setBitWidth("1");

    QSharedPointer<Register> registerOne (new Register("Mugen", registerOffset1, registerSize1));
    registerOne->setDimension(registerDimension1);
    registerOne->getFields()->append(testField);

    QSharedPointer<Register> registerTwo (new Register("Jin", registerOffset2, registerSize2));
    registerTwo->setDimension(registerDimension2);
    registerTwo->getFields()->append(testField);

    QSharedPointer<AddressBlock> testBlock (new AddressBlock("testBlock", "0"));
    testBlock->setWidth("100");
    testBlock->setRange("100");
    testBlock->getRegisterData()->append(registerOne);
    testBlock->getRegisterData()->append(registerTwo);

    QSharedPointer<AddressBlockValidator> validator = createValidator(Document::Revision::Std14);
    QCOMPARE(validator->hasValidRegisterData(testBlock, addressUnitBits), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testBlock, addressUnitBits, "test");

        QString registerId1 = registerOne->name();
        QString registerId2 = registerTwo->name();

        QStringList expectedErrors;

        if (invalidIndex1.isEmpty() == false)
        {
            registerId1 = QObject::tr("%1 (%2)").arg(registerId1).arg(invalidIndex1);
        }

        if (invalidIndex2.isEmpty() == false)
        {
            registerId2 = QObject::tr("%1 (%2)").arg(registerId2).arg(invalidIndex2);
        }

        if (registerDimension1 == "0" || registerDimension2 == "0")
        {
            if (registerDimension1 == "0")
            {
                expectedErrors.append(QObject::tr("Invalid dimensions set for register '%1' within address block %2")
                    .arg(registerOne->name()).arg(testBlock->name()));
            }

            if (registerDimension2 == "0")
            {
                expectedErrors.append(QObject::tr("Invalid dimensions set for register '%1' within address block %2")
                    .arg(registerTwo->name()).arg(testBlock->name()));
            }
        }
        else
        {
            if (regOneFirst)
            {
                expectedErrors << QObject::tr("Register data %1 and %2 overlap within address block %3")
                    .arg(registerId1).arg(registerId2).arg(testBlock->name());
            }
            else
            {
                expectedErrors << QObject::tr("Register data %1 and %2 overlap within address block %3")
                    .arg(registerId2).arg(registerId1).arg(testBlock->name());
            }
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
// Function: tst_AddressBlockValidator::testRegisterOverlapping_data()
//-----------------------------------------------------------------------------
void tst_AddressBlockValidator::testRegisterOverlapping_data()
{
    QTest::addColumn<QString>("registerOffset1");
    QTest::addColumn<QString>("registerSize1");
    QTest::addColumn<QString>("registerDimension1");
    QTest::addColumn<QString>("registerOffset2");
    QTest::addColumn<QString>("registerSize2");
    QTest::addColumn<QString>("registerDimension2");
    QTest::addColumn<QString>("addressUnitBits");

    // Indexes enumerating register replicas for register with dimension
    QTest::addColumn<QString>("invalidIndex1");
    QTest::addColumn<QString>("invalidIndex2");
    
    QTest::addColumn<bool>("regOneFirst"); // Which register should print first in error
    QTest::addColumn<bool>("isValid");

    QTest::newRow("First: offset='h0, size=32, dimension=0; Second: offset=4, size=32, dimension=0; AUB=8 is invalid")
        << "0" << "32" << "0" << "4" << "32" << "0" << "8" << "" << "" << true << false;

    QTest::newRow("Register1: offset=0, size=5, dimension=1; Register2: offset=50, size=5, dimension=2; "
        "Address unit bits: 8 is valid") <<
        "0" << "5" << "1" << "50" << "5" << "2" << "8" << "" << "" << true << true;

    QTest::newRow("Register1: offset=49, size=9, dimension=1; Register2: offset=50, size=5, dimension=2; "
        "Address unit bits: 8 is not valid") <<
        "49" << "9" << "1" << "50" << "5" << "2" << "8" << "" << "0" << true << false;

    QTest::newRow("Register1: offset=50, size=5, dimension=2; Register2: offset=49, size=9, dimension=1; "
        "Address unit bits: 8 is not valid") <<
        "50" << "5" << "2" << "49" << "9" << "1" << "8" << "0" << "" << false << false;

    QTest::newRow("Register1: offset=49, size=5, dimension=2; Register2: offset=50, size=5, dimension=2; "
        "Address unit bits: 8 is not valid") <<
        "49" << "5" << "2" << "50" << "5" << "2" << "8" << "1" << "0" << false << false;

    QTest::newRow("Register1: offset=70, size=5, dimension=1; Register2: offset=50, size=5, dimension=2; "
        "Address unit bits: 8 is valid") <<
        "70" << "5" << "1" << "50" << "5" << "2" << "8" << "" << "" << true << true;

    QTest::newRow("Register1: offset=60, size=5, dimension=1; Register2: offset=50, size=5, dimension=20; "
        "Address unit bits: 8 is not valid") <<
        "60" << "5" << "1" << "50" << "5" << "20" << "8" << "" << "10" << false << false;
}

//-----------------------------------------------------------------------------
// Function: tst_AddressBlockValidator::testRegisterIsOverlappingTwoOtherRegisters()
//-----------------------------------------------------------------------------
void tst_AddressBlockValidator::testRegisterIsOverlappingTwoOtherRegisters()
{
    QSharedPointer<Field> testField (new Field("testField"));
    testField->setBitOffset("0");
    testField->setBitWidth("1");

    // Offset = 0, size = 5, dimension = 4
    QSharedPointer<Register> registerOne (new Register("Mugen", "0", "5"));
    registerOne->setDimension("4");
    registerOne->getFields()->append(testField);

    // Offset = 10, size = 5, dimension = 4
    QSharedPointer<Register> registerTwo (new Register("Jin", "10", "5"));
    registerTwo->setDimension("4");
    registerTwo->getFields()->append(testField);

    // Offset = 3, size = 1, dimension = 8
    QSharedPointer<Register> registerThree (new Register("Fuu", "3", "1"));
    registerThree->setDimension("8");
    registerThree->getFields()->append(testField);

    QSharedPointer<AddressBlock> testBlock (new AddressBlock("champloo", "0"));
    testBlock->setRange("50");
    testBlock->setWidth("10");
    testBlock->getRegisterData()->append(registerOne);
    testBlock->getRegisterData()->append(registerTwo);
    testBlock->getRegisterData()->append(registerThree);

    QString addressUnitBits = QLatin1String("8");

    QSharedPointer<AddressBlockValidator> validator = createValidator(Document::Revision::Std14);
    QCOMPARE(validator->hasValidRegisterData(testBlock, addressUnitBits), false);

    QVector<QString> foundErrors;
    validator->findErrorsIn(foundErrors, testBlock, addressUnitBits, "test");

    QString expectedError = QObject::tr("Register data %1 (0) and %2 (3) overlap within address block %3")
        .arg(registerThree->name()).arg(registerOne->name()).arg(testBlock->name());

    if (errorIsNotFoundInErrorList(expectedError, foundErrors))
    {
        QFAIL("No error message found");
    }

    expectedError = QObject::tr("Register data %1 (7) and %2 (0) overlap within address block %3")
        .arg(registerThree->name()).arg(registerTwo->name()).arg(testBlock->name());
    if (errorIsNotFoundInErrorList(expectedError, foundErrors))
    {
        QFAIL("No error message found");
    }
}

//-----------------------------------------------------------------------------
// Function: tst_AddressBlockValidator::testRegistersWithStride()
//-----------------------------------------------------------------------------
void tst_AddressBlockValidator::testRegistersWithStride()
{
    const QString addressUnitBits = "8";

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

    // Address block with enough space for valid stride
    QSharedPointer<AddressBlock> validAddressBlock(new AddressBlock("ValidStrideFile"));
    validAddressBlock->setBaseAddress("0");
    validAddressBlock->setWidth("32");
    validAddressBlock->setRange("16");
    validAddressBlock->getRegisterData()->append(validStrideRegister);

    // Address block with insufficient range
    QSharedPointer<AddressBlock> smallAddressBlock(new AddressBlock("SmallAddressBlock"));
    smallAddressBlock->setBaseAddress("0");
    smallAddressBlock->setWidth("32");
    smallAddressBlock->setRange("12"); // Not enough for 4 * 4 stride
    smallAddressBlock->getRegisterData()->append(validStrideRegister);

    // Address block with registers with overlapping stride
    QSharedPointer<AddressBlock> overlappingAddressBlock(new AddressBlock("OverlappingStrideAddressBlock"));
    overlappingAddressBlock->setBaseAddress("0");
    overlappingAddressBlock->setWidth("32");
    overlappingAddressBlock->setRange("16"); // Enough range, but stride is too small
    overlappingAddressBlock->getRegisterData()->append(overlappingRegister);

    // Set up validator
    QSharedPointer<AddressBlockValidator> validator = createValidator(Document::Revision::Std22);

    // Valid case
    QList<QString> foundErrors;
    QCOMPARE(validator->hasValidRegisterData(validAddressBlock, addressUnitBits), true);

    validator->findErrorsIn(foundErrors, validAddressBlock, addressUnitBits, "test");
    QVERIFY(foundErrors.isEmpty());

    foundErrors.clear();

    // Small range case
    QCOMPARE(validator->hasValidRegisterData(smallAddressBlock, addressUnitBits), false);
    validator->findErrorsIn(foundErrors, smallAddressBlock, addressUnitBits, "test");

    // Fourth replica is outside range
    QString expectedError = tr("Register %1 (3) is not contained within address block %2")
        .arg(validStrideRegister->name()).arg(smallAddressBlock->name());

    if (errorIsNotFoundInErrorList(expectedError, foundErrors))
    {
        QFAIL("No error message found");
    }

    foundErrors.clear();

    // Overlapping stride case
    QCOMPARE(validator->hasValidRegisterData(overlappingAddressBlock, addressUnitBits), false);
    validator->findErrorsIn(foundErrors, overlappingAddressBlock, addressUnitBits, "test");

    QVERIFY(foundErrors.size() == 3);

    // Overlapping register replicas are (0,1), (1,2) and (2,3)
    QStringList overlapErrors = {
        tr("Register data %1 (0) and %1 (1) overlap within address block %2")
            .arg(overlappingRegister->name()).arg(overlappingAddressBlock->name()),

        tr("Register data %1 (1) and %1 (2) overlap within address block %2")
            .arg(overlappingRegister->name()).arg(overlappingAddressBlock->name()),

        tr("Register data %1 (2) and %1 (3) overlap within address block %2")
            .arg(overlappingRegister->name()).arg(overlappingAddressBlock->name()),
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
// Function: tst_AddressBlockValidator::testHasValidMemoryarray2022()
//-----------------------------------------------------------------------------
void tst_AddressBlockValidator::testHasValidMemoryarray2022()
{
    QFETCH(QString, dimValue);
    QFETCH(QString, indexVar);
    QFETCH(QString, stride);
    QFETCH(bool, validDim);
    QFETCH(bool, validStride);
 
    QSharedPointer<AddressBlockValidator> validator = createValidator(Document::Revision::Std22);

    QSharedPointer<AddressBlock> testBlock(new AddressBlock("champloo", "0"));
    testBlock->setRange("50");
    testBlock->setWidth("10");

    QSharedPointer<MemoryArray> memArray(new MemoryArray());
    QSharedPointer<MemoryArray::Dimension> testDim(new MemoryArray::Dimension());

    testDim->indexVar_ = indexVar;
    testDim->value_ = dimValue;

    memArray->getDimensions()->append(testDim);
    memArray->setStride(stride);

    testBlock->setMemoryArray(memArray);

    QCOMPARE(validator->hasValidMemoryArray(testBlock), validDim && validStride);

    if (!(validDim && validStride))
    {
        QStringList foundErrors;
        validator->findErrorsIn(foundErrors, testBlock, "8", "test");

        QString expectedError;

        if (memArray->getDimensions()->isEmpty())
        {
            expectedError = "No dimensions defined for memory array in address block champloo within test";
        }

        else if (!validDim)
        {
            expectedError = "One or more dimensions of the memory array in address block champloo within test have an invalid value.";
        }

        else if (!validStride)
        {
            expectedError = "Memory array in address block champloo within test has an invalid stride value.";
        }

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_AddressBlockValidator::testHasValidMemoryarray2022_data()
//-----------------------------------------------------------------------------
void tst_AddressBlockValidator::testHasValidMemoryarray2022_data()
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
// Function: tst_AddressBlockValidator::testAccessPolicies2022()
//-----------------------------------------------------------------------------
void tst_AddressBlockValidator::testAccessPolicies2022()
{
    QSharedPointer<AddressBlockValidator> validator = createValidator(Document::Revision::Std22);

    QSharedPointer<AddressBlock> testBlock(new AddressBlock("champloo", "0"));
    testBlock->setRange("50");
    testBlock->setWidth("10");

    QSharedPointer<ModeReference> modeRef1(new ModeReference());
    modeRef1->setPriority(0);
    modeRef1->setReference("ref");

    QSharedPointer<ModeReference> modeRef2(new ModeReference());
    modeRef2->setPriority(0);
    modeRef2->setReference("ref1");

    QSharedPointer<Mode> mode1(new Mode("ref"));
    QSharedPointer<Mode> mode2(new Mode("ref1"));

    QSharedPointer<Component> dummyComponent(new Component(VLNV("test", "vendor", "library", "component", "1.0"), Document::Revision::Std22));
    dummyComponent->getModes()->append(mode1);
    dummyComponent->getModes()->append(mode2);

    validator->componentChange(dummyComponent);

    QSharedPointer<AccessPolicy> accessPolicy1(new AccessPolicy());
    QSharedPointer<AccessPolicy> accessPolicy2(new AccessPolicy());
    accessPolicy1->getModeReferences()->append(modeRef1);
    accessPolicy2->getModeReferences()->append(modeRef2);

    testBlock->getAccessPolicies()->append(accessPolicy1);
    testBlock->getAccessPolicies()->append(accessPolicy2);

    QStringList errors;

    QStringList possibleErrors(QStringList()
        << "One or more mode references in access policies of address block champloo within test contain duplicate priority values."
        << "One or more mode references in access policies of address block champloo within test contain duplicate mode reference values."
        << "In address block testBlock in test, multiple access policies are not allowed if one of them lacks a mode reference."
        << "Mode reference in access policies of address block champloo within test has invalid or empty reference value 'ref2'."
    );

    // Test duplicate mode reference priority.
    validator->findErrorsIn(errors, testBlock, "8", "test");
    QVERIFY(errors.contains(possibleErrors.first()));
    QVERIFY(validator->hasValidAccessPolicies(testBlock) == false);

    // Test duplicate reference.
    errors.clear();

    modeRef2->setPriority(1);
    modeRef2->setReference("ref");

    validator->findErrorsIn(errors, testBlock, "8", "test");
    QVERIFY(errors.contains(possibleErrors.at(1)));
    QVERIFY(validator->hasValidAccessPolicies(testBlock) == false);

    // Test invalid (not found in component modes).
    errors.clear();

    modeRef2->setReference("ref2");

    validator->findErrorsIn(errors, testBlock, "8", "test");
    QVERIFY(errors.contains(possibleErrors.back()));

    QVERIFY(validator->hasValidAccessPolicies(testBlock) == false);

    // Finally test valid.
    errors.clear();
    modeRef2->setReference("ref1");
    
    validator->findErrorsIn(errors, testBlock, "8", "test");
    QVERIFY(std::none_of(possibleErrors.cbegin(), possibleErrors.cend(), [&errors](QString const& str)
        {
            return errors.contains(str);
        }));

    QVERIFY(validator->hasValidAccessPolicies(testBlock));

}

//-----------------------------------------------------------------------------
// Function: tst_AddressBlockValidator::testRegisterAlignment2022()
//-----------------------------------------------------------------------------
void tst_AddressBlockValidator::testRegisterAlignment2022()
{
    QFETCH(QString, registerOffset1);
    QFETCH(QString, registerSize1);
    QFETCH(QString, registerOffset2);
    QFETCH(QString, registerSize2);
    QFETCH(QString, addressBlockWidth);
    QFETCH(bool, misalignmentAllowed);
    QFETCH(bool, isValid);

    QSharedPointer<AddressBlockValidator> validator = createValidator(Document::Revision::Std22);

    QSharedPointer<AddressBlock> testBlock(new AddressBlock("champloo", "0"));
    testBlock->setRange("50");
    testBlock->setWidth(addressBlockWidth);
    testBlock->setMisalignmentAllowed(misalignmentAllowed);

    QSharedPointer<Field> testfield(new Field("testField"));

    QSharedPointer<Register> register1(new Register("register1", registerOffset1, registerSize1));
    QSharedPointer<Register> register2(new Register("register2", registerOffset2, registerSize2));

    register1->getFields()->append(testfield);
    register2->getFields()->append(testfield);

    testBlock->getRegisterData()->append(register1);
    testBlock->getRegisterData()->append(register2);

    QCOMPARE(validator->hasValidRegisterAlignment(testBlock), isValid);

    if (!isValid)
    {
        QStringList foundErrors;
        validator->findErrorsIn(foundErrors, testBlock, "8", "test");

        QString expectedError = QObject::tr("Register misalignment set to false for address block %1 within test, "
            "where one or more registers are fully or partly offset outside of the address block width.")
            .arg(testBlock->name());

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_AddressBlockValidator::testRegisterAlignment2022_data()
//-----------------------------------------------------------------------------
void tst_AddressBlockValidator::testRegisterAlignment2022_data()
{
    QTest::addColumn<QString>("registerOffset1");
    QTest::addColumn<QString>("registerSize1");
    QTest::addColumn<QString>("registerOffset2");
    QTest::addColumn<QString>("registerSize2");
    QTest::addColumn<QString>("addressBlockWidth");
    QTest::addColumn<bool>("misalignmentAllowed");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Register 1: offset = 0, size = 8; Register 2: offset = 24, size = 8; Address block width = 32, misalignment not allowed => valid")
        << "0" << "8" << "24" << "8" << "32" << false << true;

    QTest::newRow("Register 1: offset = 0, size = 8; Register 2: offset = 24, size = 8; Address block width = 32, misalignment allowed => valid")
        << "0" << "8" << "24" << "8" << "32" << true << true;
    
    QTest::newRow("Register 1: offset = 0, size = 8; Register 2: offset = 30, size = 8; Address block width = 32, misalignment not allowed => invalid")
        << "0" << "8" << "30" << "8" << "32" << false << false;

    QTest::newRow("Register 1: offset = 0, size = 8; Register 2: offset = 30, size = 8; Address block width = 32, misalignment allowed => valid")
        << "0" << "8" << "30" << "8" << "32" << true << true;
}

//-----------------------------------------------------------------------------
// Function: tst_AddressBlockValidator::testStructureIsValid2022()
//-----------------------------------------------------------------------------
void tst_AddressBlockValidator::testStructureIsValid2022()
{
    QSharedPointer<AddressBlockValidator> validator = createValidator(Document::Revision::Std22);

    QSharedPointer<AddressBlock> testBlock(new AddressBlock("champloo", "0"));
    testBlock->setRange("50");
    testBlock->setWidth("10");

    // Also set definition reference.
    testBlock->setAddressBlockDefinitionRef("testAddressBlock");
    testBlock->setTypeDefinitionsRef("testTypeDefinitions");

    QString expectedError = "Address block champloo in test must not be explicitly defined while also containing"
        " a definition reference.";

    QStringList foundErrors;
    validator->findErrorsIn(foundErrors, testBlock, "8", "test");
    QCOMPARE(foundErrors.size(), 1);
    QVERIFY(foundErrors.contains(expectedError));
    QCOMPARE(validator->hasValidStructure(testBlock), false);

    foundErrors.clear();
    testBlock->setRange("");
    testBlock->setWidth("");

    validator->findErrorsIn(foundErrors, testBlock, "8", "test");
    QCOMPARE(foundErrors.size(), 0);
    QCOMPARE(validator->hasValidStructure(testBlock), true);

    // Test with usage.
    testBlock->setUsage(General::RESERVED);
    validator->findErrorsIn(foundErrors, testBlock, "8", "test");
    QCOMPARE(foundErrors.size(), 1);
    QVERIFY(foundErrors.contains(expectedError));
    QCOMPARE(validator->hasValidStructure(testBlock), false);
}

//-----------------------------------------------------------------------------
// Function: tst_AddressBlockValidator::testRegisterDataHaveUniqueNames()
//-----------------------------------------------------------------------------
void tst_AddressBlockValidator::testRegisterDataHaveUniqueNames()
{
    // Register and register file names must be unique in an address block or register file.
    QFETCH(QString, registerOneName);
    QFETCH(QString, registerTwoName);
    QFETCH(QString, registerFileOneName);
    QFETCH(QString, registerFileTwoName);
    QFETCH(bool, isValid);
    
    QString addressUnitBits("8");

    QSharedPointer<AddressBlockValidator> validator = createValidator(Document::Revision::Std22);

    QSharedPointer<AddressBlock> testBlock(new AddressBlock("champloo", "0"));
    testBlock->setRange("50");
    testBlock->setWidth("32");

    QSharedPointer<Field> testfield(new Field("testField"));
    testfield->setBitOffset("0");
    testfield->setBitWidth("8");

    QSharedPointer<Register> registerOne(new Register(registerOneName, "0", "8"));
    QSharedPointer<Register> registerTwo(new Register(registerTwoName, "1", "8"));

    registerOne->getFields()->append(testfield);
    registerTwo->getFields()->append(testfield);

    QSharedPointer<RegisterFile> registerFileOne(new RegisterFile(registerFileOneName, "2", "1"));
    QSharedPointer<RegisterFile> registerFileTwo(new RegisterFile(registerFileTwoName, "3", "1"));
    
    testBlock->getRegisterData()->append(registerOne);
    testBlock->getRegisterData()->append(registerTwo);
    testBlock->getRegisterData()->append(registerFileOne);
    testBlock->getRegisterData()->append(registerFileTwo);

    QCOMPARE(validator->hasValidRegisterData(testBlock, addressUnitBits), isValid);

    if (!isValid)
    {
        QStringList foundErrors;
        validator->findErrorsIn(foundErrors, testBlock, addressUnitBits, "test");
        QStringList expectedErrors;

        if (registerOneName == registerTwoName)
        {
            expectedErrors.append(QObject::tr("Name %1 of registers in addressBlock %2 is not unique.")
                .arg(registerOneName).arg(testBlock->name()));
        }
        
        if (registerFileOneName == registerFileTwoName)
        {
            expectedErrors.append(QObject::tr("Name %1 of register files in addressBlock %2 is not unique.")
                .arg(registerFileOneName).arg(testBlock->name()));
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
// Function: tst_AddressBlockValidator::testRegisterDataHaveUniqueNames_data()
//-----------------------------------------------------------------------------
void tst_AddressBlockValidator::testRegisterDataHaveUniqueNames_data()
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
// Function: tst_AddressBlockValidator::errorIsNotFoundInErrorList()
//-----------------------------------------------------------------------------
bool tst_AddressBlockValidator::errorIsNotFoundInErrorList(QString const& expectedError,
    QVector<QString> errorList)
{
    if (!errorList.contains(expectedError))
    {
        qDebug() << "The following error:" << Qt::endl << expectedError << Qt::endl << "was not found in error list:";
        foreach(QString error, errorList)
        {
            qDebug() << error;
        }
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: tst_AddressBlockValidator::createValidator()
//-----------------------------------------------------------------------------
QSharedPointer<AddressBlockValidator> tst_AddressBlockValidator::createValidator(Document::Revision revision)
{
    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator> parameterValidator (
        new ParameterValidator(parser, QSharedPointer<QList<QSharedPointer<Choice> > > (), revision));
    QSharedPointer<EnumeratedValueValidator> enumValidator (new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator (new FieldValidator(parser, enumValidator, parameterValidator));
    QSharedPointer<RegisterValidator> registerValidator (new RegisterValidator(parser, fieldValidator,
        parameterValidator));
    QSharedPointer<RegisterFileValidator> registerFileValidator (new RegisterFileValidator(parser, registerValidator,
        parameterValidator, revision));
   
    QSharedPointer<AddressBlockValidator> validator(new AddressBlockValidator(parser, registerValidator, registerFileValidator, parameterValidator, revision));
    return validator;
}

QTEST_APPLESS_MAIN(tst_AddressBlockValidator)

#include "tst_AddressBlockValidator.moc"
