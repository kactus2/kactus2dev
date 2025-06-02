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

#include <KactusAPI/include/SystemVerilogExpressionParser.h>

#include <IPXACTmodels/Component/validators/MemoryMapBaseValidator.h>
#include <IPXACTmodels/Component/validators/AddressBlockValidator.h>
#include <IPXACTmodels/Component/validators/RegisterValidator.h>
#include <IPXACTmodels/Component/validators/RegisterFileValidator.h>
#include <IPXACTmodels/Component/validators/FieldValidator.h>
#include <IPXACTmodels/Component/validators/SubspaceMapValidator.h>
#include <IPXACTmodels/Component/validators/EnumeratedValueValidator.h>
#include <IPXACTmodels/common/validators/ParameterValidator.h>

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
    void testBlocksWithDimAndStrideOverlap();

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
    QSharedPointer<ParameterValidator> parameterValidator (new ParameterValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > > (), Document::Revision::Std14));
    QSharedPointer<EnumeratedValueValidator> enumValidator (new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator (new FieldValidator(parser, enumValidator, parameterValidator));
    QSharedPointer<RegisterValidator> registerValidator (
        new RegisterValidator(parser, fieldValidator, parameterValidator));
    QSharedPointer<RegisterFileValidator> registerFileValidator (
        new RegisterFileValidator(parser, registerValidator, parameterValidator, Document::Revision::Std14));
    QSharedPointer<AddressBlockValidator> addressBlockValidator (
        new AddressBlockValidator(parser, registerValidator, registerFileValidator, parameterValidator, Document::Revision::Std14));
    QSharedPointer<SubspaceMapValidator> subspaceMapValidator(new SubspaceMapValidator(parser, parameterValidator, Document::Revision::Std14));
    MemoryMapBaseValidator validator(parser, addressBlockValidator, subspaceMapValidator, Document::Revision::Std14);

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
    QSharedPointer<ParameterValidator> parameterValidator (new ParameterValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > > (), Document::Revision::Std14));
    QSharedPointer<EnumeratedValueValidator> enumValidator (new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator (new FieldValidator(parser, enumValidator, parameterValidator));
    QSharedPointer<RegisterValidator> registerValidator (
        new RegisterValidator(parser, fieldValidator, parameterValidator));
    QSharedPointer<RegisterFileValidator> registerFileValidator (
        new RegisterFileValidator(parser, registerValidator, parameterValidator, Document::Revision::Std14));
    QSharedPointer<AddressBlockValidator> addressBlockValidator (
        new AddressBlockValidator(parser, registerValidator, registerFileValidator, parameterValidator, Document::Revision::Std14));
    QSharedPointer<SubspaceMapValidator> subspaceMapValidator(new SubspaceMapValidator(parser, parameterValidator, Document::Revision::Std14));
    MemoryMapBaseValidator validator(parser, addressBlockValidator, subspaceMapValidator, Document::Revision::Std14);
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
    QSharedPointer<ParameterValidator> parameterValidator (new ParameterValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > > (), Document::Revision::Std14));
    QSharedPointer<EnumeratedValueValidator> enumValidator (new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator (new FieldValidator(parser, enumValidator, parameterValidator));
    QSharedPointer<RegisterValidator> registerValidator (
        new RegisterValidator(parser, fieldValidator, parameterValidator));
    QSharedPointer<RegisterFileValidator> registerFileValidator (
        new RegisterFileValidator(parser, registerValidator, parameterValidator, Document::Revision::Std14));
    QSharedPointer<AddressBlockValidator> addressBlockValidator (
        new AddressBlockValidator(parser, registerValidator, registerFileValidator, parameterValidator, Document::Revision::Std14));
    QSharedPointer<SubspaceMapValidator> subspaceMapValidator(new SubspaceMapValidator(parser, parameterValidator, Document::Revision::Std14));
    MemoryMapBaseValidator validator(parser, addressBlockValidator, subspaceMapValidator, Document::Revision::Std14);
    QCOMPARE(validator.hasValidMemoryBlocks(testMap, ""), false);

    QVector<QString> foundErrors;
    validator.findErrorsIn(foundErrors, testMap, "8", "test");

    QString expectedError = QObject::tr("Invalid baseAddress set for address block %1 within memory map %2 in test").
        arg(testBlock->name()).arg(testMap->name());
    if (errorIsNotFoundInErrorList(expectedError, foundErrors))
    {
        QFAIL("No error message found");
    }

    testBlock->setBaseAddress("0");
    testBlock->setRange("2");
    testBlock->setWidth("16");
    QCOMPARE(validator.hasValidMemoryBlocks(testMap, "8"), true);

    QSharedPointer<AddressBlock> otherBlock (new AddressBlock(*testBlock.data()));
    otherBlock->setBaseAddress("3");
    testMap->getMemoryBlocks()->append(otherBlock);
    QCOMPARE(validator.hasValidMemoryBlocks(testMap, "8"), false);

    foundErrors.clear();
    validator.findErrorsIn(foundErrors, testMap, "8", "test");
    expectedError = QObject::tr("Name %1 of memory blocks within memory map %2 in test is not unique")
        .arg(testBlock->name()).arg(testMap->name());

    if (errorIsNotFoundInErrorList(expectedError, foundErrors))
    {
        QFAIL("No error message found");
    }

    otherBlock->setName("otherBlock");
    QCOMPARE(validator.hasValidMemoryBlocks(testMap, "8"), true);
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
    QFETCH(bool, oneFirst);
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
    QSharedPointer<ParameterValidator> parameterValidator (new ParameterValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > > (), Document::Revision::Std14));
    QSharedPointer<EnumeratedValueValidator> enumValidator (new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator (new FieldValidator(parser, enumValidator, parameterValidator));
    QSharedPointer<RegisterValidator> registerValidator (
        new RegisterValidator(parser, fieldValidator, parameterValidator));
    QSharedPointer<RegisterFileValidator> registerFileValidator (
        new RegisterFileValidator(parser, registerValidator, parameterValidator, Document::Revision::Std14));
    QSharedPointer<AddressBlockValidator> addressBlockValidator (
        new AddressBlockValidator(parser, registerValidator, registerFileValidator, parameterValidator, Document::Revision::Std14));
    MemoryMapBaseValidator validator(parser, addressBlockValidator, nullptr);
    QCOMPARE(validator.hasValidMemoryBlocks(testMap, "8"), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testMap, "8", "test");
        
        QString expectedError;

        if (oneFirst)
        {
            expectedError = QObject::tr("Memory blocks %1 and %2 overlap within memory map %3 in test")
                .arg(blockOne->name()).arg(blockTwo->name()).arg(testMap->name());
        }
        else
        {
            expectedError = QObject::tr("Memory blocks %1 and %2 overlap within memory map %3 in test")
                .arg(blockTwo->name()).arg(blockOne->name()).arg(testMap->name());
        }
        

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
    QTest::addColumn<bool>("oneFirst");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Block1: baseAddress = 0, range = 2, Block2: baseAddress = 2, range = 2 is valid") << "0" <<
        "2" << "2" << "2" << true << true;
    QTest::newRow("Block1: baseAddress = 5, range = 10, Block2: baseAddress = 2, range = 4 is invalid") << "5" <<
        "10" << "2" << "4" << false << false;
    QTest::newRow("Block1: baseAddress = 5, range = 10, Block2: baseAddress = 6, range = 2 is invalid") << "5" <<
        "10" << "6" << "2" << true << false;
    QTest::newRow("Block1: baseAddress = 5, range = 10, Block2: baseAddress = 8, range = 10 is invalid") << "5" <<
        "10" << "8" << "10" << true << false;
    QTest::newRow("Block1: baseAddress = 5, range = 10, Block2: baseAddress = 15, range = 10 is valid") << "5" <<
        "10" << "15" << "10" << true << true;
    QTest::newRow("Block1: baseAddress = 8, range = 10, Block2: baseAddress = 5, range = 10 is invalid") << "8" <<
        "10" << "5" << "10" << false << false;
    QTest::newRow("Block1: baseAddress = 15, range = 10, Block2: baseAddress = 5, range = 10 is valid") << "15" <<
        "10" << "5" << "10" << false << true;
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapBaseValidator::testBlocksWithStrideOverlap()
//-----------------------------------------------------------------------------
void tst_MemoryMapBaseValidator::testBlocksWithDimAndStrideOverlap()
{
    // Block two overlaps with block one
    const QString baseAddressOne = "0";
    const QString rangeOne = "1";
    const QString dimOne = "3";
    const QString strideOne = "2";
    const QString baseAddressTwo = "2";
    const QString rangeTwo = "1";
    const QString dimTwo = "2";
    const QString strideTwo = "2";

    // Overlaps are (blockOne (1), blockTwo (0)) and (blockOne (2), blockTwo(1))
    QSharedPointer<AddressBlock> blockOne(new AddressBlock("One", baseAddressOne));
    blockOne->setRange(rangeOne);
    blockOne->setWidth("8");
    blockOne->setDimension(dimOne);
    blockOne->setStride(strideOne);

    QSharedPointer<AddressBlock> blockTwo(new AddressBlock("Two", baseAddressTwo));
    blockTwo->setRange(rangeTwo);
    blockTwo->setWidth("8");
    blockTwo->setDimension(dimTwo);
    blockTwo->setStride(strideTwo);

    // Block with too small stride -> replicas overlap
    QSharedPointer<AddressBlock> blockThree(new AddressBlock("Three", "50"));
    blockThree->setRange("2");
    blockThree->setWidth("8");
    blockThree->setDimension("3");
    blockThree->setStride("1");
    // Overlaps are replicas (0,1) and (1,2)
    
    QSharedPointer<MemoryMapBase> testMap(new MemoryMapBase("testMap"));
    testMap->getMemoryBlocks()->append(blockOne);
    testMap->getMemoryBlocks()->append(blockTwo);
    testMap->getMemoryBlocks()->append(blockThree);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator> parameterValidator(new ParameterValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > >(), Document::Revision::Std22));
    QSharedPointer<EnumeratedValueValidator> enumValidator(new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator(new FieldValidator(parser, enumValidator, parameterValidator));
    QSharedPointer<RegisterValidator> registerValidator(
        new RegisterValidator(parser, fieldValidator, parameterValidator));
    QSharedPointer<RegisterFileValidator> registerFileValidator(
        new RegisterFileValidator(parser, registerValidator, parameterValidator, Document::Revision::Std22));
    QSharedPointer<AddressBlockValidator> addressBlockValidator(
        new AddressBlockValidator(parser, registerValidator, registerFileValidator, parameterValidator, Document::Revision::Std22));
    MemoryMapBaseValidator validator(parser, addressBlockValidator, nullptr, Document::Revision::Std22);
    QCOMPARE(validator.hasValidMemoryBlocks(testMap, "8"), false);

    QStringList actualErrors;

    validator.findErrorsIn(actualErrors, testMap, "8", "test");

    // Alternatives for each expected overlap
    QStringList overlap1 = {
        "Memory blocks One (1) and Two (0) overlap within memory map testMap in test",
        "Memory blocks Two (0) and One (1) overlap within memory map testMap in test"
    };

    QStringList overlap2 = {
        "Memory blocks One (2) and Two (1) overlap within memory map testMap in test",
        "Memory blocks Two (1) and One (2) overlap within memory map testMap in test"
    };

    QStringList overlap3 = {
        "Memory blocks Three (0) and Three (1) overlap within memory map testMap in test",
        "Memory blocks Three (1) and Three (0) overlap within memory map testMap in test"
    };

    QStringList overlap4 = {
        "Memory blocks Three (1) and Three (2) overlap within memory map testMap in test",
        "Memory blocks Three (2) and Three (1) overlap within memory map testMap in test"
    };

    // Track which overlaps were found
    bool foundOverlap1 = false;
    bool foundOverlap2 = false;
    bool foundOverlap3 = false;
    bool foundOverlap4 = false;

    for (auto const& error : actualErrors)
    {
        if (overlap1.contains(error))
        {
            QVERIFY2(!foundOverlap1, "Duplicate overlap1 error found");
            foundOverlap1 = true;
        }
        else if (overlap2.contains(error))
        {
            QVERIFY2(!foundOverlap2, "Duplicate overlap2 error found");
            foundOverlap2 = true;
        }
        else if (overlap3.contains(error))
        {
            QVERIFY2(!foundOverlap3, "Duplicate overlap3 error found");
            foundOverlap3 = true;
        }
        else if (overlap4.contains(error))
        {
            QVERIFY2(!foundOverlap4, "Duplicate overlap4 error found");
            foundOverlap4 = true;
        }
        else
        {
            QFAIL(qPrintable(QString("Unexpected error: %1").arg(error)));
        }
    }

    // Ensure all expected overlaps were found
    QVERIFY(foundOverlap1);
    QVERIFY(foundOverlap2);
    QVERIFY(foundOverlap3);
    QVERIFY(foundOverlap4);
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapBaseValidator::errorIsNotFoundInErrorList()
//-----------------------------------------------------------------------------
bool tst_MemoryMapBaseValidator::errorIsNotFoundInErrorList(QString const& expectedError,
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

QTEST_APPLESS_MAIN(tst_MemoryMapBaseValidator)

#include "tst_MemoryMapBaseValidator.moc"
