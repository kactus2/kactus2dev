//-----------------------------------------------------------------------------
// File: tst_CPUValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 18.11.2015
//
// Description:
// Unit test for class CPUValidator.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/AddressSpace.h>
#include <IPXACTmodels/Component/Cpu.h>
#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/Component/validators/CPUValidator.h>

#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/common/validators/ParameterValidator.h>

#include <KactusAPI/include/SystemVerilogExpressionParser.h>

#include <QDebug>
#include <QtTest>

class tst_CPUValidator : public QObject
{
    Q_OBJECT

public:
    tst_CPUValidator();

private slots:
	void baseCase();
    void testNameIsNotEmpty();
	void noRef();
    void testIllegalAddressSpaceReference();
    void testMemoryMapReference2022();
    void testMemoryMapReference2022_data();

    void testRange2022();
    void testRange2022_data();

    void testWidth2022();
    void testWidth2022_data();

    void testAddressUnitBits2022();
    void testAddressUnitBits2022_data();

    void testRegionHasValidName2022();
    void testRegionHasValidName2022_data();
    void testRegionsHaveUniqueNames2022();
    void testRegionHasValidAddressOffset2022();
    void testRegionHasValidAddressOffset2022_data();
    void testRegionHasValidRange2022();
    void testRegionHasValidRange2022_data();
    void testRegionIsContainedWithinAddressSpace2022();
    void testRegionIsContainedWithinAddressSpace2022_data();
    void testOverlappingRegions2022();
    void testOverlappingRegions2022_data();

    void failParameter();
	void failPresense();
	void failRefPresense();

private:
    bool errorIsNotFoundInErrorList(QString const& expectedError, QVector<QString> errorList);
};

//-----------------------------------------------------------------------------
// Function: tst_CPUValidator::tst_CPUValidator()
//-----------------------------------------------------------------------------
tst_CPUValidator::tst_CPUValidator()
{

}

//-----------------------------------------------------------------------------
// Function: tst_CPUValidator::baseCase()
//-----------------------------------------------------------------------------
void tst_CPUValidator::baseCase()
{
	QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator> parameterValidator(new ParameterValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > >()));

    QSharedPointer<QList<QSharedPointer<AddressSpace> > > singleSpace(new QList<QSharedPointer<AddressSpace> >());
    singleSpace->append(QSharedPointer<AddressSpace>(new AddressSpace("testSpace")));

    QSharedPointer<QList<QSharedPointer<MemoryMap> > > noMaps;

    CPUValidator validator(parameterValidator, parser, singleSpace, noMaps, Document::Revision::Std14);
	
    QSharedPointer<Cpu> cpu(new Cpu());
    cpu->setName("esa");
	cpu->setIsPresent("1");

	QSharedPointer<Parameter> parameter (new Parameter());
	parameter->setName("param");
	parameter->setType("bit");
	parameter->setValue("3'b111");
	parameter->setValueId("parameterid");

	cpu->getParameters()->append(parameter);

	QSharedPointer<Cpu::AddressSpaceRef> aref(new Cpu::AddressSpaceRef("testSpace"));
	aref->setIsPresent("0");
	cpu->getAddressSpaceReferences()->append( aref );

	QVector<QString> errorList;
	validator.findErrorsIn(errorList, cpu, "test");

	QVERIFY(validator.validate(cpu));
	QCOMPARE(errorList.size(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_CPUValidator::testNameIsNotEmpty()
//-----------------------------------------------------------------------------
void tst_CPUValidator::testNameIsNotEmpty()
{
    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator> parameterValidator(new ParameterValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > >()));
    QSharedPointer<QList<QSharedPointer<AddressSpace> > > noSpaces;
    QSharedPointer<QList<QSharedPointer<MemoryMap> > > noMaps;

    CPUValidator validator(parameterValidator, parser, noSpaces, noMaps, Document::Revision::Std14);

    QSharedPointer<Cpu> cpu( new Cpu() );
    cpu->setName("");

    QVector<QString> errorList;
    validator.findErrorsIn(errorList, cpu, "test");

    QVERIFY(!validator.validate(cpu));
    QVERIFY(errorList.size() > 0);
    QCOMPARE(errorList.first(), QString("Invalid name '' set for CPU within test."));
}

//-----------------------------------------------------------------------------
// Function: tst_CPUValidator::noRef()
//-----------------------------------------------------------------------------
void tst_CPUValidator::noRef()
{
    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator> parameterValidator(new ParameterValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > >()));
    QSharedPointer<QList<QSharedPointer<AddressSpace> > > noSpaces;
    QSharedPointer<QList<QSharedPointer<MemoryMap> > > noMaps;

    CPUValidator validator(parameterValidator, parser, noSpaces, noMaps, Document::Revision::Std14);

    QSharedPointer<Cpu> cpu( new Cpu() );
    cpu->setName("testCpu");

	QVector<QString> errorList;
	validator.findErrorsIn(errorList, cpu, "test");

	QVERIFY(!validator.validate(cpu));
	QVERIFY(errorList.size() > 0);
    QCOMPARE(errorList.first(), QString("No address space reference set for CPU testCpu within test."));
}

