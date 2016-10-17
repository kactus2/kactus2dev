//-----------------------------------------------------------------------------
// File: tst_MemoryMapBaseValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 25.11.2015
//
// Description:
// Unit test for class MemoryMapBaseValidator.
//-----------------------------------------------------------------------------

#include <editors/ComponentEditor/common/SystemVerilogExpressionParser.h>

#include <IPXACTmodels/Component/validators/MemoryMapBaseValidator.h>
#include <IPXACTmodels/Component/validators/AddressBlockValidator.h>
#include <IPXACTmodels/Component/validators/RegisterValidator.h>
#include <IPXACTmodels/Component/validators/FieldValidator.h>
#include <IPXACTmodels/Component/validators/EnumeratedValueValidator.h>
#include <IPXACTmodels/common/validators/ParameterValidator2014.h>

#include <IPXACTmodels/Component/MemoryMapBase.h>
#include <IPXACTmodels/Component/AddressBlock.h>

#include <QtTest>

class tst_MemoryMapBaseValidator : public QObject
{
    Q_OBJECT

public:
    tst_MemoryMapBaseValidator();

private slots:

    void testNameIsValid();
    void testNameIsValid_data();

    void testIsPresentIsValid();
    void testIsPresentIsValid_data();

    void testAddressBlocksAreValid();
    void testAddressBlocksOverlap();
    void testAddressBlocksOverlap_data();

private:

    bool errorIsNotFoundInErrorList(QString const& expectedError, QVector<QString> errorList);
};

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapBaseValidator::tst_MemoryMapBaseValidator()
//-----------------------------------------------------------------------------
tst_MemoryMapBaseValidator::tst_MemoryMapBaseValidator()
{
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapBaseValidator::testNameIsValid()
//-----------------------------------------------------------------------------
void tst_MemoryMapBaseValidator::testNameIsValid()
{
    QFETCH(QString, name);
    QFETCH(bool, isValid);

    QSharedPointer<MemoryMapBase> testMap (new MemoryMapBase(name));

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator2014> parameterValidator (new ParameterValidator2014(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > > ()));
    QSharedPointer<EnumeratedValueValidator> enumValidator (new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator (new FieldValidator(parser, enumValidator, parameterValidator));
    QSharedPointer<RegisterValidator> registerValidator (
        new RegisterValidator(parser, fieldValidator, parameterValidator));
    QSharedPointer<AddressBlockValidator> addressBlockValidator (
        new AddressBlockValidator(parser, registerValidator, parameterValidator));
    MemoryMapBaseValidator validator(parser, addressBlockValidator);
//     MemoryMapBaseValidator validator(parser, QSharedPointer<QList<QSharedPointer<Choice> > > ());
    QCOMPARE(validator.hasValidName(testMap), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testMap, "", "test");

        QString expectedError = QObject::tr("Invalid name specified for memory map %1 within %2").
            arg(testMap->name(), "test");
        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapBaseValidator::testNameIsValid_data()
//-----------------------------------------------------------------------------
void tst_MemoryMapBaseValidator::testNameIsValid_data()
{
    QTest::addColumn<QString>("name");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Name test is valid") << "test" << true;
    QTest::newRow("Empty name is invalid") << "" << false;
    QTest::newRow("Name consisting of only white spaces is invalid") << "    " << false;
    QTest::newRow("Name consisting of characters and white spaces is valid") << "  test  " << true;
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapBaseValidator::testIsPresentIsValid()
//-----------------------------------------------------------------------------
void tst_MemoryMapBaseValidator::testIsPresentIsValid()
{
    QFETCH(QString, isPresent);
    QFETCH(bool, isValid);

    QSharedPointer<MemoryMapBase> testMap (new MemoryMapBase("TestAddressBlock"));
    testMap->setIsPresent(isPresent);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator2014> parameterValidator (new ParameterValidator2014(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > > ()));
    QSharedPointer<EnumeratedValueValidator> enumValidator (new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator (new FieldValidator(parser, enumValidator, parameterValidator));
    QSharedPointer<RegisterValidator> registerValidator (
        new RegisterValidator(parser, fieldValidator, parameterValidator));
    QSharedPointer<AddressBlockValidator> addressBlockValidator (
        new AddressBlockValidator(parser, registerValidator, parameterValidator));
    MemoryMapBaseValidator validator(parser, addressBlockValidator);
    QCOMPARE(validator.hasValidIsPresent(testMap), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testMap, "", "test");

        QString expectedError = QObject::tr("Invalid isPresent set for memory map %1 within %2").
            arg(testMap->name(), "test");
        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapBaseValidator::testIsPresentIsValid_data()
//-----------------------------------------------------------------------------
void tst_MemoryMapBaseValidator::testIsPresentIsValid_data()
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
// Function: tst_MemoryMapBaseValidator::testAddressSpacesAreValid()
//-----------------------------------------------------------------------------
void tst_MemoryMapBaseValidator::testAddressBlocksAreValid()
{
    QSharedPointer<AddressBlock> testBlock (new AddressBlock("testBlock"));

    QSharedPointer<MemoryMapBase> testMap (new MemoryMapBase("testMap"));
    testMap->getMemoryBlocks()->append(testBlock);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator2014> parameterValidator (new ParameterValidator2014(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > > ()));
    QSharedPointer<EnumeratedValueValidator> enumValidator (new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator (new FieldValidator(parser, enumValidator, parameterValidator));
    QSharedPointer<RegisterValidator> registerValidator (
        new RegisterValidator(parser, fieldValidator, parameterValidator));
    QSharedPointer<AddressBlockValidator> addressBlockValidator (
        new AddressBlockValidator(parser, registerValidator, parameterValidator));
    MemoryMapBaseValidator validator(parser, addressBlockValidator);
    QCOMPARE(validator.hasValidMemoryBlocks(testMap, ""), false);

    QVector<QString> foundErrors;
    validator.findErrorsIn(foundErrors, testMap, "", "test");

    QString expectedError = QObject::tr("Invalid baseAddress set for address block %1 within memory map %2").
        arg(testBlock->name()).arg(testMap->name());
    if (errorIsNotFoundInErrorList(expectedError, foundErrors))
    {
        QFAIL("No error message found");
    }

    testBlock->setBaseAddress("0");
    testBlock->setRange("2");
    testBlock->setWidth("16");
    QCOMPARE(validator.hasValidMemoryBlocks(testMap, ""), true);

    QSharedPointer<AddressBlock> otherBlock (new AddressBlock(*testBlock.data()));
    otherBlock->setBaseAddress("3");
    testMap->getMemoryBlocks()->append(otherBlock);
    QCOMPARE(validator.hasValidMemoryBlocks(testMap, ""), false);

    foundErrors.clear();
    validator.findErrorsIn(foundErrors, testMap, "", "test");
    expectedError = QObject::tr("Name %1 of address blocks in memory map %2 is not unique")
        .arg(testBlock->name()).arg(testMap->name());

    if (errorIsNotFoundInErrorList(expectedError, foundErrors))
    {
        QFAIL("No error message found");
    }

    otherBlock->setName("otherBlock");
    QCOMPARE(validator.hasValidMemoryBlocks(testMap, ""), true);
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapBaseValidator::testAddressBlocksOverlap()
//-----------------------------------------------------------------------------
void tst_MemoryMapBaseValidator::testAddressBlocksOverlap()
{
    QFETCH(QString, baseAddressOne);
    QFETCH(QString, rangeOne);
    QFETCH(QString, baseAddressTwo);
    QFETCH(QString, rangeTwo);
    QFETCH(bool, isValid);

    QSharedPointer<AddressBlock> blockOne (new AddressBlock("One", baseAddressOne));
    blockOne->setRange(rangeOne);
    blockOne->setWidth("8");

    QSharedPointer<AddressBlock> blockTwo (new AddressBlock("Two", baseAddressTwo));
    blockTwo->setRange(rangeTwo);
    blockTwo->setWidth("8");

    QSharedPointer<MemoryMapBase> testMap (new MemoryMapBase("testMap"));
    testMap->getMemoryBlocks()->append(blockOne);
    testMap->getMemoryBlocks()->append(blockTwo);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator2014> parameterValidator (new ParameterValidator2014(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > > ()));
    QSharedPointer<EnumeratedValueValidator> enumValidator (new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator (new FieldValidator(parser, enumValidator, parameterValidator));
    QSharedPointer<RegisterValidator> registerValidator (
        new RegisterValidator(parser, fieldValidator, parameterValidator));
    QSharedPointer<AddressBlockValidator> addressBlockValidator (
        new AddressBlockValidator(parser, registerValidator, parameterValidator));
    MemoryMapBaseValidator validator(parser, addressBlockValidator);
    QCOMPARE(validator.hasValidMemoryBlocks(testMap, ""), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testMap, "", "test");
        QString expectedError = QObject::tr("Address blocks %1 and %2 overlap in memory map %3")
            .arg(blockOne->name()).arg(blockTwo->name()).arg(testMap->name());

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapBaseValidator::testAddressBlocksOverlap_data()
//-----------------------------------------------------------------------------
void tst_MemoryMapBaseValidator::testAddressBlocksOverlap_data()
{
    QTest::addColumn<QString>("baseAddressOne");
    QTest::addColumn<QString>("rangeOne");
    QTest::addColumn<QString>("baseAddressTwo");
    QTest::addColumn<QString>("rangeTwo");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Block1: baseAddress = 0, range = 2, Block2: baseAddress = 2, range = 2 is valid") << "0" <<
        "2" << "2" << "2" << true;
    QTest::newRow("Block1: baseAddress = 5, range = 10, Block2: baseAddress = 2, range = 4 is invalid") << "5" <<
        "10" << "2" << "4" << false;
    QTest::newRow("Block1: baseAddress = 5, range = 10, Block2: baseAddress = 6, range = 2 is invalid") << "5" <<
        "10" << "6" << "2" << false;
    QTest::newRow("Block1: baseAddress = 5, range = 10, Block2: baseAddress = 8, range = 10 is invalid") << "5" <<
        "10" << "8" << "10" << false;
    QTest::newRow("Block1: baseAddress = 5, range = 10, Block2: baseAddress = 15, range = 10 is valid") << "5" <<
        "10" << "15" << "10" << true;
    QTest::newRow("Block1: baseAddress = 8, range = 10, Block2: baseAddress = 5, range = 10 is invalid") << "8" <<
        "10" << "5" << "10" << false;
    QTest::newRow("Block1: baseAddress = 15, range = 10, Block2: baseAddress = 5, range = 10 is valid") << "15" <<
        "10" << "5" << "10" << true;
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapBaseValidator::errorIsNotFoundInErrorList()
//-----------------------------------------------------------------------------
bool tst_MemoryMapBaseValidator::errorIsNotFoundInErrorList(QString const& expectedError,
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

QTEST_APPLESS_MAIN(tst_MemoryMapBaseValidator)

#include "tst_MemoryMapBaseValidator.moc"
