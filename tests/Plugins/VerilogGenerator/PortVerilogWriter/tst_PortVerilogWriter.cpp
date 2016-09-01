//-----------------------------------------------------------------------------
// File: tst_verilogport.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 14.7.2014
//
// Description:
// Unit test for class PortVerilogWriter.
//-----------------------------------------------------------------------------

#include <QString>
#include <QtTest>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Model.h>
//#include <IPXACTmodels/Component/modelparameter.h>
#include <IPXACTmodels/Component/Port.h>

#include <Plugins/VerilogGenerator/PortVerilogWriter/PortVerilogWriter.h>

#include <editors/ComponentEditor/common/ExpressionFormatter.h>
#include <editors/ComponentEditor/common/ComponentParameterFinder.h>

Q_DECLARE_METATYPE(DirectionTypes::Direction)

const QString INDENT = "    ";

class tst_PortVerilogWriter : public QObject
{
    Q_OBJECT

public:
    tst_PortVerilogWriter();

private slots:
    void initTestCase();
    void cleanupTestCase();

    void init();
    void cleanup();

    void testNullPointerAsConstructorParameter();

    void testWriteEmptyPort();

    void testWriteNormalPort();
    void testWriteNormalPort_data();

    void testWriteNonTypedPort();

    void testWriteVectorPort();
    void testWriteVectorPort_data();

    void testWriteVectorArrayPort();
    void testWriteVectorArrayPort_data();

    //void testWriteParametrizedPort();
    //void testWriteParametrizedPort_data();

private:

    void compareLineByLine(QString const& expectedOutput);

    PortVerilogWriter* makeWriter(QSharedPointer<GenerationPort> port);
   
    QSharedPointer<Component> enclosingComponent_;

    QSharedPointer<GenerationPort> port_;

    QSharedPointer<ExpressionFormatter> formatter_;

    QString outputString_;

    QTextStream outputStream_;

    PortVerilogWriter* portWriter_;
   
};

//-----------------------------------------------------------------------------
// Function: tst_PortVerilogWriter::tst_PortVerilogWriter()
//-----------------------------------------------------------------------------
tst_PortVerilogWriter::tst_PortVerilogWriter() : enclosingComponent_(new Component()), port_(), 
    formatter_(), outputString_(), outputStream_(&outputString_), portWriter_(0)
{
}

//-----------------------------------------------------------------------------
// Function: tst_PortVerilogWriter::initTestCase()
//-----------------------------------------------------------------------------
void tst_PortVerilogWriter::initTestCase()
{
}

//-----------------------------------------------------------------------------
// Function: tst_PortVerilogWriter::cleanupTestCase()
//-----------------------------------------------------------------------------
void tst_PortVerilogWriter::cleanupTestCase()
{
}

//-----------------------------------------------------------------------------
// Function: tst_PortVerilogWriter::init()
//-----------------------------------------------------------------------------
void tst_PortVerilogWriter::init()
{
    port_ = QSharedPointer<GenerationPort>(new GenerationPort());
 
    outputStream_.setString(&outputString_);

    QSharedPointer<ComponentParameterFinder> finder(new ComponentParameterFinder(enclosingComponent_));
    formatter_ = QSharedPointer<ExpressionFormatter>(new ExpressionFormatter(finder));
}

//-----------------------------------------------------------------------------
// Function: tst_PortVerilogWriter::cleanup()
//-----------------------------------------------------------------------------
void tst_PortVerilogWriter::cleanup()
{
    delete portWriter_;
    port_.clear();

    outputString_.clear();
}

//-----------------------------------------------------------------------------
// Function: tst_PortVerilogWriter::testWriteEmptyPort()
//-----------------------------------------------------------------------------
void tst_PortVerilogWriter::testNullPointerAsConstructorParameter()
{
    makeWriter(QSharedPointer<GenerationPort>(0));

    portWriter_->write(outputStream_);

    QCOMPARE(outputString_, QString());
}

//-----------------------------------------------------------------------------
// Function: tst_PortVerilogWriter::testWriteEmptyPort()
//-----------------------------------------------------------------------------
void tst_PortVerilogWriter::testWriteEmptyPort()
{
    port_->direction = (DirectionTypes::DIRECTION_INVALID);
    makeWriter(port_);

    portWriter_->write(outputStream_);

    QCOMPARE(outputString_, QString());
}

