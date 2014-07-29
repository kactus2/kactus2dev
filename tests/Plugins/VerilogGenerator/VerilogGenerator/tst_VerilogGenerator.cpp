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
#include <IPXACTmodels/businterface.h>
#include <IPXACTmodels/design.h>
#include <IPXACTmodels/model.h>
#include <IPXACTmodels/modelparameter.h>
#include <IPXACTmodels/port.h>
#include <IPXACTmodels/vlnv.h>

#include <Plugins/VerilogGenerator/VerilogGenerator.h>

const QString INDENT = "    ";

class tst_VerilogGenerator : public QObject
{
    Q_OBJECT

public:
    tst_VerilogGenerator();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Test cases:
    void testUnparsedComponent();
    void testNamedComponent();
    void testComponentWithMultiplePorts();
    void testComponentPortsOrderedByInterface();

    void addInterfaceWithOnePort(QString const& ifName, QString const& portName);

    void testComponentWithModelParameters();
    void testComponentInstantiations();
    void testHierarchicalConnection();
    void testConsecutiveParseCalls();

private:    

    void compareLineByLine(QString const& expectedOutput);
   
    //! The top level component for which the generator is run.
    QSharedPointer<Component> component_;
    
    //! The generator output as a string.
    QString outputString_;

    //! The generator output stream.
    QTextStream outputStream_;
};

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::tst_VerilogGenerator()
//-----------------------------------------------------------------------------
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
// Function: tst_VerilogGenerator::testComponentWithMultiplePorts()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testComponentWithMultiplePorts()
{
    VLNV vlnv(VLNV::COMPONENT, "Test", "TestLibrary", "TestComponent", "1.0");
    component_->setVlnv(vlnv);

    QSharedPointer<Port> clkPort = QSharedPointer<Port>(new Port("clk", General::IN, 0, 0, "", true));
    component_->addPort(clkPort);

    QSharedPointer<Port> resetPort = QSharedPointer<Port>(new Port("reset", General::IN, 0, 0, "", true));
    component_->addPort(resetPort);

    QSharedPointer<Port> vectorPort = QSharedPointer<Port>(new Port("data", General::OUT, 7, 0, "", true));    
    component_->addPort(vectorPort);

    QSharedPointer<Port> typedPort = QSharedPointer<Port>(new Port("debug", General::OUT, 1, 0, "", true));    
    typedPort->setTypeName("integer");
    component_->addPort(typedPort);

    QSharedPointer<Port> triPort = QSharedPointer<Port>(new Port("enable", General::INOUT, 0, 0, "", true));    
    triPort->setTypeName("tri");
    component_->addPort(triPort);

    VerilogGenerator generator;

    generator.parse(component_);
    generator.write(outputStream_);

    compareLineByLine(QString("module TestComponent(clk, reset, data, debug, enable);\n" +
        INDENT + "input clk;\n" +
        INDENT + "input reset;\n" +
        INDENT + "output [7:0] data;\n" +
        INDENT + "output integer [1:0] debug;\n" +
        INDENT + "inout tri enable;\n"
        "endmodule\n"));
}


//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testComponentPortsOrderedByInterface()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testComponentPortsOrderedByInterface()
{
    VLNV vlnv(VLNV::COMPONENT, "Test", "TestLibrary", "TestComponent", "1.0");
    component_->setVlnv(vlnv);

    QSharedPointer<Port> portInNoInterface = QSharedPointer<Port>(new Port("noIf", General::IN, 0, 0, "", true));
    component_->addPort(portInNoInterface);

    addInterfaceWithOnePort("X", "xData");
    addInterfaceWithOnePort("S", "sData");
    addInterfaceWithOnePort("B", "bData");
    addInterfaceWithOnePort("A", "aData");

    VerilogGenerator generator;
    generator.parse(component_);
    generator.write(outputStream_);

    compareLineByLine(QString("module TestComponent(aData, bData, xData, sData, noIf);\n" +
        INDENT + "input aData;\n" +
        INDENT + "input bData;\n" +        
        INDENT + "input xData;\n" +
        INDENT + "input sData;\n" +
        INDENT + "input noIf;\n"
        "endmodule\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::MyMethod()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::addInterfaceWithOnePort(QString const& ifName, QString const& portName)
{
    QSharedPointer<BusInterface> busIf = QSharedPointer<BusInterface>(new BusInterface());
    busIf->setName(ifName);
    component_->addBusInterface(busIf);

    QSharedPointer<Port> port = QSharedPointer<Port>(new Port(portName, General::IN, 0, 0, "", true));
    component_->addPort(port);

    QSharedPointer<General::PortMap> portMap = QSharedPointer<General::PortMap>(new General::PortMap());
    portMap->logicalPort_ = ifName.toUpper();
    portMap->physicalPort_ = port->getName();

    QList<QSharedPointer<General::PortMap> > portMapList;
    portMapList.append(portMap);
    busIf->setPortMaps(portMapList);
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

    compareLineByLine(QString("module TestComponent();\n" +
        INDENT + "parameter freq = 5000;\n" +
        INDENT + "parameter dataWidth = 8;\n"
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

    compareLineByLine(QString("module TopComponent(clk);\n" +
        INDENT + "input clk;\n" +
        "\n" +
        INDENT + "instance1 SubComponent();\n" +
        "\n" +
        INDENT + "instance2 SubComponent();\n" +
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

    compareLineByLine(QString("module TopComponent(clk);\n" +
        INDENT + "input clk;\n" +
        "\n" +
        INDENT + "instance1 SubComponent();\n" +
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
