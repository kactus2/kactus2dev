//-----------------------------------------------------------------------------
// File: tst_ModeConditionParser.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 16.10.2023
//
// Description:
// Unit test for class ModeConditionParser.
//-----------------------------------------------------------------------------

#include <QtTest>
#include <QSharedPointer>

#include <KactusAPI/include/ModeConditionParser.h>

#include <KactusAPI/include/ComponentParameterFinder.h>
#include <KactusAPI/include/ParameterCache.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Mode.h>

#include <IPXACTmodels/common/Parameter.h>


class tst_ModeConditionParser : public QObject
{
    Q_OBJECT

public:
    tst_ModeConditionParser();

private slots:

    void testParsePortCondition();
    void testParsePortCondition_data();

    void testParseFieldCondition();
    void testParseFieldCondition_data();

    void testParseModeReference();
    void testParseModeReference_data();

};

//-----------------------------------------------------------------------------
// Function: tst_ModeConditionParser::tst_ModeConditionParser()
//-----------------------------------------------------------------------------
tst_ModeConditionParser::tst_ModeConditionParser()
{
}

//-----------------------------------------------------------------------------
// Function: tst_ModeConditionParser::testParsePortCondition()
//-----------------------------------------------------------------------------
void tst_ModeConditionParser::testParsePortCondition()
{
    QFETCH(QString, expression);
    QFETCH(bool, expectedValid);

    QSharedPointer<Component> testComponent(new Component(VLNV(), Document::Revision::Std22));

    QSharedPointer<Parameter> testParameter(new Parameter());
    testParameter->setName(QStringLiteral("Limit"));
    testParameter->setValueId(QStringLiteral("LIMIT"));
    testParameter->setValue(QStringLiteral("'2'b2"));
    testComponent->getParameters()->append(testParameter);

    QSharedPointer<Mode> targetMode(new Mode());
    targetMode->setName(QStringLiteral("reset"));
    targetMode->setCondition(QStringLiteral("1"));

    QSharedPointer<PortSlice> targetPort(new PortSlice());
    targetPort->setName(QStringLiteral("external_reset"));
    targetPort->setPortRef(QStringLiteral("rst_button"));
    targetMode->getPortSlices()->append(targetPort);

    testComponent->getModes()->append(targetMode);

    ModeConditionParser parser(QSharedPointer<ParameterFinder>(new ComponentParameterFinder(testComponent)),
        targetMode->getPortSlices(), targetMode->getFieldSlices(), testComponent->getModes());

    bool valid = false;
    QString result = parser.parseExpression(expression, &valid);

    QCOMPARE(valid, expectedValid);
}

//-----------------------------------------------------------------------------
// Function: tst_ModeConditionParser::testParseConstant_data()
//-----------------------------------------------------------------------------
void tst_ModeConditionParser::testParsePortCondition_data()
{
    QTest::addColumn<QString>("expression");
    QTest::addColumn<bool>("expectedValid");

    QTest::newRow("Empty reference is invalid") << "$ipxact_port_value()" << false;
    QTest::newRow("Reference to non-existent port is invalid") << "$ipxact_port_value(none)" << false;

    QTest::newRow("Reference to existing port slice is valid") << "$ipxact_port_value(external_reset)" << true;
    QTest::newRow("Reference within expression is valid") << "$ipxact_port_value(external_reset) == 1'b1" << true; 
    QTest::newRow("Reference within parametrized expression is valid") <<
        "$ipxact_port_value(external_reset) > LIMIT" << true;
}

