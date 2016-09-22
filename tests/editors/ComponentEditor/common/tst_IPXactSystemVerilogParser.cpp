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

#include <editors/ComponentEditor/common/ComponentParameterFinder.h>
#include <editors/ComponentEditor/common/ParameterCache.h>

#include <IPXACTmodels/Component/AddressBlock.h>
#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/Component/MemoryBlockBase.h>
#include <IPXACTmodels/Component/Model.h>
#include <IPXACTmodels/Component/Register.h>

#include <IPXACTmodels/common/Parameter.h>


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
    void testParameterWithGeneratedId();

    void testExpressionWithParameterReferences();
    void testExpressionWithParameterReferences_data();

    void testExpressionWithRegisterParameterReferences();
    void testExpressionWithRegisterParameterReferences_data();

    void testExpressionWithBusInterfaceParameterReferences();
    void testExpressionWithBusInterfaceParameterReferences_data();

    void testParameterDefinedUsingOtherParameter();
    void testParameterDefinedUsingOtherParameter_data();

    void testLoopTerminatesEventually();

    void testReferenceToStringInExpression();

    void testGetBaseForExpression();

    void testExpressionWithRealValueParameterReferences();
    void testExpressionWithRealValueParameterReferences_data();

    void testExpressionComparison();
    void testExpressionComparison_data();

    void testLongReferenceChainPerformance();
    void testLongReferenceChainPerformance_data();
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

    IPXactSystemVerilogParser parser(QSharedPointer<ParameterFinder>(new ComponentParameterFinder(emptyComponent)));

    QCOMPARE(parser.parseExpression(expression), expectedResult);
}

//-----------------------------------------------------------------------------
// Function: tst_IPXactSystemVerilogParser::testParseConstant_data()
//-----------------------------------------------------------------------------
void tst_IPXactSystemVerilogParser::testParseConstant_data()
{
    QTest::addColumn<QString>("expression");
    QTest::addColumn<QString>("expectedResult");

    QTest::newRow("Boolean true is invalid") << "true" << "1";
    QTest::newRow("Boolean false is invalid") << "false" << "0";

    QTest::newRow("Decimal value 1 is valid") << "1" << "1";

    QTest::newRow("Hexadecimal value 0xff is invalid") << "0xff" << "x";
    QTest::newRow("Hexadecimal value #ff is invalid") << "#ff" << "x";
    QTest::newRow("Hexadecimal value ff is invalid") << "ff" << "x";
}

//-----------------------------------------------------------------------------
// Function: tst_IPXactSystemVerilogParser::testExpressionWithUnknownReference()
//-----------------------------------------------------------------------------
void tst_IPXactSystemVerilogParser::testExpressionWithUnknownReference()
{
    QSharedPointer<Component> emptyComponent(new Component());

    IPXactSystemVerilogParser parser(QSharedPointer<ParameterFinder>(new ComponentParameterFinder(emptyComponent)));

    QCOMPARE(parser.parseExpression("unknownParameter"), QString("x"));
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
    testComponent->getParameters()->append(firstParameter);

    IPXactSystemVerilogParser parser(QSharedPointer<ParameterFinder>(new ComponentParameterFinder(testComponent)));

    QCOMPARE(parser.parseExpression("unknownParameter"), QString("x"));
}

