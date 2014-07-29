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

#include <IPXACTmodels/port.h>
#include <Plugins/VerilogGenerator/PortVerilogWriter/PortVerilogWriter.h>

Q_DECLARE_METATYPE(General::Direction)

const QString INDENT = "    ";

class tst_PortVerilogWriter : public QObject
{
    Q_OBJECT

public:
    tst_PortVerilogWriter();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    void testWriteEmptyPort();
    void testWriteNormalPort();
    void testWriteNormalPort_data();
    void testWriteNonTypedPort();
    void testWriteVectorPort();
    void testWriteVectorPort_data();
    void testWriteDescription();

private:

    void compareLineByLine(QString const& expectedOutput);

    QSharedPointer<Port> port_;

    QString outputString_;

    QTextStream outputStream_;
   
};

//-----------------------------------------------------------------------------
// Function: tst_PortVerilogWriter::tst_PortVerilogWriter()
//-----------------------------------------------------------------------------
tst_PortVerilogWriter::tst_PortVerilogWriter()
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
    port_ = QSharedPointer<Port>(new Port());
    
    outputStream_.setString(&outputString_);
}

//-----------------------------------------------------------------------------
// Function: tst_PortVerilogWriter::cleanup()
//-----------------------------------------------------------------------------
void tst_PortVerilogWriter::cleanup()
{
    port_.clear();

    outputString_.clear();
}

//-----------------------------------------------------------------------------
// Function: tst_PortVerilogWriter::testWriteEmptyPort()
//-----------------------------------------------------------------------------
void tst_PortVerilogWriter::testWriteEmptyPort()
{
    PortVerilogWriter verilogPort(port_);

    verilogPort.write(outputStream_);

    QCOMPARE(outputString_, QString());
}

//-----------------------------------------------------------------------------
// Function: tst_PortVerilogWriter::testWriteNormalPort()
//-----------------------------------------------------------------------------
void tst_PortVerilogWriter::testWriteNormalPort()
{
    QFETCH(General::Direction, direction);
    QFETCH(QString, type);
    QFETCH(QString, expected);

    port_->setName("Data");
    port_->setDirection(direction);
    port_->setTypeName(type);

    PortVerilogWriter verilogPort(port_);

    verilogPort.write(outputStream_);

    QCOMPARE(outputString_, expected);
}

//-----------------------------------------------------------------------------
// Function: tst_PortVerilogWriter::testWriteNormalPort_data()
//-----------------------------------------------------------------------------
void tst_PortVerilogWriter::testWriteNormalPort_data()
{
    QTest::addColumn<General::Direction>("direction");
    QTest::addColumn<QString>("type");
    QTest::addColumn<QString>("expected");

    QTest::newRow("input port") << General::IN << "integer" << INDENT + "input integer Data;\n";
    QTest::newRow("output port") << General::OUT << "reg" << INDENT + "output reg Data;\n";
    QTest::newRow("inout port") << General::INOUT << "tri" << INDENT + "inout tri Data;\n";
    QTest::newRow("phantom port") << General::DIRECTION_PHANTOM << "phantom" << "";
    QTest::newRow("invalid direction port") << General::DIRECTION_INVALID << "invalid" << "";
}

//-----------------------------------------------------------------------------
// Function: tst_PortVerilogWriter::testWriteNonTypedPort()
//-----------------------------------------------------------------------------
void tst_PortVerilogWriter::testWriteNonTypedPort()
{
    port_->setName("Data");
    port_->setDirection(General::IN);

    PortVerilogWriter verilogPort(port_);

    verilogPort.write(outputStream_);

    QCOMPARE(outputString_, QString(INDENT + "input Data;\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_PortVerilogWriter::testWriteVectorPort()
//-----------------------------------------------------------------------------
void tst_PortVerilogWriter::testWriteVectorPort()
{
    QFETCH(int, leftBound);
    QFETCH(int, rightBound);
    QFETCH(QString, expected);

    port_->setName("DataBus");
    port_->setDirection(General::OUT);
    port_->setLeftBound(leftBound);
    port_->setRightBound(rightBound);

    PortVerilogWriter verilogPort(port_);

    verilogPort.write(outputStream_);

    QCOMPARE(outputString_, expected);
}

//-----------------------------------------------------------------------------
// Function: tst_PortVerilogWriter::testWriteVectorPort_data()
//-----------------------------------------------------------------------------
void tst_PortVerilogWriter::testWriteVectorPort_data()
{
    QTest::addColumn<int>("leftBound");
    QTest::addColumn<int>("rightBound");
    QTest::addColumn<QString>("expected");

    QTest::newRow("scalar port") << 0 << 0 << INDENT + "output DataBus;\n";
    QTest::newRow("normal vector port") << 7 << 0 << INDENT + "output [7:0] DataBus;\n";
    QTest::newRow("sliced vector port") << 7 << 4 << INDENT + "output [7:4] DataBus;\n";
    QTest::newRow("big endian vector port") << 0 << 15 << INDENT + "output [0:15] DataBus;\n";
}

//-----------------------------------------------------------------------------
// Function: tst_PortVerilogWriter::testWriteDescription()
//-----------------------------------------------------------------------------
void tst_PortVerilogWriter::testWriteDescription()
{
    port_->setName("DAT");
    port_->setDirection(General::OUT);
    port_->setDescription("Data from IP to bus.\nRemember to connect.");

    PortVerilogWriter verilogPort(port_);

    verilogPort.write(outputStream_);

    compareLineByLine(QString(INDENT + "output DAT; // Data from IP to bus.\n"
        "// Remember to connect.\n"));
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