//-----------------------------------------------------------------------------
// Function: tst_ModeConditionParser::testParseFieldCondition()
//-----------------------------------------------------------------------------
void tst_ModeConditionParser::testParseFieldCondition()
{
    QFETCH(QString, expression);
    QFETCH(bool, expectedValid);

    QSharedPointer<Component> testComponent(new Component(VLNV(), Document::Revision::Std22));

    QSharedPointer<Parameter> testParameter(new Parameter());
    testParameter->setName(QStringLiteral("Limit"));
    testParameter->setValueId(QStringLiteral("LIMIT"));
    testParameter->setValue(QStringLiteral("'2'b2"));
    testComponent->getParameters()->append(testParameter);

    QSharedPointer<Mode> targetMode(new Mode());
    targetMode->setName(QStringLiteral("reset"));
    targetMode->setCondition(QStringLiteral("1"));

    QSharedPointer<FieldSlice> targetPort(new FieldSlice());
    targetPort->setName(QStringLiteral("enable_reg"));
    targetMode->getFieldSlices()->append(targetPort);

    testComponent->getModes()->append(targetMode);

    ModeConditionParser parser(QSharedPointer<ParameterFinder>(new ComponentParameterFinder(testComponent)),
        targetMode->getPortSlices(), targetMode->getFieldSlices(), testComponent->getModes());

    bool valid = false;
    QString result = parser.parseExpression(expression, &valid);

    QCOMPARE(valid, expectedValid);
}

//-----------------------------------------------------------------------------
// Function: tst_ModeConditionParser::testParseFieldCondition_data()
//-----------------------------------------------------------------------------
void tst_ModeConditionParser::testParseFieldCondition_data()
{
    QTest::addColumn<QString>("expression");
    QTest::addColumn<bool>("expectedValid");

    QTest::newRow("Empty reference is invalid") << "$ipxact_field_value()" << false;
    QTest::newRow("Reference to non-existent field is invalid") << "$ipxact_field_value(none)" << false;

     QTest::newRow("Reference to existing port slice is valid") << "$ipxact_field_value(enable_reg)" << true;
     QTest::newRow("Reference within expression is valid") << "$ipxact_field_value(enable_reg) == 1'b1" << true;
     QTest::newRow("Reference within parametrized expression is valid") <<
         "$ipxact_field_value(enable_reg) > LIMIT" << true;
}

//-----------------------------------------------------------------------------
// Function: tst_ModeConditionParser::testParseModeReference()
//-----------------------------------------------------------------------------
void tst_ModeConditionParser::testParseModeReference()
{   
    QFETCH(QString, expression);
    QFETCH(bool, expectedValid);

    QSharedPointer<Component> testComponent(new Component(VLNV(), Document::Revision::Std22));

    QSharedPointer<Parameter> testParameter(new Parameter());
    testParameter->setName(QStringLiteral("Limit"));
    testParameter->setValueId(QStringLiteral("LIMIT"));
    testParameter->setValue(QStringLiteral("12"));
    testComponent->getParameters()->append(testParameter);

    QSharedPointer<Mode> targetMode(new Mode());
    targetMode->setName(QStringLiteral("reset"));
    targetMode->setCondition(QStringLiteral("1"));
    testComponent->getModes()->append(targetMode);

    ModeConditionParser parser(QSharedPointer<ParameterFinder>(new ComponentParameterFinder(testComponent)),
       targetMode->getPortSlices(), targetMode->getFieldSlices(), testComponent->getModes());

    bool valid = false;
    QString result = parser.parseExpression(expression, &valid);

    QCOMPARE(valid, expectedValid);
}

//-----------------------------------------------------------------------------
// Function: tst_ModeConditionParser::testParseConstant_data()
//-----------------------------------------------------------------------------
void tst_ModeConditionParser::testParseModeReference_data()
{
    QTest::addColumn<QString>("expression");
    QTest::addColumn<bool>("expectedValid");

    QTest::newRow("Empty reference is invalid") << "$ipxact_mode_condition()" << false;
    QTest::newRow("Reference to non-existent mode is invalid") << "$ipxact_mode_condition(none)" << false;

    QTest::newRow("Reference to existing mode is valid") << "$ipxact_mode_condition(reset)" << true;
    QTest::newRow("Reference within expression is valid") << "$ipxact_mode_condition(reset) == 1" << true;
    QTest::newRow("Reference within parametrized expression is valid") << 
        "$ipxact_mode_condition(reset) < LIMIT" << true;
}

QTEST_MAIN(tst_ModeConditionParser)

#include "tst_ModeConditionParser.moc"