//-----------------------------------------------------------------------------
// Function: tst_PortVerilogWriter::testWriteNormalPort()
//-----------------------------------------------------------------------------
void tst_PortVerilogWriter::testWriteNormalPort()
{
    QFETCH(DirectionTypes::Direction, direction);
    QFETCH(QString, type);
    QFETCH(QString, expectedOutput);

    port_->name = ("Data");
    port_->direction = (direction);
    port_->typeName = (type);

    makeWriter(port_);

    portWriter_->write(outputStream_);

    QCOMPARE(outputString_, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_PortVerilogWriter::testWriteNormalPort_data()
//-----------------------------------------------------------------------------
void tst_PortVerilogWriter::testWriteNormalPort_data()
{
    QTest::addColumn<DirectionTypes::Direction>("direction");
    QTest::addColumn<QString>("type");
    QTest::addColumn<QString>("expectedOutput");

    QTest::newRow("input port")     << DirectionTypes::IN  << "integer" << "input  integer                      Data";
    QTest::newRow("output port")    << DirectionTypes::OUT << "reg"     << "output reg                          Data";
    QTest::newRow("inout port")     << DirectionTypes::INOUT << "tri"   << "inout  tri                          Data";
    QTest::newRow("phantom port")   << DirectionTypes::DIRECTION_PHANTOM << "phantom" << "";
    QTest::newRow("invalid direction port") << DirectionTypes::DIRECTION_INVALID << "invalid" << "";
}

//-----------------------------------------------------------------------------
// Function: tst_PortVerilogWriter::testWriteNonTypedPort()
//-----------------------------------------------------------------------------
void tst_PortVerilogWriter::testWriteNonTypedPort()
{
    port_->name = "Data";
    port_->direction = DirectionTypes::IN;

    makeWriter(port_);

    portWriter_->write(outputStream_);

    QCOMPARE(outputString_, QString("input                               Data"));
}

//-----------------------------------------------------------------------------
// Function: tst_PortVerilogWriter::testWriteVectorPort()
//-----------------------------------------------------------------------------
void tst_PortVerilogWriter::testWriteVectorPort()
{
    QFETCH(QString, portName);
    QFETCH(QString, type);

    QFETCH(QString, leftBound);
    QFETCH(QString, rightBound);
    QFETCH(QString, expectedOutput);

    port_->name = (portName);
    port_->direction = (DirectionTypes::OUT);
    port_->typeName = (type);
    port_->vectorBounds.first = (leftBound);
    port_->vectorBounds.second = (rightBound);

    makeWriter(port_);

    portWriter_->write(outputStream_);

    QCOMPARE(outputString_, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_PortVerilogWriter::testWriteVectorPort_data()
//-----------------------------------------------------------------------------
void tst_PortVerilogWriter::testWriteVectorPort_data()
{
    QTest::addColumn<QString>("portName");
    QTest::addColumn<QString>("type");
    QTest::addColumn<QString>("leftBound");
    QTest::addColumn<QString>("rightBound");
    QTest::addColumn<QString>("expectedOutput");

    QTest::newRow("scalar port") << "enable" << "" << "0" << "0"
        << "output                              enable";
    QTest::newRow("normal vector port") << "bus" << "reg" << "7" << "0"
        << "output reg     [7:0]                bus";
    QTest::newRow("sliced vector port") << "slicedBus" << "" << "7" << "4"
        << "output         [7:4]                slicedBus";
    QTest::newRow("big endian vector port") << "reversed" << "" << "0" << "15"
        << "output         [0:15]               reversed";
}

//-----------------------------------------------------------------------------
// Function: tst_PortVerilogWriter::testWriteVectorArrayPort()
//-----------------------------------------------------------------------------
void tst_PortVerilogWriter::testWriteVectorArrayPort()
{
    QFETCH(QString, portName);
    QFETCH(QString, type);

    QFETCH(QString, leftArrayBound);
    QFETCH(QString, rightArrayBound);
    QFETCH(QString, expectedOutput);

    port_->name = portName;
    port_->direction = DirectionTypes::OUT;
    port_->typeName = type;
    port_->vectorBounds.first = "7";
    port_->vectorBounds.second = "0";
    port_->arrayBounds.first = leftArrayBound;
    port_->arrayBounds.second = rightArrayBound;

    makeWriter(port_);

    portWriter_->write(outputStream_);

    QCOMPARE(outputString_, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_PortVerilogWriter::testWriteVectorArrayPort_data()
//-----------------------------------------------------------------------------
void tst_PortVerilogWriter::testWriteVectorArrayPort_data()
{
    QTest::addColumn<QString>("portName");
    QTest::addColumn<QString>("type");
    QTest::addColumn<QString>("leftArrayBound");
    QTest::addColumn<QString>("rightArrayBound");
    QTest::addColumn<QString>("expectedOutput");

    QTest::newRow("scalar port") << "enable" << "" << "" << "" 
        << "output         [7:0]                enable";
    QTest::newRow("array of one") << "enable" << "" << "0" << "0" 
        << "output         [0:0][7:0]           enable";
    QTest::newRow("array with type") << "bus" << "reg" << "4" << "1" 
        << "output reg     [4:1][7:0]           bus";
    QTest::newRow("big endian array port") << "reversed" << "" << "0" << "15" 
        << "output         [0:15][7:0]          reversed";
}

//-----------------------------------------------------------------------------
// Function: tst_PortVerilogWriter::testWriteParametrizedPort()
//-----------------------------------------------------------------------------
/*void tst_PortVerilogWriter::testWriteParametrizedPort()
{
    QFETCH(QString, leftArrayExpressions);
    QFETCH(QString, rightArrayExpression);
    QFETCH(QString, leftVectorExpression);
    QFETCH(QString, rightVectorExpression);
    QFETCH(QString, expectedOutput);

    QSharedPointer<ModuleParameter> parameter1(new ModuleParameter());
    parameter1->setName("name");
    parameter1->setValueId("id");

    QSharedPointer<ModuleParameter> parameter2(new ModuleParameter());
    parameter2->setName("name2");
    parameter2->setValueId("id2");

    QSharedPointer<ComponentInstantiation> testInstantiation (new ComponentInstantiation("testInstantiation"));
    testInstantiation->getModuleParameters()->append(parameter1);
    testInstantiation->getModuleParameters()->append(parameter2);
    enclosingComponent_->getComponentInstantiations()->append(testInstantiation);

    QSharedPointer<View> testView (new View("testView"));
    testView->setComponentInstantiationRef(testInstantiation->name());
    enclosingComponent_->getViews()->append(testView);

    port_->name = ("data");
    port_->direction = (DirectionTypes::IN);
    port_->typeName = "bit";
    port_->arrayBounds.first = leftArrayExpressions;
    port_->arrayBounds.second = rightArrayExpression;
    port_->vectorBounds.first = leftVectorExpression;
    port_->vectorBounds.second = rightVectorExpression;
    makeWriter(port_);

    portWriter_->write(outputStream_);

    QCOMPARE(outputString_, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_PortVerilogWriter::testWriteVectorPort_data()
//-----------------------------------------------------------------------------
void tst_PortVerilogWriter::testWriteParametrizedPort_data()
{
    QTest::addColumn<QString>("leftArrayExpressions");
    QTest::addColumn<QString>("rightArrayExpression");
    QTest::addColumn<QString>("leftVectorExpression");
    QTest::addColumn<QString>("rightVectorExpression");
    QTest::addColumn<QString>("expectedOutput");

    QTest::newRow("Left vector parameterized") << "" << "" << "id-1" << "0"
        << "input  bit     [name-1:0]           data";
    QTest::newRow("Right vector parameterized") << "" << "" << "1" << "id" 
        << "input  bit     [1:name]             data";
    QTest::newRow("Left and right vector parameterized") << "" << "" << "id" << "id" 
        << "input  bit     [name:name]          data";
    QTest::newRow("Left and right vector parameterized with different parameters") 
        << "" << "" << "id-1" << "id2" 
        << "input  bit     [name-1:name2]       data";

    QTest::newRow("Left and right vector parameterized with spacing") << "" << "" << "  id  +    1" << " id2  "
        << "input  bit     [name+1:name2]       data";

    QTest::newRow("Left and right array parameterized") << "id" << "id" << "1" << "0" 
        << "input  bit     [name:name][1:0]     data";
    QTest::newRow("Left and right array parameterized with different parameters") 
        << "id-1" << "id2" << "1" << "1" 
        << "input  bit     [name-1:name2][1:1]  data";

    QTest::newRow("All parameterized") 
        << "id-1" << "id2" << "id+1" << "id2+1" 
        << "input  bit     [name-1:name2][name+1:name2+1] data";
}*/

//-----------------------------------------------------------------------------
// Function: tst_PortVerilogWriter::makeWriter()
//-----------------------------------------------------------------------------
PortVerilogWriter* tst_PortVerilogWriter::makeWriter(QSharedPointer<GenerationPort> port)
{
    portWriter_ = new PortVerilogWriter(port);
    return portWriter_;
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::compareLineByLine()
//-----------------------------------------------------------------------------
void tst_PortVerilogWriter::compareLineByLine(QString const& expectedOutput)
{
    QStringList outputLines = outputString_.split("\n");
    QStringList expectedLines = expectedOutput.split("\n");

    int lineCount = qMin(outputLines.count(), expectedLines.count());
    for (int i = 0; i < lineCount; i++)
    {
        QCOMPARE(outputLines.at(i), expectedLines.at(i));
    }

    QCOMPARE(outputLines.count(), expectedLines.count());
}


QTEST_APPLESS_MAIN(tst_PortVerilogWriter)

#include "tst_PortVerilogWriter.moc"