//-----------------------------------------------------------------------------
// Function: tst_IPXactSystemVerilogParser::testParameterWithGeneratedId()
//-----------------------------------------------------------------------------
void tst_IPXactSystemVerilogParser::testParameterWithGeneratedId()
{
    QSharedPointer<Component> testComponent(new Component());
    QSharedPointer<Parameter> firstParameter(new Parameter());
    firstParameter->setValue("1");
    testComponent->getParameters()->append(firstParameter);

    IPXactSystemVerilogParser parser(QSharedPointer<ParameterFinder>(new ComponentParameterFinder(testComponent)));

    QCOMPARE(parser.isPlainValue(firstParameter->getValueId()), false);
    QCOMPARE(parser.parseExpression(firstParameter->getValueId()), QString("1"));
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
    testComponent->getParameters()->append(firstParameter);

    QSharedPointer<Parameter> secondParameter(new Parameter());
    secondParameter->setValueId("two");
    secondParameter->setValue("2");
    testComponent->getParameters()->append(secondParameter);

    IPXactSystemVerilogParser parser(QSharedPointer<ParameterFinder>(new ComponentParameterFinder(testComponent)));
    
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

    QTest::newRow("Reference to unknown parameter") << "unknown" << "x" ;
    QTest::newRow("Parameter value plus unknown parameter value") << "one + unknown" << "x";

    QTest::newRow("Two parameters") << "one + two" << "3";
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
    testComponent->getMemoryMaps()->append(memoryMap);

    QSharedPointer<AddressBlock> addressBlock(new AddressBlock());
    memoryMap->getMemoryBlocks()->append(addressBlock);

    QSharedPointer<Parameter> firstParameter(new Parameter());
    firstParameter->setValueId("one");
    firstParameter->setValue("1");

    QSharedPointer<Register> register1(new Register());
    register1->getParameters()->append(firstParameter);
    addressBlock->getRegisterData()->append(register1);

    QSharedPointer<Parameter> secondParameter(new Parameter());
    secondParameter->setValueId("two");
    secondParameter->setValue("2");

    QSharedPointer<Register> register2(new Register());    
    register2->getParameters()->append(secondParameter);

    addressBlock->getRegisterData()->append(register2);

    IPXactSystemVerilogParser parser(QSharedPointer<ParameterFinder>(new ComponentParameterFinder(testComponent)));

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
    busInterface1->getParameters()->append(firstParameter);
    testComponent->getBusInterfaces()->append(busInterface1);

    QSharedPointer<BusInterface> busInterface2(new BusInterface());
    busInterface2->setName("busIf");
    busInterface2->getParameters()->append(secondParameter);
    testComponent->getBusInterfaces()->append(busInterface2);

    IPXactSystemVerilogParser parser(QSharedPointer<ParameterFinder>(new ComponentParameterFinder(testComponent)));

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
    testComponent->getParameters()->append(firstParameter);

    QSharedPointer<Parameter> secondParameter(new Parameter());
    secondParameter->setValueId("second");
    secondParameter->setValue("2*first");
    testComponent->getParameters()->append(secondParameter);

    QSharedPointer<Parameter> thirdParameter(new Parameter());
    thirdParameter->setValueId("third");
    thirdParameter->setValue("second**second");
    testComponent->getParameters()->append(thirdParameter);

    IPXactSystemVerilogParser parser(QSharedPointer<ParameterFinder>(new ComponentParameterFinder(testComponent)));

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
    QTest::newRow("Expressions in parameter values are evaluated before further evaluation") << "third" << "4";
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
    testComponent->getParameters()->append(firstParameter);

    QSharedPointer<Parameter> secondParameter(new Parameter());
    secondParameter->setValueId("second");
    secondParameter->setValue("first");
    testComponent->getParameters()->append(secondParameter);

    IPXactSystemVerilogParser parser(QSharedPointer<ParameterFinder>(new ComponentParameterFinder(testComponent)));

    QTRY_COMPARE_WITH_TIMEOUT(parser.parseExpression("second"), QString("x"), 3000);
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
    testComponent->getParameters()->append(firstParameter);

    QSharedPointer<Parameter> secondParameter(new Parameter());
    secondParameter->setValueId("second");
    secondParameter->setValue("first + 2");
    testComponent->getParameters()->append(secondParameter);

    IPXactSystemVerilogParser parser(QSharedPointer<ParameterFinder>(new ComponentParameterFinder(testComponent)));

    QCOMPARE(parser.parseExpression("first"), QString("\"text\""));
    QCOMPARE(parser.parseExpression("second"), QString("x"));
}