//-----------------------------------------------------------------------------
// Function: tst_CPUValidator::testIllegalAddressSpaceReference()
//-----------------------------------------------------------------------------
void tst_CPUValidator::testIllegalAddressSpaceReference()
{
    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator> parameterValidator(new ParameterValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > >()));
    QSharedPointer<QList<QSharedPointer<AddressSpace> > > noSpaces;
    QSharedPointer<QList<QSharedPointer<MemoryMap> > > noMaps;

    CPUValidator validator(parameterValidator, parser, noSpaces, noMaps, Document::Revision::Std14);

    QSharedPointer<Cpu> cpu(new Cpu());
    cpu->setName("testCpu");

    QSharedPointer<Cpu::AddressSpaceRef> aref(new Cpu::AddressSpaceRef("nonExistingSpace") );
    cpu->getAddressSpaceReferences()->append(aref);

    QVector<QString> errorList;
    validator.findErrorsIn(errorList, cpu, "test");

    QVERIFY(!validator.validate(cpu));
    QCOMPARE(errorList.size(), 1);
    QCOMPARE(errorList.first(), QString("Address space 'nonExistingSpace' referenced within CPU testCpu is not found."));
}

//-----------------------------------------------------------------------------
// Function: tst_CPUValidator::testMemoryMapReference()
//-----------------------------------------------------------------------------
void tst_CPUValidator::testMemoryMapReference2022()
{
    QFETCH(QString, memMapRef);
    QFETCH(bool, isValid);
    QFETCH(QString, expectedError);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator> parameterValidator(new ParameterValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > >()));
    QSharedPointer<QList<QSharedPointer<AddressSpace> > > noSpaces;

    QSharedPointer<QList<QSharedPointer<MemoryMap> > > maps(new QList<QSharedPointer<MemoryMap> >());
    auto testMap = QSharedPointer<MemoryMap>(new MemoryMap("testMap"));
    maps->append(testMap);

    CPUValidator validator(parameterValidator, parser, noSpaces, maps, Document::Revision::Std22);

    QSharedPointer<Cpu> cpu(new Cpu("testCpu"));
    cpu->setMemoryMapReference(memMapRef);

    cpu->setRange("1");
    cpu->setWidth("1");

    QVector<QString> errorList;
    validator.findErrorsIn(errorList, cpu, "test");

    QCOMPARE(validator.hasValidMemoryMapReference(cpu), isValid);

    if (expectedError.isEmpty() == false)
    {
        QCOMPARE(errorList.size(), 1);
        QCOMPARE(errorList.first(), expectedError);
    }
}

//-----------------------------------------------------------------------------
// Function: tst_CPUValidator::testMemoryMapReference2022_data()
//-----------------------------------------------------------------------------
void tst_CPUValidator::testMemoryMapReference2022_data()
{
    QTest::addColumn<QString>("memMapRef");
    QTest::addColumn<bool>("isValid");
    QTest::addColumn<QString>("expectedError");

    QTest::newRow("Empty memory map reference is invalid") << "" << false << 
        "No memory map reference set for CPU testCpu within test.";

    QTest::newRow("Non-existing memory map reference is invalid") << "noMap" << false <<
        "Memory map 'noMap' referenced within CPU testCpu is not found.";

    QTest::newRow("Existing memory map reference is valid") << "testMap" << true <<  "";
}

