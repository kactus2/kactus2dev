//-----------------------------------------------------------------------------
// File: tst_IPXactSystemVerilogParser.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 02.12.2014
//
// Description:
// Unit test for class IPXactSystemVerilogParser.
//-----------------------------------------------------------------------------

#include <QtTest>
#include <QSharedPointer>

#include <editors/ComponentEditor/common/IPXactSystemVerilogParser.h>

#include <IPXACTmodels/addressblock.h>
#include <IPXACTmodels/businterface.h>
#include <IPXACTmodels/component.h>
#include <IPXACTmodels/memorymap.h>
#include <IPXACTmodels/memorymapitem.h>
#include <IPXACTmodels/model.h>
#include <IPXACTmodels/modelparameter.h>
#include <IPXACTmodels/parameter.h>
#include <IPXACTmodels/register.h>

class tst_IPXactSystemVerilogParser : public QObject
{
    Q_OBJECT

public:
    tst_IPXactSystemVerilogParser();

private slots:

    void testParseConstant();
    void testParseConstant_data();

    void testExpressionWithUnknownReference();
    void testParameterWithoutId();

    void testExpressionWithParameterReferences();
    void testExpressionWithParameterReferences_data();

    void testExpressionWithModelParameterReferences();
    void testExpressionWithModelParameterReferences_data();

    void testExpressionWithViewParameterReferences();
    void testExpressionWithViewParameterReferences_data();

    void testExpressionWithRegisterParameterReferences();
    void testExpressionWithRegisterParameterReferences_data();

    void testExpressionWithBusInterfaceParameterReferences();
    void testExpressionWithBusInterfaceParameterReferences_data();

    void testParameterDefinedUsingOtherParameter();
    void testParameterDefinedUsingOtherParameter_data();

    void testLoopTerminatesEventually();

    void testReferenceToStringInExpression();
};

//-----------------------------------------------------------------------------
// Function: tst_IPXactSystemVerilogParser::tst_IPXactSystemVerilogParser()
//-----------------------------------------------------------------------------
tst_IPXactSystemVerilogParser::tst_IPXactSystemVerilogParser()
{
}

//-----------------------------------------------------------------------------
// Function: tst_IPXactSystemVerilogParser::testParseConstant()
//-----------------------------------------------------------------------------
void tst_IPXactSystemVerilogParser::testParseConstant()
{   
    QFETCH(QString, expression);
    QFETCH(QString, expectedResult);

    QSharedPointer<Component> emptyComponent(new Component());

    IPXactSystemVerilogParser parser(emptyComponent);

    QCOMPARE(parser.parseExpression(expression), expectedResult);
}

//-----------------------------------------------------------------------------
// Function: tst_IPXactSystemVerilogParser::testParseConstant_data()
//-----------------------------------------------------------------------------
void tst_IPXactSystemVerilogParser::testParseConstant_data()
{
    QTest::addColumn<QString>("expression");
    QTest::addColumn<QString>("expectedResult");

    QTest::newRow("Boolean true is invalid") << "true" << "true";
    QTest::newRow("Boolean false is invalid") << "false" << "false";

    QTest::newRow("Decimal value 1 is valid") << "1" << "1";

    QTest::newRow("Hexadecimal value 0xff is invalid") << "0xff" << "0xff";
    QTest::newRow("Hexadecimal value #ff is invalid") << "#ff" << "#ff";
    QTest::newRow("Hexadecimal value ff is invalid") << "ff" << "ff";

}

//-----------------------------------------------------------------------------
// Function: tst_IPXactSystemVerilogParser::testExpressionWithUnknownReference()
//-----------------------------------------------------------------------------
void tst_IPXactSystemVerilogParser::testExpressionWithUnknownReference()
{
    QSharedPointer<Component> emptyComponent(new Component());

    IPXactSystemVerilogParser parser(emptyComponent);

    QCOMPARE(parser.parseExpression("unknownParameter"), QString("unknownParameter"));
}

