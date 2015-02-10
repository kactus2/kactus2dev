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

#include <editors/ComponentEditor/common/ComponentParameterFinder.h>
#include <editors/ComponentEditor/common/ExpressionFormatter.h>

#include <IPXACTmodels/modelparameter.h>
#include <IPXACTmodels/component.h>
#include <IPXACTmodels/model.h>

#include <Plugins/VerilogGenerator/ModelParameterVerilogWriter/ModelParameterVerilogWriter.h>

//-----------------------------------------------------------------------------
//! Unit test for ModelParameterVerilogWriter.
//-----------------------------------------------------------------------------
class tst_ModelParameterVerilogWriter : public QObject
{
    Q_OBJECT

public:
    tst_ModelParameterVerilogWriter();

private slots:
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

    void testReferencingModelParameter();

private:

    QSharedPointer<ExpressionFormatter> createTestFormatterUsingComponent(QSharedPointer<Component> component);

    QSharedPointer<ModelParameter> modelParam_;

    QString outputString_;

    QTextStream outputStream_;
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
    QSharedPointer<ExpressionFormatter> formatter = createTestFormatterUsingComponent(QSharedPointer<Component>(0));

    ModelParameterVerilogWriter parameter(QSharedPointer<ModelParameter>(0), formatter);
    parameter.write(outputStream_);

    QCOMPARE(outputString_, QString());
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterVerilogWriter::testWriteEmptyModelParameter()
//-----------------------------------------------------------------------------
void tst_ModelParameterVerilogWriter::testWriteEmptyModelParameter()
{
    QSharedPointer<ExpressionFormatter> formatter = createTestFormatterUsingComponent(QSharedPointer<Component>(0));

    ModelParameterVerilogWriter parameter(modelParam_, formatter);
    parameter.write(outputStream_);

    QCOMPARE(outputString_, QString());
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterVerilogWriter::testNameButNoValue()
//-----------------------------------------------------------------------------
void tst_ModelParameterVerilogWriter::testNameButNoValue()
{
    modelParam_->setName("testParameter");

    QSharedPointer<ExpressionFormatter> formatter = createTestFormatterUsingComponent(QSharedPointer<Component>(0));

    ModelParameterVerilogWriter parameter(modelParam_, formatter);
    parameter.write(outputStream_);

    QCOMPARE(outputString_, QString("parameter         testParameter   "));
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterVerilogWriter::testWriteTypedModelParameter()
//-----------------------------------------------------------------------------
void tst_ModelParameterVerilogWriter::testWriteTypedModelParameter()
{
    modelParam_->setName("WIDTH");
    modelParam_->setDataType("integer");
    modelParam_->setValue("1");

    QSharedPointer<ExpressionFormatter> formatter = createTestFormatterUsingComponent(QSharedPointer<Component>(0));

    ModelParameterVerilogWriter parameter(modelParam_, formatter);
    parameter.write(outputStream_);

    QCOMPARE(outputString_, QString("parameter integer WIDTH            = 1"));
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterVerilogWriter::testWriteTypedModelParameter()
//-----------------------------------------------------------------------------
void tst_ModelParameterVerilogWriter::testWriteASCIIString()
{
    modelParam_->setName("ASCII");
    modelParam_->setDataType("string");
    modelParam_->setValue("\"Hello world!\"");

    QSharedPointer<ExpressionFormatter> formatter = createTestFormatterUsingComponent(QSharedPointer<Component>(0));

    ModelParameterVerilogWriter parameter(modelParam_, formatter);
    parameter.write(outputStream_);

    QCOMPARE(outputString_, QString("parameter string  ASCII            = \"Hello world!\""));
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterVerilogWriter::testWriteASCIIStringWithoutQuotation()
//-----------------------------------------------------------------------------
void tst_ModelParameterVerilogWriter::testWriteASCIIStringWithoutQuotation()
{
    modelParam_->setName("ASCII");
    modelParam_->setDataType("string");
    modelParam_->setValue("Hello world!");

    QSharedPointer<ExpressionFormatter> formatter = createTestFormatterUsingComponent(QSharedPointer<Component>(0));

    ModelParameterVerilogWriter parameter(modelParam_, formatter);
    parameter.write(outputStream_);

    QCOMPARE(outputString_, QString("parameter string  ASCII            = \"Hello world!\""));
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterVerilogWriter::testNonTypedModelParameter()
//-----------------------------------------------------------------------------
void tst_ModelParameterVerilogWriter::testNonTypedModelParameter()
{
    modelParam_->setName("BITS");
    modelParam_->setValue("8");

    QSharedPointer<ExpressionFormatter> formatter = createTestFormatterUsingComponent(QSharedPointer<Component>(0));

    ModelParameterVerilogWriter parameter(modelParam_, formatter);
    parameter.write(outputStream_);

    QCOMPARE(outputString_, QString("parameter         BITS             = 8"));
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterVerilogWriter::testReferencingModelParameter()
//-----------------------------------------------------------------------------
void tst_ModelParameterVerilogWriter::testReferencingModelParameter()
{
    QSharedPointer<ModelParameter> targetParameter(new ModelParameter);
    targetParameter->setName("target");
    targetParameter->setValueId("TARGET-ID");
    targetParameter->setValue("4");

    QSharedPointer<Component> topComponent (new Component);
    topComponent->getModel()->addModelParameter(targetParameter);

    QSharedPointer<ExpressionFormatter> formatter = createTestFormatterUsingComponent(topComponent);

    modelParam_->setName("Referer");
    modelParam_->setValue("TARGET-ID");

    ModelParameterVerilogWriter parameter(modelParam_, formatter);
    parameter.write(outputStream_);

    QCOMPARE(outputString_, QString("parameter         Referer          = target"));
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterVerilogWriter::createTestFormatterUsingComponent()
//-----------------------------------------------------------------------------
QSharedPointer<ExpressionFormatter> tst_ModelParameterVerilogWriter::createTestFormatterUsingComponent(
    QSharedPointer<Component> component)
{
    QSharedPointer<ParameterFinder> finder (new ComponentParameterFinder(component));
    QSharedPointer<ExpressionFormatter> formatter (new ExpressionFormatter(finder));

    return formatter;
}

QTEST_APPLESS_MAIN(tst_ModelParameterVerilogWriter)

#include "tst_ModelParameterVerilogWriter.moc"