//-----------------------------------------------------------------------------
// Function: tst_IPXactSystemVerilogParser::testGetBaseForExpression()
//-----------------------------------------------------------------------------
void tst_IPXactSystemVerilogParser::testGetBaseForExpression()
{
    QSharedPointer<Component> testComponent(new Component());
    QSharedPointer<Parameter> firstParameter(new Parameter());
    firstParameter->setValueId("first");
    firstParameter->setValue("'h1");
    testComponent->getParameters()->append(firstParameter);

    IPXactSystemVerilogParser parser(QSharedPointer<ParameterFinder>(new ComponentParameterFinder(testComponent)));

    QCOMPARE(parser.baseForExpression("2*first"), 16);
}

//-----------------------------------------------------------------------------
// Function: tst_IPXactSystemVerilogParser::testExpressionWithRealParameterReferences()
//-----------------------------------------------------------------------------
void tst_IPXactSystemVerilogParser::testExpressionWithRealValueParameterReferences()
{
    QFETCH(QString, expression);
    QFETCH(QString, expectedResult);

    QSharedPointer<Component> testComponent(new Component());
    QSharedPointer<Parameter> firstParameter(new Parameter());
    firstParameter->setValueId("firstValue");
    firstParameter->setValue("30");
    testComponent->getParameters()->append(firstParameter);

    QSharedPointer<Parameter> secondParameter(new Parameter());
    secondParameter->setValueId("secondValue");
    secondParameter->setValue("0.751");
    testComponent->getParameters()->append(secondParameter);

    IPXactSystemVerilogParser parser(QSharedPointer<ParameterFinder>(new ComponentParameterFinder(testComponent)));

    QCOMPARE(parser.parseExpression(expression), expectedResult);   
}

//-----------------------------------------------------------------------------
// Function: tst_IPXactSystemVerilogParser::testExpressionWithRealParameterReferences_data()
//-----------------------------------------------------------------------------
void tst_IPXactSystemVerilogParser::testExpressionWithRealValueParameterReferences_data()
{
    QTest::addColumn<QString>("expression");
    QTest::addColumn<QString>("expectedResult");

    QTest::newRow("Two parameters multiplication, integer and floating point") <<
        "firstValue * secondValue" << "22.53";
    QTest::newRow("Parameters division, integer and floating point") << "firstValue / secondValue" << "39";
    QTest::newRow("Parameters addition, integer and floating point") << "firstValue + secondValue" << "30.751";
    QTest::newRow("Parameters subtraction, integer and floating point") << "firstValue - secondValue" << "29.249";
}

//-----------------------------------------------------------------------------
// Function: tst_IPXactSystemVerilogParser::testExpressionComparison()
//-----------------------------------------------------------------------------
void tst_IPXactSystemVerilogParser::testExpressionComparison()
{
    QFETCH(QString, valueOne);
    QFETCH(QString, valueTwo);
    QFETCH(QString, expression);
    QFETCH(int, expectedResult);

    QSharedPointer<Component> testComponent(new Component());
    QSharedPointer<Parameter> firstParameter(new Parameter());
    firstParameter->setValueId("firstValue");
    firstParameter->setValue(valueOne);
    testComponent->getParameters()->append(firstParameter);

    QSharedPointer<Parameter> secondParameter(new Parameter());
    secondParameter->setValueId("secondValue");
    secondParameter->setValue(valueTwo);
    testComponent->getParameters()->append(secondParameter);

    IPXactSystemVerilogParser parser(QSharedPointer<ParameterFinder>(new ComponentParameterFinder(testComponent)));

    QCOMPARE(parser.parseExpression(expression).toInt(), expectedResult);
}

