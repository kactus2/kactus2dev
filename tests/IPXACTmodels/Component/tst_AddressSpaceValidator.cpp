//-----------------------------------------------------------------------------
// File: tst_AddressSpaceValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 30.11.2015
//
// Description:
// Unit test for class AddressSpaceValidator.
//-----------------------------------------------------------------------------

#include <editors/ComponentEditor/common/SystemVerilogExpressionParser.h>

#include <IPXACTmodels/Component/validators/AddressSpaceValidator.h>
#include <IPXACTmodels/Component/validators/MemoryMapBaseValidator.h>
#include <IPXACTmodels/Component/validators/AddressBlockValidator.h>
#include <IPXACTmodels/Component/validators/RegisterValidator.h>
#include <IPXACTmodels/Component/validators/FieldValidator.h>
#include <IPXACTmodels/Component/validators/EnumeratedValueValidator.h>
#include <IPXACTmodels/common/validators/ParameterValidator2014.h>

#include <IPXACTmodels/Component/AddressSpace.h>
#include <IPXACTmodels/Component/Segment.h>
#include <IPXACTmodels/Component/MemoryMapBase.h>
#include <IPXACTmodels/common/Parameter.h>

#include <QtTest>

class tst_AddressSpaceValidator : public QObject
{
    Q_OBJECT

public:
    tst_AddressSpaceValidator();

private slots:
  
    void testHasValidName();
    void testHasValidName_data();

    void testHasValidIsPresent();
    void testHasValidIsPresent_data();

    void testHasValidRange();
    void testHasValidRange_data();

    void testHasValidWidth();
    void testHasValidWidth_data();

    void testSegmentHasValidName();
    void testSegmentHasValidName_data();
    void testSegmentsHaveUniqueNames();
    void testSegmentHasValidIsPresent();
    void testSegmentHasValidIsPresent_data();
    void testSegmentHasValidAddressOffset();
    void testSegmentHasValidAddressOffset_data();
    void testSegmentHasValidRange();
    void testSegmentHasValidRange_data();

    void testSegmentIsContainedWithinAddressSpace();
    void testSegmentIsContainedWithinAddressSpace_data();
    void testOverlappingSegments();
    void testOverlappingSegments_data();

    void testHasValidAddressUnitBits();
    void testHasValidAddressUnitBits_data();

    void testHasValidLocalMemoryMap();

    void testHasValidParameters();

private:

    bool errorIsNotFoundInErrorList(QString const& expectedError, QVector<QString> errorList);
};

//-----------------------------------------------------------------------------
// Function: tst_AddressSpaceValidator::tst_AddressSpaceValidator()
//-----------------------------------------------------------------------------
tst_AddressSpaceValidator::tst_AddressSpaceValidator()
{

}

