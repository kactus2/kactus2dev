//-----------------------------------------------------------------------------
// File: tst_VerilogGenerator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 28.7.2014
//
// Description:
// Unit test for class VerilogGenerator.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/ComponentInstance.h>
#include <IPXACTmodels/design.h>
#include <IPXACTmodels/model.h>
#include <IPXACTmodels/modelparameter.h>
#include <IPXACTmodels/port.h>
#include <IPXACTmodels/vlnv.h>

#include <Plugins/VerilogGenerator/VerilogGenerator.h>

class tst_VerilogGenerator : public QObject
{
    Q_OBJECT

public:
    tst_VerilogGenerator();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    void testUnparsedComponent();
    void testNamedComponent();
    void testComponentWithOnePort();
    void testComponentWithTypedPort();
    void testComponentWithVectorPort();
    void testComponentWithModelParameters();
    void testComponentInstantiations();
    void testHierarchicalConnection();
    void testConsecutiveParseCalls();

private:

    void compareLineByLine(QString const& expectedOutput);
   
    QSharedPointer<Component> component_;
    
    QString outputString_;
    QTextStream outputStream_;
};

tst_VerilogGenerator::tst_VerilogGenerator(): component_(), outputString_(), outputStream_()
{

}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::initTestCase()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::initTestCase()
{

}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::cleanupTestCase()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::cleanupTestCase()
{

}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::init()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::init()
{
    component_ = QSharedPointer<Component>(new Component());

    outputStream_.setString(&outputString_);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::cleanup()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::cleanup()
{
    component_.clear();

    outputStream_.reset();
    outputString_.clear();
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testEmptyComponent()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testUnparsedComponent()
{
    VerilogGenerator generator;

    generator.write(outputStream_);

    QCOMPARE(outputString_, QString());
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testNamedComponent()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testNamedComponent()
{
    VLNV vlnv(VLNV::COMPONENT, "Test", "TestLibrary", "TestComponent", "1.0");
    component_->setVlnv(vlnv);

    VerilogGenerator generator;

    generator.parse(component_);

    generator.write(outputStream_);

    compareLineByLine(QString("module TestComponent();\n"
        "endmodule\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testComponentWithOnePort()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testComponentWithOnePort()
{
    VLNV vlnv(VLNV::COMPONENT, "Test", "TestLibrary", "TestComponent", "1.0");
    component_->setVlnv(vlnv);

    QSharedPointer<Port> port = QSharedPointer<Port>(new Port("clk", General::IN, 0, 0, "", true));
    component_->addPort(port);

    VerilogGenerator generator;

    generator.parse(component_);
    generator.write(outputStream_);

    compareLineByLine(QString("module TestComponent(clk);\n"
        "    input clk;\n"
        "endmodule\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testComponentWithTypedPort()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testComponentWithTypedPort()
{
    VLNV vlnv(VLNV::COMPONENT, "Test", "TestLibrary", "TestComponent", "1.0");
    component_->setVlnv(vlnv);

    QSharedPointer<Port> port = QSharedPointer<Port>(new Port("clk", General::IN, 0, 0, "", true));
    component_->addPort(port);

    QSharedPointer<Port> typedPort = QSharedPointer<Port>(new Port("data", General::OUT, 0, 0, "", true));
    typedPort->setTypeName("integer");
    component_->addPort(typedPort);

    VerilogGenerator generator;

    generator.parse(component_);
    generator.write(outputStream_);

    compareLineByLine(QString("module TestComponent(clk, data);\n"
        "    input clk;\n"
        "    output integer data;\n"
        "endmodule\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testComponentWithTypedPort()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testComponentWithVectorPort()
{
    VLNV vlnv(VLNV::COMPONENT, "Test", "TestLibrary", "TestComponent", "1.0");
    component_->setVlnv(vlnv);

    QSharedPointer<Port> vectorPort = QSharedPointer<Port>(new Port("data", General::OUT, 7, 0, "", true));
    component_->addPort(vectorPort);

    VerilogGenerator generator;

    generator.parse(component_);
    generator.write(outputStream_);

    compareLineByLine(QString("module TestComponent(data);\n"
        "    output [7:0] data;\n"
        "endmodule\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testComponentWithTypedPort()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testComponentWithModelParameters()
{
    VLNV vlnv(VLNV::COMPONENT, "Test", "TestLibrary", "TestComponent", "1.0");
    component_->setVlnv(vlnv);

    QSharedPointer<ModelParameter> parameter = QSharedPointer<ModelParameter>(new ModelParameter());
    parameter->setName("freq");
    parameter->setValue("5000");
    component_->getModel()->addModelParameter(parameter);

    QSharedPointer<ModelParameter> secondParameter = QSharedPointer<ModelParameter>(new ModelParameter());
    secondParameter->setName("dataWidth");
    secondParameter->setValue("8");
    component_->getModel()->addModelParameter(secondParameter);

    VerilogGenerator generator;

    generator.parse(component_);
    generator.write(outputStream_);

    compareLineByLine(QString("module TestComponent();\n"
        "    parameter freq = 5000;\n"
        "    parameter dataWidth = 8;\n"
        "endmodule\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testComponentIstantiation()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testComponentInstantiations()
{
    VLNV vlnv(VLNV::COMPONENT, "Test", "TestLibrary", "TopComponent", "1.0");
    component_->setVlnv(vlnv);

    QSharedPointer<Port> port = QSharedPointer<Port>(new Port("clk", General::IN, 0, 0, "", true));
    component_->addPort(port);

    VLNV subcomponentVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "SubComponent", "1.0");
    ComponentInstance subcomponentInstance("instance1", "", "", subcomponentVLNV, QPoint(), "");   
    ComponentInstance subcomponentInstance2("instance2", "", "", subcomponentVLNV, QPoint(), "");   

    VLNV designVLNV(VLNV::DESIGN, "Test", "TestLibrary", "TopComponent.des", "1.0");
    QSharedPointer<Design> topDesign = QSharedPointer<Design>(new Design(designVLNV));

    QList<ComponentInstance> instances;
    instances.append(subcomponentInstance);
    instances.append(subcomponentInstance2);

    topDesign->setComponentInstances(instances);

    VerilogGenerator generator;

    generator.parse(component_, topDesign);
    generator.write(outputStream_);

    compareLineByLine(QString("module TopComponent(clk);\n"
        "    input clk;\n"
        "\n"
        "    instance1 SubComponent();\n"
        "\n"
        "    instance2 SubComponent();\n"
        "\n"
        "endmodule\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testComponentIstantiation()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testHierarchicalConnection()
{
    VLNV vlnv(VLNV::COMPONENT, "Test", "TestLibrary", "TopComponent", "1.0");
    component_->setVlnv(vlnv);

    QSharedPointer<Port> topPort = QSharedPointer<Port>(new Port("clk", General::IN, 0, 0, "", true));
    component_->addPort(topPort);

    VLNV subcomponentVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "SubComponent", "1.0");
    QSharedPointer<Component> subComponent = QSharedPointer<Component>(new Component(subcomponentVLNV));
    

    QSharedPointer<Port> instancePort = QSharedPointer<Port>(new Port("clk_in", General::IN, 0, 0, "", true));
    subComponent->addPort(instancePort);

    ComponentInstance subcomponentInstance("instance1", "", "", subcomponentVLNV, QPoint(), "");   

    VLNV designVLNV(VLNV::DESIGN, "Test", "TestLibrary", "TopComponent.des", "1.0");
    QSharedPointer<Design> topDesign = QSharedPointer<Design>(new Design(designVLNV));

    QList<ComponentInstance> instances;
    instances.append(subcomponentInstance);

    topDesign->setComponentInstances(instances);

    HierConnection connection("topClock", Interface(subcomponentVLNV.toString(), "clk_if"));    
    
    QList<HierConnection> hierConnections;
    hierConnections.append(connection);

    topDesign->setHierarchicalConnections(hierConnections);

    VerilogGenerator generator;

    generator.parse(component_, topDesign);
    generator.write(outputStream_);

    compareLineByLine(QString("module TopComponent(clk);\n"
        "    input clk;\n"
        "\n"
        "    instance1 SubComponent();\n"
        "\n"
        "endmodule\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testConsecutiveParseCalls()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testConsecutiveParseCalls()
{
    VLNV vlnv(VLNV::COMPONENT, "Test", "TestLibrary", "TestComponent", "1.0");
    component_->setVlnv(vlnv);

    VLNV secondVlnv(VLNV::COMPONENT, "Test", "TestLibrary", "Override", "1.0");
    QSharedPointer<Component> secondComponent = QSharedPointer<Component>(new Component());
    secondComponent->setVlnv(secondVlnv);

    VerilogGenerator generator;

    generator.parse(component_);
    generator.parse(secondComponent);

    generator.write(outputStream_);

    compareLineByLine(QString("module Override();\n"
        "endmodule\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::compareLineByLine()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::compareLineByLine(QString const& expectedOutput)
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


QTEST_APPLESS_MAIN(tst_VerilogGenerator)

#include "tst_VerilogGenerator.moc"