//-----------------------------------------------------------------------------
// Function: tst_CPUValidator::testRange2022()
//-----------------------------------------------------------------------------
void tst_CPUValidator::testRange2022()
{
    QFETCH(QString, range);
    QFETCH(bool, isValid);
    QFETCH(QString, expectedError);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator> parameterValidator(new ParameterValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > >()));
    QSharedPointer<QList<QSharedPointer<AddressSpace> > > noSpaces;

    QSharedPointer<QList<QSharedPointer<MemoryMap> > > maps(new QList<QSharedPointer<MemoryMap> >());
    auto testMap = QSharedPointer<MemoryMap>(new MemoryMap("testMap"));
    maps->append(testMap);

    CPUValidator validator(parameterValidator, parser, noSpaces, maps, Document::Revision::Std22);

    QSharedPointer<Cpu> cpu(new Cpu("testCpu"));
    cpu->setMemoryMapReference("testMap");
    cpu->setRange(range);
    
    cpu->setWidth("1");

    QVector<QString> errorList;
    validator.findErrorsIn(errorList, cpu, "test");

    QCOMPARE(validator.hasValidRange(cpu), isValid);

    if (expectedError.isEmpty() == false)
    {
        QCOMPARE(errorList.size(), 1);
        QCOMPARE(errorList.first(), expectedError);
    }
}

//-----------------------------------------------------------------------------
// Function: tst_CPUValidator::testRange2022_data()
//-----------------------------------------------------------------------------
void tst_CPUValidator::testRange2022_data()
{
    QTest::addColumn<QString>("range");
    QTest::addColumn<bool>("isValid");
    QTest::addColumn<QString>("expectedError");

    QTest::newRow("Empty range is invalid") << "" << false <<
        "Invalid range set for CPU testCpu.";

    QTest::newRow("Text as range is not valid") << "Six" << false <<
        "Invalid range set for CPU testCpu.";

    QTest::newRow("Constant integer as range is valid") << "8" << true << "";

    QTest::newRow("Expression as range is valid") << "2*4" << true << "";

    QTest::newRow("Invalid expression as range is invalid") << "2+^^^^" << false <<
        "Invalid range set for CPU testCpu.";
}

//-----------------------------------------------------------------------------
// Function: tst_CPUValidator::testWidth2022()
//-----------------------------------------------------------------------------
void tst_CPUValidator::testWidth2022()
{
    QFETCH(QString, width);
    QFETCH(bool, isValid);
    QFETCH(QString, expectedError);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator> parameterValidator(new ParameterValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > >()));
    QSharedPointer<QList<QSharedPointer<AddressSpace> > > noSpaces;

    QSharedPointer<QList<QSharedPointer<MemoryMap> > > maps(new QList<QSharedPointer<MemoryMap> >());
    auto testMap = QSharedPointer<MemoryMap>(new MemoryMap("testMap"));
    maps->append(testMap);

    CPUValidator validator(parameterValidator, parser, noSpaces, maps, Document::Revision::Std22);

    QSharedPointer<Cpu> cpu(new Cpu("testCpu"));
    cpu->setMemoryMapReference("testMap");
    cpu->setWidth(width);

    cpu->setRange("1");

    QVector<QString> errorList;
    validator.findErrorsIn(errorList, cpu, "test");

    QCOMPARE(validator.hasValidWidth(cpu), isValid);

    if (expectedError.isEmpty() == false)
    {
        QCOMPARE(errorList.size(), 1);
        QCOMPARE(errorList.first(), expectedError);
    }
}

//-----------------------------------------------------------------------------
// Function: tst_CPUValidator::testWidth2022_data()
//-----------------------------------------------------------------------------
void tst_CPUValidator::testWidth2022_data()
{
    QTest::addColumn<QString>("width");
    QTest::addColumn<bool>("isValid");
    QTest::addColumn<QString>("expectedError");

    QTest::newRow("Empty width is invalid") << "" << false <<
        "Invalid width set for CPU testCpu.";

    QTest::newRow("Text as width is not valid") << "Six" << false <<
        "Invalid width set for CPU testCpu.";

    QTest::newRow("Constant integer as width is valid") << "8" << true << "";

    QTest::newRow("Expression as width is valid") << "2*4" << true << "";

    QTest::newRow("Invalid expression as width is invalid") << "2+^^^^" << false <<
        "Invalid width set for CPU testCpu.";
}