//-----------------------------------------------------------------------------
// Function: tst_IPXactSystemVerilogParser::testParameterWithoutId()
//-----------------------------------------------------------------------------
void tst_IPXactSystemVerilogParser::testParameterWithoutId()
{
    QSharedPointer<Component> testComponent(new Component());
    QSharedPointer<Parameter> firstParameter(new Parameter());
    firstParameter->setValueId("");
    firstParameter->setValue("1");
    testComponent->getParameters().append(firstParameter);

    IPXactSystemVerilogParser parser(testComponent);

    QCOMPARE(parser.parseExpression("unknownParameter"), QString("unknownParameter"));
}

//-----------------------------------------------------------------------------
// Function: tst_IPXactSystemVerilogParser::testExpressionWithParameterReferences()
//-----------------------------------------------------------------------------
void tst_IPXactSystemVerilogParser::testExpressionWithParameterReferences()
{
    QFETCH(QString, expression);
    QFETCH(QString, expectedResult);

    QSharedPointer<Component> testComponent(new Component());
    QSharedPointer<Parameter> firstParameter(new Parameter());
    firstParameter->setValueId("one");
    firstParameter->setValue("1");
    testComponent->getParameters().append(firstParameter);

    QSharedPointer<Parameter> secondParameter(new Parameter());
    secondParameter->setValueId("two");
    secondParameter->setValue("2");
    testComponent->getParameters().append(secondParameter);

    IPXactSystemVerilogParser parser(testComponent);
    
    QCOMPARE(parser.parseExpression(expression), expectedResult);
}

//-----------------------------------------------------------------------------
// Function: tst_IPXactSystemVerilogParser::testExpressionWithParameterReferences_data()
//-----------------------------------------------------------------------------
void tst_IPXactSystemVerilogParser::testExpressionWithParameterReferences_data()
{
    QTest::addColumn<QString>("expression");
    QTest::addColumn<QString>("expectedResult");

    QTest::newRow("Parameter value only") << "one" << "1" ;
    QTest::newRow("Parameter value plus constant decimal") << "one + 1" << "2" ;
    QTest::newRow("Parameter value plus parameter value") << "one + one" << "2";
    QTest::newRow("Parameter value multiplied") << "4*one" << "4";

    QTest::newRow("Reference to unknown parameter") << "unknown" << "unknown" ;
    QTest::newRow("Parameter value plus unknown parameter value") << "one + unknown" << "1 + unknown";

    QTest::newRow("Two parameters") << "one + two" << "3";
}

//-----------------------------------------------------------------------------
// Function: tst_IPXactSystemVerilogParser::testExpressionWithModelParameterReferences()
//-----------------------------------------------------------------------------
void tst_IPXactSystemVerilogParser::testExpressionWithModelParameterReferences()
{
    QFETCH(QString, expression);
    QFETCH(QString, expectedResult);

    QSharedPointer<Component> testComponent(new Component());
    QSharedPointer<ModelParameter> firstModelParameter(new ModelParameter());
    firstModelParameter->setValueId("one");
    firstModelParameter->setValue("1");
    testComponent->getModel()->addModelParameter(firstModelParameter);

    QSharedPointer<ModelParameter> secondModelParameter(new ModelParameter());
    secondModelParameter->setValueId("two");
    secondModelParameter->setValue("2");
    testComponent->getModel()->addModelParameter(secondModelParameter);

    IPXactSystemVerilogParser parser(testComponent);

    QCOMPARE(parser.parseExpression(expression), expectedResult);
}

//-----------------------------------------------------------------------------
// Function: tst_IPXactSystemVerilogParser::testExpressionWithModelParameterReferences()
//-----------------------------------------------------------------------------
void tst_IPXactSystemVerilogParser::testExpressionWithModelParameterReferences_data()
{
    testExpressionWithParameterReferences_data();
}

