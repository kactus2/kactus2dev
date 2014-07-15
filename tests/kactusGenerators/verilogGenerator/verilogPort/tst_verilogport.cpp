//-----------------------------------------------------------------------------
// File: tst_verilogport.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 14.7.2014
//
// Description:
// Unit test for VerilogPort.
//-----------------------------------------------------------------------------

#include <QString>
#include <QtTest>

#include <IPXACTmodels/port.h>
#include <kactusGenerators/verilogGenerator/VerilogPort.h>

Q_DECLARE_METATYPE(General::Direction)

class Tst_verilogPort : public QObject
{
    Q_OBJECT

public:
    Tst_verilogPort();

private:

    Port* port_;

    QString* outputString_;

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    void testEmptyPort();
    void testNormalPort();
    void testWriteEmptyPort();
    void testWriteNormalPort();
    void testWriteNormalPort_data();
    void testWriteNonTypedPort();
    void testWriteVectorPort();
    void testWriteVectorPort_data();
    void testWriteDescription();
};

//-----------------------------------------------------------------------------
// Function: Tst_verilogPort::Tst_verilogPort()
//-----------------------------------------------------------------------------
Tst_verilogPort::Tst_verilogPort()
{
}

//-----------------------------------------------------------------------------
// Function: Tst_verilogPort::initTestCase()
//-----------------------------------------------------------------------------
void Tst_verilogPort::initTestCase()
{
}

//-----------------------------------------------------------------------------
// Function: Tst_verilogPort::cleanupTestCase()
//-----------------------------------------------------------------------------
void Tst_verilogPort::cleanupTestCase()
{
}

//-----------------------------------------------------------------------------
// Function: Tst_verilogPort::init()
//-----------------------------------------------------------------------------
void Tst_verilogPort::init()
{
    port_ = new Port();
    outputString_ = new QString();
}

//-----------------------------------------------------------------------------
// Function: Tst_verilogPort::cleanup()
//-----------------------------------------------------------------------------
void Tst_verilogPort::cleanup()
{
    delete port_;
    port_ = 0;

    delete outputString_;
    outputString_ = 0;
}

//-----------------------------------------------------------------------------
// Function: Tst_verilogPort::testEmptyPort()
//-----------------------------------------------------------------------------
void Tst_verilogPort::testEmptyPort()
{
    VerilogPort verilogPort(port_);

    QVERIFY2(verilogPort.name().isEmpty(), "Initial port name should be empty.");
    QVERIFY2(verilogPort.type().isEmpty(), "Initial port type should be empty.");
    QVERIFY2(verilogPort.defaultValue().isEmpty(), "Initial port default value should be empty.");
    QVERIFY2(verilogPort.description().isEmpty(), "Initial port description should be empty.");
}

//-----------------------------------------------------------------------------
// Function: Tst_verilogPort::testNormalPort()
//-----------------------------------------------------------------------------
void Tst_verilogPort::testNormalPort()
{
    port_->setName("Data");
    port_->setDirection(General::IN);
    port_->setTypeName("integer");
    port_->setDescription("Port description");

    VerilogPort verilogPort(port_);

    QCOMPARE(verilogPort.name(), QString("Data"));
    QCOMPARE(verilogPort.direction(), General::IN);
    QCOMPARE(verilogPort.type(), QString("integer"));
    QCOMPARE(verilogPort.description(), QString("Port description"));
}

//-----------------------------------------------------------------------------
// Function: Tst_verilogPort::testWriteEmptyPort()
//-----------------------------------------------------------------------------
void Tst_verilogPort::testWriteEmptyPort()
{
    VerilogPort verilogPort(port_);

    QTextStream stream(outputString_);

    verilogPort.write(stream);

    QCOMPARE(*outputString_, QString());
}

//-----------------------------------------------------------------------------
// Function: Tst_verilogPort::testWriteNormalPort()
//-----------------------------------------------------------------------------
void Tst_verilogPort::testWriteNormalPort()
{
    QFETCH(General::Direction, direction);
    QFETCH(QString, type);
    QFETCH(QString, expected);

    port_->setName("Data");
    port_->setDirection(direction);
    port_->setTypeName(type);

    VerilogPort verilogPort(port_);

    QTextStream stream(outputString_);

    verilogPort.write(stream);

    QCOMPARE(*outputString_, expected);
}

//-----------------------------------------------------------------------------
// Function: Tst_verilogPort::testWriteNormalPort_data()
//-----------------------------------------------------------------------------
void Tst_verilogPort::testWriteNormalPort_data()
{
    QTest::addColumn<General::Direction>("direction");
    QTest::addColumn<QString>("type");
    QTest::addColumn<QString>("expected");

    QTest::newRow("input port") << General::IN << "integer" << "input integer Data;";
    QTest::newRow("output port") << General::OUT << "reg" << "output reg Data;";
    QTest::newRow("inout port") << General::INOUT << "tri" << "inout tri Data;";
    QTest::newRow("phantom port") << General::DIRECTION_PHANTOM << "phantom" << "";
    QTest::newRow("phantom port") << General::DIRECTION_INVALID << "invalid" << "";
}

//-----------------------------------------------------------------------------
// Function: Tst_verilogPort::testWriteNonTypedPort()
//-----------------------------------------------------------------------------
void Tst_verilogPort::testWriteNonTypedPort()
{
    port_->setName("Data");
    port_->setDirection(General::IN);

    VerilogPort verilogPort(port_);

    QTextStream stream(outputString_);

    verilogPort.write(stream);

    QCOMPARE(*outputString_, QString("input Data;"));
}

//-----------------------------------------------------------------------------
// Function: Tst_verilogPort::testWriteVectorPort()
//-----------------------------------------------------------------------------
void Tst_verilogPort::testWriteVectorPort()
{
    QFETCH(int, leftBound);
    QFETCH(int, rightBound);
    QFETCH(QString, expected);

    port_->setName("DataBus");
    port_->setDirection(General::OUT);
    port_->setLeftBound(leftBound);
    port_->setRightBound(rightBound);

    VerilogPort verilogPort(port_);

    QTextStream stream(outputString_);

    verilogPort.write(stream);

    QCOMPARE(*outputString_, expected);
}

//-----------------------------------------------------------------------------
// Function: Tst_verilogPort::testWriteVectorPort_data()
//-----------------------------------------------------------------------------
void Tst_verilogPort::testWriteVectorPort_data()
{
    QTest::addColumn<int>("leftBound");
    QTest::addColumn<int>("rightBound");
    QTest::addColumn<QString>("expected");

    QTest::newRow("scalar port") << 0 << 0 << "output DataBus;";
    QTest::newRow("normal vector port") << 7 << 0 << "output [7:0] DataBus;";
    QTest::newRow("sliced vector port") << 7 << 4 << "output [7:4] DataBus;";
    QTest::newRow("big endian vector port") << 0 << 15 << "output [0:15] DataBus;";
}

//-----------------------------------------------------------------------------
// Function: Tst_verilogPort::testWriteDescription()
//-----------------------------------------------------------------------------
void Tst_verilogPort::testWriteDescription()
{
    port_->setName("DAT");
    port_->setDirection(General::OUT);
    port_->setDescription("Data from IP to bus.\nRemember to connect.");

    VerilogPort verilogPort(port_);

    QTextStream stream(outputString_);

    verilogPort.write(stream);

    QCOMPARE(*outputString_, QString("output DAT; // Data from IP to bus.\n// Remember to connect.\n"));
}

QTEST_APPLESS_MAIN(Tst_verilogPort)

#include "tst_verilogport.moc"