//-----------------------------------------------------------------------------
// Function: tst_CPUValidator::testAddressUnitBits2022()
//-----------------------------------------------------------------------------
void tst_CPUValidator::testAddressUnitBits2022()
{
    QFETCH(QString, aub);
    QFETCH(bool, isValid);
    QFETCH(QString, expectedError);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator> parameterValidator(new ParameterValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > >()));
    QSharedPointer<QList<QSharedPointer<AddressSpace> > > noSpaces;

    QSharedPointer<QList<QSharedPointer<MemoryMap> > > maps(new QList<QSharedPointer<MemoryMap> >());
    auto testMap = QSharedPointer<MemoryMap>(new MemoryMap("testMap"));
    maps->append(testMap);

    CPUValidator validator(parameterValidator, parser, noSpaces, maps, Document::Revision::Std22);

    QSharedPointer<Cpu> cpu(new Cpu("testCpu"));
    cpu->setMemoryMapReference("testMap");
    cpu->setAddressUnitBits(aub);

    cpu->setRange("1");
    cpu->setWidth("1");

    QVector<QString> errorList;
    validator.findErrorsIn(errorList, cpu, "test");

    QCOMPARE(validator.hasValidAddressUnitBits(cpu), isValid);

    if (expectedError.isEmpty() == false)
    {
        QCOMPARE(errorList.size(), 1);
        QCOMPARE(errorList.first(), expectedError);
    }
}

//-----------------------------------------------------------------------------
// Function: tst_CPUValidator::testAddressUnitBits2022_data()
//-----------------------------------------------------------------------------
void tst_CPUValidator::testAddressUnitBits2022_data()
{
    QTest::addColumn<QString>("aub");
    QTest::addColumn<bool>("isValid");
    QTest::addColumn<QString>("expectedError");

    QTest::newRow("Empty address unit bits is valid") << "" << true <<
        "";

    QTest::newRow("Text as address unit bits is not valid") << "Six" << false <<
        "Invalid address unit bits set for CPU testCpu.";

    QTest::newRow("Constant integer as address unit bits is valid") << "8" << true << "";

    QTest::newRow("Expression as address unit bits is valid") << "2*4" << true << "";

    QTest::newRow("Invalid expression as address unit bits is invalid") << "2+^^^^" << false <<
        "Invalid address unit bits set for CPU testCpu.";
}

//-----------------------------------------------------------------------------
// Function: tst_CPUValidator::testRegionHasValidName2022()
//-----------------------------------------------------------------------------
void tst_CPUValidator::testRegionHasValidName2022()
{
    QFETCH(QString, name);
    QFETCH(bool, isValid);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());

    RegionValidator validator(parser);

    QSharedPointer<Region> testRegion(new Region(name, "10", "10"));

    QCOMPARE(validator.hasValidName(testRegion->name()), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testRegion, "CPU testCpu");

        QString expectedError = QString("Invalid name specified for region %1 in CPU testCpu.")
            .arg(testRegion->name());
        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_CPUValidator::testRegionHasValidName2022_data()
//-----------------------------------------------------------------------------
void tst_CPUValidator::testRegionHasValidName2022_data()
{
    QTest::addColumn<QString>("name");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Empty name is invalid") << "" << false;
    QTest::newRow("Name test is valid") << "test" << true;
    QTest::newRow("Name consisting of only white spaces is invalid") << "    " << false;
    QTest::newRow("Name consisting of characters and white spaces is valid") << "  test  " << true;
}

