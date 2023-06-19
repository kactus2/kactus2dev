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

    void failParameter();
	void failPresense();
	void failRefPresense();

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

QTEST_APPLESS_MAIN(tst_CPUValidator)

#include "tst_CPUValidator.moc"
