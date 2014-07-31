//-----------------------------------------------------------------------------
// File: tst_ModelParameterVerilogWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 14.7.2014
//
// Description:
// Unit test for ModelParameterVerilogWriter.
//-----------------------------------------------------------------------------

#include <QString>
#include <QtTest>
#include <QSharedPointer>

#include <IPXACTmodels/modelparameter.h>
#include <Plugins/VerilogGenerator/ModelParameterVerilogWriter/ModelParameterVerilogWriter.h>

//-----------------------------------------------------------------------------
//! Unit test for ModelParameterVerilogWriter.
//-----------------------------------------------------------------------------
class tst_ModelParameterVerilogWriter : public QObject
{
    Q_OBJECT

public:
    tst_ModelParameterVerilogWriter();

private:

    QSharedPointer<ModelParameter> modelParam_;

    QString outputString_;
	
	QTextStream outputStream_;

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    void testNullPointerAsConstructorParameter();
    void testWriteEmptyModelParameter();
    void testNameButNoValue();
    void testWriteTypedModelParameter();
    void testWriteASCIIString();
    void testWriteASCIIStringWithoutQuotation();
    void testNonTypedModelParameter();
    void testWriteDescription();

};

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterVerilogWriter::tst_ModelParameterVerilogWriter()
//-----------------------------------------------------------------------------
tst_ModelParameterVerilogWriter::tst_ModelParameterVerilogWriter(): modelParam_(0), outputString_(), outputStream_()
{
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterVerilogWriter::initTestCase()
//-----------------------------------------------------------------------------
void tst_ModelParameterVerilogWriter::initTestCase()
{

}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterVerilogWriter::cleanupTestCase()
//-----------------------------------------------------------------------------
void tst_ModelParameterVerilogWriter::cleanupTestCase()
{

}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterVerilogWriter::init()
//-----------------------------------------------------------------------------
void tst_ModelParameterVerilogWriter::init()
{
    modelParam_.clear();
    modelParam_ = QSharedPointer<ModelParameter>(new ModelParameter());
    
	outputString_.clear();
	outputStream_.setString(&outputString_);
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterVerilogWriter::cleanup()
//-----------------------------------------------------------------------------
void tst_ModelParameterVerilogWriter::cleanup()
{
    modelParam_.clear();

    outputString_.clear();
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterVerilogWriter::testNullPointerAsConstructorParameter()
//-----------------------------------------------------------------------------
void tst_ModelParameterVerilogWriter::testNullPointerAsConstructorParameter()
{
    ModelParameterVerilogWriter parameter(QSharedPointer<ModelParameter>(0));
    parameter.write(outputStream_);

    QCOMPARE(outputString_, QString());
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterVerilogWriter::testWriteEmptyModelParameter()
//-----------------------------------------------------------------------------
void tst_ModelParameterVerilogWriter::testWriteEmptyModelParameter()
{
    ModelParameterVerilogWriter parameter(modelParam_);
    parameter.write(outputStream_);

    QCOMPARE(outputString_, QString());
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterVerilogWriter::testNameButNoValue()
//-----------------------------------------------------------------------------
void tst_ModelParameterVerilogWriter::testNameButNoValue()
{
    modelParam_->setName("testParameter");

    ModelParameterVerilogWriter parameter(modelParam_);
    parameter.write(outputStream_);

    QCOMPARE(outputString_, QString("parameter testParameter;\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterVerilogWriter::testWriteTypedModelParameter()
//-----------------------------------------------------------------------------
void tst_ModelParameterVerilogWriter::testWriteTypedModelParameter()
{
    modelParam_->setName("WIDTH");
    modelParam_->setDataType("integer");
    modelParam_->setValue("1");

    ModelParameterVerilogWriter parameter(modelParam_);
    parameter.write(outputStream_);

    QCOMPARE(outputString_, QString("parameter integer WIDTH = 1;\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterVerilogWriter::testWriteTypedModelParameter()
//-----------------------------------------------------------------------------
void tst_ModelParameterVerilogWriter::testWriteASCIIString()
{
    modelParam_->setName("ASCII");
    modelParam_->setDataType("string");
    modelParam_->setValue("\"Hello world!\"");

    ModelParameterVerilogWriter parameter(modelParam_);
    parameter.write(outputStream_);

    QCOMPARE(outputString_, QString("parameter string ASCII = \"Hello world!\";\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterVerilogWriter::testWriteASCIIStringWithoutQuotation()
//-----------------------------------------------------------------------------
void tst_ModelParameterVerilogWriter::testWriteASCIIStringWithoutQuotation()
{
    modelParam_->setName("ASCII");
    modelParam_->setDataType("string");
    modelParam_->setValue("Hello world!");

    ModelParameterVerilogWriter parameter(modelParam_);
    parameter.write(outputStream_);

    QCOMPARE(outputString_, QString("parameter string ASCII = \"Hello world!\";\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterVerilogWriter::testNonTypedModelParameter()
//-----------------------------------------------------------------------------
void tst_ModelParameterVerilogWriter::testNonTypedModelParameter()
{
    modelParam_->setName("BITS");
    modelParam_->setValue("8");

    ModelParameterVerilogWriter parameter(modelParam_);
    parameter.write(outputStream_);

    QCOMPARE(outputString_, QString("parameter BITS = 8;\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterVerilogWriter::testWriteDescription()
//-----------------------------------------------------------------------------
void tst_ModelParameterVerilogWriter::testWriteDescription()
{
    modelParam_->setName("BITS");
    modelParam_->setValue("8");
    modelParam_->setDescription("Number of register bits.\n"
        "Must be set.");

    ModelParameterVerilogWriter parameter(modelParam_);
    parameter.write(outputStream_);

    QCOMPARE(outputString_, QString("parameter BITS = 8; // Number of register bits.\n"
            "// Must be set.\n"));
}

QTEST_APPLESS_MAIN(tst_ModelParameterVerilogWriter)

#include "tst_ModelParameterVerilogWriter.moc"