//-----------------------------------------------------------------------------
// Function: tst_CPUValidator::testRegionsHaveUniqueNames2022()
//-----------------------------------------------------------------------------
void tst_CPUValidator::testRegionsHaveUniqueNames2022()
{
    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator> parameterValidator(new ParameterValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > >()));
    QSharedPointer<QList<QSharedPointer<AddressSpace> > > noSpaces;

    QSharedPointer<QList<QSharedPointer<MemoryMap> > > noMaps;

    CPUValidator validator(parameterValidator, parser, noSpaces, noMaps, Document::Revision::Std22);

    QSharedPointer<Region> firstRegion(new Region("firstRegion", "0", "2"));
    QSharedPointer<Region> secondRegion(new Region("firstRegion", "5", "1"));

    QSharedPointer<Cpu> testCpu(new Cpu("testCpu"));
    testCpu->setRange("100");
    testCpu->getRegions()->append(firstRegion);
    testCpu->getRegions()->append(secondRegion);

    QCOMPARE(validator.hasValidRegions(testCpu), false);

    QVector<QString> foundErrors;
    validator.findErrorsIn(foundErrors, testCpu, "CPU testCpu");

    QString expectedError = QString("Name %1 of regions in CPU testCpu is not unique.").arg(firstRegion->name());

    if (errorIsNotFoundInErrorList(expectedError, foundErrors))
    {
        QFAIL("No error message found");
    }

    secondRegion->setName("segmentTwo");
    QCOMPARE(validator.hasValidRegions(testCpu), true);
}

//-----------------------------------------------------------------------------
// Function: tst_CPUValidator::testRegionHasValidAddressOffset2022()
//-----------------------------------------------------------------------------
void tst_CPUValidator::testRegionHasValidAddressOffset2022()
{
    QFETCH(QString, addressOffset);
    QFETCH(bool, isValid);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());

    RegionValidator validator(parser);

    QSharedPointer<Region> testRegion(new Region("testRegion", addressOffset, "10"));

    QCOMPARE(validator.hasValidAddressOffset(testRegion), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testRegion, "CPU testCpu");

        QString expectedError = QString("Invalid address offset set for region %1 in CPU testCpu.").arg(
            testRegion->name());
        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_CPUValidator::testRegionHasValidAddressOffset2022_data()
//-----------------------------------------------------------------------------
void tst_CPUValidator::testRegionHasValidAddressOffset2022_data()
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
// Function: tst_CPUValidator::testRegionHasValidRange2022()
//-----------------------------------------------------------------------------
void tst_CPUValidator::testRegionHasValidRange2022()
{
    QFETCH(QString, range);
    QFETCH(bool, isValid);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());

    RegionValidator validator(parser);

    QSharedPointer<Region> testRegion(new Region("testRegion", "10", range));

    QCOMPARE(validator.hasValidRange(testRegion), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testRegion, "CPU testCpu");

        QString expectedError = QString("Invalid range set for region %1 in CPU testCpu.").arg(testRegion->name());
        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_CPUValidator::testRegionHasValidRange2022_data()
//-----------------------------------------------------------------------------
void tst_CPUValidator::testRegionHasValidRange2022_data()
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
// Function: tst_CPUValidator::testRegionIsContainedWithinAddressSpace2022()
//-----------------------------------------------------------------------------
void tst_CPUValidator::testRegionIsContainedWithinAddressSpace2022()
{
    QFETCH(QString, segmentOffset);
    QFETCH(QString, segmentRange);
    QFETCH(QString, spaceRange);
    QFETCH(bool, isValid);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator> parameterValidator(new ParameterValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > >()));
    QSharedPointer<QList<QSharedPointer<AddressSpace> > > noSpaces;

    QSharedPointer<QList<QSharedPointer<MemoryMap> > > noMaps;

    CPUValidator validator(parameterValidator, parser, noSpaces, noMaps, Document::Revision::Std22);

    QSharedPointer<Region> testRegion(new Region("region1", segmentOffset, segmentRange));

    QSharedPointer<Cpu> testSpace(new Cpu("testCpu"));
    testSpace->setRange(spaceRange);
    testSpace->getRegions()->append(testRegion);

    QCOMPARE(validator.hasValidRegions(testSpace), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testSpace, "CPU testCpu");

        QString expectedError = "Region region1 is not contained within address space of CPU testCpu.";

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_CPUValidator::testRegionIsContainedWithinAddressSpace2022_data()
//-----------------------------------------------------------------------------
void tst_CPUValidator::testRegionIsContainedWithinAddressSpace2022_data()
{
    QTest::addColumn<QString>("segmentOffset");
    QTest::addColumn<QString>("segmentRange");
    QTest::addColumn<QString>("spaceRange");

    QTest::addColumn<bool>("isValid");

    QTest::newRow("Region: offset = 0, range = 5 is within address space with range 5") << "0" << "5" << "5" <<
        true;
    QTest::newRow("Region: offset = 0, range = 10 is not within address space with range 5") << "0" << "10" <<
        "5" << false;
    QTest::newRow("Region: offset = 20, range = 1 is not within address space with range 5") << "20" << "1" <<
        "5" << false;

    QTest::newRow("Region with a long offset is not contained within a small address space")
        << "4000000000000000000000" << "5" << "5" << false;
    QTest::newRow("Region with a long range is not contained within a small address space")
        << "0" << "4000000000000000000000" << "5" << false;
}