//-----------------------------------------------------------------------------
// Function: tst_IPXactSystemVerilogParser::testExpressionComparison_data()
//-----------------------------------------------------------------------------
void tst_IPXactSystemVerilogParser::testExpressionComparison_data()
{
    QTest::addColumn<QString>("valueOne");
    QTest::addColumn<QString>("valueTwo");
    QTest::addColumn<QString>("expression");
    QTest::addColumn<int>("expectedResult");

    QTest::newRow("Value 1 is greater than value 2 returns 1") << "10" << "4*2" << "firstValue > secondValue" << 1;
    QTest::newRow("Value 2 is greater than value 1 returns 0") << "10" << "4*2" << "secondValue > firstValue" << 0;

    QTest::newRow("Value 1 is lesser than value 2 returns 0") << "10" << "4*2" << "firstValue < secondValue" << 0;
    QTest::newRow("Value 2 is lesser than value 1 returns 1") << "10" << "4*2" << "secondValue < firstValue" << 1;

    QTest::newRow("Value 1 + value 2 is greater than value 1 returns 1") << "10" << "4*2" <<
        "firstValue + secondValue > firstValue" << 1;
    QTest::newRow("Value 1 + value 2 is lesser than value 1 returns 0") << "10" << "4*2" <<
        "firstValue + secondValue < firstValue" << 0;

    QTest::newRow("Single value: 10 is equal to 10 returns 1") << "10" << "10" << "firstValue==secondValue" << 1;
    QTest::newRow("Single value: 10 is equal to 2 returns 0") << "10" << "2" << "firstValue==secondValue" << 0;

    QTest::newRow("Expression: 10+2 is equal to 2*6 returns 1") <<  "10" << "2" <<
        "firstValue+secondValue==secondValue*6" << 1;
    QTest::newRow("Expression: 10+2+1 is equal to 2*6 returns 0") << "10" << "2" <<
        "firstValue+secondValue+1==secondValue*2" << 0;
}

//-----------------------------------------------------------------------------
// Function: tst_IPXactSystemVerilogParser::testLongReferenceChainPerformance()
//-----------------------------------------------------------------------------
void tst_IPXactSystemVerilogParser::testLongReferenceChainPerformance()
{
    QFETCH(int, chainLength);
    QFETCH(bool, useCache);

    QSharedPointer<Component> testComponent(new Component());

    QSharedPointer<Parameter> firstParameter(new Parameter());
    firstParameter->setValueId("id_0");
    firstParameter->setValue("0");
    testComponent->getParameters()->append(firstParameter);

    for (int i = 1; i <= chainLength; i++)
    {
        QSharedPointer<Parameter> nextParamter(new Parameter());
        nextParamter->setValueId("id_" + QString::number(i));
        nextParamter->setValue(QString("id_%1 + 1").arg(i-1));
        testComponent->getParameters()->append(nextParamter);
    }

    QSharedPointer<ParameterFinder> finder;
    if (useCache)
    {
        finder = QSharedPointer<ParameterFinder>(new ParameterCache(testComponent));
    }
    else
    {
        finder = QSharedPointer<ParameterFinder>(new ComponentParameterFinder(testComponent));
    }

    IPXactSystemVerilogParser parser(QSharedPointer<ParameterFinder>(new ComponentParameterFinder(testComponent)));

    QBENCHMARK
    {
        QTRY_COMPARE(parser.parseExpression(QString("id_%1").arg(chainLength)).toInt(), chainLength);
    }
}

//-----------------------------------------------------------------------------
// Function: tst_IPXactSystemVerilogParser::testLongReferenceChainPerformance_data()
//-----------------------------------------------------------------------------
void tst_IPXactSystemVerilogParser::testLongReferenceChainPerformance_data()
{
    QTest::addColumn<int>("chainLength");
    QTest::addColumn<bool>("useCache");

    QTest::newRow("Chain of 1 parameter, no cache") << 1 << false;
    QTest::newRow("Chain of 1 parameter, cached") << 1 << true;

    QTest::newRow("Chain of 5 parameters, no cache") << 5 << false;
    QTest::newRow("Chain of 5 parameters, cached") << 5 << true;

    QTest::newRow("Chain of 20 parameters, no cache") << 20 << false;
    QTest::newRow("Chain of 20 parameters, cached") << 20 << true;
}

QTEST_MAIN(tst_IPXactSystemVerilogParser)

#include "tst_IPXactSystemVerilogParser.moc"
