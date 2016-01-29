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

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Model.h>

#include <IPXACTmodels/common/ModuleParameter.h>

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
    void testModelParameterWithSpaces();
    void testModelParameterStringWithSpaces();
    void testModelParameterStringWithoutEndingQuote();

    void testModelParameterWithVectorBounds();
    void testModelParameterWithVectorAndArrayBounds();

private:

    QSharedPointer<ExpressionFormatter> createTestFormatterUsingComponent(QSharedPointer<Component> component);
   
    QSharedPointer<ModuleParameter> moduleParam_;

    QString outputString_;

    QTextStream outputStream_;
};

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterVerilogWriter::tst_ModelParameterVerilogWriter()
//-----------------------------------------------------------------------------
tst_ModelParameterVerilogWriter::tst_ModelParameterVerilogWriter(): moduleParam_(0), outputString_(), outputStream_()
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
    moduleParam_.clear();
    moduleParam_ = QSharedPointer<ModuleParameter>(new ModuleParameter());

	outputString_.clear();
	outputStream_.setString(&outputString_);
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterVerilogWriter::cleanup()
//-----------------------------------------------------------------------------
void tst_ModelParameterVerilogWriter::cleanup()
{
    moduleParam_.clear();

    outputString_.clear();
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterVerilogWriter::testNullPointerAsConstructorParameter()
//-----------------------------------------------------------------------------
void tst_ModelParameterVerilogWriter::testNullPointerAsConstructorParameter()
{
    QSharedPointer<ExpressionFormatter> formatter = createTestFormatterUsingComponent(QSharedPointer<Component>(0));

    ModuleParameterVerilogWriter parameter(QSharedPointer<ModuleParameter>(0), formatter);
    parameter.write(outputStream_);

    QCOMPARE(outputString_, QString());
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterVerilogWriter::testWriteEmptyModelParameter()
//-----------------------------------------------------------------------------
void tst_ModelParameterVerilogWriter::testWriteEmptyModelParameter()
{
    QSharedPointer<ExpressionFormatter> formatter = createTestFormatterUsingComponent(QSharedPointer<Component>(0));

    ModuleParameterVerilogWriter parameter(moduleParam_, formatter);
    parameter.write(outputStream_);

    QCOMPARE(outputString_, QString());
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterVerilogWriter::testNameButNoValue()
//-----------------------------------------------------------------------------
void tst_ModelParameterVerilogWriter::testNameButNoValue()
{
    moduleParam_->setName("testParameter");

    QSharedPointer<ExpressionFormatter> formatter = createTestFormatterUsingComponent(QSharedPointer<Component>(0));

    ModuleParameterVerilogWriter parameter(moduleParam_, formatter);
    parameter.write(outputStream_);

    QCOMPARE(outputString_, QString("parameter                              testParameter   "));
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterVerilogWriter::testWriteTypedModelParameter()
//-----------------------------------------------------------------------------
void tst_ModelParameterVerilogWriter::testWriteTypedModelParameter()
{
    moduleParam_->setName("WIDTH");
    moduleParam_->setDataType("integer");
    moduleParam_->setValue("1");

    QSharedPointer<ExpressionFormatter> formatter = createTestFormatterUsingComponent(QSharedPointer<Component>(0));

    ModuleParameterVerilogWriter parameter(moduleParam_, formatter);
    parameter.write(outputStream_);

    QCOMPARE(outputString_, QString("parameter integer                      WIDTH            = 1"));
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterVerilogWriter::testWriteTypedModelParameter()
//-----------------------------------------------------------------------------
void tst_ModelParameterVerilogWriter::testWriteASCIIString()
{
    moduleParam_->setName("ASCII");
    moduleParam_->setDataType("string");
    moduleParam_->setValue("\"Hello world!\"");

    QSharedPointer<ExpressionFormatter> formatter = createTestFormatterUsingComponent(QSharedPointer<Component>(0));

    ModuleParameterVerilogWriter parameter(moduleParam_, formatter);
    parameter.write(outputStream_);

    QCOMPARE(outputString_, QString("parameter string                       ASCII            = \"Hello world!\""));
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterVerilogWriter::testWriteASCIIStringWithoutQuotation()
//-----------------------------------------------------------------------------
void tst_ModelParameterVerilogWriter::testWriteASCIIStringWithoutQuotation()
{
    moduleParam_->setName("ASCII");
    moduleParam_->setDataType("string");
    moduleParam_->setValue("Hello world!");

    QSharedPointer<ExpressionFormatter> formatter = createTestFormatterUsingComponent(QSharedPointer<Component>(0));

    ModuleParameterVerilogWriter parameter(moduleParam_, formatter);
    parameter.write(outputStream_);

    QCOMPARE(outputString_, QString("parameter string                       ASCII            = \"Hello world!\""));
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterVerilogWriter::testNonTypedModelParameter()
//-----------------------------------------------------------------------------
void tst_ModelParameterVerilogWriter::testNonTypedModelParameter()
{
    moduleParam_->setName("BITS");
    moduleParam_->setValue("8");

    QSharedPointer<ExpressionFormatter> formatter = createTestFormatterUsingComponent(QSharedPointer<Component>(0));

    ModuleParameterVerilogWriter parameter(moduleParam_, formatter);
    parameter.write(outputStream_);

    QCOMPARE(outputString_, QString("parameter                              BITS             = 8"));
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterVerilogWriter::testReferencingModelParameter()
//-----------------------------------------------------------------------------
void tst_ModelParameterVerilogWriter::testReferencingModelParameter()
{
    QSharedPointer<ModuleParameter> targetParameter(new ModuleParameter());
    targetParameter->setName("target");
    targetParameter->setValueId("TARGET-ID");
    targetParameter->setValue("4");

    QSharedPointer<Component> topComponent (new Component);

    QSharedPointer<ComponentInstantiation> testInstantiation (new ComponentInstantiation("testInstantiation"));
    testInstantiation->getModuleParameters()->append(targetParameter);
    topComponent->getComponentInstantiations()->append(testInstantiation);

    QSharedPointer<View> testView (new View("testView"));
    testView->setComponentInstantiationRef(testInstantiation->name());
    topComponent->getViews()->append(testView);

    QSharedPointer<ExpressionFormatter> formatter = createTestFormatterUsingComponent(topComponent);

    moduleParam_->setName("Referer");
    moduleParam_->setValue("TARGET-ID");

    ModuleParameterVerilogWriter parameter(moduleParam_, formatter);
    parameter.write(outputStream_);

    QCOMPARE(outputString_, QString("parameter                              Referer          = target"));
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterVerilogWriter::testModelParameterWithSpaces()
//-----------------------------------------------------------------------------
void tst_ModelParameterVerilogWriter::testModelParameterWithSpaces()
{
    moduleParam_->setName("spaces");
    moduleParam_->setValue("  4     -   12");

    QSharedPointer<ExpressionFormatter> formatter = createTestFormatterUsingComponent(QSharedPointer<Component>(0));

    ModuleParameterVerilogWriter modelParameterWriter(moduleParam_, formatter);
    modelParameterWriter.write(outputStream_);

    QCOMPARE(outputString_, QString("parameter                              spaces           = 4-12"));
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterVerilogWriter::testModelParameterStringWithSpaces()
//-----------------------------------------------------------------------------
void tst_ModelParameterVerilogWriter::testModelParameterStringWithSpaces()
{
    moduleParam_->setName("stringSpaces");
    moduleParam_->setValue("   \"No   mutants   allowed.\"    ");
    moduleParam_->setDataType("string");

    QSharedPointer<ExpressionFormatter> formatter = createTestFormatterUsingComponent(QSharedPointer<Component>(0));

    ModuleParameterVerilogWriter modelParameterWriter(moduleParam_, formatter);
    modelParameterWriter.write(outputStream_);

    QCOMPARE(outputString_, QString("parameter string                       stringSpaces     = \"No   mutants   allowed.\""));
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterVerilogWriter::testModelParameterStringWithoutEndingQuote()
//-----------------------------------------------------------------------------
void tst_ModelParameterVerilogWriter::testModelParameterStringWithoutEndingQuote()
{
    moduleParam_->setName("noEndingQuote");
    moduleParam_->setValue("\"Need quote");
    moduleParam_->setDataType("string");

    QSharedPointer<ExpressionFormatter> formatter = createTestFormatterUsingComponent(QSharedPointer<Component>(0));

    ModuleParameterVerilogWriter modelParameterWriter(moduleParam_, formatter);
    modelParameterWriter.write(outputStream_);

    QCOMPARE(outputString_, QString("parameter string                       noEndingQuote    = \"Need quote\""));
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterVerilogWriter::testModelParameterWithVectorBounds()
//-----------------------------------------------------------------------------
void tst_ModelParameterVerilogWriter::testModelParameterWithVectorBounds()
{
    moduleParam_->setName("mask");
    moduleParam_->setDataType("bit");
    moduleParam_->setValue("'hFF");
    moduleParam_->setVectorLeft("7");
    moduleParam_->setVectorRight("0");

    QSharedPointer<ExpressionFormatter> formatter = createTestFormatterUsingComponent(QSharedPointer<Component>(0));

    ModuleParameterVerilogWriter modelParameterWriter(moduleParam_, formatter);
    modelParameterWriter.write(outputStream_);

    QCOMPARE(outputString_, QString("parameter bit     [7:0]                mask             = 'hFF"));
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterVerilogWriter::testModelParameterWithVectorAndArrayBounds()
//-----------------------------------------------------------------------------
void tst_ModelParameterVerilogWriter::testModelParameterWithVectorAndArrayBounds()
{
    moduleParam_->setName("mask");
    moduleParam_->setDataType("bit");
    moduleParam_->setValue("'hFF");
    moduleParam_->setVectorLeft("7");
    moduleParam_->setVectorRight("0");
    moduleParam_->setAttribute("kactus2:arrayLeft", "2");
    moduleParam_->setAttribute("kactus2:arrayRight", "1");

    QSharedPointer<ExpressionFormatter> formatter = createTestFormatterUsingComponent(QSharedPointer<Component>(0));

    ModuleParameterVerilogWriter modelParameterWriter(moduleParam_, formatter);
    modelParameterWriter.write(outputStream_);

    QCOMPARE(outputString_, QString("parameter bit     [2:1][7:0]           mask             = 'hFF"));
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
