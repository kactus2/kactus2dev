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

#include <IPXACTmodels/Component/validators/CPUValidator.h>
#include <IPXACTmodels/Component/cpu.h>
#include <IPXACTmodels/common/Parameter.h>

#include <editors/ComponentEditor/common/SystemVerilogExpressionParser.h>

#include <QtTest>

class tst_CPUValidator : public QObject
{
    Q_OBJECT

public:
    tst_CPUValidator();

private slots:
	void baseCase();
	void noRef();
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
	QSharedPointer<Cpu> Cpu( new Cpu );
	CPUValidator validator;
	Cpu->setName("esa");

	QSharedPointer<Parameter> parameter (new Parameter());
	parameter->setName("param");
	parameter->setType("bit");
	parameter->setValue("{3'b101,3'b111}");
	parameter->setValueId("parameterid");

	Cpu->getParameters()->append(parameter);

	QSharedPointer<Cpu::AddressSpaceRef> aref( new Cpu::AddressSpaceRef("jari") );
	aref->setIsPresent("0");
	Cpu->getAddressSpaceReferences()->append( aref );

	Cpu->setIsPresent("1");

	QVector<QString> errorList;
	validator.findErrorsIn(errorList, Cpu, "test");

	QVERIFY( validator.validate(Cpu) );
	QCOMPARE( errorList.size(), 0 );
}

//-----------------------------------------------------------------------------
// Function: tst_CPUValidator::noRef()
//-----------------------------------------------------------------------------
void tst_CPUValidator::noRef()
{
	QSharedPointer<Cpu> Cpu( new Cpu );
	CPUValidator validator;
	Cpu->setName("esa");

	QVector<QString> errorList;
	validator.findErrorsIn(errorList, Cpu, "test");

	QVERIFY( !validator.validate(Cpu) );
	QCOMPARE( errorList.size(), 1 );
}

//-----------------------------------------------------------------------------
// Function: tst_CPUValidator::failParameter()
//-----------------------------------------------------------------------------
void tst_CPUValidator::failParameter()
{
	QSharedPointer<Cpu> Cpu( new Cpu);
	CPUValidator validator;
	Cpu->setName("esa");

	QSharedPointer<Parameter> parameter (new Parameter());
	parameter->setName("param");
	parameter->setType("bit");
	parameter->setValue("{'b11, 'b00}");
	parameter->setValueId("parameterid");

	Cpu->getParameters()->append(parameter);

	QSharedPointer<Cpu::AddressSpaceRef> aref( new Cpu::AddressSpaceRef("jari") );
	Cpu->getAddressSpaceReferences()->append( aref );

	QVector<QString> errorList;
	validator.findErrorsIn(errorList, Cpu, "test");

	QVERIFY( !validator.validate(Cpu) );
	QCOMPARE( errorList.size(), 1 );
}

//-----------------------------------------------------------------------------
// Function: tst_CPUValidator::failPresense()
//-----------------------------------------------------------------------------
void tst_CPUValidator::failPresense()
{
	QSharedPointer<Cpu> Cpu( new Cpu);
	CPUValidator validator;
	Cpu->setName("esa");

	Cpu->setIsPresent("alabama-man");

	QSharedPointer<Cpu::AddressSpaceRef> aref( new Cpu::AddressSpaceRef("jari") );
	Cpu->getAddressSpaceReferences()->append( aref );

	QVector<QString> errorList;
	validator.findErrorsIn(errorList, Cpu, "test");

	QVERIFY( !validator.validate(Cpu) );
	QCOMPARE( errorList.size(), 1 );
}

//-----------------------------------------------------------------------------
// Function: tst_CPUValidator::failRefPresense()
//-----------------------------------------------------------------------------
void tst_CPUValidator::failRefPresense()
{
	QSharedPointer<Cpu> Cpu( new Cpu);
	CPUValidator validator;
	Cpu->setName("esa");

	QSharedPointer<Cpu::AddressSpaceRef> aref( new Cpu::AddressSpaceRef("jari") );
	aref->setIsPresent("ohio");
	Cpu->getAddressSpaceReferences()->append( aref );

	QVector<QString> errorList;
	validator.findErrorsIn(errorList, Cpu, "test");

	QVERIFY( !validator.validate(Cpu) );
	QCOMPARE( errorList.size(), 1 );
}

QTEST_APPLESS_MAIN(tst_CPUValidator)

#include "tst_CPUValidator.moc"