//-----------------------------------------------------------------------------
// Function: tst_CPUValidator::testOverlappingRegions2022()
//-----------------------------------------------------------------------------
void tst_CPUValidator::testOverlappingRegions2022()
{
    QFETCH(QString, segmentOffsetOne);
    QFETCH(QString, segmentRangeOne);
    QFETCH(QString, segmentOffsetTwo);
    QFETCH(QString, segmentRangeTwo);
    QFETCH(bool, isValid);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator> parameterValidator(new ParameterValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > >()));
    QSharedPointer<QList<QSharedPointer<AddressSpace> > > noSpaces;

    QSharedPointer<QList<QSharedPointer<MemoryMap> > > noMaps;

    CPUValidator validator(parameterValidator, parser, noSpaces, noMaps, Document::Revision::Std22);

    QSharedPointer<Region> segmentOne(new Region("region1", segmentOffsetOne, segmentRangeOne));
    QSharedPointer<Region> segmentTwo(new Region("region2", segmentOffsetTwo, segmentRangeTwo));

    QSharedPointer<Cpu> testCpu(new Cpu("testCpu"));
    testCpu->setRange("100");
    testCpu->getRegions()->append(segmentOne);
    testCpu->getRegions()->append(segmentTwo);

    QCOMPARE(validator.hasValidRegions(testCpu), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testCpu, "CPU testCpu");

        QString firstName = segmentOne->name();
        QString secondName = segmentTwo->name();

        if (segmentOffsetOne.toInt() >= segmentOffsetTwo.toInt())
        {
            firstName = segmentTwo->name();
            secondName = segmentOne->name();
        }

        QString expectedError = QString("Regions %1 and %2 overlap within address space of CPU testCpu").arg(
            firstName, secondName);
        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_CPUValidator::testOverlappingRegions2022_data()
//-----------------------------------------------------------------------------
void tst_CPUValidator::testOverlappingRegions2022_data()
{
    QTest::addColumn<QString>("segmentOffsetOne");
    QTest::addColumn<QString>("segmentRangeOne");
    QTest::addColumn<QString>("segmentOffsetTwo");
    QTest::addColumn<QString>("segmentRangeTwo");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Region1: offset=0, range=2; Region2: offset=5, range=2 do not overlap") <<
        "0" << "2" << "5" << "2" << true;
    QTest::newRow("Region1: offset=0, range=8; Region2: offset=5, range=6 overlap") <<
        "0" << "8" << "5" << "6" << false;
    QTest::newRow("Region1: offset=8, range=4; Region2: offset=5, range=6 overlap") <<
        "8" << "4" << "5" << "6" << false;
    QTest::newRow("Region1: offset=8, range=1; Region2: offset=5, range=6 overlap") <<
        "8" << "1" << "5" << "6" << false;

    QTest::newRow("Region1: offset=5, range=6; Region2: offset=0, range=8 overlap") <<
        "5" << "6" << "0" << "8" << false;
    QTest::newRow("Region1: offset=5, range=6; Region2: offset=8, range=4 overlap") <<
        "5" << "6" << "8" << "4" << false;
    QTest::newRow("Region1: offset=5, range=6; Region2: offset=8, range=1 overlap") <<
        "5" << "6" << "8" << "1" << false;
    QTest::newRow("Region1: offset=5, range=2; Region2: offset=0, range=2 do not overlap") <<
        "5" << "2" << "0" << "2" << true;
}