//-----------------------------------------------------------------------------
// Function: tst_AddressSpaceValidator::testHasValidName()
//-----------------------------------------------------------------------------
void tst_AddressSpaceValidator::testHasValidName()
{
    QFETCH(QString, name);
    QFETCH(bool, isValid);

    QSharedPointer<AddressSpace> testSpace (new AddressSpace(name));

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator2014> parameterValidator (
        new ParameterValidator2014(parser, QSharedPointer<QList<QSharedPointer<Choice> > > ()));
    QSharedPointer<EnumeratedValueValidator> enumeratedValueValidator (new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator (
        new FieldValidator(parser, enumeratedValueValidator, parameterValidator));
    QSharedPointer<RegisterValidator> registerValidator (
        new RegisterValidator(parser, fieldValidator, parameterValidator));
    QSharedPointer<AddressBlockValidator> addressBlockValidator (
        new AddressBlockValidator(parser, registerValidator, parameterValidator));
    QSharedPointer<MemoryMapBaseValidator> localMemoryMapValidator (
        new MemoryMapBaseValidator(parser, addressBlockValidator));
    AddressSpaceValidator validator(parser, localMemoryMapValidator, parameterValidator);
//     AddressSpaceValidator validator(parser, QSharedPointer<QList<QSharedPointer<Choice> > > ());
    QCOMPARE(validator.hasValidName(testSpace->name()), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testSpace, "test");

        QString expectedError = QObject::tr("Invalid name specified for address space %1 within %2")
            .arg(testSpace->name()).arg("test");
        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_AddressSpaceValidator::testHasValidName_data()
//-----------------------------------------------------------------------------
void tst_AddressSpaceValidator::testHasValidName_data()
{
    QTest::addColumn<QString>("name");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Name test is valid") << "test" << true;
    QTest::newRow("Empty name is invalid") << "" << false;
    QTest::newRow("Name consisting of only white spaces is invalid") << "    " << false;
    QTest::newRow("Name consisting of characters and white spaces is valid") << "  test  " << true;
}

//-----------------------------------------------------------------------------
// Function: tst_AddressSpaceValidator::testHasValidIsPresent()
//-----------------------------------------------------------------------------
void tst_AddressSpaceValidator::testHasValidIsPresent()
{
    QFETCH(QString, isPresent);
    QFETCH(bool, isValid);

    QSharedPointer<AddressSpace> testSpace (new AddressSpace("TestAddressBlock"));
    testSpace->setIsPresent(isPresent);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator2014> parameterValidator (
        new ParameterValidator2014(parser, QSharedPointer<QList<QSharedPointer<Choice> > > ()));
    QSharedPointer<EnumeratedValueValidator> enumeratedValueValidator (new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator (
        new FieldValidator(parser, enumeratedValueValidator, parameterValidator));
    QSharedPointer<RegisterValidator> registerValidator (
        new RegisterValidator(parser, fieldValidator, parameterValidator));
    QSharedPointer<AddressBlockValidator> addressBlockValidator (
        new AddressBlockValidator(parser, registerValidator, parameterValidator));
    QSharedPointer<MemoryMapBaseValidator> localMemoryMapValidator (
        new MemoryMapBaseValidator(parser, addressBlockValidator));
    AddressSpaceValidator validator(parser, localMemoryMapValidator, parameterValidator);
    QCOMPARE(validator.hasValidIsPresent(testSpace->getIsPresent()), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testSpace, "test");

        QString expectedError = QObject::tr("Invalid isPresent set for address space %1 within %2")
            .arg(testSpace->name()).arg("test");
        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_AddressSpaceValidator::testHasValidIsPresent_data()
//-----------------------------------------------------------------------------
void tst_AddressSpaceValidator::testHasValidIsPresent_data()
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
// Function: tst_AddressSpaceValidator::testHasValidRange()
//-----------------------------------------------------------------------------
void tst_AddressSpaceValidator::testHasValidRange()
{
    QFETCH(QString, range);
    QFETCH(bool, isValid);

    QSharedPointer<AddressSpace> testSpace (new AddressSpace("testSpace", range, "0"));

    QSharedPointer<ExpressionParser> parser (new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator2014> parameterValidator (
        new ParameterValidator2014(parser, QSharedPointer<QList<QSharedPointer<Choice> > > ()));
    QSharedPointer<EnumeratedValueValidator> enumeratedValueValidator (new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator (
        new FieldValidator(parser, enumeratedValueValidator, parameterValidator));
    QSharedPointer<RegisterValidator> registerValidator (
        new RegisterValidator(parser, fieldValidator, parameterValidator));
    QSharedPointer<AddressBlockValidator> addressBlockValidator (
        new AddressBlockValidator(parser, registerValidator, parameterValidator));
    QSharedPointer<MemoryMapBaseValidator> localMemoryMapValidator (
        new MemoryMapBaseValidator(parser, addressBlockValidator));
    AddressSpaceValidator validator(parser, localMemoryMapValidator, parameterValidator);
    QCOMPARE(validator.hasValidRange(testSpace), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testSpace, "test");

        QString expectedError = QObject::tr("Invalid range set for address space %1 within %2")
            .arg(testSpace->name()).arg("test");
        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_AddressSpaceValidator::testHasValidRange_data()
//-----------------------------------------------------------------------------
void tst_AddressSpaceValidator::testHasValidRange_data()
{
    QTest::addColumn<QString>("range");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("1 is valid for range") << "1" << true;
    QTest::newRow("12*2+13-5 is valid for range") << "12*2+13-5" << true;
    QTest::newRow("0 is not valid for range") << "0" << false;
    QTest::newRow("-20 is not valid for range") << "-20" << false;
    QTest::newRow("14-5*3 is not valid for range") << "14-5*3" << false;
    QTest::newRow("Text is not valid for range") << "text" << false;
    QTest::newRow("String is not valid for range") << "\"text\"" << false;
    QTest::newRow("Empty value is not valid for range") << "" << false;

    QTest::newRow("Long range is valid") << "40000000000" << true;
}

//-----------------------------------------------------------------------------
// Function: tst_AddressSpaceValidator::testHasValidWidth()
//-----------------------------------------------------------------------------
void tst_AddressSpaceValidator::testHasValidWidth()
{
    QFETCH(QString, width);
    QFETCH(bool, isValid);

    QSharedPointer<AddressSpace> testSpace (new AddressSpace("testSpace", "10", width));

    QSharedPointer<ExpressionParser> parser (new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator2014> parameterValidator (
        new ParameterValidator2014(parser, QSharedPointer<QList<QSharedPointer<Choice> > > ()));
    QSharedPointer<EnumeratedValueValidator> enumeratedValueValidator (new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator (
        new FieldValidator(parser, enumeratedValueValidator, parameterValidator));
    QSharedPointer<RegisterValidator> registerValidator (
        new RegisterValidator(parser, fieldValidator, parameterValidator));
    QSharedPointer<AddressBlockValidator> addressBlockValidator (
        new AddressBlockValidator(parser, registerValidator, parameterValidator));
    QSharedPointer<MemoryMapBaseValidator> localMemoryMapValidator (
        new MemoryMapBaseValidator(parser, addressBlockValidator));
    AddressSpaceValidator validator(parser, localMemoryMapValidator, parameterValidator);
    QCOMPARE(validator.hasValidWidth(testSpace), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testSpace, "test");

        QString expectedError = QObject::tr("Invalid width set for address space %1 within %2")
            .arg(testSpace->name()).arg("test");
        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_AddressSpaceValidator::testHasValidWidth_data()
//-----------------------------------------------------------------------------
void tst_AddressSpaceValidator::testHasValidWidth_data()
{
    QTest::addColumn<QString>("width");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("1 is valid for width") << "1" << true;
    QTest::newRow("12*2+13-5 is valid for width") << "12*2+13-5" << true;
    QTest::newRow("0 is valid for width") << "0" << true;
    QTest::newRow("-20 is not valid for width") << "-20" << false;
    QTest::newRow("14-5*3 is not valid for width") << "14-5*3" << false;
    QTest::newRow("Text is not valid for width") << "text" << false;
    QTest::newRow("String is not valid for width") << "\"text\"" << false;
    QTest::newRow("Empty value is not valid for width") << "" << false;

    QTest::newRow("Long width is valid") << "40000000000" << true;
}

//-----------------------------------------------------------------------------
// Function: tst_AddressSpaceValidator::testSegmentHasValidName()
//-----------------------------------------------------------------------------
void tst_AddressSpaceValidator::testSegmentHasValidName()
{
    QFETCH(QString, name);
    QFETCH(bool, isValid);

    QSharedPointer<Segment> testSegment (new Segment(name, "10", "10"));

    QSharedPointer<AddressSpace> testSpace (new AddressSpace("testSpace", "100"));
    testSpace->getSegments()->append(testSegment);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator2014> parameterValidator (
        new ParameterValidator2014(parser, QSharedPointer<QList<QSharedPointer<Choice> > > ()));
    QSharedPointer<EnumeratedValueValidator> enumeratedValueValidator (new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator (
        new FieldValidator(parser, enumeratedValueValidator, parameterValidator));
    QSharedPointer<RegisterValidator> registerValidator (
        new RegisterValidator(parser, fieldValidator, parameterValidator));
    QSharedPointer<AddressBlockValidator> addressBlockValidator (
        new AddressBlockValidator(parser, registerValidator, parameterValidator));
    QSharedPointer<MemoryMapBaseValidator> localMemoryMapValidator (
        new MemoryMapBaseValidator(parser, addressBlockValidator));
    AddressSpaceValidator validator(parser, localMemoryMapValidator, parameterValidator);
    QCOMPARE(validator.hasValidSegments(testSpace), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testSpace, "test");

        QString expectedError = QObject::tr("Invalid name specified for segment %1 within address space %2")
            .arg(testSegment->name()).arg(testSpace->name());
        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_AddressSpaceValidator::testSegmentHasValidName_data()
//-----------------------------------------------------------------------------
void tst_AddressSpaceValidator::testSegmentHasValidName_data()
{
    QTest::addColumn<QString>("name");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Name test is valid") << "test" << true;
    QTest::newRow("Empty name is invalid") << "" << false;
    QTest::newRow("Name consisting of only white spaces is invalid") << "    " << false;
    QTest::newRow("Name consisting of characters and white spaces is valid") << "  test  " << true;
}

//-----------------------------------------------------------------------------
// Function: tst_AddressSpaceValidator::testSegmentsHaveUniqueNames()
//-----------------------------------------------------------------------------
void tst_AddressSpaceValidator::testSegmentsHaveUniqueNames()
{
    QSharedPointer<Segment> segmentOne (new Segment("segmentOne", "0", "2"));
    QSharedPointer<Segment> segmentTwo (new Segment(*segmentOne.data()));
    segmentTwo->setOffset("5");

    QSharedPointer<AddressSpace> testSpace (new AddressSpace("SpaceDandy", "10", "10"));
    testSpace->getSegments()->append(segmentOne);
    testSpace->getSegments()->append(segmentTwo);

    QSharedPointer<ExpressionParser> parser (new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator2014> parameterValidator (
        new ParameterValidator2014(parser, QSharedPointer<QList<QSharedPointer<Choice> > > ()));
    QSharedPointer<EnumeratedValueValidator> enumeratedValueValidator (new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator (
        new FieldValidator(parser, enumeratedValueValidator, parameterValidator));
    QSharedPointer<RegisterValidator> registerValidator (
        new RegisterValidator(parser, fieldValidator, parameterValidator));
    QSharedPointer<AddressBlockValidator> addressBlockValidator (
        new AddressBlockValidator(parser, registerValidator, parameterValidator));
    QSharedPointer<MemoryMapBaseValidator> localMemoryMapValidator (
        new MemoryMapBaseValidator(parser, addressBlockValidator));
    AddressSpaceValidator validator(parser, localMemoryMapValidator, parameterValidator);

    QCOMPARE(validator.hasValidSegments(testSpace), false);

    QVector<QString> foundErrors;
    validator.findErrorsIn(foundErrors, testSpace, "test");

    QString expectedError = QObject::tr("Name %1 of segments in address space %2 is not unique")
        .arg(segmentOne->name()).arg(testSpace->name());
    if (errorIsNotFoundInErrorList(expectedError, foundErrors))
    {
        QFAIL("No error message found");
    }

    segmentTwo->setName("segmentTwo");
    QCOMPARE(validator.hasValidSegments(testSpace), true);
}

//-----------------------------------------------------------------------------
// Function: tst_AddressSpaceValidator::testSegmentHasValidIsPresent()
//-----------------------------------------------------------------------------
void tst_AddressSpaceValidator::testSegmentHasValidIsPresent()
{
    QFETCH(QString, isPresent);
    QFETCH(bool, isValid);

    QSharedPointer<Segment> testSegment (new Segment("testSegment", "10", "10"));
    testSegment->setIsPresent(isPresent);

    QSharedPointer<AddressSpace> testSpace (new AddressSpace("TestAddressBlock", "100"));
    testSpace->getSegments()->append(testSegment);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator2014> parameterValidator (
        new ParameterValidator2014(parser, QSharedPointer<QList<QSharedPointer<Choice> > > ()));
    QSharedPointer<EnumeratedValueValidator> enumeratedValueValidator (new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator (
        new FieldValidator(parser, enumeratedValueValidator, parameterValidator));
    QSharedPointer<RegisterValidator> registerValidator (
        new RegisterValidator(parser, fieldValidator, parameterValidator));
    QSharedPointer<AddressBlockValidator> addressBlockValidator (
        new AddressBlockValidator(parser, registerValidator, parameterValidator));
    QSharedPointer<MemoryMapBaseValidator> localMemoryMapValidator (
        new MemoryMapBaseValidator(parser, addressBlockValidator));
    AddressSpaceValidator validator(parser, localMemoryMapValidator, parameterValidator);
    QCOMPARE(validator.hasValidSegments(testSpace), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testSpace, "test");

        QString expectedError = QObject::tr("Invalid isPresent set for segment %1 within address space %2")
            .arg(testSegment->name()).arg(testSpace->name());
        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_AddressSpaceValidator::testSegmentHasValidIsPresent_data()
//-----------------------------------------------------------------------------
void tst_AddressSpaceValidator::testSegmentHasValidIsPresent_data()
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
// Function: tst_AddressSpaceValidator::testSegmentHasValidAddressOffset()
//-----------------------------------------------------------------------------
void tst_AddressSpaceValidator::testSegmentHasValidAddressOffset()
{
    QFETCH(QString, addressOffset);
    QFETCH(bool, isValid);

    QSharedPointer<Segment> testSegment (new Segment("testSegment", addressOffset, "10"));

    QSharedPointer<AddressSpace> testSpace (new AddressSpace("TestAddressBlock", "100"));
    testSpace->getSegments()->append(testSegment);

    if (addressOffset == "40000000000")
    {
        testSpace->setRange("50000000000");
    }

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator2014> parameterValidator (
        new ParameterValidator2014(parser, QSharedPointer<QList<QSharedPointer<Choice> > > ()));
    QSharedPointer<EnumeratedValueValidator> enumeratedValueValidator (new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator (
        new FieldValidator(parser, enumeratedValueValidator, parameterValidator));
    QSharedPointer<RegisterValidator> registerValidator (
        new RegisterValidator(parser, fieldValidator, parameterValidator));
    QSharedPointer<AddressBlockValidator> addressBlockValidator (
        new AddressBlockValidator(parser, registerValidator, parameterValidator));
    QSharedPointer<MemoryMapBaseValidator> localMemoryMapValidator (
        new MemoryMapBaseValidator(parser, addressBlockValidator));
    AddressSpaceValidator validator(parser, localMemoryMapValidator, parameterValidator);
    QCOMPARE(validator.hasValidSegments(testSpace), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testSpace, "test");

        QString expectedError = QObject::tr("Invalid address offset set for segment %1 within address space %2")
            .arg(testSegment->name()).arg(testSpace->name());
        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_AddressSpaceValidator::testSegmentHasValidAddressOffset_data()
//-----------------------------------------------------------------------------
void tst_AddressSpaceValidator::testSegmentHasValidAddressOffset_data()
{
    QTest::addColumn<QString>("addressOffset");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("12*2+13-5 is valid for address offset") << "12*2+13-5" << true;
    QTest::newRow("0 is valid for address offset") << "0" << true;
    QTest::newRow("14-5*3 is not valid for address offset") << "14-5*3" << false;
    QTest::newRow("Text is not valid for address offset") << "text" << false;
    QTest::newRow("String is not valid for address offset") << "\"text\"" << false;
    QTest::newRow("Empty value is not valid for address offset") << "" << false;

    QTest::newRow("Long address offset is valid for segment") << "40000000000" << true;
}

//-----------------------------------------------------------------------------
// Function: tst_AddressSpaceValidator::testSegmentHasValidRange()
//-----------------------------------------------------------------------------
void tst_AddressSpaceValidator::testSegmentHasValidRange()
{
    QFETCH(QString, range);
    QFETCH(bool, isValid);

    QSharedPointer<Segment> testSegment (new Segment("testSegment", "10", range));

    QSharedPointer<AddressSpace> testSpace (new AddressSpace("TestAddressBlock", "100"));
    testSpace->getSegments()->append(testSegment);

    if (range == "40000000000")
    {
        testSpace->setRange("50000000000");
    }

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator2014> parameterValidator (
        new ParameterValidator2014(parser, QSharedPointer<QList<QSharedPointer<Choice> > > ()));
    QSharedPointer<EnumeratedValueValidator> enumeratedValueValidator (new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator (
        new FieldValidator(parser, enumeratedValueValidator, parameterValidator));
    QSharedPointer<RegisterValidator> registerValidator (
        new RegisterValidator(parser, fieldValidator, parameterValidator));
    QSharedPointer<AddressBlockValidator> addressBlockValidator (
        new AddressBlockValidator(parser, registerValidator, parameterValidator));
    QSharedPointer<MemoryMapBaseValidator> localMemoryMapValidator (
        new MemoryMapBaseValidator(parser, addressBlockValidator));
    AddressSpaceValidator validator(parser, localMemoryMapValidator, parameterValidator);
    QCOMPARE(validator.hasValidSegments(testSpace), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testSpace, "test");

        QString expectedError = QObject::tr("Invalid range set for segment %1 within address space %2")
            .arg(testSegment->name()).arg(testSpace->name());
        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_AddressSpaceValidator::testSegmentHasValidRange_data()
//-----------------------------------------------------------------------------
void tst_AddressSpaceValidator::testSegmentHasValidRange_data()
{
    QTest::addColumn<QString>("range");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("12*2+13-5 is valid for range") << "12*2+13-5" << true;
    QTest::newRow("0 is not valid for range") << "0" << false;
    QTest::newRow("14-5*3 is not valid for range") << "14-5*3" << false;
    QTest::newRow("Text is not valid for range") << "text" << false;
    QTest::newRow("String is not valid for range") << "\"text\"" << false;
    QTest::newRow("Empty value is not valid for range") << "" << false;

    QTest::newRow("Long range is valid for segment") << "40000000000" << true;
}

//-----------------------------------------------------------------------------
// Function: tst_AddressSpaceValidator::testSegmentsAreContainedWithinAddressSpace()
//-----------------------------------------------------------------------------
void tst_AddressSpaceValidator::testSegmentIsContainedWithinAddressSpace()
{
    QFETCH(QString, segmentOffset);
    QFETCH(QString, segmentRange);
    QFETCH(QString, spaceRange);
    QFETCH(bool, isValid);

    QSharedPointer<Segment> testSegment (new Segment("Saitama", segmentOffset, segmentRange));

    QSharedPointer<AddressSpace> testSpace (new AddressSpace("Wanpan", spaceRange));
    testSpace->getSegments()->append(testSegment);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator2014> parameterValidator (
        new ParameterValidator2014(parser, QSharedPointer<QList<QSharedPointer<Choice> > > ()));
    QSharedPointer<EnumeratedValueValidator> enumeratedValueValidator (new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator (
        new FieldValidator(parser, enumeratedValueValidator, parameterValidator));
    QSharedPointer<RegisterValidator> registerValidator (
        new RegisterValidator(parser, fieldValidator, parameterValidator));
    QSharedPointer<AddressBlockValidator> addressBlockValidator (
        new AddressBlockValidator(parser, registerValidator, parameterValidator));
    QSharedPointer<MemoryMapBaseValidator> localMemoryMapValidator (
        new MemoryMapBaseValidator(parser, addressBlockValidator));
    AddressSpaceValidator validator(parser, localMemoryMapValidator, parameterValidator);
    QCOMPARE(validator.hasValidSegments(testSpace), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testSpace, "test");

        QString expectedError = QObject::tr("Segment %1 is not contained within address space %2")
            .arg(testSegment->name()).arg(testSpace->name());

        if (!parser->isValidExpression(segmentOffset))
        {
            expectedError = QObject::tr("Invalid address offset set for segment %1 within address space %2").
                arg(testSegment->name()).arg(testSpace->name());
        }
        else if (!parser->isValidExpression(segmentRange))
        {
            expectedError = QObject::tr("Invalid range set for segment %1 within address space %2").
                arg(testSegment->name()).arg(testSpace->name());
        }

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_AddressSpaceValidator::testSegmentsAreContainedWithinAddressSpace_data()
//-----------------------------------------------------------------------------
void tst_AddressSpaceValidator::testSegmentIsContainedWithinAddressSpace_data()
{
    QTest::addColumn<QString>("segmentOffset");
    QTest::addColumn<QString>("segmentRange");
    QTest::addColumn<QString>("spaceRange");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Segment: offset = 0, range = 5 is within address space with range 5") << "0" << "5" << "5" <<
        true;
    QTest::newRow("Segment: offset = 0, range = 10 is not within address space with range 5") << "0" << "10" <<
        "5" << false;
    QTest::newRow("Segment: offset = 20, range = 1 is not within address space with range 5") << "20" << "1" <<
        "5" << false;

    QTest::newRow("Segment with a long offset is not contained within a small address space")
        << "4000000000000000000000" << "5" << "5" << false;
    QTest::newRow("Segment with a long range is not contained within a small address space")
        << "0" << "4000000000000000000000" << "5" << false;
}

//-----------------------------------------------------------------------------
// Function: tst_AddressSpaceValidator::testOverlappingSegments()
//-----------------------------------------------------------------------------
void tst_AddressSpaceValidator::testOverlappingSegments()
{
    QFETCH(QString, segmentOffsetOne);
    QFETCH(QString, segmentRangeOne);
    QFETCH(QString, segmentOffsetTwo);
    QFETCH(QString, segmentRangeTwo);
    QFETCH(bool, isValid);

    QSharedPointer<Segment> segmentOne (new Segment("Mugen", segmentOffsetOne, segmentRangeOne));
    QSharedPointer<Segment> segmentTwo (new Segment("Jin", segmentOffsetTwo, segmentRangeTwo));

    QSharedPointer<AddressSpace> testSpace (new AddressSpace("SamuraiChamploo", "100"));
    testSpace->getSegments()->append(segmentOne);
    testSpace->getSegments()->append(segmentTwo);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator2014> parameterValidator (
        new ParameterValidator2014(parser, QSharedPointer<QList<QSharedPointer<Choice> > > ()));
    QSharedPointer<EnumeratedValueValidator> enumeratedValueValidator (new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator (
        new FieldValidator(parser, enumeratedValueValidator, parameterValidator));
    QSharedPointer<RegisterValidator> registerValidator (
        new RegisterValidator(parser, fieldValidator, parameterValidator));
    QSharedPointer<AddressBlockValidator> addressBlockValidator (
        new AddressBlockValidator(parser, registerValidator, parameterValidator));
    QSharedPointer<MemoryMapBaseValidator> localMemoryMapValidator (
        new MemoryMapBaseValidator(parser, addressBlockValidator));
    AddressSpaceValidator validator(parser, localMemoryMapValidator, parameterValidator);
    QCOMPARE(validator.hasValidSegments(testSpace), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testSpace, "test");

        QString firstName = segmentOne->name();
        QString secondName = segmentTwo->name();

        if (parser->parseExpression(segmentOffsetOne).toInt() >= parser->parseExpression(segmentOffsetTwo).toInt())
        {
            firstName = segmentTwo->name();
            secondName = segmentOne->name();
        }

        QString expectedError = QObject::tr("Segments %1 and %2 overlap within address space %3")
            .arg(firstName).arg(secondName).arg(testSpace->name());
        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_AddressSpaceValidator::testOverlappingSegments_data()
//-----------------------------------------------------------------------------
void tst_AddressSpaceValidator::testOverlappingSegments_data()
{
    QTest::addColumn<QString>("segmentOffsetOne");
    QTest::addColumn<QString>("segmentRangeOne");
    QTest::addColumn<QString>("segmentOffsetTwo");
    QTest::addColumn<QString>("segmentRangeTwo");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Segment1: offset=0, range=2; Segment2: offset=5, range=2 do not overlap") <<
        "0" << "2" << "5" << "2" << true;
    QTest::newRow("Segment1: offset=0, range=8; Segment2: offset=5, range=6 overlap") <<
        "0" << "8" << "5" << "6" << false;
    QTest::newRow("Segment1: offset=8, range=4; Segment2: offset=5, range=6 overlap") <<
        "8" << "4" << "5" << "6" << false;
    QTest::newRow("Segment1: offset=8, range=1; Segment2: offset=5, range=6 overlap") <<
        "8" << "1" << "5" << "6" << false;

    QTest::newRow("Segment1: offset=5, range=6; Segment2: offset=0, range=8 overlap") <<
        "5" << "6" << "0" << "8" << false;
    QTest::newRow("Segment1: offset=5, range=6; Segment2: offset=8, range=4 overlap") <<
        "5" << "6" << "8" << "4" << false;
    QTest::newRow("Segment1: offset=5, range=6; Segment2: offset=8, range=1 overlap") <<
        "5" << "6" << "8" << "1" << false;
    QTest::newRow("Segment1: offset=5, range=2; Segment2: offset=0, range=2 do not overlap") <<
        "5" << "2" << "0" << "2" << true;
}

//-----------------------------------------------------------------------------
// Function: tst_AddressSpaceValidator::testHasValidAddressUnitBits()
//-----------------------------------------------------------------------------
void tst_AddressSpaceValidator::testHasValidAddressUnitBits()
{
    QFETCH(QString, addressUnitBits);
    QFETCH(bool, isValid);

    QSharedPointer<AddressSpace> testSpace (new AddressSpace("TestAddressBlock"));
    testSpace->setAddressUnitBits(addressUnitBits);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator2014> parameterValidator (
        new ParameterValidator2014(parser, QSharedPointer<QList<QSharedPointer<Choice> > > ()));
    QSharedPointer<EnumeratedValueValidator> enumeratedValueValidator (new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator (
        new FieldValidator(parser, enumeratedValueValidator, parameterValidator));
    QSharedPointer<RegisterValidator> registerValidator (
        new RegisterValidator(parser, fieldValidator, parameterValidator));
    QSharedPointer<AddressBlockValidator> addressBlockValidator (
        new AddressBlockValidator(parser, registerValidator, parameterValidator));
    QSharedPointer<MemoryMapBaseValidator> localMemoryMapValidator (
        new MemoryMapBaseValidator(parser, addressBlockValidator));
    AddressSpaceValidator validator(parser, localMemoryMapValidator, parameterValidator);
    QCOMPARE(validator.hasValidAddressUnitBits(testSpace), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testSpace, "test");

        QString expectedError = QObject::tr("Invalid address unit bits set for address space %1 within %2")
            .arg(testSpace->name()).arg("test");
        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_AddressSpaceValidator::testHasValidAddressUnitBits_data()
//-----------------------------------------------------------------------------
void tst_AddressSpaceValidator::testHasValidAddressUnitBits_data()
{
    QTest::addColumn<QString>("addressUnitBits");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("12*2+13-5 is valid for address unit bits") << "12*2+13-5" << true;
    QTest::newRow("0 is not valid for address unit bits") << "0" << false;
    QTest::newRow("14-5*3 is not valid for address unit bits") << "14-5*3" << false;
    QTest::newRow("Text is not valid for address unit bits") << "text" << false;
    QTest::newRow("String is not valid for address unit bits") << "\"text\"" << false;
    QTest::newRow("Empty value is valid for address unit bits") << "" << true;

    QTest::newRow("Long address unit bits value is valid") << "40000000000" << true;
}

//-----------------------------------------------------------------------------
// Function: tst_AddressSpaceValidator::testHasValidLocalMemoryMap()
//-----------------------------------------------------------------------------
void tst_AddressSpaceValidator::testHasValidLocalMemoryMap()
{
    QSharedPointer<MemoryMapBase> testLocalMap (new MemoryMapBase(""));

    QSharedPointer<AddressSpace> testSpace (new AddressSpace("testSpace"));
    testSpace->setLocalMemoryMap(testLocalMap);

    QSharedPointer<ExpressionParser> parser (new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator2014> parameterValidator (
        new ParameterValidator2014(parser, QSharedPointer<QList<QSharedPointer<Choice> > > ()));
    QSharedPointer<EnumeratedValueValidator> enumeratedValueValidator (new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator (
        new FieldValidator(parser, enumeratedValueValidator, parameterValidator));
    QSharedPointer<RegisterValidator> registerValidator (
        new RegisterValidator(parser, fieldValidator, parameterValidator));
    QSharedPointer<AddressBlockValidator> addressBlockValidator (
        new AddressBlockValidator(parser, registerValidator, parameterValidator));
    QSharedPointer<MemoryMapBaseValidator> localMemoryMapValidator (
        new MemoryMapBaseValidator(parser, addressBlockValidator));
    AddressSpaceValidator validator(parser, localMemoryMapValidator, parameterValidator);
    QCOMPARE(validator.hasValidLocalMemoryMap(testSpace), false);

    QVector<QString> foundErrors;
    validator.findErrorsIn(foundErrors, testSpace, "test");

    QString expectedError = QObject::tr("Invalid name specified for memory map %1 within address space %2").
        arg(testLocalMap->name()).arg(testSpace->name());
    if (errorIsNotFoundInErrorList(expectedError, foundErrors))
    {
        QFAIL("No error message found");
    }

    testLocalMap->setName("testMap");
    QCOMPARE(validator.hasValidLocalMemoryMap(testSpace), true);
}

//-----------------------------------------------------------------------------
// Function: tst_AddressSpaceValidator::testHasValidParameters()
//-----------------------------------------------------------------------------
void tst_AddressSpaceValidator::testHasValidParameters()
{
    QSharedPointer<Parameter> testParameter (new Parameter());
    testParameter->setValueId("Sanger");
    testParameter->setName("Daizengar");
    testParameter->setValue("");
    testParameter->setType("int");

    QSharedPointer<AddressSpace> testSpace (new AddressSpace("testSpace"));
    testSpace->getParameters()->append(testParameter);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator2014> parameterValidator (
        new ParameterValidator2014(parser, QSharedPointer<QList<QSharedPointer<Choice> > > ()));
    QSharedPointer<EnumeratedValueValidator> enumeratedValueValidator (new EnumeratedValueValidator(parser));
    QSharedPointer<FieldValidator> fieldValidator (
        new FieldValidator(parser, enumeratedValueValidator, parameterValidator));
    QSharedPointer<RegisterValidator> registerValidator (
        new RegisterValidator(parser, fieldValidator, parameterValidator));
    QSharedPointer<AddressBlockValidator> addressBlockValidator (
        new AddressBlockValidator(parser, registerValidator, parameterValidator));
    QSharedPointer<MemoryMapBaseValidator> localMemoryMapValidator (
        new MemoryMapBaseValidator(parser, addressBlockValidator));
    AddressSpaceValidator validator(parser, localMemoryMapValidator, parameterValidator);

    QCOMPARE(validator.hasValidParameters(testSpace), false);

    QVector<QString> errorsFound;
    validator.findErrorsIn(errorsFound, testSpace, "test");

    QString expectedError = QObject::tr("No value specified for %1 %2 within address space %3").
        arg(testParameter->elementName()).arg(testParameter->name()).arg(testSpace->name());

    if (errorIsNotFoundInErrorList(expectedError, errorsFound))
    {
        QFAIL("Error was not found");
    }

    testParameter->setValue("20");
    QCOMPARE(validator.hasValidParameters(testSpace), true);

    QSharedPointer<Parameter> otherParameter (new Parameter(*testParameter.data()));
    otherParameter->setValue("2");
    testSpace->getParameters()->append(otherParameter);

    QCOMPARE(validator.hasValidParameters(testSpace), false);

    errorsFound.clear();
    validator.findErrorsIn(errorsFound, testSpace, "test");
    expectedError = QObject::tr("Name %1 of parameters in address space %2 is not unique.")
        .arg(otherParameter->name()).arg(testSpace->name());
    if (errorIsNotFoundInErrorList(expectedError, errorsFound))
    {
        QFAIL("Error was not found");
    }
}

//-----------------------------------------------------------------------------
// Function: tst_AddressSpaceValidator::errorIsNotFoundInErrorList()
//-----------------------------------------------------------------------------
bool tst_AddressSpaceValidator::errorIsNotFoundInErrorList(QString const& expectedError,
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

QTEST_APPLESS_MAIN(tst_AddressSpaceValidator)

#include "tst_AddressSpaceValidator.moc"
