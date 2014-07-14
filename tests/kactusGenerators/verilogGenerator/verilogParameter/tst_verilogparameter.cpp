//-----------------------------------------------------------------------------
// File: tst_verilogparameter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 14.7.2014
//
// Description:
// Unit test for VerilogParameter.
//-----------------------------------------------------------------------------

#include <QString>
#include <QtTest>

#include <IPXACTmodels/modelparameter.h>
#include <kactusGenerators/verilogGenerator/VerilogParameter.h>

//-----------------------------------------------------------------------------
//! Unit test for VerilogParameter.
//-----------------------------------------------------------------------------
class Tst_verilogParameter : public QObject
{
    Q_OBJECT

public:
    Tst_verilogParameter();

    ~Tst_verilogParameter();

private:

    ModelParameter* modelParam_;

    QString* outputString_;

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    void testEmptyModelParameter();
    void testNormalModelParameter();
    void testWriteEmptyModelParameter();
    void testWriteTypedModelParameter();
    void testWriteASCIIString();
    void testWriteASCIIStringWithoutQuotation();
    void testNonTypedModelParameter();
};

//-----------------------------------------------------------------------------
// Function: Tst_verilogParameter::Tst_verilogParameter()
//-----------------------------------------------------------------------------
Tst_verilogParameter::Tst_verilogParameter(): modelParam_(0), outputString_(0)
{
}

//-----------------------------------------------------------------------------
// Function: Tst_verilogParameter::~Tst_verilogParameter()
//-----------------------------------------------------------------------------
Tst_verilogParameter::~Tst_verilogParameter()
{
}

//-----------------------------------------------------------------------------
// Function: Tst_verilogParameter::initTestCase()
//-----------------------------------------------------------------------------
void Tst_verilogParameter::initTestCase()
{

}

//-----------------------------------------------------------------------------
// Function: Tst_verilogParameter::cleanupTestCase()
//-----------------------------------------------------------------------------
void Tst_verilogParameter::cleanupTestCase()
{

}

//-----------------------------------------------------------------------------
// Function: Tst_verilogParameter::init()
//-----------------------------------------------------------------------------
void Tst_verilogParameter::init()
{
    modelParam_ = new ModelParameter();
    outputString_ = new QString();
}

//-----------------------------------------------------------------------------
// Function: Tst_verilogParameter::cleanup()
//-----------------------------------------------------------------------------
void Tst_verilogParameter::cleanup()
{
    delete modelParam_;
    modelParam_ = 0;

    delete outputString_;
    outputString_ = 0;
}

//-----------------------------------------------------------------------------
// Function: Tst_verilogParameter::testEmptyModelParameter()
//-----------------------------------------------------------------------------
void Tst_verilogParameter::testEmptyModelParameter()
{
    VerilogParameter parameter(modelParam_);

    QVERIFY2(parameter.name().isEmpty(), "Initial parameter name should be empty.");
    QVERIFY2(parameter.type().isEmpty(), "Initial parameter type should be empty.");
    QVERIFY2(parameter.defaultValue().isEmpty(), "Initial parameter default value should be empty.");
    QVERIFY2(parameter.description().isEmpty(), "Initial parameter description should be empty.");
}

//-----------------------------------------------------------------------------
// Function: Tst_verilogParameter::testNormalModelParameter()
//-----------------------------------------------------------------------------
void Tst_verilogParameter::testNormalModelParameter()
{
    modelParam_->setName("Test");
    modelParam_->setDataType("string");
    modelParam_->setValue("Test1");
    modelParam_->setDescription("Test description.");

    VerilogParameter parameter(modelParam_);

    QCOMPARE(parameter.name(), QString("Test"));
    QCOMPARE(parameter.type(), QString("string"));
    QCOMPARE(parameter.defaultValue(), QString("Test1"));
    QCOMPARE(parameter.description(), QString("Test description."));
}

//-----------------------------------------------------------------------------
// Function: Tst_verilogParameter::testWriteEmptyModelParameter()
//-----------------------------------------------------------------------------
void Tst_verilogParameter::testWriteEmptyModelParameter()
{
    VerilogParameter parameter(modelParam_);

    QTextStream stream(outputString_);

    parameter.write(stream);

    QCOMPARE(*outputString_, QString());
}

//-----------------------------------------------------------------------------
// Function: Tst_verilogParameter::testWriteTypedModelParameter()
//-----------------------------------------------------------------------------
void Tst_verilogParameter::testWriteTypedModelParameter()
{
    modelParam_->setName("WIDTH");
    modelParam_->setDataType("integer");
    modelParam_->setValue("1");

    VerilogParameter parameter(modelParam_);

    QTextStream stream(outputString_);

    parameter.write(stream);

    QCOMPARE(*outputString_, QString("parameter integer WIDTH = 1;"));
}

//-----------------------------------------------------------------------------
// Function: Tst_verilogParameter::testWriteTypedModelParameter()
//-----------------------------------------------------------------------------
void Tst_verilogParameter::testWriteASCIIString()
{
    modelParam_->setName("ASCII");
    modelParam_->setDataType("string");
    modelParam_->setValue("\"Hello world!\"");

    VerilogParameter parameter(modelParam_);

    QTextStream stream(outputString_);

    parameter.write(stream);

    QCOMPARE(*outputString_, QString("parameter string ASCII = \"Hello world!\";"));
}

//-----------------------------------------------------------------------------
// Function: Tst_verilogParameter::testWriteASCIIStringWithoutQuotation()
//-----------------------------------------------------------------------------
void Tst_verilogParameter::testWriteASCIIStringWithoutQuotation()
{
    modelParam_->setName("ASCII");
    modelParam_->setDataType("string");
    modelParam_->setValue("Hello world!");

    VerilogParameter parameter(modelParam_);

    QTextStream stream(outputString_);

    parameter.write(stream);

    QCOMPARE(*outputString_, QString("parameter string ASCII = \"Hello world!\";"));
}

//-----------------------------------------------------------------------------
// Function: Tst_verilogParameter::testNonTypedModelParameter()
//-----------------------------------------------------------------------------
void Tst_verilogParameter::testNonTypedModelParameter()
{
    modelParam_->setName("BITS");
    modelParam_->setValue("8");

    VerilogParameter parameter(modelParam_);

    QTextStream stream(outputString_);

    parameter.write(stream);

    QCOMPARE(*outputString_, QString("parameter BITS = 8;"));
}

QTEST_APPLESS_MAIN(Tst_verilogParameter)

#include "tst_verilogparameter.moc"