//-----------------------------------------------------------------------------
// Function: tst_CPUValidator::failParameter()
//-----------------------------------------------------------------------------
void tst_CPUValidator::failParameter()
{
    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator> parameterValidator(new ParameterValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > >()));

    QSharedPointer<QList<QSharedPointer<AddressSpace> > > singleSpace(new QList<QSharedPointer<AddressSpace> >());
    singleSpace->append(QSharedPointer<AddressSpace>(new AddressSpace("testSpace")));
    QSharedPointer<QList<QSharedPointer<MemoryMap> > > noMaps;

    CPUValidator validator(parameterValidator, parser, singleSpace, noMaps, Document::Revision::Std14);

    QSharedPointer<Cpu> cpu( new Cpu());
    cpu->setName("testCpu");

	QSharedPointer<Parameter> parameter (new Parameter());
	parameter->setName("testParameter");
	parameter->setValue("");

	cpu->getParameters()->append(parameter);

	QSharedPointer<Cpu::AddressSpaceRef> aref(new Cpu::AddressSpaceRef("testSpace") );
	cpu->getAddressSpaceReferences()->append(aref);

	QVector<QString> errorList;
	validator.findErrorsIn(errorList, cpu, "test");

	QVERIFY(!validator.validate(cpu));
	QCOMPARE(errorList.size(), 1);
    QCOMPARE(errorList.first(), QString("No value specified for parameter testParameter within cpu testCpu"));
}

//-----------------------------------------------------------------------------
// Function: tst_CPUValidator::failPresense()
//-----------------------------------------------------------------------------
void tst_CPUValidator::failPresense()
{
    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator> parameterValidator(new ParameterValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > >()));

    QSharedPointer<QList<QSharedPointer<AddressSpace> > > singleSpace(new QList<QSharedPointer<AddressSpace> >());
    singleSpace->append(QSharedPointer<AddressSpace>(new AddressSpace("testSpace")));
    QSharedPointer<QList<QSharedPointer<MemoryMap> > > noMaps;

    CPUValidator validator(parameterValidator, parser, singleSpace, noMaps, Document::Revision::Std14);

    QSharedPointer<Cpu> cpu( new Cpu());
    cpu->setName("testCpu");
	cpu->setIsPresent("alabama-man");

	QSharedPointer<Cpu::AddressSpaceRef> aref( new Cpu::AddressSpaceRef("testSpace") );
	cpu->getAddressSpaceReferences()->append( aref );

	QVector<QString> errorList;
	validator.findErrorsIn(errorList, cpu, "test");

	QVERIFY(!validator.validate(cpu));
	QCOMPARE(errorList.size(), 1);
    QCOMPARE(errorList.first(), QString("Is present expression 'alabama-man' in CPU testCpu is invalid."));
}

//-----------------------------------------------------------------------------
// Function: tst_CPUValidator::failRefPresense()
//-----------------------------------------------------------------------------
void tst_CPUValidator::failRefPresense()
{
    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator> parameterValidator(new ParameterValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > >()));

    QSharedPointer<QList<QSharedPointer<AddressSpace> > > singleSpace(new QList<QSharedPointer<AddressSpace> >());
    singleSpace->append(QSharedPointer<AddressSpace>(new AddressSpace("testSpace")));
    QSharedPointer<QList<QSharedPointer<MemoryMap> > > noMaps;

    CPUValidator validator(parameterValidator, parser, singleSpace, noMaps, Document::Revision::Std14);

    QSharedPointer<Cpu> cpu( new Cpu());
    cpu->setName("testCpu");

	QSharedPointer<Cpu::AddressSpaceRef> aref(new Cpu::AddressSpaceRef("testSpace"));
	aref->setIsPresent("ohio");
	cpu->getAddressSpaceReferences()->append( aref );

	QVector<QString> errorList;
	validator.findErrorsIn(errorList, cpu, "test");

	QVERIFY(!validator.validate(cpu));
	QCOMPARE(errorList.size(), 1);
    QCOMPARE(errorList.first(), 
        QString("Is present expression 'ohio' for address space reference testSpace in CPU testCpu is invalid."));
}


//-----------------------------------------------------------------------------
// Function: tst_CPUValidator::errorIsNotFoundInErrorList()
//-----------------------------------------------------------------------------
bool tst_CPUValidator::errorIsNotFoundInErrorList(QString const& expectedError,
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
QTEST_APPLESS_MAIN(tst_CPUValidator)

#include "tst_CPUValidator.moc"