//-----------------------------------------------------------------------------
// Function: tst_IPXactSystemVerilogParser::testExpressionWithViewParameterReferences()
//-----------------------------------------------------------------------------
void tst_IPXactSystemVerilogParser::testExpressionWithViewParameterReferences()
{
    QFETCH(QString, expression);
    QFETCH(QString, expectedResult);

    QSharedPointer<Component> testComponent(new Component());
    QSharedPointer<Parameter> firstParameter(new Parameter());
    firstParameter->setValueId("one");
    firstParameter->setValue("1");
    View* view1 = testComponent->createView();
    view1->getParameters().append(firstParameter);

    QSharedPointer<Parameter> secondParameter(new Parameter());
    secondParameter->setValueId("two");
    secondParameter->setValue("2");
    View* view2 = testComponent->createView();
    view2->getParameters().append(secondParameter);

    IPXactSystemVerilogParser parser(testComponent);

    QCOMPARE(parser.parseExpression(expression), expectedResult);
}

//-----------------------------------------------------------------------------
// Function: tst_IPXactSystemVerilogParser::testExpressionWithViewParameterReferences_data()
//-----------------------------------------------------------------------------
void tst_IPXactSystemVerilogParser::testExpressionWithViewParameterReferences_data()
{
    testExpressionWithParameterReferences_data();
}

//-----------------------------------------------------------------------------
// Function: tst_IPXactSystemVerilogParser::testExpressionWithRegisterParameterReferences()
//-----------------------------------------------------------------------------
void tst_IPXactSystemVerilogParser::testExpressionWithRegisterParameterReferences()
{
    QFETCH(QString, expression);
    QFETCH(QString, expectedResult);

    QSharedPointer<Component> testComponent(new Component());

    QSharedPointer<MemoryMap> memoryMap(new MemoryMap());
    testComponent->getMemoryMaps().append(memoryMap);

    QSharedPointer<AddressBlock> addressBlock(new AddressBlock());
    QList<QSharedPointer<MemoryMapItem> > addressBlocks;
    addressBlocks.append(addressBlock);
    memoryMap->setItems(addressBlocks);

    QSharedPointer<Parameter> firstParameter(new Parameter());
    firstParameter->setValueId("one");
    firstParameter->setValue("1");

    QSharedPointer<Register> register1(new Register());
    QList<QSharedPointer<Parameter> > firstParameterList;
    firstParameterList.append(firstParameter);
    register1->setParameters(firstParameterList);

    addressBlock->getRegisterData().append(register1);

    QSharedPointer<Parameter> secondParameter(new Parameter());
    secondParameter->setValueId("two");
    secondParameter->setValue("2");

    QSharedPointer<Register> register2(new Register());
    QList<QSharedPointer<Parameter> > secondParameterList;
    secondParameterList.append(secondParameter);
    register2->setParameters(secondParameterList);

    addressBlock->getRegisterData().append(register2);

    IPXactSystemVerilogParser parser(testComponent);

    QCOMPARE(parser.parseExpression(expression), expectedResult);
}

//-----------------------------------------------------------------------------
// Function: tst_IPXactSystemVerilogParser::testExpressionWithRegisterParameterReferences_data()
//-----------------------------------------------------------------------------
void tst_IPXactSystemVerilogParser::testExpressionWithRegisterParameterReferences_data()
{
    testExpressionWithParameterReferences_data();
}

//-----------------------------------------------------------------------------
// Function: tst_IPXactSystemVerilogParser::testExpressionWithBusInterfaceParameterReferences()
//-----------------------------------------------------------------------------
void tst_IPXactSystemVerilogParser::testExpressionWithBusInterfaceParameterReferences()
{
    QFETCH(QString, expression);
    QFETCH(QString, expectedResult);

    QSharedPointer<Component> testComponent(new Component());
    QSharedPointer<Parameter> firstParameter(new Parameter());
    firstParameter->setValueId("one");
    firstParameter->setValue("1");

    QSharedPointer<Parameter> secondParameter(new Parameter());
    secondParameter->setValueId("two");
    secondParameter->setValue("2");

    QSharedPointer<BusInterface> busInterface1(new BusInterface());
    busInterface1->getParameters().append(firstParameter);
    testComponent->addBusInterface(busInterface1);

    QSharedPointer<BusInterface> busInterface2(new BusInterface());
    busInterface2->setName("busIf");
    busInterface2->getParameters().append(secondParameter);
    testComponent->addBusInterface(busInterface2);

    IPXactSystemVerilogParser parser(testComponent);

    QCOMPARE(parser.parseExpression(expression), expectedResult);
}

