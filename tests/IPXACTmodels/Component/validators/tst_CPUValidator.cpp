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
#include <IPXACTmodels/Component/validators/CPUValidator.h>

#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/common/validators/ParameterValidator2014.h>

#include <editors/ComponentEditor/common/SystemVerilogExpressionParser.h>

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
    void testIllegalReference();
	void failParameter();
	void failPresense();
	void failRefPresense();

private:
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
    QSharedPointer<ParameterValidator2014> parameterValidator(new ParameterValidator2014(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > >()));

    QSharedPointer<QList<QSharedPointer<AddressSpace> > > singleSpace(new QList<QSharedPointer<AddressSpace> >());
    singleSpace->append(QSharedPointer<AddressSpace>(new AddressSpace("testSpace")));

    CPUValidator validator(parameterValidator, parser, singleSpace);
	
    QSharedPointer<Cpu> cpu(new Cpu());
    cpu->setName("esa");
	cpu->setIsPresent("1");

	QSharedPointer<Parameter> parameter (new Parameter());
	parameter->setName("param");
	parameter->setType("bit");
	parameter->setValue("{3'b101,3'b111}");
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
    QSharedPointer<ParameterValidator2014> parameterValidator(new ParameterValidator2014(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > >()));
    QSharedPointer<QList<QSharedPointer<AddressSpace> > > noSpaces;
    CPUValidator validator(parameterValidator, parser, noSpaces);

    QSharedPointer<Cpu> Cpu( new Cpu );
    Cpu->setName("");

    QVector<QString> errorList;
    validator.findErrorsIn(errorList, Cpu, "test");

    QVERIFY(!validator.validate(Cpu));
    QVERIFY(errorList.size() > 0);
    QCOMPARE(errorList.first(), QString("Invalid name '' set for CPU within test."));
}

//-----------------------------------------------------------------------------
// Function: tst_CPUValidator::noRef()
//-----------------------------------------------------------------------------
void tst_CPUValidator::noRef()
{
    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator2014> parameterValidator(new ParameterValidator2014(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > >()));
    QSharedPointer<QList<QSharedPointer<AddressSpace> > > noSpaces;
    CPUValidator validator(parameterValidator, parser, noSpaces);

    QSharedPointer<Cpu> Cpu( new Cpu );
    Cpu->setName("testCpu");

	QVector<QString> errorList;
	validator.findErrorsIn(errorList, Cpu, "test");

	QVERIFY(!validator.validate(Cpu));
	QVERIFY(errorList.size() > 0);
    QCOMPARE(errorList.first(), QString("No address space reference set for CPU testCpu within test."));
}

//-----------------------------------------------------------------------------
// Function: tst_CPUValidator::testIllegalReference()
//-----------------------------------------------------------------------------
void tst_CPUValidator::testIllegalReference()
{
    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator2014> parameterValidator(new ParameterValidator2014(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > >()));
    QSharedPointer<QList<QSharedPointer<AddressSpace> > > noSpaces;
    CPUValidator validator(parameterValidator, parser, noSpaces);

    QSharedPointer<Cpu> cpu(new Cpu());
    cpu->setName("testCpu");

    QSharedPointer<Cpu::AddressSpaceRef> aref(new Cpu::AddressSpaceRef("nonExistingSpace") );
    cpu->getAddressSpaceReferences()->append(aref);

    QVector<QString> errorList;
    validator.findErrorsIn(errorList, cpu, "test");

    QVERIFY(!validator.validate(cpu));
    QCOMPARE(errorList.size(), 1);
    QCOMPARE(errorList.first(), QString("Address space 'nonExistingSpace' referenced within cpu testCpu not found."));
}

//-----------------------------------------------------------------------------
// Function: tst_CPUValidator::failParameter()
//-----------------------------------------------------------------------------
void tst_CPUValidator::failParameter()
{
    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator2014> parameterValidator(new ParameterValidator2014(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > >()));

    QSharedPointer<QList<QSharedPointer<AddressSpace> > > singleSpace(new QList<QSharedPointer<AddressSpace> >());
    singleSpace->append(QSharedPointer<AddressSpace>(new AddressSpace("testSpace")));

    CPUValidator validator(parameterValidator, parser, singleSpace);

    QSharedPointer<Cpu> cpu( new Cpu);
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
    QSharedPointer<ParameterValidator2014> parameterValidator(new ParameterValidator2014(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > >()));

    QSharedPointer<QList<QSharedPointer<AddressSpace> > > singleSpace(new QList<QSharedPointer<AddressSpace> >());
    singleSpace->append(QSharedPointer<AddressSpace>(new AddressSpace("testSpace")));

    CPUValidator validator(parameterValidator, parser, singleSpace);

    QSharedPointer<Cpu> cpu( new Cpu);
    cpu->setName("testCpu");
	cpu->setIsPresent("alabama-man");

	QSharedPointer<Cpu::AddressSpaceRef> aref( new Cpu::AddressSpaceRef("testSpace") );
	cpu->getAddressSpaceReferences()->append( aref );

	QVector<QString> errorList;
	validator.findErrorsIn(errorList, cpu, "test");

	QVERIFY(!validator.validate(cpu));
	QCOMPARE(errorList.size(), 1);
    QCOMPARE(errorList.first(), QString("Is present expression 'alabama-man' in cpu testCpu is invalid."));
}

//-----------------------------------------------------------------------------
// Function: tst_CPUValidator::failRefPresense()
//-----------------------------------------------------------------------------
void tst_CPUValidator::failRefPresense()
{
    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<ParameterValidator2014> parameterValidator(new ParameterValidator2014(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > >()));

    QSharedPointer<QList<QSharedPointer<AddressSpace> > > singleSpace(new QList<QSharedPointer<AddressSpace> >());
    singleSpace->append(QSharedPointer<AddressSpace>(new AddressSpace("testSpace")));

    CPUValidator validator(parameterValidator, parser, singleSpace);

    QSharedPointer<Cpu> Cpu( new Cpu);
    Cpu->setName("testCpu");

	QSharedPointer<Cpu::AddressSpaceRef> aref(new Cpu::AddressSpaceRef("testSpace"));
	aref->setIsPresent("ohio");
	Cpu->getAddressSpaceReferences()->append( aref );

	QVector<QString> errorList;
	validator.findErrorsIn(errorList, Cpu, "test");

	QVERIFY(!validator.validate(Cpu));
	QCOMPARE(errorList.size(), 1);
    QCOMPARE(errorList.first(), 
        QString("Is present expression 'ohio' for address space reference testSpace in cpu testCpu is invalid."));
}

QTEST_APPLESS_MAIN(tst_CPUValidator)

#include "tst_CPUValidator.moc"
