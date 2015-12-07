//-----------------------------------------------------------------------------
// File: tst_BusDefinitionValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 17.11.2015
//
// Description:
// Unit test for class BusDefinitionValidator.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <IPXACTmodels/BusDefinition/BusDefinition.h>
#include <IPXACTmodels/BusDefinition/validators/BusDefinitionValidator.h>

#include <editors/ComponentEditor/common/SystemVerilogExpressionParser.h>

class tst_BusDefinitionValidator : public QObject
{
    Q_OBJECT

public:
    tst_BusDefinitionValidator();

private slots:

    void baseCase();
	void failVLNV();
	void failParameter();
	void maxFail();
	void maxPass();
};

//-----------------------------------------------------------------------------
// Function: tst_BusDefinitionValidator::tst_BusDefinitionValidator()
//-----------------------------------------------------------------------------
tst_BusDefinitionValidator::tst_BusDefinitionValidator()
{

}

//-----------------------------------------------------------------------------
// Function: tst_BusDefinitionValidator::baseCase()
//-----------------------------------------------------------------------------
void tst_BusDefinitionValidator::baseCase()
{
	QSharedPointer<BusDefinition> busDefinition(new BusDefinition());
	busDefinition->setVlnv(VLNV("tyyppi", "vendori", "kurjasto", "jaska", "eka"));

    QSharedPointer<Parameter> parameter(new Parameter());
    parameter->setName("param");
    parameter->setType("bit");
    parameter->setValue("'b01");
    parameter->setValueId("parameterid");

    busDefinition->getParameters()->append(parameter);

	BusDefinitionValidator validator(QSharedPointer<ExpressionParser>(new SystemVerilogExpressionParser()));

	QVector<QString> errorList;
	validator.findErrorsIn(errorList, busDefinition);

	QCOMPARE(errorList.size(), 0);
	QVERIFY(validator.validate(busDefinition));
}

//-----------------------------------------------------------------------------
// Function: tst_BusDefinitionValidator::failVLNV()
//-----------------------------------------------------------------------------
void tst_BusDefinitionValidator::failVLNV()
{
	QSharedPointer<BusDefinition> busDefinition(new BusDefinition());

	BusDefinitionValidator validator(QSharedPointer<ExpressionParser>(new SystemVerilogExpressionParser()));

	QVector<QString> errorList;
	validator.findErrorsIn(errorList, busDefinition);

	QVERIFY(!validator.validate(busDefinition));
	QVERIFY(errorList.size() > 0);
}

//-----------------------------------------------------------------------------
// Function: tst_BusDefinitionValidator::failParameter()
//-----------------------------------------------------------------------------
void tst_BusDefinitionValidator::failParameter()
{
	QSharedPointer<BusDefinition> busDefinition(new BusDefinition());
	busDefinition->setVlnv(VLNV("tyyppi", "vendori", "kurjasto", "jaska", "eka"));

    QSharedPointer<Parameter> parameter (new Parameter());
    parameter->setName("param");
    parameter->setType("bit");
    parameter->setValue("{'b11, 'b00}");
    parameter->setValueId("parameterid");

    busDefinition->getParameters()->append(parameter);

	BusDefinitionValidator validator(QSharedPointer<ExpressionParser>(new SystemVerilogExpressionParser()));

	QVector<QString> errorList;
	validator.findErrorsIn(errorList, busDefinition);

	QVERIFY(!validator.validate(busDefinition));
	QVERIFY(errorList.size() == 1);
}

//-----------------------------------------------------------------------------
// Function: tst_BusDefinitionValidator::maxFail()
//-----------------------------------------------------------------------------
void tst_BusDefinitionValidator::maxFail()
{
	QSharedPointer<BusDefinition> busDefinition(new BusDefinition());
    busDefinition->setVlnv(VLNV("tyyppi", "vendori", "kurjasto", "jaska", "eka"));
    busDefinition->setMaxMasters("ghhhk,");
    busDefinition->setMaxSlaves("jii oku");

	BusDefinitionValidator validator(QSharedPointer<ExpressionParser>(new SystemVerilogExpressionParser()));

	QVector<QString> errorList;
	validator.findErrorsIn(errorList, busDefinition);

	QVERIFY(!validator.validate(busDefinition));
	QVERIFY(errorList.size() == 2);
}

//-----------------------------------------------------------------------------
// Function: tst_BusDefinitionValidator::maxPass()
//-----------------------------------------------------------------------------
void tst_BusDefinitionValidator::maxPass()
{
	QSharedPointer<BusDefinition> busDefinition(new BusDefinition());
    busDefinition->setVlnv(VLNV("tyyppi","vendori","kurjasto","jaska","eka"));
    busDefinition->setMaxMasters("1337");
    busDefinition->setMaxSlaves("5");

	BusDefinitionValidator validator(QSharedPointer<ExpressionParser>(new SystemVerilogExpressionParser()));

	QVector<QString> errorList;
	validator.findErrorsIn(errorList, busDefinition);

	QVERIFY(validator.validate(busDefinition));
	QCOMPARE(errorList.size(), 0);
}

QTEST_APPLESS_MAIN(tst_BusDefinitionValidator)

#include "tst_BusDefinitionValidator.moc"