//-----------------------------------------------------------------------------
// Function: tst_IPXactSystemVerilogParser::testExpressionWithBusInterfaceParameterReferences_data()
//-----------------------------------------------------------------------------
void tst_IPXactSystemVerilogParser::testExpressionWithBusInterfaceParameterReferences_data()
{
    testExpressionWithParameterReferences_data();
}

//-----------------------------------------------------------------------------
// Function: tst_IPXactSystemVerilogParser::testParameterDefinedUsingOtherParameter()
//-----------------------------------------------------------------------------
void tst_IPXactSystemVerilogParser::testParameterDefinedUsingOtherParameter()
{
    QFETCH(QString, expression);
    QFETCH(QString, expectedResult);

    QSharedPointer<Component> testComponent(new Component());
    QSharedPointer<Parameter> firstParameter(new Parameter());
    firstParameter->setValueId("first");
    firstParameter->setValue("1");
    testComponent->getParameters().append(firstParameter);

    QSharedPointer<Parameter> secondParameter(new Parameter());
    secondParameter->setValueId("second");
    secondParameter->setValue("2*first");
    testComponent->getParameters().append(secondParameter);

    IPXactSystemVerilogParser parser(testComponent);

    QCOMPARE(parser.parseExpression(expression), QString(expectedResult));
}

//-----------------------------------------------------------------------------
// Function: tst_IPXactSystemVerilogParser::testParameterDefinedUsingOtherParameter_data()
//-----------------------------------------------------------------------------
void tst_IPXactSystemVerilogParser::testParameterDefinedUsingOtherParameter_data()
{
    QTest::addColumn<QString>("expression");
    QTest::addColumn<QString>("expectedResult");

    QTest::newRow("Parameter defined using other parameter") << "second" << "2";
    QTest::newRow("Parameter value is evaluated before other operations") << "second**3" << "8";
}

//-----------------------------------------------------------------------------
// Function: tst_IPXactSystemVerilogParser::testLoopTerminatesEventually()
//-----------------------------------------------------------------------------
void tst_IPXactSystemVerilogParser::testLoopTerminatesEventually()
{
    QSharedPointer<Component> testComponent(new Component());
    QSharedPointer<Parameter> firstParameter(new Parameter());
    firstParameter->setValueId("first");
    firstParameter->setValue("second");
    testComponent->getParameters().append(firstParameter);

    QSharedPointer<Parameter> secondParameter(new Parameter());
    secondParameter->setValueId("second");
    secondParameter->setValue("first");
    testComponent->getParameters().append(secondParameter);

    IPXactSystemVerilogParser parser(testComponent);

    QCOMPARE(parser.parseExpression("second"), QString("x"));
}

//-----------------------------------------------------------------------------
// Function: tst_IPXactSystemVerilogParser::testReferenceToString()
//-----------------------------------------------------------------------------
void tst_IPXactSystemVerilogParser::testReferenceToStringInExpression()
{
    QSharedPointer<Component> testComponent(new Component());
    QSharedPointer<Parameter> firstParameter(new Parameter());
    firstParameter->setValueId("first");
    firstParameter->setValue("\"text\"");
    testComponent->getParameters().append(firstParameter);

    QSharedPointer<Parameter> secondParameter(new Parameter());
    secondParameter->setValueId("second");
    secondParameter->setValue("first + 2");
    testComponent->getParameters().append(secondParameter);

    IPXactSystemVerilogParser parser(testComponent);

    QCOMPARE(parser.parseExpression("first"), QString("\"text\""));
    QCOMPARE(parser.parseExpression("second"), QString("\"text\" + 2"));
}

QTEST_APPLESS_MAIN(tst_IPXactSystemVerilogParser)

#include "tst_IPXactSystemVerilogParser.moc"
