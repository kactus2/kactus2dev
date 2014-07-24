//-----------------------------------------------------------------------------
// File: tst_verilogComponentInstance.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 15.7.2014
//
// Description:
// Unit test for VerilogComponentInstance.
//-----------------------------------------------------------------------------

#include <QString>
#include <QtTest>

#include <kactusGenerators/verilogGenerator/VerilogComponentInstance.h>

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/port.h>

#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Unit test for VerilogComponentInstance.
//-----------------------------------------------------------------------------
class Tst_verilogComponentInstance : public QObject
{
    Q_OBJECT

public:
    Tst_verilogComponentInstance();

private:

    QSharedPointer<Component> component_;

    QString* outputString_;

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    void testInitialization();
    void testWriteSimpleInstance();
    void testWriteParametrizedInstance();
    void testWriteSinglePortMapInstance();
    void testWritePortMapsInstance();
    void testWriteBitMapInstance();
};

//-----------------------------------------------------------------------------
// Function: Tst_verilogComponentInstance::~Tst_verilogComponentInstance()
//-----------------------------------------------------------------------------
Tst_verilogComponentInstance::Tst_verilogComponentInstance(): component_()
{
}

//-----------------------------------------------------------------------------
// Function: Tst_verilogComponentInstance::initTestCase()
//-----------------------------------------------------------------------------
void Tst_verilogComponentInstance::initTestCase()
{

}

//-----------------------------------------------------------------------------
// Function: Tst_verilogComponentInstance::cleanupTestCase()
//-----------------------------------------------------------------------------
void Tst_verilogComponentInstance::cleanupTestCase()
{
}

//-----------------------------------------------------------------------------
// Function: Tst_verilogComponentInstance::init()
//-----------------------------------------------------------------------------
void Tst_verilogComponentInstance::init()
{
    component_ = QSharedPointer<Component>(new Component());
    outputString_ = new QString();
}

//-----------------------------------------------------------------------------
// Function: Tst_verilogComponentInstance::cleanup()
//-----------------------------------------------------------------------------
void Tst_verilogComponentInstance::cleanup()
{
    component_.clear();

    delete outputString_;
    outputString_ = 0;
}

//-----------------------------------------------------------------------------
// Function: Tst_verilogComponentInstance::testInitialization()
//-----------------------------------------------------------------------------
void Tst_verilogComponentInstance::testInitialization()
{
    VLNV vlnv(VLNV::COMPONENT, "TUT", "Testlib", "TestComponent", "1.0");
    component_->setVlnv(vlnv);

    VerilogComponentInstance instance(0, component_, "instance1", "flat", "Test instance");

    QCOMPARE(instance.name(), QString("instance1"));
    QCOMPARE(instance.type(), QString("TestComponent"));
    QCOMPARE(instance.defaultValue(), QString(""));
    QCOMPARE(instance.description(), QString("Test instance"));
    QCOMPARE(instance.componentModel(), component_);
    QCOMPARE(instance.vlnv(), vlnv);
}

//-----------------------------------------------------------------------------
// Function: Tst_verilogComponentInstance::testWriteSimpleInstance()
//-----------------------------------------------------------------------------
void Tst_verilogComponentInstance::testWriteSimpleInstance()
{
    VLNV vlnv(VLNV::COMPONENT, "TUT", "Testlib", "TestComponent", "1.0");
    component_->setVlnv(vlnv);

    VerilogComponentInstance instance(0, component_, "instance1", "flat");

    QTextStream stream(outputString_);

    instance.write(stream);

    QCOMPARE(*outputString_, QString("TestComponent instance1(\n);"));
}

//-----------------------------------------------------------------------------
// Function: Tst_verilogComponentInstance::testWriteParametrizedInstance()
//-----------------------------------------------------------------------------
void Tst_verilogComponentInstance::testWriteParametrizedInstance()
{
    VLNV vlnv(VLNV::COMPONENT, "TUT", "Testlib", "TestComponent", "1.0");
    component_->setVlnv(vlnv);

    VerilogComponentInstance instance(0, component_, "instance1", "flat");
    instance.assignModelParameter("freq", "10000");
    instance.assignModelParameter("width", "8");

    QTextStream stream(outputString_);

    instance.write(stream);

    QCOMPARE(*outputString_, QString("TestComponent #(.freq(10000), .width(8)) instance1(\n);"));
}

//-----------------------------------------------------------------------------
// Function: Tst_verilogComponentInstance::testWriteSinglePortMapInstance()
//-----------------------------------------------------------------------------
void Tst_verilogComponentInstance::testWriteSinglePortMapInstance()
{
    QSharedPointer<Port> port(new Port("ack_in", General::IN, 0, 0, "", true));
    component_->addPort(port);

    VLNV vlnv(VLNV::COMPONENT, "TUT", "Testlib", "TestComponent", "1.0");
    component_->setVlnv(vlnv);

    VerilogComponentInstance instance(0, component_, "instance1", "flat");
    instance.mapToSignal("ack_in", 0, 0, "", "ack_wire", 0, 0, "");

    QTextStream stream(outputString_);

    instance.write(stream);

    QCOMPARE(*outputString_, QString("TestComponent instance1(\n.ack_in(ack_wire)\n);"));
}

//-----------------------------------------------------------------------------
// Function: Tst_verilogComponentInstance::testWritePortMapsInstance()
//-----------------------------------------------------------------------------
void Tst_verilogComponentInstance::testWritePortMapsInstance()
{
    VLNV vlnv(VLNV::COMPONENT, "TUT", "Testlib", "TestComponent", "1.0");
    component_->setVlnv(vlnv);

    VerilogComponentInstance instance(0, component_, "instance1", "flat");
    instance.mapToSignal("ack_in", 0, 0, "", "ack_wire", 0, 0, "");
    instance.mapToSignal("data_out", 7, 0, "", "data", 7, 0, "");

    QTextStream stream(outputString_);

    instance.write(stream);

    QCOMPARE(*outputString_, QString("TestComponent instance1(\n.ack_in(ack_wire),\n.data_out(data)\n);"));
}

//-----------------------------------------------------------------------------
// Function: Tst_verilogComponentInstance::testWriteBitMapInstance()
//-----------------------------------------------------------------------------
void Tst_verilogComponentInstance::testWriteBitMapInstance()
{
    VLNV vlnv(VLNV::COMPONENT, "TUT", "Testlib", "TestComponent", "1.0");
    component_->setVlnv(vlnv);

    VerilogComponentInstance instance(0, component_, "instance1", "flat");
    instance.mapToSignal("ack_in", 0, 0, "", "ack_wire", 7, 7, "");
    instance.mapToSignal("data_out", 0, 0, "", "data", 7, 0, "");
    instance.mapToSignal("data_inv", 0, 0, "", "data_inv", 0, 7, "");

    QTextStream stream(outputString_);

    instance.write(stream);

    QCOMPARE(*outputString_, QString("TestComponent instance1(\n.ack_in(ack_wire[7]),\n.data_inv(data_inv[0:7]),\n.data_out(data[7:0])\n);"));
}

QTEST_APPLESS_MAIN(Tst_verilogComponentInstance)

#include "tst_verilogcomponentinstance.moc"
