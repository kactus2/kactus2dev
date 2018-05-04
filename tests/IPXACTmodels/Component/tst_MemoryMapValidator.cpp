//-----------------------------------------------------------------------------
// File: tst_MemoryMapValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 25.11.2015
//
// Description:
// Unit test for class MemoryMapValidator.
//-----------------------------------------------------------------------------

#include <editors/ComponentEditor/common/SystemVerilogExpressionParser.h>

#include <IPXACTmodels/Component/validators/MemoryMapValidator.h>
#include <IPXACTmodels/Component/validators/AddressBlockValidator.h>
#include <IPXACTmodels/Component/validators/RegisterValidator.h>
#include <IPXACTmodels/Component/validators/FieldValidator.h>
#include <IPXACTmodels/Component/validators/EnumeratedValueValidator.h>
#include <IPXACTmodels/common/validators/ParameterValidator2014.h>

#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/Component/MemoryRemap.h>
#include <IPXACTmodels/Component/RemapState.h>
#include <IPXACTmodels/Component/AddressBlock.h>

#include <QtTest>

class tst_MemoryMapValidator : public QObject
{
    Q_OBJECT

public:
    tst_MemoryMapValidator();

private slots:

    void testNameIsValid();
    void testNameIsValid_data();

    void testAddressUnitBitsIsValid();
    void testAddressUnitBitsIsValid_data();

    void testAddressBlockWidthIsMultipleOfAddressUnitBits();
    void testAddressBlockWidthIsMultipleOfAddressUnitBits_data();

    void testHasValidMemoryRemaps();
    void testHasValidMemoryRemaps_data();
    void testMemoryRemapHasValidName();
    void testMemoryRemapHasValidName_data();
    void testMemoryRemapHasValidIsPresent();
    void testMemoryRemapHasValidIsPresent_data();
    void testMemoryRemapsHaveUniqueRemapStates();
    void testMemoryRemapsHaveUniqueRemapStates_data();

private:

    bool errorIsNotFoundInErrorList(QString const& expectedError, QVector<QString> errorList);
};

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapValidator::tst_MemoryMapValidator()
//-----------------------------------------------------------------------------
tst_MemoryMapValidator::tst_MemoryMapValidator()
{
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapValidator::testNameIsValid()
//-----------------------------------------------------------------------------
void tst_MemoryMapValidator::testNameIsValid()
{
    QFETCH(QString, name);
    QFETCH(bool, isValid);

    QSharedPointer<MemoryMap> testMap (new MemoryMap(name));

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator2014> parameterValidator (new ParameterValidator2014(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > > ()));
    QSharedPointer<EnumeratedValueValidator> enumValidator (new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator (new FieldValidator(parser, enumValidator, parameterValidator));
    QSharedPointer<RegisterValidator> registerValidator (
        new RegisterValidator(parser, fieldValidator, parameterValidator));
    QSharedPointer<AddressBlockValidator> addressBlockValidator (
        new AddressBlockValidator(parser, registerValidator, parameterValidator));
    MemoryMapValidator validator(parser, addressBlockValidator,
        QSharedPointer<QList<QSharedPointer<RemapState> > > ());
//     MemoryMapValidator validator(parser, QSharedPointer<QList<QSharedPointer<Choice> > > (),
//         QSharedPointer<QList<QSharedPointer<RemapState> > > ());
    QCOMPARE(validator.hasValidName(testMap), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testMap, "test");

        QString expectedError = QObject::tr("Invalid name specified for memory map %1 within %2").
            arg(testMap->name(), "test");
        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapValidator::testNameIsValid_data()
//-----------------------------------------------------------------------------
void tst_MemoryMapValidator::testNameIsValid_data()
{
    QTest::addColumn<QString>("name");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Name test is valid") << "test" << true;
    QTest::newRow("Empty name is invalid") << "" << false;
    QTest::newRow("Name consisting of only white spaces is invalid") << "    " << false;
    QTest::newRow("Name consisting of characters and white spaces is valid") << "  test  " << true;
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapValidator::testAddressUnitBitsIsValid()
//-----------------------------------------------------------------------------
void tst_MemoryMapValidator::testAddressUnitBitsIsValid()
{
    QFETCH(QString, aub);
    QFETCH(bool, createBlock);
    QFETCH(bool, isValid);

    QSharedPointer<MemoryMap> testMap (new MemoryMap("testMap"));
    testMap->setAddressUnitBits(aub);

    if (createBlock)
    {
        QSharedPointer<AddressBlock> testBlock (new AddressBlock("testBlock"));
        testMap->getMemoryBlocks()->append(testBlock);
    }

    QSharedPointer<ExpressionParser> parser (new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator2014> parameterValidator (new ParameterValidator2014(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > > ()));
    QSharedPointer<EnumeratedValueValidator> enumValidator (new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator (new FieldValidator(parser, enumValidator, parameterValidator));
    QSharedPointer<RegisterValidator> registerValidator (
        new RegisterValidator(parser, fieldValidator, parameterValidator));
    QSharedPointer<AddressBlockValidator> addressBlockValidator (
        new AddressBlockValidator(parser, registerValidator, parameterValidator));
    MemoryMapValidator validator(parser, addressBlockValidator,
        QSharedPointer<QList<QSharedPointer<RemapState> > > ());
    QCOMPARE(validator.hasValidAddressUnitBits(testMap), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testMap, "test");

        QString expectedError = QObject::tr("Invalid address unit bits specified for memory map %1 within %2").
            arg(testMap->name(), "test");
        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapValidator::testAddressUnitBitsIsValid_data()
//-----------------------------------------------------------------------------
void tst_MemoryMapValidator::testAddressUnitBitsIsValid_data()
{
    QTest::addColumn<QString>("aub");
    QTest::addColumn<bool>("createBlock");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Address unit bits of 8 is valid") << "8" << false << true;
    QTest::newRow("Address unit bits of 0 is invalid") << "0" << false << false;
    QTest::newRow("Address unit bits of 32/2 is valid") << "32/2" << false << true;
    QTest::newRow("Address unit bits of -2*2 is invalid") << "-2*2" << false << false;
    QTest::newRow("Address unit bits of text is invalid") << "text" << false << false;
    QTest::newRow("Address unit bits of string is invalid") << "\"text\"" << false << false;
    QTest::newRow("Empty address unit bits is valid if there exists no address blocks") << "" << false << true;
    QTest::newRow("Empty address unit bits is invalid if there exists address blocks") << "" << true << false;
    QTest::newRow("Long address unit bit value is valid") << "40000000000" << false << true;
}


//-----------------------------------------------------------------------------
// Function: tst_MemoryMapValidator::testAddressUnitBitsIsValidForAddressBlocks()
//-----------------------------------------------------------------------------
void tst_MemoryMapValidator::testAddressBlockWidthIsMultipleOfAddressUnitBits()
{
    QFETCH(QString, addressUnitBits);
    QFETCH(QString, addressBlockWidth);
    QFETCH(bool, isValid);

    QSharedPointer<AddressBlock> testBlock (new AddressBlock("testBlock", "0"));
    testBlock->setRange("5");
    testBlock->setWidth(addressBlockWidth);

    QSharedPointer<MemoryMap> testMap (new MemoryMap("TestAddressBlock"));
    testMap->setAddressUnitBits(addressUnitBits);
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
    MemoryMapValidator validator(parser, addressBlockValidator,
        QSharedPointer<QList<QSharedPointer<RemapState> > > ());
    QCOMPARE(validator.hasValidMemoryBlocks(testMap, addressUnitBits), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testMap, "test");

        QString expectedError = QObject::tr("Width of address block %1 is not a multiple of the address unit "
            "bits of %2 %3").arg(testBlock->name()).arg(testMap->elementName()).arg(testMap->name());
        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapValidator::testAddressUnitBitsIsValidForAddressBlocks_data()
//-----------------------------------------------------------------------------
void tst_MemoryMapValidator::testAddressBlockWidthIsMultipleOfAddressUnitBits_data()
{
    QTest::addColumn<QString>("addressUnitBits");
    QTest::addColumn<QString>("addressBlockWidth");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("AddressUnitBits: 4, addressBlock width: 8 is valid") << "4" << "8" << true;
    QTest::newRow("AddressUnitBits: 14-6, addressBlock width: 8*2 is valid") << "14-6" << "8*2" << true;
    QTest::newRow("AddressUnitBits: 16, addressBlock width: 16 is valid") << "16" << "16" << true;
    QTest::newRow("AddressUnitBits: 16, addressBlock width: 21 is invalid") << "16" << "21" << false;
    QTest::newRow("AddressUnitBits: 16, addressBlock width: 8 is invalid") << "16" << "8" << false;
    QTest::newRow("AddressUnitBits not defined, addressBlock width: 8 is valid") << "" << "8" << true;
    QTest::newRow("AddressUnitBits not defined, addressBlock width: 2 is invalid") << "" << "2" << false;
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapValidator::testHasValidMemoryRemaps()
//-----------------------------------------------------------------------------
void tst_MemoryMapValidator::testHasValidMemoryRemaps()
{
    QFETCH(QString, remapState);
    QFETCH(bool, createRemapState);
    QFETCH(bool, isValid);

    QSharedPointer<MemoryRemap> testRemap (new MemoryRemap("testRemap", remapState));

    QSharedPointer<QList<QSharedPointer<RemapState> > > remapStates (new QList<QSharedPointer<RemapState> > ());
    if (createRemapState)
    {
        if (remapState.isEmpty())
        {
            remapState = "champloo";
        }
        QSharedPointer<RemapState> newState (new RemapState(remapState));
        remapStates->append(newState);
    }

    QSharedPointer<MemoryMap> testMap (new MemoryMap("testMap"));
    testMap->getMemoryRemaps()->append(testRemap);

    QSharedPointer<ExpressionParser> parser (new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator2014> parameterValidator (new ParameterValidator2014(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > > ()));
    QSharedPointer<EnumeratedValueValidator> enumValidator (new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator (new FieldValidator(parser, enumValidator, parameterValidator));
    QSharedPointer<RegisterValidator> registerValidator (
        new RegisterValidator(parser, fieldValidator, parameterValidator));
    QSharedPointer<AddressBlockValidator> addressBlockValidator (
        new AddressBlockValidator(parser, registerValidator, parameterValidator));
    MemoryMapValidator validator(parser, addressBlockValidator, remapStates);
    QCOMPARE(validator.hasValidMemoryRemaps(testMap), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testMap, "test");

        QString expectedError = QObject::tr("Invalid remap state %1 set for memory remap %2 within memory map %3")
            .arg(testRemap->getRemapState()).arg(testRemap->name()).arg(testMap->name());
        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapValidator::testHasValidMemoryRemaps_data()
//-----------------------------------------------------------------------------
void tst_MemoryMapValidator::testHasValidMemoryRemaps_data()
{
    QTest::addColumn<QString>("remapState");
    QTest::addColumn<bool>("createRemapState");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Memory remap referencing non-existing remap state is invalid") << "remapState" << false << false;
    QTest::newRow("Memory remap referencing existing remap state is valid") << "remapState" << true << true;
    QTest::newRow("Empty remap state reference is invalid") << "" << true << false;
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapValidator::testHasValidMemoryRemapName()
//-----------------------------------------------------------------------------
void tst_MemoryMapValidator::testMemoryRemapHasValidName()
{
    QFETCH(QString, nameOne);
    QFETCH(QString, nameTwo);
    QFETCH(bool, isValid);

    QSharedPointer<QList<QSharedPointer<RemapState> > > remapStates (new QList<QSharedPointer<RemapState> > ());
    QSharedPointer<RemapState> newState (new RemapState("remapState"));
    QSharedPointer<RemapState> otherState (new RemapState("otherState"));
    remapStates->append(newState);
    remapStates->append(otherState);

    QSharedPointer<MemoryRemap> testRemap (new MemoryRemap(nameOne, newState->name()));
    QSharedPointer<MemoryRemap> otherRemap (new MemoryRemap(nameTwo, otherState->name()));

    QSharedPointer<MemoryMap> testMap (new MemoryMap("testMap"));
    testMap->getMemoryRemaps()->append(testRemap);
    testMap->getMemoryRemaps()->append(otherRemap);

    QSharedPointer<ExpressionParser> parser (new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator2014> parameterValidator (new ParameterValidator2014(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > > ()));
    QSharedPointer<EnumeratedValueValidator> enumValidator (new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator (new FieldValidator(parser, enumValidator, parameterValidator));
    QSharedPointer<RegisterValidator> registerValidator (
        new RegisterValidator(parser, fieldValidator, parameterValidator));
    QSharedPointer<AddressBlockValidator> addressBlockValidator (
        new AddressBlockValidator(parser, registerValidator, parameterValidator));
    MemoryMapValidator validator(parser, addressBlockValidator, remapStates);
    QCOMPARE(validator.hasValidMemoryRemaps(testMap), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testMap, "test");

        if (nameOne == nameTwo)
        {
            QString expectedError = QObject::tr("Name %1 of memory remaps in memory map %2 is not unique.")
                .arg(testRemap->name()).arg(testMap->name());
            if (errorIsNotFoundInErrorList(expectedError, foundErrors))
            {
                QFAIL("No error message found");
            }
        }
        else
        {
            QString expectedError = QObject::tr("Invalid name specified for memory remap %1 within memory map %2")
                .arg(testRemap->name()).arg(testMap->name());
            if (errorIsNotFoundInErrorList(expectedError, foundErrors))
            {
                QFAIL("No error message found");
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapValidator::testHasValidMemoryRemapName_data()
//-----------------------------------------------------------------------------
void tst_MemoryMapValidator::testMemoryRemapHasValidName_data()
{
    QTest::addColumn<QString>("nameOne");
    QTest::addColumn<QString>("nameTwo");
    QTest::addColumn<bool>("isValid");


    QTest::newRow("Name test is valid") << "test" << "test2" << true;
    QTest::newRow("Empty name is invalid") << "" << "test2" << false;
    QTest::newRow("Name consisting of only white spaces is invalid") << "    " << "test2" << false;
    QTest::newRow("Name consisting of characters and spaces is valid") << "test " << "test2" << true;
    QTest::newRow("Non-unique names are not valid") << "test" << "test" << false;
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapValidator::testMemoryRemapHasValidIsPresent()
//-----------------------------------------------------------------------------
void tst_MemoryMapValidator::testMemoryRemapHasValidIsPresent()
{
    QFETCH(QString, isPresent);
    QFETCH(bool, isValid);

    QSharedPointer<RemapState> testState(new RemapState("testState"));
    QSharedPointer<QList<QSharedPointer<RemapState> > > remapStates (new QList<QSharedPointer<RemapState> > ());
    remapStates->append(testState);

    QSharedPointer<MemoryRemap> testRemap (new MemoryRemap("testRemap", testState->name()));
    testRemap->setIsPresent(isPresent);

    QSharedPointer<MemoryMap> testMap (new MemoryMap("TestAddressBlock"));
    testMap->getMemoryRemaps()->append(testRemap);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator2014> parameterValidator (new ParameterValidator2014(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > > ()));
    QSharedPointer<EnumeratedValueValidator> enumValidator (new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator (new FieldValidator(parser, enumValidator, parameterValidator));
    QSharedPointer<RegisterValidator> registerValidator (
        new RegisterValidator(parser, fieldValidator, parameterValidator));
    QSharedPointer<AddressBlockValidator> addressBlockValidator (
        new AddressBlockValidator(parser, registerValidator, parameterValidator));
    MemoryMapValidator validator(parser, addressBlockValidator, remapStates);
    QCOMPARE(validator.hasValidMemoryRemaps(testMap), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testMap, "test");

        QString expectedError = QObject::tr("Invalid isPresent set for memory remap %1 within memory map %2").
            arg(testRemap->name(), testMap->name());
        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapValidator::testMemoryRemapHasValidIsPresent_data()
//-----------------------------------------------------------------------------
void tst_MemoryMapValidator::testMemoryRemapHasValidIsPresent_data()
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
// Function: tst_MemoryMapValidator::testMemoryRemapsHaveUniqueRemapStates()
//-----------------------------------------------------------------------------
void tst_MemoryMapValidator::testMemoryRemapsHaveUniqueRemapStates()
{
    QFETCH(QString, remapStateOne);
    QFETCH(QString, remapStateTwo);
    QFETCH(QString, remapStateThree);
    QFETCH(bool, isValid);

    QSharedPointer<RemapState> stateOne (new RemapState(remapStateOne));
    QSharedPointer<RemapState> stateTwo (new RemapState(remapStateTwo));
    QSharedPointer<RemapState> stateThree (new RemapState(remapStateThree));

    QSharedPointer<QList<QSharedPointer<RemapState> > > remapStates (new QList<QSharedPointer<RemapState> > ());
    remapStates->append(stateOne);
    remapStates->append(stateTwo);
    remapStates->append(stateThree);

    QSharedPointer<MemoryRemap> remapOne (new MemoryRemap("remapOne", stateOne->name()));
    QSharedPointer<MemoryRemap> remapTwo (new MemoryRemap("remapTwo", stateTwo->name()));
    QSharedPointer<MemoryRemap> remapThree (new MemoryRemap("remapThree", stateThree->name()));

    QSharedPointer<MemoryMap> testMap (new MemoryMap("testMap"));
    testMap->getMemoryRemaps()->append(remapOne);
    testMap->getMemoryRemaps()->append(remapTwo);
    testMap->getMemoryRemaps()->append(remapThree);

    QSharedPointer<ExpressionParser> parser (new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator2014> parameterValidator (new ParameterValidator2014(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > > ()));
    QSharedPointer<EnumeratedValueValidator> enumValidator (new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator (new FieldValidator(parser, enumValidator, parameterValidator));
    QSharedPointer<RegisterValidator> registerValidator (
        new RegisterValidator(parser, fieldValidator, parameterValidator));
    QSharedPointer<AddressBlockValidator> addressBlockValidator (
        new AddressBlockValidator(parser, registerValidator, parameterValidator));
    MemoryMapValidator validator(parser, addressBlockValidator, remapStates);
    QCOMPARE(validator.hasValidMemoryRemaps(testMap), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testMap, "test");

        QString expectedError = QObject::tr("Remap states are not unique for each memory remap within memory map "
            "%1").arg(testMap->name());
        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapValidator::testMemoryRemapsHaveUniqueRemapStates_data()
//-----------------------------------------------------------------------------
void tst_MemoryMapValidator::testMemoryRemapsHaveUniqueRemapStates_data()
{
    QTest::addColumn<QString>("remapStateOne");
    QTest::addColumn<QString>("remapStateTwo");
    QTest::addColumn<QString>("remapStateThree");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Different remapStates are valid") << "remap1" << "remap2" << "remap3" << true;
    QTest::newRow("Similar remapStates are not valid") << "remap" << "remap" << "remap" << false;
    QTest::newRow("Similar remapStates are not valid") << "remap1" << "remap" << "remap" << false;
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapBaseValidator::errorIsNotFoundInErrorList()
//-----------------------------------------------------------------------------
bool tst_MemoryMapValidator::errorIsNotFoundInErrorList(QString const& expectedError, QVector<QString> errorList)
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

QTEST_APPLESS_MAIN(tst_MemoryMapValidator)

#include "tst_MemoryMapValidator.moc"
