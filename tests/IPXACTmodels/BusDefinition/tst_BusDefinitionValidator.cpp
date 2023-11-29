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

#include <KactusAPI/include/SystemVerilogExpressionParser.h>

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
    void testEmptyMaximumMastersAndSlaves();
    
    void testFailChoices();
    void testValidChoices();
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
    VLNV vlnv(VLNV::BUSDEFINITION, "vendori", "kurjasto", "jaska", "eka");
    QSharedPointer<BusDefinition> busDefinition(new BusDefinition(vlnv, Document::Revision::Std14));

    QSharedPointer<Parameter> parameter(new Parameter());
    parameter->setName("param");
    parameter->setType("bit");
    parameter->setValue("'b01");
    parameter->setValueId("parameterid");

    busDefinition->getParameters()->append(parameter);

	BusDefinitionValidator validator(nullptr, QSharedPointer<ExpressionParser>(new SystemVerilogExpressionParser()));

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
	QSharedPointer<BusDefinition> busDefinition(new BusDefinition(VLNV(VLNV::INVALID, "a", "b", "c", "d"), Document::Revision::Std14));

	BusDefinitionValidator validator(nullptr, QSharedPointer<ExpressionParser>(new SystemVerilogExpressionParser()));

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
    VLNV vlnv(VLNV::BUSDEFINITION, "vendori", "kurjasto", "jaska", "eka");
    QSharedPointer<BusDefinition> busDefinition(new BusDefinition(vlnv, Document::Revision::Std14));

    QSharedPointer<Parameter> parameter (new Parameter());
    parameter->setName("param");
    parameter->setType("bit");
    parameter->setValue("{'b11, 'b00}");
    parameter->setValueId("parameterid");

    busDefinition->getParameters()->append(parameter);

	BusDefinitionValidator validator(nullptr,QSharedPointer<ExpressionParser>(new SystemVerilogExpressionParser()));

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
    VLNV vlnv(VLNV::BUSDEFINITION, "vendori", "kurjasto", "jaska", "eka");
    QSharedPointer<BusDefinition> busDefinition(new BusDefinition(vlnv, Document::Revision::Std14));
    busDefinition->setMaxMasters("ghhhk,");
    busDefinition->setMaxSlaves("jii oku");

	BusDefinitionValidator validator(nullptr,QSharedPointer<ExpressionParser>(new SystemVerilogExpressionParser()));

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
    VLNV vlnv(VLNV::BUSDEFINITION, "vendori", "kurjasto", "jaska", "eka");
    QSharedPointer<BusDefinition> busDefinition(new BusDefinition(vlnv, Document::Revision::Std14));
    busDefinition->setMaxMasters("1337");
    busDefinition->setMaxSlaves("5");

	BusDefinitionValidator validator(nullptr,QSharedPointer<ExpressionParser>(new SystemVerilogExpressionParser()));

	QVector<QString> errorList;
	validator.findErrorsIn(errorList, busDefinition);

	QVERIFY(validator.validate(busDefinition));
	QCOMPARE(errorList.size(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_BusDefinitionValidator::testEmptyMaximumMastersAndSlaves()
//-----------------------------------------------------------------------------
void tst_BusDefinitionValidator::testEmptyMaximumMastersAndSlaves()
{
    VLNV vlnv(VLNV::BUSDEFINITION, "vendori", "kurjasto", "jaska", "eka");
    QSharedPointer<BusDefinition> busDefinition(new BusDefinition(vlnv, Document::Revision::Std14));
    busDefinition->setMaxMasters("");
    busDefinition->setMaxSlaves("");

    BusDefinitionValidator validator(nullptr,QSharedPointer<ExpressionParser>(new SystemVerilogExpressionParser()));

    QVector<QString> errorList;
    validator.findErrorsIn(errorList, busDefinition);

    QVERIFY(validator.validate(busDefinition));
    QCOMPARE(errorList.size(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_BusDefinitionValidator::testFailChoices()
//-----------------------------------------------------------------------------
void tst_BusDefinitionValidator::testFailChoices()
{
    VLNV vlnv(VLNV::BUSDEFINITION, "vendor", "library", "name", "version");
    QSharedPointer<BusDefinition> busDefinition(new BusDefinition(vlnv, Document::Revision::Std22));

    // Invalid enumeration.
    QSharedPointer<Choice> testChoice(new Choice());
    testChoice->setName("testChoice");

    QSharedPointer<Enumeration> testEnumeration(new Enumeration());
    testEnumeration->setValue("32#|!|!||||");

    testChoice->enumerations()->append(testEnumeration);

    busDefinition->getChoices()->append(testChoice);

    // No enumerations, no name.
    QSharedPointer<Choice> testChoice2(new Choice());

    busDefinition->getChoices()->append(testChoice2);

    BusDefinitionValidator validator(nullptr, QSharedPointer<ExpressionParser>(new SystemVerilogExpressionParser()));

    QVector<QString> errorList;
    validator.findErrorsIn(errorList, busDefinition);

    QCOMPARE(errorList.size(), 3);
    QVERIFY(!validator.validate(busDefinition));
}

//-----------------------------------------------------------------------------
// Function: tst_BusDefinitionValidator::testValidChoices()
//-----------------------------------------------------------------------------
void tst_BusDefinitionValidator::testValidChoices()
{
    VLNV vlnv(VLNV::BUSDEFINITION, "vendor", "library", "name", "version");
    QSharedPointer<BusDefinition> busDefinition(new BusDefinition(vlnv, Document::Revision::Std22));

    QSharedPointer<Choice> testChoice(new Choice());
    testChoice->setName("TestChoice");

    QSharedPointer<Enumeration> testEnumeration(new Enumeration());
    testEnumeration->setValue("32");

    testChoice->enumerations()->append(testEnumeration);
    
    busDefinition->getChoices()->append(testChoice);

    BusDefinitionValidator validator(nullptr, QSharedPointer<ExpressionParser>(new SystemVerilogExpressionParser()));

    QVector<QString> errorList;
    validator.findErrorsIn(errorList, busDefinition);

    QCOMPARE(errorList.size(), 0);
    QVERIFY(validator.validate(busDefinition));
}

QTEST_APPLESS_MAIN(tst_BusDefinitionValidator)

#include "tst_BusDefinitionValidator.moc"
